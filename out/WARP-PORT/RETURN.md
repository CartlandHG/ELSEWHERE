# RETURN: card WARP-PORT, warp's sidecar tools ported to C

Standard: edition `2026-10-13.v3`; its NORMATIVE-SHA256SUMS has SHA256
`c1016a7ff6fa9a26acb7a5dc74ab9db7bb5598dbc8d6f0a2d5dcd19508d8b8ec`.
ADDRESSING.md is `f7307c644387d9779a59d6bb2a60f8c39d66d15132150bfa3f904cfc4e04aeb0`,
as that manifest lists it.
Inputs: frozen-src-75ad5ce4.tgz
(`8cf5a408d1fc34f1204a7a70414ca7b389e33b7de677bf15941a04b339708480`) and
kit-2d6c0739.tgz
(`5fccc65f23a7cbe7b1f2ee39357572d592fff4fa6275cb8f90c94304c3254cdd`). Both digests matched the card, and every file in both
trees matched its MANIFEST (`sha256sum -c`, no failures).
Author: Claude (claude.ai/code), run as a scheduled cloud job. Reviewer,
integrator and rollback: not named by the card; see debt D1.

Nothing here has been compiled, run or linted. Every result that a build or
test must show is UNKNOWN (section 5).

**What came back.** The three Python programs are replaced by one C99
program, `sidecar`, with two verbs. `check` validates a `weft-byte-range/1`
sidecar exactly as ADDRESSING.md defines it. `select` resolves a sidecar
content-only against a file the caller names. It verifies size and SHA256
over one private snapshot, then serves the range from that same snapshot.
No Python survives. The warp object store, transclusion pointers, per-seat
log, envelopes and row_sign are cut. Their reasons are in section 1.

## 1. Deletions, with reasons

### 1a. Whole programs and verbs removed

