# W17 motion-custody: frame custody and the room hand-off, without physics

- **Proposed goal:** WEFT-3. **Class:** rewrite (port, split).
  **Tier:** c99.
- **Precondition:** D2. The owner accepts static posing, with physics
  deferred.
- **Ceiling:** 2,550 lines in weft/motion (adds about 1,050). The
  estimate:
  - weft_motion_room.c is 678 lines, of which about 500 are custody;
    reflowed, about 575;
  - weft_room_motion_handoff is 122 lines, about 140 after reflow;
  - regress is about 330: motion-direct test_constructor_alloc,
    test_desktop_custody and test_idle_skip (the frame tests), and
    help/test_help_frame.

## Goal
Port the frame custody state machine and the room-to-motion hand-off.
Pose each window at its committed target rect, so the posed scene is the
committed scene.

## Inputs (frozen)
- rm/weft_motion_room.c. Keep `wmf_create`, `wmf_apply` (the scene
  witness), the state machine, `internal_prepare`, the change filter,
  `wmf_complete`, `wmf_abort` and the overlays. Leave `wmf_tick` physics
  and `wmf_pose_scene`'s bridge read.
- rm/weft_room_motion_handoff.c, weft_room_motion_handoff.h
- ctl/motion-direct/test_constructor_alloc.c, test_desktop_custody.c,
  test_idle_skip.c, test_tick_visible.c (the visible-change report kept
  for the heartbeat), ctl/help/test_help_frame.c

## Outputs (kit)
weft/motion/wmf_room.h weft/motion/wmf_room.c weft/motion/wmf_handoff.h
weft/motion/wmf_handoff.c weft/motion/Makefile
weft/motion/regress/Makefile weft/motion/regress/custody_test.c RETURN.md

## Deletions
These are DEFER, and stay in frozen src:
- engine/* (1,710 lines);
- weft_room_bridge.[ch];
- the `wrb_*` calls in weft_motion_room.c.

The old and new contracts:
- **Old:** windows are posed by a damped servo, and `steps` counts
  physics ticks.
- **New:** windows are posed at target. `steps` is 0 and `posed_scene`
  equals `source_scene` in every WNPOSE01 header.
- The manual says so. A consumer that relied on nonzero steps is named
  by grep in RETURN.md; the survey found none outside the deferred
  motion-direct controls.

Two more deletions:
- The `wmf_tick_visible` heartbeat logic stays only as far as the
  producer's 3 s heartbeat frame needs it (weft-room-motion.c:3245-3248).
- motion_benchmark.c and test_motion_direct.c (its 500 ms ceiling) are
  DEFER with physics.

## Safeguards that must survive
- The apply witness: the scene digest must match the receipt, and
  publication is monotonic (weft_motion_room.c:187-219).
- Create bounds, with a frame limit of 4096 unless live (:165-176).
- Aliasing and span disjointness, and capacity bounds, in prepare
  (:441-500).
- The EMPTY, READY, PENDING and ABORTED states never skip.
- The hand-off gates on room pending, motion state and the expected
  receipt (weft_room_motion_handoff.c:6-60).

## Acceptance (on the build host)
1. Strict build and check both hold.
2. A mismatched witness is refused.
3. An allocation failure at construction is refused cleanly
   (test_constructor_alloc).
4. A pending frame cannot be overwritten.
5. Red control: the witness check dropped by a regress-only `-D` FAILs.
6. A diff of the posed scene against the source scene is empty on every
   regress frame.

## Rollback
Revert. The Makefiles return to their W16 blobs. Physics stays
available in frozen src for a later goal.
