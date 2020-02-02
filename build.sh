#!/usr/bin/env bash

set -Eeuo pipefail

function info() {
    echo -e "\e[38;5;14m$*\e[0m"
}

function create_if_not() {
    buildah inspect --type container --format '{{.Container}}' "$1" || buildah from --name "$1" "$2"
}

info "prepare..."
WORK=$(create_if_not gongt-init-build alpine-cn)
MNT=$(buildah mount $WORK)
RESULT=$(create_if_not gongt-init-result alpine)

info "install packages..."
buildah run $WORK apk add cmake gcc g++ make musl-dev linux-headers

info "copy source files..."
buildah copy $WORK . /opt

info "run make..."
echo "
cd /opt
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
make --no-print-directory
" | buildah run $WORK sh

info "copy compiled result..."
TMP=/tmp/init-compile-result
cp "$MNT/opt/build/init" "$TMP"
buildah copy $RESULT "$TMP" /sbin/init


info "update settings..."
buildah config --cmd '/sbin/init' --stop-signal=SIGINT "$RESULT"
buildah config --author "GongT <admin@gongt.me>" --created-by "GongT" --label name=gongt/alpine-init "$RESULT"

info "commit..."
buildah commit "$RESULT" gongt/alpine-init
info "Done!"
