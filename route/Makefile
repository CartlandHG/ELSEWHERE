# route(8): the queue and dispatcher of the spine (SPINE.md, goal SPINE-3).
# "make" builds route; "make test-build" builds route-test, the fault build
# that route(8) CAVEATS describes.  There is no install target.

CFLAGS=	-std=c99 -Wall -Wextra -Wpedantic -Werror -O2

all: route

route: route.c
	${CC} ${CFLAGS} ${LDFLAGS} -o route route.c

test-build: route-test

route-test: route.c
	${CC} ${CFLAGS} -DROUTE_TEST ${LDFLAGS} -o route-test route.c

clean:
	rm -f route route-test

.PHONY: all test-build clean
