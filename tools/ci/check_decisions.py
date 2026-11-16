#!/usr/bin/env python3
"""Krimble 2026-11-13: George: "I'm concerned about his decisions get made and then later ignored." Checks the decisions in KRIMBLE_DECISIONS.md that a program can check.
Prints a line per problem and a summary; the exit code is 1 if there is any (the workflow runs it as a warning, it does not stop a build).

    python3 tools/ci/check_decisions.py
"""
import os
import re
import sys

root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
problems = []

def read(path):
    try:
        with open(os.path.join(root, path), encoding="utf-8", errors="replace") as f:
            return f.read()
    except OSError:
        return None

# 1. the brand word (see BRAND below) is in none of the project's own text: no .md file anywhere in the repository may hold it, in any spelling
#    (George, 2026-11-16: remove any and all mentions; say "industry standard"). The word is put together here so that this file does not hold it either.
#    What may still hold it: identifiers and file-format constants in the code (MIME types, blending-mode ids, the JPEG and TIFF markers), which are not text for people.
BRAND = "photo" + "shop"
for dirpath, dirnames, filenames in os.walk(root):
    dirnames[:] = [d for d in dirnames if d not in (".git", "build", "node_modules")]
    for filename in filenames:
        if not filename.lower().endswith(".md"):
            continue
        name = os.path.relpath(os.path.join(dirpath, filename), root)
        text = read(name)
        if text is None:
            continue
        count = len(re.findall(BRAND, text, re.I))
        if count:
            problems.append("%s has the brand word %d time(s) (the rule: say 'industry standard')" % (name, count))

# 2. JNI class paths: org/krimble/android; the old org/krita/android must not be in the sources
old_path = "org/krita/android"
bad_files = []
for top in ("packaging", "libs", "krita", "plugins"):
    for folder, _, files in os.walk(os.path.join(root, top)):
        for filename in files:
            if filename.endswith((".java", ".cpp", ".cc", ".h", ".xml", ".gradle", ".kt")):
                path = os.path.join(folder, filename)
                try:
                    with open(path, encoding="utf-8", errors="replace") as f:
                        if old_path in f.read():
                            bad_files.append(os.path.relpath(path, root))
                except OSError:
                    pass
for path in bad_files[:10]:
    problems.append("%s still has the old JNI path %s (the rule: org/krimble/android)" % (path, old_path))

# Krimble 2026-11-13: on GitHub Actions a step that is allowed to fail shows as green, so every problem is also written as a warning annotation (it appears on the run's page)
for line in problems:
    print("DECISION BROKEN:", line)
    if os.environ.get("GITHUB_ACTIONS"):
        print("::warning title=Krimble decision broken::%s" % line)
print("all checkable decisions hold" if not problems else "%d problem(s)" % len(problems))
sys.exit(1 if problems else 0)
