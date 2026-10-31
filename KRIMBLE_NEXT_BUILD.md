# What is in the repository but NOT in the 1.0.43 build

Written 2026-10-25, 2:55 PM Mountain Time. The 1.0.43 build on George's phone was compiled from commit **`d6b6b48`**. Everything below was pushed after it. **None of it has been compiled or run on a phone** (the sandbox cannot compile the app; changes were syntax-checked with `tools/syntax-check/syntax_check.py` where that works, and the benches in `tools/*-bench` run the new windows and engines on a desktop with Qt 5.15). The next build is the first time it is all built together, so **the compile may fail**: if it does, George sends the last 30 lines of `~/build-latest.log`.

**The version number in the repo is now 44** (entry 303, 2026-10-25, after George said "Let's build"). 1.0.43 was published on GitHub on 2026-10-25 at 3:09 PM MT (APK attached), so the process rule was satisfied. This list is what **1.0.44** adds to 1.0.43.

## The changes (entry numbers are in `KRIMBLE_CHANGES.md`; times in `KRIMBLE_WORKLOG.md`)

| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 284 | **Batch Process with recipes**: resize, JPG / PNG / WebP lossless / WebP lossy, quality, name ending; WebP read and written with transparency; saved recipes | Image > Batch Process... | links libwebp (`WebP::webp`); saving into Pictures/Krimble/&lt;folder&gt; is Android code never run |
| 285, 288 | **Krimble Script**, 15 commands (`resize`, `fit`, `crop`, `rotate`, `flip`, `gray`, `brightness`, `contrast`, `flatten`, `format`, `name`, `folder`, `frames`, `fps`, `loop`); animated GIF and WebP read, animated WebP written, `frames` writes numbered pictures | "Use a script instead" in Batch Process; `KRIMBLE_SCRIPT.md` | needs libwebp demux and mux; **Qt's GIF reader may be missing on Android** (GIFs would be skipped) |
| 286 | **Help > Krimble Help**: the in-app handbook (with the script reference), Go to list, Find | Help menu | the file is read from the app's resources (`:/krimble-help.md`); panel commands are said to be in the Window menu (code comments say Settings) |
| 290 | **Eraser tool** in the toolbox (after Soften); turns on the eraser switch while it is the tool | toolbox | tool or icon missing; paints colour; Brush erases afterwards |
| 291 | the toolbox / panel **edge tab is not drawn** any more; its touch zone is unchanged; the little dots stay | both panel edges | the zone no longer shows where to grab |
| 292, 293, 294, 295 | icons: George's **keyboard** (Preferences > Keyboard Shortcuts), **house** (Preferences > General), **clipboard** (File > New > Create from Clipboard); 26 unused art files removed | Preferences, File > New | none known |
| 296, 297 | **New window**: labels beside their fields, Cancel and Create side by side. The global rule that put labels above fields and stacked rows of three now applies only to windows with no scroll area | File > New > Custom Document, and the other windows (Image Size, Canvas Size, export options, File Information...) | some windows may now be wider than the screen (they scroll sideways); Profile row in the New window is the widest |
| 298 | **Polygonal Selection**: a bar over the picture with Undo point, END, Cancel while a polygon is being made | Polygonal Selection tool | the bar may cover points near the bottom edge, or take a touch meant for a point |
| 299 | **Crash fix, the likely cause of the relaunches**: three native functions kept the old `org_krita` names; `hasMainWindowLoaded()` threw on every Back press and back gesture, so Krimble crashed and started again. Java no longer crashes on a missing native. `tools/check-jni-names.py` checks the names | leaving Krimble with Back or the back gesture; switching apps | `saveState()` now works again on every pause: it saves state through the service when Krimble goes to the background (a saving notice may flash) |
| 300 | wider list of configuration changes the activity handles itself; **Help > Show Krimble Log...** shows how each earlier run ended and why Android last destroyed the window | Help menu | none known; read the log if relaunches continue |

Also in the repo: the roadmap section "Animation and game sprites (proposed)", `KRIMBLE_SCRIPT.md`, `KRIMBLE_WORKLOG.md`, test list items 54 to 67.

## What George should test first (in this order)

1. Does it still relaunch when he switches apps? How does he leave Krimble: Back or the back gesture, Home, Recents, another app? If it still relaunches: **Help > Show Krimble Log...** and a screenshot of the first screen.
2. File > New > Custom Document: is the window shorter, are the labels beside the fields, are Cancel and Create side by side?
3. Polygonal Selection: the bar (Undo point, END, Cancel); does END close the selection?
4. The Eraser tool; Image > Batch Process... with a PNG that has transparency to WebP.
5. Help > Krimble Help.

## Known not built, and open questions

