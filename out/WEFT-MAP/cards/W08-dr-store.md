# W08 dr-store: exact document revisions

- **Proposed goal:** WEFT-2. **Class:** rewrite (port). **Tier:** c99:
  a persistent format with locks and untrusted reads.
- **Ceiling:** 1,600 lines in weft/dr. The estimate:
  - dr.c 155, dr.h 115 and dr_openbsd.c 557 make 827; times 1.1 gives
    about 910 (the frozen dr code is already close to style(9));
  - regress C is about 670: test_revisions, test_m01_seam and the
    boundary control.

## Goal
Port the dr store: content objects, 121-byte DRREV001 revision records,
compare-and-switch heads and labelled views.

## Inputs (frozen)
- weft/deps/dr/dr.c, dr.h, dr_openbsd.c, Makefile, README.md
- weft/deps/dr/tests/test_revisions.c, test_m01_seam.c,
  VIOLETTE-SAVED-REVISION-BOUNDARIES-20260913-r1.c, run-m01-seam.sh,
  run-boundaries.sh
- weft/deps/dr/vi-experiment/README.md: its conclusion only, for CAVEATS

## Outputs (kit)
weft/dr/Makefile weft/dr/dr.h weft/dr/dr.c weft/dr/dr_openbsd.c
weft/dr/dr.3 weft/dr/regress/Makefile weft/dr/regress/revisions_test.c
weft/dr/regress/seam_test.c weft/dr/regress/boundary_test.c RETURN.md

## Deletions
- README.md and NEXT.md are not carried. Their contract goes to dr.3,
  and their history stays in frozen src.
- vi-experiment/ (RETIRE: research). Its conclusion goes to dr.3
  CAVEATS: nvi `:w` writes in place, not atomically.
- The `-Wl,--no-execute-only` UB build. Whether a UB trap build stays is
  for the reviewer; if it stays, it is a named regress target.

## Safeguards that must survive
These are at dr_openbsd.c lines in the frozen copy.
- CAS on the expected base gives DR_MOVED and reports the newer head;
  it never overwrites (:408-411).
- A broken chain is never extended: the parent record must resolve and
  match the docid, and a sequence at UINT64_MAX gives DR_LIMIT
  (:413-420).
- Single writer: `flock(LOCK_EX|LOCK_NB)` gives DR_BUSY (:113).
- `safe_fd` checks owner and mode (:41-44).
- Every lookup is O_NOFOLLOW relative to a held directory descriptor.
- Pointer commit: tmp, fchmod 0400, fsync, renameat, fsync of the
  directory (:218-227).
- Record and object commit: linkat, then fsync, then re-verify, so a
  corrupt dedup target is DR_CORRUPT (:270-290, :364-370).
- Durable create fsyncs the parent and the root (:137-147, :163).
- Corruption refusals at :87, :190-195, :245-249, :319-338 and :516.
- Outputs are zeroed on every failure, and `dr_resolve` clears them at
  entry (:477).
- dr.c admits the docid byte by byte, so an embedded NUL is corruption.

## Acceptance (on the build host)
STANDARDS 3 requires a persistent format to prove reopen, malformed and
truncated input, failed writes and recovery. Each of these is a regress
row:
- reopen after commit;
- a truncated record;
- a malformed pointer;
- a changed object;
- an injected fsync failure through the `-Dfsync=` seam, recorded as a
  test-only seam;
- recovery leaving the head unchanged.

The red control is the boundary control: a deliberately weakened parse,
built only in regress, FAILs. The fd count is flat across the journey.
dr.3 is lint clean.

## Rollback
Revert the one commit.
