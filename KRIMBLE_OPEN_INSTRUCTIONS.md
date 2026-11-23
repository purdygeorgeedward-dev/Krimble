# Krimble open instructions

George's instructions that are not finished yet, in his words, with the reference image where there is one (`docs/krimble-refs/`).
Release notes are written from this list and from his test results, not from assumptions. Move a line to "Done" only when it is in a build and he has not reported it again.
Last updated: 2026-11-21 03:42 MST.

## Open

| Since | Instruction | Reference | Status |
|---|---|---|---|
| Nov 20 | Grips like the mockup: tall block of many thin lines filling the width | 2026-11-20_1943_MOCKUP-grips-and-layers-title.jpg, batch-01, batch-05 | 779th + 788th pushed, checked against the mockup by measurement; not in a build |
| Nov 20 | Layers panel title positioned like the mockup | same mockup | 779th + 788th pushed, checked against the mockup by measurement; not in a build |
| Nov 20 | Same grip lines on the black part of the Color Lab logo band, centered, letters untouched | batch-09 | 781st pushed, not in a build |
| Nov 20 | The dull aqua in the theme (selected Mode/Action buttons in Tool Options) changed; he asked before and it was not done | batch-03 (circled), batch-04 | 782nd: replaced by the Aqua-style selection; pushed, not in a build |
| Nov 20 | "Just the one I identified": the other aqua changes in 780th were not asked for | batch-03 | settled Nov 21 04:08: "use it anywhere we can": 793rd made those elements Aqua; pushed, not in a build |
| Nov 20 | Save As dialog: taller only, NOT wide | batch-06 | 785th pushed, not in a build |
| Nov 20 | Edit > Fill: highlighting Fill makes the menu miniature; the Fill submenu looks tiny | batch-07, batch-08, 2026-11-20_1943_9e68dea9, _ea45659b | 777th (squeeze) and 786th (submenu room and row height) pushed, not in a build |
| Nov 20 | Recent Images area too small, tiny scroll bars | batch-02, batch-15 | 787th pushed, not in a build |
| Nov 20 | Aqua-style gradient (Mac OS Aqua) on every place that had the glowy highlight | 2026-11-20_2311_macos-aqua-gradient-reference.png, 2026-11-20_2312_APPROVED-aqua-gradient-demo.png | 782nd and 783rd pushed (toolbox, button rows, Color Lab, every other checked button), not in a build |
| Oct 23 / Oct 26 | Auto Tone, Auto Contrast, Auto Color: "These should be based on curve profiles." Auto Color "should default to sweetening the colour saturation without changing the overall tone". Nov 20: "I don't think we ever fixed auto tone or auto color, any of that stuff." | KRIMBLE_RESEARCH_auto_tone_color.md | 784th rebuilt as per-channel curves + 792nd Auto button in Curves; pushed, not in a build |
| Nov 20 | Lens flare effects made from scratch (starburst, rays, colored ghosts), dragged live on the picture | 2026-11-20_2247_lens-flare-reference.png, 2026-11-20_2327_demo-lens-flares-v1.png | 789th filter + 790th finger drag on the picture; pushed, not in a build |
| Nov 20 | Quick Mask and Screen Mode under the color swatches, as in the industry-standard toolbox ("We can make what the industry standard has.", Nov 21 04:28) | 2026-11-21_0428_ps-toolbox-bottom-quickmask-screenmode.jpg | 794th pushed, not in a build |
| Nov 21 | Idea, not asked for yet: a Perchance connector for generative images ("We have a full toolbox. Otherwise we could add an interface connector for Perchance." / "We already built it.": the Perchance side exists in his Claude-Perchance extension, repo purdygeorgeedward-dev/Claude-Perchance; his working generators: perchance.org/imagine-it and perchance.org/image-donkey; use perchance.org/image-donkey, the address kept as a setting: "Just use one or the other. It's just a url.") | | DONE 810 (Nov 21 17:07 "Add it now and document it."; 17:08 "Put it in File as \"A. I. Image generation\"."): File > A. I. Image gen (17:09: "shorten it to gen") |
| Nov 21 | Black and white toolbox icons as a third style: the silhouettes in black ("I'd like to see the silhouettes in black.") | 2026-11-21_0436_ps-toolbar-labeled-grayscale-icons.jpg, 2026-11-21_0510_preview-black-silhouettes-light-gray.png | 795th + 797th pushed, not in a build |
| Nov 21 | A switch for light gray toolbox buttons, like the Raised switch | 2026-11-21_0500_preview-light-gray-buttons.png | 796th pushed, not in a build |
| Nov 21 | White, black, grayscale icons (color removed 800th: "Remove the color option completely"); light gray or white button backgrounds; a bigger eyedropper | 2026-11-21_0520_preview-icon-styles-and-button-backgrounds.png | 798th pushed, not in a build |
| Nov 21 | Classic icon style: black line art for shape tools, grayscale art for object tools, like the classic toolbar | 2026-11-21_0436_ps-toolbar-labeled-grayscale-icons.jpg, 2026-11-21_0530_preview-classic-icons.png | 799th pushed, not in a build |
| Nov 20 | Lightsabers (from scratch) | 2026-11-21_0500_test-lightsabers.png | 791st: Filter > Render > Lightsaber, finger drag; pushed, not in a build |
| Nov 20 | Constant resets (app killed by Android) | | Don'tCrash (KeepAliveService) pushed (background kills); front kills rare per George (Nov 21 04:16): watch after the next build, Krimble log screenshot if one happens |
| Nov 20 | Release notes with no mention of his name | | repo notes done (5d8c6a34); GitHub release text of 1.0.59 to 1.0.63 and 1.0.66 still says "George" (to edit on GitHub by hand). 1.0.41 (full name in About credit) stays: "The one with my full name is fine." |
| Oct 23 (test list 50) | Restyle part of a text (one word bold, one letter another color) | | NOT DONE |
| Oct 23 to Nov 1 | 52 test-list items marked DONE by Claude (KRIMBLE_TEST_LIST_2026-10-23.md); only one has his confirmation | | treat as untested until he says they work |
| Nov 20 | Release plan: release candidate, then an aab and a testing release, two weeks of testing, then the Play Store | | plan |

