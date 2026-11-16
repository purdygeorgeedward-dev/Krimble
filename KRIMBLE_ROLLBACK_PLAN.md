# Krimble: every revision after 1.0.58, and how to redo them after a rollback

Written 2026-11-16, 15:00 MST, at George's request ("Please make a list of all revisions requested/attempted in subsequent versions and formulate the best implementation if a rollback is performed.").

- **1.0.58-beta** = commit `1c86669` (build 58, Nov 13). George is using it because it is stable.
- After it: builds 59 to 64 (`77dcb3e`, `12b860a`, `a88d845`, `69d2873`, `f8edc72`, `4ba8bb1`), then entries 712 to 724 (pushed, not built).
- In all: 188 commits, 52 source files changed (about 3,000 lines added), plus resources removed.

**Nothing is rolled back unless George says so.**

---

## 1. What most likely made builds 59 to 64 unstable

Neither cause below has been confirmed on a phone. No crash log from builds 59 to 64 has been read.

1. **The canvas stretched under the panels (`KrimbleCanvasOverlay`, `KisMainWindow.cpp`).** It went into the first build after 58, on Nov 13 ("panels lay over the canvas"). It fights Qt's own layout. Every time the window was laid out again, the canvas was moved twice: first by Qt, then back by Krimble, a moment later. That caused the jolts while painting (item 1 of the 64 list). The 722nd entry makes the second move happen at once. If the crashes stay, this is the first thing to switch off. The switch is the setting `krimble/PanelsOverlayCanvas = false`; making that the default is a one-line change.
2. **Resources taken out:**
   - In build 60: patterns cut to five. This was not asked for. The 724th entry restores 1.0.58's full list.
   - In builds 62 to 64: brush presets cut to five, and the default bundles deleted. Both were asked for ("Now delete bundles."). A saved brush, pattern or tool setting from 1.0.58 that names a removed resource is the kind of thing that crashes a fill or a brush.
3. **Many timers and event filters added to the main window:** the menu placer, bevel overlays, dock edge grabs, tab titles, the toolbox snap. Each runs during touches. The touch-time ones are the next suspects.

**How to find the real cause before a rollback:** after a crash, open Krimble again, go to **Help > Show Krimble Log...** and send a screenshot. It shows how the last run ended.

---

## 2. All revisions after 1.0.58, grouped

Status key:

- **asked** = George asked for it.
- **mine** = Claude did it without being asked.
- **fix** = a repair of an earlier change.
- **built** = in a build George tested.
- **unbuilt** = pushed only.

### A. Toolbox

| # | Revision | Status | Redo after rollback? |
|---|---|---|---|
| A1 | Toolbox order as George's list (Move ... Transform, UNDO, colors), set with the tools' own `setToolBoxPriority` places | asked (64 list item 9); 722nd, unbuilt | **Yes.** Only the priority numbers and the group table in `KoToolBox.cpp` |
| A2 | Hand is the starting tool | asked (original goal; 64 list item 8); 722nd, unbuilt | **Yes.** One change in `KoToolManager.cpp` (start with `PanTool`) |
| A3 | Flyouts: long-press groups (Marquee, Lasso, Wand, Eyedropper, Gradient, Pen, Type, Shapes, Transform, Blur) | asked; built 59 to 64 | **Yes.** `KrimbleToolFlyout.h` is self-contained |
| A4 | Flyout delay 250 ms | asked; 722nd, unbuilt | Yes |
| A5 | Type tools named "Krimble Type" and "Old Type" | asked; 714th, unbuilt | Yes |
| A6 | bandaid and stamp each on their own button | asked; 715th and 717th, unbuilt | Yes (part of A1) |
| A7 | Screen Mode button removed | asked; 716th, unbuilt | Yes |
| A8 | New Sharpen tool; the Soften tool named Blur | asked (list item 9); 722nd, unbuilt | Yes |
| A9 | Polygon and Star tool and its icon | tool built 59+; icon 722nd, unbuilt | Yes |
| A10 | UNDO button (raised box, no hover) | built | Yes |
| A11 | Docking rules: one place per side, snap flush, no height cap, columns snap | mine and fix, 627th to 708th; built, unreliable ("unpredictable results") | **Redo only what George asks for**, one at a time, tested |
| A12 | Raised button style in all themes but Dark Gray | asked (64 list item 5A); 722nd, unbuilt | Yes |
| A13 | Default brush when a painting tool starts empty | asked (64 list item 10); 722nd, unbuilt | Yes |

### B. Menu bar and window

