# RETURN: route(8), card SPINE-3-ROUTE-R2-CLOUD

Plan SPINE.md, goal SPINE-3. Standard edition 2026-10-13.v3, whose
NORMATIVE-SHA256SUMS has SHA256
c1016a7ff6fa9a26acb7a5dc74ab9db7bb5598dbc8d6f0a2d5dcd19508d8b8ec. Author:
this session (Claude on claude.ai/code, started by Claudia for Hugh).
Reviewer: named at integration. Integrator and owner of the seam: claudia.

## First: the line ceiling is not met

`wc -l route.c` is **748**. The ceiling is 600. This acceptance item fails,
and I am saying so here rather than widening the seam or squeezing the code.

Without its 66 comment lines and 67 blank lines, route.c would still be 615
lines. So no amount of trimming prose or whitespace reaches 600 in KNF. A
600-line route.c would have to drop a duty the brief lists, or break
style(9) by packing statements. I did neither. The owner's words that came
with the ceiling were "we don't remove or break or weaken load bearing shit
just to say we reduced the codebase", and the brief asks for readable code,
not squeezed code.

What each part costs (lines of route.c, comments and blanks included):

| Part | Lines | Where |
|---|---:|---|
| Header comment, includes, limits, alphabets, tables, globals | 61 | 1-61 |
| Operands and header values: usage, mkpath, inalpha, parsedec, hval, splitnames | 89 | 62-150 |
| State I/O: readfile, syncdir, writefile, jobstage, loadcard, seatjob, notdot, listjobs | 115 | 151-265 |
| Commit: one rename, directory fsyncs, test fault, one ledger line | 35 | 266-300 |
| `add`: bounded reads, staging, card(1) exec, header values, ustar names, return_max | 70 | 301-370 |
| `hold`, `release`, `status` | 54 | 371-424 |
| `serve`: request line, `next`, `delivered`, `returned` | 125 | 425-448, 556-656 |
| Return archive check: octal, iszero, checktar (ustar walk and SHA256SUMS) | 107 | 449-555 |
| Confinement and layout: confine (unveil, pledge), openstate | 41 | 657-697 |
| main: options, operand checks, dispatch | 51 | 698-748 |
| **Total** | **748** | |

The owner can choose one of these:

1. Admit a ceiling of 750 for route(8). Under standards section 2, that is
   an admission that names the old and new values (600, 750) and the
   safeguards that need the lines (the table above).
2. Revise the card to remove named duties. The parts most separable are
   the pre-lock read in `returned` (about 8 lines, and it keeps a slow
   upload from holding the lock) and the test-only `ROUTE_CONF` and
   `ROUTE_CARD` overrides (6 lines). Both together save 14 lines, which is
   far from 148.

Nothing else in this return is affected. All five files are complete.

## Input digests

All four spec files were recovered from this session's stored first
message, not retyped. Each matched its header with `sha256sum`:

| File | SHA256 | Result |
|---|---|---|
| spec/brief/ROUTE-BRIEF.md | 272b5fc788fa350f08765a066bc63bf771de5c3f1d3948f14eea70f1fb67d5ec | match |
| spec/kit/man/jobcard.5 | b68dd5522e43c024aad3212055705b7f39108131d743f19f605a0b678cdb315a | match |
| spec/kit/man/route.conf.5 | 1b2ef6e56192e3d3757b3721035b756592a7bc5c664658f7076e64460d1f4ec0 | match |
| spec/kit/policy/STANDARDS.md | aee7277c9b6c6b7e35b18b82cb4826674e628e326c62210f0f8406e5c2494eb1 | match |

## Requirements of the brief, with the lines that meet them

Line numbers are those of route.c unless a file is named.

