#!/bin/bash
# Builds and runs the animation test. Needs qtbase5-dev and libwebp-dev. Usage: build_anim_test.sh OUTDIR sprite.gif sprite.webp
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/batch_build && cd /tmp/batch_build
g++ -std=c++17 -fPIC -Wall -O1 -DKRIMBLE_HAVE_WEBP -I"$R/libs/ui" $(pkg-config --cflags Qt5Core Qt5Gui) "$R/libs/ui/KrimbleBatch.cpp" "$R/tools/batch-bench/anim_test.cpp" $(pkg-config --libs Qt5Core Qt5Gui) -lwebp -lwebpmux -lwebpdemux -o anim_test
QT_QPA_PLATFORM=offscreen ./anim_test "$@"