| # | Revision | Status | Redo? |
|---|---|---|---|
| B1 | Orange K drawn on the menu bar, menus clear of it in every state | asked; built (with no image it was wrong); fixed in the 722nd with a corner holder, tried in real Qt | Yes, the 722nd form only |
| B2 | Image window buttons centered in the bar | asked; 722nd, unbuilt | Yes |
| B3 | Menu bar 4 units taller | asked; 722nd, unbuilt | Yes |
| B4 | Menus open under their bar name and fit on screen (`KrimbleMenuPlacer`, `krimbleMenuDeskFrame`) | asked; built, partly working | Yes, but test the Window menu |
| B5 | Image tabs at the top (tabs the default) | asked (list item 15); 724th, unbuilt | **Yes.** In 1.0.58 floating windows were already the default (since Oct 11), so this is needed after a rollback too |
| B6 | Panels lie over the canvas instead of shrinking it (`KrimbleCanvasOverlay`) | asked ("BUILD NOW" item 6, Nov 13); built 59+; cause of the jolts | **Redo only after a phone test**, and make it switchable from the Window menu |
| B7 | Status bar size figure fits in portrait and no longer resizes while painting | asked (list item 12); 722nd, unbuilt | Yes |
| B8 | QuickAdjust tool bar removed | asked (list item 7); 722nd, unbuilt | Yes |

### C. Themes and look

| # | Revision | Status | Redo? |
|---|---|---|---|
| C1 | Themes: Blue-Gray, Dark Gray (new, flat), Painted; Krita-named and Bevel themes removed; files named `Krimble*.colors` | asked; built 62+ | Yes |
| C2 | Bevel lines (3 px, four tones, 45 degree joins) on panels and the toolbox; none on menus, maximized windows or thin bars | asked, many rounds; built | Yes, with the final rules only (one change, not twenty) |
| C3 | Selected tab joins its panel; in windows, tab and page are #364064 | asked; 697th built, 722nd unbuilt | Yes |
| C4 | Selection gradient (#1196D8 to #074A85) on lists and menus | asked (list item 5C); 722nd, unbuilt | Yes |
| C5 | Ball bearing slider handles; the smaller one darker with a cast shadow | asked; built, then 722nd unbuilt | Yes |
| C6 | Buttons less rounded (radius 3) | asked; built | Yes |
| C7 | Darker base screen and recent files area | asked; built | Yes |
| C8 | Layers panel: gray rows, lighter selected row, no glow, search box | asked; built | Yes |

### D. Windows and panels

| # | Revision | Status | Redo? |
|---|---|---|---|
| D1 | QuickAdjust: compact rows, natural size, Preview switch in the bottom row | asked; built | Yes |
| D2 | New Image dialog: no wider than 75% of the short side; bigger icons | asked; built | Yes |
| D3 | Preferences: orange K keycap icon, page list highlight | asked; fixed in the 706th (never ran before that) | Yes |
| D4 | Save As shows every file type | asked; built | Yes |
| D5 | Color Lab: name shown once, size limit 384, ball bearing sliders, recent colors not reordered | asked; built | Yes |
| D6 | File Info > General icon | asked; 722nd, unbuilt | Yes |
| D7 | Panel tab tear-off and header tab | tried, then turned off (685th) | No |

### E. Content and size

| # | Revision | Status | Redo? |
|---|---|---|---|
| E1 | Nine largest font families deleted (Montserrat kept) | asked | Yes |
| E2 | Bundles deleted; no message about missing bundles | asked | Yes. **First check** that no tool needs a resource from them (the crash suspect) |
| E3 | Brush presets cut to five | asked (with the bundles) | Yes, but keep the preset the program names as its default ("b) Basic-5 Size Opacity") |
| E4 | Patterns cut to five | mine; restored in the 724th | No |
| E5 | No mention of the brand name anywhere; "Adobe RGB" kept | asked; 723rd | Yes. Script over the repository; file format ids kept |

### F. Documents (no effect on the app)

Release notes 58 to 64, the help file checked against the code, the manual, build log, decisions, conversation log. **These stay as they are.** A rollback is of the source code only.

---

## 3. Best way to do a rollback, if George orders it

1. **New branch `rollback-58` from `1c86669`.** Master and its history stay as they are; nothing is lost.
2. Bring the documents over unchanged (section F).
3. Build 1.0.58 from the branch as a check: it should behave like George's phone.
4. Redo the revisions in **small batches, one build each, each tested on the phone before the next**. Suggested order: what is safe and asked for first; what touches layout and touches last.
   1. A1, A2, A4 to A8, A13 (toolbox order, Hand first, flyouts, tool names). Pure tool data, low risk.
   2. B5, B7, B8, D6, E5 (tabs, status bar, toolbar removed, icon, brand name).
   3. C1, C3 to C8 (themes and style sheets).
   4. B1 to B4 (menu bar).
   5. D1 to D5 (windows).
   6. E1 to E3 (resources), with a fill and brush test right after.
   7. C2 (bevels), A11 (docking), B6 (panels over the canvas): the event filters and timers, **each in a build of its own**.
5. Each batch is moved over by hand from the master commits listed in `KRIMBLE_CHANGES.md` (entry numbers), not by cherry-picking whole days. Many of the later entries repair earlier ones, and only the final form is wanted.
6. After each batch, George tests. Only George decides when an item is resolved.
7. When the branch has everything George wants and is stable, it becomes master. The old master is kept as a tag `pre-rollback-64`.

**Without a rollback:** the same order applies to finding the crash. Build master once (65) with the 722nd to 724th. If it still crashes, build 66 with `PanelsOverlayCanvas` off by default. If it still crashes, read the log from Help > Show Krimble Log.
