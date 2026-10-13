# W13 pty-terminal: the terminal cell model

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99:
  it parses untrusted terminal output.
- **Ceiling:** 1,100 lines in weft/pty. The estimate:
  - weft_terminal.c is 545 lines, about 630 after reflow;
  - regress is about 450: native-terminal test_boundaries,
    test_author_r2 and the terminal rows of test_raster_images.

## Goal
Port the escape-sequence decoder that turns a pane's output bytes into
the lib cell grid (weft_cells.h, from W02).

## Inputs (frozen)
- ns/terminal/weft_terminal.c, ns/terminal/weft_terminal.h (the
  remaining declarations), ns/terminal/README.md
- ctl/native-terminal/test_boundaries.c, test_author_r2.c,
  test_raster_images.c, Makefile

## Outputs (kit)
weft/pty/Makefile weft/pty/weft_terminal.h weft/pty/weft_terminal.c
weft/pty/regress/Makefile weft/pty/regress/terminal_test.c RETURN.md

## Deletions
- README.md is not carried. Its contract goes to weft_terminal.h and the
  weft-room-motion manual.
- test_author_r2.c's `return 1` style is folded into terminal_test.c as
  rows. No assertion is dropped.

## Safeguards that must survive
- Hostile CSI and OSC sequences are bounded: a parameter count and
  length cap, and no sequence ever becomes a control on the host.
- The grid is bounded by its declared size, and wrap never writes out of
  bounds.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Each hostile-sequence row holds.
3. Red control: a parameter cap raised by a regress-only `-D` FAILs the
   overflow row.

## Rollback
Revert the one commit.
