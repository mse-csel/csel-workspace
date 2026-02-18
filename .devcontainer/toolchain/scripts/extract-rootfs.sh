#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

tar xf /buildroot/output/images/rootfs.tar -C /rootfs/