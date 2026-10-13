# Building Krimble for Android

This replaces README.android.md, which documents the old upstream Krita
build (NDK r18b, androidbuild.sh, armeabi-v7a) — none of that applies
here anymore.

## Server
- Oracle Cloud instance `instance-20260819-1011`
- IP 129.151.24.131, user `ubuntu`, Ubuntu 24.04, 4 OCPU / 24GB RAM
- SSH: ssh -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131

## Directory layout on the server — READ THIS FIRST
(Corrected 2026-09-25. Earlier versions of this file said ~/krimble/app
was the checkout — that directory does not exist.)
- ~/krimble — the git checkout of THIS repo. Only correct source dir.
  `git pull` here. setup-env.py also drops workspace files in here
  (env, venv/, cache/, ccache/, krita-deps-management/) — not source,
  untracked, leave them alone.
- ~/kwd — build work dir (KDECI_WORKDIR_PATH). Build output lands in
  ~/kwd/krita/_build and ~/kwd/krita/_install. MUST be outside the
  source tree: androiddeployqt silently drops all Qt QML modules if
  _install sits inside the source root (per upstream android.yml).
- ~/krita — raw source drop, NOT a git repo. Do not build or edit here.

## Server CPU is ARM64 — READ THIS TOO
- `uname -m` = aarch64 (Oracle Ampere). The NDK's host tools are
  x86_64 and run under emulation. The emulated linker (ld.lld)
  segfaults. Symptom: CMake fails with "Host compiler must support
  64-bit std::atomic!" — misleading; the real error is a linker
  segfault in _build/CMakeFiles/CMakeConfigureLog.yaml.
- The NDK's prebuilt/linux-aarch64 folder is an empty stub. Unusable.
- NDK r30 = x86-only host tools. Use 27.3.13750724.
- Fix (one-time, server): replace NDK lld with native Ubuntu lld-18:
  ```
  sudo apt-get install -y lld-18 ccache
  cd ~/Android/sdk/ndk/27.3.13750724/toolchains/llvm/prebuilt/linux-x86_64/bin
  mv lld lld.x86_64.orig
  ln -s /usr/lib/llvm-18/bin/lld lld
  ./ld.lld --version     # expect: Ubuntu LLD 18.x
  ```
  <!-- Undo: `rm lld && mv lld.x86_64.orig lld` in the same folder. -->
  Undo: NOT AVAILABLE on the current server (2026-09-26). The swap was
  run twice, so lld.x86_64.orig is also a symlink — the Intel lld
  backup is gone. Reverting means reinstalling NDK 27.3.13750724.
  Guard against re-runs: only run the mv if `lld` is NOT already a
  symlink (`test -L lld || mv lld lld.x86_64.orig`).
  (ld -> ld.lld -> lld symlink chain follows automatically.)

## Toolchain
- Qt5/KF5, NOT Qt6 — BUILD_WITH_QT6 must stay OFF
- Target ABI: arm64-v8a
- Android NDK 27.3.13750724 at ~/Android/sdk/ndk/27.3.13750724
- CMake toolchain file: krita-deps-management/tools/android-toolchain-krita.cmake
  (NOT the ECM Android.cmake, NOT the raw NDK android.toolchain.cmake)
- Dependencies: prebuilt KDE CI packages via setup-env.py — do NOT
  introduce kdesrc-build or any other toolchain

## Build steps
Follows Krita's own CI recipe: build-tools/ci-scripts/android.yml.
Do not hand-patch build.gradle or hand-run cmake/gradle.
All commands run on the CLOUD SERVER.

### Step 1 — fetch prebuilt dependencies (confirmed working 2026-09-25)
```
cd ~/krimble && git pull
python3 krita-deps-management/tools/setup-env.py --full-krita-env --android-abi arm64-v8a -r ~/krimble -o env
```
Then check ~/krimble/env points every NDK variable
(KDECI_ANDROID_NDK_ROOT, ANDROID_NDK_ROOT, ANDROID_NDK_HOME) at
27.3.13750724, not 30.x.