| Frozen | Lines | Why it goes | What of it survives |
|---|---:|---|---|
| warp.py `put`, object layout `objects/xx/NAME`, `.tmp` + `os.replace` | 87-105 | ADDRESSING.md says it "introduces no store". A resolver's store authority belongs to an admitted resolver profile, which the card does not ask for. STANDARDS' introduction also makes a transclusion resolver a new kind of component that first brings its own rule through section 11. Nothing in the kit consumes the store. Porting a writable store would also bring the persistent-format proofs of STANDARDS 3 (reopen, truncation, failed writes, recovery). That is outside this card. | The store's read rule. `cat` refused a missing object loudly and refused bytes that did not hash to their name. Both survive in `select`: unavailable is exit 2 (sidecar.c:109, 123), and a digest mismatch is exit 3 (sidecar.c:341-344). The rule "never overwrite a corrupt object" has no writer left to bind. |
| warp.py `cat` | 108-115 | It is folded into `select`. The caller names the content file; a name grants nothing, and the bytes are accepted only by digest and size. | Its refusals, as above. |
| warp.py `ref`/`deref`, the JSON pointer `warp:transclude/1` | 79, 119-134 | A JSON transclusion pointer is the "executable transclusion language" or transclusion resolver that ADDRESSING.md and the STANDARDS introduction keep out until they are admitted. A sidecar now plays the pointer's role, as a description that names content by digest. | `deref`'s "MISSING stays loud, never empty". A missing source is exit 2 with nothing on stdout. Only a verified range gives exit 0 (tests `missing-content`, `empty-at-eof`). |
| warp.py per-seat chained log (`_append_log`, `_log_head`, `audit_log`, `log`) | 137-204, 672-674 | It is the audit trail of the store's writes, and with no store it has nothing to record. It also took the seat from `ELSEWHERE_SEAT`, which is self-declared, and it was JSON. | Nothing. Its tamper-evidence claim (an altered or truncated chain is detected; this is not tamper-proof) is recorded here for the store's own card. |
| warp.py `verify` | 207-221 | It re-hashes a store that is no longer ported. | The re-hash-before-trust rule, in `select`. |
| warp.py `measure` | 224-273 | A read-only research count of THE-LIBRARY's duplication. It hardcodes site paths (`commons/PROJECTS/the-mesh/...`), and SITE.md says programs hardcode no site values. Research stays outside kit inputs (STANDARDS 1). | Nothing. |
| warp.py envelopes (`seal`, `open_envelope`, `_canon`, `ENVELOPE_V`) | 401-503 | ADDRESSING.md's native provenance is a pair, document ID and immutable revision ID. Verifying it needs an admitted revision-record format. The frozen JSON envelope is not that format, and porting its canonical JSON to C would add a JSON writer and parser that nothing in the kit needs. | The rule "with provenance, failure never downgrades to content-only". `select` refuses every `docid`/`revision_sha256` sidecar (sidecar.c:314-316; test `provenance-no-downgrade`). |
| warp.py `co_resident_seats` | 507-546 | It measured whether one uid could read other seats' private keys under THE-LIBRARY's seat rooms. That is a site path and a property of the old deployment. Its only consumer was `open_envelope`. | Nothing. Its finding stands in the record: on one uid, a seal does not prove a seat. |
| warp.py `selfcheck`, `selfcheck_envelope`, argparse `main` | 276-361, 549-685 | These are replaced by tests/run.sh and by the two-verb `main`. | The five store controls fall with the store. The control idea (plant the fault, then require the red) is kept in every refusal case. |
| row_sign.py (all) and row_sign.py.md | 1-262, 1-149 | Its consumers were envelope `seal`/`open` and sidecar's R9 `signature` key. Neither survives (above, and 1b item 12). ADDRESSING.md: "Neither part grants authority"; authenticated provenance "has separate duties". It also locates keys by a site path (`commons/PROJECTS/the-mesh/seats`). A port would be base-tier /bin/sh over ssh-keygen -Y, but with no consumer in the kit it would be a seam with no load. | Its laws, recorded here for whatever card brings signing back: a missing roster is UNCHECKED and never a pass; an un-rostered key is SELF-SEALED and never ENROLLED; a seat signs only with its own room's key. |
| sidecar.py `index DIR` and R10 (`transcluded_by` reverse index) | 266-304 | ADDRESSING.md: "new association schemas … remain separate designs". The schema has no `transcludes` or `transcluded_by` key. | The rule "judged nothing is UNKNOWN, never PASS". `sidecar` takes exactly one sidecar, and with no operand it exits 6 (usage), never 0. |
| sidecar.py `validate FILE...` taking many files | 439-452 | One file per run gives one result class per exit status. Callers loop in the shell. | The validation itself, as `check`. |
| sidecar.py `--selftest` sandbox | 307-431 | It is replaced by tests/run.sh: fixed fixtures, a red control for each refusal, a mutant and an infrastructure failure. | — |
| frozen Makefile include of `mk/house.mk` | warp/Makefile 1-2 | The card asks for a self-contained Makefile. | The flags and `MALLOC_OPTIONS=S` for tests, as Makefile:6 and :17. |
| README | 1-29 | Its content is now in sidecar(1) and this record. | — |

### 1b. Frozen behaviour replaced because ADDRESSING.md disagrees

The card says ADDRESSING.md wins. Each disagreement:

1. **Key set.** The frozen keys were `id type title owner created
   bytes_path bytes_sha256 status provenance_parents`, plus optional
   `bytes_warp license envelope docs tests transcludes transcluded_by
   provenance_got signature provenance_source`. ADDRESSING.md has
   `schema content_sha256 content_size offset length`, plus `docid` and
   `revision_sha256` together. Every frozen key is now unknown and
   refused.
2. **Unknown keys.** The frozen parser accepted any key matching
   `[a-z][a-z0-9_]*` (sidecar.py:94). ADDRESSING.md refuses every key
   outside its table.
3. **Quoting.** The frozen values were unquoted, `key: value`.
   ADDRESSING.md requires `key: "value"` and refuses unquoted values.
4. **Size limits.** The frozen reader had none. ADDRESSING.md sets 1024
   bytes for the header and 1 MiB for the file.
5. **CR.** `CANON`'s `.*` let a CR stay at the end of a frozen header line.
   ADDRESSING.md refuses CR in the header.
6. **Body.** The frozen reader never checked the body. A non-UTF-8 file
   raised a Python exception, which was not a named refusal.
   ADDRESSING.md refuses invalid UTF-8 and NUL.
