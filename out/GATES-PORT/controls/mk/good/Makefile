# Green control: C built with the strict flags, and a check that runs a
# program under the strengthened allocator options.
PROG=	ok
SRCS=	ok.c
CFLAGS=	-std=c99 -Wall -Wextra -Wpedantic -Werror -O2
CFLAGS+=	-g
check: ok
	env MALLOC_OPTIONS=S ./ok
.include <bsd.prog.mk>
