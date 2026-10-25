#!/bin/bash
# Builds and draws the Krimble Log window offscreen. Needs qtbase5-dev. Usage: build_log.sh OUT.png WIDTH HEIGHT krita.log ["extra text"]
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/log_build/stub && cd /tmp/log_build
cp "$R/tools/textdlg-bench/klocalizedstring.h" stub/
g++ -std=c++17 -fPIC -Wall -O1 -Istub -I"$R/libs/ui" $(pkg-config --cflags Qt5Widgets Qt5Gui Qt5Core) "$R/libs/ui/KrimbleLogDialog.cpp" "$R/tools/log-bench/main.cpp" $(pkg-config --libs Qt5Widgets Qt5Gui Qt5Core) -o log_dialog
QT_QPA_PLATFORM=offscreen ./log_dialog "$@"
