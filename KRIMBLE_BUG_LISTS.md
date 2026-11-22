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

**More from testing 1.0.73 (Nov 22, 2026, 00:54 to 02:46 MST; screenshots docs/krimble-refs/2026-11-22_*):**
- (screenshot) "Transform still doesn't work the way it should. It has the little indicator to show that it has a flyout, so I long press and get this." (one row with the tool tip text, as a big Aqua bar) / "Why would the UI promise other options and instead just tell you what the tool is?"
- "there's no reason to add the word "free" to any tool. It means nothing." / "stop calling it free rotate. It communicates nothing"
- "The transform tool in the toolbox should only lead to versions of the transform tool, i. e. Not a non-transformative tool, not an explanation or tooltip, not a banana or anything else."
- "But the radio buttons are counter-intuitive and I don't think they actually work."
- (screenshot, circled) "The mess is the circled text." "Tool Options" and "Free Rotate" stacked on top of each other: "Nobody writes lines on top of each other like that. It's a mess." The title also said Free Rotate while Mesh was selected.
- "The dark side areas on the aqua are too prominent and not curved enough on larger areas. The arrowheads on the rotate tool are not prominent enough and the box is too small, the whole icon too small"
- "Why is the 60% in the corner?" (progress bar text)
- The empty area behind the panel tabs was not darker than the panel.
- No light theme ("Don't we have any light themes any more????").
All of these are addressed in 821 to 828 (1.0.74).

## 1.0.72-beta (Nov 20, 2026, 19:43 to 23:05 MST; screenshots in docs/krimble-refs/2026-11-20_*)

- 19:43: "Just stop the build. There are more issues." Grips like his mockup; grips in the black part of the Color Lab logo; menus squeeze on Fill; magenta move box; "I have the Layers menu title repositioned. See graphic."
- "You need to make the grips bigger to match my illustration and move the Layers panel title like I showed you." ".72 does not feature these changes."
- 23:00: Ball bearing image: grips not done as instructed. Recent Images area too small, tiny scroll bars. Circled aqua: "I previously asked for that specific dull aqua color in the theme to be changed and you didn't do it." Grips and Layers window title: "you're ignoring my instructions." Save As: "still massively wide... All I wanted was for it to be taller... I don't want it to be wide." Fill submenu: "turns menus miniature and it looks so tiny itself as a submenu. It's ridiculous."
- 23:03: "I want the same grip lines on the black part of the Color Lab logo so I can grab it. Make it look nice and centered on the banner, and don't touch the letters."

## 1.0.71-beta (Nov 20, 2026, during the day, MST; the chat was compressed, so some times were not kept)

- "For the hundredth time: THE ORANGE K IS RIGHT ON TOP OF THE FILE MENU TITLE. FIX IT." "Lock it into a specific pixel distance for both before and after file open."
- 10:01: Color Lab is easy to grab but won't reattach. Brighter blue toolbox selection, flat like the menu: "The glowy thing doesnt look professional." Swatches up to 256 with resizing. New document window size on launch based on the screen. "I have another image loaded at tiny size again."
- "I don't want these little report boxes any more. Report these stats on the status bar."
- "Why is there a gray stripe outside the shadow????" RULE: "We do not draw any more content outside the shadow area. EVER. THE SHADOW IS THE OUTER EDGE."
- Toolbox bevel: "They need to be thicker. The hilight area and shadow area should be the same number of pixels thick." "As it is the shadow just looks like a black line and that is no good."
- K on image windows: "Yeah. Too far from the corner." "Yes. Pin them to the corner."
- Resets: "You have to find a solution to the constant resets. DATA LOSS IS NOT THE PROBLEM!!!!!!! THE PROBLEM IS WASTED TIME RESTARTING AND FRUSTRATION WITH HAVING NO CONTROL OVER THE APP!!!" "The app is generally running better after the efficiency edits. But the app still restarts."
- Layers: "The check mark is too close to the eye."

