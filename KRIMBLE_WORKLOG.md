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
