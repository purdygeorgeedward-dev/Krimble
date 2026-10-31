# Krimble Status Index

Read this first. One line per item — only open KRIMBLE_CHANGES.md if you
need the full explanation, reasoning, or diff for a specific fix. Update
this file whenever a bug's status changes; regenerate from
KRIMBLE_CHANGES.md if this file and the changelog ever disagree.

## Now (2026-10-30, updated 2:55 PM MT): branch `feature/krimble-ui-refresh`
- **Branch head:** `a9c18e1b67` (before the docs commit). Changelog entries **324 to 332** are on this branch only, **not on master, not compiled, not run on a phone**. The branch was cut from `3cf6fc0`; master has moved on (13 commits at last count, all the `krita/pics/krimble-toolbox` folder). A test merge into master was **not done** (the checkout timed out); do it before merging. The docs on this branch are as they were at `3cf6fc0` plus this day's additions: master's copies may be newer.
- **Done on the branch (324 to 349):** QuickAdjust heading is just "QuickAdjust" (324); classic Krita color selector hidden (325); Scale mode has no rotating and the Transform Tool Options have a seventh button, **Rotate** (326-328); Line tool **Thickness** (329); Brushes panel **Size, Opacity, Flow** rows (330) and a **brush name** field (336); **Layer > New > Star...** (331); the 32 old icon PNGs deleted (332); docs (333); QuickAdjust **Keep as layers** toggle (off: merged down on OK), the earlier layer active again, and Quick Adjust first in the Image menu (334); the Brushes panel **never appears by itself** (335); stylesheet **tool buttons app-wide**, toolbox included (337); panels **dock only after a 350 ms dwell** at a side (338); six stale "excluded from toolbox" comments corrected (339); File > New icons at most 64 px, column 30% (340); Color panel **one page** with a **gradient-painted, growing SV square** (341); the **Mixer** as its own panel (342); progress bars and slider fills in **Krimble orange `#FD6001`** (344); the **new main splash** (345); the accent rule, the status bar memory bar in orange (347); **toolbox icons: 32 Full color pictures wired, with a Full color / Silhouette switch in the toolbox menu** (348); **blue glow on checked tool buttons and push buttons** (349).
- **Answered by George (2026-10-30, after 12:35 PM MT):** QuickAdjust: a toggle, not layers by default (done, 334); the Brushes panel attached to an **edge** (dwell added, 338); Line Thickness as the brush's own size is fine; stylesheet: do not exclude the toolbox (done, 337); the brush name field instead of a guess (done, 336). **Still open:** is the object scaled a layer (answered in chat: the active layer, or the selected pixels on it); Scale: dragging inside the box still moves (unchanged).
- **Approved, not started:** smaller sizes of the icon pictures by script (the 240 px pictures are scaled by Qt now); icon pictures for Dodge, Burn, Colorize Mask Editing and Enclose and Fill (none drawn); the other stylesheet faults (unverified guesses, not changed).
- **On hold (George, 2026-10-30):** Rounded Box, the Tail option (on Rectangle, Ellipse, Rounded Box), Speech Balloon. **Not toolbox tools**: the toolbox is full.
- **Details:** `SESSION_CATCHUP.md` (decisions), `KRIMBLE_NEXT_BUILD.md` (what to look for on the phone), `KRIMBLE_WORKLOG.md` (push times), `KRIMBLE_TEST_LIST_2026-10-23.md` (branch list B1 to B12), `packaging/android/TOOLBOX-ICON-COVERAGE.md` (icons).

## Older "Now" (2026-10-25; the section above is newer)
### Now (2026-10-25, 2:55 PM MT)
- **Released:** v1.0.29-beta to **v1.0.44-beta** (1.0.44 published by George 2026-10-26, 9:32 PM MT = 03:32 UTC on the 27th; APK `Krimble-1.0.44-beta-Oct25-2229.apk`, 181,273,588 bytes, attached; tag on `f3960e9`, the build commit; verified through the API). 1.0.44 is installed on George's phone. 1.0.43 was published 2026-10-25, 3:09 PM MT.
- **In the repo, not in 1.0.43:** see `KRIMBLE_NEXT_BUILD.md`. Repo version number: still 44 (the next build, 1.0.45, needs the number bumped first). 20 commits (entries 305 to 322) are in the repo and not in 1.0.44.
- **Biggest open problem:** Krimble relaunches when George switches apps (the picture is gone). Likely cause found and fixed in the repo (entry 299: native functions with old Krita names crashed the app on every Back press); a log window (Help > Show Krimble Log...) was added for evidence (entry 300). **Update 2026-10-26 (George sent the log screen of 1.0.44): the restarts are Android's low-memory killer, not crashes:** both recorded ends were "reason 3 (LOW_MEMORY)": one while Krimble was cached in the background (importance 400, "sits in the background too long"), one while it was in the foreground ("[OOM KILL] oom", importance 100: the "crash in the type tool"). So the 299th fix was not the (only) cause: Krimble holds too much memory for Android. Next: the memory trail (entry 319) shows what holds it; then lower it (and swap or release memory when going to the background).
- **Every push with its time:** `KRIMBLE_WORKLOG.md`. **Full explanation of each change:** `KRIMBLE_CHANGES.md`. **George's test list:** `KRIMBLE_TEST_LIST_2026-10-23.md` (items up to 67). **Where to pick up:** `SESSION_CATCHUP.md`.

