# W24 launch-reap: the producer reaper

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 660 lines in weft/launch. The estimate:
  - ns_reap.c and .h are 342 lines, about 390 after reflow;
  - regress: test_reap is 241 lines, with its seam changed.

## Goal
Port the producer reaper: one TERM, one KILL, WNOHANG polling, and
ECHILD as loss of authority.

## Inputs (frozen)
- ns/launcher/ns_reap.c, ns_reap.h, test_reap.c, Makefile (test_reap
  rule)

## Outputs (kit)
weft/launch/Makefile weft/launch/ns_reap.h weft/launch/ns_reap.c
weft/launch/regress/Makefile weft/launch/regress/reap_test.c RETURN.md

## Deletions
- `-Wl,--wrap=kill,waitpid,clock_gettime` becomes a `-D` rename seam on
  the one object (STANDARDS 1: no GNU-linker idiom in the kit build).

## Safeguards that must survive
- The signal handler only calls kill(SIGTERM), and only while authority
  is held (ns_reap.c:21-26).
- WNOHANG polling, with ECHILD ending authority (:95-101, :172-179).
- At most one SIGTERM and one SIGKILL, with ESRCH tolerated (:164-210).
- No waitpid(-1).
- External cancellation never yields OK (ns_reap.h).

## Acceptance (on the build host)
1. Strict build and check both hold.
2. All test_reap rows pass with the `-D` seam. The injected clock is
   deterministic.
3. Red control: a reaper that sends a second KILL after ECHILD, built
   only in regress, FAILs.

## Rollback
Revert the one commit.
