# W04 keys-map: key names, chords and the keymap file

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99:
  it parses the operator's keymap file, which is untrusted input.
- **Ceiling:** 1,700 lines in weft/keys. The estimate:
  - weft_key 716, weft_bindings 269 and keymap.conf 67 make 1,052;
    times 1.15 gives about 1,210;
  - regress is about 450: test_bindings, test_named_house and
    test_commands, minus their shared harness.

## Goal
Port the key and chord model and the keymap loader. Turn
CONTROL-SCHEMA.md into a section 5 manual for the keymap format.

## Inputs (frozen)
- rm/component/profile/weft_key.c, weft_key.h, weft_bindings.c,
  weft_bindings.h
- ns/keymap.conf, ns/CONTROL-SCHEMA.md
- ctl/bindings/test_bindings.c, test_named_house.c, test_commands.c,
  Makefile

## Outputs (kit)
weft/keys/Makefile weft/keys/weft_key.h weft/keys/weft_key.c
weft/keys/weft_bindings.h weft/keys/weft_bindings.c weft/keys/keymap.conf
weft/keys/weft-keymap.5 weft/keys/regress/Makefile
weft/keys/regress/bindings_test.c weft/keys/regress/commands_test.c
RETURN.md

## Deletions
- CONTROL-SCHEMA.md is not carried as prose. Its grammar and refusal
  rules go to weft-keymap.5.
- Seat banners in headers go to the manual's AUTHORS section.

## Safeguards that must survive
- A malformed or colliding map, or an explicit file that is missing, is
  refused, naming the line. It never falls back to the defaults
  (test_bindings.c:114-116).
- `wkb_load(NULL)` uses the compiled-in defaults and touches no file.
- A named action and its physical chord give the same intent.
- An unknown command is refused, and so is an absent observation.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Each refusal class names its line.
3. The shipped keymap.conf loads and equals the defaults' chord table.
4. Red control: a map with a duplicate chord is refused, and the same
   map with the collision check compiled out by a regress-only `-D` is
   accepted and FAILs the row.
5. weft-keymap.5 is lint clean, with its grammar, every field, the
   refusal rules and an example (STANDARDS 7).

## Rollback
Revert the one commit.
