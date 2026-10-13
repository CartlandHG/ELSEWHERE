# RETURN: the kit's checks, card GATES-PORT

Standard: edition 2026-10-13.v3, NORMATIVE-SHA256SUMS
c1016a7ff6fa9a26acb7a5dc74ab9db7bb5598dbc8d6f0a2d5dcd19508d8b8ec.
Inputs: frozen-src-75ad5ce4.tgz
(8cf5a408d1fc34f1204a7a70414ca7b389e33b7de677bf15941a04b339708480) and
kit-2d6c0739.tgz
(5fccc65f23a7cbe7b1f2ee39357572d592fff4fa6275cb8f90c94304c3254cdd). Both
digests matched before use, and every line of both MANIFESTs checked.
Author: this session (Claude on claude.ai/code, a scheduled cloud job).
Nothing here was compiled, run or linted: every result below that a run
must show is UNKNOWN.

## Before anything else: the card has no plan goal

SPINE.md has no goal for the gates, and the card names none, nor a line
ceiling, path classes, a reviewer, an integrator or a rollback (STANDARDS
section 2, "a change with no plan goal"). I did the work the card
describes and stopped there. Admission needs the plan revised to carry a
goal for it, or the card withdrawn. It is entered as debt D1 below.

## 1. Deletions, with reasons

Of the nine frozen tools, three survive as ports and six are retired. No
frozen file is copied unchanged.

| Frozen tool | Lines (all its files) | Disposition | Reason |
|---|---:|---|---|
| tools/census (run-census.sh, CENSUS.md, 4 controls) | 249 | RETIRED | A report-only measurement, not a gate: it decides nothing and exits 0 on any finding. It needs cppcheck, shellcheck and ruff, which are packages, not the base system (STANDARDS 1). Its useful parts live on: the banned-call grep is banned-lint, and its rule that a tool must go red on its own bad fixture before it is trusted is the control discipline of every surviving gate. |
| tools/comment-only-proof | 292 | RETIRED | Python, and STANDARDS 1 lets Python in only with a written reason that C or a base tool fails; C would serve, so no such reason exists. The kit does not need it: STANDARDS 4 lets a comment-only change stand as REVIEW without a comparator, and no kit card is comment-only. It cannot judge mdoc, which is where most kit prose lives. It returns, as C99 and from its frozen text, when a card needs a mechanical comment-only decision. |
| tools/gates/include-closure-lint (script, page, 15 control files) | 1648 | RETIRED | The question it answered ("can this named source compile from the packet?") is now answered by the compiler: STANDARDS 3 requires the native build before any review, and a missing header fails it. It also passed a macro include as PASS (an absent diagnostic from an incomplete checker, which STANDARDS 3 forbids), built a shell command from the root path inside awk (untrusted text as shell, STANDARDS 6), and its symlink control has lost its link in the frozen export. |
| tools/gates/pin-consistency-lint | 238 | RETIRED | It checks `PIN_*` constants in `worker/*.py`. The kit has no Python and no worker; manifests are checked with `sha256 -c`. |
| tools/gates/asset-origin-lint (script, page, sidecar control; 2 binary controls excluded) | 387 | RETIRED | PRESERVATION.md retires passive-asset records until the kit holds a passive asset. It also needs got(1) and warp/sidecar.py, a Python validator outside the kit. |
| tools/gates/house-mk-law (Python, 7 controls) | 152 | MERGED into mk-lint, Python dropped | Its INCLUDE, BODY and NO-CHECK laws serve mk/house.mk, which the kit does not have. Its load-bearing safeguards are kept per Makefile in mk-lint: the strict flags (FLAGS), CFLAGS never defaulted with `?=` (FLAGS), CC not replaced (CC), MALLOC_OPTIONS never weakened (MALLOC), and a check that runs no real command (EMPTY, now including `:`, `true`, `exit 0` and a plain echo). |
| tools/gates/check-targets-lint (script, page, 11 controls) | 560 | PORTED as mk-lint | Kept: EMPTY (empty, comment-only, bare continuation, empty `;`, non-check prerequisite), delegation, variable target or prerequisite is UNKNOWN, includes followed. Cut: `--selftest` and `--help` (now `make check` and gates(7)); the root-Makefile rule became "every Makefile has a check target"; `eval find` over registry paths (untrusted text as shell); predictable /tmp files; the duplicated closure code (two copies of the same 33 lines). Fixed: a missing or variable include was skipped silently, now UNKNOWN when it could hide the check; UNKNOWN no longer outranks FAIL (STANDARDS 3: a known failure makes the aggregate FAIL); the selftest case `check-vacuous-red` had no fixture and passed only because a missing directory also exits 2, so it is dropped rather than carried as a vacuous control. |
| tools/gates/product-banned-calls-lint (script, page, classifier, 4 controls) | 195 | PORTED as banned-lint | Kept: the eight names and the call pattern. Cut: the classifier and its test-side exemption, because STANDARDS 4 binds all new C, tests included, and the 15 exempted hits were in frozen tests the kit does not carry; the g1-test-side control goes with it. This is stricter, not looser. Cut also: `--selftest`, predictable /tmp files. |
| tools/gates/stack-law-lint (Python, 21 controls) | 397 | PORTED as foreign-lint, to /bin/sh and awk | Kept: NO-LINUX, NO-GNU-C, NO-GNU-MAKE, POSIX-SH, NO-GNU-TOOL, C99-FLAGS, NO-GIT, comment blanking for C, comment lines skipped for shell and make, the fixture registry and its refusal rules, UNKNOWN when nothing is judged. Cut: STDLIB-ONLY and PY-PARSE, replaced by PYTHON (any Python is a finding until a written reason admits it, STANDARDS 1; PRESERVATION.md retires the Python rules until a Python component is proposed). Cut: `--report`, which printed FAIL and exited 0. Cut: the `clean-embedded` and `stdlib-only*` controls, which tested the retired Python rules. Fixed: an indented `source` was not caught. |

