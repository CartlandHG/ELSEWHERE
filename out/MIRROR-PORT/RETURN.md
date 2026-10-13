# RETURN: card MIRROR-PORT (stopped before the port)

Standard edition 2026-10-13.v3, NORMATIVE-SHA256SUMS SHA256
c1016a7ff6fa9a26acb7a5dc74ab9db7bb5598dbc8d6f0a2d5dcd19508d8b8ec.
Plan in force in the kit: plans/SPINE.md, SHA256
c4651a4bc52816d7d37e27c501cb01e6aa7eee38d17b7966677c2db041ea4a87.
Inputs: frozen-src-75ad5ce4.tgz (SHA256
8cf5a408d1fc34f1204a7a70414ca7b389e33b7de677bf15941a04b339708480) and
kit-2d6c0739.tgz (SHA256
5fccc65f23a7cbe7b1f2ee39357572d592fff4fa6275cb8f90c94304c3254cdd). Both
archive digests matched the card, and every file matched its tree's MANIFEST.
Author: this session (Claude on claude.ai/code, a scheduled run). Reviewer:
not named by the card.

## First: why there is no port in this return

I stopped before writing any code. The common rules say that where the card
conflicts with the standard, the standard wins, and that I should stop and
say so when the work outgrows the card. Four conflicts apply here, and the
first one is enough on its own.

1. **The card cites no goal in the plan in force.** The card serves "NARF
   goals G1 and G2". The kit's only plan is SPINE.md, and its goals are
   SPINE-1 to SPINE-5, STD-3 and LANES-1. NARF-1.md is listed in
   FROZEN-EXCLUDED.txt as a superseded plan. STANDARDS section 2 says every
   change "cites the plan goal it advances", and names "a change with no plan
   goal" as a violation. SPINE.md ("What waits") says deferred work "returns
   as a new goal here once SPINE-5 stands, and never by widening a spine
   program".
2. **The weave is a new kind of component.** The STANDARDS introduction
   says a "transclusion resolver" first brings its own rule through
   section 11. sew_weave_resolve is that resolver: it re-finds anchored
   spans in source bytes and decides LINKED, CONFLICT or DANGLING. The card
   itself names G1 "mirror and transclusion". PRESERVATION.md retired the
   resolver obligations of v2-r4 (lines 166-178) until such work is
   authorized. Nothing in the kit records that authorization. Section 1
   says only the owner authorizes this kind of work, and that general
   permission to work does not count.
3. **weft_glyph is not part of G1 or G2.** In the frozen tree its only
   consumer is marble/legs (lib/glyph/README:6 "the seam the G3 legs link";
   marble/legs/src/lg3_recovery.c:70). It is the marble and garden seam.
   SPINE.md lists "the garden" under "What waits". Neither mirror nor the
   pane includes it.
4. **The card is missing fields that jobcard(5) and STANDARDS section 2
   require**: author, reviewer, integrator, tier, ceiling, the class of each
   path, and rollback. The card asks me to propose the ceiling (I do so
   below), but the other fields cannot come from the author.

A fifth problem would have made the port unsafe even with a goal in place.
mirror_pane's confinement does not live in mirror_pane. Its unveil(2) veil,
which is what keeps the source document read-only, is applied by its
runner sew_pty (frozen sew/pty/src/sew_pty.c:290-322), and the runner is
outside this card. If I ported the pane alone, that safeguard would be lost.
If I added a veil inside the pane, I would be adding authority and
interface beyond what the card covers. Section 5 says "an unenforced
required boundary blocks admission".

So this return carries the analysis a correct card needs: the dependency
and include map, the defects that a v3 port must repair, the proposed cuts,
and a costed ceiling for each component. None of these files is a port,
and nothing here is PASS.

**What the owner can decide** (pick one):

- (a) Add a goal to SPINE.md, or to a new plan, for the mirror lens. Record
  the authorization for a transclusion resolver and its section 11 rule
  (the starting text is v2-r4 166-178, as PRESERVATION.md names). Then
  reissue this card with the jobcard(5) fields, either including
  sew_pty's mirror veil or making self-confinement in the pane part of the
  card's purpose.
