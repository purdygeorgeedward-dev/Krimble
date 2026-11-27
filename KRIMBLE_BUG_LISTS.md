# Krimble bug lists

Every bug list from testing a build, filed under the build it was found in, reworded as plain issue statements.
Release notes take their Known issues from here. Newest first. The status of each fix is in KRIMBLE_CHANGES.md.

## Special test build 81, 1.0.81-test-no72 (tested Nov 27, 2026, 9:21 AM MST; screenshots docs/krimble-refs/2026-11-27_0921_build81-*)

Build 81 ignored the instructions and rebuilt the same bug we were trying to kill: it still carried 1.0.72 code. Its errors:

1. Buttons have different shapes and styles throughout the app.
2. The grip on the toolbox is bad.
3. The vertical dividers in menus do not reach the bottom.
4. Toolbar grips are black instead of white.
5. A white frame shows around dialogs (Save options, Preferences, Configure Toolbars, New File).
6. Aqua blue still shows: Change Icon when pressed, the selected Exposure and Gamma row, the Color Lab Picker tab.
7. Buttons are not consistent: bevel pills, square buttons (Clear, the arrow pad, Apply) and black pills (Canvas Input rows, New File, Color Space Browser); the pills are small; some have color and some not.
8. Preferences: Restore Defaults, Cancel and OK are stacked as full-width bars; the page list is squeezed to a sliver and its icons are tiny.
9. Configure Toolbars upright: the top is cut off, "Toolbar:" overlaps, Available actions has blank rows. It should display upright, with at least the two bottom buttons side by side.
10. Recent Images shows blank thumbnails.
11. The image window's minimize, restore and close buttons sit in the menu bar over Help.
12. The toolbox top shows a small dotted grip with a scroll arrow.
13. Color Lab's panel color does not match the panel beside it.
14. New File leaves a hole in the window below Cancel and Create.
15. The Layers panel has an empty row under the layer.
16. The Layers options menu (the side menu) shows its section titles as bare lines, with no labels.

## 1.0.80-beta (tested Nov 26, 2026, 2:36 PM MST; screenshots docs/krimble-refs/2026-11-26_1436_*)

1. The New File dialog is very tall no matter what is done to it.
2. All buttons should have pill curves, like the reference (2026-11-26_1436_pill-button-curve-reference.jpg).
3. Aqua in or out is undecided. Plain beveled shaded buttons are okay. Wanted: a demo of Aqua with the dark end of the gradient darker and more saturated. Otherwise leaning to a more professional look with the old glow for the selected button; the toolbox shows selection with a dark tool button instead of a glow.
4. Wanted: a fully transparent Cyber (no opaque anything) to look at, with larger text.
5. Multitasking is working better: the New dialog was left open a long time and Krimble came back without restarting.
6. Review all proposed bug fixes and changes.
7. QuickAdjust has lost the resize gadget on its corner. The OK button is out of the way, but the selection glow is not as good as the earlier version's.
8. Preferences icons need to be smaller.
9. In Krimble Light the bottom icons of the toolbox disappear.
10. The lines of the eyedropper icon are too thick.
11. The Configure Toolbars window is far too big; it should be small. No stacked, very wide buttons.
12. The Toolbars submenu causes a visual anomaly.

## 1.0.80-beta, later report (Nov 26, 2026, 6:30 PM MST)

- The blurry, upside-down image is back. Many images were loaded at once; the Layers panel was hidden over and over until it finally went away, then the image showed blurry and upside down. George thinks it may have something to do with memory.

## Status of the 1.0.80 list (Nov 26, 2026, 3:20 PM MST; pushed to cyber-theme, not built)

| # | Status |
|---|---|
| 1 | Fixed, 904th. |
| 2 | Fixed, 901st (pill buttons). |
| 3 | Demo shown; George chose A (plain bevel pills, old glow selection). Aqua stays out. |
| 4 | Fixed, 911th (see-through, text 25% larger). |
| 5 | Noted (working better). |
| 7 | Grip fixed, 905th. |
| 8 | Fixed, 903rd. |
| 9 | Fixed, 909th; the theme is now Black and White, 910th. |
| 10 | Fixed, 908th. |
| 11 | Fixed, 902nd. |
| 12 | Fixed, 907th. |
| 13 (added) | Open: the menu bar bottom edge is not drawn by Qt or the style sheets (checked in desktop Qt); source on the phone not found yet. |
| 14 (added) | Fixed, 906th. |

