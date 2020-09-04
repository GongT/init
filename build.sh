#!/usr/bin/env bash

set -Eeuo pipefail

function info() {
    echo -e "\e[38;5;14m$*\e[0m"
}

function create_if_not() {
    buildah inspect --type container --format '{{.Container}}' "$1" 2>/dev/null || buildah from --name "$1" "$2"
}

info "prepare..."
WORK=$(create_if_not gongt-init-build gongt/alpine)
MNT=$(buildah mount $WORK)
RESULT=$(create_if_not gongt-init-result gongt/alpine)

info "install packages..."
buildah run $WORK apk add cmake gcc g++ make musl-dev linux-headers
buildah run $RESULT apk --no-cache add libstdc++

info "copy source files..."
buildah copy $WORK . /opt

info "run make..."
echo "
cd /opt
mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
make --no-print-directory
" | buildah run $WORK sh

info "copy compiled result..."
TMP=/tmp/init-compile-result
cp "$MNT/opt/build/init" "$TMP"

buildah copy $RESULT "$TMP" /sbin/init

info "update settings..."
buildah config --entrypoint='["/bin/sh", "-c"]' --cmd '/sbin/init' --stop-signal=SIGINT "$RESULT"
buildah config --author "GongT <admin@gongt.me>" --label name=gongt/alpine-init "$RESULT"

info "commit..."
buildah commit "$RESULT" docker.io/gongt/alpine-init:latest
info "Done!"
