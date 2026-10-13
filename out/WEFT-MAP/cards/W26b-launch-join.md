# W26b launch-join: the whole session joined, device-free

- **Proposed goal:** WEFT-4. **Class:** rewrite (port of controls).
  **Tier:** c99.
- **Ceiling:** 4,810 lines in weft/launch (adds about 1,700). The
  estimate:
  - the kept root-join controls are about 1,495 frozen lines: 2,077,
    less test_idle_join and test_responsiveness (DEFER, D6), and less
    the producer rows W19 takes;
  - test_live_launcher is 130;
  - robust-base is 879 lines: robust.h, four cycle tests, run.sh and a
    README. Its tests include the same join harness;
  - about 30% comes off for the shared harness, which is included as a
    `.c` file six times today and once more by robust-base.

## Goal
Keep the end-to-end proofs: keyboard, then producer, then store, then
motion, then the substituted DRM. They run as one regress suite through
the kit libraries, not through `#include` of other directories' `.c`
files.

## Inputs (frozen)
- ctl/root-join/test_clients_join.c (the harness), ram_launcher.c,
  delayed_producer.c, test_client_cap.c, test_deadline.c,
  test_full_join.c, test_pane_runtime.c, test_soak_eof.c,
  test_store_churn.c, test_undo.c, test_viewport_join.c, Makefile
- ctl/live/test_live_launcher.c, ctl/live/producer_probe.c (shared with
  W25)
- ctl/robust-base/robust.h, test_eof_cycles.c, test_exit_reopen.c,
  test_flood_focus.c, test_recovery_busy.c, run.sh, README.md, Makefile

## Outputs (kit)
weft/launch/regress/Makefile weft/launch/regress/join.h
weft/launch/regress/join.c weft/launch/regress/ram_launcher.c
weft/launch/regress/join_test.c weft/launch/regress/undo_test.c
weft/launch/regress/launcher_argv_test.c
weft/launch/regress/robust_test.c RETURN.md

## Deletions
- The `#include "../../launcher/test_session.c"`,
  `../../room-motion/weft-room-motion.c` and `wp_drm_adapter.c` includes
  become links against the launch, motion and driver objects.
- test_idle_join and test_responsiveness are wall-clock latency
  measures. They leave regress (DEFER, D6) and become one supervised
  measurement outside `make check`.
- test_deadline and delayed_producer keep their proof (termination on
  source time) with an injected clock in place of nanosleep.
- The root-join Makefile's orphan targets go. Every kept test runs under
  `check`.
- robust-base's latency measures go with D6. Its README table (zero
  strays, flat fd and RSS, restoration readback MATCH) becomes assertions
  in robust_test.c. Today robust-base has no explicit red control, so
  this card adds one.

## Safeguards that must survive
- A real store-permission failure must still end the session
  (test_undo.c:1-4; Hugh's UNDO failure).
- No fork-then-fail at the last finite frame.
- An orderly EOF still delivers the final frame.
- Store churn beyond the history limits is survived.
- The viewport-edge failure is reproduced and stays fixed.
- The launcher execs the producer with the exact argv and reaps it.

## Acceptance (on the build host)
1. Strict build.
2. Every join row passes with no device. Only GETMAP and the DRM edges
   are fixtures.
3. Red control: test_undo against a launcher that ignores the store
   failure, built only in regress, FAILs.
4. Zero strays, and flat fd counts across EOF, exit and reopen, flood
   and busy-recovery cycles.
5. A second red control: a stray child planted by a regress-only build
   FAILs robust_test.

## Rollback
Revert. The regress Makefile returns to its W26 blob.
