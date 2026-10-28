# Android release packaging — Krimble

This repository currently builds Android packages through
`packaging/android/apk/build.gradle` and the existing Qt/Krita Android
build process. The UI-refresh branch must be built and tested before release.

## Release invariants

- Package/application ID: `org.krimble`
- Target SDK: 36
- Minimum SDK: 24
- NDK: **27.3.13750724 (r27)**; do not silently upgrade it
- Build number: explicitly edited by the release maintainer in `build.gradle`
- Signing keys and passwords: outside the repository; never committed

## Build and package

1. Use the existing configured Qt 5.15/Android/CMake build environment
   and build/install the native Krimble libraries into the staging prefix.
2. Set `KRITA_INSTALL_PREFIX`, `ANDROID_ABI=arm64-v8a`, and
   `KDECI_ANDROID_NDK_ROOT` for the existing Gradle packaging workflow.
3. Set signing variables `KRIMBLE_KEYSTORE_PASSWORD` and
   `KRIMBLE_KEY_PASSWORD` only in the protected build environment.
   The keystore is expected at `$HOME/krimble-release.jks`.
4. From `packaging/android/apk`, run the established Gradle
   `assembleRelease` task once the native build is complete.
5. Locate the resulting release APK and run:

   ```bash
   bash packaging/android/verify-apk.sh /path/to/krimble-release.apk
   ```

   Run that command from the repository root (or use an absolute script path).
   `aapt`, `apksigner`, and `unzip` must be on `PATH`.

## Before publication

Check QuickAdjust Preview, Cancel and OK; the foreground color picker;
RGB/HSV/HEX numeric entry; palette persistence and touch long-press; dialog
bounds, white borders, orientations, and UI scaling on a real Android phone.
Validate `versionCode` increases from the last Google Play upload.
Perform a fresh install and an upgrade test without losing user settings.
Submit the produced artifact for Play Console automated validation.

The APK verification script is a *packaging preflight*, not a substitute
for building, installing, running, or Play Console review. The UI branch
should remain a draft until these checks pass.
