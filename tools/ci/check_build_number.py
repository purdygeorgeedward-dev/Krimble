#!/usr/bin/env python3
"""Krimble 2026-11-08: George: "From now on follow strict renumbering. We don't always have to release a new build, but we should always change the number."

Checks, before a build starts:
  1. the number in packaging/android/apk/build.gradle (`def krimbleBuildNumber = N`) is higher than every number in KRIMBLE_BUILD_LOG.md (one row per successful build);
  2. if a label is given (the workflow's input, for example 1.0.55-beta), it is exactly 1.0.<N>-beta.
Exit 1 with a message if not. After a successful build, add its row to KRIMBLE_BUILD_LOG.md.

    python3 tools/ci/check_build_number.py [1.0.55-beta]
"""
import os
import re
import sys

root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

def current_number():
    text = open(os.path.join(root, "packaging", "android", "apk", "build.gradle")).read()
    found = [m for m in re.finditer(r"^\s*def\s+krimbleBuildNumber\s*=\s*(\d+)", text, re.M)]
    if len(found) != 1:
        print("build.gradle must have exactly one active `def krimbleBuildNumber = N` line (found %d)" % len(found))
        sys.exit(1)
    return int(found[0].group(1))

def logged_numbers():
    numbers = []
    for line in open(os.path.join(root, "KRIMBLE_BUILD_LOG.md")):
        m = re.match(r"^\|\s*(\d+)\s*\|", line)
        if m:
            numbers.append(int(m.group(1)))
    return numbers

def main(argv):
    n = current_number()
    used = logged_numbers()
    problems = []
    if used and n <= max(used):
        problems.append("build number %d is not higher than the last built number %d (KRIMBLE_BUILD_LOG.md): change krimbleBuildNumber in packaging/android/apk/build.gradle" % (n, max(used)))
    if len(argv) > 1 and argv[1] != "1.0.%d-beta" % n:
        problems.append("the label %r does not match the build number %d (it must be 1.0.%d-beta)" % (argv[1], n, n))
    for p in problems:
        print("BUILD NUMBER: " + p)
        if os.environ.get("GITHUB_ACTIONS"):
            print("::error title=Build number::" + p)
    if not problems:
        print("build number %d (1.0.%d-beta) is new; the last built number is %d" % (n, n, max(used) if used else 0))
    return 1 if problems else 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
