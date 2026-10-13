# W32 weft-overview: weft.7 and the one top-level check

- **Proposed goal:** WEFT-6. **Class:** rewrite. **Tier:** base: a
  Makefile and a manual.
- **Ceiling:** 60 lines in weft/ (the Makefile). The manual is not
  ceilinged. The estimate: SUBDIR over 11 components with `all` and
  `check`, about 40 lines; the frozen weft/Makefile is 13 lines plus
  tests/run.sh at 54.

## Goal
One `make` and one `make check` at weft/, and a nature page that names
only what the kit contains.

## Inputs (frozen)
- weft/weft.7, weft/Makefile, weft/README, weft/tests/run.sh,
  ns/Makefile

## Outputs (kit)
weft/Makefile weft/weft.7 RETURN.md

## Deletions
- tests/run.sh's garden leg ran *outside* MALLOC_OPTIONS=S, and its
  weft3d leg ran Python. Both are deferred with the garden. Every
  remaining leg runs under S. The ruled-pending exemption ends because
  its subject is not in the kit.
- weft.7's descriptions of sew_prompt, sew_pty, mirror_pane and span go.
  The kit page names its own components, and SEE ALSO points to the
  other components' pages once they exist.
- README and README.md are not carried; weft.7 replaces them.

## Safeguards that must survive
- The "seams" section's rules: a refusal frame, never a stale scene; no
  policy in the raster; the launcher supervises and components do not
  supervise each other.
- "Say what is unknown."

## Acceptance (on the build host)
1. `make` and `make check` at weft/ run every component's regress under
   MALLOC_OPTIONS=S.
2. A component whose check executes nothing makes the aggregate FAIL.
3. Red control: an empty regress target planted in one component is
   caught.
4. weft.7 is lint clean and found with `man -k weft`.

## Rollback
Revert the one commit.
