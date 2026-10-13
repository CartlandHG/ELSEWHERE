# foreign.awk: judge one file for foreign material (STANDARDS 1 and 6).
# Run by foreign-lint, one file per run.  Each finding is printed as
# LAW<TAB>file<TAB>line<TAB>reason: text.  Exit 0 when the file is
# judged clean, 1 with findings, 4 when the file is of no kind judged here.
# It is a marker search: it helps to find foreign material and never shows
# that there is none.

function report(law, why)
{
	printf "%s\t%s\t%d\t%s: %s\n", law, FILENAME, FNR, why,
	    substr($0, 1, 160)
	found++
}

# The line with C comments removed; incomment carries a block comment
# across lines.  A comment marker inside a string literal is not seen.
function uncomment(s,    out, i, j)
{
	out = ""
	while (s != "") {
		if (incomment) {
			j = index(s, "*/")
			if (j == 0)
				return out
			s = substr(s, j + 2)
			incomment = 0
			out = out " "
			continue
		}
		i = index(s, "/*")
		j = index(s, "//")
		if (i == 0 && j == 0)
			return out s
		if (j != 0 && (i == 0 || j < i))
			return out substr(s, 1, j - 1)
		out = out substr(s, 1, i - 1)
		s = substr(s, i + 2)
		incomment = 1
	}
	return out
}

# Every -std setting on the line must name C99.
function dialects(s,    d)
{
	while (match(s, /(^|[^A-Za-z0-9_-])-std(=|[ \t]+)[A-Za-z0-9_:]+/)) {
		d = substr(s, RSTART, RLENGTH)
		sub(/.*-std(=|[ \t]+)/, "", d)
		if (d != "c99" && d != "iso9899:1999")
			report("C99-FLAGS", "dialect " d)
		s = substr(s, RSTART + RLENGTH)
	}
}

