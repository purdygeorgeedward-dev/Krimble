#!/usr/bin/env python3
"""
Krimble 2026-10-24: a quick syntax check of one Krita/Krimble source file, in about a minute, instead of a 2 hour build on the server.
It runs g++ -fsyntax-only on the file against Krita's own headers, with empty stand-ins for the headers CMake would generate (*_export.h, KoConfig.h, config-*.h).
It finds mistakes of syntax, names and types (not link errors, and not what a generated header would have defined).

Needs, on the machine where it runs (the sandbox has them after):
  apt-get install -y qtbase5-dev libkf5i18n-dev liblcms2-dev libboost-dev libeigen3-dev libkf5itemviews-dev libkf5widgetsaddons-dev libkf5configwidgets-dev libkf5config-dev libkf5coreaddons-dev libkf5guiaddons-dev libkf5iconthemes-dev
  the headers of the libraries, from the repository: git checkout origin/master -- libs/flake libs/pigment libs/command libs/global libs/widgetutils \
      libs/koplugin libs/store libs/version libs/resources libs/basicflakes libs/image libs/color libs/psdutils libs/metadata libs/multiarch plugins/tools/svgtexttool
Use:  python3 tools/syntax-check/syntax_check.py plugins/tools/svgtexttool/KrimbleTextTool.cpp
Other plugins need their own folders added to DIRS below (and more libs may be needed: the script says which header is missing).
"""
import glob, os, re, subprocess, sys

DIRS = ["libs/flake", "libs/flake/commands", "libs/flake/svg", "libs/flake/text", "libs/flake/tools", "libs/flake/resources", "libs/pigment", "libs/pigment/compositeops",
        "libs/pigment/resources", "libs/command", "libs/global", "libs/widgetutils", "libs/koplugin", "libs/store", "libs/version", "libs/resources", "libs/basicflakes"]
STUBS = "/tmp/krimble_stubinc"

def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    source = sys.argv[1]
    os.makedirs(STUBS, exist_ok=True)
    # every folder under libs/ and plugins/ that has headers (that is what the CMake files of Krita give as include folders, more or less)
    walked = [d for top in ("libs", "plugins") if os.path.isdir(top) for d, _, files in os.walk(top) if any(f.endswith(".h") for f in files) and "/tests" not in d]
    inc = ["-I" + STUBS, "-I."] + ["-I" + d for d in DIRS + [os.path.dirname(source)] + walked if os.path.isdir(d)]
    qt = subprocess.run(["pkg-config", "--cflags", "Qt5Widgets", "Qt5Gui", "Qt5Core", "Qt5Xml"], capture_output=True, text=True).stdout.split()
    cmd = ["g++", "-std=c++17", "-fsyntax-only", "-fno-operator-names", "-fPIC"] + inc + qt + ["-I/usr/include/KF5", "-I/usr/include/eigen3"] + ["-I" + d for d in sorted(glob.glob("/usr/include/KF5/*")) if os.path.isdir(d)] + [source]
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
            m2 = re.match(r"ui_(.*)\.h$", base)
            if m2:
                # a header that uic makes from a .ui file: found in the repository (fetched when it is not in the working copy) and made with uic
                name = m2.group(1).lower() + ".ui"
                found = [os.path.join(d, f) for d, _, fs in os.walk(".") for f in fs if f.lower() == name and ".git" not in d]
                if not found:
                    listing = subprocess.run(["git", "ls-tree", "-r", "--name-only", "origin/master"], capture_output=True, text=True).stdout.split("\n")
                    hit = [l for l in listing if l.lower().endswith("/" + name)]
                    if hit:
                        subprocess.run(["git", "checkout", "origin/master", "--", hit[0]], capture_output=True)
                        found = [hit[0]]
                uic = next((u for u in ("/usr/lib/qt5/bin/uic", "/usr/lib/x86_64-linux-gnu/qt5/bin/uic", "uic") if os.path.exists(u) or u == "uic"), "uic")
                if found and subprocess.run([uic, found[0], "-o", os.path.join(STUBS, base)], capture_output=True).returncode == 0:
                    continue
            if re.match(r"(KoConfig\.h|config-.*\.h|.*_config\.h|moc_.*\.(cpp|h)|.*\.moc)$", base):
                open(os.path.join(STUBS, base), "w").write("#pragma once\n")
                continue
        print("\n".join(errors[:12]))
        return 1
    return 1

if __name__ == "__main__":
    sys.exit(main())