7. **Locating bytes.** The frozen `bytes_path` was resolved relative to
   the sidecar (sidecar.py:195), so location chose which bytes were
   judged. ADDRESSING.md: "A pathname … cannot substitute for either
   digest". The sidecar carries no path, and the caller names the content.
8. **Size and range.** The frozen identity was the whole-file digest
   alone, with no size and no range. ADDRESSING.md binds the complete size
   too, with a half-open range checked before any addition and never
   clamped.
9. **Store.** The frozen R3 also required `bytes_warp` to read back clean
   from the warp store. There is no store now (1a).
10. **Clock.** The frozen `created` stamp was compared with the clock, and
    a future stamp was refused (sidecar.py:168-187). ADDRESSING.md has no
    such key. The comparison also made validation depend on the clock,
    which STANDARDS 6 forbids for checks.
11. **Enums.** The frozen `type` and `status` enums (R7, R8) have no key
    now.
12. **Signature marker.** The frozen `signature: sealed` marker (R9) was a
    schema marker that looked like authority. ADDRESSING.md: neither part
    grants authority.
13. **Provenance.** The frozen `provenance_parents` named warp envelopes
    and required each to open INTACT (R6). ADDRESSING.md's provenance is
    the `docid` + `revision_sha256` pair against a native revision record.
    This profile cannot verify it, so it refuses it (1a, envelopes).
14. **Reverse index.** The frozen `transcluded_by` reverse index (R10) has
    no counterpart (1a).
15. **Exit statuses.** The frozen statuses were 0/1/2,
    PASS/FAIL/UNKNOWN. ADDRESSING.md requires the result to distinguish
    malformed or unsupported, unavailable, identity mismatch, out of
    bounds or resource limit, I/O failure, and success. Those are exits
    1-5 and 0, and 6 is usage or setup failure (sidecar.c:31-39,
    sidecar(1) EXIT STATUS).
16. **Naming and placement.** The frozen sidecar(5) required the name
    `ARTIFACT.sidecar.md` and placement beside the artifact.
    ADDRESSING.md requires neither, and P5 says location never changes
    identity. The new page names no file convention.
17. **Purpose.** The frozen sidecar(5) said every program, library and
    gate carries a sidecar. ADDRESSING.md's sidecar describes one byte
    range and makes no such demand.

Where the two agreed, the rule is kept: duplicate keys are refused
(sidecar.py:117), the opening line is exactly `---`, a missing closing
delimiter is refused, trailing white space and tabs are refused, and the
header has no blank lines.

**An interpretation for the reviewer.** ADDRESSING.md names only invalid
UTF-8 and NUL for the body. STANDARDS 8 says a format "refuses control
characters". I applied both. The body allows tab and newline, and refuses
other C0 characters, DEL and C1, as jobcard(5) does for its body
(sidecar.c:157-195; sidecar(5) Body). A CR in a body is therefore refused.
If the reviewer reads STANDARDS 8 as not reaching the body, three lines of
`textok` change.

## 2. The frozen files and lines this started from

From src.git 75ad5ce4, `warp/`, with MANIFEST digests:

