# W03 lib-status: the desktop status line

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99:
  it reads kernel state.
- **Ceiling:** 3,830 lines in weft/lib (adds about 480). The estimate:
  - weft_status.c and .h are 266 lines, times 1.15 gives about 306;
  - regress is about 150: test_status 138, plus a Makefile row.

## Goal
Port the status line that the producer and weft-view draw. It shows CPU,
battery, AC, memory and load, read through sysctl(2) and `/dev/apm`
(APM_IOC_GETPOWER, weft_status.c:134-140). A reading that cannot be
taken is shown as unknown. The first CPU sample is always unknown.
`wsys_read` is the only part that touches the system. `wsys_format` is
pure, and regress drives it with injected samples. Any program that
calls `wsys_read` must unveil `/dev/apm` read-only; W18 records this.

## Inputs (frozen)
- ns/status/weft_status.c, weft_status.h
- ctl/status/test_status.c, Makefile

## Outputs (kit)
weft/lib/weft_status.h weft/lib/weft_status.c weft/lib/Makefile
weft/lib/regress/Makefile weft/lib/regress/status_test.c RETURN.md

## Deletions
None. The frozen file is already minimal for its duty.

## Safeguards that must survive
- An invalid battery or AC reading is shown as unknown, never as
  healthy (STANDARDS 3: a health display never shows a stale or missing
  value as healthy).
- A tiny output buffer is refused.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Each injected `wsys_sample` (valid, invalid, absent, tiny buffer)
   gives the documented text through `wsys_format`. `wsys_read` is
   observed once, under ktrace, opening only `/dev/apm` read-only.
3. Red control: a reading forced to "healthy" when it is absent FAILs
   the row.

## Rollback
Revert. weft/lib/Makefile and regress/Makefile return to their W02 blobs.