- (b) Keep the mirror family frozen until SPINE-5 stands, as SPINE.md
  already says. In that case this return is simply the record for that later
  card.
- In both cases, give weft_glyph its own card under a garden goal. It does
  not belong in this component.

## 1. Deletions, with reasons

**Deletions made: none.** No file was ported, so nothing was removed.

The cuts the port should make are listed below. Each would be justified in
that card's diff, and section 10 obliges the port to reconcile callers
first. Frozen line numbers are at 75ad5ce4.

| Proposed cut | Frozen location | Lines | Reason | What must be reconciled first |
|---|---|---:|---|---|
| Revision-history banner comments (KIKI verdicts, R13/R17 narratives) | weft_weave.c:1-36, 245-255, 392-399; weft_weave.h:1-25; weft_mirror.h:1-34; weft_mirror.c:1-14; mirror_pane.c:1-46 | ~190 | History belongs in the record. Section 4 asks for contracts and invariants, not narration | None |
| v1 sidecar reader and migration | weft_weave.c:58-75 (v1 part), 330-390, 517-518 | ~70 | The kit holds no v1 store. mirror_pane removes its store at start (mirror_pane.c:885) and never reopens it | Any v1 `WEAVE-SEAVE-V1` file still on WHEATFIELD (UNKNOWN). If one exists, keep the reader, or convert the file once by an explicit act |
| `sew_weave_edit_before`, the documented no-op verb | weft_weave.h:107-112; weft_weave.c:724-738 | 21 | It is an interface that does nothing ("kept for callers that may rely on it") | Callers: none in the card's files. Other frozen callers UNKNOWN until an inventory is taken |
| `edit_offset` parameter of `sew_weave_edit_inside` | weft_weave.c:745 | 1 | The parameter is accepted and discarded, so the interface misdescribes itself | test_leg2_mirror.c and the pane's TYPE path |
| 256-bucket FNV hash table | weft_weave.c:45-56, 155-204 | ~50 | It serves MAX_SPANS 8 (mirror_pane.c:80). A bounded array with linear search is simpler | Any consumer with many spans (UNKNOWN) |
| Test knobs `no_colour`, `strong` and `override_colour` in product state | weft_mirror.h:71-77; weft_mirror.c:88-89 | ~6 | These are controls (T1, T11). The tests can build their wrong version instead | test_leg2_mirror.c rows T1-CONTROL-NO-COLOUR and T11-CONTROL-RECOLOUR |
| Unused includes `<stdio.h>` and `<stdlib.h>` | weft_mirror.c:17-18 | 2 | Not used | None |
| RAW-FILE control compiled into the product source via `#ifdef` | mirror_pane.c:116, 140, 152, 335, 542, 581, 590, 795-836, 853, 883, 1022, 1072 | 13 conditional lines and the 42-line control repaint (795-836) | A known-bad build should be a separate test stub, not a mode of the product file | battery row control_rawfile_verdict |
| The scratch store and the per-render span it creates | mirror_pane.c:335-395, 912-918 | ~65 | It grows by two spans per span on every repaint and is never freed (see defect P6). Its job is to re-find the anchors inside the body, which is unclear (see the open question below) | Reviewer decision on the open question |
| `wgs_memory` | weft_glyph.c:348-353 | 6 | It returns sizeof, which needs no routine | marble/legs (out of card) |
| Copies of weft_scene.h and weft_marble.h inside lib/glyph | lib/glyph/weft_scene.h, weft_marble.h | 180 | These are duplicate weft headers. The card says weft material must be named as a dependency, not copied | weft_scene.h differs from weft's own copy (weft/native-source/room-motion/component/core/weft_scene.h) only in two attribution comments. Settle which attribution is right before deleting (section 10) |

Open question for the reviewer (not decided here): the pane renders each
body through a scratch-store resolve, so a working body whose edit destroys
an anchor exits 3 (BLOCKED). The comment at mirror_pane.c:997-1003 treats one
such exit as a fault. If it is a safeguard, the port keeps it as an explicit
anchor check. If it is not, the port renders the body directly with the
offsets 0 and its length. That choice changes the exit-3 contract in
mirror_pane.1:300-303.

