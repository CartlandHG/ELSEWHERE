# W22 driver-frame: frame copy and paced publication

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 2,760 lines in weft/driver (adds about 960). The estimate:
  - wb_frame.c and .h are 88 lines and wp_paced_body.inc and
    wp_paced_drm.h are 307; reflowed, about 455;
  - regress is about 500: test_frame_scanout 106, test_publication 95,
    and stream/test_stream and test_dirty (the scanout rows).

## Goal
Port the frame copy into the scanout and the paced publication slots
shared with the display child.

## Inputs (frozen)
- ns/base-driver/wb_frame.c, wb_frame.h, wp_paced_body.inc,
  wp_paced_drm.h
- weft/FLICKER-CHANGE.md: its contract only. wb_frame writes each
  visible pixel once.
- ns/base-driver/tests/test_frame_scanout.c, test_publication.c
- ctl/stream/test_stream.c, test_dirty.c, Makefile

## Outputs (kit)
weft/driver/wb_frame.h weft/driver/wb_frame.c weft/driver/wp_paced.h
weft/driver/wp_paced.c weft/driver/Makefile weft/driver/regress/Makefile
weft/driver/regress/frame_test.c weft/driver/regress/stream_test.c
RETURN.md

## Deletions
- `wp_paced_body.inc` is a `.c` body included by its user. It becomes
  `wp_paced.c`, a normal translation unit with the same functions.
- FLICKER-CHANGE.md is not carried. Its claim, that tear-free changing
  frames are not claimed, goes to the manual CAVEATS.

## Safeguards that must survive
- Margins, padding, bounds, alpha and refusal guarantees in
  `wb_frame_copy`, with no transient erase.
- Publication slots are lock-free, and only fully published slots are
  read.
- A digest corruption is refused.
- A DIRTYFB failure is reported.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. A store-observing regression shows that the five frozen cases fail
   against the old transient-write sequence and pass against the port.
3. Red control: that old sequence.
4. A corrupt digest is refused.

## Rollback
Revert. The Makefiles return to their W21 blobs.
