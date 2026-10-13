# W15 pty-clients: the producer's client registry

- **Proposed goal:** WEFT-3. **Class:** rewrite (port). **Tier:** c99.
- **Ceiling:** 3,000 lines in weft/pty (adds about 650). The estimate:
  - wrm_clients.c and .h are 356 lines, about 410 after reflow;
  - regress: client-capacity is 244 lines.

## Goal
Port the registry that binds at most eight terminal clients to WINDOW
nodes for the producer.

## Inputs (frozen)
- rm/wrm_clients.c, wrm_clients.h
- ctl/client-capacity/test_client_capacity.c, Makefile
- ctl/root-join/test_client_cap.c, test_clients_join.c: the registry
  rows only

## Outputs (kit)
weft/pty/weft_clients.h weft/pty/weft_clients.c weft/pty/Makefile
weft/pty/regress/Makefile weft/pty/regress/clients_test.c RETURN.md

## Deletions
- The `wrm_` prefix, which belongs to the producer, becomes `wcl_`. The
  module moves beside the runner it owns, with the same functions.
- Real sleeps in test_client_capacity become an injected stamp.

## Safeguards that must survive
- The client directory must be absolute, and the registry refuses to run
  as root (wrm_clients.c:54).
- At most WRMC_MAX = 8 clients (:22, :75, :119).
- Serial overflow is refused (:128).
- A WN_VIEWPANE node is never chosen.
- An existing matching client is preserved.
- A refusal never loses custody.

## Acceptance (on the build host)
1. Strict build and check both hold.
2. The ninth client is refused with the others intact.
3. Red control: the capacity raised by a regress-only `-D` FAILs the
   ninth-client row.

## Rollback
Revert. The Makefiles return to their W14 blobs.
