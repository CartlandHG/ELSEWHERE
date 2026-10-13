# RETURN WEFT-MAP

Card WEFT-MAP: a port plan for weft. It writes no code. Standard: edition
`2026-10-13.v3`. Inputs were checked before use:
- frozen-src-75ad5ce4.tgz, sha256 `8cf5a408d1fc34f1204a7a70414ca7b389e33b7de677bf15941a04b339708480`;
- kit-2d6c0739.tgz, sha256 `5fccc65f23a7cbe7b1f2ee39357572d592fff4fa6275cb8f90c94304c3254cdd`.

Both matched the card's digests. Nothing here is PASS: no native
observation exists.

## 1. Deletions

This return deletes nothing. No kit or frozen file was changed. The plan
**proposes** these deletions. Each is a verdict in INVENTORY.tsv with a
reason, and each is carried out only by the card that names it.

- **RETIRE (59 files, 9,599 lines):**
  - drifted duplicate copies of the raster, scene and terminal header
    (deps/compare/inputs/raster, 5 files);
  - research traces (deps/dr/vi-experiment, 5 files);
  - evidence and packaging notes (10);
  - an ssh runner, a preview aid and a runner for absent fixtures (3);
  - one empty stray file (launcher/launcher/test_early_cancel.c);
  - 35 origin-copy files that no static import reaches, or that are
    foreign toolchains (Rust/Cargo/Zig), correspondence or generated
    output.

  Reasons are in PLAN.md section 4. None of them is load-bearing. The one
  conclusion of the research (nvi writes in place) moves to dr.3
  CAVEATS (W08).
