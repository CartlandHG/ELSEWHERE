# W21 driver-supervisor: the display child supervisor

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99:
  it holds child processes.
- **Precondition:** D5 (atomics) is decided. If it is still open, the
  header is carried unchanged with a debt entry.
- **Ceiling:** 1,800 lines in weft/driver (adds about 700). The estimate:
  - wp_supervisor_openbsd.c and .h are 318 lines and wp_atomic_openbsd.h
    is 48; reflowed, about 420;
  - regress: test_quiescence is 248 lines, plus the seam.

## Goal
Port the single-threaded supervisor that forks the display child, waits
on kqueue, kills only that child, reaps within a bound, and recovers only
after a reap.

## Inputs (frozen)
- ns/base-driver/wp_supervisor_openbsd.c, wp_supervisor.h,
  wp_atomic_openbsd.h
- ns/base-driver/CONTRACT.md, README.md, which go to wp_supervise.3
- ns/base-driver/tests/test_quiescence.c

## Outputs (kit)
weft/driver/wp_supervisor.h weft/driver/wp_supervisor.c
weft/driver/wp_atomic.h weft/driver/wp_supervise.3 weft/driver/Makefile
weft/driver/regress/Makefile weft/driver/regress/quiescence_test.c
RETURN.md

## Deletions
- CONTRACT.md and README.md prose goes to the manual. The packet history
  stays in frozen src.

## Safeguards that must survive
These are at wp_supervisor_openbsd.c lines in the frozen copy.
- The bounded reap: after one kill, at most 64 WNOHANG polls within a
  250 ms monotonic budget, and no blocking waitpid (:37-69).
- ECHILD sets loss of authority, and no kill follows it (:53, :145-146,
  :161).
- Kill targets only the exact child PID, never 0 or a group (:163-167).
- The kqueue EVFILT_SIGNAL is registered before fork, and SIGCHLD is
  never ignored (:103-107).
- Signals are blocked across fork. The child resets its dispositions
  (:115-125).
- When the child was not reaped, normal recovery is skipped and only the
  parent-only abandon runs (:179-188).
- The result is never OK after a skipped recovery or a failed kill or
  reap (:214-216).

## Debt carried (D5, D6)
- `__atomic_*` builtins on memory shared between processes (STANDARDS 1,
  dialect). Owner: the integrator. The remedy is the D5 ruling. The
  review is reopened by any change to wp_atomic.h.
- The real 250 ms budget stays in the product. Regress injects the clock
  through a `-D` seam (the frozen `supervisor-q.o` pattern).

## Acceptance (on the build host)
1. Strict build.
2. The 12 quiescence cases pass normally and under the UB trap build.
3. Red control: one of the frozen "red6 to green0" controls, recorded in
   the custody-repair evidence, rebuilt against a supervisor that kills
   after ECHILD, is detected.
4. wp_supervise.3 is lint clean.

## Rollback
Revert. The Makefiles return to their W20 blobs.
