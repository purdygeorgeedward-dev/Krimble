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
| 55 | Nov 9, 11:40 AM | 42676143585 | `69c3a1c` | no | Color Lab, new look; built from the workflow-trigger commit; tag `v1.0.55-beta` moved to this commit on 2026-11-11; draft release, APK to be attached by George |
| 56 | Nov 11, 7:57 PM | 43144941653 | `ad64d9b` | yes | published as pre-release v1.0.56-beta on 2026-11-12 (tag v1.0.56-beta on this commit) |
| 57 | Nov 12, 8:30 PM | 43359369032 | `cdca33d` | yes | started with the PAT George gave (HTTP 204), finished in 57 minutes; George installed it and tested (krita.log: 1.0.57-beta, git cdca33d); published as pre-release v1.0.57-beta on 2026-11-13 (tag on this commit; the APK is the artifact krimble-apk of this run, to be attached by George: the artifact host is not reachable from Claude's sandbox); first build with the flyouts and the Hexagon Selection tool |
| 58 | Nov 13, 2:50 AM | 43404551956 | `1c86669` | yes | started with the PAT George gave (dispatch, label 1.0.58-beta); first build that compiled the SmartCompression panel, the Hexagon Selection tool's flyout entry and the new panel code with the Krita headers: it succeeded; published as pre-release v1.0.58-beta on 2026-11-13, 9:47 PM (tag on this commit through the branch release-1.0.58) |
| 59 | Nov 14, 11:39 PM | 43686436841 | `77dcb3e` | no | started with the PAT George gave (dispatch, HTTP 204, label 1.0.59-beta, run #33); the earlier attempts: run 43407141407 (voided), run #31 and #32 (compile errors, fixed) |
| 60 | Nov 15, 9:38 AM | 43736626312 | `12b860a` | no | George tested it: not to be released; failures written into RELEASE_NOTES_1.0.60-beta.md |
| 61 | Nov 15, 11:54 AM | 43749388783 | `a88d845` | no | George tested it (menu, tab, Color Lab and recent images reports) |
| 62 | Nov 15, 3:28 PM | 43769444886 | `69d2873` | no | George tested it (theme switch, docking, K gap, Layers reports); contains commits up to the 677th only |
| 63 | Nov 15, 5:18 PM | 43779575290 | `f8edc72` | no | built after the first run (43778358166) failed on a compile error (fixed in the 702nd); contains entries 679 to 703 |
| 64 | Nov 15, 9:11 PM | 43802615314 | `4ba8bb1` | no | started by the push of the 710th; 15 min; APK artifact 14197553400 (about 146 MB); contains entries 704 to 710 (706th to 709th: the fixes for the ten faults of build 63) |
| 65 | Nov 16, 3:01 PM | 43965939960 | `50c3a4e` | no | started by the push of the 726th, after the first run (43963457903) failed on a compile error (two lines in kis_jpeg_converter.cc, fixed in the 726th); 15 min; APK artifact 14276432810 (about 146 MB); contains entries 712 to 726 (722nd: the 1.0.64 bug list items 1 to 14; 723rd: the brand name removed; 724th: image tabs the default, patterns restored) |