| Requirement | route.c lines |
|---|---|
| One C99 program, no daemon or listener; sshd is the transport | whole file; no socket call; `serve` reads stdin, 617-656 |
| Seat known by its channel (`route serve SEAT`), never by the request | 730-731 (SEAT from argv), 232-243 (card seat must equal SEAT) |
| `add` reads the card once, at most 65536 bytes | 313-316 |
| Writes those bytes to a private staging file and runs `card -c CONF STAGED`, requiring exit 0 | 307, 319-339 |
| Reads route.conf once into memory and gives card(1) a private copy | 317-318, 323, 325 |
| Values only from the header, the first `key: "` line between the delimiters | 106-135 (hval), 344-350 |
| Takes job and outputs from the card and return_max from route.conf | 344-350 (seat is read from the stored card at 217-229) |
| The job keeps its return_max in its own directory | 365-367 (file `return_max`), 577-586 (used at return) |
| `add` refuses outputs that ustar cannot hold | 351-360 |
| State: stage directories, tmp/, ledger opened O_APPEND, lock | 43-46, 676-697 |
| Lock (flock LOCK_EX) around every transition and around status | 737, 644, 599 |
| route never creates the layout; a missing layout is exit 2 | 684-695 |
| Nothing is ever removed (only the private conf copy in staging is unlinked) | no unlink or rmdir of state; 340 is the staged conf copy |
| A job is a directory named by its job; `card` holds the accepted bytes | 322, 368 |
| One rename(2) per transition, then one ledger line | 282, 292-298 |
| Ledger line `SECONDS VERB JOB FROM TO [SHA256]`, one write(2) | 292-298 |
| Staging safe to redo after a kill | 319-323 (new mkdtemp each time), 178-198 (O_TRUNC rewrite) |
| fsync of each staged file and its directory before rename; destination after; ledger after write | 194-196, 284-287, 297 |
| `add`: commit tmp/ to queue/JOB; refuse an id in any stage | 361-363, 368 |
| `hold`: queue/, sent/ or delivered/ to held/, recording the stage first | 376-383 |
| `release`: held/ to queue/ only if held from queue/ | 393-401 |
| `status`: one line per job (stage, job, seat) in byte order; changes nothing | 404-424 |
| JOB operands match the job alphabet before any path is built | 39, 727-729, 641, 649, 262 |
| SIZE canonical decimal, at most return_max, checked before path or arithmetic | 93-105, 641-642, 585-586 |
| Request: one line, at most 128 bytes, ending in a newline | 627-640 |
| `next`: first job in byte order of name with this seat; commit to sent/, then `card JOB SHA256 SIZE` and the bytes; exit 3 and print nothing if none | 425-448, 250-265 (sorted by alphasort) |
| `delivered`: sent/ to delivered/; repeat for this seat's job in delivered/ or returned/ exits 0 | 649-655 |
| `returned`: SIZE at most return_max; read exactly SIZE bytes, nothing more | 575-594 |
| Check the archive, then commit delivered/ to returned/ with the archive inside | 595-614 |
| Repeat `returned`: same SHA256 exits 0; different SHA256 is refused | 601-607 |
| Anything else, another stage, another seat's job: refused, nothing changes | 232-243, 575-576, 646-655 |
| Nothing is resent | 425-448 (commit before send), 386-403 |
| Archive: ustar as OpenBSD tar writes it | 509 (magic and version) |
| Exactly the outputs plus top-level SHA256SUMS; regular files only | 491-496, 511-512, 519-534 |
| No name missing, repeated or not listed | 519-522, 532-534 |
| Every size within the archive | 513-514 |
| Every header checksum correct | 505-508 |
| SIZE a multiple of 512; zeros to SIZE after the last entry; at least two zero blocks | 497-498, 503, 530-531 |
| Links, directories, devices, FIFOs, pax headers, GNU long names refused | 511-512 (only type `0` or NUL passes) |
| SHA256SUMS lists exactly the outputs, never itself, as `HEX  NAME` lines | 537-553 |
| Each digest equals the SHA256 computed with sha2.h | 527, 549-550 |
| Archive stored byte for byte; its SHA256 goes into the ledger | 597, 610-612, 292-294 |
| unveil only what the verb needs, then lock unveil, then pledge | 657-675, called at 733 before any state or stdin read |
| serve, hold, release, status never exec; only add forks and execs card(1) | 670-672 |
| card(1) gets nothing else in its environment | 307, 331 |
| Exit 0, 1, 2, 3 | 42, used throughout; 440 (exit 3) |
| A diagnostic is one line on stderr starting "route: " | err(3) and errx(3) everywhere; 704 sets the prefix |
| On the seat channel, stdout carries only the protocol | 443-447 is the only stdout write in `serve` |
| Faults only in a separate test build | 24-26, 288-291, 705-710; Makefile `test-build` |
| No leases, budgets, reassignment, hand-off, reordering, daemon, JSON, Python, other shell-outs | none present |
| route.8 states what survives power loss, the kill gap, the fault build, ustar names, growth | route.8 CAVEATS |

