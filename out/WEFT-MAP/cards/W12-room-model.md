# W12 room-model: the observed room and its receipt

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 4,100 lines in weft/room (adds about 1,750). The estimate:
  - weft_room.c and .h are 889 lines and weft_room_receipt is 100, so
    989 times 1.15 gives about 1,140;
  - regress is about 600: the rest of test_room.c, frame_support.h and
    weft_scene_source, minus the rows W10 and W11 took.

## Goal
Port `weft_room`: one input epoch, one observed room, pending intents,
menu and help state, and the receipt.

## Inputs (frozen)
- rm/component/weft_room.c, weft_room.h, weft_room_receipt.c,
  weft_room_receipt.h
- rm/component/test_room.c, frame_support.h,
  core/weft_scene_source.c, core/weft_scene_source.h, run-native.sh

## Outputs (kit)
weft/room/weft_room.h weft/room/weft_room.c weft/room/weft_receipt.h
weft/room/weft_receipt.c weft/room/weft_room.3 weft/room/Makefile
weft/room/regress/Makefile weft/room/regress/room_test.c
weft/room/regress/scene_source.c weft/room/regress/scene_source.h
RETURN.md

## Deletions
- `weft_scene_source` is a test frame source (it is used only by
  test-room). It moves to regress/.
- run-native.sh is folded into regress/Makefile. Its `ubtrap` mode
  (`-Wl,--no-execute-only`) is kept only if the reviewer wants a UB
  build, and then as a named target.
- test_room.c:34 calls `clock_gettime(CLOCK_MONOTONIC)`. It is replaced
  by an injected stamp, because STANDARDS 6 allows no clock in checks.
- The "Violette's fixed menu contract, copied verbatim" duplicate
  declarations in weft_room.h are removed. The header includes
  weft_menu.h from keys instead: one truth for each type.

## Safeguards that must survive
- The room borrows an open store until `wrc_end` and never closes or
  reuses it.
- No allocation, retained hook, device open, clock, signal or retry.
- One pending intent is consumed per attempt.
- `cas_attempted`, `confirmed` and `published` are reported separately
  and never conflated.
- The header-dependency rule in rm/component/Makefile:37-38 is kept, so
  no object is compiled against an old layout.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. All test_room rows pass with the injected stamp.
3. A permission failure on the store ends the epoch with a named result.
4. Red control: a receipt that reports published without a confirmed
   CAS, built only in regress, FAILs.
5. weft_room.3 is lint clean.

## Rollback
Revert. The Makefiles return to their W11 blobs.
