# W05 keys-input: wscons decode, text and house input

- **Proposed goal:** WEFT-1. **Class:** rewrite (port). **Tier:** c99:
  it decodes raw wscons event records.
- **Ceiling:** 3,300 lines in weft/keys (adds about 1,600). The estimate:
  - weft_wskbd 540, weft_text 193, weft_house_input 253 and
    weft_key_read 94 make 1,080; times 1.15 gives about 1,240;
  - regress is about 350: the input rows from ctl/help/test_actions_input
    and the bindings house rows not taken in W04.

## Goal
Port the decoding of `struct wscons_event` records into keys, held-key
state, typed text and house-input intents.

## Inputs (frozen)
- rm/component/profile/weft_wskbd.c, weft_wskbd.h, weft_text.c,
  weft_text.h, weft_house_input.c, weft_house_input.h
- rm/component/core/weft_key_read.c, weft_key_read.h
- ctl/help/test_actions_input.c and ctl/help/events.c: the event writer,
  used as a regress fixture

## Outputs (kit)
weft/keys/weft_wskbd.h weft/keys/weft_wskbd.c weft/keys/weft_text.h
weft/keys/weft_text.c weft/keys/weft_house_input.h
weft/keys/weft_house_input.c weft/keys/weft_key_read.h
weft/keys/weft_key_read.c weft/keys/Makefile weft/keys/regress/Makefile
weft/keys/regress/input_test.c weft/keys/regress/events.c RETURN.md

## Deletions
- `weft_key_read.c` moves from rm/component/core to keys, because its
  only dependency is weft_wskbd. The functions do not change.

## Safeguards that must survive
- A held key never becomes fresh typing.
- Menu typing never reaches an app.
- Recovery preempts (test_actions_input.c:3-14).
- Unsupported keys are refused, never guessed.
- Reads are bounded per batch.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Recorded event sequences (from events.c) give the expected intents.
3. A truncated event record is refused.
4. Red control: a sequence with a held key, run against a regress-only
   build that drops the repeat filter, FAILs.

## Rollback
Revert. weft/keys/Makefile and regress/Makefile return to their W04 blobs.
