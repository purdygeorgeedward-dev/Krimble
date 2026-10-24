#!/bin/bash
# kb-release.sh - attaches the newest ~/Krimble-*.apk to the GitHub release of the same version, from the cloud server.
# Runs ON THE CLOUD SERVER. Usage:  ~/kb-release.sh            (attaches; the release stays a draft)
#                                   ~/kb-release.sh --publish  (attaches, then publishes the release)
# Needs the release to exist already (a draft is fine). The token is asked for (typed hidden, never saved) unless GITHUB_PAT is set.
# Krimble 2026-10-17: George: "It's always so much faster from the cloud server. Let's make that our standard process."
set -e
REPO="purdygeorgeedward-dev/krimble"
APK=$(ls -t "$HOME"/Krimble-*.apk 2>/dev/null | head -1)
if [ -z "$APK" ]; then echo "NO APK found in $HOME (run ~/kb-apk.sh first)"; exit 1; fi
NAME=$(basename "$APK")
# TAG=$(echo "$NAME" | sed -n 's/^Krimble-\(1\.0\.[0-9]*\)-beta.*/\1b/p')
# Krimble 2026-10-24: George: GitHub says a version tag traditionally starts with v, and a beta is -beta (the name the app shows), not b. New tags are v1.0.N-beta (the name of the
# APK file without its date and time, with a v in front); the old form 1.0.Nb is still accepted, so that releases made before 2026-10-24 can be found.
TAG=$(echo "$NAME" | sed -n 's/^Krimble-\(1\.0\.[0-9]*-beta\).*/v\1/p')
OLDTAG=$(echo "$NAME" | sed -n 's/^Krimble-\(1\.0\.[0-9]*\)-beta.*/\1b/p')
if [ -z "$TAG" ]; then echo "Cannot read the version from $NAME"; exit 1; fi
echo "APK: $NAME ($(du -h "$APK" | cut -f1))"
echo "RELEASE TAG: $TAG"
if [ -z "$GITHUB_PAT" ]; then
  read -r -s -p "Paste the GitHub token (typing is hidden), then Enter: " GITHUB_PAT
  echo
fi
AUTH="Authorization: Bearer $GITHUB_PAT"
ID=$(curl -s -H "$AUTH" -H "Accept: application/vnd.github+json" "https://api.github.com/repos/$REPO/releases?per_page=30" \
  | python3 -c "import sys,json
t='$TAG'
o='$OLDTAG'
for r in json.load(sys.stdin):
    if r['tag_name'] in (t, o): print(r['id']); break")
if [ -z "$ID" ]; then echo "NO release with tag $TAG (or $OLDTAG) found (check the token, or make the release first)"; exit 1; fi
echo "RELEASE ID: $ID"
# an asset with the same name is removed first, so the command can be run again
OLD=$(curl -s -H "$AUTH" "https://api.github.com/repos/$REPO/releases/$ID/assets" \
  | python3 -c "import sys,json
for a in json.load(sys.stdin):
    if a['name']=='$NAME': print(a['id'])")
if [ -n "$OLD" ]; then curl -s -X DELETE -H "$AUTH" "https://api.github.com/repos/$REPO/releases/assets/$OLD" > /dev/null; echo "Old copy removed"; fi
echo "Uploading (about a minute)..."
curl -s -X POST -H "$AUTH" -H "Content-Type: application/vnd.android.package-archive" --data-binary @"$APK" \
  "https://uploads.github.com/repos/$REPO/releases/$ID/assets?name=$NAME" \
  | python3 -c "import sys,json
r=json.load(sys.stdin)
print('UPLOADED:', r.get('name'), r.get('size'), 'bytes', r.get('state')) if r.get('name') else print('FAILED:', r)"
if [ "$1" = "--publish" ]; then
  curl -s -X PATCH -H "$AUTH" -H "Accept: application/vnd.github+json" "https://api.github.com/repos/$REPO/releases/$ID" -d '{"draft":false}' \
    | python3 -c "import sys,json
r=json.load(sys.stdin); print('PUBLISHED:' if not r.get('draft') else 'STILL A DRAFT:', r.get('html_url'))"
fi
unset GITHUB_PAT
