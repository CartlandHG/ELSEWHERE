# Green control: BSD make conditionals and a C99 build.
PROG=	ok
SRCS=	ok.c
CFLAGS=	-std=c99 -Wall -Wextra -Wpedantic -Werror
.if defined(DEBUG)
CFLAGS+=	-g
.endif
check: ok
	./ok
.include <bsd.prog.mk>
