#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-or-later
# Inspect a built Krimble APK before uploading it to an app store.
# Usage: bash packaging/android/verify-apk.sh /path/to/krimble-release.apk
set -euo pipefail

if [[ $# -ne 1 || ! -f "$1" ]]; then
  echo "Usage: $0 /path/to/krimble-release.apk" >&2
  exit 2
fi
apk="$1"
command -v unzip >/dev/null || { echo "Missing unzip" >&2; exit 2; }
command -v aapt >/dev/null || { echo "Missing aapt (Android SDK build-tools)" >&2; exit 2; }
command -v apksigner >/dev/null || { echo "Missing apksigner (Android SDK build-tools)" >&2; exit 2; }

echo "Checking archive integrity..."
unzip -tqq "$apk"

echo "Reading package metadata..."
badging="$(aapt dump badging "$apk")"
package_line="$(printf '%s\n' "$badging" | grep '^package: ' | head -n 1)"
sdk_line="$(printf '%s\n' "$badging" | grep '^sdkVersion:' | head -n 1)"
target_line="$(printf '%s\n' "$badging" | grep '^targetSdkVersion:' | head -n 1)"
printf '%s\n' "$package_line" "$sdk_line" "$target_line"

[[ "$package_line" == *"name='org.krimble'"* ]] || {
  echo "ERROR: Unexpected package ID" >&2; exit 1;
}
[[ "$target_line" == *"'36'"* ]] || {
  echo "ERROR: Expected targetSdkVersion 36" >&2; exit 1;
}

echo "Checking arm64 library payload..."
if ! unzip -Z1 "$apk" | grep -qE '^lib/arm64-v8a/.*\.so$'; then
  echo "ERROR: No arm64-v8a native libraries found" >&2
  exit 1
fi

echo "Checking release signature..."
apksigner verify --verbose "$apk"

echo "APK size and SHA-256:"
du -h "$apk"
sha256sum "$apk"
echo "APK preflight PASSED. Device testing and Play Console validation are still required."
