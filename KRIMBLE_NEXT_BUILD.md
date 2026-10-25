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