Interfaces removed: `--selftest`, `--help` and `--report` on every tool;
`classifier.txt`; `STACK-LAW-FIXTURES` (renamed GATE-FIXTURES below);
`house-mk-law.py ROOT COMPONENT...`;
`include-closure-lint ROOT [-I DIR] FILE...`; `comment-only-proof.py`;
`pin-consistency-lint`; `asset-origin-lint`; `run-census.sh`. No kit file
calls any of them (the kit holds only man/, plans/ and policy/), and
src.git is frozen, so there is no caller to reconcile.

## 2. The frozen files and lines I started from

From src.git 75ad5ce4, digests as in its FROZEN-SRC-MANIFEST:

| Frozen file | Lines | SHA256 | Became |
|---|---:|---|---|
| tools/gates/stack-law-lint/stack-law-lint.py | 329 | e7e787802f487e28f9e9584026eab6dcb1fd296218bb427f1a348729010b2c60 | foreign-lint, foreign.awk, walk |
| tools/gates/stack-law-lint/controls/* (21 files) | 68 | | controls/foreign/*, controls/walk/fixtures-bad |
| tools/gates/check-targets-lint/check-targets-lint | 310 | 39b7993dd54bb721dd762178239e0d0b899ef6972e84acb349c44e01c166c9cf | mk-lint, mk.awk, walk |
| tools/gates/check-targets-lint/controls/* (11 files) | 152 | | controls/mk/* (rewritten small) |
| tools/gates/house-mk-law/house-mk-law.py | 117 | f247f48c276433cc6249fe3047da53e9bf703a00c5585f4b343de096b08a2ca4 | mk.awk (FLAGS, CC, MALLOC, noop) |
| tools/gates/house-mk-law/controls/* (7 files) | 35 | | controls/mk/{defaulted,malloc,cc,noop}-red |
| tools/gates/product-banned-calls-lint/product-banned-calls-lint | 101 | 8126c3a239ad15a4f307e0b9ce59bccae50e4015a248fdea4195b506dadbd200 | banned-lint |
| tools/gates/product-banned-calls-lint/controls/* (4 files) | 24 | | controls/banned/* |
| tools/gates/check-targets-lint/check-targets-lint.1, tools/gates/product-banned-calls-lint/product-banned-calls-lint.1 | 161 | | gates.7 |
| mk/house.mk | 24 | | the MALLOC_OPTIONS=S rule and the `?=` lesson in mk.awk |

The ported code totals 857 frozen lines of program (329 + 310 + 117 +
101). The port is 799 lines of program (walk 98, foreign-lint 73,
foreign.awk 142, mk-lint 162, mk.awk 179, banned-lint 66, want 79), plus
the Makefile (65) and gates.7 (414). Fewer lines is not the claim; the
tables in this section and the one above are.

## 3. STANDARDS, section by section

### 1. Foundation: applies

- Tier. The gates are tier 1: /bin/sh with base awk, find, grep and
  sort. They hold no state, take no lock and write nothing. They do read
  untrusted bytes (a returned tree), which section 1 assigns to C99. My
  reading, which the reviewer must accept or refuse: the bytes are read
  only by base C programs (awk at foreign-lint:46 and mk-lint:140-141, grep
  at banned-lint:39), never by the shell, which handles only names that
  walk:40-47 has checked against the name alphabet. If the reviewer
  refuses that reading, the remedy is a C99 port of foreign.awk and
  mk.awk, about 600 lines, and it is debt D2.
- No Python, no GNU dialect, no fetched or foreign helper. foreign-lint
  judges the gates' own directory as a case of `make check`
  (Makefile:10). That is a marker search, not the inventory section 1
  asks for: the inventory is the file list in section 6 of this return.
- No host names in commands.

### 2. Work and seams: applies, not met

The card cites no plan goal and names no ceiling, reviewer, integrator or
rollback. Debt D1.

### 3. Evidence: applies

- What each gate decides and cannot decide: gates.7 DESCRIPTION (laws)
  and CAVEATS (macros, conditionals, `#if 0`, strings, continuations,
  variables).
- Standing known-bad control for every law: Makefile:12-23 (foreign, all
  eight laws), 28-39 (mk: EMPTY five ways, NOCHECK, FLAGS three ways,
  WEAKEN, MALLOC, CC), 45 (BANNED).
- Valid positive: Makefile:10 and 24 (the gates' own directory, real kit
  bytes), 11, 25-27, 44, and fixtures-good at 46, 51 and 56, which also
  shows the registry honoured.
- Infrastructure failure: for each gate, a refused registry, a name
  outside the alphabet, nothing to judge, and a missing root (Makefile:
  47-50, 52-55, 57-60); for mk-lint also a missing include and three
  undecidable cases (40-43).
- Status reaching its caller: every gate ends `exit "$status"`
  (foreign-lint:73, mk-lint:162, banned-lint:66); want keeps the status
  (want:44-45) and exits 1 on a wrong result; `make check` keeps every
  case's status and exits non-zero if any is wrong (Makefile:9, 63).
  Makefile:61 shows a wrong expectation reaching want's caller as exit 1.
- Distinct results: want names PASS, FAIL, UNKNOWN, SETUP, TIMEOUT,
  KILLED and ERROR (want:46-60); Makefile:62 shows a kill reported as
  KILLED, not as a gate result.
- Precedence: FAIL outranks UNKNOWN and every UNKNOWN line stays printed
  (foreign-lint:64-70, mk-lint:154-159, banned-lint:57-62).
- Nothing judged is UNKNOWN, never PASS (same lines).
- Not met: no control yet for TIMEOUT (it costs 60 CPU seconds), for the
  refusal of a symbolic link or special file (a link does not travel
  safely in this packet), or for a file awk or grep cannot read
  (permissions do not survive the packet as root). Debt D3.

### 4. C: applies to what the gates enforce, not to their own code

No C is written. mk-lint enforces the flags (mk.awk:63-76, 164-166), CC
(mk.awk:78-80), no weakening (mk.awk:81-83) and the allocator options
(mk.awk:84-88); banned-lint enforces the unbounded calls. The 80-column
rule binds C; 13 lines of foreign.awk (between lines 58 and 111) are
longer, because an awk regex literal cannot be split. The shell files keep to 80 columns.

### 5. Authority and confinement: applies, not met

gates.7 SECURITY CONSIDERATIONS states what the gates protect, trust,
touch and send, delegation and failure. They write no file at all (the
frozen tools wrote predictable /tmp files), read stdin from /dev/null
(foreign-lint:46, mk-lint:141, banned-lint:39, want:44) and run nothing
from the tree. But section 5 requires a shell component to be confined by
a C99 launcher, and the kit has none; "an unenforced required boundary
blocks admission". Debt D4.

### 6. Shell and checks: applies

- /bin/sh, `set -u`, `LC_ALL=C` in every script (walk:21-23,
  foreign-lint:8-10, mk-lint:9-11, banned-lint:8-10, want:14-16).
- Expansions quoted; word splitting only on newline with globbing off,
  over names walk has checked (foreign-lint:29-31, mk-lint:124-125,
  banned-lint:27-29).
- Each producer's status kept: no pipeline anywhere; every find, awk and
  grep status is tested (walk:40, 82, 91; foreign-lint:21, 47;
  mk-lint:82, 142; banned-lint:40). walk sorts from a here-document so
  that its producer is not lost in a pipe (walk:95-98).
- Untrusted data only as arguments: the registry builds find's arguments
  with `set --`, never `eval` (walk:50, 78).
- No function shadows a ksh alias (functions: physical, resolve,
  include_closure, usage).
- Bounded: a CPU limit per case (want:44), includes at most 8 deep
  (mk-lint:76). Fixed inputs; no network; no clock.
- Explicit make target: `check` (Makefile:8). No install target.
- Not met: no bound on the size of the tree walked. Debt D5.

### 7. Manuals: applies

One gates(7) page (gates/gates.7), `.Os ELSEWHERE`, dated October 13,
2026, with the commands' synopsis, laws, results, EXIT STATUS, FILES,
EXAMPLES, CAVEATS and SECURITY CONSIDERATIONS. Its lint, rendering,
`man -k` discovery and the run of its examples are UNKNOWN (section 5).

### 8. Custody and identity: applies

SHA256SUMS is written last, lists every other file by relative name,
never itself. The packet is not edited after it is sealed.

### 9. Review and admission: not applicable to the author

This return asks for review; it claims no admission. Delivery, review,
test and admission stay separate.

### 10. Adding and removing: applies

Section 1 above gives the old and new contract of each removed or relaxed
safeguard. Section 4 below names each addition.

### 11. Changing this standard: not applicable

Nothing here changes the normative set.

### Debt

| Id | Rule | Location | Owner | Remedy | Review trigger |
|---|---|---|---|---|---|
| D1 | 2: a change with no plan goal; no ceiling, reviewer, integrator, rollback | this card | Claudia (helm), Hugh to approve the plan | add a gates goal to SPINE.md (or a successor plan) and re-issue the card with the jobcard(5) fields, or withdraw it | before review is enrolled |
| D2 | 1: C99 for anything that reads an untrusted byte | foreign.awk, mk.awk, banned-lint | reviewer, then integrator | accept the tier-1 reading in section 3.1 above, or port the judging to C99 under pledge("stdio rpath") and unveil(root, "r") | review of this return |
| D3 | 3: a gate shows an infrastructure failure | want TIMEOUT; walk's link refusal; an unreadable file | integrator | controls made on the build host in a scratch tree: a link, a mode-000 file run as a non-root user, a CPU loop under a lowered limit | the first native run |
| D4 | 5: a shell component confined by a C99 launcher | all five scripts | Claudia (helm) | a launcher card: unveil the root read-only and the gates directory, pledge "stdio rpath proc exec", fixed environment; or the owner's decision that check helpers run only inside the supervised lane | before admission |
| D5 | 6: checks bound each case | walk | integrator | a limit on file count and size in walk, or a wall-clock bound in the lane | first tree larger than the kit |

## 4. What I added, and why

- GATE-FIXTURES (format, walk:51-80): the frozen STACK-LAW-FIXTURES under
  a name that is not tied to a retired tool, with a stricter grammar: no
  inline comments, no `.` step, no trailing slash, no link. Needed so the
  gates can run over a tree that holds their own red controls.
- walk (interface): the walk the three gates share, so its refusals live
  in one place. New refusals: names outside `A-Z a-z 0-9 . _ + -`, a root
  starting with `-`, symbolic links and special files (the frozen tools
  skipped links silently).
- want (interface: `want [-l law] status command...`): replaces three
  copies of a selftest `t()` function. New: `-l`, so a red control must go
  red at its named law (the frozen shell selftests checked only the exit
  status); the CPU limit; the named result kinds.
- Laws PYTHON, FLAGS, CC, WEAKEN, NOCHECK per Makefile, and MALLOC outside
  house.mk: each carries a rule of STANDARDS 1 or 4 that the frozen tools
  held only for house.mk or not at all. FLAGS and CC replace the
  house.mk-specific NO-FLAGS.
- `make check` (Makefile): the card asks for it.
- Dependencies: none beyond the base system: sh, awk, find, grep, sort,
  dirname, printf, ulimit.
- No state, no authority: the gates write nothing and hold nothing.

## 5. What stays UNKNOWN until the native runs

Everything a run must show, on WHEATFIELD in the supervised lane:

- `make check` in gates/: every one of its 53 cases ok, and its exit
  status. In particular whether OpenBSD awk accepts every regular
  expression in foreign.awk and mk.awk, whether `ulimit -t` is available
  in sh, and whether `dirname --` and `find ... \( ... \) -prune` behave
  as written.
- That the controls go red for the reason named and no other (the `-l`
  checks).
- `mandoc -Tlint -W style -Ios=ELSEWHERE gates/gates.7` clean; the page
  rendered, indexed by makewhatis(8) and found with `man -k gates`; its
  three EXAMPLES run and their output, error and status matching the
  page.
- How the gates judge the kit as it stands. By reading only (REVIEW, not
  PASS): route/Makefile of SPINE-3 has no check target, so mk-lint should
  report NOCHECK there; its CFLAGS line carries the strict set.

## 6. Line counts (wc -l)

Counted with wc -l on the sealed bytes. SHA256SUMS is written after this
file and is not counted here.

| File | Lines |
|---|---:|
| RETURN.md | 303 |
| controls/banned/clean/main.c | 13 |
| controls/banned/red/main.c | 8 |
| controls/foreign/c99-make/Makefile | 4 |
| controls/foreign/c99-shell/run.sh | 3 |
| controls/foreign/clean/Makefile | 10 |
| controls/foreign/clean/hash.sh | 4 |
| controls/foreign/clean/ok.c | 12 |
| controls/foreign/no-git-file/.gitignore | 1 |
| controls/foreign/no-git-run/version.sh | 4 |
| controls/foreign/no-gnu-c/attr.c | 4 |
| controls/foreign/no-gnu-make/Makefile | 6 |
| controls/foreign/no-gnu-tool/hash.sh | 3 |
| controls/foreign/no-linux/poll.c | 4 |
| controls/foreign/posix-sh-array/run.sh | 3 |
| controls/foreign/posix-sh/run.sh | 2 |
| controls/foreign/python-run/run.sh | 3 |
| controls/foreign/python/tool | 4 |
| controls/mk/backslash-red/Makefile | 4 |
| controls/mk/baddeleg-red/Makefile | 4 |
| controls/mk/cc-red/Makefile | 4 |
| controls/mk/defaulted-red/Makefile | 5 |
| controls/mk/delegate-green/Makefile | 4 |
| controls/mk/empty-red/Makefile | 5 |
| controls/mk/flags-red/Makefile | 5 |
| controls/mk/good/Makefile | 9 |
| controls/mk/include-green/Makefile | 2 |
| controls/mk/include-green/check.mk | 2 |
| controls/mk/include-missing/Makefile | 4 |
| controls/mk/malloc-red/Makefile | 3 |
| controls/mk/noflags-red/Makefile | 5 |
| controls/mk/none-red/Makefile | 5 |
| controls/mk/noop-red/Makefile | 6 |
| controls/mk/semicolon-red/Makefile | 2 |
| controls/mk/var-unknown/Makefile | 5 |
| controls/mk/vardeleg-unknown/Makefile | 5 |
| controls/mk/varinclude-unknown/Makefile | 3 |
| controls/mk/weaken-red/Makefile | 5 |
| controls/walk/bad-name/a b.sh | 2 |
| controls/walk/fixtures-bad/GATE-FIXTURES | 2 |
| controls/walk/fixtures-bad/src/ok.sh | 2 |
| controls/walk/fixtures-good/GATE-FIXTURES | 2 |
| controls/walk/fixtures-good/Makefile | 3 |
| controls/walk/fixtures-good/controls/Makefile | 1 |
| controls/walk/fixtures-good/controls/poll.c | 3 |
| controls/walk/fixtures-good/ok.c | 7 |
| controls/walk/nothing/README.txt | 1 |
| gates/Makefile | 65 |
| gates/banned-lint | 66 |
| gates/foreign-lint | 73 |
| gates/foreign.awk | 142 |
| gates/gates.7 | 414 |
| gates/mk-lint | 162 |
| gates/mk.awk | 179 |
| gates/walk | 98 |
| gates/want | 79 |
