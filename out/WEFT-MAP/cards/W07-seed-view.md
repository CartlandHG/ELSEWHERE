# W07 seed-view: seed-session(1), weft-view(1) and the font

- **Proposed goal:** WEFT-1. This card is its finish line: on the build
  host, a seeded store renders to a PPM. **Class:** rewrite (port).
  **Tier:** c99.
- **Precondition:** D4. The passive-asset rule returns through STANDARDS
  11 before `font.816` enters the kit.
- **Ceiling:** 300 lines in weft/seed and 300 in weft/view. The estimate:
  - seed_session.c is 196 lines; dropping the garden style branch
    (:45, :161-180) and reflowing gives about 200;
  - weft_view.c is 118 lines, about 150 after reflow;
  - the Makefiles are about 20 each;
  - regress: seed-session/run.sh 64, weft-view/run.sh 50 and a new
    confinement control of about 40.

## Goal
Port the two device-free programs that prove the data plane end to end,
with their sandbox envelopes and their manuals.

## Inputs (frozen)
- ns/seed_session.c, ns/seed_session.1, ns/weft_view.c, weft/weft-view.1
- weft/font.816, font.816.sidecar.md. worker/reader/font.816 is a
  byte-identical duplicate (sha256 954ce847...) and is not carried.
- ctl/seed-session/run.sh, ctl/weft-view/run.sh

## Outputs (kit)
weft/seed/Makefile weft/seed/seed_session.c weft/seed/seed-session.1
weft/seed/regress/run.sh weft/view/Makefile weft/view/weft_view.c
weft/view/weft-view.1 weft/view/font.816 weft/view/font.816.sidecar.md
weft/view/regress/run.sh RETURN.md

## Deletions
- The `WEFT_SESSION_STYLE=garden` branch and its scene. The garden
  waits. `WEFT_SESSION_STYLE` is then an option with one value, so it is
  removed from the environment interface. Old contract: "garden" or
  empty. New contract: no variable. A set value is ignored and recorded
  in the manual CAVEATS. Alternatively the owner keeps the refusal of
  unknown values; the reviewer decides.
- The "warren-host" plane name in weft_view's refusal frame becomes a
  site-neutral word (STANDARDS: no site names in code).
- The manuals are trimmed to the standard's sections. The weft-view.1
  narrative (399 lines) keeps every caller contract.

## Safeguards that must survive
- weft_view.c:38-55: unveil the store `r`, the font `r` and the output
  `rwc`, then `unveil(NULL,NULL)`, then
  `pledge("stdio rpath wpath cpath")`, all before the first store byte.
  Any failure is a named REFUSAL.
- seed_session.c:106-118: unveil the parent and store `rwc` and the
  keymap `r`, then `pledge("stdio rpath wpath cpath fattr flock")`.
- Both refuse to run as root.
- The font must be exactly 4096 bytes, a regular file and O_NOFOLLOW.
- The output is opened O_EXCL|O_NOFOLLOW with mode 0600.
- An unreadable store gives a refusal frame, never the last good scene.
- The exit status is as the manual says: the image exists even on
  refusal.

## Acceptance (on the build host)
1. Strict build.
2. `seed-session $T/store` then `weft-view $T/store font.816 $T/out.ppm`
   gives `RESULT=OK` and P6 output.
3. Red control (envelope arming): a path that cannot be unveiled refuses
   with "sandbox envelope failed to arm" and leaves no output.
4. A **new** confinement control, which STANDARDS 5 requires because an
   errno alone is not evidence. A symlink would not do here: the store
   already refuses symlinks through O_NOFOLLOW, so it cannot isolate the
   veil. Instead a regress-only probe build (`-DWEFT_PROBE`) opens a
   readable file outside the veil right after the envelope arms:
   - built with the envelope compiled out, the probe reads the file;
   - with the envelope, the open is refused.

   Both observations are recorded, for weft-view and for seed-session.
5. Seeded output is readable by weft-view, which shows the veil is not
   too narrow.
6. Both manuals are lint clean and found with `man -k`. Their EXAMPLES
   are run and compared against output and status.
7. The font's sha256 equals its sidecar.

## Rollback
Revert. No installed artifact exists.