## 1.0.74-beta (tested Nov 22, 2026; screenshots docs/krimble-refs/2026-11-22_12*, _13*, _030216*)

1. Opening a new file does not keep the selected tool. Pan is the default only at program launch; on file open the current tool stays.
2. The selected-tool highlight is too small and barely covers the icon; it should fill the whole button square.
3. The Aqua buttons look very good. Wanted: experimental previews that look more transparent, one with a slightly darker darkest blue edge, and slightly rounder corners.
4. Odd edges on dialogs (Text, Canvas Size, JPEG options); see the screenshots.
5. The Screen Mode and Quick Mask icons should look more like the industry-standard ones.
6. Make Quick Adjust slightly smaller.
7. Versions of Times New Roman, Helvetica/Arial and Courier are required fonts.
8. Docking Tool Options stacked it instead of joining the tabs, and it could not be found afterwards.
9. The Tool Options panel title must say "Tool Options", not only the tool's name.
10. Move Panels to a top-level menu before Window; it is used too often to sit in a submenu.
11. Images loaded blurry and upside down (or mirrored) again.
12. Badly chosen buttons in various places.

Status (Nov 22, 15:45): 2, 3, 4, 9, 12 fixed in the 834th; 10 in the 835th to 837th; 1, 6, 7, 8 and 11 (a mitigation) in the 840th (not built). Waiting: 5 (drafts shown).

## 1.0.73-beta (tested Nov 21, 2026)

1. The first image loaded showed upside down and very blurry; it returned to normal after some handling.
2. The new Aqua effect is too light and should not be used on layers; on large areas it looks poor and white text disappears on it.
3. The new toolbox buttons look good, but the eyedropper icon is missing.
4. The new grab areas are good. Add a lip at the top of the toolbox and use fewer grip lines.
5. The extended Color Lab color wells look very good.
6. The Tool Options title overlaps the grab area.
7. Make Soften the default tool of its group instead of Smudge.
8. The opacity slider looks good.
9. The File menu title could sit a few pixels closer to the K.
10. Save As looks good, but its OK button should have the Aqua style.
11. Use the Aqua style on buttons only; keep selections the flat blue.
12. Preferences looks good.
13. File > New looks good, but its Create button needs the Aqua style.
14. Quick Adjust looks good; use the larger logo instead of the small Krimble one.
15. Rename Image > Image Rotation to Rotate.
16. Make the default toolbox icon size 22 x 22.
17. Make Undo black in the Black icon style; remove the Classic icon style.
18. Krimble still restarted after switching apps.

Screenshots: docs/krimble-refs/2026-11-21_1537_test73-*.jpg

**More from testing 1.0.73 (Nov 22, 2026; screenshots docs/krimble-refs/2026-11-22_*):**
- Long-pressing Transform shows the flyout indicator's promise of options, but opens only one row with the tool tip text, as a large Aqua bar.
- No tool name should contain the word "Free" (Free Transform, Free Rotate).
- The Transform button's flyout should hold only Transform modes, not other tools or tool tips.
- The Transform radio buttons (Position, Rotate, Scale, Shear) are hard to understand and may not work.
- In Tool Options, "Tool Options" and the tool name are stacked on top of each other; the title also said Free Rotate while Mesh was selected.
- The Aqua highlight's dark sides are too wide and not curved enough on large buttons; the Rotate icon's arrowheads and box are too small.
- The percentage on progress bars sits in the top left corner.
- The empty area behind the panel tabs is not darker than the panel.
- There is no light theme.

All of these are addressed in 821 to 828 (1.0.74).

## 1.0.72-beta (tested Nov 20, 2026; screenshots docs/krimble-refs/2026-11-20_*)

