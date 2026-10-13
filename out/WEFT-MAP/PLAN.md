# WEFT-MAP: a port plan for weft

Card WEFT-MAP, 2026-10-13. Standard: edition `2026-10-13.v3`. Frozen
input: src.git `75ad5ce4` (frozen-src-75ad5ce4.tgz, sha256 `8cf5a408...0480`).
Kit: `2d6c0739` (kit-2d6c0739.tgz, sha256 `5fccc65f...4cdd`). This is a
plan. It holds no code, and every claim in it about behaviour is UNKNOWN
until it is built and tested on the build host.

How it was made: wc, grep and reading of the READMEs, manuals, Makefiles,
headers, entry points and main paths. The large bodies were skimmed, not
read. RETURN.md, section 2, says what was read and what was not.
INVENTORY.tsv gives one verdict for each of the 603 weft files.

## 1. What weft does for the house

weft is the house's own desktop: one person, one screen, no X. In plain
words, it has five duties.

1. **Keep a scene.** A *store* is a directory of immutable,
   content-addressed generations. A *scene* is a bounded set of panes
   (rectangles, colours, roles, text). Publication is append-only and
   compare-and-switch: a stale writer is refused, never merged.
   (`weft_store_openbsd.c`, `weft_scene.c`)
2. **Draw it without a display.** A software rasteriser paints a scene
   through one 8x16 bitmap font (`font.816`) into pixels. When the store
   cannot be read, it paints a *refusal frame* that names the reason; it
   never shows the last good scene. `weft-view` writes this to a PPM and
   opens no device. (`weft_raster.c`, `weft_view.c`)
3. **Turn keys into actions.** A keyboard profile (wscons decode, a
   keymap file, named commands, a help strip and a menu) feeds a reducer
   that edits the scene by compare-and-switch. Panes are focused, moved,
   closed and opened. Terminal panes run `ksh` or `vi` in a pty.
   (`profile/`, `component/`, `terminal/`, `wrm_clients.c`)
4. **Put it on the glass.** A producer (`weft-room-motion`) composes
   frames and writes a bounded binary stream (WNPOSE01). The launcher
   (`weft-native-session`) forks the producer before it opens any device.
   It presents the stream through a DRM dumb buffer in a supervised
   display child, and it reaps every child on every path. (`launcher/`,
   `base-driver/`)
5. **Keep exact document versions.** `dr` stores exact byte revisions
   with parent links, compare-and-switch heads and named views. `svs` and
   `weft-compare` show pinned or following views and diffs of them.
   (`deps/dr`, `deps/svs`, `deps/compare`, `weft_pane.c`)

The rest is the **garden**: Molly's Garden, a Python 3D world (weft3d,
molly, about 72k lines of Python) drawn into a pane by a worker process.
Most of the other files are evidence notes, controls and research.

The safeguards that make it safe to simplify, and that every port keeps:
- compare-and-switch publication;
- refusal frames instead of stale scenes;
- exclusive-create output with O_NOFOLLOW;
- an exact font size;
- bounded counts, sizes, deadlines and ticks;
- fork before device;
- a bounded reap with ECHILD treated as loss of authority, and no kill
  after that loss;
- signals blocked across fork;
- parent-only abandonment when a child cannot be reaped;
- dr's linkat/renameat commit points with fsync and re-verification;
- zeroed outputs on every failed read.

The cards name each one by file and line.

## 2. Scope against the plan in force

**No goal in plans/SPINE.md covers weft.** SPINE puts "the garden" under
What waits, and says new goals return only once SPINE-5 stands.
STANDARDS section 2 requires each card to cite a plan goal, so **none of
the cards in cards/ can be dispatched as written.** Each one names a
*proposed* goal. The owner first adds these goals to the plan, or
declines them.

| Proposed goal | Finish line (observable on the build host) | Cards |
|---|---|---|
| WEFT-1 | Headless room: lib, keys, seed and view build. `seed-session` then `weft-view` writes a PPM, and the envelope controls go red when broken. | W01-W07 |
| WEFT-2 | dr and drctl keep exact revisions. Their reopen, truncation, corruption and failed-write controls run. | W08-W09 |
| WEFT-3 | The room, pty and producer run device-free against fixtures. The refusal rows and the producer's frame stream decode. | W10-W19 |
| WEFT-4 | Driver and launcher regress pass device-free: fixtures substitute every device call. | W20-W26b |
| WEFT-5 | Saved views and weft-compare. | W27-W31 |
| WEFT-6 | Overview page and the one top-level `make check`. | W32 |
| (later) WEFT-INSTALL | The root change list (device ownership, the xenodm hand-off, the installed launcher), with the owner's recorded authority. | none: not carded here |

