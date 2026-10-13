# W29 views-compare-cmd: weft-compare(1), confined

- **Proposed goal:** WEFT-5. **Class:** rewrite (port), adding
  confinement. **Tier:** c99.
- **Ceiling:** 3,100 lines in weft/views (adds about 550). The estimate:
  - compare_snapshots.c is 299 lines; with pledge and unveil, about 350;
  - regress is about 200 of sh, rewriting native_checks.py (230).

## Goal
Port the read-only comparison command: two UTF-8 files go to a new
directory holding the two sources, a PPM and a receipt.

## Inputs (frozen)
- weft/deps/compare/compare_snapshots.c, README.md, native_checks.py

## Outputs (kit)
weft/views/compare_snapshots.c weft/views/weft-compare.1
weft/views/Makefile weft/views/regress/Makefile
weft/views/regress/compare_cli.sh RETURN.md

## Deletions
- native_checks.py becomes compare_cli.sh. It is the only home of the
  CLI IO red controls, so every row moves before the Python is dropped.
- run_native.py (ssh to a named host) and ppm_preview.py are RETIRE.
- The output `receipt.json` is a format question. STANDARDS 8 and
  PRESERVATION (JSON receipts CHANGED) prefer the restricted header
  grammar. This card keeps the JSON output unchanged, because the format
  is a product output, not a house receipt. The reviewer decides; a
  change of format is its own card.

## Safeguards that must survive
These are at compare_snapshots.c lines in the frozen copy.
- Sources are opened O_NOFOLLOW and must be S_ISREG (:26-29).
- A source whose mtime or ctime changes during the read is refused
  (:40-44).
- The output directory must not exist. It is created 0700, and its files
  O_EXCL|O_NOFOLLOW 0600 (:125).
- fsync of the files and the directory (:136, :274).
- The SIGXFSZ policy (:253).
- Unlink cleanup on failure (:281).
- The font is exactly 4096 bytes.

## Added authority
- unveil the font and both sources `r`, and the output parent `rwc`;
  then `unveil(NULL,NULL)`; then `pledge("stdio rpath wpath cpath")`.

## Acceptance (on the build host)
1. Strict build.
2. Every row of the old native_checks.py, now in sh, passes:
   - symlink refusal;
   - an existing output is left untouched;
   - the font bound;
   - a forced 256-byte RLIMIT_FSIZE cleans up.
3. Red control: the existing-output row against a build without O_EXCL,
   built only in regress, FAILs.
4. Confinement control by the probe pattern.
5. The manual is lint clean and its EXAMPLES are run.

## Rollback
Revert. The Makefiles return to their W28 blobs.
