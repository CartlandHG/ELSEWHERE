# W23 driver-adapter: the paced DRM adapter and parent-only abandonment

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 4,110 lines in weft/driver (adds about 1,350). The
  estimate:
  - wp_drm_adapter.c is 450 lines, about 520 after reflow;
  - regress is about 830: test_adapter 410, test_adapter_abandon 109,
    inherited_checks.inc 294 and the stream/test_abandon rows,
    de-duplicated; receipt_abi.c rewritten to assert.

## Goal
Port the adapter that begins presentation (open card0, dumb buffer, fb,
map), runs the display child, and restores or abandons.

## Inputs (frozen)
- ns/base-driver/wp_drm_adapter.c
- ns/base-driver/tests/test_adapter.c, test_adapter_abandon.c,
  inherited_checks.inc, receipt_abi.c, link_only.c
- ctl/stream/test_abandon.c

## Outputs (kit)
weft/driver/wp_adapter.c weft/driver/Makefile
weft/driver/regress/Makefile weft/driver/regress/adapter_test.c
weft/driver/regress/abandon_test.c weft/driver/regress/abi_test.c
weft/driver/regress/link_only.c RETURN.md

## Deletions
- inherited_checks.inc is a `.c` body included by its user. It is folded
  into adapter_test.c with all 185 assertions kept, and the count is
  stated.
- receipt_abi.c only prints the layout, so it can never fail
  (STANDARDS 3). It becomes abi_test.c, which asserts the sizes and
  offsets that callers rebuild against.

## Safeguards that must survive
- `adapter_abandon` marks the context abandoned, then calls munmap and
  close once each (wp_drm_adapter.c:331-345).
- Normal restore is refused on an abandoned context (:281, :306, :319).
- Normal recovery closes the painting descriptor, opens a fresh
  read-only one, and compares all recorded baseline fields.
- On an unknown-child return, ordinary paced observations are withheld.
- The production link opens no device (link_only.c).

## Added authority
Confinement for the display child is UNKNOWN. Whether OpenBSD's `drm`
pledge admits SETCRTC, CREATE_DUMB and ADDFB is to be observed. Two
outcomes:
- If it admits them, the child pledges `"stdio drm"` after setup.
- If it does not, the master ioctls run before pledge, and the gap is
  named as debt.

Either way the result is recorded, and the contract is not weakened to
fit the mechanism.

## Acceptance (on the build host)
1. Strict build.
2. Driver185 and abandon14 pass normally and with UB traps.
3. The ABI rows pass.
4. Red control: abandon run against a build that restores after
   abandonment is detected.
5. `ldd` on the link-only binary shows libc only.
6. The drm-pledge observation is recorded, with the device substituted.

## Rollback
Revert. The Makefiles return to their W22 blobs.