## 3. The minimal kit shape

Everything goes under kit `weft/`. Each directory is one component with
its own Makefile, `regress/` and line ceiling. Arrows read "uses".

```
weft/lib    scene, store, action, raster, cell grid header, status   (none)
weft/keys   keymap, key/chord, wskbd decode, text, house input,
            help, menu, catalogue                                    -> lib
weft/seed   seed-session(1)                                          -> lib, keys
weft/view   weft-view(1), font.816 + sidecar                         -> lib
weft/dr     dr(3) store, drctl(1)                                    (none)
weft/room   reducer, focus, home session, panes, layout, direction,
            room, receipt                                            -> lib, keys
weft/pty    terminal cells, pty runner, client registry              -> lib
weft/motion wire frames, writer, stream, codec, frame custody,
            handoff, weft-room-motion(1)                             -> lib, keys, room, pty
weft/driver KMS fixed records, frame copy, paced adapter, supervisor (none; one DRM ABI header)
weft/launch reaper, presenter, report, weft-native-session(8)        -> driver, motion (exec only)
weft/views  svs, doc diff/view, snapshot compare, weft-compare(1),
            pane, room views                                         -> lib, dr, (motion join)
weft/weft.7 + weft/Makefile                                          (all)
```

**Interfaces kept, unchanged in meaning:**
- the store directory format and `wn_store_*`;
- the scene wire (`WN_*` limits);
- the 4096-byte font profile;
- the WNPOSE01 frame stream (224-byte header);
- the producer and launcher argv (14 operands);
- the `WEFT-ROOM-MOTION1` and `WEFT-NATIVE-SESSION1` receipt lines;
- the dr store layout (DRREV001);
- drctl's exit codes;
- weft-compare's argv and outputs.

Removing a field from an interface is itself a change. When a deferred
feature leaves a receipt field (touch, garden or heartbeat), the card for
that step states the old and new contracts.

**Dependencies outside weft.** weft does not link sew, mirror, marble or
the frozen lib. The ties are these:
- **lib/glyph** carries a drifted copy of `weft_scene.h` (diff: the
  attribution comment only; sha `bb8e09ed...` against `e9e392d8...`). The
  marble legs link lib/glyph. When lib/glyph is ported, it must include
  `weft/lib/weft_scene.h` and not keep a copy (D8).
- **sew/pty** (`sew_pty.c`, 983 lines) is a second pty runner. It confines
  its children: a per-kind veil, a network block and a private workdir.
  weft's `weft_pty.c` runs `ksh` and `vi` without that (D9).
- **mirror/sew** are named in the frozen weft.7 overview only. The kit
  weft.7 names what the kit contains.

**Cross-cutting rules for every card:**
- The Makefile uses `-std=c99 -O2 -Wall -Wextra -Wpedantic -Werror`. One
  `weft/Makefile.inc` names these once (W01). The frozen dr Makefile and
  most control Makefiles lack `-Wpedantic`.
- Checks run under `MALLOC_OPTIONS=S`, with no network, no wall clock and
  fixed locale and seed.
- Every program pledges and unveils before its first untrusted byte.
  Today only weft-view and seed-session do; the producer, launcher,
  presenter, display child, drctl, weft-compare and the pty children do
  not.
- `-Wl,--wrap` link seams become `-D` rename seams or an injected ops
  table, as base-driver already does.
- `#include` of a `.c` file across directories (about 33 sites in
  controls) becomes a link against the component's library.
- Each check keeps a standing known-bad control for the rule it enforces.

## 4. Retired, and why

RETIRE has 59 files and 9,599 lines. The INVENTORY gives a reason for
each. By kind:

