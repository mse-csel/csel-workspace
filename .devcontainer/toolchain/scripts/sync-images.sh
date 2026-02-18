#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

rsync -rlt --progress --delete /buildroot/output/images/ /workspace/buildroot-images