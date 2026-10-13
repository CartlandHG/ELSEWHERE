# W25 launch-presenter: the in-process motion presenter and its report

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99:
  it decodes the producer's stream.
- **Ceiling:** 1,560 lines in weft/launch (adds about 900). The estimate:
  - vendor/weft_motion_present.c and .h are 447 lines and wmp_report is
    107; reflowed, about 640;
  - regress is about 260: live test_live_stream, test_live_supervisor,
    test_producer_grace and the presenter rows.

## Goal
Port the presenter. It reads WNPOSE01 frames from the producer pipe,
validates them with W16's decoder, and hands them to W23's adapter
under W21's supervisor.

## Inputs (frozen)
- ns/launcher/vendor/weft_motion_present.c, weft_motion_present.h,
  wmp_report.c, wmp_report.h
- ctl/live/test_live_stream.c, test_live_supervisor.c,
  test_producer_grace.c, producer_probe.c

## Outputs (kit)
weft/launch/wmp_present.h weft/launch/wmp_present.c
weft/launch/wmp_report.h weft/launch/wmp_report.c weft/launch/Makefile
weft/launch/regress/Makefile weft/launch/regress/present_test.c
weft/launch/regress/producer_probe.c RETURN.md

## Deletions
- The `vendor/` directory and the `provenance-check` rule. The rule
  checks `vendor/SHA256`, which does not exist, so `run.sh` fails at
  that step today.
- In the kit, the presenter is house code under review, not a vendored
  drop. Its origin goes to the manual's HISTORY.

## Safeguards that must survive
- Every frame is decoded and checked (alpha, size, sequence) before
  presentation.
- A 15 s frame-stall bound in soak.
- The presenter terminates TOTAL-MS + 3 s after start, giving the
  producer one second to drain.
- The counter-overflow and clock-jump rows (+306 s injected).

## Acceptance (on the build host)
1. Strict build and check both hold.
2. A frame stream from producer_probe is presented to the fixture
   adapter.
3. A corrupt frame is refused.
4. A stall bound is hit with an injected clock.
5. Red control: the presenter with the stall bound removed, built only
   in regress, FAILs the stall row.

## Rollback
Revert. The Makefiles return to their W24 blobs.
