# W19 motion-joins: terminal clients, help, menu and status in the producer

- **Proposed goal:** WEFT-3. **Class:** repair (adds joins to W18's
  file). **Tier:** c99.
- **Ceiling:** 5,700 lines in weft/motion (adds about 1,250). The estimate:
  - the producer's client calls are about 30 lines, text delivery 35,
    and the status, help and menu overlay about 150 (the garden panel
    text dropped); with the input-path joins, about 380 after reflow;
  - regress is about 520: help test_client_views and
    test_unmapped_input, and the two non-garden Python journeys
    (test_producer.py, test_actions_producer.py) rewritten as sh driving
    the C event writer (keys/regress/events.c) and a C frame decoder;
  - about 350 more for the producer rows with injected clock and poll:
    live/test_live_room 64, root-join/test_input_counter 154 and
    test_quiet_producer 224, with the harness shared.

## Goal
Join the W15 client registry, W06 help and menu, and W03 status into the
producer, and widen its pledge to run pane children.

## Inputs (frozen)
- rm/weft-room-motion.c: 835-1013 (overlay), 1508-1542 (text delivery),
  and the `wrmc_*` call sites
- ctl/help/test_client_views.c, test_unmapped_input.c, test_producer.py,
  test_actions_producer.py, plant_focused_view.c, README.md
- ctl/live/test_live_room.c, ctl/root-join/test_input_counter.c,
  test_quiet_producer.c. These include weft-room-motion.c as a `.c`
  file today; the port links the producer objects instead.

## Outputs (kit)
weft/motion/weft_room_motion.c weft/motion/weft-room-motion.1
weft/motion/Makefile weft/motion/regress/Makefile
weft/motion/regress/views_test.c weft/motion/regress/journey.sh
weft/motion/regress/decode.c weft/motion/regress/producer_test.c RETURN.md

## Deletions
- The garden branches in `wrm_deliver_text` and the help panel.
- Python leaves weft's checks entirely.

## Safeguards that must survive
- A refused app adds no layer and creates no file
  (test_client_views.c:45-54).
- Menu typing never reaches an app.
- Text goes only to the focused client.
- The eight-client cap is enforced through W15.

## Added authority
The pledge gains `tty proc exec`, and the unveil gains `/bin/ksh` and
`/usr/bin/vi` `x` plus `/dev/ptm`. The child envelope comes from W14.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. The journey opens a ksh pane and types into it. The frame decoder
   shows the cells.
3. A refused app leaves no layer.
4. Red control: text delivery forced to an unfocused client, built only
   in regress, FAILs.
5. W18's rows still hold.
6. Counter exhaustion gives WN_LIMIT. Completion records match the
   decoded wire under an injected clock.

## Rollback
Revert. weft_room_motion.c and the manual return to their W18 blobs.
