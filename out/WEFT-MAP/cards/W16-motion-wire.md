# W16 motion-wire: frames, writer, stream and the WNPOSE01 codec

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99:
  it encodes and decodes a binary wire.
- **Ceiling:** 1,500 lines in weft/motion. The estimate:
  - wf_frame, wf_writer, wf_stream, wf_hash, weft_motion_codec and
    weft_motion_frame.h make 964 lines, about 1,110 after reflow;
  - regress is about 400: live/test_frame_io, stream/test_stream (the
    codec rows), and codec decode refusals.

## Goal
Port the WNFRAME1 and WNPOSE01 frame formats, the deadline-bounded
writer, and the stream reader.

## Inputs (frozen)
- rm/component/core/wf_frame.c, wf_frame.h, wf_writer.c, wf_writer.h,
  wf_stream_openbsd.c, wf_hash_openbsd.c
- rm/weft_motion_codec.c, rm/weft_motion_frame.h
- ctl/live/test_frame_io.c, ctl/stream/test_stream.c: the codec rows
- weft/weft-room-motion.1, "Wire" section

## Outputs (kit)
weft/motion/Makefile weft/motion/wf_frame.h weft/motion/wf_frame.c
weft/motion/wf_writer.h weft/motion/wf_writer.c weft/motion/wf_stream.c
weft/motion/wf_hash.c weft/motion/wmf_codec.h weft/motion/wmf_codec.c
weft/motion/weft-frame.5 weft/motion/regress/Makefile
weft/motion/regress/frame_test.c RETURN.md

## Deletions
- weft_motion_frame.h includes weft_touch_chrome.h and
  weft_room_bridge.h. Both includes go, along with the declarations
  that need them (touch and physics are deferred). Every other
  declaration moves unchanged into wmf_codec.h.
- `wf_hash_openbsd.c` (15 lines) is kept, but the reviewer checks
  whether lib's `wn_digest` serves instead. If it does, wf_hash.c is
  deleted in this card.

## Safeguards that must survive
- WMF_HEADER is 224 bytes. Pixels are bounded by WF_WIRE_MAX.
- Every pixel's alpha byte must be 0xff, or the frame is refused
  (weft_motion_codec.c:89).
- At most WMF_FRAMES_MAX = 4096 frames.
- Each frame write has a deadline, clamped to the total, with cancel
  during a partial write.
- Size is checked before arithmetic in `wmf_frame_size`.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. Encode and decode round-trip.
3. Truncated, oversize, bad-alpha and trailing frames are each refused,
   by name.
4. A writer under an injected stalled fd stops at its deadline with
   STOP=DEADLINE.
5. Red control: the alpha check removed by a regress-only `-D` FAILs
   its row.
6. weft-frame.5 (grammar, version, fields, refusals, example) is lint
   clean.

## Rollback
Revert the one commit.