- **Deletions inside PORT files, by card:**
  - W17 and W18: physics posing (`wrb_*`, engine/*); touch (about 970
    lines of weft-room-motion.c); garden and heartbeat (about 360).
  - W07: the garden session style (seed_session.c :45, :161-180).
  - W14: the Python workbench pty kind.
  - W25: the `vendor/` provenance-check rule, which points at a missing
    file.
  - W09: the drctl `fault-commit` verb moves to a regress-only build.
  - Every `-Wl,--wrap` seam (W14, W24, W26) becomes a `-D` seam.
  - W12: the test clock.
  - W18 and W19: Python in the wired checks (nonblock_exec.py,
    test_producer.py, test_actions_producer.py); W29: native_checks.py.
  - W23: receipt_abi.c's print-only test becomes an asserting one.
  - W26b: the latency measures leave regress (D6).

  Each card states the old and new contract for an interface it removes.
- **DEFER (286 files, 83,511 lines, 72,231 of them Python):** the
  garden, touch, window physics, the operator/install script and the
  garden-only controls. They stay in frozen src for later goals.

## 2. Frozen files and lines started from

All 603 weft files in FROZEN-SRC-MANIFEST: 137,880 lines by wc -l. That
is 39,892 C, 7,992 H, 501 INC and 77,102 Python, with the rest docs,
make, sh, JSON and assets. INVENTORY.tsv has one row for each file. The
count was checked: 603 data rows, the path set equal to the manifest's
weft paths, no duplicates, and 6 fields on every row.

Also read: kit policy/PRINCIPLES.md, STANDARDS.md, SITE.md, README.md,
PRESERVATION.md, plans/SPINE.md and man/jobcard.5; and frozen
mk/house.mk, lib/glyph/README, lib/mirror/README, sew/pty/README and the
lib/glyph/weft_scene.h diff.

**What was read, and what was not.** Not every line was read, and no
such claim is made. The survey was split four ways: room-motion,
native-other, controls, and deps/worker/top. Four subagents did the
per-file reading. I set the rubric and component map, checked their
counts, spot-read the key files myself, and changed five verdicts.

- **Read in full, or in the parts named:**
  - all weft Makefiles; README and README.md; weft.7;
  - the NAME and DESCRIPTION sections of the five section-1 pages;
  - tests/run.sh;
  - weft_view.c:1-80; weft_store.h; wp_supervisor_openbsd.c;
    wp_atomic_openbsd.h;
  - base-driver README.md and PROVENANCE.md;
  - launcher Makefile, README.md and run.sh;
  - wrm_parse.c and wrm_io.c;
  - weft-room-motion.c's struct, its safeguard helpers (:386-507), its
    writer, its hand-off and its main loop;
  - weft_motion_frame.h; most of weft_room_bridge;
  - the core of weft_motion_room.c;
  - every control Makefile and run.sh;
  - dr_commit (dr_openbsd.c:387-462);
  - deps READMEs; garden_desktop_entry.py's docstring and pin table.
- **Skimmed (heads, comments, function lists, grep):** every other C and
  H file, including the large test bodies (test_session.c 620,
  test_weft_pty.c 578, test_room.c 374, ram_launcher.c 484).
- **Classified without reading bodies:** the 214 origin-copy files
  (68.5k lines of Python). They were classified by module docstrings and
  a static import closure; dynamic imports may be under-counted.
- **Read only for licence and includes:** drm.h and drm_mode.h.

**Verdicts I changed from the survey.**
- `weft_motion_room.c`, `weft_room_motion_handoff.c` and
  `weft_room_motion_handoff.h` went from DEFER to PORT. Their frame
  custody and gates are needed by the producer without physics (W17).
- `controls/root-join/test_idle_join.c` and `test_responsiveness.c` went
  from MERGE to DEFER. They are wall-clock latency measures (D6).

**Coverage check:** every PORT and MERGE file is named in at least one
card's inputs, checked by basename or directory glob.

## 3. STANDARDS sections

The rules apply in two ways: to this return, which is a plan, and to the
ports the plan proposes. Plan locations are PLAN.md line numbers.

| Section | Applies to this return | How it is met | Debt |
|---|---|---|---|
| 1 Foundation | Yes | No code or tool enters the kit. The tiers are applied per card (`Tier:` line in each card). Python is planned out of weft entirely (PLAN.md:173-198). Foreign material is named: the drm headers (D3, PLAN.md:210), `__atomic` (D5, PLAN.md:223) and `-Wl,--wrap` (PLAN.md:136-148). | D3 and D5 are open (entries below). |
| 2 Work and seams | Yes | Every card names its class, tier, ceiling with the estimate behind it, outputs, deletions and rollback (cards/README.md). Component ceilings and running totals are in PLAN.md:250-318. | **Not met:** no plan goal covers weft or this card. Entry D-1 below. |
| 3 Evidence | Yes | Nothing is PASS. Every card's acceptance is native and observable, and names a standing red control. The new controls are named where the frozen tree has none or weak ones: directional (W11), robust-base (W26b), receipt_abi (W23) and confinement (W07, W09, W14, W18, W26, W29). | REVIEW-only: the survey judgements in INVENTORY.tsv are reading, not a checker. Entry D-2. |
| 4 C | No code in this return | For the ports: style(9) and strict flags in one Makefile.inc (W01), plus the ownership and failure contracts carried in the safeguard lists. | None here |
| 5 Authority | Yes, for the ports | The missing pledge and unveil are named for the producer, launcher, display child, pty children, drctl and weft-compare, and added in W09, W14, W18, W19, W23, W26 and W29. Restriction controls use a probe that shows the operation was otherwise possible (W07). Root work is not carded (D7). | The drm pledge class is UNKNOWN (W23 and W26 record it). Entry D-3. |
| 6 Shell and checks | Yes, for the ports | Clock-dependent checks become injected seams or leave regress (D6). There is no network. Fixtures are in-tree, so the absent `../../M02` and `evidence/` are dropped. | None here |
| 7 Manuals | Yes, for the ports | Each user-facing card carries its page: weft_store.3, weft_raster.3, weft-keymap.5, seed-session.1, weft-view.1, dr.3, drctl.1, weft_room.3, weft-frame.5, weft-room-motion.1, wp_supervise.3, weft-native-session.8, svs.3, weft-compare.1 and weft.7. Lint and `man -k` are in the acceptance. | None here |
| 8 Custody | Yes | SHA256SUMS is written last and does not list itself. Duplicate identities are found and retired: the font copy (identical sha256) and the scene and raster copies. | None |
| 9 Review | Yes | Each card needs a reviewer who is not its author (cards/README.md). This return also needs one. | Pending |
| 10 Adding and removing | Yes | Every removed safeguard, interface or option is named per card with its old and new contract (W07, W09, W14, W17, W18). Size claims are labelled estimates, to be measured on the returned bytes (PLAN.md:304-312). | None |
| 11 Changing the standard | Yes, as prerequisites | Rules that return before a port: passive assets (D4, before W07); retained foreign material (D3, before W20). The plan does not change the standard. | D3 and D4 entries |

**Debt entries** (rule; location; owner; remedy; review trigger):

- **D-1.** STANDARDS 2 (card cites a plan goal); this card and all of
  cards/; owner Hugh; add WEFT-1 to WEFT-6 to the plan, or decline them
  (PLAN D1); review on the dispatch of any W card.
- **D-2.** STANDARDS 3 (REVIEW is never PASS); INVENTORY.tsv verdicts;
  owner the integrator; the reviewer of each W card re-checks its rows
  against the bytes it ports; review on each W card's admission.
- **D-3.** STANDARDS 5 (an enforcement gap is named); the display child
  and launcher `drm` promise; owner the integrator; observe it on the
  build host in W23 and W26; review on W23.
- **D-4.** STANDARDS 1 (dialect); `wp_atomic_openbsd.h:27-46`; owner
  Hugh (D5); exception, stdatomic or redesign; review on W21.
- **D-5.** STANDARDS 11 (passive asset and retained foreign material);
  font.816 and kms/abi/*.h; owner Hugh (D3, D4); bring the rules back
  through section 11; review on W07 and W20.

## 4. Authorities, interfaces, options, state and dependencies added

**This return adds none to the kit.** The plan proposes the following,
each with its reason in the card:

- **Plan goals:** WEFT-1 to WEFT-6. Without them, no card can be
  dispatched.
- **Manuals as new interfaces:**
  - weft-keymap.5 (W04), from CONTROL-SCHEMA.md;
  - weft-frame.5 (W16): the WNPOSE01 wire, already a de-facto interface;
  - weft-native-session.8 (W26): renamed from weft-launch.1 to match the
    binary.
- **Header weft/lib/weft_cells.h (W02):** this moves the cell types so
  that lib does not include from pty.
- **Confinement:** pledge and unveil in drctl, the pty children, the
  producer, the launcher, the display child and weft-compare. These are
  authority reductions, but each promise is new contract text.
- **Regress-only builds:** `-DWEFT_PROBE`, `-DDRCTL_FAULT` and the
  `-D` link seams.
- **Renames proposed for review:**
  - `sewwm_*` to `room_*` (W10), because the name describes an X window
    manager the kit lacks;
  - `wrm_clients` to `weft_clients` (W15);
  - `wp_paced_body.inc` to `wp_paced.c` (W22).

  The reviewer may keep the frozen names.
- **Removed interfaces:**
  - `WEFT_SESSION_STYLE` (W07), with its contract change noted;
  - `WEFT_GARDEN_*`, `WEFT_TOUCH_DEV`, `WEFT_SESSION_STATE` and possibly
    `WEFT_HELP_ON_START` (W18);
  - the drctl `fault-commit` verb leaves the product (W09);
  - the WNPOSE01 physics fields, which are kept and carry
    0 and the source digest (W17).

## 5. What stays UNKNOWN until the native runs

- Whether any port builds clean under `-std=c99 -Wpedantic -Werror` with
  the frozen logic reflowed. The frozen dr and many control Makefiles
  lacked `-Wpedantic`.
- Every line estimate and ceiling. The 1.15 reflow factor is an
  assumption.
- Whether OpenBSD's `drm` pledge admits SETCRTC, CREATE_DUMB and ADDFB
  for the display child. Whether WSKBDIO_GETMAP works under the
  producer's pledge.
- Whether base or xbase on the build host installs `drm_mode.h`, and
  whether the trimmed ABI header matches it.
- Whether the proposed pty-child execpromises let ksh and vi work.
- Whether each existing control fails when its safeguard is removed. The
  frozen "yes" red controls are mostly negative-path assertions, not
  demonstrated mutants.
- Whether static posing (D2) changes anything a consumer relies on,
  beyond the deferred controls found by grep.
- Whether any origin-copy file retired by static import closure is
  reached dynamically.
- The frozen evidence claims cited in notes, such as 185 driver
  assertions, 12 and 14 custody cases and 27 CLI rows. They are counted
  from the frozen sources and READMEs, not observed.

## 6. wc -l of every file

Counted with `wc -l` on the sealed bytes. SHA256SUMS is written after
this table and is not listed here.

| Lines | File |
|---:|---|
| 604 | INVENTORY.tsv |
| 318 | PLAN.md |
| 241 | RETURN.md |
| 35 | cards/README.md |
| 78 | cards/W01-lib-core.md |
| 62 | cards/W02-lib-raster.md |
| 44 | cards/W03-lib-status.md |
| 53 | cards/W04-keys-map.md |
| 48 | cards/W05-keys-input.md |
| 51 | cards/W06-keys-help.md |
| 80 | cards/W07-seed-view.md |
| 72 | cards/W08-dr-store.md |
| 58 | cards/W09-drctl.md |
| 53 | cards/W10-room-reduce.md |
| 54 | cards/W11-room-panes.md |
| 58 | cards/W12-room-model.md |
| 43 | cards/W13-pty-terminal.md |
| 66 | cards/W14-pty-runner.md |
| 43 | cards/W15-pty-clients.md |
| 60 | cards/W16-motion-wire.md |
| 79 | cards/W17-motion-custody.md |
| 109 | cards/W18-motion-producer.md |
| 63 | cards/W19-motion-joins.md |
| 51 | cards/W20-driver-kms.md |
| 65 | cards/W21-driver-supervisor.md |
| 50 | cards/W22-driver-frame.md |
| 65 | cards/W23-driver-adapter.md |
| 40 | cards/W24-launch-reap.md |
| 53 | cards/W25-launch-presenter.md |
| 81 | cards/W26-launch-core.md |
| 76 | cards/W26b-launch-join.md |
| 45 | cards/W27-views-svs.md |
| 49 | cards/W28-views-compare-lib.md |
| 60 | cards/W29-views-compare-cmd.md |
| 45 | cards/W30-views-pane.md |
| 53 | cards/W31-views-join.md |
| 46 | cards/W32-weft-overview.md |
