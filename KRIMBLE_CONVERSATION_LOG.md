# Krimble conversation log (George's messages)

Kept so a compressed session never loses what he said. Newest at the end. Times are UTC. Claude's replies are summarized in KRIMBLE_CHANGES.md and KRIMBLE_WORKLOG.md.

## Session of 2026-11-15 (messages in the part of the session that is still on disk; earlier parts were compressed)

- **2026-11-15T21:55:57Z:** Push the new theme.
- **2026-11-15T21:57Z:** No theme should have the word Krita in it from now on. Not in the filename or in the file.
- **2026-11-15T21:57Z:** Remove all the tab customizations. They are all a failure.
- **2026-11-15T21:59:08Z:** Delete krimbleblue.
- **2026-11-15T22:00:04Z:** Did you remove fonts and brushes and other bloat earlier or did you leave it?
- **2026-11-15T22:00:31Z:** Earlier did we not discuss removing bloat??????????????
- **2026-11-15T22:00:56Z:** Ugh!!!! I hate this!!!!
- **2026-11-15T22:01:22Z:** WHY DONT YOU EVER KEEP ANY CONVERSATIONS???????
- **2026-11-15T22:02:34Z:** I talked to you about how slow the app starts. We talked about removing unnecessary stuff. WHY DONT YOU KEEP BETTER NOTES?????? KEEP DETAILED NOTES AND TRANSCRIPTS ON THE REPO!!!!

## Standing rule (2026-11-15, 15:02 MST)
George: "KEEP DETAILED NOTES AND TRANSCRIPTS ON THE REPO!!!!" Every session appends his messages and the decisions to this file, and every push gets dated notes in `KRIMBLE_CHANGES.md` and `KRIMBLE_WORKLOG.md`.

## Open topic recorded 2026-11-15, 15:02 MST: slow app start and unnecessary stuff
George says he talked to Claude (in an earlier session; the text of that talk is not on disk and was not found) about how slowly the app starts and about removing unnecessary stuff. Nothing was removed. What the repo has in `krita/data`: `bundles` 36 MB, `fonts` 24 MB, `patterns` 7.6 MB, `paintoppresets` 2.1 MB, `gradients` 76 KB, `brushes` 12 KB. What counts as unnecessary is to be decided by George; nothing is cut until he says.

