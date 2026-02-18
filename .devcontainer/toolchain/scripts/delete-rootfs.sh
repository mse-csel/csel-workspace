#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

find /rootfs/ -maxdepth 1 -mindepth 1 -exec rm -rf -- {} +
