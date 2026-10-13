# sidecar(1): check and select weft-byte-range/1 sidecars (ADDRESSING.md).
# "make" builds sidecar; "make test" also builds sidecar-mutant, the standing
# wrong implementation that clamps ranges, and runs the controls against
# both; "make lint" checks the manuals.  There is no install target.

CFLAGS=	-std=c99 -Wall -Wextra -Wpedantic -Werror -O2
MANS=	sidecar.1 sidecar.5

all: sidecar

sidecar: sidecar.c
	${CC} ${CFLAGS} ${LDFLAGS} -o sidecar sidecar.c

sidecar-mutant: sidecar.c
	${CC} ${CFLAGS} -DSIDECAR_MUTANT ${LDFLAGS} -o sidecar-mutant sidecar.c

test: sidecar sidecar-mutant
	env MALLOC_OPTIONS=S sh tests/run.sh ./sidecar ./sidecar-mutant

lint:
	mandoc -Tlint -W style -Ios=ELSEWHERE ${MANS}

clean:
	rm -f sidecar sidecar-mutant

.PHONY: all test lint clean
