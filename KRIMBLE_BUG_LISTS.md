# Krimble bug lists

Every bug list George sends after testing a build, in his words, filed under the build it was found in.
Release notes take their Known issues from here ("Remember to keep all my bug lists in a file. They need to be added to their respective releases." — Nov 21, 2026 15:45).
Newest first. Status of each fix is in KRIMBLE_CHANGES.md.

## 1.0.73-beta (list sent Nov 21, 2026, 15:37 MST)

1. For some reason the first image I loaded was upside down and super blurry! After messing with it somehow it went back to normal. Something you tried to change backfired badly.
2. I like the new aqua effect but it's too light and it shouldn't be used on the layers. Big areas look bad. White disappears over it.
3. New toolbox buttons look nice, but the eyedropper disappeared.
4. New grab areas are good. Add a lip area at the top of the toolbox and lose a few lines of the grab.
5. Color Lab extended color wells look fantastic.
6. Tool options title overlaps onto grab area.
7. Make soften tool the default instead of smudge.
8. Opacity slider still looks awesome.
9. File menu title could be a few pixels closer to the K.
10. Save As... dialog looks nice, but OK button looks bad. No aqua style?
11. Instead of selections, just use new aqua style on buttons, not selections. Keep selections that flat pretty blue.
12. Preferences looks great.
13. File - New looks great except for the create button. Again, buttons need the new style.
14. QuickAdjust looks great, but use my larger logo instead of the tiny Krimble one.
15. Change Image - Image Rotation to just Rotate.
16. Default the toolbox size to 22 x 22.
17. Make UNDO black for black toolbox style. Lose the "classic" version. It's not great.
18. It relaunched on switch again. No difference.

Screenshots: docs/krimble-refs/2026-11-21_1537_test73-*.jpg

## 1.0.69-beta or 1.0.70-beta (list sent Nov 20, 2026, 10:57Z; which build to confirm)

**10:57Z (Nov 20):** 1. Much harder to just select toolbox tools. 2. grab areas look bad... 6. Menus are still a jumble of different hues. Make them match. Same hue. (757th)

## 1.0.65-beta (list sent Nov 17, 2026, 01:03Z)

**2026-11-17T01:03Z:** File menu title is still too close to the orange K. Shadow on the toolbox is just a black line. Would like a couple more lines under the menu titles before getting down to the shadow. Foreground and background start as dark gray still instead of white and black. Odd number of tools in toolbox makes 2 columns look lopsided. Sharpen should be a flyout of soften/blur. We should never have an odd number of tools. I thought the icon was the stamp tool, but it's the healing brush. I don't get it. It's a form of stamp tool, right? I tried to leave the toolbox at the bottom, out of the way, but it snapped to the side. Not sure how to set that. Can docking be turned on and off? That seems like a good idea. Extra shadow lines on menubar disappeared. Menu appeared detached from title. No good. That was a decision before. The menu connects to the title without overlap and not separated. Canvas input settings selection overlaps onto the keyboard title area. New dialog selection maps over the icon. Recent images should be a bigger scrolling area with smaller preview images to choose from. Layers panel is a mess. It still has the black things at the bottom. The search field is a mess. Get rid of the search. It's useless. Change the layer colors back. Give me the selection effect blue for the selected layer and make the other layers dark and match the theme. Make the canvas background darker. It's hard to move the edge of the right hand pane again. So sick of the same issues coming back. Save As... dialog is WAY TOO WIDE. I ASKED FOR MORE HEIGHT, NOT MORE WIDTH. Resizing Save As... window does not set the size for next time it is invoked. When the dimensions of a window are resized by user, they should be that size and position when invoked again. Layers did super weird snap into a column for no reason. Madness. OK does not close QuickAdjust. It's probably blocked by the sizing gadget. The little pointer gadgets beside the file tabs are too thin. Double the width. Wishlist version of ColorLab as mockup image attached.

## 1.0.64-beta (list sent Nov 16, 2026, 16:21Z and 21:39Z)

**2026-11-16T16:21Z:** New bugs 1.0.64-beta: (14 items: jolts while painting; window buttons too high; K too close to File with no image; Polygon and Star icon; themes: raised buttons in the white theme, tab color (saved #364064), selection gradient also in menus; taller menu bar; remove the QuickAdjust toolbar; Hand as the start tool; the toolbox order; a default brush; ball bearing darker with a cast shadow; status bar size cut off in portrait; File Info General icon; 250 ms flyouts.) "Almost everything is perfect. Very few issues left to fix."

**2026-11-16T21:39Z:** (the list again, plus) 15. Krita previously had tabs at the top for the open image files so you could select them. Now it doesn't for some reason. I didn't ask for that to be removed. Please restore it. 16. There are stability issues. Even just doing a simple operation like a fill can crash the app. I have been forced to use 1.0.58 due to severe stability issues with the new build. if these issues persist I will be forced to roll back to 1.0.58 and start over on revisions. Please make a list of all revisions requested/attempted in subsequent versions and formulate the best implementation if a rollback is performed.
