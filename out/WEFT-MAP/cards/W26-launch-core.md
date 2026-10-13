# W26 launch-core: weft-native-session, confined

- **Proposed goal:** WEFT-4. **Class:** rewrite (port), adding
  confinement. **Tier:** c99: it holds privilege-adjacent devices and
  children.
- **Ceiling:** 3,110 lines in weft/launch (adds about 1,550). The
  estimate:
  - weft-native-session-core.c and the main are 528 lines; reflowed with
    pledge and unveil, about 610;
  - regress is about 850: test_session slimmed from 620 by dropping the
    garden-linked producer, test_early_cancel 67, and the two stub
    presenters 248. The joined suites (root-join, robust-base) are W26b.

## Goal
Port the launcher. It validates 14 operands, opens the keyboard and a
nonblocking pipe, forks and execs the producer before any display
acquisition, presents in-process, and cancels and reaps on every path.

## Inputs (frozen)
- ns/launcher/weft-native-session-core.c, weft-native-session.c,
  test_session.c, test_early_cancel.c, wmp_stub_count.c,
  wmp_stub_fail1.c, Makefile, run.sh, README.md
- ctl/fairness-lag/vendor/plant_room_store.c: compare it with
  seed-session first; it is dropped if it is redundant
- weft/weft-launch.1

## Outputs (kit)
weft/launch/weft_native_session.c weft/launch/weft-native-session.8
weft/launch/Makefile weft/launch/regress/Makefile
weft/launch/regress/session_test.c weft/launch/regress/cancel_test.c
weft/launch/regress/stub_count.c weft/launch/regress/stub_fail1.c
RETURN.md

## Deletions
- `-Wl,--wrap=fork,weft_present_motion_fd,kill` in the earlycancel
  build becomes a `-D` seam.
- run.sh (`uname`, `cc --version` and `evidence/` writing) goes. Build
  evidence is the lane's (STANDARDS 4).
- The manual moves to section 8 as `weft-native-session.8`. The program
  is a session launcher, and the frozen page's name, weft-launch, does
  not match the binary.

## Safeguards that must survive
- Refuses to run as root (geteuid 0).
- The producer is forked and exec'd before any DRM descriptor exists
  (core.c:14-25, :376).
- The receipt log is opened O_EXCL|O_NOFOLLOW (:339).
- Managed signals are blocked across fork (:364-368).
- The producer is cancelled and reaped on every return path.
- The launcher never stops xenodm and never opens DRM itself; the
  adapter does that.

## Added authority and confinement
After the operands are validated, the log and keyboard are opened and
the pipe is made:
- unveil the producer `x`, the store, font and keymap `r`, and
  `/dev/dri/card0` `rw`;
- `unveil(NULL,NULL)`;
- `pledge("stdio rpath wpath proc exec drm")`.

The producer gets its own envelope (W18) through execve. The launcher
delivers a fixed environment at exec and removes MALLOC_OPTIONS and
similar weakening options (STANDARDS 5). The exact promise set is
UNKNOWN until observed on the build host (whether `drm` admits the
master ioctls is shared with W23).

## Acceptance (on the build host)
1. Strict build.
2. Each test_session row passes without a device: card0 is refused by
   fixture, and the case is refused when the test principal can access
   card0.
3. Early cancel before exec is refused deterministically.
4. Flat fd counts across the session rows.
5. Red control: a launcher that opens card0 before fork, built only in
   regress, FAILs.
6. Confinement control by the probe pattern.
7. The manual is lint clean and its EXAMPLES (preflight) are run.

## Rollback
Revert. Nothing is installed: installation is WEFT-INSTALL, under D7
with owner authority.
