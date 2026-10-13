# W18 motion-producer: weft-room-motion, core and confinement

- **Proposed goal:** WEFT-3. **Class:** rewrite (port, split), adding
  confinement. **Tier:** c99: it reads the keyboard fd, the store and
  the font.
- **Ceiling:** 4,450 lines in weft/motion (adds about 1,900). The
  estimate:
  - the kept producer core of weft-room-motion.c is about 1,550 lines:
    the header, run struct, safeguards, receipt, writer, store hand-off,
    keyboard input and main loop, without the pty, help and views joins
    (W19, W31) and without touch, garden, heartbeat or physics;
  - wrm_parse and wrm_io are 127 lines;
  - pledge and unveil add about 40;
  - regress is about 170: room-motion-refusals/run.sh rewritten, plus a
    C nonblock-exec helper of about 30 lines, replacing nonblock_exec.py.

## Goal
Port the producer process as a device-free, confined program that turns
keyboard records into room edits and room scenes into a WNPOSE01 stream.

## Inputs (frozen)
- rm/weft-room-motion.c. Its sections are mapped in the survey notes:
  - keep: 1-180, 181-319 (part), 320-507, 508-700 (part), 701-834,
    1191-1252, 1253-1429, 2460-2794 (part), 2836-3294 (part) and
    3295-3317;
  - DEFER: touch (1543-2459), garden and heartbeat (1014-1190,
    1430-1507), and the physics tick (2795-2835).
- rm/wrm_parse.c, wrm_parse.h, wrm_io.c, wrm_io.h
- ctl/room-motion-refusals/run.sh, nonblock_exec.py
- ctl/fairness-lag/wrm_io_maptest.c: the GETMAP-only fixture
- weft/weft-room-motion.1

## Outputs (kit)
weft/motion/weft_room_motion.c weft/motion/wrm_parse.h
weft/motion/wrm_parse.c weft/motion/wrm_io.h weft/motion/wrm_io.c
weft/motion/weft-room-motion.1 weft/motion/Makefile
weft/motion/regress/Makefile weft/motion/regress/refusals.sh
weft/motion/regress/nbexec.c weft/motion/regress/maptest_io.c RETURN.md

## Deletions
- Touch (about 970 lines), garden and heartbeat (about 360), the physics
  tick (about 60), and `sess_state` and the heartbeat link (DEFER with
  the garden).
- These environment interfaces are removed, with their old and new
  contracts in the manual: `WEFT_GARDEN_*`, `WEFT_TOUCH_DEV`,
  `WEFT_SESSION_STATE`, `WEFT_HELP_ON_START` (if only the garden used
  it; the reviewer checks).
- These are kept: `WEFT_CLIENT_DIR` (W19), `WEFT_DOCSTORE` (W31) and
  `WEFT_KEYMAP`.
- The receipt fields that belong to deferred features leave the
  `WEFT-ROOM-MOTION1` line. Each field removed is listed in RETURN.md.
- nonblock_exec.py becomes nbexec.c. Python leaves the wired check.
- The garden requirements in refusals run.sh (:30, :37, :73-75) go.
- The mapio build no longer links touch, heartbeat or sess_state
  (launcher/Makefile:94-98).

## Safeguards that must survive
These are at weft-room-motion.c and wrm_parse.c lines in the frozen copy.
- Strict decimals, with geometry fitting WF_WIRE_MAX (wrm_parse.c:8-41).
- Exact argc, fd >= 3, in != out, and the total, frame and write bounds
  (:44-76).
- Refuses to run as root (:2866-2870).
- The fds must be nonblocking with the right access mode (:469-489).
- The total-deadline gate runs between phases (:386-411).
- Counter-wrap refusal (:413-426).
- Font exactly 4096 bytes (:428-467).
- Deadline-overflow refusals.
- Input batches capped at 64 events or 8 ms (:3112-3138).
- Ticks clamped to 1..1000 ms (:3214-3215).
- The store edit budget (:2933-2941).
- A keymap refusal names its line (:2958-2965).
- A heartbeat frame every 3 s within the 15 s consumer bound
  (:3245-3248).
- Stop fields are initialised before refusal, so a refused run prints
  instead of faulting (:2853-2860).

## Added authority and confinement
The order is: parse; refuse root; validate the fds; read the keyboard
map (WSKBDIO_GETMAP, before pledge, because its pledge class is
UNKNOWN); then confine:
- unveil the store `rwc`, the font `r`, the keymap `r`, `/dev/apm` `r`
  (status, W03) and the client directory `rwc`;
- unveil `/bin/ksh` and `/usr/bin/vi` `x` for W19;
- unveil the docstore for W31;
- `unveil(NULL,NULL)`;
- `pledge("stdio rpath wpath cpath flock tty proc exec")`.

The `tty proc exec` promises are needed only once W19 lands. Until
then, W18 pledges without them and W19 widens, naming the change. Each
promise and path is justified in the manual's SECURITY section
(STANDARDS 5).

## Acceptance (on the build host)
1. Strict build.
2. The refusal rows hold: a missing, wrong-type or unreadable store,
   font or client directory each gives a named `RESULT= STOP=` receipt,
   never a signal death.
3. A fixture run (maptest_io, injected keyboard records, a seeded store
   and the formula font) gives a WNPOSE01 stream that W16's decoder
   accepts.
4. The receipt line matches the manual.
5. Confinement control, by the W07 probe pattern: an open outside the
   veil is refused, and the same open succeeds with the envelope
   compiled out.
6. The 3 s heartbeat is observed under an injected clock.
7. The manual is lint clean and its EXAMPLES are run.

## Rollback
Revert. The Makefiles return to their W17 blobs.
