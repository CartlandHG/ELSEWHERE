# W10 room-reduce: reducer, focus and home session

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 1,200 lines in weft/room. The estimate:
  - the sewwm_* reducer and commit, weft_focus, and weft_home_adapter
    and weft_home_session make 742 lines; times 1.15 gives about 855;
  - regress is about 300: the reducer and home rows lifted from
    test_room.c and test_client_route.c.

## Goal
Port the pure reducer, which computes the next scene from an action, and
the home session, which publishes through the store's compare-and-switch.

## Inputs (frozen)
- rm/component/core/sewwm_reduce.c, sewwm_reduce.h, sewwm_commit.c,
  sewwm_display.h, weft_focus.c, weft_focus.h, weft_home_adapter.c,
  weft_home_adapter.h, weft_home_session.c, weft_home_session.h
- rm/component/test_room.c and test_client_route.c: the reducer and home
  rows only

## Outputs (kit)
weft/room/Makefile weft/room/room_reduce.h weft/room/room_reduce.c
weft/room/room_commit.c weft/room/weft_focus.h weft/room/weft_focus.c
weft/room/weft_home.h weft/room/weft_home.c weft/room/regress/Makefile
weft/room/regress/reduce_test.c RETURN.md

## Deletions
- The `sewwm_` names describe an X window manager the kit does not have.
  The functions are renamed `room_`, with the same signatures and
  behaviour.
- `weft_home_adapter` and `weft_home_session` are 80 and 172 lines
  joined by one caller. They merge into weft_home.c with the same
  functions.
- Both changes are named in RETURN.md, and the reviewer checks that no
  logic changed.
- The X-era comments about XIDs go (weft_room_bridge, which is deferred,
  holds the rest).

## Safeguards that must survive
- The reducer is pure: no IO, clock or allocation.
- Publication is only through `wn_store_publish_if`, so a stale base is
  refused (WN_MOVED) and never merged.
- The home adapter checks the receipt it expects.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Reducer rows give fixed scenes to fixed digests.
3. A stale-base publish gives WN_MOVED with the store unchanged.
4. Red control: a reducer that ignores focus, built only in regress,
   FAILs the focus row.

## Rollback
Revert the one commit.