## Older list (September 2026; kept as it was, not kept up to date)

## Bugs
1. Krita icon instead of Krimble icon — FIXED `a4a5b53`
2. Brush presets open on load — LIKELY FIXED `a73387e`, unconfirmed on-device
3/6. UI-scale-on-startup dialog / zoom scale — FIXED (2026-09-03)
4. Contrast doesn't reach full black/white — FIXED (2026-09-03)
5. Resize handles hard to grab — OPEN, not investigated
7. Window docking broken — FIXED `c50f03a`, unconfirmed on-device
8. Transform tool does nothing — FIXED `05d1fc1`
9. Brush painting lag — OPEN, needs on-device profiling
10. Type tool does nothing — OPEN, needs on-device repro; tool disabled in KoToolRegistry for now
11. Cartoon mascot in support screen — USER-OWNED, not Claude's task
12. Docker panels render as unmanageable strips — FIXED, two contributing bugs: `c50f03a` (global setAllowedAreas contradiction) + `af46ff2` (4 dockers defaulting to DockRight). Unconfirmed on-device.
13. File dialogs wrong default size — FIXED `4016d2b`
14. Dialog size/position not persisted — FIXED `c27416b`
15-17. Convert to Profile / Shortcuts / Toolbar customization missing from Edit menu — FIXED `623f3c5`
18. Assign Profile doesn't exist — OPEN, not investigated
19. Purge missing — NOT A BUG, already exists in Image menu
20. Step Forward/Backward missing — NOT A BUG, Undo/Redo already covers it (Krita's Undo isn't a toggle)

## Standing rules
12 rules governing all Krimble work — see "Standing rules for this fork"
section in KRIMBLE_CHANGES.md. Not duplicated here to avoid two copies
drifting out of sync. Key ones worth remembering without opening that
file: no change without showing the exact diff and getting explicit
approval; no batching multiple files under one approval; every fix needs
both an inline code comment AND a KRIMBLE_CHANGES.md entry; never write
"Photoshop" in README.md/KRIMBLE_CHANGES.md (use "industry standard");
mobile-first design priority, then industry-standard parity.

## Website / Forum
- krimble.org: LIVE, images uploaded and confirmed rendering
- forum.krimble.org: IN PROGRESS — Flarum zip uploaded to
  `~/krimble-forum/`, extraction in progress as of 2026-09-10, DB
  `dcitzdte_flarum` already created with full privileges, subdomain
  Document Root not yet pointed at `public/`

## Build
- Server: Oracle instance `instance-20260819-1011`, NDK `27.3.13750724`
- Last confirmed: `libkrita_arm64-v8a.so` compiles and links clean
  (2026-09-06). No confirmed packaged APK yet — full cheat sheet at
  `claude/KRIMBLE-BUILD-CHEATSHEET.md`

Last updated: 2026-10-30 (the branch section at the top); the 2026-10-25 "Now" part and the bug list below are older (bug list: 2026-09-10)

## Closed or decided by George, 2026-10-30, 6:35 PM MT
- **Scale** acts on the active **layer** (the selected pixels of it when there is a selection) on a raster layer, and on the selected **objects** on a vector layer.
- **The transparent default brush is fixed** ("back to normal").
- **Rounded Box is not a tool.** It is a setting in the Rectangle tool's options (the box preferences). The **Tail** and the **Speech Balloon** are features to add to a box or an oval; **no separate tool and no separate toolbox icon** for any of them. Still on hold.
- **The stylesheet "faults"** (combo and spin arrows, checkbox indicator, slider handle height, colors vs theme) were my guesses from reading the code, never seen on a device: dropped unless George sees a problem.
- **Quick Adjust:** complete the ChatGPT dialog, do not replace it (352nd entry). George, 2026-10-30: "I Did not say the mockups logo is left out. I said it shouldn't say KRIMBLE." So the heading must not say the word Krimble; the mockup's logo **mark** (the K and paw) is wanted and **not yet in the heading** (its artwork is not a picture file the program can load; the launcher icon art is an Android resource).
- **The Play Store .aab** made before is out of date; **another is made when release candidate 1 (rc-1) is reached.**
- **The full-APK 16 KB alignment check was completed.**
- **The Flarum forum setup was completed.**
- **Channels docker:** the panel **exists** (George, 2026-10-30: "I think the channels docker/panel exists?" Yes: it has per-channel thumbnails and visibility boxes). Only **Ctrl+click to load a selection** is dropped (no keyboard). Single-click isolation (hide the other channels) is **not decided**; the line that said the Channels docker was dropped was wrong.
- **Dodge, Burn, Colorize Mask Editing and Enclose and Fill (the four tools with no new picture): George, 2026-10-30, 7:04 PM MT: "The four tools you listed are supposed to be commented out."** They are **still registered in the toolbox in this branch**; the 339th entry and the six code comments it changed wrongly say they are not excluded. **Not yet changed; waiting for George to confirm which four tools** (the list he means is the one in the message he answered: Dodge, Burn, Colorize Mask Editing, Enclose and Fill; the older notes in `packaging/android/TOOLBOX-ICON-COVERAGE.md` name Colorize, Enclose and Fill, Ruler/Measure and Reference Images). If George's smaller icons look soft, he reworks them.