| File | Lines | SHA256 | Used for |
|---|---:|---|---|
| warp/sidecar.py | 460 | `7b6adb2c8815b51a39e36630231ad742d52124cbefdc6748e4f31bc0c5247006` | header parse 99-121 → `parse`/`entry`; required keys 151-159 → `check`; digest agreement 199-222 → `selectrange`; exit and "nothing judged" rules 29-32, 266-304; controls 307-431 → tests/run.sh |
| warp/warp.py | 685 | `396117e4e760b5db0c9bc43f7b44b09a45ce74cfeafe0863d4597091a058b076` | `cat` 108-115 (MISSING loud, CORRUPT refused) → `select`; the rest cut (1a) |
| warp/row_sign.py | 262 | `48b553b5ec4e3221abdfb6dc4e559e782beeefaf935881cfba8454c0208b8124` | cut (1a); its laws are recorded |
| warp/row_sign.py.md | 149 | `d2fc2266d72a44683838d20ba26a0c55dca962d17622f54cb2555243db4fede3` | read; cut with row_sign |
| warp/sidecar.1 | 80 | `740a2228369a238a126aca5bb7dd6fe2aeca3a2bce71045154efbc25b1d63167` | structure of the new sidecar(1) (EXIT STATUS, controls) |
| warp/sidecar.5 | 115 | `8423cf1062a7a70f36106b1abc1376ea67b0f5aad54180c039396022cdf67638` | replaced by the weft-byte-range/1 page |
| warp/tests/run.sh | 150 | `2a0faf713994d892508b2bfe53815c8b8067d6419e5c31cd7ab8bfbae6225152` | harness idioms: `mktemp -d`, trap cleanup, named FAIL lines, exit status |
| warp/Makefile | 12 | `d7fc714f02c19da29b6e3fca296c7d856a8b18fabe61a76ce2da19fd5293546e` | flags, `MALLOC_OPTIONS=S` for tests |
| warp/README | 29 | `f89c233eea1a802dfd5c4cc537e6f780e3b9ffcddad3706427de1e5522a53f0e` | read |

Also read: ADDRESSING.md (all), jobcard(5) for the shared header grammar
and its body rule, and route/ on this branch's base for house C style.

**Consumers reconciled (STANDARDS 10).** In the frozen tree,
`tools/gates/asset-origin-lint` runs `python3 -I -B warp/sidecar.py
validate` on asset sidecars (lines 25 and 128). Nine `*.sidecar.md` files
use the old schema. Neither the gate nor those files is in the kit. If they
enter it, they need a schema of their own, a new version with its own key
table (ADDRESSING.md: "Extending the schema requires a new version"), and
not this one. Debt D6.

## 3. STANDARDS, section by section

| § | Applies | How it is met | Debt |
|---|---|---|---|
| 1 Foundation | yes | C99, BSD make and base tools only (Makefile; tests/run.sh uses sh, printf, cmp, grep, sed, mktemp, dd, tr, sha256, chmod, id). No Python: no lower-tier failure was found, so there is no reason to keep any. C is the tier because the program reads untrusted bytes. Nothing is fetched. | — |
| 2 Work and seams | yes | One purpose, one component. Ceiling proposed in section 6. | **D1** |
| 3 Evidence | yes | A red control for every refusal rule. Each refusal case checks the status, an empty stdout and the named reason on stderr, so a different refusal does not count (run.sh:29-63). A standing wrong implementation, `sidecar-mutant` (sidecar.c:317-323, Makefile:14-15), must be caught by all three bounds cases (run.sh `mutant-clamp-detected`). An infrastructure failure gives UNKNOWN and exit 2 (run.sh `infrastructure-failure-is-unknown`). UNKNOWN is counted and shown, and a FAIL beats it in the aggregate (run.sh, last lines). No PASS is claimed here. | **D2**, **D3** |
| 4 C | yes | style(9) layout, 80 columns (checked with `expand -t8`), static functions, return type above the name. The order is acquire (open), confine (pledge), validate (`parse`, `check`), transform and publish (`selectrange`). Sizes are bounded before arithmetic: decimal overflow is checked digit by digit (sidecar.c:143), and the range is compared before any addition (:325-326). Fail closed with a named reason (`refuse`, :63-78). Nothing goes to stdout before verification (:336-345). The ownership of `buf` is stated (:335) and it is freed (:353); every refusal exits the process. No secrets are held: content is data, not credentials, so explicit_bzero does not apply. | build evidence UNKNOWN (section 5) |
| 5 Authority | yes | `pledge("stdio rpath")` is the first act (:364). The operands are opened, then `pledge("stdio")` (:377) runs before the first byte is read. A pledge failure exits 6, so setup failure refuses the work. unveil(2) is not used: once only "stdio" remains, no path operation is possible, and before that the only path operations are the open(2) calls on the operands (:370-376). A FIFO cannot block the open (O_NONBLOCK, :109). The SECURITY CONSIDERATIONS section of sidecar(1) states what it protects, trusts and touches, and how it fails. No child, no exec, no privilege, nothing delegated. | **D2** |
| 6 Shell and checks | yes | Expansions quoted. Fixed inputs, `LC_ALL=C TZ=UTC`, `umask 077`, no network, no clock. Untrusted data passes only as arguments and files, never as shell source. No function shadows a ksh alias (`t sc raw bounds num bad body`). Builds use explicit make targets. There is no install target. | — |
| 7 Manuals | yes | sidecar(1) and sidecar(5) in mdoc, `.Os ELSEWHERE`, dated October 13, 2026, with attribution under AUTHORS. sidecar(1) has SYNOPSIS, verbs, EXIT STATUS, EXAMPLES, SEE ALSO, CAVEATS and SECURITY CONSIDERATIONS. sidecar(5) has the grammar and version, every field with its alphabet, the refusal rules and an example. The commands in the example are run by run.sh as explicit commands: `select-bc` and `pinned-after-edit` check the page's output, its message and status 3. The page is not executed. | lint, makewhatis and `man -k` UNKNOWN |
| 8 Custody | yes | Unknown and duplicate keys are refused, and so are control characters, NUL and truncation where it is detectable: an unclosed header, size disagreement, a cut UTF-8 sequence. SHA256SUMS is written last, uses relative names and does not list itself. Identity is separate from location (1b items 7 and 16). | — |
| 9 Review and admission | not this return's to meet | A return is not a review or an admission. The states delivery, review, test, integration and acceptance stay separate. | — |
| 10 Adding and removing | yes | Additions in section 4, deletions in section 1 with the surviving safeguard for each, and consumers reconciled in section 2. | **D6** |
| 11 Changing the standard | not applicable | No policy file is changed. A byte-range resolver is governed by ADDRESSING.md, which is already normative. sidecar(1) names its profile: no store, no revision format, a capacity of 64 MiB, and the failure contract. | — |

