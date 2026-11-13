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

# 1. no "Photoshop" in the public text (README.md, KRIMBLE_CHANGES.md); code comments are allowed
for name in ("README.md", "KRIMBLE_CHANGES.md"):
    text = read(name)
    if text is None:
        continue
    count = len(re.findall(r"photoshop", text, re.I))
    if count:
        problems.append("%s has the word 'Photoshop' %d time(s) (the rule: say 'industry standard')" % (name, count))

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

for line in problems:
    print("DECISION BROKEN:", line)
print("all checkable decisions hold" if not problems else "%d problem(s)" % len(problems))
sys.exit(1 if problems else 0)