- The grips should match the mockup and be bigger, and the black part of the Color Lab banner should have grip lines (centered, letters untouched).
- The Layers panel title should be placed as in the mockup.
- Menus squeeze together when Fill opens its submenu, and the submenu itself is very small.
- A magenta box shows around the Move tool's frame.
- The Recent Images area is too small, with tiny scroll bars.
- The dull blue-green highlight in the theme was still there.
- Save As is still much too wide; it should be taller, not wider.

## 1.0.71-beta (tested Nov 20, 2026)

- The orange K sits on top of the File menu title; the gap should be a fixed distance with or without an image open.
- Color Lab is easy to grab but will not dock again.
- The toolbox selection should be a brighter, flat blue like the menus, without the glow.
- Swatches should go up to 256 and resize. The New document window should start at a size based on the screen. Images still open at a tiny size.
- Small report boxes on the canvas should go; their information belongs on the status bar.
- A gray stripe shows outside the shadow. Rule: nothing is drawn outside a panel's shadow.
- The toolbox bevel should be thicker, with highlight and shadow the same thickness; the shadow looks like a plain black line.
- The K on image windows is too far from the corner; pin it to the corner.
- Krimble still restarts when switching apps. Data loss is not the main concern; the lost time and the interrupted work are. The app runs better after the efficiency changes.
- In Layers, the check mark is too close to the eye.

## 1.0.70-beta (tested Nov 20, 2026)

1. Selecting toolbox tools is much harder.
2. The grab areas look clumsy, with big lines.
3. The Layers top row is still disorganized.
4. Layers has two grab areas and Color Lab has none.
5. Other selections should be plain blue like the menus, with no gradients.
6. Menus use several different hues; they should all use the same hue.

## 1.0.69-beta (tested Nov 19, 2026)

- The layer type slider is back at the top of Layers; it was moved away on purpose and should stay away.
- Grab pads should be bigger and fill the empty spots; the tabs slide sideways and spring back instead of tearing a panel off.

## 1.0.68-beta (tested Nov 18 to 19, 2026)

- Color Lab's logo slides up out of sight and needs the scroll bar to come back; it should always stay pinned.
- The top of Layers needs a minimum height.
- Desaturate should not open a window. Menu text is spaced too tightly.
- The Painted theme changes spacing; Smudge should lead its flyout group (later changed to Soften in 1.0.74).
- A box with the pixel size covers the area being selected.
- Resizing Color Lab hides its close button; the buttons should follow the window edge.
- Tapping + in Layers opens a menu instead of adding a layer; the extra Layers buttons are disorganized and should go.
- The toolbox's long-press menu is hard to reach; add some space at the top and bottom, and a line at the bottom of the docked toolbox.
- The gray theme has orange selections; all themes should use the plain blue.
- The good default brush from earlier builds should be restored.

## 1.0.67-beta (tested Nov 17, 2026)

- Menus are still cut short, still have a line between them and their titles, and the orange K is still too close to File. A blank line at the bottom of menus would help.
- Panels cannot be torn off; there is nothing to grab, and the side panel's edge cannot be grabbed.
- An extra line on the right of the toolbox and the left of the side panel serves no purpose; remove it.
- Grab areas must be big enough for a finger and invisible.
- In the Painted theme, the selected tab should use the colors from the supplied Color Lab image.

## 1.0.75-beta (tested Nov 23, 2026)

His words, as reported on the installed 1.0.75 (run 45158674034, 972169d1):
- "Bad build." (App not installed; fixed by freeing storage on the phone)
- "The aqua turns into a ugly gray turd on button down now. ... The aqua looks faded and unsaturated."
- "The menus look horrible now. All are outlined." / "The menus suddenly have big margins all around."
- "Quick Adjust is still tiny as F due to the previous misunderstanding."
- "The giant white line at the top and top left of the toolbar has to go."
- "Need vertical dividers for menus that expand wide."
- "Gadgets are creeping into menu title areas and too far from the corner." (portrait)
- "Color Lab was perfect before and now it's ruined."
- "And the ugly edges I asked you to remove are back." (shaded blue edges, broken corners on a drop-down)
- "Where is the grab area to drag out the size of the toolbox?" / "I can't grab it at all."
- "the new icons at the bottom of the toolbox are too big. They get too close to the edges."
- "There is still a dividing line between the menu title and the menu."
Fixed for 1.0.76: menus (865, 874), toolbox grip edge (866), Color Lab (867), Quick Mask and Screen Mode icons (870), blue edges and corners (871, 872), column dividers (868). Still open: gray on press, Quick Adjust window width, window buttons in portrait, toolbox grab strip.