### Step 2 — native build (re-run the whole block in every new SSH session)
CONFIRMED WORKING 2026-09-26: started ~00:56, finished clean ~08:30
(server time) on commit bf2f7b3 with native lld. Output libs in
~/kwd/krita/_install/lib, qml present in ~/kwd/krita/_install.
```
cd ~/krimble
source ~/krimble/env
export KDECI_CACHE_PATH=/home/ubuntu/krimble/cache
export KDECI_GITLAB_SERVER=https://invent.kde.org/
export KDECI_PACKAGE_PROJECT=teams/ci-artifacts/krita-android-arm64-v8a
export KDECI_WORKDIR_PATH=/home/ubuntu/kwd
export CMAKE_TOOLCHAIN_FILE=/home/ubuntu/krimble/krita-deps-management/tools/android-toolchain-krita.cmake
export KDECI_EXTRA_CMAKE_ARGS="-DHIDE_SAFE_ASSERTS=ON -DBUILD_TESTING=OFF"
export KRITACI_ANDROID_PACKAGE_TYPE=release
export KRITACI_ANDROID_RELEASE_MODE=1
unset VIRTUAL_ENV
nohup python3 -u krita-deps-management/ci-utilities/run-ci-build.py --project krita --branch master --platform Android/arm64-v8a/Qt5/Shared --only-build --skip-publishing > ~/build-krita.log 2>&1 &
tail -f ~/build-krita.log
```
Notes:
- The KDECI_CACHE_PATH / GITLAB_SERVER / PACKAGE_PROJECT values are set
  by setup-env.py in memory only and never written to env — they must
  be exported by hand.
- Do NOT set KDECI_GLOBAL_CONFIG_OVERRIDE_PATH. It points at the deps'
  config and breaks the config merge (TypeError: list indices must be
  integers or slices, not str).
- After a failed configure, `rm -rf ~/kwd/krita/_build` before retrying
  — CMake caches failed checks.
- nohup keeps the build alive if SSH drops. Ctrl+C only stops `tail`.
- If the Termux screen freezes during `tail -f`, the SSH link dropped.
  Close the session, reconnect, then check:
  `pgrep -f run-ci-build.py || echo "BUILD PROCESS FINISHED"`
- A full build takes ~7.5 hours on this server (x86 compiler under
  emulation). ccache makes rebuilds faster.