## Choices made where the brief left one

1. **card(1) path**: `/usr/local/bin/card`, compiled in (route.c:57). The
   brief names card(1) but not its path.
2. **Per-job files**: `return_max` (canonical decimal and a newline) keeps
   the return_max; `held_from` (a stage name and a newline) records where a
   held job came from. `held_from` stays after a release, because nothing is
   removed. A later `hold` overwrites it.
3. **Staging for add**: `tmp/add.XXXXXXXXXX` from mkdtemp(3). It holds
   `card`, `conf` (unlinked once card(1) has judged it) and `return_max`,
   and the whole directory is renamed to `queue/JOB`. A refused or
   interrupted add leaves its staging directory in tmp/. route.8 CAVEATS says
   so; it is never read again.
4. **Staging for returned**: `return.tar` is written, with O_TRUNC, inside
   the job's directory in delivered/ before the rename. A copy left there by
   a kill is rewritten by the next return; route.8 FILES says so.
5. **status order**: the lines themselves are in byte order. So they sort
   by stage name and then by job. The space after a stage name sorts before
   every job byte, so this is also the job order within each stage.
6. **Ledger fields**: the FROM of `add` is `tmp`. The SHA256 is a sixth field,
   present only for `returned`.
7. **Directory fsyncs after the rename**: both the destination (as the brief
   says) and the source stage directory, so that the removal is on disk too.
8. **Fault trigger**: in the test build only, `ROUTE_FAULT=VERB` makes that
   verb SIGKILL itself after its rename and directory fsyncs, before its
   ledger line.
9. **Test fixtures**: in the test build only, `ROUTE_CONF` and `ROUTE_CARD`
   replace the two compiled paths, so the proofs need not touch
   /etc/route.conf on the build host. The installed binary reads no
   environment.
10. **returned and the lock**: the seat, stage, return_max and SIZE checks,
    the read of the archive and its check all happen before the lock. They
    read only what is fixed once a job is added (its card and return_max).
    Under the lock the stage is checked again, so a slow upload holds up no
    other channel. Cost: if the operator holds the job at the very moment
    its return is being checked, the unlocked card read can fail, which
    exits 2 and changes nothing.
11. **Exit codes at the edges**: an invalid SEAT operand exits 2, because it
    is a configuration error in authorized_keys, not a request. A card file
    over 65536 bytes exits 1, and an unreadable card file exits 2.
    route.conf unreadable or over 65536 bytes exits 2. If card(1) exits
    non-zero, add exits 1. If card(1) could not be run (exit 127, or killed
    by a signal), add exits 2.
12. **Same refusal for "no such job" and "another seat's job"**, so that one
    seat cannot probe the job names of another.
13. **SIZE syntax**: SIZE must be a canonical decimal of at most 8 digits and
    at most 67108864, the hard maximum of route.conf(5). This is checked
    while parsing the request, before any path is built. The job's own
    return_max is then checked before any archive byte is read.
14. **Numeric ustar fields**: octal digits, then only NULs and spaces, with
    no leading spaces, since OpenBSD tar zero-pads. The version must be
    `00`.
15. **SHA256SUMS line order**: any order is accepted. The brief fixes the
    line form, not the order.
16. **File mode**: route creates files with mode 0640, and the umask can
    narrow it further.
17. **The archive in memory**: one malloc(3) of SIZE bytes, at most the
    job's return_max (at most 64 MiB). It is freed at the end of `returned`,
    or released by exit on any failure.
18. **Diagnostic prefix**: `setprogname("route")`, so the test build also
    prints `route: `.
19. **No prototype block**: functions are defined before use and main comes
    last. This saves 33 lines. It departs from style(9); see the debt
    entries.

## STANDARDS, section by section

**1. Foundation: applies.** route.c is C99 (Makefile:5) and uses libc only,
with sha2.h, which is part of OpenBSD libc (route.c:7-26). It is built with
BSD make (Makefile) and fetches nothing. The tier is c99, because the
program holds state, locks, and validates untrusted bytes. Met in the
source; the native build is UNKNOWN.

**2. Work and seams: applies.** One card, one seat of paths (route/), class
new, goal SPINE-3. The ceiling is **not met** (748 against 600; see the top
of this file).
- Debt D1. Rule: section 2, line ceiling. Location: route/route.c (whole
  file). Owner: Hugh (owner), via claudia (integrator). Remedy: an admission
  raising the ceiling to 750 with the cost table above, or a revised card
  that drops named duties. Review trigger: integration review of this return.

