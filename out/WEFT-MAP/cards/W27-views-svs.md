# W27 views-svs: saved-view sessions

- **Proposed goal:** WEFT-5. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 1,050 lines in weft/views. The estimate:
  - svs.c and .h are 563 lines, about 650 after reflow;
  - regress is about 400: svs_example.c (387) reworked to in-tree
    fixtures.

## Goal
Port the svs module: eight caller-owned view slots over exact
revisions, pinned or following.

## Inputs (frozen)
- weft/deps/svs/svs.c, svs.h, svs_example.c, Makefile, README.md

## Outputs (kit)
weft/views/Makefile weft/views/svs.h weft/views/svs.c weft/views/svs.3
weft/views/regress/Makefile weft/views/regress/svs_test.c RETURN.md

## Deletions
- run.sh and the `../../M02` fixtures it needs are absent, so it is
  RETIRE. svs_test.c builds its fixtures through W08's dr in a temporary
  directory.
- README's "pure C11" claim is false: the build is C99. The manual says
  C99.

## Safeguards that must survive
- `svs_advance` is all-or-nothing compare-and-switch across the session
  (svs.c:329-331).
- SVS_MOVED is returned for a stale point or close (:174, :240, :274,
  :299).
- `anchor_clamped` is sticky (:25, :247-250).
- Closed ids answer SVS_STALE forever.
- No allocation, IO or descriptors.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. A pinned view holds across saves. A following view moves only on a
   verified observation. A shorter head clamps and flags.
3. Red control: a non-atomic advance, built only in regress, FAILs the
   CAS row.
4. svs.3 is lint clean.

## Rollback
Revert the one commit.