### Step 3 — package the APK
[UNCONFIRMED as of 2026-09-25 — not yet run.] Same shell/exports as
step 2, then:
```
python3 -u build-tools/ci-scripts/build-android-package.py 2>&1 | tee ~/build-apk.log
```
Upstream CI output location: $KDECI_WORKDIR_PATH/krita/_packaging/*.apk

### Signed release
Requires KRIMBLE_KEYSTORE_PASSWORD and KRIMBLE_KEY_PASSWORD in the
server's shell profile, and ~/krimble-release.jks present on the server.

## Signing
- Keystore: ~/krimble-release.jks (server), alias `krimble`
- Passwords: KRIMBLE_KEYSTORE_PASSWORD / KRIMBLE_KEY_PASSWORD env vars
- Never commit the .jks file or passwords — .gitignore already blocks
  *.jks / *.keystore

## Known open issues
- claude/KRIMBLE-BUILD-CHEATSHEET.md is referenced in KRIMBLE_STATUS.md
  but has never existed in this repo's git history — either lost
  (written locally, never committed) or never actually created
- Last confirmed working state (per KRIMBLE_STATUS.md, 2026-09-06):
  libkrita_arm64-v8a.so compiles and links clean. No confirmed
  packaged APK yet as of that date.

## Server helper scripts (added 2026-10-06)

The build settings and the long build / package / check commands live in `tools/server/`. Copy them once to the home folder of the server:

    cd ~/krimble && git pull origin master && cp tools/server/kb-*.sh ~/ && chmod +x ~/kb-*.sh

Then the whole routine is four short commands: `~/kb-build.sh` (pull the code and build), `~/kb-status.sh` (how it is going, in plain words), `~/kb-package.sh` (make the APK once the build has finished) and `~/kb-apk.sh` (check the APK, name it, print the copy line for the phone).
`kb-env.sh` holds the settings that used to have to be pasted into every new SSH session. `kb-build.sh` will not start a second build; `kb-package.sh` always leaves exactly one packaging run.

## Release builds, version numbers, size and the Play Store AAB (added 2026-10-09)

Appended to this file. Nothing above was changed. Where this section and an
older section disagree, THIS section is newer.

Sources: George's server (`~/.bash_history`, file listings), the repo, and
two ChatGPT session records recovered 2026-10-09. Every statement is tagged:
**[verified]** = seen on the server or in the repo, **[recorded]** = from the
ChatGPT records only, **[unconfirmed]** = a theory or a gap.

Times: the server clock is UTC. George works in Mountain time (MDT = UTC-6).
The size-reduction work happened on 2026-10-07 in Mountain time, which is
2026-10-08 in UTC. Both dates appear in older notes. [verified: AAB file
dated 2026-10-08 04:28 UTC on the server]

### 1. Step 3 is confirmed (supersedes "UNCONFIRMED" above)

Packaging the APK with `build-android-package.py` was confirmed working
2026-09-26 and has been run many times since. [verified]

### 2. Version numbers (rewritten 2026-10-09)

> **Update 2026-10-12:** the number the app shows on launch (and the APK version) is the hand-set
> `krimbleBuildNumber` in `packaging/android/apk/build.gradle`. The counter described below is retired
> (commit `3b7ae14`). A version change needs `kb-build.sh` before `kb-package.sh` (the stamp is made at build
> time). Where this section disagrees with section 8, section 8 is right.

**The version is set by hand. Nothing sets it automatically.**

- File: `packaging/android/apk/build.gradle`. One number:
  `def krimbleBuildNumber = 30`
- That one number gives:
  - `versionName` = `1.0.<number>-beta` (so `1.0.30-beta`)
  - `versionRelease` = the number, so the Android version code =
    `5000000 + 50000 + 400 + number` = `5050400 + number`
    (the 5,000,000 is `project.ext.constant`, worked out from the used code
    5050400, not read from the file; major 5, minor 4) [verified in repo]
- Google Play rejects an upload whose version code it has already seen. Every
  upload needs a higher number than every earlier upload. 5050400 was used by
  an earlier upload and was rejected as a duplicate (2026-10-07).
- When leaving beta: the text `-beta` is in the `versionName` line. Change it
  to `-rc1`, `-rc2`, then drop the suffix for the release.

**Routine (George's choice, 2026-10-09).** Before each build:
1. Claude asks George for the number.
2. Claude checks it is higher than the last number used.
3. Claude changes the one line, shows the diff, and on approval pushes it
   (logged with date and time).
4. George pulls on the server and builds (section 7).
Each bump is one line in `KRIMBLE_CHANGES.md` under "Version numbers".

**Why by hand:**
- `versionName` was hardcoded `"1.0.0-beta2"` for many builds, so the APK
  never showed a new version.
- On 2026-10-09 a counter-based version was tried. It was dropped the same
  day: the counter (below) grew on `make` and packaging runs that were never
  released, reaching 42 when the last release was 1.0.29. George wanted no
  automation.
- 1.0.29-beta matches the GitHub release 1.0.29b (2026-10-07, commit
  f01a17e, 158 MB). The next build is 30.

**The build counter (separate from the version. Ignore it for releases.)**
[verified]
- File: `~/krimble-build-number.txt` on the server.
- `libs/version/KrimbleBuildStamp.cmake` adds 1 to it every time the stamp
  target runs, and writes `krimble_build_stamp.h` (number, date, git hash).
  The app shows these in the splash and logs.
- The stamp target runs on every `make` AND on every `kb-package.sh` run
  (packaging rebuilds the version library first). Measured 2026-10-09:
  37 -> 39 after one build and one packaging run; 39 -> 42 after one build
  and two packaging runs. That is +1 for each run.
- So the counter counts runs, not releases. It is not the version name and
  not the version code. Its value was 42 on 2026-10-10 04:07 UTC.
- If the file is lost, counting restarts at 1. This does not affect the
  version.

**APK file names** [verified]
- `kb-apk.sh` names the copy `Krimble-<versionName>-<MonDay>-<HHMM>.apk`,
  for example `Krimble-1.0.30-beta-Oct9-2230.apk`. The version is read from
  the APK's own file name. The date and time are Mountain time, from the
  APK file's time.
- Before 2026-10-09 it was `Krimble-Beta2-<date>-b<counter>.apk` (`Beta2`
  hardcoded; who chose it is not recorded).
- The server's `~/kb-*.sh` are copies. After a pull, refresh them:
  `cp ~/krimble/tools/server/kb-*.sh ~/`

### 3. Why the package was about 509 MB, and the fix

**Cause: native debug symbols.** [recorded, measurements exact]
- Debug symbols are data for debuggers (source lines, symbol names). The
  app does not run them. Removing them does not remove any app code.
- The C++ was already built optimised (`-O3 -DNDEBUG`), so this was not a
  Debug-versus-Release problem. [recorded]
- Largest case, `lib_kritalcmsengine_arm64-v8a.so`:
  219,082,456 bytes -> 23,180,648 bytes (89.4% smaller). [recorded]
- APK: about 509 MB -> 165,553,065 bytes (157.8 MiB). [recorded; matches
  the 158 MB asset on the GitHub release 1.0.29b, verified]

**How it was found** [recorded, commands from `~/.bash_history`: verified]
1. Stale NDK references were replaced (see section 4).
2. A copy of the library was stripped by hand to prove the idea:
   `llvm-strip --strip-debug` using NDK 27's `llvm-strip`
   (about 209 MB -> about 31 MB on the test copy).
3. A temporary CMake hook (`strip-android-libs.cmake`, called from
   `ECMAndroidDeployQt.cmake`) was run to strip during packaging.
4. That hook was removed. Verified 2026-10-09: the file is gone and
   `ECMAndroidDeployQt.cmake` has no "strip" text on the server.
5. The final mechanism is Gradle's own task `stripReleaseDebugSymbols`,
   which runs as part of a release package build. No custom strip step
   remains.

**Verified 2026-10-09: no strip hook is needed** [verified]
- Two fresh builds, on a server with no strip hook and no strip helper
  (confirmed absent), produced
  `krimble-arm64-v8a-1.0.39-beta-release.apk` (165,508,434 bytes) and
  `krimble-arm64-v8a-1.0.42-beta-release.apk` (165,517,982 bytes), both about
  157.8 MiB.
- So the size fix needs only: the repo as it is, NDK 27.3.13750724 installed,
  and `ndkVersion "27.3.13750724"` in `build.gradle`.
- The server's `.bash_history` merges several SSH sessions, so its line order
  cannot show whether the hook ran before the last 2026-10-07 builds. The
  fresh builds settle that: Gradle's own stripping is enough.

**Still unconfirmed:** that the `ndkVersion` change is what made Gradle's
stripping start working. The repo's own comment says a mismatched NDK makes
AGP fail to strip native libraries, and the change came before the strip
tests, but nobody compared the Gradle log before and after.
- If a package ever grows back to about 500 MB, check `ndkVersion` in
  `build.gradle` and the NDK folder under `~/Android/sdk/ndk/` first.

### 4. NDK versions: only 27.3.13750724 is correct

- Correct: `27.3.13750724` at `~/Android/sdk/ndk/27.3.13750724`.
- r22b and r30 are wrong for this server. r30 has x86-only host tools.
- Places that held stale versions, all fixed on the server [verified in
  history]: `~/krimble/env` (r30 -> 27), `~/.bashrc` lines 135-137, and
  `ndkVersion` in the server's `build.gradle` (r22 -> 27).
- Removed 2026-10-09 [verified]: `~/Android/android-ndk-r22b` and its
  `.zip` (about 4.7 GB). `~/Android/sdk/ndk/` now holds only 27.3.13750724.
  The empty folder `~/Android/ndk` remains.
- Repo text that still mentions old NDKs (comments and warnings only, none
  used by a build): a comment in `build.gradle` saying r22b is installed
  (outdated), the commented-out `ndkVersion "22.1.7171670"` line,
  `build-tools/ci-scripts/android.yml` line 18 (upstream comment), and the
  r18b/r30 mentions at the top of this file.

### 5. Building the AAB for Google Play

Play Store needs an `.aab`, not an `.apk`.

Script: `build-tools/ci-scripts/krimble-build-aab.py` [verified: in repo]
- A copy of `build-android-package.py` with `./gradlew assembleRelease`
  changed to `./gradlew bundleRelease`, and the output match changed from
  `*.apk` to `*.aab`. The header comment in the file says the same.
- Do not use `build-android-appbundle.py` for this. It expects artifacts
  from a separate earlier job.
- It deletes the whole `~/kwd/krita/_packaging` folder before writing, so
  an APK made earlier is wiped. Copy it out first.

Run (cloud server), after the native build has finished, with the same
exports as step 2 (`kb-env.sh` has them):
```
export KDECI_ANDROID_ABI=arm64-v8a
export KDECI_WORKDIR_PATH=$HOME/kwd
cd ~/kwd/krita
python3 ~/krimble/build-tools/ci-scripts/krimble-build-aab.py --package-type release
```
Output: `~/kwd/krita/_packaging/krimble-arm64-v8a-<versionName>-release.aab`.
First AAB: 271,313,916 bytes (258.8 MiB), `BUILD SUCCESSFUL`. [verified:
file on the server, dated 2026-10-08 04:28 UTC]

Target API: `targetSdkVersion` is 36 (was 35). Google Play required it. It
is in the repo as of 2026-10-09. `compileSdk` stays 35. [verified]

Play Console result for the first upload (internal testing track)
[recorded]: the size check and the target-API check passed. The upload was
rejected because versionCode 5050400 was already used. Not confirmed: that
a later upload was accepted or that the app was published.

Open question [unconfirmed]: the AAB (258.8 MiB) is larger than the
stripped APK (157.8 MiB). Not known whether stripping was applied to the AAB.

### 6. Server housekeeping notes (2026-10-09)

- `~/Krimble-Beta2-*.apk` (11 files, pre-strip, 486 to 497 MB each) were
  deleted. The stripped 158 MB APK exists only as the GitHub release asset.
- The server's `~/krimble/.kde-ci.yml` is modified (a 168-line dependency
  list replacing the repo's 19 lines). It is generated output: server history
  shows `generate-deps-file.py -o .kde-ci.yml`. It was made before
  2026-10-01 and can be regenerated. Do not overwrite it without reading its
  diff.
- Done 2026-10-09: the server's own edits to `build.gradle` were discarded
  with `git checkout -- packaging/android/apk/build.gradle` once the same
  edits were in the repo. `git pull` now works. The server's modified
  `.kde-ci.yml` was left alone.
- `kb-build.sh` runs `git pull origin master | tail -n 2` and does not stop
  if the pull fails. A failed pull builds the old code without warning. Check
  that the line `CODE VERSION:` in its output is the commit you expect.
- **`kb-status.sh` can show an old result.** Its PACKAGING line reads the
  last packaging log and lists the newest APK in `_packaging`. Right after a
  build, "PACKAGING: FINISHED OK" and an old APK mean nothing: packaging has
  not run yet. Run `kb-package.sh`.
- **`kb-package.sh` and `krimble-build-aab.py` both delete everything in
  `~/kwd/krita/_packaging`.** Move an APK or AAB you want to keep out first.
  Running `kb-package.sh` twice is safe: the second run stops the first and
  leaves one run.
- Old AAB kept: `~/Krimble-old-Oct8.aab` (271 MB, version code 5050400, built
  before the version changes).
- Copies of APKs on the phone: `Downloads/` on George's phone.
- The history of every command run on the server is in `~/.bash_history`.
  Long commands: always `setsid nohup ... < /dev/null &`, not `nohup` alone.

### 7. Routine for a release build (added 2026-10-09)

Runs on the cloud server unless marked. Check each result before the next
step.

1. Claude sets the version number and pushes (section 2).
2. Pull and refresh the scripts:
   `cd ~/krimble && git pull origin master | tail -3 && cp tools/server/kb-*.sh ~/ && git log -1 --oneline`
   The last line must be the commit Claude pushed.
3. Only if C++ files changed since the last build: `~/kb-build.sh`, then
   `~/kb-status.sh` until it says BUILD: FINISHED OK. Check that
   `CODE VERSION:` in the output matches. If only `build.gradle` or other
   non-C++ files changed, skip this step. [expected; the skip has not been
   run yet]
4. `~/kb-package.sh`. About 10 minutes.
5. `~/kb-status.sh`. PACKAGING must say FINISHED OK and list an APK whose
   name contains the new version. If it lists an old APK, packaging has not
   finished.
6. `~/kb-apk.sh`. It copies and names the APK and prints the copy line.
7. Type `exit`. On the phone, in Termux (not on the server), run the `scp`
   line it printed. The APK lands in `Downloads/`.
8. Size check: a normal APK is about 158 MB (165.5 million bytes). About
   500 MB means stripping failed (section 3).

### 8. Where things stand, and what was learned (added 2026-10-12)

**Builds so far (all from `master`, tags on GitHub):**

| Version | Commit | Tag | Notes |
|---|---|---|---|
| 1.0.29 beta | `f01a17e` | `1.0.29b` | |
| 1.0.30 beta | `baa1509` | `1.0.30b` | |
| 1.0.31 beta | `117207e` | `1.0.31b` | first build with the toolbox/menu work |
| 1.0.32 beta | `3774d49` | `1.0.32b` | |
| 1.0.33 beta | `3c7f2aa` | not tagged yet | first build with Share, Quick tools, Help items |
| 1.0.34 beta | see `git log` | | |

Each tag must sit on the commit the APK was built from, not on `master` at posting time. Create it with
`git push origin <commit>:refs/tags/<name>` before choosing "existing tag" on the release page.

**The build, short:** (all on the server; label every step local/server)
1. Local (phone, Termux): `ssh -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131`
2. Server: `cd ~/krimble && git pull origin master && cp tools/server/kb-*.sh ~/ && git log -1 --oneline && ~/kb-build.sh`
   (check that the commit line is the one expected)
3. Server, later: `~/kb-status.sh` until `BUILD: FINISHED OK` (an incremental build takes about 20 to 30 minutes; a first build about 7.5 hours)
4. Server: `~/kb-package.sh` (about 10 minutes), then `~/kb-status.sh`: the APK listed must show the new version and a newer time
5. Server: `~/kb-apk.sh` (never before step 4 has finished: it names whatever APK is newest, which would be the old one), then `exit`
6. Local: the `scp` line that `kb-apk.sh` prints, into `~/storage/downloads/`

**Things learned the hard way:**
- `kb-status.sh` shows only the first 6 error lines. Paste them to whoever is fixing the code.
- The server clock is UTC; the phone is MDT (UTC minus 6 in October). An APK stamped `Oct 11 01:46` was made at 7:46 PM on Oct 10.
- The PACKAGING line of `kb-status.sh` lists the previous APK until `kb-package.sh` has run.
- Code that has never been compiled can stop a build on a type error; only the failed file is recompiled on the next `kb-build.sh`.
- `ShareHelper.java` (new in 1.0.33) is compiled by the packaging step, not the native build; a Java error would show there.
- A 158 MB APK is slow to upload from a phone. Upload it from the server instead: on the server run
  `read -rs -p "GitHub token: " GH; echo; curl --progress-bar -X POST -H "Authorization: Bearer $GH" -H "Accept: application/vnd.github+json" -H "Content-Type: application/vnd.android.package-archive" --data-binary @$HOME/<apk file> "https://uploads.github.com/repos/purdygeorgeedward-dev/krimble/releases/<release id>/assets?name=<apk file>" | head -c 300; echo; unset GH`
  (the release id is shown by the GitHub API for the draft release; the token prompt hides what is typed and keeps it out of the shell history).

**What is in the app now, and where the code is** (every item has a dated entry in `KRIMBLE_CHANGES.md`, entries 12 to 75; Claude's ideas are marked "(Claude's idea)" in the code and the entry):

| Feature | Main file | Entry |
|---|---|---|
| Toolbox: columns, 2 by default, saved column count, one block of tools, drag bar with grip lines, wells under the tools, floating at first start | `libs/ui/toolbox/` | 22, 24, 31, 35, 36, 38, 40, 43, 64 |
| Panel edges: thin invisible strip, small touch handle | `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `KisMainWindow.cpp` | 55, 56, 58, 64 |
| Window menu: Tabs/Floating Windows switch, Show Contents While Dragging, Hide All Panels, Layout Presets, Quick Adjust, Save/Reset Layout | `KisMainWindow.cpp` | 33, 34, 37, 60, 70, 75 |
| Menus without shortcut column, 1 px dividers | `KisMainWindow.cpp`, style | 21, 23, 64 |
| Floating windows by default | `KisMainWindow.cpp` | 34 |
| Help menu: Where is..., Send Feedback, QA Overlay | `KisMainWindow.cpp` | 50 |
| File menu: Share as JPG/PNG, Batch Process | `KisMainWindow.cpp`, `ShareHelper.java` | 53, 73, 74 |
| Layer menu: Style Presets | `KisMainWindow.cpp` | 75 |
| Quick Adjust panel | `KisMainWindow.cpp` | 70 |
| Toolbars: stacked, Reset, Close All, seven extra toolbars (hidden by default) | `KisMainWindow.cpp`, `krita5.xmlgui` | 41, 49, 61, 66 |
| Dialogs: fit the screen, corner grip, long labels wrap, two-finger move, no bounce except the toolbox | `KisApplication.cpp`, `KoFileDialog.cpp` | 29, 39, 46, 57, 63, 72 |
| New Document: size presets, thin left list, colored section icons | `KisOpenPane.cpp`, `krita/data/` | 48, 51, 59, 67, 69 |
| Dodge and Burn, Type tool fixes | `plugins/tools/basictools/`, `svgtexttool/` | 44, 45 |
| Layers panel can be narrow | `plugins/dockers/layerdocker/LayerBox.cpp` | 68 |
| README "Thank you" section and icons | `README.md`, `thanks-icons/` | 71 |

**Removed after testing (code kept in comments):** Cheat Sheet (crashed), Mirror Layout (corrupted the display), three of the ten extra toolbars.

**Known open issues (2026-10-12):**
- Preferences, Brightness/Contrast and the JPEG export window can be wider than the screen in portrait.
- Custom toolbars: no bigger grab handle when vertical, tools do not arrange into columns.
- Camera button, upscale item and AI upscaling are not built.
- Each Android phone shows 2 to 3.4 device pixels per logical pixel; use Help > QA Overlay to get real numbers from a screenshot.

### 9. Where things stand on 2026-10-13 (entries 76 to 106; added Tue Oct 13, 2026, 07:00 MDT)

**Releases on GitHub:**

| Version | Commit | State |
|---|---|---|
| 1.0.32 beta | `3774d49` | published |
| 1.0.33 beta | `3c7f2aa` | published (checked on GitHub 2026-10-13), file attached |
| 1.0.34 beta | `c814ef1` | published, file attached (`Krimble-1.0.34-beta-Oct12-1620.apk`) |
| 1.0.35 beta | | **not built.** `krimbleBuildNumber` is 35 (`752408e`); the build was started and stopped at George's request before it finished. Nothing from it was released. |

The next build, when George asks, carries everything since `c814ef1`: entries 84 to 106. Version 35 is already set.

**New since section 8 (feature | main file | entry):**

| Feature | Main file | Entry |
|---|---|---|
| Version 35 | `packaging/android/apk/build.gradle` | 95 |
| Toolbox: fits its tools (no empty space), curved handle, no bounce off the bottom, horizontal dock rows | `libs/ui/toolbox/KoToolBoxDocker.cpp`, `KisApplication.cpp`, `KisMainWindow.cpp` | 84, 85, 100 |
| Menu items capped at their text plus 60 px | `libs/widgetutils/KisWideDockSeparatorStyle.cpp` | 84 |
| Preferences list 24% of the screen; New Document list 22% in portrait, Predefined list 104 px in portrait | `KisApplication.cpp`, `KisOpenPane.cpp`, `kis_custom_image_widget.cc` | 84, 86 |
| View > Rotation Gadget (hidden by default) | `KisMainWindow.cpp`, `kis_statusbar.cc` | 87, 88 |
| Batch Process: Cancel and Close, moved to the Image menu | `KisMainWindow.cpp` | 89, 90 |
| New Document starts with the copied picture's size, with a preference (Preferences > General > Pasting) | `KisDlgCreateNewDocument.cpp`, `kis_dlg_preferences.cc` | 91, 94 |
| Layers panel: six buttons in two rows of three | `plugins/dockers/layerdocker/LayerBox.cpp` | 93 |
| Floating panels dock only right on an edge (8 px); Dock menu choices work on floating panels | `KisMainWindow.cpp`, `KoDockWidgetTitleBar.cpp`, `KoToolBoxDocker.cpp` | 92, 96, 102 |
| Paste: a picture that misses the page is moved to the center (cause not proven) | `libs/ui/actions/KisPasteActionFactories.cpp` | 98 |
| Image windows: 12 px frame to grab | `KisWideDockSeparatorStyle.cpp` | 99 |
| Polygon tools for a finger: marching ants, points by tap or drag, close by dropping on the first point | `libs/ui/tool/kis_tool_polyline_base.cpp` | 97, 106 |
| PNG export options narrow; every window wider than the screen is narrowed | `plugins/impex/png/kis_png_export.cc`, `KisApplication.cpp` | 103, 104 |
| George's icons for Preferences and New Document; originals in `krimble-artwork/` | `krita/pics/svg/`, `krita/data/templates/` | 105 |
| Wording: "docking" for panels and windows, "snapping" for the canvas only | text only | 101 |

**Known open issues (2026-10-13):**
- **About Krimble** window is too big on a small screen (reported 2026-10-13; not looked at yet).
- "Brushes still open by default": what opens is unknown (not the Brushes panel in the 1.0.34 screenshots); needs a description from George.
- A docked toolbox does not change its width in portrait; the toolbox starts floating, so it matters less.
- Paste into a new document: nothing shows (layer thumbnail fine, page empty, saved file empty); "Paste as New Image" works. The fix in entry 98 is a guess from reading the code and has not been tried.
- Image windows are hard to resize: entry 99 is a first step; a corner grip is the next if it is not enough.
- Polygon tools: Tool Options buttons (Undo Point, Close, Cancel) and moving a placed point are not built.
- Other export windows (JPEG, TIFF, WebP and so on) were not looked at one by one; the rule in entry 104 narrows any window wider than the screen.
- Camera, the Upscale item, and AI upscaling are not built. Quick Adjust, Batch Process, Style Presets, Startup Layouts and the polygon changes are new and have had little or no testing on a device.

**Lessons added:**
- George edits `README.md` on GitHub while work is going on. Before every push: fetch, and if the remote moved, `git stash`, `git merge --ff-only FETCH_HEAD`, `git stash pop`, then commit and push. His edits are kept.
- An idle Termux session to the server is closed after a while ("Connection timed out"); a running build is not affected. Reconnect with the ssh line.
- `kb-package.sh` before `kb-apk.sh`, always; the stamp in `kb-apk.sh`'s output (`KRIMBLE_BUILD_NUMBER`, `KRIMBLE_BUILD_GIT`) shows what the APK really is.
- Every change is documented in `KRIMBLE_CHANGES.md` with an entry number, date and what was not verified. Claude's ideas are marked "(Claude's idea)", George's "(George's idea)". The app's own wording: "docking" (panels and windows), "fitting" (toolbox columns), "snapping" (canvas only).
