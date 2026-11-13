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
| Oct 30 7:11 PM | (this push, 356th) | the Quick Adjust logo drawn in code switched off; waiting for George's graphic |
| Oct 30 7:12 PM | (this push, 357th) | Colorize Mask Editing and Enclose and Fill commented out; George: keep Ruler and Reference Images; comments corrected |
| Oct 30 7:28 PM | (this push, 358th) | toolbox is 32 tools: four tools shown on the phone again, arrow mapped to Edit Shapes |
| Oct 30 7:35 PM | (this push, 359th) | Select Shapes has the toolbox button; Edit Shapes is an Edit points toggle |
| Oct 30 7:39 PM | (docs push, 360th) | the app starts on the Hand tool: recorded; my wrong statements withdrawn |
| Oct 30 7:40 PM | (docs push, 361st) | Rule zero: never answer without checking the notes (top of SESSION_CATCHUP.md) |
| Oct 30 7:41 PM | (docs push, 362nd) | seven docs restored after the 359th to 361st commits had cut them down |
| Oct 30 7:42 PM | `07910db16e` | **merge of `feature/krimble-ui-refresh` into master** (GitHub API) for 1.0.45-beta |
| Oct 30 7:42 PM | (docs push, 363rd) | 1.0.45 merge recorded |
| Oct 31 7:15 PM | (docs push, 364th) | KRIMBLE_STATUS.md: the branch is merged; 1.0.45 from master |
| Nov 3  1:46 PM | `01b495d` | purdygeorgeedward-dev | GitHub Actions workflow added (`android-build.yml`) |
| Nov 3  3:00 PM | `3c7fcaa` | purdygeorgeedward-dev | Workflow: APK label default 1.0.45-beta (was Beta2) |
| Nov 4  8:14 AM | `b278e96` | purdygeorgeedward-dev | Workflow: KDECI_ANDROID_SDK_ROOT set; step fails on `## Failed` in the build log |
| Nov 4  8:17 AM | (docs push, 365th) | builds moved to GitHub Actions: changelog, status, build doc, worklog |
| Nov 4  10:15 AM | (this push, 366th) | Color panel: the 42% height cap applies only while docked; a floating panel can be made taller |
| Nov 4  10:17 AM | `190efe3` | purdygeorgeedward-dev | Style sheet: one blue family, flat fields, softer shading, toolbox unshaded (367th) |
| Nov 4  10:17 AM | `5a8f3a5` | purdygeorgeedward-dev | Theme Krimble dark: selection colors (368th) |
| Nov 4  10:17 AM | `2f5a995` | purdygeorgeedward-dev | Toolbox icons: Silhouette default (369th) |
| Nov 4  10:17 AM | `96e58f6` | purdygeorgeedward-dev | Style sheet set again after the theme palette, for the startup gray (370th) |
| Nov 4  10:17 AM | `6f1046c` | purdygeorgeedward-dev | Percentage fills: selection blue instead of orange (371st) |
| Nov 4  10:17 AM | (docs push, 367th to 371st) | changelog, worklog, status for the five changes above; none built yet |
| Nov 4  6:29 PM | (this push, 372nd) | File menu: no invisible system-menu button over "File" (WindowSystemMenuHint removed from image windows) |
| Nov 4  7:10 PM | (this push, 373rd) | Save As: the file type selector opens at 90% of the screen height (was 75%) |
| Nov 4  7:10 PM | (this push, 374th) | Splash: the version number is at the bottom left |
| Nov 5  8:25 AM | `4a3620a` | purdygeorgeedward-dev | QuickAdjust rebuilt to follow the mockup, QuickAdjust logo, logo images (375th) |
| Nov 5  8:25 AM | `8a9e437` | purdygeorgeedward-dev | Sliders everywhere: 4 px groove, 14 px round handle (376th) |
| Nov 5  8:25 AM | `04e0b78` | purdygeorgeedward-dev | Color Studio rewritten to follow the Picker mockup (377th) |
| Nov 5  8:25 AM | `64376af` | purdygeorgeedward-dev | Mixer panel scrolls; mixing pad never cut off (378th) |
| Nov 5  8:25 AM | `79113b1` | purdygeorgeedward-dev | Theme palette: Light and Midlight blue-gray (379th) |
| Nov 5  8:25 AM | (docs push, 375th to 379th) | changelog, worklog, status |
| Nov 5  8:42 AM | `3cda7ae` | purdygeorgeedward-dev | Image window frame drawn in the window color, 16 px width kept (380th) |
| Nov 5  7:10 PM | `68d35b5` | purdygeorgeedward-dev | Icons: keyboard taller, Rotate arrows, sharp rectangle corners (384th) |
| Nov 5  7:10 PM | `be7d3b4` | purdygeorgeedward-dev | Close button back (hint restored, menu bar icon click-through); QuickAdjust thin lines, metal balls, left logo (381st to 383rd) |
| Nov 5  7:10 PM | `21ade60` | purdygeorgeedward-dev | Every slider: 2 px line, 12 px metal ball (383rd) |
| Nov 5  7:10 PM | `c3d2c62` | purdygeorgeedward-dev | Color Studio logo left aligned, 40% (382nd) |
| Nov 5  7:10 PM | (docs push, 381st to 385th) | changelog, worklog, status |
| Nov 5  3:18 AM | `dd4b191` | purdygeorgeedward-dev | Rotate icon redrawn with clean arrowheads (386th) |
| Nov 5  3:18 AM | `e2275a1` | purdygeorgeedward-dev | Polygon / magnetic selection: the Undo point / END / Cancel bar off (387th) |
| Nov 5  3:18 AM | `50496f0` | purdygeorgeedward-dev | Toolbox: no 8 columns after docking, no dead space when floating (388th) |
| Nov 5  3:18 AM | `12a5340` | purdygeorgeedward-dev | DIAGNOSTIC (temporary): dialog layers dark red, green, blue (389th) |
| Nov 5  3:18 AM | (docs push, 386th to 389th) | changelog, worklog, status |
| Nov 6  4:00 AM | (this push, 390th) | Buy Me a Coffee picture: the robot mascot (product_donation.png) |
| Nov 6  4:02 AM | `ca98b1f` | purdygeorgeedward-dev | Long press: the finger-up no longer picks a menu item (toolbox jumping to Dock Bottom) (391st) |
| Nov 6  4:13 AM | `fe8669d` | purdygeorgeedward-dev | Toolbox: docking at a screen edge waits 1 s (392nd) |
| Nov 6  11:25 AM | `e1658d8` | purdygeorgeedward-dev | Diagnostic dialog colors switched off (393rd) |
| Nov 6  11:36 AM | `2e7ba06` | purdygeorgeedward-dev | Color panel: no height cap, no logo, narrow enough to dock (394th) |
| Nov 6  12:05 PM | `14156cf` | purdygeorgeedward-dev | QuickAdjust: shorter, no empty space, logo 50%, gradient lines for Brightness, Saturation, Lightness (395th, 396th) |
| Nov 6-7  1:34 AM | `5a77c11` | purdygeorgeedward-dev | Toolbox icons: rectangular select, magnet, wand, select similar, eraser, paint bucket, triangle, thumbtack (401st) |
| Nov 6-7  1:34 AM | `f0f1dec` | purdygeorgeedward-dev | Style sheet: ChatGPT's as the base, mockup colors and gradients, whole-app menu bar and menus (397th) |
| Nov 6-7  1:34 AM | `9b65a3f` | purdygeorgeedward-dev | QuickAdjust from the mockup; the K in the menu bar (398th) |
| Nov 6-7  1:34 AM | `6a65b3f` | purdygeorgeedward-dev | Color Studio and Mixer: touching cells, smaller, sharp, soft brush (399th) |
| Nov 6-7  1:34 AM | `63a5df7` | purdygeorgeedward-dev | Toolbox selected tool, Tool Options tool name, Scale moves the selection, toolbox log lines (400th, 403rd) |
| Nov 6-7  1:34 AM | `c44a484` | purdygeorgeedward-dev | Android splash dialog: version text at the bottom left (402nd) |
| Nov 7  1:38 AM | `bff632e` | George Purdy | Android splash dialog: version text at the bottom right (402nd updated) |
| Nov 7  4:52 AM | `28b40c7` | George Purdy | Window > Stack Panels includes open floating panels (404th) |
| Nov 7  4:52 AM | `7380b87` | George Purdy | Transform: uniform scaling is the default for every new transform (405th) |
| Nov 7  4:52 AM | `21834be` | George Purdy | New dialog: OK and Cancel taken out of the scrolling page, always visible; shorter description box (406th) |
| Nov 7  4:53 AM | `4345149` | George Purdy | Build workflow: compiler errors fail the native step, shown as annotations; native build log kept as an artifact (407th). Build 42356342646 (bff632e) had failed in 4 min without saying why |
| Nov 7  5:00 AM | `add724c` | George Purdy | KoToolDocker: include KoCanvasController.h, the compile error found by the annotations (408th). Build 42370554831 succeeded (APK 172 MB) |
| Nov 7  12:27 PM | `4ab027e` | George Edward Purdy | (George's own edit on GitHub) Refactor menu structure in krita5.xmlgui. The file was NOT valid XML (a line cut off); repaired in `ae22039` |
| Nov 7  2:22 PM | `2649594` | George Purdy | The Settings menu bar entry is hidden (409th) |
| Nov 7  2:22 PM | `9cd8402` | George Purdy | Transform: Position/Rotate/Scale/Shear radio buttons are the drag mode; no Rotate button in the toolbox (410th) |
| Nov 7  2:22 PM | `1b11e63` | George Purdy | Floating messages at the bottom left of the canvas (411th) |
| Nov 7  2:22 PM | `8a2f6fd` | George Purdy | Toolbox: UNDO button under the color wells, long press for Redo (412th) |
| Nov 7  2:22 PM | `8de18a1`, `01ebaa5` | George Purdy | DPaint toolbar (extra toolbar, hidden at the start); renamed DPaint because the longer name may be a trademark (413th) |
| Nov 7  2:22 PM | `bf99fd9` | George Purdy | Pixel-art icons for DPaint brush buttons, not used yet (414th) |
| Nov 7  2:22 PM | `d21966b` | George Purdy | Type tool: every new text on its own vector layer, only the new text selected (415th) |
| Nov 7  2:22 PM | `ae22039` | George Purdy | krita5.xmlgui: the truncated line of the 12:27 edit repaired; the menu bar still has TWO Layer menus (416th) |
| Nov 7  2:47 PM | `fd62f88` | George Purdy | DPaint panel, first version: tools, UNDO/CLR, color indicator, 256 color palette with Range and Edit (417th). Build 42417539040 succeeded |
| Nov 7  9:36 PM | `32cbd91` | George Purdy | Menu bar K drawn 18 px; Preferences keyboard icon shows the whole keyboard (418th) |
| Nov 7  9:36 PM | `8524008` | George Purdy | Style sheet: gradient on the main window, panels and status bar (419th) |
| Nov 7  9:36 PM | `911a752` | George Purdy | Layers panel: 24 px before the Layer Type drop-down, 20 px strip above (420th) |
| Nov 7  9:36 PM | `b1f9055` | George Purdy | Toolbox selected tool drawn like the mockup, measured edge glow (421st) |
| Nov 7  9:36 PM | `f77d7d5` | George Purdy | Check boxes: a check mark in a checked box (422nd) |
| Nov 7  9:36 PM | `0babdac` | George Purdy | Style sheet: the Prefs mockup's selected look, cyan check boxes (423rd) |
| Nov 7  9:36 PM | `cb75239` | George Purdy | Style sheet: more contrast (424th). **This is v1.0.45-beta.** Build 42447492701 succeeded (APK 172 MB) |
| Nov 7  10:35 PM | `2e7369a` | George Purdy | Dev journal for 2026-11-07 and RELEASE_NOTES_1.0.45-beta.md; tag `v1.0.45-beta` on `cb75239` and a draft release (the first version of this row said 10:37 PM; the push was at 10:35 PM) |
| Nov 7  10:35 to 10:40 PM | `45edcf8` and the push after it | George Purdy | Only the times in the two rows above corrected (both pushes of the first correction were in the 10:35 PM minute; this log is for real times) |
| Nov 7  11:14 PM | `011ea89` | George Edward Purdy | (George's own edit on GitHub) Update krita5.xmlgui: removes the extra Layer menu, but only the end of it; the file was not valid XML again |
| Nov 7  11:17 PM | `f31a0ca` (and the push right after it, which only corrected this row's time) | George Purdy | krita5.xmlgui: the unfinished first Layer block removed whole, file valid, one Layer menu; 9 actions lost in the reformat reported, not restored (425th) |
| Nov 8  3:30 AM | `ada6eca` (and the push right after it, which only corrected this row's date: it said Nov 7) | George Purdy | krita5.xmlgui restored to the file of `add724c` (before the Gemini reformat) plus the DPaint toolbar block only; valid XML, one Layer menu, nothing missing (426th) |
| Nov 8  3:40 AM | `4bd6c06` | George Purdy | Menu bar: the K is no longer drawn (427th) |
| Nov 8  3:40 AM | `06fceec` | George Purdy | XML check before every build: tools/ci/check_xml.py, first step of the workflow (428th) |
| Nov 8  3:40 AM | `71844fd` | George Purdy | Gradient tool icon: square corners, no outline, both icon styles (429th) |
| Nov 8  3:40 AM | `ec45565` | George Purdy | Style sheet: one hue for the whole UI, stronger gradient, 16 px check boxes (430th) |
| Nov 8  3:40 AM | `4b446d5` | George Purdy | Preferences: rows never pressed together (krimbleNoSqueeze), wider page list, larger keyboard icon (431st) |
| Nov 8  3:40 AM | `7ba23a8` | George Purdy | Color panel: number boxes show their numbers (66 px), framed picker, rounded square, larger swatches; Mixer boxes 76 px (432nd) |
| Nov 8  3:51 AM | (this push, docs only) | George Purdy | Dev journal rows fixed (a stray backslash-n). Build 42472436825 on `8c38709` succeeded (APK 172 MB); its first step, the XML check, passed |
| Nov 8  4:10 PM | `b00c135`, `3dc55e6` | George Purdy | Menu bar like the mockups' title bars (436th); progress bars smooth left to right and square (437th) |
| Nov 8  4:10 PM | `7545c5e` | George Purdy | Color panel: the Picker page rebuilt from the mockup's measurements (438th) |
| Nov 8  4:10 PM | `44cc395`, `958091b` | George Purdy | The classic color selector back as its own panel, "HSV Triangle Color Picker" (439th) |
| Nov 8  4:10 PM | `33bcca0` | George Purdy | Color panel: tab strip, Sliders and Values pages, alpha channel (440th) |
| Nov 8  4:10 PM | `17b0552` | George Purdy | Named palettes (GPL files) (441st); History page first version and icons, not yet in the panel (442nd). The Palette and History tabs are blank in this state |
| Nov 8  4:10 PM | (this push, docs only) | George Purdy | Dev journal rows for the pushes above |
| Nov 8  8:21 PM | `74a6576` | George Purdy | The Color panel is called Color Lab (443rd) |
| Nov 8  8:21 PM | `69134de` | George Purdy | Color Lab: Palette and History tabs, palette commands, history recording, the palette row's buttons (444th) |
| Nov 8  8:21 PM | `835329f`, `530a42a` | George Purdy | Color Lab logo as the panel's header, left aligned (445th); KRIMBLE_DESIGN_RULES.md |
| Nov 8  8:21 PM | `d8e164c`, `ac61d9f` | George Purdy | Design rules: never touch the splash images; do nothing without an explicit request |
| Nov 8  8:21 PM | `12c4514` | George Purdy | Color Lab: the tabs fill the width (446th) |
| Nov 8  8:21 PM | (this push, docs only) | George Purdy | Dev journal rows for the pushes above; build 1.0.55-beta started right after |
| Nov 8  8:43 PM | `aff6920`, `0bb70bf` | George Purdy | UNDO is a silhouette icon button in the grid of tools, after the last tool; the old text button deleted (447th) |
| Nov 8  8:43 PM | (this push, docs only) | George Purdy | Dev journal rows for the pushes above |
| Nov 8  8:45 PM | `d57c2ff` | George Purdy | The UNDO button never grays out (447th) |
| Nov 8  8:45 PM | (this push, docs only) | George Purdy | Dev journal row for the push above |
| Nov 9  12:26 AM | `90ac86e`, `017775f` | George Purdy | The palette area scrolls through all swatches (448th); CMYK palettes, plates stored and edited (449th) |
| Nov 9  12:26 AM | (this push) | George Purdy | CMYK conversion engine, written and tested alone (450th); the CMYK to-do list in the roadmap; release notes for 1.0.55-beta; build 1.0.55-beta started right after |
| Nov 8  4:15 AM | `400ab12` | George Purdy | Transform and Pan names; the name in Tool Options follows the Transform mode; no "Tool" after tool names (433rd) |
| Nov 8  4:15 AM | `d1be4c6` | George Purdy | Toolbox tear-off log lines written to krita.log with KisUsageLogger (434th) |
| Nov 8  4:15 AM | (this push) | George Purdy | Strict renumbering: build number 55, KRIMBLE_BUILD_LOG.md (builds 45 to 54), tools/ci/check_build_number.py, BUILD_ANDROID.md; the draft release v1.0.45-beta and its tag deleted (435th) |
| Nov 9  3:10 AM | `2636234`, `d872078`, `311e0cc`, (this push) | George Purdy | Toolbox columns after a tear-off (452nd); square color wells and swatches (453rd); flat list selection and menu titles, the Preferences keyboard turned (454th); build 1.0.55-beta started again, same number |
| Nov 9  12:48 AM | `65fcae2`, (this push) | George Purdy | Effects plan notes; the toolbox header fix for the failed 1.0.55-beta build (QToolButton include); build 1.0.55-beta started again, same number |

## Notes of Saturday 2026-11-07 (written 10:3x PM MT)

**What happened.** A long day on the UI after the first build with George's mockups. The build of `bff632e` failed in 4 minutes while GitHub said "success": `run-ci-build.py` exits 0 on a compile error and the job log cannot be read from outside GitHub (it redirects to a blob host the sandbox cannot reach). The workflow now writes compiler errors as annotations (readable through the API) and keeps the log as an artifact; that found the missing include in `KoToolDocker.cpp`. Three builds succeeded: 42370554831 (`add724c`), 42417539040 (`fd62f88`, the DPaint panel) and 42447492701 (`cb75239`, **v1.0.45-beta**). Each APK is the artifact `krimble-apk` of its run (172 MB, kept 90 days).

**Incident.** At 12:27 PM George edited `krita5.xmlgui` on GitHub (commit `4ab027e`, "Refactor menu structure"). The file was not valid XML: the `<text>` of `layercolorspaceconversion` was cut to `<text>` and `t>`, so the program would have had no menus. Repaired with the smallest change (`ae22039`, 416th). **Left for George:** the menu bar now has two `Layer` menus (line 275 and line 473 of the file); before his edit there was one. Lesson: after a web edit of an `.xmlgui` or `.ui` file, parse it as XML before building.

**How the UI was matched to the mockups (a method that worked).** Do not guess: cut the mockup's element out, render the same element in real Qt (PyQt5 5.15 is in the sandbox), put them side by side, and measure real pixel colors in both (edges, corners, middle). The selected tool took seven renders; "irregular glow" was measured around the whole edge (72 values, every 5 degrees) and drawn with conical gradients. The two mockups disagree on the base color (Picker and Mixer pages #212638 to #1A2034, Prefs page #303454 with a #1F223B recessed list); George chose "more contrast", so the app now uses the Prefs palette pushed a step further (424th).

**Decisions (George, 2026-11-07).** Rotate is not a toolbox tool: it is a mode of Transform's Tool Options (the Rotate tool stays registered; Edit > Transform > Rotate still starts it). Scale is uniform by default every time. New texts start on their own layer. Messages at the bottom left. Settings menu removed (Preferences in Edit). DPaint, not the longer name (possible trademark): a gray bevelled look with black pixel icons, a 256-color palette in 4 x 8 pages with Range (tap Range, the first well, the last well), a full-screen palette editor with RGB and HSV sliders planned. "Selections are better looking in the mockups": bright cyan outline and glow on selected items; checked boxes have a real check.

**Open.** (1) Two Layer menus in the menu bar (George's menu edit). (2) The toolbox tear-off goes to one column and then horizontal: log lines are in the build (`KRIMBLE-TOOLBOX`, Help > Show Krimble Log); George needs to tear off once, pick "2 columns" once and send the lines. (3) Color panel wider than a small screen; number boxes show one digit. (4) The glow on tabs, list entries and the OK button needs painting in code (a style sheet cannot do it); the toolbox's selected tool is the model (421st). (5) The DPaint panel's full-screen palette editor, brush preset buttons (pixel, spray, dither; the presets exist in the default bundles), diagonal-split outline/filled shape buttons, hue blend for Range. (6) Color Picker and Mixer page pieces from the mockups (HSV drop-down, eyedropper, palette bar, Brush Test Settings). (7) The v1.0.45-beta release is a draft: the APK must be attached (artifact download is blocked from the sandbox). (8) Older: Play Store `.aab`, 16 KB alignment check of every `.so`, Channels docker features, mascot orange-K headphone check, forum.krimble.org, BUILD_ANDROID.md notes.

## 2026-11-10 evening to 2026-11-11 early morning (MST): the night of the flat theme

Written as a journal on purpose: everything tried, built, argued about and lost is here.

**The brief (Nov 10, 22:00).** Screenshots with finger drawings and a list: the painted (gradient) theme conflicts with every other theme and has to go; keep the glow but with thinner edges; the Image Size dialog wastes space around its units; Color Lab lost its black lines between the color wells; no Undo button in the toolbox; gradient tool always blends to transparent; transparent text renders differently in a saved image; rendering glitches (transparent panels for the column selector).

**The color scheme (Nov 10, 22:01 to 22:48), worked out with rendered demo windows.** Colors were sampled from the painted theme (menu bar `#4d568d` down to `#252d5c`, panels `#364064`, fields `#0b1020`, accent `#1d7cb2`, glow `#3bd5f7`). George spotted that the hues drifted (top bar 231, other objects 226 or 221), so the final rule is one hue and one saturation for every background (**H 227, S 48**, Color Lab's HSV) and only the brightness changes: top bar `#3e4b78` (V47), panels `#2c3554` (V33), lists and menus `#20263d` (V24), inactive tabs `#1b2033` (V20), fields `#111521` (V13). Selected and pressed items: flat `#1d7cb2` with a 2 px `#3bd5f7` edge. The Saturation slider of QuickAdjust runs from gray (zero saturation) to the current hue's full color; black-to-red is a brightness ramp, not a saturation ramp. Handles: small steel ball bearings.

**What went wrong, so it is not repeated.**
- A demo showed an Undo icon and the summary said "Undo button added"; it was only in the mockup. The real button existed since Nov 8 but never appeared: `KisMainWindow` set up the toolbox (line 5099) before `createActions()` created `edit_undo` (line 5121), so the lookup found nothing and the button was silently skipped. Fixed by building it one event-loop turn later (`fd30ba7`, pushed Nov 10 22:31).
- Old notes said the Type tool was disabled and that Channels needed Ctrl+click. Both were wrong (the Type tool has worked for weeks; there is no Ctrl on a phone). Rule: this is a mobile-first app, there is no separate keyboard.
- Text meant to be removed (the Oracle thank-you) was first commented out. Commenting out is for active code only; text is deleted.
- **Work was lost.** Three improvements were made in an earlier chat on Nov 9 (percentage bars shaded left to right with a glow, a finger drag for number fields, bars shaded across only) and only committed in that chat's sandbox, so they never reached the repo. Rule: push every change as soon as it is made.

**Pushed, in order (MST).**
- 05:46 (seven commits, `fd30ba7` to `1099e79`): flat theme applied only for "Krimble dark" (`b0f146d`); black lines between swatches back; tapping a toolbox color well opens Color Lab (it edits the foreground or, after a tap on the background well, the background); gradient default is Foreground to Background; the gradient chooser is in the Gradient tool's options; Color Lab picker with a strict square color area, a thinner hue strip and rounded foreground/background wells (`595d0c5`); Oracle thank-you and logo files deleted (`fad216d`, `819f437`, `8e89b8b`); the orange K drawn again at the left end of the menu bar, sized to the menu text and centered on the bar (`7fdc69f`), with the forced menu bar minimum height removed (`1099e79`).
- 05:49 `6df313e`: Color Lab hex box (the `#` is optional; 6 digits apply as typed; 3 digits on finish).
- 05:52 `76ce728`: "Krimble Painted" is its own selectable theme; the painted sheet applies only for it; every other theme gets no sheet.
- 05:54 `937ff33`: percentage bars shaded left to right with a glow past the end of the fill.
- 05:56 `d505427`: sideways finger drag for plain number fields (inside the existing `KrimbleNumberFields` filter, so the two do not fight).
- 05:57 `bc57924`: bars shade across only (no bottom shading); the menu bar painter paints only for Krimble Painted (it used to paint over every theme).

**Open.** QuickAdjust (all sliders the same blue except Hue, Hue lands on the hue it selects, Saturation gray to the current hue, ball-bearing handles inside QuickAdjust, smaller Preview toggle); Recent colors must record only after the finger lifts; Channels docker touch features (load as selection, show one channel); Preferences keyboard icon (George's new picture, at an angle); Image Size dialog space; JPEG options dialog and floating image window look; transparent text export mismatch; transparent column-selector panels; multi-point gradient builder and saved favorites; a Custom CMYK page for the CMYK engine (`krimble_cmyk_engine.h`, written, not connected); the docked Color Lab height (minimum yes, maximum no: no cap found in the code); build 56 and the build log row for build 55.

## 2026-11-11, 06:00 to 06:10 (MST): less glow on the selected tool, and the CMYK brief

**Less glow (pushed 06:07, `55250bc`, changelog 467).** George, looking at a zoomed picture of the Pan tool: "The hand (Pan tool) example shows too much glow." The selected tool in `KoToolBoxButton::paintEvent` is four layers (outer glow, a vivid blue band inside the edge, a soft halo under the outline, a thin bright core). The outer glow now reaches 5 percent of the button's width (was 10) at 60 percent strength; the inner band is 6 percent (was 11.5); the halo is 2.2 percent (was 3.4). The core line, fill and shadow are unchanged. All old values are in comments.

**The CMYK brief (George, same time; nothing built yet).** He wants industry-standard style interfaces for RGB to CMYK and back:
1. **Control over how a CMYK file is shown on the screen in RGB**: set the points at the edges of the ranges, and also midpoints, to compensate for irregular devices. A **phone camera snapshot of a printed page** should be able to make a color profile from it, and the user must be able to tweak the result.
2. **Control over conversion of RGB, Lab or camera raw to CMYK through output-device profiles**, from professional setups such as SWOP Coated to ordinary consumer printers, with customizable UCR and GCR separations.
3. **A simple set of color wells for defining those control points.**

*What already exists:* the conversion engine `krimble_cmyk_engine.h` (changelog 450) has ink color presets (SWOP Coated, ISO Coated FOGRA39, ISO Matte Coated PC4), dot gain curves (a standard curve or a 13-point table per ink), UCR or GCR black generation (None, Light, Medium, Heavy, Maximum), black ink limit, total ink limit, undercolor addition, black point compensation and saved profiles. It is not connected to anything: there is no page, and Color Lab's CMYK palettes (entry 449) do not use it. *What is new in the brief:* display control points with midpoints; a camera-made profile; consumer printer profiles; the color wells for the control points. *A caution about the camera:* a phone photo is not a measurement (lighting, white balance and the camera's own processing change every color), so the plan is to print a test chart made by Krimble, photograph it with the white paper and known gray patches in the frame, and normalize against them; the result is an approximation the user then tweaks.

## 2026-11-11, 06:10 to 06:25 (MST): the Custom CMYK dialog

George sent nine pictures of the industry-standard CMYK dialogs (Ink Colors, Color Settings, Custom CMYK, Dot Gain Curves, Convert to Profile) and, asked whether to begin with Custom CMYK, said yes. **Pushed 06:25, `739a56f` (changelog 470).** The conversion engine moved to `libs/ui/KrimbleCmykEngine.h` (it had no users yet); a new header-only `libs/ui/KrimbleCustomCmykDialog.h` holds the Custom CMYK dialog (name, ink presets, Standard or Curves dot gain, GCR or UCR, black generation, black and total ink limits, UCA, black point compensation, a gray ramp graph of the plates, Load and Save) and the Dot Gain Curves dialog (13 points per ink, All Same, graph). It opens from **Edit > Custom CMYK...**, added in code above Preferences because `krita5.xmlgui` is George's file. Also pushed earlier in this stretch: the selected tool is a sharp-cornered square that still glows (`e835c96`, 469). Next in the CMYK plan: the Ink Colors dialog, Edit > Color Settings, Edit > Convert to Profile, the display control points with midpoints, the camera made profile, and the control point color wells. The first compile of the dialog happens in the next GitHub build (this sandbox has no Qt headers), so compile errors, if any, will show there.

## 2026-11-11, 06:26 to 06:40 (MST): compile checks before the next build

George: "Proceed with the most important work." A failed GitHub build costs hours, and this night's changes had never been compiled. **The sandbox can compile against real Qt:** `apt-get install -y qtbase5-dev` works here (the Ubuntu archive is allowed), giving Qt 5.15.13. Checked with `g++ -fsyntax-only`, copying a class out of its `.cpp` into a small test file where needed: the new `KrimbleCustomCmykDialog.h` (and the engine it includes) compiles and **runs offscreen**; the rendered dialog is correct (name follows the settings; Standard 20 percent; GCR; Medium; limits; gray ramp graph; Load, Save, OK, Cancel); `KrimbleNumberFields` and the number pad with the new sideways drag compile; `KrimbleMenuBarFilter` with the orange K compiles; both style sheets (flat and painted) parse in real Qt with no warnings. Not checkable here (they need Krita headers): the other edited files, whose changes are small (`KoDualColorButton.cpp`, `kis_slider_spin_box_p.h`'s `paintGlowFill`, `KoToolBoxButton.cpp`, `thememanager.cpp`, `krimble_color_studio.h`); `paintSliderRect` was checked by reading and takes a brush.

**A finding about the CMYK engine.** The conversion itself is accurate (a gray goes to CMYK and back to the same gray at every step I tried), but the gray ramp graph is not smooth: with SWOP (Coated), 20 percent, GCR, Medium and a total ink limit of 300, the cyan, magenta and yellow plates rise to about 56 percent at a 35 percent gray, dip to about 49 percent at a 24 percent gray, then rise again to about 78 percent before falling to 67 percent at black, while black climbs in a rough S. The industry-standard dialog shows smooth curves that rise and then flatten. The cause is in how the engine decides how much black to use (`blackFraction` and `equivalentBlack`). It is a quality item for the engine, not a crash: noted for tuning.

## 2026-11-11, 09:30 onward (MST): a smooth CMYK separation

George saw the gray ramp of the Custom CMYK dialog, said the calculation "should be a smooth calculation between the nodes in the gamut", then pointed out that the profiles are meant to prevent excessive ink, so some backing off at the ink limit is correct. Both true: the CMY drop in the last step before black is the total ink limit working (total ink reaches 300 percent there), but the dip at the 24 percent gray was not (total ink 186 percent). Found by testing with real Qt in the sandbox (`tools/cmyk-engine-test`): the old engine chose the black from the gray component's plate level, which saturates for colors darker than C, M, Y all solid; the least-black search used multiples of one eighth of the black limit; the ink guard used a 24 step scan. **First try (not used):** black as a smooth function of darkness with no match guard: smooth ramp, but the round trip error of printable colors rose from 0.5 to 6.5 (and to 48 for GCR None). **What shipped:** the smooth darkness curve for the wanted black, with the old match guard kept and its two searches turned into bisections (changelog 472). The ramp now matches the shape of the industry-standard graph (black convex from 0 to 100, C, M, Y rising to a plateau); accuracy within about one delta E.

## 2026-11-11, 11:30 MST (18:30 UTC): build 56 started
Row 55 added to KRIMBLE_BUILD_LOG.md (run 42676143585, 69c3a1c, Nov 9 11:40 AM MST). Build number 56 set in build.gradle. android-build.yml not changed (the PAT has no workflow scope), so the build is started with workflow_dispatch and the label 1.0.56-beta. Changelog 473rd.

## 2026-11-11, 02:04 PM MST (21:04 UTC): error-scan fix, build 56 rerun
android-build.yml: "Internal error:" lines ignored by the native build error scan (changelog 474th). Pushing the workflow file starts build 56 again (row 56 not in the build log yet).
## 2026-11-11, 10:15 to 10:25 (MST): the Preferences keyboard icon, finally

George sent the angled keyboard picture with red arrows along its tilted edges: "This was the new keyboard icon for Preferences. Remember???" The history, so it is not lost again: the icon had been the old keyboard picture (first only its left half, then the whole picture wide, 2026-11-05 and 07); on 2026-11-09 he asked "Rotate the keyboard." and it was turned 90 degrees clockwise (upright), which he called wrong, and he provided a new picture, a keyboard seen at an angle (the picture he uploaded on 2026-11-10). That new picture was never put into the icon, and it was in the to-do list ever since. Now it is (changelog 473): cropped to the keyboard, 720 px wide, in a 480 x 317 SVG for both themes; the source is in `krimble-artwork/3d/keyboard-george-2026-11-10.png`.

## 2026-11-11, 16:50 to 17:15 (MST): QuickAdjust, finally

George sent a QuickAdjust screenshot from build 56 (no text), then: "You made no changes to QuickAdjust in 56." Correct: the QuickAdjust items (all sliders the same blue except Hue, Hue landing on its own hue, ball bearing handles, a smaller Preview switch) had been on my open list since 05:03 and I had not done them. Done now, changelog 476; checked in real Qt (Qt 5.15 installed in the sandbox): rendered the slider styles (steel ball handles, blue fills, the two hue lines for a red and a green foreground color), compiled and drew the switch. They go into whichever build George starts next; he decides that.

## 2026-11-11, 17:10 to 17:30 (MST): the swatch lines that vanished when Color Lab was small

George sent three screenshots (Color Lab floating, Color Lab docked, and the Custom CMYK dialog, all from build 56): "ColorLab scaled down so far that some of the black lines dividing the color wells disappeared." Cause found and **reproduced in real Qt**: the swatch cells were placed with `toAlignedRect()` (rounds each cell outward), so at fractional cell sizes neighbors overlapped and covered each other's 1 px black border; the test grid kept 1 of 19 lines with outward rounding and all 19 with shared whole-pixel edges (changelog 477). The Custom CMYK dialog screenshot (it opened inside the docked panel area and is cut off at the right, where the Gray Ramp is clipped) is noted for a look: the dialog is wider than the visible area on the phone, and its scroll area shows a horizontal bar.

**17:30 to 17:50 (MST), the same screenshots: the Custom CMYK dialog on the phone.** The third screenshot showed my new dialog cut off at the right (the sandbox renders had been at 520 px wide, the phone is about 310 logical px). Measured the minimum width with real Qt (360 px), found the causes (the group boxes' labels and combo boxes, a long checkbox, four buttons in one row) and fixed them (changelog 478); rendered at 310 px: no cut-off, no horizontal scroll. Lesson: render every new dialog at phone width (about 310 px wide), not only at the size it opens with on a desktop.

**17:55 (MST), the DPaint panel.** George sent two screenshots (portrait and landscape) of the DPaint panel: "Undo is in the toolbox now" (good: the Undo button appears in build 56) "but look at this...": the DPaint tool buttons were enormous (about 190 px, each icon pixel a 12 px block). Cause: panel filled the docker and the buttons are square and stretch with the width. Fixed with a maximum panel width of 104 px (changelog 479). Noticed, not fixed: in the portrait screenshot the selected ellipse tool in the main toolbox shows a dark filled ellipse inside the glow, unlike the light icon of the selected Hand tool; not known whether that is part of "this".

**17:05 (MST), the DPaint panel.** George: "I thought I indicated that the DPaint toolbox was pointless and you commented it out." No such instruction exists in the changelog or the chats I could search; the panel was registered and showed in build 56 (the giant icons, the "house" polygon icon and the "football" fill icon he pointed out in the 5th and 6th buttons). I stopped redrawing icons (the polygon and fill redraws I had started were dropped, uncommitted), and switched the panel off by commenting out its one registration line (changelog 481). I also found that my 480th entry had redrawn the 4th icon (the ellipse) because I guessed which icon he meant by "football"; he meant the 5th and 6th. Lesson: when George names an icon by what it looks like, ask which one, or count the buttons, before redrawing.

**17:15 (MST), Free Rotate handles.** George sent two landscape screenshots (Transform tool, Free Rotate; the second in Scale mode): the picture rotates but the handles stay upright. Traced in the code: the dial calls `rotateByDegrees()` during a drag; `slotUiChangedConfig` returns at once in paint mode, so the handles were never refreshed while the picture (refreshed through `requestImageRecalculation()`) turned. New `krimbleRefreshHandles()` (changelog 482). Not testable in the sandbox; to confirm on the phone.

**17:20 (MST), dark insides.** George's build 56 feedback: the percentage bars "look fantastic and the drag works great"; the flat selections are cleaner but "don't look as cool without the dark areas inside"; which ones: "Tabs, anything really." Done in the flat style sheet (changelog 483): everything that glows when selected is dark navy inside with the cyan edge (selected tab, pressed/checked/default button, selected menu items, checked box); list rows and slider fills stay flat blue.

**17:25 (MST).** George: "Gradient works great now." and, of the Gradient options screenshot with the chooser (thumbnails, Tag, Search, Add..., Edit...): "it's amazing." So the chooser and its editor buttons from the 456th entry are in build 56 and show. Found in the screenshot: it highlights Foreground to Transparent while Foreground to Background is in use; fixed (changelog 484). Whether Edit... opens the multi-point stop editor, and whether a saved gradient shows up as a favorite, are still to be checked by George.

**19:58 (MST), build 56 started again.** George asked if we were ready for a follow-up build, then: "Doing 56 again is just bugfixes." and, when I asked whether to start it: "We didn't release 56, so it's fine." Started by a comment in `.github/workflows/android-build.yml` (the push trigger), label `1.0.56-beta` unchanged, changelog 486.

**20:50 (MST), release notes.** George: "Prepare the tag and full comments for the release." / "add it and check." Notes written from changelog entries 455th to 486th and added as `release-notes/v1.0.56-beta.md` (changelog 487). Tag `v1.0.56-beta` still to be made on `ad64d9b` when George says.

**21:00 (MST), the transparent menu.** George's screenshot of the toolbox long press menu: "Transparent menu." Reproduced in real Qt: `QDockWidget > QWidget { background: transparent; }` also matched the menu (the toolbox menu has the docker as parent); added `QDockWidget > QMenu` with the menu's fill (changelog 488). George also asked for the APK link (given: artifact krimble-apk of run 43144941653).

**21:10 (MST), the K and File.** George's screenshot of the File menu open: the K overlapped the start of File and its open-menu box ("Why not K always in the corner a little left?"). Class compiled and run in real Qt with a maximized image window: K button forced to the corner (x 1 to 23), File box pushed to x 35 by the spacer item (changelog 489). Also this evening: transparent toolbox menu (488), APK link, draft release and tag for 1.0.56-beta.

**2026-11-12, 04:30 (MST), correction.** In my list of what was left I wrote that the selected Ellipse tool in the main toolbox "shows a dark football" and offered to weaken the dark disc behind selected icons. George: "This was your assumption. I corrected it. The football was in the DPaint toolbox." True: the football was only in the DPaint panel (switched off, changelog 481). The toolbox shadow claim was my assumption, first written in the 17:55 entry above and in the 480th changelog entry; both are corrected (the 480th entry has a CORRECTION line). Not an open item.

**2026-11-12, 04:50 (MST), the wand icon.** George: "magick wand icon should be bigger." Measured: it filled 109 x 108 of 128 px; now 124 x 122 (scaled 13 percent about its middle in both SVG variants, changelog 490). The first attempt wrapped the wrong `<svg>` (the files hold older drawings inside comments, with their own `<svg>`); the check by drawing showed no change, so it was redone on the last `<svg>`.

**2026-11-12, 04:36 (MST), the DPaint toolbar.** George: "Remove the DPaint toolbar. Nobody uses DPaint any more." The toolbar lives only in `krita/krita5.xmlgui` (his file), so I showed him the exact change (lines 1601 to 1617 wrapped in a comment) and asked; he said "yes". Done, changelog 491; XML checked.

**2026-11-12, 08:55 (MST), the wand again.** George: "let me see it." (showed wand_icon_before_after), then "Move the biggest sparkle up and extend the length of the wand, then show me." Done on both SVG variants (changelog 492): big sparkle +1.1 units up (4th subpath, own path with a translate), wand 2.08 units longer at the handle end, refitted to 15.5 of 16 units; overlap check 0 px; picture shown (wand_icon_new). Side effect: the pieces are about 9 percent smaller because the drawing got longer and was fitted to the same box.

**2026-11-12, 09:05 (MST), the wand: "more".** George: "more." Read as the same two changes again. Sparkle limit found: 1.4 units up before it touches the small sparkle (1.6 touches). Wand handle end -7.6 (36 percent longer than the original). First attempt at the fit cut the drawing off at the left and bottom (the measuring canvas was the icon itself); caught by checking the final bbox, redone on a larger measuring canvas (changelog 493). Shown as wand_icon_more.

**2026-11-12, 11:30 (MST), "All 227 please."** George's "HUE?" screenshot (misread by me as "HUE 2" first): measured, the scroll bars were hue 235/238; cause = palette of the "Krimble dark" scheme (Fusion draws the track from Window): all 12 background entries moved to hue 227, saturation 48 (changelog 494); checked in real Qt (track #34364b to #31374c). Image Size dialog (the white boxes): still waiting for his yes to my reading.

**2026-11-12, 12:00 (MST), Image Size.** George: "Yes, white areas represent wasted space. Please eliminate as much as you can." Found the causes (size groups, widest-possible spin box width, long resolution unit text, spacers, window wider than its contents); wrote krimbleCompactImageSizeWidgets and tested it in real Qt with the uic form (window 618 to 444 px, least width 545 to 444). Changelog 495. Not run on the phone.

**2026-11-12, 11:25 (MST), task 1: scroll bars less gray.** George: "Can we make them less gray? Make the handles match better in saturation and brightness?" (then: "I was not focusing on the image size dialog. I should remember to number all the tasks."). Fusion draws scroll bars from the palette Button color x 0.75 saturation: Button now H227 S64 V30; real-Qt check: track #2f3859, handle #333d61 (S47), changelog 496.

**2026-11-12, 11:30 (MST), task 4.** George: "Make the track a little darker." Button color brightness 30 to 27: track V35 to V32, handle V38 to V35 (changelog 497).

**2026-11-12, 11:45 (MST), task 20.** George (JPEG options Metadata screenshot): "These dark areas are too dark. black disappears against it. please lighten in theme." Lists/groups V24 to V30, inactive tabs V20 to V26, fields V13 to V19 (+ two odd darks), scheme View colors follow (changelog 498). Before/after mock: dark_areas_before_after.png.

**2026-11-12, 11:55 (MST), task 21.** George: "Try two points darker." Read as the dark areas of task 20: lists/groups 30 to 28, tabs 26 to 24, fields 19 to 17 (+ two odd darks, scheme View colors), changelog 499; three-level picture dark_areas_three.png. Caught a mistake while doing it: my first replacement also rewrote the color names inside the task 20 comment; fixed before the commit.

**2026-11-12, 12:05 (MST), task 22.** George: "Try 26." Main dark level 28 to 26, tabs 22, fields 15, odd darks, scheme View colors 26/22; changelog 500; picture dark_areas_26.png.

**2026-11-12, 12:15 (MST), task 23.** George asked where "Load Layer Transparency as Selection" is in Photoshop (answer: a Ctrl-click on the layer thumbnail; menu: Select > Load Selection with the layer's transparency channel), then "Shouldnt we rename it to match Ps?"; I proposed "Load Layer Transparency" and the two exact lines of krita5.xmlgui; "yes. push." Done, changelog 501.

**2026-11-12, 12:45 (MST), task 27.** George sent a screenshot of the old plain gray look (every bar #373952, no flat sheet) and asked to keep today's theme as Blue and make the old one the default ("blue-gray"). Two schemes (KritaBlueGray.colors from ad64d9b, KritaBlue.colors), flat sheet gated on "Krimble Blue", default name "Krimble Blue-Gray", legacy "Krimble dark" read as the default (changelog 502).

**2026-11-12, 12:55 (MST), task 27 (second part).** George: "Please keep all the glow stuff. It's all really good." The default theme Krimble Blue-Gray now = the flat sheet recolored to the old blue-gray (hue 236, saturation 33) at run time (krimbleRecolorSheet), changelog 503; real-Qt test and picture bluegray_with_glow.png. Also his answer about the weird menu bar: "size and spacing of menu items" (task 26, still to look at).

**2026-11-12, 12:58 (MST), task 26.** George: "Please restore the old height." Menu bar items back to padding 1px 9px, margin 1px 1px in the flat sheet (504th entry). Portrait/landscape spacing not built.

**2026-11-12, 12:51 (MST), push.** 5334111 pushed to master (confirmed by git ls-remote): menu bar items old height back (504th entry).

**2026-11-12, 13:05 (MST), task 29.** George: remove the little locks from the panel title bars. Lock button hidden in both title bars; saved locks ignored (505th entry). He also reports that dragging a dock tab does nothing; no touch-blocking code found by reading, so the next step is our own drag-out-of-the-tab gesture (not built).

**2026-11-12, 13:20 (MST), task 28.** George: "Please implement the hilight and shadow lines." Done in the flat sheet and as an overlay on the image windows (506th entry); picture bevels_before_after.png. Task 29 tab gesture code added but the real-Qt test did not float the panel (507th entry). Committed locally, not pushed.

**2026-11-12, 13:35 (MST), task 30.** George: menu separators were lovely before. QMenu::separator rule (black line + highlight line) added to the flat and the painted sheet (508th entry); picture menu_separators_before_after.png. Committed locally, not pushed.

**2026-11-12, 13:03 MST, push.** 9968e77, d43d743, 29a8d03, 7eee3e5 pushed to master (tasks 29, 28, 30; 505th to 508th entries).

**2026-11-12, 13:08 (MST), task 31.** George: "The Nov 10 version is so much better. Revert to that and make it the default." KrimbleVisualStyleNov10.h (the exact pre-flat sheet + dock menu and divider rules) applied to the default theme Krimble Blue-Gray; Blue and Painted unchanged (509th entry). Committed locally, not pushed.

**2026-11-12, 13:11 MST, push.** 6c84be5 (task 31, 509th entry: the Nov 10 sheet is the default theme) pushed to master.

**2026-11-12, 13:15 (MST), task 32.** George: "Can we reapply the fixes?" Across-only shading for tool bars, status bar and headers in the Nov 10 default sheet (510th entry); picture bars_across_fix.png. Pushed with the PAT George pasted earlier in this chat (he had sent it more than once).

**2026-11-12, 13:19 (MST), task 33.** George: "Rough. No shadow or hilight lines?" Bevel edges added to the Nov 10 default sheet (511th entry); picture default_bevel_demo.png. Committed locally, waiting for his OK before the push. He asked if the percentage bar glow is still there: it is code (463rd), in every theme.

**2026-11-12, 13:22 (MST), tasks 33 and 34.** George: "And all the same hue in the theme?" and "Definitely need bevels in the themes, all of them." Same-hue normalizer for the default theme (hue 227, sat 48); bevel edges in Krimble Painted (512th entry); the default sheet bevels of 7ba7266 go up with it. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 13:25 (MST), task 35.** George: "Can we reverse bevel on menus so they look recessed?" Menu overlay lines swapped (513rd entry). He also asked for the glow on all themes (proposal pictures themes_now.png and themes_with_glow_everywhere.png; not built) and how many themes: 11. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 13:43 (MST), tasks 4 and 5.** George: "4. Delete Krimble Blue. It looks bad." (514th entry: not installed, saved Krimble Blue reads as the default) and "5. match this version of QuickAdjust." (515th entry; picture quickadjust_compare.png). Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 13:51 (MST), push and build 57.** George: "perfect. Push all changes and start a build." Build number 57, label 1.0.57-beta; the PAT has no workflow scope, so the workflow file is unchanged and the run is started by hand with the label typed in (516th entry). Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 15:05 (MST), task 37.** George: thinner glow edges, no rounded corners in the toolbox glow. KoToolBoxButton: thinner layers and miter join (517th entry); picture toolbox_glow_thinner.png. Build 57: the PAT has no Actions or workflow permission (HTTP 403 on dispatch, push of the workflow file rejected), so the run is NOT started; it needs George (Actions > Krimble Android build > Run workflow, label 1.0.57-beta). Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 15:08 (MST), task 37 correction.** George: "That before image was fine. The one I didn\x27t like was much bigger, had way thicker sides." Toolbox glow thickness back to the Nov 11 values, sharp corners kept (518th entry); picture toolbox_glow_restored.png. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 15:23 (MST), task 38.** George: "yes. I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection." Step 1: Marquee, Lasso, Wand groups (519th entry). Real-Qt test of the button and flyout passed; KoToolBox.cpp not compiled with the Krita headers. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:05 (MST), task 39.** George: "Make new icons for new tools." 19 Full Color icons drawn (520th entry), in krita/pics/krimble-toolbox-color/new-tools/ (not registered anywhere); sheet new_tool_icons_sheet1.png. Waiting for his look at the sheet. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:13 (MST), task 39 correction.** George: "I didn\x27t want to go back to color icons." The 19 new tool icons redrawn in the Clear style (38 svg files, 521st entry); flyout of a tall list grows upward beside its button; mockup toolbox_two_columns_clear.png. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:15 (MST), task 40.** George: choice between flat buttons and raised buttons with thicker DPaint-style bevels for the toolbox. Toolbox menu > Button Style > Flat / Raised, saved (522nd entry); picture toolbox_button_style.png. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:20 (MST), task 39 second correction.** George: "color replacement looks like a Boolean again." Icon redrawn as brush plus a color swatch (523rd entry); preview color_replacement_icon.png. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:21 (MST), task 25.** George sent SmartCompression.png (the logo). Trimmed to 1104 x 449, stored as krita/pics/svg/dark_krimble-smartcompression-logo.png and registered in svg-icons.qrc (524th entry); not used yet (the SmartCompression tab is not built). Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:26 (MST), task 24.** SmartCompression step 1: the tab with the logo and the gray-as-one-channel option (525th entry). Correction: my estimate of a third to half was wrong, a test gave 3 to 8 percent. George added: a level control and a protected area shown as a red film (Quick Mask style); next. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:30 (MST), task 24a.** SmartCompression step 2: the level (Light/Medium/Strong/Maximum), the Smart Selection and the protected area in the JPEG save dialog; engine tested standalone (526th entry). Red film editing not built yet. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 18:48 (MST), task 41.** George: hex overlay lines too thin, no thickness or color control. View > Show > Hex Lines... (thickness, color, opacity; default 2 px) (527th entry). Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 19:30 (MST), task 42.** George: hexagon selection that snaps to the hex grid, with a lock (no rotating, scaling, line thickness or grid change until unlocked). Hexagon Selection tool in the Marquee flyout, snap tested against the overlay lattice, shared hex lock in View > Show (528th entry). Not compiled with the Krita headers. Pushed with the PAT George pasted earlier in this chat.

**2026-11-12, 20:30 (MST), build 57 started.** George pasted a new PAT (it can start runs). workflow_dispatch of "Krimble Android build" on master, label 1.0.57-beta: run 43359369032 on commit cdca33d (HTTP 204, in_progress at 03:30Z Nov 13). Contains everything pushed up to the Hexagon Selection tool (528th entry). Row 57 goes into KRIMBLE_BUILD_LOG.md when it succeeds.

**2026-11-12, 00:00 (MST), task 43.** George (new build): menu bar has no border, highlight or shadow at start, then becomes the painted version. Overlay now also on the menu bar, theme applied early, bevels installed at once (529th entry). Not in build 57. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:02 (MST), task 44.** George sent krita.log of build 57 (cdca33d), "weird toolbox stuff". Cause found: a floating toolbox at the default 640 x 480 window turned HORIZONTAL (updateToolBoxOrientation fallback); fixed (530th entry). Both sessions ended by Samsung background kills (AL_Kill), not crashes. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:05 (MST), task 45.** George: Preferences keyboard too small in a too wide column; selection darkens the icon; where is the bright blue selection. Keyboard fills the item width, column 24 percent, icon painted Normal, list selection #1d7cb2 again in the Nov 10 and Painted sheets (531st entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:11 (MST), tasks 46 and 47.** George (build 57 screenshots): flyout size; menu bar paint job removed for the default theme; K drawn by the bar at the left corner; palette backgrounds to hue 227 sat 48 (532nd entry). Still to do from his list: the New dialog size, Color Lab scaling like a picture, toolbox starting floating and crooked. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:13 (MST), task 48.** New dialog: column 14 to 18 percent (landscape), 26 percent (portrait), icons 44 px. First-run toolbox float switched off (533rd entry). Color Lab scaling like a picture: not changed, needs a redesign with real layouts; waiting for his go. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:17 (MST), task 49.** George: SmartCompression logo too big, too much explanation, not like XAT, JPEG dialog enormous. Compact XAT-style tab (quality, mart, Extra Color, Grayscale, Progressive, protect, sensitivity), logo 30 px, dialog capped to 94 percent of the screen (534th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:18 (MST), task 50.** George: do not start with the toolbox floating; dock left with 2 columns on start. One-time reset on the next build: columns 2 before the toolbox is made, docked left 2.5 s after start (535th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:19 (MST), tasks 50 and 51.** George: toolbox must dock left with 2 columns on start; SmartCompression logo now too small. One-time reset of the columns to 2 and docking at the left (setting ToolBoxDockedLeftReset); logo 54 px (535th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:19 (MST), correction.** The first push of tasks 50 and 51 (3b8f95f) had the toolbox reset code but NOT the logo change (my edit script stopped at the logo line); the 535th entry said both. The logo is 54 px in the next commit.

**2026-11-13, 00:22 (MST), task 52.** George: edit the selection starting from the smart selection; label without the percent sign. SmartCompression panel (docker plugin) written, Smart/Clear/Whole/Regions/tools/Save, selection = area to compress, film covers the protected area; label "SmartCompression" (536th entry). Panel NOT compiled with the Krita headers. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:23 (MST), task 53.** George: Color Lab color wells: no blue box, no glow. Outlines and glow removed from the two wells and the page preview boxes (537th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:25 (MST), task 54.** George: the SmartCompression panel must not be ugly. Rendered it in real Qt next to the theme, reworked it (header band with logo, joined mode pair, wide buttons with pictures, 2x2 tools grid, wide Save button) (538th entry); picture smartcompression_panel_compare.png. Pushed with the PAT George pasted in this chat.

**2026-11-13, 00:35 (MST), task 55.** George: Chatty's first mockup is pretty. SmartCompression panel rebuilt after it (look in Qt-only SmartCompressionPanel.h, rendered), live preview with sizes (libjpeg in memory, tested), Save JPEG hands the settings to the JPEG dialog (539th entry). The dock is NOT compiled with the Krita headers. Pushed with the PAT George pasted in this chat.

**2026-11-13, 02:50 (MST), build 58 prepared.** Build 57 (run 43359369032) succeeded in 57 minutes; row added to KRIMBLE_BUILD_LOG.md; krimbleBuildNumber 58. Contains everything pushed through 125b7b3 (the SmartCompression panel is the first code that was never compiled with the Krita headers: expect possible compile errors). Pushed with the PAT George pasted in this chat.

**2026-11-13, 02:53 (MST), release 57.** George: "Update for 57 and post it on Github." Pre-release v1.0.57-beta created on commit cdca33d with notes (tag on that commit), no APK attached: the artifact krimble-apk of run 43359369032 (182 MB) cannot be downloaded from Claude's sandbox (the artifact host is blocked). Build log row 57 marked released. Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:06 (MST), release names.** George: where is the v / why the inconsistency / "Do it again." Titles of 18 releases set to Krimble v1.0.N-beta (tags, notes, APKs untouched), rule written into BUILD_ANDROID.md and SESSION_CATCHUP.md, tools/ci/check_release_names.py added (540th entry). Noted: build 58 was started before release 57 was posted, against the publish-first rule. Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:06 (MST), build 58 finished.** Run 43404551956 (commit 1c86669, label 1.0.58-beta) succeeded; the SmartCompression panel and the other code that had never been compiled with the Krita headers compiled. Row 58 added to the build log (not released yet). Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:09 (MST), decisions.** George: both (a decisions file and a workflow check). KRIMBLE_DECISIONS.md, tools/ci/check_decisions.py, workflow step "Check the decisions (a warning)", SESSION_CATCHUP pointer (541st entry). The check finds 30 mentions of Photoshop in KRIMBLE_CHANGES.md (not changed). Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:10 (MST), build 59 started by the push.** The push of the workflow with the decisions check started run 43406947650 with the old label; it stopped at the build number check (expected). Label and number moved to 59; this push starts build 59 (542nd entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:12 (MST), annotations.** The decisions check step shows green on GitHub even when it fails (continue-on-error); both scripts now also print ::warning:: annotations (543rd entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:40 (MST), tasks 56 and 57.** George: tabs never tear off; Raised buttons darker, highlight not white; start screen menu bar not distinct. New app-level tab tear-off filter with touch and a KRIMBLE-TABS log (tested in real Qt), Raised look lighter with a near-white highlight, flat lighter menu bar, K repaint at start (544th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 03:42 (MST), task 58.** George: Layers panel is a mess, no design or spacing. Header regrouped as one aligned grid (blend box and opacity bar share edges, 34 px rows, 6 px spacing, dividers) (545th entry). Not rendered. Pushed with the PAT George pasted in this chat.

**2026-11-13, 04:09 (MST), task 59.** George: the right panel forces the picture to zoom out. Zoom frozen (fit mode becomes constant, picture kept in place) when the canvas area changes without the main window changing (546th entry). Pushed with the PAT George pasted in this chat.

**2026-11-13, 04:24 (MST), task 60.** George: selected menu item solid light blue, no gradient. QMenu::item:selected is flat #0b87dd in the default and Painted sheets (547th entry). Pushed with the PAT George pasted in this chat.
