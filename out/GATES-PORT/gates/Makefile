# gates: the kit's checks, gates(7).  "make check" runs every gate on every
# control in ../controls through want, prints one line per case, and fails
# unless each case gives its wanted result.  Every case runs, so a failure
# never hides the cases after it.

C=	../controls

check:
	@cd ${.CURDIR}; st=0; \
	sh want 0 sh foreign-lint . || st=1; \
	sh want 0 sh foreign-lint ${C}/foreign/clean || st=1; \
	sh want -l NO-GIT 1 sh foreign-lint ${C}/foreign/no-git-file || st=1; \
	sh want -l NO-GIT 1 sh foreign-lint ${C}/foreign/no-git-run || st=1; \
	sh want -l NO-LINUX 1 sh foreign-lint ${C}/foreign/no-linux || st=1; \
	sh want -l NO-GNU-C 1 sh foreign-lint ${C}/foreign/no-gnu-c || st=1; \
	sh want -l NO-GNU-MAKE 1 sh foreign-lint ${C}/foreign/no-gnu-make || st=1; \
	sh want -l POSIX-SH 1 sh foreign-lint ${C}/foreign/posix-sh || st=1; \
	sh want -l POSIX-SH 1 sh foreign-lint ${C}/foreign/posix-sh-array || st=1; \
	sh want -l NO-GNU-TOOL 1 sh foreign-lint ${C}/foreign/no-gnu-tool || st=1; \
	sh want -l C99-FLAGS 1 sh foreign-lint ${C}/foreign/c99-make || st=1; \
	sh want -l C99-FLAGS 1 sh foreign-lint ${C}/foreign/c99-shell || st=1; \
	sh want -l PYTHON 1 sh foreign-lint ${C}/foreign/python || st=1; \
	sh want -l PYTHON 1 sh foreign-lint ${C}/foreign/python-run || st=1; \
	sh want 0 sh mk-lint . || st=1; \
	sh want 0 sh mk-lint ${C}/mk/good || st=1; \
	sh want 0 sh mk-lint ${C}/mk/include-green || st=1; \
	sh want 0 sh mk-lint ${C}/mk/delegate-green || st=1; \
	sh want -l EMPTY 1 sh mk-lint ${C}/mk/empty-red || st=1; \
	sh want -l EMPTY 1 sh mk-lint ${C}/mk/noop-red || st=1; \
	sh want -l EMPTY 1 sh mk-lint ${C}/mk/backslash-red || st=1; \
	sh want -l EMPTY 1 sh mk-lint ${C}/mk/semicolon-red || st=1; \
	sh want -l EMPTY 1 sh mk-lint ${C}/mk/baddeleg-red || st=1; \
	sh want -l NOCHECK 1 sh mk-lint ${C}/mk/none-red || st=1; \
	sh want -l FLAGS 1 sh mk-lint ${C}/mk/flags-red || st=1; \
	sh want -l FLAGS 1 sh mk-lint ${C}/mk/defaulted-red || st=1; \
	sh want -l FLAGS 1 sh mk-lint ${C}/mk/noflags-red || st=1; \
	sh want -l WEAKEN 1 sh mk-lint ${C}/mk/weaken-red || st=1; \
	sh want -l MALLOC 1 sh mk-lint ${C}/mk/malloc-red || st=1; \
	sh want -l CC 1 sh mk-lint ${C}/mk/cc-red || st=1; \
	sh want -l UNKNOWN 2 sh mk-lint ${C}/mk/var-unknown || st=1; \
	sh want -l UNKNOWN 2 sh mk-lint ${C}/mk/vardeleg-unknown || st=1; \
	sh want -l UNKNOWN 2 sh mk-lint ${C}/mk/varinclude-unknown || st=1; \
	sh want -l UNKNOWN 2 sh mk-lint ${C}/mk/include-missing || st=1; \
	sh want 0 sh banned-lint ${C}/banned/clean || st=1; \
	sh want -l BANNED 1 sh banned-lint ${C}/banned/red || st=1; \
	sh want 0 sh foreign-lint ${C}/walk/fixtures-good || st=1; \
	sh want 2 sh foreign-lint ${C}/walk/fixtures-bad || st=1; \
	sh want 2 sh foreign-lint ${C}/walk/bad-name || st=1; \
	sh want 2 sh foreign-lint ${C}/walk/nothing || st=1; \
	sh want 2 sh foreign-lint ${C}/no-such-directory || st=1; \
	sh want 0 sh mk-lint ${C}/walk/fixtures-good || st=1; \
	sh want 2 sh mk-lint ${C}/walk/fixtures-bad || st=1; \
	sh want 2 sh mk-lint ${C}/walk/bad-name || st=1; \
	sh want 2 sh mk-lint ${C}/walk/nothing || st=1; \
	sh want 2 sh mk-lint ${C}/no-such-directory || st=1; \
	sh want 0 sh banned-lint ${C}/walk/fixtures-good || st=1; \
	sh want 2 sh banned-lint ${C}/walk/fixtures-bad || st=1; \
	sh want 2 sh banned-lint ${C}/walk/bad-name || st=1; \
	sh want 2 sh banned-lint ${C}/walk/nothing || st=1; \
	sh want 2 sh banned-lint ${C}/no-such-directory || st=1; \
	sh want -l FAIL 1 sh want 0 sh banned-lint ${C}/banned/red || st=1; \
	sh want -l KILLED 1 sh want 0 sh -c 'kill -9 $$$$' || st=1; \
	exit $$st

.PHONY: check
