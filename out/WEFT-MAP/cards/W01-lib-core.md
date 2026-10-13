# W01 lib-core: scene, store and action

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99:
  it holds state, locks and parses untrusted store bytes.
- **Ceiling:** 1,700 lines in weft/lib. The estimate:
  - kept frozen product is 1,103 lines (scene 453, store 590, action 60);
    times 1.15 for style(9) gives about 1,270;
  - the new Makefile.inc and Makefile come to about 50;
  - regress comes to about 380: store-budget 129 plus the scene and store
    rows lifted out of test_room.c.

## Goal
Port the scene model and the content store into `weft/lib` as one
canonical copy. This is the base that every other component links.

## Inputs (frozen)
- rm/component/core/weft_scene.c, weft_scene.h
- rm/component/core/weft_store.h, weft_store_openbsd.c
- rm/component/core/weft_action.c, weft_action.h
- ctl/store-budget/test_store_budget.c, Makefile
- rm/component/test_room.c: only the scene and store rows, for regress
- weft.7, "COMPONENTS" (weft_store, weft_scene), for the manual

## Outputs (kit)
weft/Makefile.inc weft/lib/Makefile weft/lib/weft_scene.h
weft/lib/weft_scene.c weft/lib/weft_store.h weft/lib/weft_store.c
weft/lib/weft_action.h weft/lib/weft_action.c weft/lib/weft_store.3
weft/lib/regress/Makefile weft/lib/regress/scene_test.c
weft/lib/regress/store_test.c weft/lib/regress/budget_test.c RETURN.md

## Deletions
- The drifted copies of the scene are not carried:
  - deps/compare/inputs/raster/.../weft_scene.[ch];
  - lib/glyph/weft_scene.h, which W01 does not touch. D8 records that
    lib/glyph must include this header.
- Seat and date banners in headers. Attribution moves to the manual's
  AUTHORS section.
- The `-Dwn_store_publish_if=root_publish_if` build trick in
  rm/component/Makefile. The regress observes publication through the
  existing `enum wn_fault` parameter, not a macro rename.

## Safeguards that must survive
- `WN_*` bounds in weft_scene.h: 32 nodes, 32768-byte wire, 8 MiB
  objects, 16384 extent.
- The validator refuses a WN_VIEWPANE node unless it is a WN_WINDOW node
  that also carries WN_INPUT.
- Store: `safe_fd` owner and type checks, and lookups by O_NOFOLLOW
  relative to the held directory descriptor.
- `writer_lock`: flock.
- `wn_store_budget_edits`: limits are absolute ceilings, and a failure
  leaves both unchanged (weft_store.h comment, store-budget test :63-77).
- `wn_store_publish_if` compare-and-switch: a zero base requires an
  empty store.
- `sealed_file` and the `publish` commit order. The receipt keeps
  `switched` apart from `durable`, because a failed final sync can still
  have switched.
- `refs_check` and `generation_load` refuse missing or corrupt
  references.

## Acceptance (on the build host)
1. `make -C weft/lib` with the strict flags gives the actual compiler
   invocation and output hashes (STANDARDS 4).
2. `make -C weft/lib check` passes. Each of the following holds:
   - a seeded generation reopens with the same digest;
   - truncated, malformed and trailing scene bytes are refused with their
     named results;
   - every `wn_fault` stage gives the documented receipt (failed write,
     no partial generation visible);
   - a stale base gives WN_MOVED;
   - lock contention gives WN_BUSY;
   - a budget overflow gives WN_LIMIT with the limits unchanged.
3. Red control: a deliberately broken validator, built only in regress
   (one refusal removed by `-D`), makes the matching regress row FAIL.
4. weft_store.3 is clean under `mandoc -Tlint -W style -Ios=ELSEWHERE`
   and is found with `man -k`.

## Rollback
Revert the one commit. There is no earlier kit file.
