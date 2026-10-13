# Port cards for weft

There is one card per port step, in the order PLAN.md section 7 gives.
These are drafts. A card becomes dispatchable only when four things are
set:
- the owner has added its proposed goal to the plan (PLAN.md D1);
- the integrator has filled the jobcard(5) header: job, seat, author,
  reviewer (not the author), plan_sha256 and standard_sha256;
- its owner decisions are settled;
- the cards before it in its component are admitted.

Common to every card:
- **Frozen root:** frozen-src-75ad5ce4 (src.git 75ad5ce4). "ns/" means
  `weft/native-source/`, "rm/" means `weft/native-source/room-motion/`,
  "ctl/" means `weft/native-source/controls/`.
- **Build:** BSD make with `weft/Makefile.inc` (W01): `-std=c99 -O2 -Wall
  -Wextra -Wpedantic -Werror`. `make check` runs `regress/` under
  `MALLOC_OPTIONS=S`. There is no network, no wall clock, a fixed `LC_ALL=C`
  and no fetch.
- **Style:** style(9). Port the frozen logic; do not redesign it. A
  presentation reflow is part of the rewrite class and carries no
  semantic change beyond the deletions listed.
- **Ceiling:** wc -l over every file in the component except manuals.
  The card's ceiling is the component's running total after the card.
- **Outputs:** at most 16 names, RETURN.md included, as jobcard(5)
  requires.
- **Return:** RETURN.md puts deletions first, then the frozen inputs
  read, the section-by-section standard account, the additions, what
  stays UNKNOWN and wc -l. SHA256SUMS is written last.
- **Acceptance:** observed on the build host under the supervised lane,
  and only there. Until then everything is UNKNOWN.
- **Rollback (all cards):** the change is one got commit adding new kit
  paths. Rollback is `got revert` or a reverting commit of that one
  commit. Nothing is installed, so nothing on the host changes. A card
  that edits an earlier component names that file's previous blob.
