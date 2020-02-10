#!/usr/bin/env bash

set -Eeuo pipefail

function info() {
    echo -e "\e[38;5;14m$*\e[0m"
}

function create_if_not() {
    buildah inspect --type container --format '{{.Container}}' "$1" || buildah from --name "$1" "$2"
}

info "prepare..."
WORK=$(create_if_not gongt-init-build gongt/alpine-cn)
MNT=$(buildah mount $WORK)
RESULT=$(create_if_not gongt-init-result alpine)
RESULT_CN=$(create_if_not gongt-init-result-cn gongt/alpine-cn)

info "install packages..."
buildah run $WORK apk add cmake gcc g++ make musl-dev linux-headers
buildah run $RESULT apk --no-cache add libstdc++
buildah run $RESULT_CN apk --no-cache add libstdc++

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
buildah copy $RESULT_CN "$TMP" /sbin/init

info "update settings..."
buildah config --cmd '/sbin/init' --stop-signal=SIGINT "$RESULT"
buildah config --author "GongT <admin@gongt.me>" --created-by "GongT" --label name=gongt/alpine-init "$RESULT"

buildah config --cmd '/sbin/init' --stop-signal=SIGINT "$RESULT_CN"
buildah config --author "GongT <admin@gongt.me>" --created-by "GongT" --label name=gongt/alpine-init "$RESULT_CN"

info "commit..."
buildah commit "$RESULT" gongt/alpine-init
buildah commit "$RESULT_CN" gongt/alpine-init:cn
info "Done!"
