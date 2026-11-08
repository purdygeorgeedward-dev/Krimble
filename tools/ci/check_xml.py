#!/usr/bin/env python3
"""Krimble 2026-11-08: George: "Yes, do an XML check each time."

Parses every menu layout (.xmlgui), form (.ui) and resource list (.qrc) file of the repository as XML and fails (exit 1) if any is invalid.
Why: on 2026-11-07 a file reformatted by a tool and edited on GitHub (krita/krita5.xmlgui, commits 4ab027e and 011ea89) was not valid XML twice; the program would have started
with no menus, and nothing in the build said so. Run it before every push of such a file and before every build (the build workflow runs it first):

    python3 tools/ci/check_xml.py            # all files
    python3 tools/ci/check_xml.py krita/krita5.xmlgui   # only the files named

It prints one line per bad file (path, line and column of the problem) and, inside GitHub Actions, the same as ::error annotations.
"""
import os
import subprocess
import sys
import xml.dom.minidom as minidom

EXTENSIONS = (".xmlgui", ".ui", ".qrc", ".action")   # 2026-11-08: the .action files (tool and menu action texts) too
SKIP_DIRS = {".git", "node_modules", "_build", "_install", "build"}

def candidates(root):
    for folder, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for name in files:
            if name.endswith(EXTENSIONS):
                yield os.path.join(folder, name)

def main(argv):
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    files = argv[1:] if len(argv) > 1 else sorted(candidates(root))
    bad = []
    for path in files:
        try:
            with open(path, "rb") as handle:
                minidom.parseString(handle.read())
        except Exception as error:   # xml.parsers.expat.ExpatError and read errors
            bad.append((path, str(error)))
    for path, error in bad:
        rel = os.path.relpath(path, root)
        print("INVALID XML: %s: %s" % (rel, error))
        if os.environ.get("GITHUB_ACTIONS"):
            print("::error file=%s,title=Invalid XML::%s" % (rel, error))
    print("%d file(s) checked, %d invalid" % (len(files), len(bad)))
    return 1 if bad else 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