## 2. Frozen files and lines read

All files under src.git 75ad5ce4, counted with `wc -l`:

| File | Lines | SHA256 |
|---|---:|---|
| lib/mirror/weft_weave.c | 806 | c66d82340c287077389ca684cd6f1ef1c25d7323f444a1bfda4fedb5c13a9aa7 |
| lib/mirror/weft_weave.h | 143 | 696f0d87027b563094f312dc64a8e6c566e5ed22af3ca5b25f9d2390280d9908 |
| lib/mirror/weft_mirror.c | 114 | 20aebcfab5648f7a5f073b5fc53e07bde33f28e07b6017142cbaac13b44004fa |
| lib/mirror/weft_mirror.h | 101 | c4cc19c43dbea7f62076a82d5529c96f14540384899c7dbe01c877d7a528fafc |
| lib/mirror/weft_weave.3 | 593 | 514f26462e39d91cb9ab9ba2fdd11a0d1a2e18cb057b5a3823f8c632d0741ebf |
| lib/mirror/weft_mirror.3 | 316 | f7a9e27b0dd25d90385282c9a6bf9b65d476bc0cb8217a1aa2607266465d3182 |
| lib/mirror/tests/test_leg2_mirror.c | 333 | f505221e80403bec89690e48d99262c57b250502fc8749e01ac5fdba95c850f9 |
| lib/mirror/tests/run.sh | 50 | 0ab9455cd45048b369e3ce279f311bc9714d98cdaf21ccacfebb0afee4b84015 |
| lib/mirror/Makefile | 18 | eb3c3e8d144614ecafb9c477dcfd07124010287847d256dd910f555891f6e218 |
| lib/mirror/README | 15 | 4c661da49a836eceeb0550478c541448d8bfa2eb3b37510b1da5e8f10e68f016 |
| lib/glyph/weft_glyph.c | 353 | 6124c2bbcfcc794f8a9b420a2789902ee944efca3e289c624b9be798587a7c87 |
| lib/glyph/weft_glyph.h | 96 | c545d1d8d00385b2d1527aca9fc11d7e873dbd274b4dc5c76543d6ec4d6b13c4 |
| lib/glyph/weft_scene.h | 92 | bb8e09ede0162245d75cabfb35a57502080b36b1ef423b6ed187221c1fc47af1 |
| lib/glyph/weft_marble.h | 88 | 5e293773e057227ed6ca948907198791330b2d0d67ff48b7d73519a99fe7b6bd |
| lib/glyph/tests/smoke.c | 34 | 893072af37190b7a84434ea5a661eb0c50210b872f896e1297ec1c4fbc3d4a59 |
| lib/glyph/tests/run.sh | 27 | 41b5bb1d29800eeca912b84a222e6dc6ae3f296387af846c01382097025d74bc |
| lib/glyph/Makefile | 16 | 4b96d38d33f87a1429918be244b704a038767c685229a1cd9aa42eae18f2f713 |
| lib/glyph/README | 19 | 6647d2d2b5ed2f187655560626f50481962ded67f5c2b9cbfa6b3fa4c35233e0 |
| mirror/pane/src/mirror_pane.c | 1079 | 063454c48513cf11b163176229a704780073acd0fa778476d4dbf70db2d97910 |
| mirror/pane/mirror_pane.1 | 328 | b638901b1d0821033a3f5e90a75b94cbd702b8370e830a02a7140cc97b7de2ea |
| mirror/pane/tests/battery.py | 598 | 6a3e0333a8dc8f771d5aadd63248a072128e07c298b93b872593eb852eca9afd |
| mirror/pane/tests/run.sh | 32 | f6a0ba250ce28aafc08c8b99bb065e8b3fd613e364068a091c077a06787ec013 |
| mirror/pane/Makefile | 17 | f45ae717f57b0de6e1550004ab42809fd63225429174a2bc16d4708e61aa0e03 |
| mirror/pane/README | 22 | 9fc1972a94d43204543fb649277199470139f974b65bcc46401522fcddf5725f |

