# W02 lib-raster: the software rasteriser and the cell grid

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 3,350 lines in weft/lib (adds about 1,650). The estimate:
  - weft_raster.c and .h are 876 lines and weft_terminal.h (the cell
    types the raster reads) is 39, so 915 times 1.15 gives about 1,060;
  - regress is about 560: focus-raise 512 and the native-terminal cell
    rows, de-duplicated.

## Goal
Port the software rasteriser, including the help strip, shortcuts,
image layers and the ambient duck. Move the cell-grid type it draws into
lib, so that lib does not reach into the pty component.

## Inputs (frozen)
- rm/component/core/weft_raster.c, weft_raster.h
- ns/terminal/weft_terminal.h: the cell types. weft_raster.c includes it
  through `../../../terminal/`.
- ns/DUCK-PROVENANCE.md, which becomes a source comment and the manual
  HISTORY
- ctl/focus-raise/test_focus_raise.c, Makefile
- ctl/native-terminal/test_raster_cells.c, test_boundaries.c: the raster
  rows only

## Outputs (kit)
weft/lib/weft_raster.h weft/lib/weft_raster.c weft/lib/weft_cells.h
weft/lib/weft_raster.3 weft/lib/Makefile weft/lib/regress/Makefile
weft/lib/regress/raster_test.c weft/lib/regress/cells_test.c RETURN.md

## Deletions
- The drifted copies deps/compare/inputs/raster/.../weft_raster.[ch] and
  terminal/weft_terminal.h are not carried.
- The upward include path `../../../terminal/` goes. weft_cells.h holds
  only the cell types the raster reads (W13 includes it). The type names
  stay the same, so this is a move with no semantic change.

## Safeguards that must survive
- An invalid scene is rejected before any pixel is written.
- A refusal scene paints its refusal words, skips all world nodes and
  has no duck.
- BLIND and REFUSAL have no duck.
- Integer duck scale is bounded to 1..8.
- After the nodes, the duck resets the clip to the viewport.
- The report counts drawn nodes, missing glyphs and clipped text.
- Pixel-count bounds (`WN_RASTER_PIXELS_MAX`) apply before allocation in
  callers.

## Acceptance (on the build host)
1. Strict build and `make check` both hold.
2. Golden digests over fixed scenes and a fixed 4096-byte test font. The
   test font is generated in regress from a formula, not the asset, so
   W02 does not depend on D4.
3. A refusal scene has no duck pixels.
4. An oversize clip is bounded.
5. Hostile cell content (CSI bytes in cells) draws as cells, never as
   control.
6. Red control: one golden digest deliberately altered makes its row
   FAIL.
7. weft_raster.3 is lint clean.

## Rollback
Revert the commit. weft/lib/Makefile returns to its W01 blob.
