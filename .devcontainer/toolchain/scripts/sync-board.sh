#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

BOARD=board/friendlyarm/nanopi-neo-plus2

rsync -a --progress /workspace/config/$BOARD/ /buildroot/$BOARD