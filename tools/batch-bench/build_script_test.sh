#!/bin/bash
# Builds and runs the Krimble Script test. Needs qtbase5-dev and libwebp-dev.
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/batch_build && cd /tmp/batch_build
g++ -std=c++17 -fPIC -Wall -O1 -DKRIMBLE_HAVE_WEBP -I"$R/libs/ui" $(pkg-config --cflags Qt5Core Qt5Gui) "$R/libs/ui/KrimbleBatch.cpp" "$R/tools/batch-bench/script_test.cpp" $(pkg-config --libs Qt5Core Qt5Gui) -lwebp -lwebpmux -lwebpdemux -o script_test
QT_QPA_PLATFORM=offscreen ./script_test