- Not built: writing an animated GIF, APNG, frames in (numbered pictures to an animation), the animation timeline tools, sprite sheets, the Magnetic Selection bar (it still needs Enter and Escape), partial-text styling in the Type tool, Auto Color as a vibrance-style default, Quick Mask design, the user manual.
- Open questions to George: hide the status bar by default? what is the teal teardrop in the Layers list (probably Android's text-cursor handle)? which on-screen messages are unnecessary? did "the handle" mean the edge tab? the Type tool "red box" (needs a screenshot); the polygon Tool Options buttons were not seen in 1.0.43 (probably clipped in the narrow panel; a screenshot would show).

## On the branch `feature/krimble-ui-refresh` (NOT on master; written 2026-10-30, 12:37 PM MT)

None of this is compiled or run on a phone. Entry numbers are in `KRIMBLE_CHANGES.md`; times in `KRIMBLE_WORKLOG.md`. Before it can be built from master the branch needs a test merge (master has deleted `krita/pics/krimble-toolbox`; the branch has now deleted its 32 PNGs too).

| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 324 | QuickAdjust heading reads "QuickAdjust" | Image > Adjustments > Quick Adjust... | none known |
| 325 | classic color selector hidden; Color docker has no outer tabs (Color Studio's own Picker/Mixer tabs remain) | Color panel | the hidden classic widgets could draw stray on the picker |
| 326 | Scale mode: outside the box does nothing, no rotate arrows, Alt no longer rotates the bounds | Transform tool | inside the box still moves |
| 327 | the Rotate tool uses its dial only in the Free mode; other modes act as the Transform tool | Rotate tool, Warp/Cage/Liquify/Mesh/Perspective buttons | mode switching |
| 328 | seventh mode button **Rotate**; first button is **Scale**; Rotate and Scale switch between the two tools | Transform Tool Options | tool switch applies the stroke; the button state in the Rotate tool |
| 329 | **Thickness** slider and number box = the current brush's size | Line tool Tool Options | changes the brush's size for every brush tool |
| 330 | Brushes panel: **Size, Opacity, Flow** rows above the presets; a 0.4 s timer keeps them in step | Brushes panel | the preset picker below is unchanged Krita |
| 331 | **Layer > New > Star...** (Points, Inner circle, Size): a vector star, foreground fill, no outline, on a new vector layer | Layer > New | rotate handles may be small; the star is not auto-selected |
| 332 | the 32 old toolbox PNGs deleted from the branch | none | none (they were not used) |

Also on the branch from ChatGPT (reviewed 2026-10-30, not changed): the app-wide stylesheet (`KrimbleVisualStyle.h`; known faults: combo/spin arrows, checkbox indicator, slider handle height, hardcoded colors), Color Studio (picker and mixing pad in the color docker, per-pixel SV square), QuickAdjust spinboxes and Preview, Preferences icons 44 px, build number 45, packaging scripts, `RELEASE-CHECKLIST.md`.

### More on the branch `feature/krimble-ui-refresh` (entries 334 to 342, written 2026-10-30, 12:55 PM MT)

| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 334 | QuickAdjust: **Keep as layers** (off at first; off merges the adjustment layers down into the paint layer below on OK); the earlier layer is active again; **Quick Adjust...** first in the Image menu | Image menu, the QuickAdjust window | merge down onto a paint layer; a layer not on a paint layer stays a layer |
| 335 | the Brushes panel is hidden every time it shows by itself; the Window menu opens it | make a new file | a saved layout with Brushes open no longer shows it |
| 336 | Brushes panel: read-only **Brush** name field | Brushes panel | none known |
| 337 | stylesheet: **all tool buttons**, toolbox and panels included, get the look; the selected tool has the blue border | the toolbox | toolbox buttons may change size, icons may clip |
| 338 | floating panels dock at a window side only after the finger stays within reach for 350 ms | drag a panel to an edge | 350 ms is a first guess |
| 339 | six stale comments corrected | none | none (comments) |
| 340 | File > New: list icons at most 64 px, column 30% | File > New | the size is a choice from the history |
| 341 | Color panel: one page; **SV square painted with gradients**, grows both ways | Color panel | the classic widgets are hidden, not removed |
| 342 | **Mixer** panel (its own dock, `KrimbleMixer`, header only) | Window menu | a new dock registration: check it appears and opens |

Not in the branch yet: the 32 icons wired to the toolbox (see `packaging/android/TOOLBOX-ICON-COVERAGE.md`).

### Also on the branch (entries 344 and 345, written 2026-10-30, 2:40 PM MT)
| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 344 | progress bars, slider fills and number-slider fills are Krimble orange `#FD6001` | any progress dialog, Tool Options sliders, Brushes panel | white text over the orange (contrast about 3.4 to 1) |
| 345 | new main splash (1672 x 941, JPG quality 88 is the packaged one) | start screen, Help > About | about 18% wider than the old splash at the same height |

### Also on the branch (entries 347 to 349, written 2026-10-30, 2:55 PM MT)
| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 347 | the status bar memory bar starts in Krimble orange | status bar | none known |
| 348 | toolbox icons: **Full color** (32 pictures, `krita/pics/krimble-toolbox-color/`, a new resource file in `krita/CMakeLists.txt`) or **Silhouette** (the old icons), switched in the toolbox menu | touch and hold the toolbox | a new resource list in CMake; two mappings are guesses (Smart Patch, Smudge); the 240 px pictures are scaled down by Qt |
| 349 | blue glow (drop shadow effect) on checked tool buttons and push buttons | the selected tool, option buttons | clipping, speed, a glow that stays |

### Also on the branch (entries 351 and 352, written 2026-10-30, 6:35 PM MT)
| Entry | What | Where to look on the phone | Risk |
|---|---|---|---|
| 351 | the second toolbox icon style is named **Silhouette** (was Clear) | touch and hold the toolbox > Icon Style | none known |
| 352 | **Quick Adjust completed**: a picture on each row, a rainbow Hue slider, an ON/OFF Preview switch | Image > Quick Adjust... | the switch replaced a check box; the pictures are drawn in code |

| 357 | **Colorize Mask Editing** and **Enclose and Fill** commented out of the toolbox (not registered); six comments corrected | the toolbox | a colorize mask can not be edited with a tool until it comes back |
