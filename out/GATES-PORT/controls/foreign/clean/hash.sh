#!/bin/sh
# Green control: the base sha256(1), not GNU sha256sum.
set -eu
sha256 -q "$1"
