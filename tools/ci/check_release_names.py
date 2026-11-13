#!/usr/bin/env python3
"""Krimble 2026-11-13: George: "Are we not putting the v before the version number?" / "Why the lack of consistency?" / "Do it again."

The names of a release must follow one rule (written in BUILD_ANDROID.md, section 7):
  tag     v1.0.N-beta
  title   Krimble v1.0.N-beta
(the APK file name is made by the workflow from the label, Krimble-1.0.N-beta-<date>-<time>.apk, and has no v.)

Before creating a release:   python3 tools/ci/check_release_names.py --tag v1.0.58-beta --title "Krimble v1.0.58-beta"
To check all the releases:   GITHUB_PAT=<token> python3 tools/ci/check_release_names.py          (reads them from GitHub with the token; without one GitHub refused it in a test, HTTP 403)
Exit 1 if a name is wrong.
"""
import json
import os
import re
import sys
import urllib.request

REPO = "purdygeorgeedward-dev/Krimble"

def problems(tag, title):
    found = []
    m = re.fullmatch(r"v1\.0\.(\d+)-beta", tag)
    if not m:
        found.append("tag %r is not v1.0.N-beta" % tag)
        return found
    want = "Krimble v1.0.%s-beta" % m.group(1)
    if title != want:
        found.append("title %r should be %r" % (title, want))
    return found

def main(argv):
    if "--tag" in argv:
        tag = argv[argv.index("--tag") + 1]
        title = argv[argv.index("--title") + 1] if "--title" in argv else ""
        found = problems(tag, title)
        for line in found:
            print(line)
        if not found:
            print("names are right: %s / %s" % (tag, title))
        return 1 if found else 0
    bad = 0
    page = 1
    while True:
        request = urllib.request.Request("https://api.github.com/repos/%s/releases?per_page=100&page=%d" % (REPO, page))
        if os.environ.get("GITHUB_PAT"):
            request.add_header("Authorization", "Bearer " + os.environ["GITHUB_PAT"])
        with urllib.request.urlopen(request) as response:
            releases = json.load(response)
        if not releases:
            break
        for release in releases:
            for line in problems(release["tag_name"], release["name"] or ""):
                print("%s: %s" % (release["tag_name"], line))
                if os.environ.get("GITHUB_ACTIONS"):   # Krimble 2026-11-13: a warning annotation on the run's page (an allowed-to-fail step shows as green)
                    print("::warning title=Krimble release name::%s: %s" % (release["tag_name"], line))
                bad += 1
        page += 1
    print("all release names follow the rule" if not bad else "%d problem(s)" % bad)
    return 1 if bad else 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))
