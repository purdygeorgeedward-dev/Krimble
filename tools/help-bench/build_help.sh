#!/bin/bash
# Builds and runs the Krimble Help window offscreen. Needs qtbase5-dev. Usage: build_help.sh OUTDIR WIDTH HEIGHT [part to go to] [word to find]
set -e
R="$(cd "$(dirname "$0")/../.." && pwd)"
mkdir -p /tmp/help_build/stub && cd /tmp/help_build
cp "$R/tools/textdlg-bench/klocalizedstring.h" stub/
python3 "$R/tools/help/make_help.py" > /dev/null
OUT=$1; W=$2; H=$3; shift 3
g++ -std=c++17 -fPIC -Wall -O1 -Istub -I"$R/libs/ui" $(pkg-config --cflags Qt5Widgets Qt5Gui Qt5Core) "$R/libs/ui/KrimbleHelpDialog.cpp" "$R/tools/help-bench/main.cpp" $(pkg-config --libs Qt5Widgets Qt5Gui Qt5Core) -o help_dialog
QT_QPA_PLATFORM=offscreen ./help_dialog "$OUT" $W $H "$R/krita/data/aboutdata/krimble-help.md" "$@"