### Debt

| Id | Rule | Location | Owner | Consequence | Remedy | Review trigger | Closing evidence |
|---|---|---|---|---|---|---|---|
| D1 | STANDARDS 2: a change comes from a card that cites a plan goal and names a reviewer, an integrator, a ceiling and a rollback. | this card | integrator; Hugh for the plan | SPINE.md has no warp goal and does not list warp under "What waits", so this return cannot be admitted as it stands. | Add a goal to the plan, or revise the card under an existing goal with a reviewer, a ceiling (section 6 proposes one) and a rollback (delete `warp/`, since nothing depends on it). | before review is enrolled | a revised card that cites a goal |
| D2 | STANDARDS 5: a control shows that a restriction caused the refusal. | sidecar.c:364-378 | integrator | The pledge boundary is argued, not observed. | A fault build that calls open(2) after `pledge("stdio")` and is killed (SIGABRT), with the same build without the pledge succeeding. | admission; any change to `main` | the native observation |
| D3 | ADDRESSING.md control "mutation between lookup, verification and selection". | sidecar.c:297-354 | integrator | Concurrent mutation is not exercised. Static fixtures reach the same checks: content longer, shorter or same-size-tampered gives exit 3 (`longer-content`, `wrong-size`, `tampered-content`). Safety against a write after the read holds by construction, because the range is served from the verified buffer. This is REVIEW, not PASS. | A fault build that rewrites the file between the size check and the read. | any change to `selectrange` | the native observation |
| D4 | ADDRESSING.md: native provenance is resolved, and a docid that does not match is refused. | sidecar.c:314-316 | Hugh (scope) | This profile refuses every provenance pair, so "a valid revision of another docid" is never reached. It is never downgraded either. | A resolver profile with an admitted revision-record format, on its own card. | when a native document store is admitted | that profile's controls |
| D5 | STANDARDS 3: every case is observed natively. | tests/run.sh `withheld-content` | integrator | Run as root, mode 000 does not withhold, and the case reports UNKNOWN. | Run the tests unprivileged in the supervised lane. | every native run | that case's PASS in the receipt |
| D6 | STANDARDS 10: reconcile consumers before deleting. | frozen tools/gates/asset-origin-lint; 9 old-schema sidecars | integrator | The old schema has no validator in the kit. | Give those files their own schema version if they are proposed for the kit. | asset-origin-lint or asset sidecars proposed for the kit | a card for that schema |

