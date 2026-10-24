#!/usr/bin/env python3
"""
Krimble 2026-10-24: a quick syntax check of one Krita/Krimble source file, in about a minute, instead of a 2 hour build on the server.
It runs g++ -fsyntax-only on the file against Krita's own headers, with empty stand-ins for the headers CMake would generate (*_export.h, KoConfig.h, config-*.h).
It finds mistakes of syntax, names and types (not link errors, and not what a generated header would have defined).

Needs, on the machine where it runs (the sandbox has them after):
  apt-get install -y qtbase5-dev libkf5i18n-dev liblcms2-dev libboost-dev libeigen3-dev
  the headers of the libraries, from the repository: git checkout origin/master -- libs/flake libs/pigment libs/command libs/global libs/widgetutils \
      libs/koplugin libs/store libs/version libs/resources libs/basicflakes libs/image libs/color libs/psdutils libs/metadata libs/multiarch plugins/tools/svgtexttool
Use:  python3 tools/syntax-check/syntax_check.py plugins/tools/svgtexttool/KrimbleTextTool.cpp
Other plugins need their own folders added to DIRS below (and more libs may be needed: the script says which header is missing).
"""
import os, re, subprocess, sys

DIRS = ["libs/flake", "libs/flake/commands", "libs/flake/svg", "libs/flake/text", "libs/flake/tools", "libs/flake/resources", "libs/pigment", "libs/pigment/compositeops",
        "libs/pigment/resources", "libs/command", "libs/global", "libs/widgetutils", "libs/koplugin", "libs/store", "libs/version", "libs/resources", "libs/basicflakes"]
STUBS = "/tmp/krimble_stubinc"

def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    source = sys.argv[1]
    os.makedirs(STUBS, exist_ok=True)
    inc = ["-I" + STUBS] + ["-I" + d for d in DIRS + [os.path.dirname(source)] if os.path.isdir(d)]
    qt = subprocess.run(["pkg-config", "--cflags", "Qt5Widgets", "Qt5Gui", "Qt5Core", "Qt5Xml"], capture_output=True, text=True).stdout.split()
    cmd = ["g++", "-std=c++17", "-fsyntax-only", "-fno-operator-names", "-fPIC"] + inc + qt + ["-I/usr/include/KF5/KI18n", "-I/usr/include/KF5", "-I/usr/include/eigen3", source]
    for _ in range(60):
        out = subprocess.run(cmd, capture_output=True, text=True).stderr
        errors = [l for l in out.split("\n") if re.search(r"error|fatal", l)]
        if not errors:
            print("NO ERRORS in " + source); return 0
        m = re.search(r"fatal error: ([\w./+-]+): No such file", errors[0])
        if m:
            base = os.path.basename(m.group(1))
            if base.endswith("_export.h"):
                pre = base[:-len("_export.h")].upper()
                open(os.path.join(STUBS, base), "w").write("#pragma once\n#define %s_EXPORT\n#define %s_NO_EXPORT\n#define %s_DEPRECATED\n#define %s_DEPRECATED_EXPORT\n" % ((pre,) * 4))
                continue
            if re.match(r"(KoConfig\.h|config-.*\.h|.*_config\.h)$", base):
                open(os.path.join(STUBS, base), "w").write("#pragma once\n")
                continue
        print("\n".join(errors[:12]))
        return 1
    return 1

if __name__ == "__main__":
    sys.exit(main())