## 1.0.70-beta (list sent Nov 20, 2026, 10:57Z; the 757th entry names it "branch build 70")

**10:57Z (Nov 20):** "1. Much harder to just select toolbox tools. 2. grab areas look bad. Clunky big weird lines. 3. Layers menu still a mess. 4. layers has two grab areas, color lab has none. 5. Make the other selections plain blue like menus. No gradients. 6. Menus are still a jumble of different hues. Make them match. Same hue." (757th)

## 1.0.69-beta (Nov 19, 2026, 18:39Z to 18:57Z)

- "Now the goddamned layer type slider is back. I don't want that there!!!!!! / We moved that for a reason, and it wasn't to make room for the stupid extra tools." (753rd)
- Grab pads: "Make it bigger", "The same spaces that were odd empty spots should be grab pads.", "Instead the tabs just slide to the side and spring back into place." (754th)

## 1.0.68-beta (Nov 18 00:53Z to Nov 19 04:24Z)

- "Plus color lab slides the logo up into nowhere and then you have to use a scrollbar to get it back. That should never happen." / "THE COLOR LAB LOGO SHOULD ALWAYS HAVE BEEN PINNED." (742nd)
- "The top of layers should always have had a minimum height." (743rd)
- Desaturate dialog: "Can that dialog." Menu text vertical spacing too tight. (744th)
- "And fix the painted theme. I had no idea themes changed spacing. Also, the smudge tool should be the default for that flyout set, not soften." (745th)
- "I try to select something and a box covers what I'm trying to do just to tell me the pixel dimensions. I thought we had moved that thing." (746th)
- "I tried resizing the color lab panel and it hid the close gadget on the upper right corner." / "The buttons should move with the window edge." (747th)
- "The layers panel looks weird. I tried to hit the plus to add a layer and instead got a menu. Why???" (748th) / "All those extra buttons are just a mess. The empty space looks bad and the sloppy symbol positions look bad. Just get rid of them." (749th)
- "It's hard now to get to the long press menu for the toolbox... Unresponsive. I need somewhere to press. Maybe a little space at the top and bottom?" / "And a line at the bottom of the toolbox even when it's docked." (750th)
- "I want to change the gray theme that has the orange selections. They should all have the plain blue selections." (751st)
- "Did we ever get a decent default brush set up like before? ... Just restore or recreate that brush." (752nd)

## 1.0.67-beta (Nov 17, 2026, 20:11Z to 20:24Z)

- "Same short menus. Same line dividing menus from titles. Same orange K too close to the File menu. No changes." / "Can't you just add a blank line at the bottom of the menus?" (738th)
- "Can't grab panels to tear them off. No handles." / "There is nothing to grab." / "Can't grab the edge of the side panel either."
- "There is an ugly extra line on the right side of the toolbox and the left side of the side panel. It does nothing but ruin my day. Remove it" (739th)
- "If it's a grab area, it's not big enough and should also be invisible as previously discussed. Make a note of this." (740th)
- "In painted theme make the selected tab the color values in this color lab image." (741st)

## 1.0.66-beta (Nov 17, 2026, 17:36Z and 18:28Z)

- "Bottoms of menus are chopped off. Tops of menus still have a line separating them from menu titles. Layers menu is a mess at small sizes. The icons jumble. There is a miniature gadget overlapping on the + sign at bottom of layers. Layers opacity bar goes all the way to the edge with no margin. Bad. Menus have no line around them or beveled edges so they just merge with other menus. Text in the tool options for Pan tool are too close to the edge. Go back to simple blue hilight for menu selections. The glow is too much." (735th)
- "It's not the name of the tool. It's where it says no options." / "5. Why not just ask, chatbot? It's the left margin." (736th, 737th)

## 1.0.65-beta (list sent Nov 17, 2026, 01:03Z)

