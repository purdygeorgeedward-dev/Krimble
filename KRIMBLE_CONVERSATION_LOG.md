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