C in scope: 2,872 lines (glyph 629, mirror and weave 1,164, pane 1,079), plus
367 lines of C tests. I also read these files to map dependencies (I took
nothing from them): sew/pty/src/sew_pty.c, marble/legs/src/lg3_recovery.c,
weft/weft.7, weft/native-source/room-motion/component/core/weft_scene.h,
and tools/census/CENSUS.md.

### Include and dependency map

| Includer | Include | Provider | Port disposition |
|---|---|---|---|
| weft_weave.c | `<sha2.h>` | OpenBSD libc, sha2(3) | Base. Keep |
| weft_weave.c, weft_mirror.c, mirror_pane.c | `<stdio.h> <stdlib.h> <string.h> <stddef.h> <stdint.h>` | C99 | Base |
| mirror_pane.c | `<errno.h> <fcntl.h> <poll.h> <time.h> <unistd.h> <sys/types.h> <sys/event.h>` | POSIX and OpenBSD (kqueue(2)) | Base |
| mirror_pane.c | pledge(2) via `<unistd.h>` | OpenBSD | Base. The port adds unveil(2) |
| weft_mirror.h, mirror_pane.c | "weft_weave.h" | lib/mirror (in card) | Internal |
| weft_glyph.h | "weft_scene.h" | **weft**: weft/native-source/room-motion/component/core/weft_scene.h (types wn_scene, wn_node, wn_rect, wn_error, enum wn_result) | **Dependency on weft, named, not copied.** Only the types are used; glyph calls no wn_* routine |
| weft_glyph.h | "weft_marble.h" | **weft marble adapter** (types wmb_marble, wmb_body). The frozen tree holds no implementation of wmb_feed and the other wmb_* routines outside this header | **Dependency on weft, named.** The header's owner and its implementation are UNKNOWN |
| mirror_pane (runtime) | sew_pty `mirror` kind | sew/pty (out of card) | **Runtime dependency for confinement** (see section 5) |
| mirror_pane spans file | span(1) format | sew/span (out of card) | Input format owned outside the card, with no section 5 manual |
| lib/mirror/tests/run.sh, mirror/pane/tests/run.sh | mk/house.mk (through Makefile), python3 | frozen mk/, Python | The card forbids house.mk. battery.py has to become /bin/sh |

## 3. STANDARDS, section by section

All of these refer to a port that has not been written. "Defect" rows name
frozen code that a v3 port must repair. They are written as debt entries so
the next card inherits them. Owner of every entry: the integrator of the
reissued card. Review trigger for every entry: the reissue of this card.