**2026-11-17T01:03Z:** File menu title is still too close to the orange K. Shadow on the toolbox is just a black line. Would like a couple more lines under the menu titles before getting down to the shadow. Foreground and background start as dark gray still instead of white and black. Odd number of tools in toolbox makes 2 columns look lopsided. Sharpen should be a flyout of soften/blur. We should never have an odd number of tools. I thought the icon was the stamp tool, but it's the healing brush. I don't get it. It's a form of stamp tool, right? I tried to leave the toolbox at the bottom, out of the way, but it snapped to the side. Not sure how to set that. Can docking be turned on and off? That seems like a good idea. Extra shadow lines on menubar disappeared. Menu appeared detached from title. No good. That was a decision before. The menu connects to the title without overlap and not separated. Canvas input settings selection overlaps onto the keyboard title area. New dialog selection maps over the icon. Recent images should be a bigger scrolling area with smaller preview images to choose from. Layers panel is a mess. It still has the black things at the bottom. The search field is a mess. Get rid of the search. It's useless. Change the layer colors back. Give me the selection effect blue for the selected layer and make the other layers dark and match the theme. Make the canvas background darker. It's hard to move the edge of the right hand pane again. So sick of the same issues coming back. Save As... dialog is WAY TOO WIDE. I ASKED FOR MORE HEIGHT, NOT MORE WIDTH. Resizing Save As... window does not set the size for next time it is invoked. When the dimensions of a window are resized by user, they should be that size and position when invoked again. Layers did super weird snap into a column for no reason. Madness. OK does not close QuickAdjust. It's probably blocked by the sizing gadget. The little pointer gadgets beside the file tabs are too thin. Double the width. Wishlist version of ColorLab as mockup image attached.

**2026-11-17T11:29Z:** "I believe the crashes may be dealt with, but there is a noticable slowdown in some features and I wasn't expecting that after trying to remove bloat from the app." **11:41Z:** "I also notice a lot of files are opening zoomed way out so they look tiny on load."

## 1.0.64-beta (list sent Nov 16, 2026, 16:21Z and 21:39Z)

