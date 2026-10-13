# W06 keys-help: help strip, menu and command catalogue

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 4,400 lines in weft/keys (adds about 1,100). The estimate:
  - weft_help 170, weft_menu 178 and weft_catalogue 229 make 577; times
    1.15 gives about 665;
  - regress is about 430: test_catalogue, test_menu and
    test_menu_claudia merged into one suite, plus test_help_input (225),
    which fits inside the same estimate once the shared harness is
    removed.

## Goal
Port the help text, the typed command menu and the bounded command
catalogue.

## Inputs (frozen)
- rm/component/profile/weft_help.c, weft_help.h, weft_menu.c,
  weft_menu.h, weft_catalogue.c, weft_catalogue.h
- ctl/bindings/test_catalogue.c, test_menu.c, test_menu_claudia.c
- ctl/help/test_help_input.c: help modality, SAVE refused under help,
  alias and repeat custody, recovery

## Outputs (kit)
weft/keys/weft_help.h weft/keys/weft_help.c weft/keys/weft_menu.h
weft/keys/weft_menu.c weft/keys/weft_catalogue.h
weft/keys/weft_catalogue.c weft/keys/Makefile weft/keys/regress/Makefile
weft/keys/regress/menu_test.c weft/keys/regress/help_test.c RETURN.md

## Deletions
- test_menu_claudia.c is merged into menu_test.c as its boundary
  section. No assertion is dropped, and its second-author provenance is
  noted in a comment.
- Garden panel text in the help strings is deferred with the garden; the
  diff names the strings removed.

## Safeguards that must survive
- Catalogue limits: a 129-byte name is refused.
- Non-ASCII or unknown menu input is refused without mutation.
- An absent target is refused, using the confirmation wording.
- Help never overruns its in-source buffer: overflow is refused, and
  seed_session exits 2 on it.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Every refusal row holds with state unchanged, compared by memcmp of
   the before and after state.
3. Red control: a catalogue bound relaxed by a regress-only `-D` makes
   the 129-byte row FAIL.

## Rollback
Revert. The two Makefiles return to their W05 blobs.