| What | Files | Why |
|---|---|---|
| compare/inputs/raster/* | 5 | Drifted older copies of the core raster, scene and terminal header. weft.7's own rule is "one canonical tree". The canonical copies are PORTed in lib. |
| deps/dr/vi-experiment/* | 5 | Research: kdump traces and a pty driver. Research stays out of kit inputs (STANDARDS 1). Its one finding, that nvi `:w` writes in place and not atomically, goes to dr(3) CAVEATS. |
| Evidence and packaging notes (BASELINE-README, README.md, DRIFT-FIX, FLICKER-CHANGE, ATTRIBUTION, NEXT.md, input-pins.json, edge-api/OFFER.md, garden provisional records) | 10 | Packet history, kept in frozen src and the record. Their duties live in the manuals and card records. |
| run_native.py, ppm_preview.py, svs/run.sh | 3 | ssh to a named host; a viewing aid; a runner for fixtures that are absent (`../../M02`). |
| launcher/launcher/test_early_cancel.c | 1 | Empty stray. The real control is launcher/test_early_cancel.c. |
| origin-copy files no static import reaches, plus Rust/Cargo/Zig, agent correspondence and generated files | 35 | Foreign toolchains and unreached files. The bytes stay in frozen src. Static closure can under-count dynamic imports, so a garden goal re-checks them. |

Nothing load-bearing is retired. Each RETIRE row says where its duty
went, or that it had none.

**DEFER** has 286 files and 83,511 lines (72,231 of them Python). These
are kept in frozen src for a later goal: the garden, touch, the
window-motion physics, the operator/install script and the garden-only
controls.

## 5. Python

**None of weft's Python enters the kit in this plan.** All of it is
garden, touch, operator or test.

Python in kept controls becomes C or sh, in the card that merges the
control:

| Frozen | Becomes | Card |
|---|---|---|
| controls/room-motion-refusals/nonblock_exec.py | a C helper of about 30 lines (set O_NONBLOCK, exec) | W18 |
| controls/help/test_producer.py, test_actions_producer.py | sh plus the C event writer (events.c) and a C frame decoder | W19 |
| deps/compare/native_checks.py (the only home of the CLI IO red controls) | sh regress | W29 |

If the garden returns:
- The custody entry (pin check then exec), the raster adapters (a C
  library from `weft_garden_raster2.join-index.c`, whose header is
  missing), painted_cache, the frame checks and the broker client all
  have a lower tier.
- The weft3d and molly engine (about 60k lines of world simulation and
  content) would keep Python only with a written reason, and only after
  the Python rules return through STANDARDS 11 (v2-r4 391-401, now
  RETIRED-UNTIL).
- The accepted r5 `libweft_garden_raster.so` has **no source in the
  tree**. A garden goal must start from source, not that binary.

## 6. Owner decisions this plan needs

- **D1 Goals.** Add WEFT-1 to WEFT-6 to the plan, or decline them
  (section 2).
- **D2 Static posing.** The physics engine (engine/*, 1,710 lines) and
  `weft_room_bridge` animate panes toward their targets.
  `wmf_pose_scene` only replaces x/y with the physics pose, so posing at
  the target gives the committed scene back unchanged. The frame custody
  in `weft_motion_room.c` is needed either way. This plan ports custody
  and defers physics (W17). Cost: the WNPOSE01 `steps` and `posed_scene`
  fields carry 0 and the source digest. Say yes, or carry the physics.
- **D3 DRM ABI header.** `kms/abi/drm.h` and `drm_mode.h` are 2,758
  copied lines of xenocara libdrm (MIT), already OpenBSD-flavoured. The
  code uses 11 ioctls and 10 structs. There are two options:
  1. a trimmed `wd_kms_abi.h` of about 150 lines that keeps the notices.
     This is retained foreign material, so that rule returns through
     STANDARDS 11 first;
  2. include the installed xbase headers. This makes the X set a build
     input, and whether base installs them is UNKNOWN.

  The plan assumes option 1 (W20).
- **D4 Passive asset.** `font.816` is a 4096-byte imported asset with no
  build rule. The passive-asset rule (v2-r4 124-131) is RETIRED-UNTIL the
  kit holds one, so it returns through STANDARDS 11 before W07.
- **D5 Atomics.** `wp_atomic_openbsd.h` uses the `__atomic_*` builtins on
  memory shared between processes (KMS receipt slots). These builtins are
  a compiler extension: they are neither C99 nor base API. Options:
  - a recorded exception;
  - C11 `<stdatomic.h>`, which is outside the C99 foundation;
  - a redesign that passes receipts through a pipe.

  The plan carries the header unchanged with a debt entry (W21).
- **D6 Clocks in tests.** The supervisor's 250 ms reap budget and some
  controls use real time. The plan gives each product clock an injected
  seam for regress and keeps the real budget in the product. Latency
  claims (root-join, robust-base) leave the regress and become a separate
  supervised measurement.
- **D7 Install.** `operator/operate.sh` runs as root under doas:
  - it chowns /dev/dri/card0 and /dev/wskbd0;
  - it stops xenodm with rcctl;
  - it su's to a hard-coded user;
  - it calls a `custody.awk` that is missing.

  It is not ported. A WEFT-INSTALL goal writes a root change list with a
  rollback for each item, under STANDARDS 5.
- **D8 lib/glyph** uses the weft/lib scene header (section 3).
- **D9 One pty runner or two.** W14 ports weft_pty and adds child
  confinement: execpromises and a veil, following sew_pty's pattern.
  Whether sew_pty and weft_pty become one is a separate card, not a
  widening of this plan.

## 7. Order of ports

The order puts the data plane first, devices last and the garden never.
Each step builds and tests alone on the build host, and each finish line
is visible: the PPM at W07 is the first picture.

| Step | Card | Component | Adds (est.) | Component ceiling after |
|---|---|---|---|---|
| 1 | W01 lib-core | lib | 1,700 | 1,700 |
| 2 | W02 lib-raster | lib | 1,650 | 3,350 |
| 3 | W03 lib-status | lib | 480 | 3,830 |
| 4 | W04 keys-map | keys | 1,700 | 1,700 |
| 5 | W05 keys-input | keys | 1,600 | 3,300 |
| 6 | W06 keys-help | keys | 1,100 | 4,400 |
| 7 | W07 seed-view | seed, view | 300 + 300 | 300 / 300 |
| 8 | W08 dr-store | dr | 1,600 | 1,600 |
| 9 | W09 drctl | dr | 450 | 2,050 |
| 10 | W10 room-reduce | room | 1,200 | 1,200 |
| 11 | W11 room-panes | room | 1,150 | 2,350 |
| 12 | W12 room-model | room | 1,750 | 4,100 |
| 13 | W13 pty-terminal | pty | 1,100 | 1,100 |
| 14 | W14 pty-runner | pty | 1,250 | 2,350 |
| 15 | W15 pty-clients | pty | 650 | 3,000 |
| 16 | W16 motion-wire | motion | 1,500 | 1,500 |
| 17 | W17 motion-custody | motion | 1,050 | 2,550 |
| 18 | W18 motion-producer | motion | 1,900 | 4,450 |
| 19 | W19 motion-joins | motion | 1,250 | 5,700 |
| 20 | W20 driver-kms | driver | 1,100 | 1,100 |
| 21 | W21 driver-supervisor | driver | 700 | 1,800 |
| 22 | W22 driver-frame | driver | 960 | 2,760 |
| 23 | W23 driver-adapter | driver | 1,350 | 4,110 |
| 24 | W24 launch-reap | launch | 660 | 660 |
| 25 | W25 launch-presenter | launch | 900 | 1,560 |
| 26 | W26 launch-core | launch | 1,550 | 3,110 |
| 26b | W26b launch-join | launch | 1,700 | 4,810 |
| 27 | W27 views-svs | views | 1,050 | 1,050 |
| 28 | W28 views-compare-lib | views | 1,500 | 2,550 |
| 29 | W29 views-compare-cmd | views | 550 | 3,100 |
| 30 | W30 views-pane | views | 1,400 | 4,500 |
| 31 | W31 views-join | views, motion | 650 + 100 | 5,150 / 5,800 |
| 32 | W32 weft-overview | weft | 60 | 60 |

**How a ceiling counts:** `wc -l` over every file in the component
directory, including Makefile and regress/, but not manuals (section 1,
5, 7 and 8 pages are recorded, not ceilinged).

**How the estimates were made:**
- start from the kept frozen lines (wc -l, INVENTORY);
- multiply by 1.15 for the style(9) reflow of dense frozen code (an
  assumption; each card's return measures it);
- add pledge/unveil and seams;
- add the kept controls after de-duplicating harnesses (about 30% off the
  frozen control lines).

**The total ceiling** across all components is 37,910 lines, regress
included, when every card spends its whole ceiling. The frozen weft C is
47,884 lines (C, H and INC) plus 77,102 lines of Python. The PORT and
MERGE C behind the cards is 39,851 frozen lines (INVENTORY). Inside the
PORT files, deferred code leaves with its feature: about 1,400 lines of
weft-room-motion.c (touch, garden, heartbeat, physics tick), about 180 of
weft_motion_room.c (posing) and the garden branches of seed_session.c and
weft_pty.c. A size claim is measured on the returned bytes, never taken
from this table (STANDARDS 10).

Steps 8-9 (dr), 20-23 (driver) and 24 (reap) depend on none of steps
1-7 and can run beside them. W25 needs W16's decoder. W26 needs W18's
producer for test_session. W26b links motion, so it follows W19. The
views need dr (W08) and, for W30-W31, the producer. Within a component
the order holds.