Items 1 to 14 as recorded in the 722nd entry (the exact wording of the list was not kept): 1 jolts while painting ("Very bad bug"); 2 image window buttons too high; 3 K too close to File with no image; 4 Polygon and Star icon missing; 5 themes: raised buttons in the light theme, tab color (his saved #364064), selection gradient also in menus; 6 taller menu bar; 7 remove the Quick Adjust toolbar; 8 and 9 a new canvas should start with the Hand (it started with the arrow), and the toolbox order; 10 a default brush; 11 the ball bearing darker with a cast shadow; 12 the status bar size cut off in portrait; 13 File Info General icon; 14 flyouts at 250 ms.

**2026-11-16T13:28Z to 13:40Z** (looking at the toolbox): "Smudge and soften should be on their own. The bandaid should be on its own. I don't even know what the stamp is. Why are there two Type tools? A lot of these changes are never approved." / "Label one type tool Krimble Type and the other Old Type." / "Make the stamp a flyout from the bandaid." / "What is that gadget at the bottom under the color wells in the toolbox?" / "Remove it. I don't think anyone will use it." / "Separate the stamp tool." / "Make it 400 Ms."

**2026-11-16T16:21Z:** New bugs 1.0.64-beta: (14 items: jolts while painting; window buttons too high; K too close to File with no image; Polygon and Star icon; themes: raised buttons in the white theme, tab color (saved #364064), selection gradient also in menus; taller menu bar; remove the QuickAdjust toolbar; Hand as the start tool; the toolbox order; a default brush; ball bearing darker with a cast shadow; status bar size cut off in portrait; File Info General icon; 250 ms flyouts.) "Almost everything is perfect. Very few issues left to fix."

**2026-11-16T21:39Z:** (the list again, plus) 15. Krita previously had tabs at the top for the open image files so you could select them. Now it doesn't for some reason. I didn't ask for that to be removed. Please restore it. 16. There are stability issues. Even just doing a simple operation like a fill can crash the app. I have been forced to use 1.0.58 due to severe stability issues with the new build. if these issues persist I will be forced to roll back to 1.0.58 and start over on revisions. Please make a list of all revisions requested/attempted in subsequent versions and formulate the best implementation if a rollback is performed.

## 1.0.63-beta (Nov 16, 2026, 02:10Z; 18 screenshots)

Ten faults (as recorded): 1 toolbox docking; 2 "The right dock has no hilight line on the left side."; 3 Window menu; 4 "Toolbox still cuts off halfway down and forces scrolling."; 5 "K still starts out too close then corrects later."; 6 "Color Lab still MAGNIFIES TO GIANT SIZE."; 7 "Preferences selection still tints icons."; 8 Dock right gives dock top; 9 "Unwanted stamp icon still leads to nameless tools."; 10 "Raised toolbox skips three icons." "Version numbers mean nothing because you never fix what I ask you to fix." 03:54Z: "I tried different ways. I used the menu and dragged the toolbox and got unpredictable results."

## 1.0.62-beta (Nov 15, 2026, 23:15Z to 24:10Z)

- (screenshot) "ColorLab has THREE TITLES."
- "It's super buggy and twitchy now." / "The tabs work better, but titles are weird now." / "Nothing wants to dock right now." / "When it started the file menu was still too close to the K. Somehow I changed themes. Now it refuses to change themes." / "Also put a separator before change language in window menu."
- "I got a message about not having any resource bundles." / "I didn't want you to delete the whole folder before."
- (welcome page) "The k too close." / (after loading an image) "Disaster. Complete mess."
- "QuickAdjust size is better, but the ball bearings are too big." / "And I can't see the numbers any more. They have no space." / "And the preview toggle button is too big to fit in between the buttons now. Shrink it" / "But put it in the space on the bottom."
- (Preferences) "The hilight still tints the icon when selected. Fix it. Again."
- "Breeze dark theme has blue lines added?" / "Breeze light has a dark menu bar."
- "There are still weird black things at the bottom of the layers panel."
- Krimble Blue-Gray would not load: "It was called Krita something before, but we changed it because this is not Krita." / "Ok it finally switched. There was a long delay"
- "Please make tabs match their panels and don't put a line between them." / "Remove the outline around menus."
- "I tapped the layer tab and the whole right dock vanished. Then I rotated and it came back. Twitchy." / "Can the layers use warm greys instead of cold grays?"
- "It looks like menus are a little too high."

Builds 55 to 61: their release notes carry the faults found in testing (written Nov 15-16 from his reports, "the notes need to be constructed about the many failures in those builds"); the exact wording of those reports was not kept.

## Status of the 1.0.73 list (Nov 21, 2026)

| # | Status |
|---|---|
| 1 | Open. Nothing in 72 → 73 changed how the canvas is drawn; needs more detail (how the image was opened). |
| 2, 11 | Fixed, 806: Aqua darker and only on buttons; selections flat #0b87dd. |
| 3 | Fixed, 804: eyedropper drawn at any size. |
| 4 | Fixed, 807: lip at the top, six lines. |
| 6 | Fixed, 807: tool name in the title row, grip after it. |
| 7 | Fixed, 805: Soften first. |
| 9 | Fixed, 805: 70 real px after the K. |
| 10, 13 | Fixed, 806: OK and Create in Aqua. |
| 14 | Fixed, 805: large QuickAdjust logo. |
| 15 | Fixed, 811 (with his permission): krita5.xmlgui, Image Rotation → Rotate. |
| 16 | Fixed, 805: toolbox 22 by default. |
| 17 | Fixed, 804: black Undo; Classic removed. |
| 18 | Fixed, 808 (to confirm in testing): the log showed Krimble was finished (isFinishing=true), most likely by Back; Back now sends Krimble to the background. |
