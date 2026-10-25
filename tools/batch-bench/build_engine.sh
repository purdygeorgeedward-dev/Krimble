#!/bin/bash
# Builds and runs the engine test of Batch Process. Needs qtbase5-dev and libwebp-dev. Usage: build_engine.sh OUTDIR pictures...
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/batch_build && cd /tmp/batch_build
g++ -std=c++17 -fPIC -Wall -O1 -DKRIMBLE_HAVE_WEBP -I"$R/libs/ui" $(pkg-config --cflags Qt5Core Qt5Gui) "$R/libs/ui/KrimbleBatch.cpp" "$R/tools/batch-bench/engine_test.cpp" $(pkg-config --libs Qt5Core Qt5Gui) -lwebp -o engine_test
QT_QPA_PLATFORM=offscreen ./engine_test "$@"