## Earlier in the 2026-11-15 session (from the compression summary; his words as recorded there)
- Slider handles are his ball-bearing picture, on every slider including Color Lab. Recent colors must not reorder on reselect. Raised toolbox highlight dimmer. Donate button Krimble orange (#FD6001). QuickAdjust tiny, one line per row. Painted theme scroll bar: darker track. Remove the Krita themes. New theme Krimble Dark Gray (flat, no bevels, orange selection, squared corners, warm grays). Menu bar title recessed; menu rows light up. Recent Images clear of the toolbox; darker base screen. Menus start further right of the orange K. Color Lab keeps its logo. Layers panel needs a title. Toolbox has 2 columns in portrait but 1 in landscape (cause unknown). Crash when picking Krimble Blue-Gray (cause unknown). Optional 4-sided 3D color triangle: "It's more important to get a functional build."
- Rules: build only when he says "BUILD"; a push is not a build; failed builds are deleted and the number reused; terse replies; no multiple-choice menus; one step at a time; offers, not orders; no unprompted theories; never claim done unless it is in the repo; comment out, never delete; dated notes for every change and push.
- **2026-11-15T22:03Z:** I said before we don't need more than about 5 brushes. Delete the others.
- **2026-11-15T22:04Z:** You pick. I don't care.
- **2026-11-15T22:05Z:** This app is sick with bloat. Remove the biggest fonts except Montserrat. You don't remember deleting 9 font families?
- **2026-11-15T22:06Z:** DELETE!!!!!!!!
- **2026-11-15T22:06Z:** Now delete bundles.
- **2026-11-15T22:08Z:** Templates for what? / No. Leave those. Fix this. Make the area where recent files live darker than the UI components so they stand out.
- **2026-11-15T22:11Z:** (15 screenshots) Make the bevel edges of the toolbox better.
- **2026-11-15T22:12Z:** Make sure menu titles recede when selected, not glow.
- **2026-11-15T22:13Z:** Do you still have the comments and tags for the last few builds? I deleted them. / I definitely don't want those APKs, although I have them. / No. I think the notes need to be constructed about the many failures in those builds.
- **2026-11-15T22:27Z:** Build the new fixes. 1.0.62
- **2026-11-15T23:05Z:** Restore previous docking behavior for toolbox and previous status bar height
- **2026-11-15T23:15Z:** (screenshot, build 62) ColorLab has THREE TITLES.
- **2026-11-15T23:20Z:** It's super buggy and twitchy now. / The tabs work better, but titles are weird now. / Nothing wants to dock right now. / I thought the whole point of this build was to implement all new changes. Why didn't you do that? / No. You're way off. / When it started the file menu was still too close to the K. Somehow I changed themes. Now it refuses to change themes. / Also put a separator before change language in window menu.
- **2026-11-15T23:29Z:** I got a message about not having any resource bundles.
- **2026-11-15T23:30Z:** I didn't want you to delete the whole folder before.
- **2026-11-15T23:32Z:** (welcome page screenshot) The k too close.
- **2026-11-15T23:33Z:** (screenshot) Disaster. Complete mess. / I loaded an image.
- **2026-11-15T23:38Z:** QuickAdjust size is better, but the ball bearings are too big. (Also: roll back to 1.0.58 is a temptation, not an instruction yet.)
- **2026-11-15T23:39Z:** And I can't see the numbers any more. They have no space.
- **2026-11-15T23:40Z:** And the preview toggle button is too big to fit in between the buttons now. Shrink it
- **2026-11-15T23:41Z:** But put it in the space on the bottom.
- **2026-11-15T23:45Z:** (screenshot) Preferences. The hilight still tints the icon when selected. Fix it. Again.
- **2026-11-15T23:44Z:** (screenshot) Breeze dark theme has blue lines added?
- **2026-11-15T23:46Z:** Yes (derive bevel colors from each theme)
- **2026-11-15T23:50Z:** (screenshot) Breeze light has a dark menu bar.
- **2026-11-15T23:52Z:** (screenshot) There are still weird black things at the bottom of the layers panel.
- **2026-11-15T23:55Z:** It was called Krita something before, but we changed it because this is not Krita. (re: Krimble blue gray won't load)
- **2026-11-15T23:58Z:** Ok it finally switched. There was a long delay
- **2026-11-15T23:59Z:** (3 screenshots) Please make tabs match their panels and don't put a line between them.
- **2026-11-15T24:00Z:** Remove the outline around menus.
- **2026-11-15T24:05Z:** I tapped the layer tab and the whole right dock vanished. Then I rotated and it came back. Twitchy. / Can the layers use warm greys instead of cold grays?
- **2026-11-15T24:10Z:** (screenshot) It looks like menus are a little too high.
- **2026-11-15T24:15Z:** Let's build 1.0.63. / All pushes in?
- **2026-11-16T00:16Z:** Analyze all the recent commits for errors.
- **2026-11-16T00:29Z:** There are no comments about the last several builds.
- **2026-11-16T00:30Z:** 1.0.59 and 1.0.60 are still missing. (GitHub releases: this session may not create releases, HTTP 403)
- **2026-11-16T02:10Z:** Ten faults after build 63 (toolbox docking, right dock highlight line, Window menu, toolbox cut off, K too close, Color Lab giant, Preferences tint, dock right gives dock top, stamp icon with nameless tools, raised toolbox skips three icons). "Version numbers mean nothing because you never fix what I ask you to fix."
- **2026-11-16T03:39Z:** "my fix has never run, in any build." WHY? (answer: exact class-name test never matched the namespaced class; 706th)
- **2026-11-16T03:40Z:** Fix these issues and push.
- **2026-11-16T03:54Z:** I tried different ways. I used the menu and dragged the toolbox and got unpredictable results.
- **2026-11-16T03:55Z:** If I give you a PAT can you try to update the release docs?
- **2026-11-16T03:56Z:** Work independently to resolve issues and document everything. (new PAT given)
- **2026-11-16T04:11Z:** Build
- **2026-11-16T12:37Z:** Tag and document.
- **2026-11-16T12:38Z:** Save all documentation from releases in your project documents. Construct accurate help file for app.
- **2026-11-16T13:28Z:** Did we finally get rid of the stamp icon and make the flyout better?
- **2026-11-16T13:29Z:** Show me the toolbox.
- **2026-11-16T13:34Z:** Smudge and soften should be on their own. The bandaid should be on its own. I don't even know what the stamp is. Why are there two Type tools? A lot of these changes are never approved.
- **2026-11-16T13:36Z:** Label one type tool Krimble Type and the other Old Type.
- **2026-11-16T13:36Z:** Make the stamp a flyout from the bandaid.
- **2026-11-16T13:37Z:** What is that gadget at the bottom under the color wells in the toolbox?
- **2026-11-16T13:38Z:** Remove it. I don't think anyone will use it.
- **2026-11-16T13:38Z:** Are there an even number of tools or odd now?
- **2026-11-16T13:39Z:** Separate the stamp tool.
- **2026-11-16T13:40Z:** The flyouts already cleaned up the UI a lot. How long is the flyout delay.
- **2026-11-16T13:40Z:** Make it 400 Ms.
- **2026-11-16T13:41Z:** Push all changes to master.
- **2026-11-16T13:42Z:** Can you write a simple Krimble manual? Basic introduction and documentation for beginners?
- **2026-11-16T13:44Z:** Manual should not focus on painting. Krimble is not a paint app.
- **2026-11-16T13:45Z:** Krimble is a graphics editor like the industry standard (although we should avoid mentioning the industry standard or Adobe).
- **2026-11-16T13:46Z:** Remove ANY AND ALL MENTIONS OF [the brand name]!!!!!!
- **2026-11-16T13:47Z:** Adobe RGB is fine.
- **2026-11-16T13:47Z:** Push and build.
- **2026-11-16T14:30Z:** Belay the build. I have a bug list from 1.0.64-beta.
- **2026-11-16T16:21Z:** New bugs 1.0.64-beta: (14 items: jolts while painting; window buttons too high; K too close to File with no image; Polygon and Star icon; themes: raised buttons in the white theme, tab color (saved #364064), selection gradient also in menus; taller menu bar; remove the QuickAdjust toolbar; Hand as the start tool; the toolbox order; a default brush; ball bearing darker with a cast shadow; status bar size cut off in portrait; File Info General icon; 250 ms flyouts.) "Almost everything is perfect. Very few issues left to fix."
- **2026-11-16T16:23Z:** There should already be a sorting assignment system in place for the toolbox. It was the first thing I asked for because I wanted pan/hand to be the default. It was why I started editing Krita for mobile in the first place.
- **2026-11-16T16:28Z:** The jolt may be some kind of reset or redraw linked to the theme. I don't know.
- **2026-11-16T21:39Z:** (the list again, plus) 15. Krita previously had tabs at the top for the open image files so you could select them. Now it doesn't for some reason. I didn't ask for that to be removed. Please restore it. 16. There are stability issues. Even just doing a simple operation like a fill can crash the app. I have been forced to use 1.0.58 due to severe stability issues with the new build. if these issues persist I will be forced to roll back to 1.0.58 and start over on revisions. Please make a list of all revisions requested/attempted in subsequent versions and formulate the best implementation if a rollback is performed.
- **2026-11-16T21:46Z:** Build.
- **2026-11-17T01:03Z:** File menu title is still too close to the orange K. Shadow on the toolbox is just a black line. Would like a couple more lines under the menu titles before getting down to the shadow. Foreground and background start as dark gray still instead of white and black. Odd number of tools in toolbox makes 2 columns look lopsided. Sharpen should be a flyout of soften/blur. We should never have an odd number of tools. I thought the icon was the stamp tool, but it's the healing brush. I don't get it. It's a form of stamp tool, right? I tried to leave the toolbox at the bottom, out of the way, but it snapped to the side. Not sure how to set that. Can docking be turned on and off? That seems like a good idea. Extra shadow lines on menubar disappeared. Menu appeared detached from title. No good. That was a decision before. The menu connects to the title without overlap and not separated. Canvas input settings selection overlaps onto the keyboard title area. New dialog selection maps over the icon. Recent images should be a bigger scrolling area with smaller preview images to choose from. Layers panel is a mess. It still has the black things at the bottom. The search field is a mess. Get rid of the search. It's useless. Change the layer colors back. Give me the selection effect blue for the selected layer and make the other layers dark and match the theme. Make the canvas background darker. It's hard to move the edge of the right hand pane again. So sick of the same issues coming back. Save As... dialog is WAY TOO WIDE. I ASKED FOR MORE HEIGHT, NOT MORE WIDTH. Resizing Save As... window does not set the size for next time it is invoked. When the dimensions of a window are resized by user, they should be that size and position when invoked again. Layers did super weird snap into a column for no reason. Madness. OK does not close QuickAdjust. It's probably blocked by the sizing gadget. The little pointer gadgets beside the file tabs are too thin. Double the width. Wishlist version of ColorLab as mockup image attached.
- **2026-11-17T03:03Z:** Continue
- **2026-11-17T03:13Z:** How are we doing?
- **2026-11-17T10:11Z:** Can the selected item in menu have a glow line around it?
- **2026-11-17T10:12Z:** Yes, let's try it. After that return to the list.
- **2026-11-17T10:22Z:** Continue.
- **2026-11-17T11:36Z:** You could just make the corner grip on QuickAdjust smaller and a little outside of the corner and invisible and scoot the button a little to the left. Then both functions would work.

## 2026-11-17 session (Opus 5.5), George's words (times UTC)
- **11:29Z:** I believe the crashes may be dealt with, but there is a noticable slowdown in some features and I wasn't expecting that after trying to remove bloat from the app. Theorize.
- **11:30Z:** Look at the repo and compare to your theories.
- **11:39Z:** I think it's that canvas effect. / **11:40Z:** Yes. That's the correct solution. (732nd: canvas no longer under the panels)
- **11:41Z:** I also notice a lot of files are opening zoomed way out so they look tiny on load. Can we fix that? What's the default zoom level on new files when they open? (733rd)
- **11:44Z:** But why? The whole problem I had before was that the canvas would shrink and the image would get tiny when the side panels opened.
- **11:45Z:** Ok. Let's build it and see.
- **11:49Z–12:56Z:** Release notes and tags: "If I give you a PAT can you try to update the release documentation and tag?", "I can handle the apk.", "It used to work fine. What changed?", "I don't want to fold different releases notes into one. They should all be separate. I'll go back and create empty placeholders and we will reconstruct the notes." (tags and releases are refused for this session since Nov 15; George makes them by hand; release-1.0.59-beta to release-1.0.67-beta branches are the targets)
- **15:03Z:** I need to download the apk for .66 beta. (build 66 had failed on a compile error; 734th fixed it; built)
- **17:36Z:** New issues: Bottoms of menus are chopped off. Tops of menus still have a line separating them from menu titles. Layers menu is a mess at small sizes. The icons jumble. There is a miniature gadget overlapping on the + sign at bottom of layers. Layers opacity bar goes all the way to the edge with no margin. Bad. Menus have no line around them or beveled edges so they just merge with other menus. Text in the tool options for Pan tool are too close to the edge. Go back to simple blue hilight for menu selections. The glow is too much. (735th)
- **18:28Z:** It's not the name of the tool. It's where it says no options. (736th) / **18:29Z:** 5. Why not just ask, chatbot? It's the left margin. (737th) / **18:30Z:** Build it. (build 67)
- **20:11Z:** Same short menus. Same line dividing menus from titles. Same orange K too close to the File menu. No changes. / Can't you just add a blank line at the bottom of the menus? (738th)
- **20:13Z:** Can't grab panels to tear them off. No handles. / There is nothing to grab. / **20:16Z:** Can't grab the edge of the side panel either. (open)
- **20:17Z:** There is an ugly extra line on the right side of the toolbox and the left side of the side panel. It does nothing but ruin my day. Remove it (739th)
- **20:18Z:** If it's a grab area, it's not big enough and should also be invisible as previously discussed. Make a note of this. Save notes. (740th; rule written in KRIMBLE_CHANGES.md)
- **20:24Z:** In painted theme make the selected tab the color values in this color lab image. (Color Lab showing #374165; 741st)
- **00:53Z (Nov 18):** Plus color lab slides the logo up into nowhere and then you have to use a scrollbar to get it back. That should never happen. / DUH. It should always have been pinned. / THE COLOR LAB LOGO SHOULD ALWAYS HAVE BEEN PINNED. (742nd) / Keep your answers really really short because I won't read long answers. Like cut all the fat off your answers.
- **00:56Z (Nov 18):** The top of layers should always have had a minimum height. (743rd)
- **00:21Z–00:28Z (Nov 19):** Desaturate dialog: "Can that dialog." S to 0 in HSV, instant (744th). Menu text vertical spacing too tight (proposal: about 9 real pixels more above and below; not answered yet).
- **00:29Z (Nov 19):** And fix the painted theme. I had no idea themes changed spacing. Also, the smudge tool should be the default for that flyout set, not soften. (745th)
- **00:52Z (Nov 19):** I try to select something and a box covers what I'm trying to do just to tell me the pixel dimensions. I thought we had moved that thing. / Perfect. (746th)
- **00:54Z (Nov 19):** I tried resizing the color lab panel and it hid the close gadget on the upper right corner. / The buttons should move with the window edge. (747th)
- **00:57Z (Nov 19):** The layers panel looks weird. I tried to hit the plus to add a layer and instead got a menu. Why??? / Yes. (748th)
- **00:58Z (Nov 19):** All those extra buttons are just a mess. The empty space looks bad and the sloppy symbol positions look bad. Just get rid of them. (749th)
- **01:01Z–01:03Z (Nov 19):** It's hard now to get to the long press menu for the toolbox... Unresponsive. I need somewhere to press. Maybe a little space at the top and bottom? / Ok. And a line at the bottom of the toolbox even when it's docked. (750th)
- **04:01Z (Nov 19):** I guess so. I want to change the gray theme that has the orange selections. They should all have the plain blue selections. (751st)
- **04:24Z (Nov 19):** Did we ever get a decent default brush set up like before? ... Find out what it was before. / Just restore or recreate that brush. (752nd)
- **18:39Z:** Now the goddamned layer type slider is back. I don't want that there!!!!!! / We moved that for a reason, and it wasn't to make room for the stupid extra tools. (753rd)
- **18:43Z–18:57Z:** grab pad request, mockups, "Make it bigger", "The same spaces that were odd empty spots should be grab pads.", "Yes", "Instead the tabs just slide to the side and spring back into place." (754th)
- **06:52Z (Nov 20):** Use branching. Prioritize and implement all. (755th, branch krimble-cleanup)
- **10:22Z (Nov 20):** Do the other steps next. (756th, branch krimble-cleanup)
- **10:57Z (Nov 20):** 1. Much harder to just select toolbox tools. 2. grab areas look bad... 6. Menus are still a jumble of different hues. Make them match. Same hue. (757th)
- **11:04Z (Nov 20):** Grab lines should not go all the way to the top edge, but they should fill up the area... / In the layers panel reserve an area for those top sections so they don't get squeezed and overlap at all. (758th)
- **11:06Z (Nov 20):** Make grab area lines invisible on color lab. (759th)
- **11:16Z (Nov 20):** Fix all of that before any build can take place. (760th)
- **11:19Z (Nov 20):** Yes. Delete the inline notes. (761st)

## Session of 2026-11-20, 04:19 MST to 23:05 MST (times in MST; "~" = time not kept, the chat was compressed and only a summary survived)

What was lost: the screenshots George sent during the day were not saved anywhere until 22:51 MST. They were recovered from the session's upload folder at 23:01 and are now in `docs/krimble-refs/`. From now on every image goes there as it arrives.

- **~ day:** For the hundredth time: THE ORANGE K IS RIGHT ON TOP OF THE FILE MENU TITLE. FIX IT. (screenshots TOO FAR / TOO CLOSE) "Lock it into a specific pixel distance for both before and after file open." "Let's try 90." (767th+)
- **10:01:** Color Lab is easy to grab but won't reattach. Brighter blue toolbox selection, flat like the menu: "The glowy thing doesnt look professional." Swatches up to 256 with resizing. New document window size on launch based on the screen. "I have another image loaded at tiny size again."
- **~:** "I don't want these little report boxes any more. Report these stats on the status bar."
- **~:** "Why is there a gray stripe outside the shadow????" "It is NOT pointing at the glowing selection." "There should not be a gray stripe there. Ask questions if you're not sure about something." RULE: "We do not draw any more content outside the shadow area. EVER. THE SHADOW IS THE OUTER EDGE." "Now, maybe the shadow can be moved a little farther out, but NOTHING SHOULD BE DRAWN OUTSIDE THE SHADOW."
- **~:** Toolbox bevel: "They need to be thicker. The hilight area and shadow area should be the same number of pixels thick." "As it is the shadow just looks like a black line and that is no good." "The raised buttons came out pretty good. Why the difference?" "Demo for me." "Let's try the raised button style. Didn't the old Photoshop have black lines between the buttons?"
- **~:** K on image windows: "Yeah. Too far from the corner." "Yes. Pin them to the corner." (screenshot) "I'm trying to show you what looks right."
- **~:** "Build it." / "Everything goes into the build." / "Stop. No build." / RULE: "I told you everything goes into the build. Don't start a build until all the changes are written. Ever." / "Get rid of that stripe. Fix the hilights. All of it. Everything."
- **~:** Resets: "You have to find a solution to the constant resets. DATA LOSS IS NOT THE PROBLEM!!!!!!! THE PROBLEM IS WASTED TIME RESTARTING AND FRUSTRATION WITH HAVING NO CONTROL OVER THE APP!!!" "Stopping in the middle of a workflow is extremely frustrating." "The app is generally running better after the efficiency edits. But the app still restarts." (KeepAliveService pushed; still open: memory use, needs a log screenshot after a reset)
- **~:** Release notes: "I need to do the .71 release first." "a lot of the notes you write about these releases are inaccurate or omit major issues. You don't get my bug lists until afterward. I think these should reflect my actual issues with the code instead of your assumptions."
- **~:** Layers: "The check mark is too close to the eye." "After that's fixed build."
- **19:43:** "Just stop the build. There are more issues." Issues with screenshots: grips like his mockup; grips in the black part of the Color Lab logo; menus squeeze on Fill; magenta move box; Wesnoth fire frames alignment and Krita's animation tools; "I have the Layers menu title repositioned. See graphic."
- **~:** "Aqua has to go. Replace with selection blue."
- **~:** "You need to make the grips bigger to match my illustration and move the Layers panel title like I showed you." ".72 does not feature these changes." "I have had so many discussions with you about edits. I know you cannot retain these memories."
- **21:48:** "Soon we will have a release candidate. Then an aba and a testing release. Then two weeks of testing and a release on the Play Store."
- **21:49:** "Bright aqua outlines? You mean the glow effect?" "The glow effect is more cyan I think."
- **22:47:** "Listen. Don't worry about those. Just the one I identified." "I was wrong about lens flares. I just saw a high quality one on civitai and realize we need them." (reference image saved)
- **22:49:** "You have to make a set of lens flare effects from scratch. And light sabres." "None of those boxes you named were the problems I showed you."
- **22:50:** "What do you mean except the glow? What about the color lab colors? What are you doing??"
- **22:51:** "I use images to clarify what I want and you lose the images and do random things? No bueno." RULE: "Save everything I send you."
- **22:52:** (16 screenshots) "What was I trying to say with each of these images?"
- **23:00:** Ball bearing image: grips not done as instructed. Recent Images area too small, tiny scroll bars. Circled aqua: "I previously asked for that specific dull aqua color in the theme to be changed and you didn't do it." Grips and Layers window title: "you're ignoring my instructions." Save As: "still massively wide... All I wanted was for it to be taller... I don't want it to be wide." Fill submenu: "turns menus miniature and it looks so tiny itself as a submenu. It's ridiculous." "Are you getting any of this at all?"
- **23:01:** "Why didn't you keep my mockup???"
- **23:02:** "These are important development instructions. This is how real development gets done. Art departments sketch things out and use Photoshop and other tools to communicate."
- **23:03:** "I want the same grip lines on the black part of the Color Lab logo so I can grab it. Make it look nice and centered on the banner, and don't touch the letters."
- **23:05:** RULE: "Keep lists of these instructions. Keep a summary of every chat at the very least, if not the whole chat." "You're not properly tracking my side of this." RULE: "don't mention my name in the release documents."
- **23:06:** "Yes." (name taken out of the release notes) "Also try putting a mild gradient in the glue area. Not too intense. Make it nice."
- **23:06:** "Blue not glue." (the gradient is for a blue area)
- **23:07:** "Show me a gradient effect on a button." (demo sent: flat / mild / a bit more)
- **23:11:** (Mac OS Aqua button image) "When I said "aqua" before I was talking about that ugly seaweed blue green. But mac os Aqua was beautiful. Can you match this gradient effect? Is that asking too much? It's a complicated multi point gradient."
- **23:12:** "It's amazing. I didn't know you could do that." (Aqua-style gradient demo approved; where it goes not said yet)
- **23:13:** "Try all the places we had a glowy hilight before."
- **23:14:** "If you can do that the whole UI could look amazing."
- **23:17:** "Yes. All of it yes." (781st Color Lab grip lines, 782nd Aqua selection)
- **23:18:** "Yes." (do the remaining checked buttons: Transform tool modes, checked dialog buttons)
- **23:20:** "Yes" (783rd committed)
- **23:21:** "I was going to ask if we could figure out how to build an all-in-one gadget that does what the bottom of the Photoshop toolbox does, but it seems like a lot." "Did you put any planning into lens flares?"
- **23:23:** "We already have the foreground and background color swatches. I meant the gadgets below that. We had something clunky there before. Quickmask and screen modes never sat right." "Yes, try to make lens flare effects."
- **23:25:** "Nice. What parameters do they use? Are they shaders or vectors?"
- **23:26:** "So they just put it where they want it in an image? Can we rearrange it in realtime as they drag it around in the image?"
- **23:27:** "Be sure to document this in the readme and the help file and the user manual."
- **23:28:** "Please get the features up to date in the readme. We have accomplished a lot."
- **23:28:** "Yes." (lens flare text added to README, help file and manual)
- **23:29:** "Make sure all the new features (the ones not in Krita) are mentioned in the readme."
- **23:30:** "Do we have customizable vector based hex map overlays now? You can set line thickness and color?"
- **23:31:** "I don't think we ever fixed auto tone or auto color, any of that stuff."
- **23:32:** "You don't understand curves yet?"
- **23:33:** "Research these features online. Look for open functional algorithms."
