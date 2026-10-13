# W20 driver-kms: fixed KMS records and the DRM ABI

- **Proposed goal:** WEFT-4. **Class:** rewrite (port). **Tier:** c99.
- **Precondition:** D3. This card assumes option 1, a trimmed ABI
  header, with the retained-foreign-material rule returned through
  STANDARDS 11.
- **Ceiling:** 1,100 lines in weft/driver. The estimate:
  - wd_kms_openbsd.c and .h are 461 lines, about 530 after reflow;
  - the trimmed `wd_kms_abi.h` keeps the 11 `DRM_IOCTL_MODE_*` ioctls,
    the 10 structs and the MIT notices, about 150;
  - regress: test_kms is 407 lines.

## Goal
Port the fixed-record KMS layer: resources, the CRTC, the connector, the
encoder, dumb buffers and fb.

## Inputs (frozen)
- ns/base-driver/kms/wd_kms_openbsd.c, wd_kms_openbsd.h
- ns/base-driver/kms/abi/drm.h, drm_mode.h (2,758 lines, xenocara
  libdrm, MIT; already OpenBSD-flavoured, with no linux includes)
- ns/base-driver/tests/test_kms.c, ns/base-driver/PROVENANCE.md
  (origins)

## Outputs (kit)
weft/driver/Makefile weft/driver/wd_kms.h weft/driver/wd_kms.c
weft/driver/wd_kms_abi.h weft/driver/regress/Makefile
weft/driver/regress/kms_test.c RETURN.md

## Deletions
- About 2,600 lines of drm.h and drm_mode.h that the code never names.
  The kept declarations are byte-identical to their source, with the
  notices intact. RETURN.md lists every kept symbol and the source line.

## Safeguards that must survive
- Fixed-value KMS records are compared field by field on restore.
- Indexed capability masks compare referenced IDs.
- Uncertain allocation IDs are diagnostic hints and never authorise ID
  removal (base-driver/README.md).

## Acceptance (on the build host)
1. Strict build.
2. Regress: every ioctl is substituted by fixtures, so no device is
   opened. All test_kms rows pass.
3. Red control: one mutation (a restore field skipped) is detected,
   from the frozen seven-mutation set in the r1 evidence.
4. A symbol cross-check compares each kept struct's size and each ioctl
   number against the build host's installed xbase `drm_mode.h`, if one
   is present. If none is present, this is UNKNOWN.

## Rollback
Revert the one commit.