**3. Evidence: applies.** Nothing here was built, run or linted, and no
PASS is claimed. Every native observation is UNKNOWN; see the list below.
The proofs, their red controls and receipts belong to the proof author and
the integrator.

**4. C: applies.**
- Flags: Makefile:5.
- Layout: tabs, at most 80 columns (checked with `expand | awk`), no
  trailing white space, a final newline, the return type above each
  function, every local function static.
- Order of work: acquire, validate, transform, publish (commit), release.
  See cmd_add 301-370 and serve_returned 556-616.
- Bounds before arithmetic: parsedec (93-105) bounds the length before
  strtoul. octal (449-467) reads at most 12 bytes, so it stays below 2^36.
  The entry size is compared with the bytes left before it is used
  (513-514), and the header value length with the buffer (127).
- Input is validated at its boundary, failing closed with a named reason:
  727-731 and 627-655.
- Partial state stays private until one commit point, the rename at 282.
- Recovery validates what it finds: stage entries at 260-262, stored seats
  at 224-228, stored return_max at 578-584, and held_from at 396-400.
- Ownership: card[] is static. The conf buffer is on the stack of cmd_add.
  The archive runs from malloc to free (588, 614). The scandir lists are
  released at exit (stated at 250).
- No reallocation. No secrets are held, so explicit_bzero does not apply.
- Debt D2. Rule: section 4, style(9), which wants prototypes for private
  functions at the top of the file. Location: route/route.c (whole file),
  which has no prototype block; functions are defined before use. Owner: claudia.
  Remedy: add the 33-line block if the ceiling admission allows it.
  Review trigger: the ceiling decision (D1). Closure evidence: the
  prototype block is present and the native build is clean.

**5. Authority and confinement: applies.**
- Per-verb unveil and pledge, locked before the first untrusted byte is
  read: confine at 657-675, called at 733, before openstate, any file read
  or stdin.
- A setup failure exits 2.
- Every descriptor route opens is O_CLOEXEC. card(1) gets an empty
  environment (307, 331), which also drops MALLOC_OPTIONS.
- route.8 Authority states what route protects and trusts, what it may
  touch, how a seat's authority is granted and revoked, and how it fails.
- No root artifact; route runs as the router's user and nothing is
  installed.
- Enforcement gaps, named:
  - Debt D3. Rule: section 5, every child and exec transition reviewed and
    confined. Location: route.c:670-672, where pledge is called with
    `execpromises` NULL, and 328-333. card(1) is not confined by route
    after exec: it runs with only its own pledge and unveil. route cannot
    choose its execpromises without knowing what card(1) and the dynamic
    loader need. card(1) also inherits route's standard input. Owner:
    claudia. Remedy: once card(1)'s promises are known, pass them as
    `execpromises` and give the child /dev/null on standard input. Review
    trigger: the review of card(1), or any change to its promises.
  - Debt D4. Rule: section 5, a control showing the restriction caused the
    refusal. Location: route.c:657-675. Owner: the proof author. Remedy:
    native proofs in which a verb attempts an access outside its unveil or
    pledge and is refused for that reason. Status: UNKNOWN.

**6. Shell and checks: applies to the Makefile only.** It has explicit BSD
make targets (`all`, `test-build`, `clean`), no install target and no
network. route.c contains no shell. The checks are the proof suite's, and
they are not in this seam.

**7. Manuals: applies.** route.8 is semantic mdoc with `.Os ELSEWHERE`, the
true date and genuine attribution (AUTHORS). It has the sections the card
asks for, in mandoc's order, plus AUTHORS. Every exit status in the page is
one the code returns. The lint, rendering, `man -k` and the run of its
examples are UNKNOWN until the build host does them.

**8. Custody and identity: applies.**
- Exact bytes are kept: the card as accepted (322) and the archive byte for
  byte (610), with its SHA256 in the ledger.
- No identity is reused (361-363). An uncertain delivery is never resent:
  the commit to sent/ comes before the send (442-447), and `release` refuses
  a job that left the queue (396-400).
- There is one header grammar: card(1) judges it, and route reads values
  only from bytes card(1) accepted.
