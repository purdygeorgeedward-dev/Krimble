# Krimble work log (pushes with their real times)

Made 2026-10-25, 2:50 PM Mountain Time, from the commit history on GitHub (every push since 2026-10-23, oldest first). Times are **Mountain Time (El Paso)**, taken from the commits themselves. **Do not guess the time of day when writing notes: ask the clock** (the `user_time_v0` tool, or the time of a commit). Several notes written on 2026-10-25 had times of day that were wrong by hours; this file is the correct record.

The column "Author" is the name on the commit. **Two Claude sessions work on Krimble** (this one outside the Krimble project, and one inside it), and **both can use the name "George Purdy"**: only the commits named "purdygeorgeedward-dev" are certainly from the other session (and `2785d1d`, marked). "George Edward Purdy" is George himself, editing on GitHub's web page (the README). The full explanation of each change is in `KRIMBLE_CHANGES.md` (the entry numbers are in most commit messages).

| Time (MT) | Commit | Author | What |
|---|---|---|---|
| Oct 23  7:32 AM | `c7590e6` | George Purdy | New dialog: smaller minimum size, clipboard/camera pictures, new size presets; File > Revert back; test list saved; changelog 236th-237th |
| Oct 23  9:20 AM | `17c83cf` | purdygeorgeedward-dev | JPEG save options window: 505 px spacer removed, filter list capped, window fitted to the screen; changelog 238th |
| Oct 23  9:31 AM | `6b7719e` | purdygeorgeedward-dev | Save As / Export format window opens taller (75% of screen height); changelog 239th; test list item 4 done |
| Oct 23  9:35 AM | `8adc89f` | purdygeorgeedward-dev | Panel handle: visible tab from George's drawing, centred on each side's edge; changelog 240th; test list item 27 done |
| Oct 23  9:37 AM | `4b9505c` | purdygeorgeedward-dev | File Info window: pages scroll, fitted to the screen; changelog 241st; test list item 5 (size) |
| Oct 23  9:41 AM | `af1806b` | purdygeorgeedward-dev | File Info Author page: George's robot head icon; changelog 242nd; test list item 5 done |
| Oct 23  9:43 AM | `5bb0a72` | purdygeorgeedward-dev | Preferences > Author picture: robot head; changelog 243rd |
| Oct 23  9:44 AM | `8c8be8b` | purdygeorgeedward-dev | Puppet Warp: floating message explains the cage; changelog 244th; test list item 6 |
| Oct 23  9:46 AM | `ff2beb7` | purdygeorgeedward-dev | Free Transform: touching outside the box rotates again; changelog 245th; test list item 7 |
| Oct 23  10:08 AM | `84bc382` | purdygeorgeedward-dev | Free Transform: curved-arrow rotate targets outside the corners; changelog 246th; test list item 7 done |
| Oct 23  10:12 AM | `ed43c9b` | George Purdy | Test list: items 14, 25, 26, 29, 30 claimed by the KRIMBLE-MAIN chat |
| Oct 23  10:12 AM | `a1b6488` | George Purdy | Floating toolbox: width and height together, no empty bottom after detaching; changelog 247th; test list item 30 |
| Oct 23  10:13 AM | `a1541d6` | George Purdy | Window > Stack Panels; changelog 248th; test list item 29 done, item 30 done |
| Oct 23  8:34 PM | `cea5716` | George Purdy | Test list: item 31 (3D icons for Preferences and New dialogs) claimed by the KRIMBLE-MAIN chat |
| Oct 23  8:43 PM | `a8b67e8` | George Purdy | 3D icons for Preferences and File > New from George's icon sheet; originals in krimble-artwork/3d; changelog 249th; test list item 31 done |
| Oct 23  8:46 PM | `d9b3a7d` | George Purdy | KoDialog windows: main part in a scroll area so no window is wider than the screen; narrowing judges the contents' width; changelog 250th; test list i |
| Oct 23  8:49 PM | `2e7957f` | George Purdy | Windows: no empty negative space (small spacers, tight margins, tabs as tall as the showing page, open at contents' size); changelog 251st; test list  |
| Oct 23  8:53 PM | `8c1e0b8` | George Purdy | Selection tools start on Replace in every run; changelog 252nd; test list item 33 |
| Oct 23  8:55 PM | `17ffc8e` | George Purdy | Reset Tool Options on Start toggle (menu, next to Reset Preferences); changelog 253rd; test list item 34 |
| Oct 23  9:03 PM | `f0c70d9` | George Purdy | Version 42; changelog 254th |
| Oct 23  11:50 PM | `b4d3edd` | George Purdy | Panel edge tab: 70 px tall; right tab drawn with an edge; overlay shows why a tab is hidden; changelog 255th; test list item 35 |
| Oct 24  12:12 AM | `6bcb890` | George Purdy | Smudge: the retouch tools are found by their real tool ids; Stack Panels via remove/add; New window pictures back; right panels attach from 28 px; Fre |
| Oct 24  12:12 AM | `4c12bde` | George Purdy | Restore the files that the previous commit (6bcb890) removed by mistake (a whole folder was staged from a partial working copy); the intended changes  |
| Oct 24  12:15 AM | `5556e1b` | George Purdy | Toolbox: orientation no longer follows the window shape while floating; docked side decides; changelog 261st; test list item 37 |
| Oct 24  12:27 AM | `19a624f` | George Purdy | Research notes on github.com/storytold (ArtCraft/PhotoCraft); changelog 262nd |
| Oct 24  12:34 AM | `9368b84` | George Purdy | Content-Aware Fill: coarsest level filled ring by ring from the best patches; thin holes by thickness; changelog 263rd |
| Oct 24  4:03 AM | `235e92d` | George Purdy | Version 43; changelog 264th |
| Oct 24  4:29 AM | `330d0d7` | George Purdy | About > Also Thanks To: Claude and ChatGPT first, Krita's list kept below; changelog 265th; test list item 44 |
| Oct 24  4:32 AM | `833740b` | George Purdy | About > Also Thanks To: the README's thank-you list with logos; changelog 266th; test list item 45 |
| Oct 24  4:36 AM | `b0e6c54` | George Purdy | Process rule: the current build is released on GitHub before the version number changes (BUILD_ANDROID.md step 0); changelog 267th |
| Oct 24  4:40 AM | `3f1856c` | George Purdy | Release tags are v1.0.N-beta (old 1.0.Nb still accepted by kb-release.sh); changelog 268th |
| Oct 24  4:45 AM | `45cb4b4` | George Purdy | Old releases and tags renamed to v1.0.N-beta; BUILD_ANDROID.md table updated; changelog 269th |
| Oct 24  2:05 PM | `2143731` | George Purdy | SESSION_CATCHUP.md rewritten for a new chat in the Krimble project; changelog 270th |
| Oct 24  4:14 PM | `7e19e0e` | George Purdy | Start page: no Krita text, DEV BUILD, manual/getting started/community, KDE, news; changelog 271st; test list 46 |
| Oct 24  4:26 PM | `ce16448` | George Purdy | Type tool window redesigned (tabs, font star, B/I/U/S/Aa, 4 alignments, spacing, kerning, preview, two columns sideways); changelog 272nd; test list 4 |
| Oct 24  4:42 PM | `884423e` | George Purdy | Type tool: frame with handles on the chosen text (move, width, size); syntax checker tool; changelog 273rd; test list 48 |
| Oct 24  9:34 PM | `0c799f2` | George Purdy | Customize Toolbar to the Window menu and narrower; Edit Presets narrower; 9 retro palettes for Indexed Color; monochrome Noto Emoji first fallback in  |
| Oct 24  10:08 PM | `275eae0` | George Purdy | Test list item 26 (Text menu) closed; changelog 277th |
| Oct 24  11:43 PM | `8c6aed1` | George Purdy | README: See it in action (first screenshot); changelog 278th |
| Oct 24  11:50 PM | `f2f39ff` | George Purdy | About dialog no longer locked to a huge size (two columns sideways); on-screen messages shorter; CAF message only on fallback; README second screensho |
| Oct 25  12:54 AM | `6cfe342` | George Purdy | Polygonal Selection tool: how-to messages removed; changelog 282nd; test list 53 |
| Oct 25  12:58 AM | `d6b6b48` | George Purdy | README: six screenshots (whole editor, floating toolbox, three windows, ALARM, upright pair); changelog 283rd |
| Oct 25  4:15 AM | `815b943` | George Edward Purdy | Update README.md |
| Oct 25  8:14 AM | `58d80cd` | George Purdy | Batch Process: recipes and WebP with transparency (engine, window, tests); changelog 284th; test list 54 |
| Oct 25  8:31 AM | `799e638` | George Purdy | Krimble Script: a very small script language (12 commands) in Batch Process; KRIMBLE_SCRIPT.md; README feature list; 59 tests; changelog 285th; test l |
| Oct 25  8:47 AM | `c545d38` | George Purdy | Krimble Help: in-app handbook (Help > Krimble Help) with Krimble Script; roadmap: Animation and game sprites (proposed); changelog 286th and 287th; te |
| Oct 25  8:52 AM | `5e658be` | George Purdy | Batch Process and Krimble Script: animated GIF and WebP (read; write animated WebP; frames, fps, loop commands); tests; docs; changelog 288th; test li |
| Oct 25  8:54 AM | `f7f94c1` | George Purdy | Release 1.0.43 beta: tag v1.0.43-beta on d6b6b48, notes final, draft kept until the APK is attached; changelog 289th; build table row |
| Oct 25  9:39 AM | `04e07af` | George Purdy | Eraser tool: dedicated toolbox tool (brush with the eraser switch on); icons into the build; handbook part; changelog 290th; test list 59 |
| Oct 25  12:00 PM | `c22e1ae` | George Purdy | Panel and toolbox edge tab: not drawn any more (touch zone unchanged, the little dots stay); changelog 291st; test list 60 |
| Oct 25  12:27 PM | `b25ebbb` | George Purdy | Keyboard Shortcuts icon: George's keyboard picture (old one kept); changelog 292nd; test list 61 |
| Oct 25  12:40 PM | `9ff7c00` | George Purdy | Remove the old keyboard picture (3d/keyboard-first-2026-10-23.png) at George's request; notes updated; changelog 293rd |
| Oct 25  12:44 PM | `42f433b` | George Purdy | House icon (Preferences > General) from George; unused art removed (26 files); artwork README rewritten; changelog 294th |
| Oct 25  12:46 PM | `9a02b85` | George Purdy | Create from Clipboard picture: George clipboard (rainbow one replaced); changelog 295th; test list 62 |
| Oct 25  12:54 PM | `b4c9add` | George Purdy | New window: labels beside fields, Cancel and Create side by side (marked parts skip the narrowing); changelog 296th; test list 63 |
| Oct 25  1:27 PM | `758fa77` | George Purdy | Global narrowing rules (label above field, rows of three stacked) now only for windows with no scroll area; changelog 297th; test list 64 |
| Oct 25  1:51 PM | `be32b9d` | George Purdy | Polygonal Selection: on-canvas bar with Undo point, END, Cancel; handbook part Selecting; changelog 298th; test list 65 |
| Oct 25  2:21 PM | `2785d1d` | George Purdy (the other session) | Crash fix: three native functions still had the old org_krita names (hasMainWindowLoaded on every Back, openFileFromIntent, saveState); Java side no l |
| Oct 25  2:39 PM | `dfd7352` | George Purdy | Relaunch on app switch: more configChanges, Help > Show Krimble Log (sessions, last exit, why the window was destroyed); changelog 299th; test list 66 |
| Oct 25  2:40 PM | `e2db1fd` | George Purdy | Docs: my relaunch entry is the 300th (299th is the JNI names fix of the other session); test item 67 |
| Oct 25  2:40 PM | `001623c` | George Purdy | Test list: the other session item stays 66 (JNI names fix); mine is 67 |

## Build events that are known (from the server's own output)

- **Oct 25, 8:13 AM MT** (14:13 UTC): the 1.0.43 APK was packaged: `Krimble-1.0.43-beta-Oct25-0813.apk`, 180,983,744 bytes, build stamp 43, commit `d6b6b48`.
- **Oct 25, 9:37 AM MT** (15:37 UTC): `kb-status.sh`: BUILD FINISHED OK, PACKAGING FINISHED OK. **9:40 AM MT**: `kb-apk.sh` made the copy to download. George installed it from there.
- The tag `v1.0.43-beta` was made through the API on `d6b6b48`. **Oct 25, 3:09 PM MT** (21:09 UTC): `~/kb-release.sh --publish` was run on the server; the release was **published with the APK attached** (180,983,744 bytes; checked through the API).

## Branch `feature/krimble-ui-refresh`, 2026-10-30 (pushes with their real times)

Times are Mountain Time from the clock of the sandbox that pushed (`date`), checked with `git ls-remote`. Commits are named "George Purdy" (the identity set for this session; the work was done by the Claude session inside the Krimble project). None is on master.

| Time (MT) | Commit | What |
|---|---|---|
| Oct 30  9:41 AM | `c270d0dc53` | QuickAdjust heading shows the tool name only; changelog 324th |
| Oct 30  9:41 AM | `d0a5e0b387` | classic color selector hidden, outer tab bar removed; changelog 325th |
| Oct 30 10:06 AM | `1a6968320c` | Scale mode: outside the box no longer rotates, rotate arrows off; changelog 326th |
| Oct 30 10:06 AM | `9c4ccd7421` | Rotate tool: modes other than Free act as the Transform tool; changelog 327th |
| Oct 30 10:06 AM | `dc1a432a93` | Rotate mode button in Transform Tool Options, first button is Scale; changelog 328th |
| Oct 30 10:45 AM | `73b50863b8` | Line tool: Thickness slider and number box; changelog 329th |
| Oct 30 10:52 AM | `bd5f52d7cc` | Brushes panel: Size, Opacity, Flow rows; changelog 330th |
| Oct 30 12:23 PM | `760e8c87ed` | Layer > New > Star...; changelog 331st |
| Oct 30 12:30 PM | `a9c18e1b67` | the 32 old toolbox PNGs removed from the branch; changelog 332nd |
| Oct 30 12:37 PM | `3bc2e91217` | documentation updated (STATUS, SESSION_CATCHUP, NEXT_BUILD, WORKLOG, ROADMAP, test list, icon coverage); changelog 333rd. The first text said 12:45 PM, a guessed time; the clock said 12:37 PM, corrected in the next commit |
| Oct 30 12:43 PM | `d720ab2485` `85f6f57493` `ceb84c87f2` `dbc37944be` `eb0bc3f31f` | one push: QuickAdjust Keep as layers and Image menu item (334th); Brushes panel never appears by itself (335th); Brush name field (336th); tool buttons app-wide in the stylesheet (337th); panel docking dwell 350 ms (338th) |
| Oct 30 12:55 PM | `cdcac43d9b` `30cb7cecc8` `e9e9285c6b` `1d54a0a7aa` | one push: six stale comments (339th); File > New icons 64 px (340th); Color Studio one page, gradient SV square (341st); Mixer panel (342nd) |
| Oct 30 2:40 PM | `5c5c35f1cc` `47cae9dddf` | one push: orange progress bars and fills (344th); new main splash (345th) |
| Oct 30 2:55 PM | `752e5de643` `e61dba9d82` | one push: toolbox icons Full color / Clear (348th); blue glow on checked buttons (349th) |
| Oct 30 5:41 PM | (this push) | toolbox icon style renamed Clear to Silhouette (351st) |
| Oct 30 6:35 PM | `79d49255ea` + docs | one push: Quick Adjust completed (352nd); George's answers recorded in the docs (353rd) |
| Oct 30 7:04 PM | (docs push, 354th) | corrections: the four tools are to be commented out (not yet done), the Channels docker exists, the Quick Adjust logo mark, keeping notes |
| Oct 30 7:10 PM | (this push) | Quick Adjust own logo (355th) |
