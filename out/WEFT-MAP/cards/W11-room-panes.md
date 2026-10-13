# W11 room-panes: panes, layout and direction

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 2,350 lines in weft/room (adds about 1,150). The estimate:
  - weft_panes 366, weft_room_layout 123 and weft_direction 138 make
    627; times 1.15 gives about 720;
  - regress is about 430: test_panes 272, directional 258, and the
    edge-api rows, de-duplicated.

## Goal
Port pane open and close custody, tidy layout and directional
selection.

## Inputs (frozen)
- rm/component/weft_panes.c, weft_panes.h, weft_room_layout.c,
  weft_room_layout.h, weft_direction.c, weft_direction.h, PANES.md
- rm/component/test_panes.c, pane_controls.h, layout_controls.h
- ctl/directional/test_direction.c, ctl/edge-api/test_edge_api.c

## Outputs (kit)
weft/room/weft_panes.h weft/room/weft_panes.c weft/room/weft_layout.h
weft/room/weft_layout.c weft/room/weft_direction.h
weft/room/weft_direction.c weft/room/Makefile weft/room/regress/Makefile
weft/room/regress/panes_test.c weft/room/regress/direction_test.c
RETURN.md

## Deletions
- PANES.md is not carried. Its close-gate contract goes to the weft.7
  room section (W32) and to a comment beside the gate.
- edge-api/OFFER.md is RETIRE (an offer note). edge-api/build-edge-api.sh
  goes, because its include of `../../room-motion/component/test_room.c`
  becomes a link against the room library.
- The CONTRACT.md that weft_room_layout.h:2 cites does not exist. The
  reference is replaced by the manual.

## Safeguards that must survive
- The close gate gives DENIED, STALE or BUSY and never kills a process
  (PANES.md, weft_panes.c).
- The 32-node scene cap.
- Direction uses half-plane and overlap with no wrap, and refuses an
  invalid argument.
- Layout is computed only from the caller's explicit tidy rects.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Direction rows include the refusals.
3. Close-gate rows: a stale or busy pane is refused with the scene
   unchanged.
4. Red control: a direction function that wraps, built only in regress,
   FAILs the no-wrap row. Today the directional control's red is weak
   (NOTES), so this row is new.

## Rollback
Revert. The two Makefiles return to their W10 blobs.
