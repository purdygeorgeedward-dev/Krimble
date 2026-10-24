#!/bin/bash
# Builds and runs the bench for the Type tool window. Needs Qt5 (qtbase5-dev). Usage: build.sh [width] [height]. Pictures: /tmp/textdlg_port_tab0.png ... (or _land_ when a height is given)
set -e
cd "$(dirname "$0")"
D=../../plugins/tools/svgtexttool
mkdir -p /tmp/textdlg_build && cd /tmp/textdlg_build
INC="-I$(realpath "$OLDPWD")/$D -I$(realpath "$OLDPWD") $(pkg-config --cflags Qt5Widgets Qt5Gui Qt5Core)"
g++ -std=c++17 -fPIC -Wall -O1 $INC "$(realpath "$OLDPWD")/$D/KrimbleTextDialog.cpp" "$(realpath "$OLDPWD")/main.cpp" $(pkg-config --libs Qt5Widgets Qt5Gui Qt5Core) -o textdlg_bench
QT_QPA_PLATFORM=offscreen ./textdlg_bench "${1:-400}" ${2:-}