## 2026-11-21 15:39 — New tool concept: multi-point gradient (not started)
"User taps screen ti create control points and the tool creates a web of lines between those nodes and fills it in with a gradient based on colors selected by the user."
15:42: "The multi-point gradient would create either linear ._._._._. or 3d gradients (many island nodes) and gratients form between the nodes in different directions. Each would exist as a vector object with control points the user can move and reposition at will."
(Two kinds: a chain of nodes (linear), or free island nodes blended across a mesh between them. A vector object; nodes stay movable.)

## 2026-11-21 17:38 — Virtual screens (back burner, after the Play Store release)
"Put different images and/or panels on different virtual screens and flip back and forth between them." / "No more than 3. Dots make sense. I don't think 3 finger swipes will catch on."
(Up to 3 screens, each one image plus its own panel layout; switched with dots, no three-finger swipe.)

## 2026-11-21 17:41 — Voice control (idea, asked about)
"Could we add voice control to the app? Activate tools, open and close files, that kind of thing?"

## 2026-11-21 17:36 — Back burner: "Let's put these complicated things on the back burner." (d4 color picker, color sphere, 3D map tools, curve and mesh gradients, true dual-screen)

## 2026-11-21 17:29 — Gradients (three features) and 3D map tools (not started)
"The industry standard has complex gradients with multiple colors at different points on a line. I want that too, and I want a linear preview in the editor where it shows a large box with a vertical image of the selected gradient."
"1. Gradient on a curve with the points of the gradient plotted on the curve to control how it renders, 2. A 3D mesh based gradient map where the gradients flow from node to node in a freeform way."
"how hard would it be to add a bump map editing tool and a normal map editing tool for 3d work?" / 17:31 "I think editing/painting those maps requires a 3d interface."
(Already in the code base: multi-stop gradients and their editor (KisStopGradientEditor), SVG mesh gradients for vector shapes (SvgMeshGradient, KoMeshGradientBackground), the Height to Normal Map and Phong Bumpmap filters, the Tangent Normal brush.)

## 2026-11-21 15:37 — Test 1.0.73 list (in progress; see KRIMBLE_CONVERSATION_LOG.md for his words)
- **Before every push of C++ (2026-11-22):** run the syntax check twice, plain and in Android mode: `KRIMBLE_CHECK_FLAGS="-DQ_OS_ANDROID -Itools/syntax-check/android-stubs" python3 tools/syntax-check/syntax_check.py <file>`. UI changes are rendered with the Preview Tool before they are shown or pushed.