## 4. Everything added, and why

- **Program `sidecar`** (C99): the card's port target.
- **Interface: `check FILE`, `select FILE CONTENT`.** The first is
  ADDRESSING.md's reader. The second is its resolver, with the
  content-only profile.
- **Exit statuses 0-6.** ADDRESSING.md requires the result classes to be
  distinguished, and 6 keeps usage and setup failure apart from them.
- **Limits:** 1024-byte header and 1 MiB sidecar, both from ADDRESSING.md;
  64 MiB content capacity (`CONTENT_MAX`, sidecar.c:25). ADDRESSING.md
  requires a profile to declare its capacity. Holding the whole content
  in memory is what makes the served bytes the verified bytes, and 64 MiB
  bounds that on a small machine. A larger capacity is a one-line
  admission.
- **Build option `-DSIDECAR_MUTANT`** (Makefile target `sidecar-mutant`):
  the standing wrong implementation that ADDRESSING.md and STANDARDS 3
  require. It is never built by `all`.
- **No persistent state, no environment variable, no configuration file,
  no new dependency.** The C library's sha2.h, pledge(2) and err(3) are
  all base.
- **Authority:** none beyond reading the two operands. It is reduced to
  stdio before any byte is read.

## 5. UNKNOWN until the native runs

- `make`, `make sidecar-mutant` with `-std=c99 -Wall -Wextra -Wpedantic
  -Werror` on WHEATFIELD (OpenBSD 8.0): the actual compiler invocation,
  diagnostics and output hashes.
- `make test` under `MALLOC_OPTIONS=S`: each of the 86 named cases, the
  three mutant detections, the infrastructure case and the aggregate
  status reaching make.
- `make lint`: `mandoc -Tlint -W style -Ios=ELSEWHERE sidecar.1 sidecar.5`,
  then makewhatis(8) and `man -k sidecar` on staged pages.
- The behaviour of OpenBSD's sh `printf` octal escapes in fixtures, and of
  `sha256 -q`, `dd` and `tr` as the script assumes.
- The pledge abort behaviour (D2), concurrent mutation (D3) and the
  withheld case when run as root (D5).
- Nothing is PASS.

## 6. Line counts and proposed ceilings

`wc -l` of every file in this return (SHA256SUMS excluded; it is written
after this file):

```
  390 warp/sidecar.c
   26 warp/Makefile
  172 warp/sidecar.1
  174 warp/sidecar.5
  325 warp/tests/run.sh
```

RETURN.md is 256 lines; the number was filled in after the last edit,
in place, which leaves the count unchanged.

**Proposed ceiling for sidecar(1): 400 lines of sidecar.c**, about 10 lines
of margin. What each part costs, with comments and blanks (35 blank lines
and about 52 comment lines in all; about 303 lines of code):

| Part | Lines | sidecar.c |
|---|---:|---|
| File comment, includes, limits, result classes, key table, header struct, sidecar buffer | 54 | 1-54 |
| `usage`, `refuse` (one named refusal) | 24 | 55-78 |
| Input: `readfull`, `openfile`, `regular` (regular file, no FIFO block) | 47 | 79-125 |
| Value alphabets: `decimal` (overflow digit by digit), `hex64` | 31 | 126-156 |
| Body: `textok` (RFC 3629 UTF-8 and control characters) | 39 | 157-195 |
| Header grammar: `entry`, `parse` (exact lines, 1024 bytes, NUL) | 63 | 196-258 |
| Key table and alphabets: `check` | 38 | 259-296 |
| Resolution: provenance refusal, mutant (7), bounds, capacity, size, snapshot read, digest, write | 58 | 297-354 |
| `main`: pledge, operands, open, pledge, read, dispatch | 36 | 355-390 |

The parts that could be cut are each a safeguard: `textok` (39), which the
body rule needs; the FIFO and regular-file check (13), without which a
FIFO operand would hang; and the mutant (7), which STANDARDS 3 requires.
None should go to meet a smaller number.

**tests/run.sh: proposed ceiling 340.** Its 325 lines hold 86 named cases,
each one or two lines. The harness itself is about 70 lines.

The Makefile and manuals are not code. They are bounded by their content.