## 1.0.66-beta (tested Nov 17, 2026)

- The bottoms of menus are cut off, and a line still separates menus from their titles.
- The Layers top row jumbles its icons at small sizes, a small gadget overlaps the + button, and the opacity bar has no margin.
- Menus have no outline or bevel, so they merge with each other.
- The "No options" text in Tool Options (Pan tool) is too close to the left edge.
- Menu selections should go back to a simple blue highlight; the glow is too strong.

## 1.0.65-beta (tested Nov 16 to 17, 2026)

- The File menu title is still too close to the orange K.
- The toolbox shadow is just a black line.
- A little more space is wanted under the menu titles before the shadow.
- Foreground and background start dark gray instead of white and black.
- An odd number of tools makes the two toolbox columns lopsided; there should always be an even number. Sharpen should be in the Soften/Blur flyout.
- The healing brush icon looks like a stamp.
- The toolbox will not stay docked at the bottom; it snaps to the side. Docking should be switchable on and off.
- The extra shadow lines on the menu bar disappeared, and a menu appeared detached from its title (the earlier decision was that a menu joins its title with no overlap and no gap).
- Canvas Input settings: the selection overlaps the keyboard title area.
- File > New: the selection covers the icon.
- Recent images should be a bigger scrolling area with smaller previews.
- Layers: black bars at the bottom; remove the search field; the selected layer should be the selection blue and the others dark, matching the theme.
- The canvas background should be darker.
- The edge of the right panel is hard to move again; the same issues keep returning.
- Save As is much too wide (more height was wanted, not width), and it does not remember a size set by the user. Windows should reopen at the size and position the user left them.
- Layers snapped into a column for no reason.
- OK does not close Quick Adjust, probably blocked by the sizing gadget.
- The small arrow buttons beside the image tabs are too thin; double their width.
- A wish-list Color Lab mockup was supplied.
- The crashes seem fixed, but some features are noticeably slower; many files open zoomed far out.

## 1.0.64-beta (tested Nov 16, 2026)

1. Jolts while painting (serious).
2. The image window buttons sit too high.
3. With no image open, the K is too close to File.
4. The Polygon and Star tool has no icon.
5. Themes: raised buttons are missing in the light theme; the tab color should be #364064; the selection gradient also shows in menus.
6. The menu bar should be taller.
7. Remove the Quick Adjust toolbar.
8. A new canvas should start with Pan (it started with the arrow).
9. The toolbox order should follow the supplied list.
10. A usable default brush is needed.
11. The slider ball bearing should be darker, with a cast shadow.
12. The status bar's size display is cut off in portrait.
13. File Info > General needs the supplied icon.
14. Flyouts should open after 250 ms.
15. The tabs for open images are gone; restore them.
16. Crashes, even on a simple fill. If stability does not improve, roll back to 1.0.58; prepare a list of every change made since then and how to redo it on that base.

Also (looking at the toolbox): Smudge and Soften should be on their own, and the healing brush on its own; the stamp is unclear; there are two Type tools (label them Krimble Type and Old Type); make the stamp a flyout of the healing brush, then separate it; remove the gadget under the color wells; flyout delay 400 ms (later 250 ms).

## 1.0.63-beta (tested Nov 16, 2026; 18 screenshots)

1. The toolbox does not dock normally; docking by menu or by dragging gives unpredictable results.
2. The right dock has no highlight line on its left side.
3. The Window menu does not respond.
4. The toolbox is cut off halfway down and has to be scrolled.
5. The K starts out too close to File and corrects itself later.
6. Color Lab grows to a giant size.
7. The Preferences selection still tints the icons.
8. Dock Right puts the panel at the top.
9. The stamp icon leads to tools with no names.
10. The raised toolbox style skips three icons.

