# W28 views-compare-lib: document diff, document view, snapshot compare

- **Proposed goal:** WEFT-5. **Class:** rewrite (port). **Tier:** c99:
  it parses untrusted UTF-8 documents.
- **Ceiling:** 2,550 lines in weft/views (adds about 1,500). The
  estimate:
  - weft_doc_diff 638, weft_document_view 320 and weft_snapshot_compare
    199 make 1,157 lines, about 1,270 after reflow;
  - regress: test_compare 187 and test_refusals 45.

## Goal
Port the diff, the paired document view (rendered through lib's raster)
and the snapshot comparison core.

## Inputs (frozen)
- weft/deps/compare/inputs/diff/weft_doc_diff.c, weft_doc_diff.h
- weft/deps/compare/inputs/view/weft_document_view.c,
  weft_document_view.h
- weft/deps/compare/weft_snapshot_compare.c, weft_snapshot_compare.h
- weft/deps/compare/test_compare.c, test_refusals.c, Makefile

## Outputs (kit)
weft/views/doc_diff.h weft/views/doc_diff.c weft/views/doc_view.h
weft/views/doc_view.c weft/views/snap_compare.h
weft/views/snap_compare.c weft/views/Makefile
weft/views/regress/Makefile weft/views/regress/compare_test.c RETURN.md

## Deletions
- deps/compare/inputs/raster/* is RETIRE (drifted duplicates). This port
  links weft/lib.
- input-pins.json is RETIRE (pins of the drifted inputs).
- The `-Dcalloc/-Dfree` seam stays test-only, named in the regress
  Makefile.

## Safeguards that must survive
- Partial or malformed diff scripts are refused
  (weft_snapshot_compare.c:134-135).
- Allocation-failure paths are covered.
- Sources are at most 65,536 bytes and must be valid UTF-8.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. The refusal rows hold.
3. An allocation failure at each site is refused cleanly.
4. Red control: a diff that accepts a truncated script, built only in
   regress, FAILs.

## Rollback
Revert. The Makefiles return to their W27 blobs.
