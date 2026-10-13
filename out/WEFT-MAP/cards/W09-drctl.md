# W09 drctl: the dr command

- **Proposed goal:** WEFT-2. **Class:** rewrite (port), adding
  confinement. **Tier:** c99.
- **Ceiling:** 2,050 lines in weft/dr (adds about 450). The estimate:
  - drctl.c is 207 lines, plus about 30 for pledge and unveil;
  - run-cli-checks.sh is 168 lines, and about 210 with the new
    confinement rows.

## Goal
Port `drctl` (init, commit, head, cat, info, view, view-set), confined.

## Inputs (frozen)
- weft/deps/dr/drctl.c, weft/deps/dr/tests/run-cli-checks.sh

## Outputs (kit)
weft/dr/drctl.c weft/dr/drctl.1 weft/dr/Makefile
weft/dr/regress/Makefile weft/dr/regress/cli.sh RETURN.md

## Deletions
- The `fault-commit` verb is a test hook in the product binary. It moves
  to a regress-only build (`-DDRCTL_FAULT`), so the installed command
  carries no fault surface. Old contract: the verb exists. New contract:
  it exists only in the regress build.
- The `./evidence` copying in run-cli-checks.sh goes. Evidence is the
  lane's, not the test's.

## Safeguards that must survive
- Bytes enter only by stdin and leave only by `cat`. Every other line is
  diagnostic.
- The exit codes are 0 ok, 1 invalid, 2 moved, 3 missing, 4 corrupt,
  5 bounds, 6 busy or denied, 7 io or limit.
- The ktrace proof shows no fork or exec (run-cli-checks.sh:145-153) and
  the init fsyncs (:156-161).

## Added authority
- `unveil(store, "rwc")` (`"r"` for read verbs), then `unveil(NULL,NULL)`,
  then `pledge("stdio rpath wpath cpath fattr flock")` for write verbs,
  or `"stdio rpath"` for read verbs.
- This happens after the argv check and before the store is opened. It
  is new, because the frozen drctl has neither (STANDARDS 5).

## Acceptance (on the build host)
1. All 27 frozen CLI rows hold.
2. A new row checks every documented exit status.
3. A new confinement control, using the W07 probe pattern. dr already
   refuses symlinks, so a symlink cannot isolate the veil. A
   regress-only `-DWEFT_PROBE` build opens a readable file outside the
   store after confinement:
   - the build without the envelope reads it;
   - the confined build is refused.

   Also, a write attempt under the read pledge aborts, and that abort is
   observed.
4. drctl.1 is lint clean and its EXAMPLES are run.

## Rollback
Revert. The Makefiles return to their W08 blobs.