## 1.0.62-beta (tested Nov 15, 2026)

- Color Lab shows three titles.
- The app is unstable and jumpy; the tabs work better but the titles look wrong; panels will not dock.
- The File menu is still too close to the K; after a theme change, the theme would not change again. Add a separator before Change Language in the Window menu.
- A message says there are no resource bundles (the whole folder should not have been deleted).
- After loading an image the layout is badly broken.
- Quick Adjust: the size is better, but the ball bearings are too big, the numbers have no room, and the Preview switch is too big; move it to the space at the bottom.
- Preferences: the highlight still tints the selected icon.
- Breeze Dark has added blue lines; Breeze Light has a dark menu bar.
- Black bars remain at the bottom of the Layers panel.
- Krimble Blue-Gray would not load (an old Krita theme name was saved); switching themes was slow.
- Tabs should match their panels with no line between them; remove the outline around menus.
- Tapping the Layers tab made the whole right dock vanish until the phone was rotated. Layers should use warm grays.
- Menus open a little too high.

Builds 55 to 61: their release notes carry the faults found in testing; the original reports were not kept word for word.

## Status of the 1.0.73 list (Nov 21, 2026)

| # | Status |
|---|---|
| 1 | Open. Nothing in 72 → 73 changed how the canvas is drawn; needs more detail (how the image was opened). |
| 2, 11 | Fixed, 806: Aqua darker and only on buttons; selections flat #0b87dd. |
| 3 | Fixed, 804: eyedropper drawn at any size. |
| 4 | Fixed, 807: lip at the top, six lines. |
| 6 | Fixed, 807 and 825: the tool name in the title row on one line, grip after it. |
| 7 | Fixed, 805: Soften first. |
| 9 | Fixed, 805: 70 real px after the K. |
| 10, 13 | Fixed, 806 and 821: OK and Create in Aqua. |
| 14 | Fixed, 805: large Quick Adjust logo. |
| 15 | Fixed, 811 (with permission): krita5.xmlgui, Image Rotation → Rotate. |
| 16 | Fixed, 805: toolbox 22 by default. |
| 17 | Fixed, 804: black Undo; Classic removed. |
| 18 | Fixed, 808 (to confirm in testing): Back now sends Krimble to the background instead of closing it. |
- **Nov 24, 5:05 PM MST:** 1.0.58 retested: no blurry or upside-down images so far (George: ".58 has not blurred or flipped any."). 1.0.79: new image opened tiny.
- **Nov 24, 7:32 PM MST:** 1.0.59 retested: opens images full size; "weird transparency effects" (George).
- **Nov 24, 7:37 PM MST:** 1.0.60 retested: loads fine (George: ".60 loads fine...").
- **Nov 24, 7:45 PM MST:** 1.0.61 retested: loads fine, but opens small then zooms in fast; has the orange K and the File menu positioned well (George).
- **Nov 24, 7:57 PM MST:** 1.0.62 retested: shows the squeeze, no upside-down or blur; "lots of other issues" (George: "I should have rolled back to .58 at this point.").
- **Nov 24, 8:09 PM MST:** 1.0.63 retested: still the quick zoom; no blur or flip.
- **Nov 24, 8:42 PM MST:** retest run (updates over the top, except an uninstall after 70). 63: jitter, weird panel handling. 64: zooms and moves the image right. 65: opens fine. 66: opens perfectly. 67: loads well. 68: squeeze, loads fine, no flip or blur. 69: pause, then opens fine. 70: loads fine. (uninstalled 70) 71: squeeze, loads fine. **72: upside down and blur.**
- **Nov 24, 8:45 PM MST:** upside-down/blur first appears in 72 (a71b1a03); 71 (ba8603a) is clean. Main suspect in 71→72: d39709a3, the zoom-to-fit 1 s after opening (removed after 79 in 9182d358). Other canvas-side changes in 72: canvas messages moved to the status bar (aaaf95d8), dock separator 0 (edda5f6e), keep-alive service (f0c269f5).