| Section | Applies | State | Debt and defects found in the frozen code (rule; location; remedy) |
|---|---|---|---|
| 1 Foundation | Yes | Not met. weft_* and pane are C99 with base headers. battery.py (598 lines) is Python with no written tier reason, and it drives a PTY | Tier: battery.py → /bin/sh driving the pane over a pipe on stdin. The pane reads commands from stdin and needs no PTY. Cost below |
| 2 Work and seams | Yes | **Not met: no plan goal** (see First). Card fields are missing | Remedy: a goal in the plan in force and a complete card. Ceilings proposed below |
| 3 Evidence | Yes | UNKNOWN. Nothing was built or run. Existing controls: PURITY-RED-CONTROL (lib), control_noveil and control_rawfile (pane) | Persistent format (WEAVE-SEAVE-V2): no control yet for truncated, failed-write or recovery. Remedy: add one per case |
| 4 C | Yes | Not met. Defects follow | W1 no commit point: weft_weave.c:568 rewrites the sidecar in place with fopen "w"; a crash leaves it truncated. Write and fclose results are ignored, and close returns void. Remedy: temp file, fsync, rename(2); return a status |
| | | | W2 silent data loss: weft_weave.c:514-525,559 opens an unknown header as an EMPTY store with its path kept, so close then overwrites the file. This contradicts its own comment at :483. Remedy: refuse (NULL) |
| | | | W3 unchecked numbers: weft_weave.c:283-307 strtoull/strtol accept a sign and leading space, with no ERANGE check and a long→int narrowing. :523 a bad next_id falls back to 1 (ID reuse). Remedy: digit-by-digit parsing with bounds |
| | | | W4 silent truncation changes identity: weft_weave.c:145 (docid), :593 (uri) strncpy; :146 context clamped to 8 bytes, while :152 hashes the untruncated length (the stored sha does not match the stored context). The pane passes contexts of up to 16 bytes (mirror_pane.c:88). Remedy: refuse anything over the bound |
| | | | W5 range arithmetic: weft_weave.c:654-675 adds an int mark offset (possibly negative) to size_t, clamps the end to src_len (ADDRESSING: "never clamp a persistent address"), and returns 0 (LINKED) with begin = (size_t)-1 when begin passes EOF (:674). Remedy: bound before addition and refuse |
| | | | W6 duplicate span ids are accepted on load (put_span :179). Remedy: refuse (section 8) |
| | | | W7 a uri containing a space is written unchecked (:87) and shifts its row. Remedy: validate the alphabet at create time |
| | | | P1 partial input published as complete: mirror_pane.c:284-294 reads at most 16383 bytes and silently renders a truncated source. Read error and empty file look the same. Remedy: refuse over MAX_DOC |
| | | | P2 malformed spans are skipped silently: mirror_pane.c:209-282 `continue`, a negative strtol is cast to uint64, the uri is truncated, and extra spans are dropped (:275). Remedy: refuse the whole file and name the line |
| | | | P3 a TYPE whose edit_inside fails (:1023) is still applied to the pane's delta, so the store and the pane diverge. Remedy: apply only on success |
| | | | P4 wrong diagnostic: mirror_pane.c:170-172 reports CONFLICT as "begin anchor gone" when both anchors resolved |
| | | | P6 unbounded growth: every render adds a span to scratch_store (:367) and never drops it. Receipts grow by two files per repaint, and the events file grows without bound. Remedy: drop the scratch store (see Deletions), and bound or rotate the receipts |
| 5 Authority | Yes | **Not met.** The pane pledges `stdio rpath wpath cpath tty` (mirror_pane.c:868) but never calls unveil. Its read-only source relies on sew_pty | P5 terminal injection: display() (:564-588) writes raw source bytes, ESC included, to the terminal. Remedy: show bytes outside 0x20-0x7e as '.'. A5 PROBE (:967-981) opens the source O_WRONLY from the product path. Remedy: keep it only as the in-process refusal control, behind unveil "r", and document it. The `tty` promise has no use in the code (UNKNOWN until native). The pane reads an untrusted spans file and source, so it owes a confinement statement in its manual. Remedy: self-unveil(source "r", spans "r", cwd "rwc") then lock, before parse_spans |
| 6 Shell and checks | Yes | Not met. In lib/mirror/tests/run.sh:11-20 and lib/glyph/tests/run.sh:13-17, a build failure runs `exit 2`, but the EXIT trap `exit $rc` replaces that 2 with 1. A setup error then reads the same as a test failure, which section 3 forbids. The tests depend on mk/house.mk and python3. The pane waits on a 150 ms clock window (mirror_pane.c:1054) | Remedy: sh tests with a fixed locale (LC_ALL=C), no clock dependence except a bounded timeout, and one known-bad control per rule |
| 7 Manuals | Yes | Not met. weft_weave.3 (593), weft_mirror.3 (316) and mirror_pane.1 (328) carry `.Os ELSEWHERE` and are dated October 7, 2026. They describe frozen behaviour, including the v1 reader and the silent empty store. No lint record exists in the inputs (UNKNOWN). The WEAVE-SEAVE-V2 sidecar has no section 5 page. Neither does the span(1) spans file | Remedy: rewrite with the port, plus weave(5) for the sidecar format |
| 8 Custody | Yes | Not met for the sidecar: it is not a restricted-header format, and duplicate ids and truncation are not refused (W4, W6) | Remedy: refuse truncation, duplicates and control characters |
| 9 Review and admission | Yes | Nothing to admit. No build, no reviewer named | — |
| 10 Adding and removing | Yes | No additions made. Each proposed cut lists its reconciliation (section 1). The proposed additions (unveil, atomic write, refusals) widen nothing that is accepted; they narrow it | — |
| 11 Changing the standard | Yes | **Needed**: a transclusion resolver brings its rule through section 11 first (see First) | Remedy: the owner admits a resolver rule starting from v2-r4 166-178 |

