#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

find /buildroot/ -maxdepth 1 -mindepth 1 -exec rm -rf -- {} +
