# Krimble test list "Mercury's Shadow" (George, 2026-10-23, on 1.0.41)

Several chats work on this list at the same time (each commit names its test list item). Before starting an item, check here that nobody has claimed it; claim it by adding a CLAIMED mark.

Status: DONE = pushed (in the build after 1.0.41); OPEN = not done yet; ASK = a question for George.

1. File > New: the window's minimum size is too big; clipboard picture smaller, camera picture larger. **DONE** (236th)
2. Presets: 8.5 x 11 @ 300 and 11 x 17 @ 300 (they existed as "US Letter" and "US Tabloid"; the names now show the inches), the comic page / daily strip / digest sizes, A4 and A3 (exist as "A4 (300 ppi)", "A3 (300 ppi)"), photo prints 3.5x5, 4x6, 5x7, 8x10, 11x14, 16x20 @ 300. **DONE** (236th)
3. Open Recent: great.
4. The Save and Export windows should open a little taller. **DONE** (239th)
5. File Info window is WAY TOO BIG; replace the Author picture with a robot head. **DONE** (241st size, 242nd robot head)
6. Edit > Puppet Warp opens the polygon select tool? No explanation? **DONE** (244th: it is the cage transform; a floating message now explains it)
7. Edit > Free Transform is just scale? Add rotate targets outside the corners? **DONE** (245th: touching outside the box rotates again; 246th: curved-arrow rotate targets outside the corners)
8. Edit > Sample Color: what does it do exactly? **ASK**
9. Edit > Convert to Profile window is too wide. **IN PROGRESS** (250th: every KoDialog window now has its main part in a scroll area; George's Oct 23 screenshots of Image Size, Image Properties, Split Image and File Info) **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23)**
10. Edit > Customize Toolbar is double the width it should be; why is it in Edit and not with the other toolbar menu items? Move it. **OPEN**
11. Edit > Preferences: smaller icons, narrower column (they are cool though). **OPEN**
12. Image > Mode > Color Mode: what does it do? **ASK**
13. Image > Mode > Edit Presets is too wide; it should come with more presets; 8-bit 256-colour palettes (old days): where is the option? **OPEN**
14. Image > Auto Color is bad: it should default to sweetening the colour saturation without changing the overall tone; these features need tweaking. **OPEN** **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23, in progress; other chats: skip)**
15. Image Size window is too wide. **IN PROGRESS** (250th: every KoDialog window now has its main part in a scroll area; George's Oct 23 screenshots of Image Size, Image Properties, Split Image and File Info) **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23)**
16. Canvas Size is perfect. 17. Image Rotation is good. 18. Crop tools are good.
19. Image Properties is too wide and has no corner sizing handle. **IN PROGRESS** (250th: every KoDialog window now has its main part in a scroll area; George's Oct 23 screenshots of Image Size, Image Properties, Split Image and File Info) **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23)**
20. Upscale and Batch look good, need more testing.
21. Layers menu looks good.
22. Quick Mask Edit: like what Photoshop has at the bottom of the toolbox; not sure how to implement. **OPEN / design**
23. Select menu looks good.
24. Filter menu looks good (too much to analyze right now).
25. The Text window is way too wide; it needs a complete redesign. **OPEN** **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23, in progress; other chats: skip)**
26. No Text menu at all? **OPEN** **CLAIMED by the KRIMBLE-MAIN chat (2026-10-23, in progress; other chats: skip)**
27. The handles on the toolbox and side panel are still too small to grab easily, but close. George drew an idea. **DONE** (240th, from his drawing)
28. Where is File > Revert? Put it back. **DONE** (237th)
29. Window > Stack panels (in the panels section, to stack the side panels). **DONE** (248th)
30. Detaching the toolbox goes to one column, then has the dangling empty bottom. **DONE** (247th)
31. (added 2026-10-23, George) 3D icons for the Preferences and New dialog lists, made from his new icon sheet (23 icons: General, Keyboard Shortcuts, Canvas Input Settings, Display, Color Management, Performance, Tablet Settings, Canvas-Only Settings, Pop-Up Palette, Author, Color Selector Settings, S-Pen Actions, Custom Document, Create from Clipboard, Animation / Comic / Design / DSLR / Texture Templates, Language, Resource Paths, Advanced Settings, Reset / Restore; the robot head top-left is already used for the Author pictures, do not use the sheet for those). **DONE** (249th; four spare icons saved: Language, Resource Paths, Advanced Settings, Reset / Restore)
32. (added 2026-10-23, George, with screenshots) "There's just a lot of empty negative space in these windows." **DONE** (251st: plain windows open at their contents' size with tight margins; tabs are as tall as the showing page)
33. (added 2026-10-23, George) The selection mode (Add / Subtract ...) was saved on exit and restored on the next start; he wants it to start on Replace. Everything else the tools remember stays. **DONE** (252nd)
34. (added 2026-10-23, George) A toggle to reset all options to defaults on restart. **DONE** (253rd: "Reset Tool Options on Start", next to Reset Preferences; resets the tools' saved options, not Preferences/panels/brushes)
35. (added 2026-10-23, George, 1.0.42) The toolbox's edge tab is about twice as tall as it should be (but works great); the side (right) panel has no tab. **DONE in part** (255th: tab 70 instead of 140; right tab drawn with a thin light edge, and the layout-info overlay says whether each tab is shown and why not; the cause of the missing right tab is not known yet)
36. (added 2026-10-23, George, 1.0.42) Window > Stack Panels does not work. **DONE** (257th, not yet seen)
37. (George, 1.0.42) "Toolbox got caught in horizontal mode for a minute." He was dragging it. **DONE** (261st)
38. (George, 1.0.42) Right panels could stand to be a touch more eager to attach. **DONE** (259th: the right side reaches 28 px, was 14)
39. (George, 1.0.42) The 3D icons in File > New are giant and he does not like them: go back; the Preferences icons are good. **DONE** (258th: the New window's pictures are the 2026-10-13 ones again; the Preferences 3D icons stay)
40. (George, 1.0.42) Smudge still takes over the other tools. **DONE** (256th: the real cause found, see the entry)
41. (George, 1.0.42) Free Transform's corner rotate gadgets are massive overkill: a little arrow, not loops. **DONE** (260th)
42. (George, 1.0.42) Image Properties is awkward but okay; Image Size is good; Content-Aware Fill works okay; Revert works; the right panel's size tab appeared. (notes)
43. (George, 1.0.41 item 6, again with 1.0.42 "works okay") Content-Aware Fill is a blur. **IMPROVED** (263rd: the coarsest level is filled ring by ring from the best patches instead of an average; thin holes measured by thickness; tested only on the bench pictures, not on the phone)
44. (added 2026-10-24, George) The About > "Also Thanks To" page is Krita's: put Claude and ChatGPT in it. **DONE** (265th)