### Proposed line ceilings, with what each part costs

These are estimates for a v3 port that keeps every load-bearing behaviour
and makes the repairs above. Lines are style(9) lines counted with `wc -l`,
comments included. I estimated them by reading the code; none was written,
so they bind nothing. Tests and manuals do not count toward a component's
ceiling.

**lib/weave + lib/mirror (one component, libmirror.a): ceiling 750**

| Part | Frozen | Port estimate | Why |
|---|---:|---:|---|
| weft_weave.h: types, contracts | 143 | 95 | Banners cut; contracts kept |
| Store: array of at most N spans, find, put, drop | ~60 | 40 | Hash table replaced |
| Anchor make and resolve-in-source | ~50 | 55 | Refuses over-long context and docid (W4) |
| sew_weave_resolve: reconcile, states, offsets | 120 | 110 | Bounds before addition, no clamp (W5) |
| edit_inside, detach, delete_source, delete_copy, state, lookup | 85 | 65 | edit_before and its unused parameter cut |
| Sidecar v2 parse: strict fields, numbers, hex, docid, duplicates | ~200 | 170 | v1 reader cut; strict numbers and duplicate refusal added (W3, W6) |
| Sidecar write: temp, fsync, rename, status | ~50 | 75 | Commit point added (W1) |
| Open and close, refuse bad header | ~80 | 60 | W2 repaired |
| weft_mirror.h + .c: palette, prepare, pure render | 215 | 80 | Test knobs and banners cut |
| **Total** | **1,164** | **~750** | |

**pane (mirror_pane): ceiling 750**

| Part | Frozen | Port estimate | Why |
|---|---:|---:|---|
| Header, limits, types, globals | ~140 | 70 | Banners and #ifdef cut |
| Spans file parse, strict, refusing | 95 | 90 | P2 |
| Source read, bounded and refusing; hash | 35 | 40 | P1 |
| State mapping and reason | 35 | 30 | P4 |
| Render seam and cells_check | 140 | 110 | Scratch store removed or replaced by an explicit anchor check (open question) |
| Repaint: source half, edit half, delta | 210 | 180 | |
| Receipts and events | 75 | 70 | Bounded (P6) |
| Display with filtered bytes | 25 | 25 | P5 |
| Confinement: unveil, pledge, lock | 10 | 30 | Section 5, added |
| main loop: commands, kqueue drain | 230 | 150 | RAW-FILE control moves to a test stub |
| **Total** | **1,079** | **~750** | Raw-file control repaint (42) excluded |

**lib/glyph: not in this card.** Proposed ceiling 420 when it gets its own
card under a garden goal: glyph.c 353 to about 330 and glyph.h 96 to about
90. This assumes weft_scene.h and weft_marble.h come from weft as a named
dependency.

**Tests and manuals (outside the ceilings, estimated):** lib test driver in C
~300 plus sh wrapper ~60; pane battery in /bin/sh ~250 plus a known-bad stub
~40; weave.3 ~300, mirror.3 ~120, weave(5) ~150, mirror_pane.1 ~220.

## 4. Authorities, interfaces, options, state and dependencies added

None. This return adds no code. For the record, the port would add:
unveil(2) in the pane (authority narrowed, not widened); a temporary
sidecar file plus rename (new transient state); and a section 5 page for an
existing format.

## 5. What stays UNKNOWN until the native runs

Everything that a build or test must show: whether the frozen code builds
with `-Werror` on OpenBSD 8.0; whether any defect above reproduces natively
(each was found by reading the code); mandoc lint of the three frozen pages;
whether a v1 sidecar exists on WHEATFIELD; whether the `tty` promise is
needed; whether the scratch-store anchor check is a safeguard; who owns
weft_marble.h and where it is implemented; and every line estimate in
section 3.

## 6. wc -l

| File | Lines |
|---|---:|
| RETURN.md | 261 |
| SHA256SUMS | 1 |
