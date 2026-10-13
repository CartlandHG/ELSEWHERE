# W30 views-pane: the saved-view pane

- **Proposed goal:** WEFT-5. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 4,500 lines in weft/views (adds about 1,400). The
  estimate:
  - weft_pane.c and .h are 1,048 lines, about 1,200 after reflow;
  - regress is about 200: the pane rows of help/test_client_views and
    plant_focused_view.

## Goal
Port the pane: view state (svs), then exact bytes (dr_resolve), then
presentation (compare or document view), drawn into a caller-owned
raster surface.

## Inputs (frozen)
- rm/weft_pane.c, weft_pane.h
- ctl/help/test_client_views.c (the pane rows), plant_focused_view.c

## Outputs (kit)
weft/views/weft_pane.h weft/views/weft_pane.c weft/views/Makefile
weft/views/regress/Makefile weft/views/regress/pane_test.c RETURN.md

## Deletions
None planned. The reviewer looks for garden coupling; the survey found
none.

## Safeguards that must survive
These are the four contract laws in weft_pane.h:
- The pane proves every revision before render: `dr_resolve` must be
  green and the docid must be the pane's own.
- One observation per move, re-proved pane-side. A stale or fabricated
  observation is refused, with no move and no render.
- Compare by request, not by pointer.
- No lifetimes beyond the raster surface, and ids never return.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. A fabricated observation is refused.
3. A foreign docid is refused.
4. A closed id stays stale.
5. Red control: a pane that skips re-proof, built only in regress,
   FAILs.

## Rollback
Revert. The Makefiles return to their W29 blobs.
