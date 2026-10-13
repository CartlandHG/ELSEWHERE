# W14 pty-runner: running ksh and vi in a pty, confined

- **Proposed goal:** WEFT-3. **Class:** rewrite (port), adding
  confinement. **Tier:** c99: it holds child processes.
- **Precondition:** D9 is acknowledged. This card ports weft_pty and adds
  child confinement. It does not merge it with sew_pty.
- **Ceiling:** 2,350 lines in weft/pty (adds about 1,250). The estimate:
  - weft_pty.c and .h are 462 lines; dropping the workbench worker gives
    about 440, and reflow about 510;
  - child confinement adds about 40;
  - regress is about 700: native-pty test_weft_pty 578 and test_wpt_wrap
    with its harness, de-duplicated.

## Goal
Port the pty runner: forkpty, drain on stop, bounded escalation, and
ECHILD treated as honest loss. Confine each pane child.

## Inputs (frozen)
- ns/terminal/weft_pty.c, weft_pty.h
- ctl/native-pty/test_weft_pty.c, test_wpt_wrap.c, fdcount.c, Makefile
- sew/pty/src/sew_pty.c, read only: its per-kind veil and execpromises
  pattern, which is the model for the added confinement

## Outputs (kit)
weft/pty/weft_pty.h weft/pty/weft_pty.c weft/pty/Makefile
weft/pty/regress/Makefile weft/pty/regress/pty_test.c
weft/pty/regress/wrap_test.c weft/pty/regress/fdcount.c RETURN.md

## Deletions
- `WPT_WORKBENCH`, the Python workbench worker run from
  `WEFT_WORKBENCH_WORKER`, goes with the garden. Old contract: three
  program kinds. New contract: ksh and vi.
- The `-Wl,--wrap` seam in ctl/native-pty/Makefile becomes a `-D`
  rename of fcntl, waitpid and clock_gettime on the one object, as
  base-driver already does.

## Safeguards that must survive
- ECHILD, or a matching reap, ends authority (weft_pty.c:34, :154, :381,
  :408).
- HUP, then TERM, then KILL escalation with a finite grace (:200).
- Drain never spins on EINTR or monopolises teardown.
- An fcntl failure resolves custody.
- A clock failure stops escalation (test_wpt_wrap.c:5-14).
- vi gets an argv with no shell string.
- Inherited descriptors are closed.

## Added authority and confinement
The child pledges after fork and before exec, using execpromises:
- for ksh: `"stdio rpath wpath cpath tty proc exec"`;
- for vi: `"stdio rpath wpath cpath tty"`;
- unveil limits it to the pane's private work directory, plus `/bin`,
  `/usr/bin` and `/usr/share/misc` read and execute.

These exact sets are a proposal. The reviewer accepts or narrows them,
and they are UNKNOWN until observed.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Drain-on-stop, ECHILD honesty and flat fd hygiene (fdcount) hold.
3. Every injected fcntl, waitpid or clock failure row holds.
4. Confinement control: a pane child tries to read a file outside its
   veil and to open a socket. Both succeed in an unconfined regress
   build, and both are refused in the product build.

## Rollback
Revert. The Makefiles return to their W13 blobs.