- Control characters in the archive's SHA256SUMS fail the strict line form.
- SHA256SUMS for this return is written last and never lists itself.
- Debt D5 (from the brief). Rule: section 8, a format refuses what cannot
  be carried. Location: kit/man/jobcard.5, `outputs`, which allows names
  that a ustar header cannot hold. Owner: claudia. Remedy: tighten the
  grammar in the next jobcard schema version. Review trigger: jobcard/2.
  Meanwhile `add` refuses such cards (route.c:351-360), and route.8 CAVEATS
  says so.

**9. Review and admission: applies to the process, not to these bytes.**
This return is a delivery, not an admission. The reviewer is named at
integration.

**10. Adding and removing: applies.** The additions are listed in the next
section. Nothing was removed, because there was no baseline.

**11. Changing this standard: not applicable.** This work proposes no
change to the standard.

## Additions, and why each is needed

| Addition | Kind | Why |
|---|---|---|
| Verbs `add`, `hold`, `release`, `status`, `serve`; the request protocol | interface | The brief |
| Option `-d dir` | option | The brief, for tests |
| Compiled paths /var/route, /etc/route.conf, /usr/local/bin/card | interface | The brief names the first two; card(1) needs a fixed path for unveil "x" |
| `tmp/add.XXXXXXXXXX` staging, with `card`, `conf`, `return_max` | state | The brief: a private staging file; the job's directory is built here and renamed |
| `return_max` file in each job | state | The brief (N1): the job keeps its return_max |
| `held_from` file in each held job | state | The brief: record the stage before holding |
| `return.tar` in each returned job | state | The brief |
| Ledger line format, with `tmp` as the FROM of add | format | The brief, with one name chosen |
| `ROUTE_FAULT` (test build only) | interface | The brief requires the faults; an environment variable triggers them without a new option |
| `ROUTE_CONF`, `ROUTE_CARD` (test build only) | interface | Lets the proofs use fixture files without editing /etc on the build host |
| pledge `stdio rpath flock` (status); `stdio rpath wpath cpath flock` (hold, release, serve); plus `proc exec` (add) | authority | rpath for reads and directory fsyncs; wpath for files and the ledger; cpath for mkdtemp, rename and unlink of the staged conf; flock for the lock; proc and exec to run card(1) |
| unveil of the state dir (`r` for status, `rwc` otherwise); for add, FILE `r`, route.conf `r`, card(1) `x` | authority | What each verb touches, and no more |
| sha2.h (libc), scandir(3) and alphasort(3), err(3), setprogname(3) | dependency | All in OpenBSD libc; no other dependency |
| Exit 127 from the child means card(1) could not be run | convention | Separates "could not run" (2) from "refused" (1) |

## What stays unproved until the native proofs run

Nothing was compiled, run or linted in this sandbox, as the card requires.
Every item below is **UNKNOWN**:

- `make` and `make test-build` build clean with -std=c99 -Wall -Wextra
  -Wpedantic -Werror on the OpenBSD build host. These constructs are the
  most likely to draw a warning: `__dead` on a static definition, the
  scandir and alphasort prototypes, string literals in `char *argv[]`,
  `%.*s` with an `int` precision, and implicit size conversions in
  readfile and writefile.
- `mandoc -Tlint -W style` with `-Ios=ELSEWHERE` is clean on route.8.
- `wc -l route.c` is at most 600. Known: it is 748, so this fails.
- Each proof in the brief: deterministic replay without the time column; a
  kill at each transition leaving consistent state (route-test,
  ROUTE_FAULT); no card going out twice; exactly one of two concurrent
  claims winning; each channel getting its own first job and nothing else
  (`next` exit 3; `delivered` and `returned` exit 1); a header-shaped body
  line changing nothing; a bad JOB and a non-canonical SIZE refused before
  any path; a repeated `delivered`, and a repeated `returned` with the same
  archive, exiting 0; each return hazard refused; an archive padded to a
  whole record accepted; a card with a ustar-unholdable name refused at
  `add`.
- That OpenBSD `tar -cf -` output passes checktar unchanged: the magic and
  version, the checksum field format, and no pax extended headers for
  ordinary names.
- That unveil "x" on card(1) alone is enough to exec it when it is
  dynamically linked.
- The pledge and unveil controls (debt D4).
- What survives power loss, beyond what the code orders.

## Line counts (`wc -l`)

| File | Lines |
|---|---:|
| route.c | 748 |
| route.8 | 626 |
| Makefile | 20 |
| RETURN.md | 356 |
| SHA256SUMS | 4 |
