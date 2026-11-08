# Krimble build log (one row per SUCCESSFUL build)

**Rule (George, 2026-11-08): strict renumbering.** Every build that makes an APK gets its own number, higher than the last one in this table. We do not always release a build, but we always change the number.
Before a build: set `krimbleBuildNumber` in `packaging/android/apk/build.gradle` and the label default in `.github/workflows/android-build.yml` to `1.0.<number>-beta`.
After a successful build: add its row here (number, time, run, commit, notes). The build workflow runs `tools/ci/check_build_number.py` first and stops the build if the number was already used or the label does not match.
Failed or cancelled builds make no APK, so they do not use a number.

Numbers 45 to 54 were given afterwards, on 2026-11-08, to the ten builds that had all been labeled `1.0.45-beta` (their APKs and the app still say 1.0.45-beta). Times are Mountain Time.

| Number | Built (MT) | Run | Commit | Released | Notes |
|---|---|---|---|---|---|
| 45 | Nov 4, 8:14 AM | 41855727577 | `b278e96` | no | first successful build on GitHub Actions |
| 46 | Nov 5, 4:48 AM | 42021981429 | `48bb7f9` | no | |
| 47 | Nov 5, 8:25 AM | 42054910591 | `b065f8f` | no | |
| 48 | Nov 5, 7:11 PM | 42148668674 | `800d676` | no | |
| 49 | Nov 6, 3:18 AM | 42201619759 | `a3d0e07` | no | |
| 50 | Nov 6, 4:14 AM | 42208788950 | `9b4c368` | no | the red, green and blue diagnostic dialog colors were still on |
| 51 | Nov 7, 5:00 AM | 42370554831 | `add724c` | no | last build with the menu file from before the reformat |
| 52 | Nov 7, 2:47 PM | 42417539040 | `fd62f88` | no | DPaint panel; TWO Layer menus in the menu bar (the Gemini reformat of the menu file) |
| 53 | Nov 7, 9:36 PM | 42447492701 | `cb75239` | no | TWO Layer menus; the draft release v1.0.45-beta and its tag pointed here and were deleted on 2026-11-08 |
| 54 | Nov 8, 3:40 AM | 42472436825 | `8c38709` | no | the menu file restored (one Layer menu); XML check ran first |
