#!/bin/bash
# Builds and runs the Batch Process window offscreen. Needs qtbase5-dev and libwebp-dev. Usage: build_dialog.sh OUTDIR WIDTH HEIGHT [pictures...]
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/batch_build/stub && cd /tmp/batch_build
cp "$R/tools/textdlg-bench/klocalizedstring.h" stub/
g++ -std=c++17 -fPIC -Wall -O1 -DKRIMBLE_HAVE_WEBP -Istub -I"$R/libs/ui" $(pkg-config --cflags Qt5Widgets Qt5Gui Qt5Core) "$R/libs/ui/KrimbleBatch.cpp" "$R/libs/ui/KrimbleBatchDialog.cpp" "$R/tools/batch-bench/main.cpp" $(pkg-config --libs Qt5Widgets Qt5Gui Qt5Core) -lwebp -o batch_dialog
QT_QPA_PLATFORM=offscreen ./batch_dialog "$@"