function shell_or_make(line)
{
	dialects(line)
	if (line ~ /(^|[^A-Za-z0-9_])(sha256sum|sha1sum|md5sum|tac)([^A-Za-z0-9_]|$)/)
		report("NO-GNU-TOOL", "GNU tool (OpenBSD: sha256, md5, tail -r)")
	else if (line ~ /(^|[^A-Za-z0-9_])stat[ \t]+(-[A-Za-z]*c|--format)/)
		report("NO-GNU-TOOL", "GNU stat -c (OpenBSD: stat -f)")
	else if (line ~ /(^|[^A-Za-z0-9_])find[ \t].*[ \t]-printf([^A-Za-z0-9_]|$)/)
		report("NO-GNU-TOOL", "GNU find -printf")
	else if (line ~ /(^|[^A-Za-z0-9_])grep[ \t]+(-[A-Za-z]*P|--perl)/)
		report("NO-GNU-TOOL", "GNU grep -P")
	else if (line ~ /(^|[^A-Za-z0-9_])date[ \t]+(-[A-Za-z]*d[ \t]|--date)/)
		report("NO-GNU-TOOL", "GNU date -d (OpenBSD: date -j -f)")
	if (line ~ /(^|[ \t;&|(`"'])git[ \t]+(clone|commit|add|log|status|diff|rev-parse|checkout|push|pull|fetch|init|show|ls-files|describe|apply|am|tag|branch|merge|rebase|stash|config|archive|blame)([^A-Za-z0-9_-]|$)/)
		report("NO-GIT", "runs git")
	if (line ~ /(^|[^A-Za-z0-9_])python[0-9.]*([^A-Za-z0-9_]|$)/)
		report("PYTHON", "runs Python, tier 3, which needs a written reason")
}

FNR == 1 {
	base = FILENAME
	sub(/.*\//, "", base)
	if (base ~ /\.[ch]$/)
		kind = "c"
	else if (base ~ /^([Mm]akefile|BSDmakefile|GNUmakefile)$/ || base ~ /\.mk$/)
		kind = "mk"
	else if (base ~ /\.py$/ || $0 ~ /^#!.*python/) {
		kind = "py"
		report("PYTHON", "Python is tier 3 and needs a written reason")
	} else if (base ~ /\.sh$/ || $0 ~ /^#!.*[\/ \t](sh|bash|ksh|zsh|dash)([ \t]|$)/) {
		kind = "sh"
		if ($0 ~ /^#!/ && $0 !~ /^#![ \t]*\/bin\/sh([ \t]|$)/)
			report("POSIX-SH", "interpreter is not /bin/sh")
	} else
		kind = "none"
}

kind == "c" {
	code = uncomment($0)
	if (code ~ /#[ \t]*include[ \t]*<(linux\/|asm\/|sys\/(epoll|inotify|sendfile|prctl|signalfd|timerfd|eventfd|sysmacros|vfs|personality)\.h|malloc\.h|features\.h|gnu\/|bits\/|alloca\.h|mcheck\.h|error\.h|argp\.h)/ ||
	    code ~ /(^|[^A-Za-z0-9_])(__linux__|__linux|__gnu_linux__|__GLIBC__|__GLIBC_MINOR__|_GNU_SOURCE)([^A-Za-z0-9_]|$)/)
		report("NO-LINUX", "Linux header or token")
	if (code ~ /(^|[^A-Za-z0-9_])(__attribute__|__typeof__|__typeof|__builtin_[A-Za-z0-9_]+|__extension__|__asm__)([^A-Za-z0-9_]|$)/ ||
	    code ~ /(^|[^A-Za-z0-9_])asm[ \t]*\(/)
		report("NO-GNU-C", "GNU C extension")
	next
}

kind == "mk" && $0 !~ /^[ \t]*#/ {
	if ($0 ~ /^[ \t]*(ifeq|ifneq|ifdef|ifndef|endif)([^A-Za-z0-9_]|$)/ ||
	    $0 ~ /^[ \t]*else[ \t]*$/)
		report("NO-GNU-MAKE", "GNU conditional (BSD make uses .if)")
	else if ($0 ~ /\$\((shell|wildcard|patsubst|subst|filter|filter-out|foreach|addprefix|addsuffix|notdir|dir|abspath|realpath|call|eval|info|error|warning|sort|word|words|firstword|lastword|strip|findstring|basename|suffix|join|if|or|and|value|origin|flavor)[ \t]/)
		report("NO-GNU-MAKE", "GNU make function")
	else if ($0 ~ /^[^\t#=:]*%[^=]*:([^=]|$)/)
		report("NO-GNU-MAKE", "pattern rule (BSD make uses suffix rules)")
	else if ($0 ~ /^[ \t]*(define|endef|override|export|unexport|vpath)([^A-Za-z0-9_]|$)/ ||
	    $0 ~ /^\.ONESHELL/)
		report("NO-GNU-MAKE", "GNU directive")
	shell_or_make($0)
	next
}

kind == "sh" && $0 !~ /^[ \t]*#/ {
	if ($0 ~ /(^|[ \t;&|({])\[\[[ \t]/)
		report("POSIX-SH", "[[ (use [ or test)")
	else if ($0 ~ /<<</)
		report("POSIX-SH", "here-string")
	else if ($0 ~ /^[ \t]*(declare|typeset|shopt)([^A-Za-z0-9_]|$)/)
		report("POSIX-SH", "bash builtin")
	else if ($0 ~ /(^|[ \t;&|])[A-Za-z0-9_]+=\([^),]*(\)|$)/)
		report("POSIX-SH", "array assignment")
	else if ($0 ~ /\$\{[A-Za-z0-9_]+\/\//)
		report("POSIX-SH", "${var//...} substitution")
	else if ($0 ~ /\$\{[A-Za-z0-9_]+:-?[0-9]+:/)
		report("POSIX-SH", "${var:n:m} substring")
	else if ($0 ~ /(^|[ \t=(])\$'/)
		report("POSIX-SH", "$'...' quoting")
	else if ($0 ~ /(^|[;&|{]|then|do|else)[ \t]*source[ \t]+[^ \t]/)
		report("POSIX-SH", "source (use .)")
	shell_or_make($0)
}

END {
	if (kind == "" || kind == "none")
		exit 4
	exit (found > 0)
}
