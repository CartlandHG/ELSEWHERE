#!/bin/sh
# Controls for sidecar(1) and sidecar(5): every case ADDRESSING.md requires
# that this content-only profile can reach, a known-bad control for each
# refusal rule, the standing wrong implementation (sidecar-mutant, which
# clamps ranges) and an infrastructure failure.  Each case fixes its status,
# its exact stdout and, for a refusal, the reason on stderr, so a refusal
# for some other rule does not count.
#
# usage: sh tests/run.sh sidecar sidecar-mutant
# Exit 0 when every case passes, 1 when any fails, 2 when the harness cannot
# run or a case is UNKNOWN.

set -u
umask 077
export LC_ALL=C TZ=UTC
if [ $# -ne 2 ] || [ ! -x "$1" ] || [ ! -x "$2" ]; then
	echo "UNKNOWN infrastructure: usage: sh run.sh sidecar sidecar-mutant"
	exit 2
fi
bin=$1 mut=$2
T=$(mktemp -d /tmp/sidecar-test.XXXXXXXXXX) || {
	echo "UNKNOWN infrastructure: mktemp"
	exit 2
}
trap 'chmod -R u+rwx "$T"; rm -rf "$T"' EXIT
trap 'exit 2' HUP INT TERM
pass=0 fail=0 unknown=0 detected=0 mutant=0

# t NAME STATUS EXPECTED PATTERN PROGRAM ARG...: run PROGRAM; require its
# exit STATUS, stdout equal to the file EXPECTED (or stdout closed when
# EXPECTED is "closed") and, when PATTERN is not empty, PATTERN on stderr.
# With mutant=1 a failing case is the detection of the wrong program.
t() {
	name=$1 want=$2 exp=$3 pat=$4
	shift 4
	if [ "$exp" = closed ]; then
		"$@" >&- 2>"$T/err"
	else
		"$@" >"$T/out" 2>"$T/err"
	fi
	got=$?
	why=
	if [ "$got" -ne "$want" ]; then
		why="status $got, want $want: $(sed -n 1p "$T/err")"
	elif [ "$exp" != closed ] && ! cmp -s "$T/out" "$exp"; then
		why="stdout differs from $exp"
	elif [ -n "$pat" ] && ! grep -q -- "$pat" "$T/err"; then
		why="stderr lacks '$pat': $(sed -n 1p "$T/err")"
	fi
	if [ "$mutant" -eq 1 ]; then
		if [ -n "$why" ]; then
			echo "DETECTED $name: $why"
			detected=$((detected + 1))
		else
			echo "MISSED $name"
		fi
	elif [ -n "$why" ]; then
		echo "FAIL $name: $why"
		fail=$((fail + 1))
	else
		echo "PASS $name"
		pass=$((pass + 1))
	fi
}

# sc FILE LINE...: write a sidecar whose header holds LINE... and a body.
sc() {
	f=$T/$1
	shift
	{
		printf '%s\n' ---
		for l in "$@"; do
			printf '%s\n' "$l"
		done
		printf '%s\n' --- '# A small passage' '' \
		    'The selected bytes are bc.'
	} >"$f"
}

# raw FILE FORMAT: write FILE with printf FORMAT, octal escapes and all.
raw() {
	printf "$2" >"$T/$1"
}

ABC=ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
EMPTY=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
ZERO=0000000000000000000000000000000000000000000000000000000000000000
REV=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
MAX=18446744073709551615
S='schema: "weft-byte-range/1"'
H="content_sha256: \"$ABC\""
Z='content_size: "3"'
O='offset: "1"'
L='length: "2"'
printf abc >"$T/abc"
: >"$T/empty"
: >"$T/none"
printf bc >"$T/bc"
mkdir "$T/dir"

# Valid positives.
sc good.md "$S" "$H" "$Z" "$O" "$L"
t check-good 0 "$T/none" '' "$bin" check "$T/good.md"
t select-bc 0 "$T/bc" '' "$bin" select "$T/good.md" "$T/abc"
sc order.md "$L" "$O" "$Z" "$H" "$S"
t any-key-order 0 "$T/bc" '' "$bin" select "$T/order.md" "$T/abc"
sc eof.md "$S" "$H" "$Z" 'offset: "3"' 'length: "0"'
t empty-at-eof 0 "$T/none" '' "$bin" select "$T/eof.md" "$T/abc"
sc empty.md "$S" "content_sha256: \"$EMPTY\"" 'content_size: "0"' \
    'offset: "0"' 'length: "0"'
t empty-content 0 "$T/none" '' "$bin" select "$T/empty.md" "$T/empty"
raw nobody.md "---\n$S\n$H\n$Z\n$O\n$L\n---\n"
t empty-body 0 "$T/bc" '' "$bin" select "$T/nobody.md" "$T/abc"
sc maxint.md "$S" "$H" "content_size: \"$MAX\"" "offset: \"$MAX\"" \
    'length: "0"'
t uint64-max-parses 0 "$T/none" '' "$bin" check "$T/maxint.md"

# Native provenance is grammar, but this profile cannot verify it, so it
# never resolves as content-only.
sc prov.md "$S" "$H" "$Z" "$O" "$L" 'docid: "doc-1"' \
    "revision_sha256: \"$REV\""
t provenance-checks 0 "$T/none" '' "$bin" check "$T/prov.md"
t provenance-no-downgrade 1 "$T/none" 'provenance' \
    "$bin" select "$T/prov.md" "$T/abc"

# Bounds and capacity, refused without clamping.  The same cases run again
# below against the mutant, which must be caught.
bounds() {
	p=$1
	sc off4.md "$S" "$H" "$Z" 'offset: "4"' 'length: "0"'
	t offset-past-end 4 "$T/none" 'range outside' \
	    "$p" select "$T/off4.md" "$T/abc"
	sc len2.md "$S" "$H" "$Z" 'offset: "2"' 'length: "2"'
	t length-past-end 4 "$T/none" 'range outside' \
	    "$p" select "$T/len2.md" "$T/abc"
	sc wrap.md "$S" "$H" "$Z" 'offset: "1"' "length: \"$MAX\""
	t sum-would-wrap 4 "$T/none" 'range outside' \
	    "$p" select "$T/wrap.md" "$T/abc"
}
bounds "$bin"
sc cap.md "$S" "$H" 'content_size: "67108865"' 'offset: "0"' 'length: "0"'
t over-capacity 4 "$T/none" 'capacity' "$bin" select "$T/cap.md" "$T/abc"

# Parse refusals before any range arithmetic.
num() {
	sc "num-$1.md" "$S" "$H" "$Z" "$2" "$L"
	t "number-$1" 1 "$T/none" 'offset is not' \
	    "$bin" select "$T/num-$1.md" "$T/abc"
}
num overflow 'offset: "18446744073709551616"'
num negative 'offset: "-1"'
num leading-zero 'offset: "01"'
num partial 'offset: "1x"'
num empty 'offset: ""'
num sign 'offset: "+1"'
num point 'offset: "1.0"'
num exponent 'offset: "1e0"'
num underscore 'offset: "1_0"'

# Identity and size.
sc size4.md "$S" "$H" 'content_size: "4"' "$O" "$L"
t wrong-size 3 "$T/none" 'size differs' "$bin" select "$T/size4.md" "$T/abc"
printf abd >"$T/abd"
t tampered-content 3 "$T/none" 'digest differs' \
    "$bin" select "$T/good.md" "$T/abd"
printf abcd >"$T/abcd"
t longer-content 3 "$T/none" 'size differs' \
    "$bin" select "$T/good.md" "$T/abcd"

# A pinned address stays pinned: an edit to the content is refused, and the
# new bytes are never served in place of the old.
printf abc >"$T/edited"
t pinned-before-edit 0 "$T/bc" '' "$bin" select "$T/good.md" "$T/edited"
printf abX >"$T/edited"
t pinned-after-edit 3 "$T/none" 'digest differs' \
    "$bin" select "$T/good.md" "$T/edited"

# A changed description leaves the target address alone and changes the
# sidecar's own identity.
cp "$T/good.md" "$T/redescribed.md"
printf '%s\n' 'Now with more duck.' >>"$T/redescribed.md"
t redescribed-same-bytes 0 "$T/bc" '' \
    "$bin" select "$T/redescribed.md" "$T/abc"
if [ "$(sha256 -q "$T/good.md")" != "$(sha256 -q "$T/redescribed.md")" ]
then
	echo "PASS redescribed-new-identity"
	pass=$((pass + 1))
else
	echo "FAIL redescribed-new-identity: sidecar digests equal"
	fail=$((fail + 1))
fi

# Unavailable sources.
t missing-content 2 "$T/none" 'gone' "$bin" select "$T/good.md" "$T/gone"
t missing-sidecar 2 "$T/none" 'gone' "$bin" check "$T/gone"
t content-directory 2 "$T/none" 'not a regular' \
    "$bin" select "$T/good.md" "$T/dir"
cp "$T/abc" "$T/withheld"
chmod 000 "$T/withheld"
if [ "$(id -u)" -eq 0 ]; then
	echo "UNKNOWN withheld-content: root reads mode 000 files"
	unknown=$((unknown + 1))
else
	t withheld-content 2 "$T/none" 'withheld' \
	    "$bin" select "$T/good.md" "$T/withheld"
fi

# Schema, domain and the key table.
bad() {
	name=$1 pat=$2
	shift 2
	sc "$name.md" "$@"
	t "$name" 1 "$T/none" "$pat" "$bin" check "$T/$name.md"
}
bad wrong-schema 'unsupported schema' 'schema: "weft-byte-range/2"' \
    "$H" "$Z" "$O" "$L"
bad wrong-domain 'unsupported schema' 'schema: "weft-text-range/1"' \
    "$H" "$Z" "$O" "$L"
bad duplicate-key 'duplicate key' "$S" "$H" "$Z" "$O" "$L" "$O"
bad unknown-key 'unknown key' "$S" "$H" "$Z" "$O" "$L" 'title: "x"'
bad missing-key 'missing key length' "$S" "$H" "$Z" "$O"
bad docid-alone 'come together' "$S" "$H" "$Z" "$O" "$L" 'docid: "d"'
bad revision-zero 'revision_sha256' "$S" "$H" "$Z" "$O" "$L" 'docid: "d"' \
    "revision_sha256: \"$ZERO\""
bad docid-dot 'bad docid' "$S" "$H" "$Z" "$O" "$L" 'docid: ".d"' \
    "revision_sha256: \"$REV\""
bad docid-long 'bad docid' "$S" "$H" "$Z" "$O" "$L" \
    'docid: "abcdefghijklmnopqrstuvwxyz0123456"' "revision_sha256: \"$REV\""
bad docid-slash 'bad docid' "$S" "$H" "$Z" "$O" "$L" 'docid: "a/b"' \
    "revision_sha256: \"$REV\""
bad upper-hex 'content_sha256' "$S" \
    "content_sha256: \"$(printf %s "$ABC" | tr a-f A-F)\"" "$Z" "$O" "$L"
bad short-hex 'content_sha256' "$S" 'content_sha256: "ba78"' "$Z" "$O" "$L"
bad value-too-long 'too long' "$S" "$H" "$Z" "$O" "$L" \
    "docid: \"${ABC}x\"" "revision_sha256: \"$REV\""

# Restricted YAML: no unquoted value, alias, anchor, tag, merge,
# collection, null or multiline scalar.
bad unquoted 'not exactly' "$S" "$H" "$Z" 'offset: 1' "$L"
bad single-quoted 'not exactly' "$S" "$H" "$Z" "offset: '1'" "$L"
bad alias 'not exactly' "$S" "$H" "$Z" 'offset: *a' "$L"
bad anchor 'not exactly' "$S" "$H" "$Z" 'offset: &a "1"' "$L"
bad tag 'not exactly' "$S" "$H" "$Z" 'offset: !!str "1"' "$L"
bad merge 'unknown key' "$S" "$H" "$Z" '<<: "x"' "$O" "$L"
bad collection 'not exactly' "$S" "$H" "$Z" 'offset: ["1"]' "$L"
bad null 'not exactly' "$S" "$H" "$Z" 'offset: null' "$L"
bad multiline 'not exactly' "$S" "$H" "$Z" 'offset: "1' '"' "$L"
bad extra-token 'not exactly' "$S" "$H" "$Z" 'offset: "1" # c' "$L"
bad two-values 'offset is not' "$S" "$H" "$Z" 'offset: "1" "2"' "$L"

# Delimiters and white space.
bad indented 'unknown key' "$S" "$H" "$Z" ' offset: "1"' "$L"
bad comment 'not exactly' "$S" "# a comment" "$H" "$Z" "$O" "$L"
bad blank-line 'not exactly' "$S" '' "$H" "$Z" "$O" "$L"
bad trailing-space 'not exactly' "$S" "$H" "$Z" 'offset: "1" ' "$L"
bad tab 'not exactly' "$S" "$H" "$Z" 'offset:	"1"' "$L"
raw bom.md "\357\273\277---\n$S\n$H\n$Z\n$O\n$L\n---\n"
t bom 1 "$T/none" 'first line' "$bin" check "$T/bom.md"
raw open-space.md "--- \n$S\n$H\n$Z\n$O\n$L\n---\n"
t opening-delimiter 1 "$T/none" 'first line' "$bin" check "$T/open-space.md"
raw unclosed.md "---\n$S\n$H\n$Z\n$O\n$L\n"
t unclosed 1 "$T/none" 'no closing' "$bin" check "$T/unclosed.md"
raw close4.md "---\n$S\n$H\n$Z\n$O\n$L\n----\n"
t closing-delimiter 1 "$T/none" 'not exactly' "$bin" check "$T/close4.md"
raw cr.md "---\n$S\r\n$H\n$Z\n$O\n$L\n---\n"
t carriage-return 1 "$T/none" 'not exactly' "$bin" check "$T/cr.md"
long=$(printf '%01100d' 0)
raw longhdr.md "---\n$S\ncontent_sha256: \"$long\"\n$Z\n$O\n$L\n---\n"
t header-over-1024 1 "$T/none" 'within 1024' "$bin" check "$T/longhdr.md"

# The body: UTF-8 text without NUL or control characters, within 1 MiB.
body() {
	raw "body-$1.md" "---\n$S\n$H\n$Z\n$O\n$L\n---\n$2\n"
	t "body-$1" 1 "$T/none" "$3" "$bin" check "$T/body-$1.md"
}
body nul 'a\000b' 'NUL byte'
body invalid '\377' 'body is not'
body overlong '\300\201' 'body is not'
body surrogate '\355\240\200' 'body is not'
body above-max '\364\220\200\200' 'body is not'
body truncated '\342\202' 'body is not'
body c1-control '\302\200' 'body is not'
body escape '\033[2J' 'body is not'
body carriage-return 'a\r' 'body is not'
raw body-utf8.md "---\n$S\n$H\n$Z\n$O\n$L\n---\nduck \360\237\246\206\tok\n"
t body-utf8-tab 0 "$T/none" '' "$bin" check "$T/body-utf8.md"
cp "$T/good.md" "$T/huge.md"
dd if=/dev/zero bs=1048576 count=1 2>/dev/null | tr '\000' a >>"$T/huge.md"
t sidecar-over-1mib 1 "$T/none" 'over 1048576' "$bin" check "$T/huge.md"

# Failures of I/O, usage and setup are their own results.
t stdout-closed 5 closed 'stdout' "$bin" select "$T/good.md" "$T/abc"
t usage-none 6 "$T/none" 'usage' "$bin"
t usage-verb 6 "$T/none" 'usage' "$bin" resolve "$T/good.md"
t usage-check-two 6 "$T/none" 'usage' "$bin" check "$T/good.md" "$T/abc"
t usage-select-one 6 "$T/none" 'usage' "$bin" select "$T/good.md"

# The standing wrong implementation must be caught by every bounds case.
mutant=1
bounds "$mut"
mutant=0
if [ "$detected" -eq 3 ]; then
	echo "PASS mutant-clamp-detected (3 of 3)"
	pass=$((pass + 1))
else
	echo "FAIL mutant-clamp-detected ($detected of 3)"
	fail=$((fail + 1))
fi

# An infrastructure failure is UNKNOWN, never PASS, and its status reaches
# the caller.
sh "$0" "$T/no-such-program" "$T/no-such-program" >"$T/out" 2>&1
got=$?
if [ "$got" -eq 2 ] && grep -q '^UNKNOWN infrastructure' "$T/out"; then
	echo "PASS infrastructure-failure-is-unknown"
	pass=$((pass + 1))
else
	echo "FAIL infrastructure-failure-is-unknown: status $got"
	fail=$((fail + 1))
fi

echo "sidecar controls: $pass pass, $fail fail, $unknown unknown"
[ "$fail" -eq 0 ] || exit 1
[ "$unknown" -eq 0 ] || exit 2
exit 0
