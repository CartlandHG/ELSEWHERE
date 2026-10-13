# W31 views-join: room views in the producer

- **Proposed goal:** WEFT-5. **Class:** rewrite (port) of
  weft_room_views, plus a repair to the producer. **Tier:** c99.
- **Ceiling:** 5,150 lines in weft/views (adds about 650) and 5,800 in
  weft/motion (adds about 100). The estimate:
  - weft_room_views.c and .h are 449 lines, about 520 after reflow;
  - the producer's `wrv_*` call sites are about 25 lines, and adding the
    docstore unveil and the open brings that to about 80;
  - regress is about 150.

## Goal
Port the registry that owns one dr open and every pane lifetime for the
scene's view nodes, and join it into the producer.

## Inputs (frozen)
- rm/weft_room_views.c, weft_room_views.h
- rm/weft-room-motion.c: the `wrv_*` call sites (about 25 lines)

## Outputs (kit)
weft/views/room_views.h weft/views/room_views.c weft/views/Makefile
weft/views/regress/Makefile weft/views/regress/room_views_test.c
weft/motion/weft_room_motion.c weft/motion/weft-room-motion.1
weft/motion/Makefile RETURN.md

## Deletions
None planned.

## Safeguards that must survive
- The pane is the only path to the document store. This module and the
  producer never call `dr_*` for these documents except through pane
  verbs.
- At most WRV_MAX = 8 views.
- Reconcile is idempotent and runs only after confirmed scene changes.
- WN_VIEWPANE is admitted only with WN_INPUT on a WINDOW node (W01).

## Added authority
- `WEFT_DOCSTORE` stays as an environment interface, documented in the
  manual.
- The producer unveils the docstore `rwc` and needs `flock`, which W18
  already pledges.

## Acceptance (on the build host)
1. Strict build.
2. All W18 and W19 rows still hold.
3. A view pane renders a pinned revision and holds across a save.
4. A view node without WN_INPUT is refused.
5. Red control: a producer that calls `dr_resolve` directly is caught
   by a link-time check (`nm` on the producer object must show no `dr_`
   symbol outside room_views and weft_pane).

## Rollback
Revert. The three motion files return to their W19 blobs.
