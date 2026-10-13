# mk.awk: judge one Makefile and its include closure (STANDARDS 3, 4, 6).
# Run by mk-lint with the Makefile first and its included files after it;
# makefile names the Makefile and unresolved counts the includes that
# mk-lint could not follow.  Each finding is printed as
# LAW<TAB>file<TAB>line<TAB>reason.  Exit 0 PASS, 1 FAIL, 3 UNKNOWN.

function report(law, file, line, why)
{
	printf "%s\t%s\t%d\t%s\n", law, file, line, why
	found++
}

function unknown(file, line, why)
{
	printf "UNKNOWN\t%s\t%d\t%s\n", file, line, why
	unknowns++
}

function ischeck(name)
{
	return name == "check" || name == "test" ||
	    name ~ /^(check|test)-/ || name ~ /-(check|test)$/
}

# True when a recipe command executes nothing: empty, a comment, a bare
# continuation, ":", "true", "exit 0", or an echo or printf that pipes,
# redirects and substitutes nothing.
function noop(cmd)
{
	sub(/^[ \t@+-]+/, "", cmd)
	sub(/[ \t]+$/, "", cmd)
	if (cmd == "" || cmd ~ /^#/ || cmd == "\\" ||
	    cmd ~ /^(:|true)([ \t]|$)/ || cmd ~ /^exit([ \t]+0)?[ \t]*$/)
		return 1
	return cmd ~ /^(echo|printf)([ \t]|$)/ && cmd !~ /[|;&<>`]/ &&
	    index(cmd, "$(") == 0
}

# A check target is judged when the rule that opened it ends.
function close_target()
{
	if (cur == "")
		return
	if (vartarget)
		unknown(tfile, tline, "target " cur " is spelled through a" \
		    " variable")
	else if (!real && !delegates) {
		if (vardeps)
			unknown(tfile, tline, "target " cur " delegates" \
			    " through a variable")
		else
			report("EMPTY", tfile, tline, "target " cur \
			    " executes nothing")
	}
	cur = ""
}

# The compiler, its flags and the allocator options on one line.
function scan(s,    rest)
{
	if (s ~ /^[ \t]*#/)
		return
	if (s ~ /\$[({]CC[)}]/ || s ~ /bsd\.prog\.mk/ ||
	    s ~ /(^|[ \t=])[A-Za-z0-9_.\/-]+\.c([ \t]|$)/)
		usesc = 1
	if (s ~ /^[ \t]*CFLAGS[ \t]*[+:!?]?=/) {
		if (s ~ /^[ \t]*CFLAGS[ \t]*\?=/)
			report("FLAGS", FILENAME, FNR,
			    "CFLAGS ?= has no effect after sys.mk")
		else if (s ~ /-std=c99([ \t]|$)/ && s ~ /-Wall([ \t]|$)/ &&
		    s ~ /-Wextra([ \t]|$)/ && s ~ /-Wpedantic([ \t]|$)/ &&
		    s ~ /-Werror([ \t]|$)/)
			strict = 1
		else if (s !~ /^[ \t]*CFLAGS[ \t]*\+=/)
			report("FLAGS", FILENAME, FNR, "CFLAGS replaced" \
			    " without the strict set")
	}
	if (s ~ /^[ \t]*CC[ \t]*[+:!?]?=/ &&
	    s !~ /^[ \t]*CC[ \t]*:?=[ \t]*cc[ \t]*$/)
		report("CC", FILENAME, FNR, "CC is not the base cc")
	if (s ~ /(^|[ \t])-(Wno-|w([ \t]|$)|fpermissive)/)
		report("WEAKEN", FILENAME, FNR, "an option weakens the" \
		    " warnings")
	rest = s
	gsub(/MALLOC_OPTIONS=S([ \t]|$)/, "", rest)
	if (rest ~ /MALLOC_OPTIONS/)
		report("MALLOC", FILENAME, FNR, "MALLOC_OPTIONS is set to" \
		    " other than S, or unset")
}

FNR == 1 {
	close_target()
	cont = 0
}

cont {
	if (cur != "" && !noop($0))
		real++
	cont = ($0 ~ /\\$/)
	scan($0)
	next
}

/^\t/ {
	if (cur != "" && !noop(substr($0, 2)))
		real++
	cont = ($0 ~ /\\$/)
	scan(substr($0, 2))
	next
}

/^[ \t]*(#|$)/ {
	next
}

{
	close_target()
	scan($0)
}

/^\./ {
	next
}

/^[A-Za-z0-9_\/.%{}$() \t-]+:/ && !/^[^:]*:=/ {
	names = $0
	sub(/[ \t]*:.*/, "", names)
	rest = $0
	sub(/^[^:]*:+[ \t]*/, "", rest)
	pick = ""
	n = split(names, name, /[ \t]+/)
	for (i = 1; i <= n; i++) {
		if (index(name[i], "$") > 0)
			vartargets++
		if (name[i] == "check")
			hascheck = 1
		if (pick == "" && ischeck(name[i]))
			pick = name[i]
	}
	if (pick == "")
		next
	cur = pick
	tfile = FILENAME
	tline = FNR
	real = delegates = vardeps = 0
	vartarget = (index(pick, "$") > 0)
	deps = rest
	if ((k = index(rest, ";")) > 0) {
		deps = substr(rest, 1, k - 1)
		if (!noop(substr(rest, k + 1)))
			real++
	}
	n = split(deps, dep, /[ \t]+/)
	for (i = 1; i <= n; i++) {
		if (ischeck(dep[i]))
			delegates = 1
		if (index(dep[i], "$") > 0)
			vardeps = 1
	}
}

END {
	close_target()
	if (usesc && !strict)
		report("FLAGS", makefile, 1, "C is built, but no CFLAGS line" \
		    " carries -std=c99 -Wall -Wextra -Wpedantic -Werror")
	if (!hascheck) {
		if (vartargets || unresolved)
			unknown(makefile, 1, "no check target seen; a target" \
			    " or an include is spelled through a variable")
		else
			report("NOCHECK", makefile, 1, "no check target")
	}
	if (found)
		exit 1
	if (unknowns)
		exit 3
	exit 0
}
