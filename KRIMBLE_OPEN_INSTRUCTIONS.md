# Krimble open instructions

George's instructions that are not finished yet, in his words, with the reference image where there is one (`docs/krimble-refs/`).
Release notes are written from this list and from his test results, not from assumptions. Move a line to "Done" only when it is in a build and he has not reported it again.
Last updated: 2026-11-26 19:05 MST. Current release: v1.0.80-beta (docs/releases/).

## Open

| Since | Instruction | Reference | Status |
|---|---|---|---|
| Nov 24 / Nov 26 | Blurry, upside-down image. Gone in 1.0.80 at first, back on Nov 26 after many images were opened and the Layers panel was hidden over and over; possibly memory related | KRIMBLE_BUG_LISTS.md (1.0.80, later report) | open; the 72 suspects were removed in 891st |
| Nov 20 | Restarts (Android closing Krimble) | | better in 1.0.80 (the New dialog sat open a long time without a restart); still happens |
| Nov 26 | The menu bar does not need its bottom edge (it makes menus look separated) | 2026-11-26_1436_preferences-icons-too-big.jpg (dark band under the bar) | parked by George ("We can worry about it later"); not drawn by Qt or the style sheets in desktop Qt |
| Nov 25 | Layers rows back to grays, selected layer a dull yellowish color | | shade not confirmed; not done |
| Oct 23 (test list 50) | Restyle part of a text (one word bold, one letter another color) | | NOT DONE |
| since 1.0.56 | Transparent text can save differently from how it looks on the canvas | | known issue |
| Oct 23 to Nov 1 | 52 test-list items marked DONE by Claude (KRIMBLE_TEST_LIST_2026-10-23.md); only one has his confirmation | | treat as untested until he says they work |
| Nov 26 | 1.0.80 list items 1, 2, 4, 7 to 12, 14 and the image-window grip | KRIMBLE_CHANGES.md 901st to 912th | pushed to cyber-theme, not in a build |
| Nov 26 | Feature videos (screen recordings, one short clip per feature) | | when the app is stable |
| Nov 26 | Subscription model | KRIMBLE_DECISIONS.md | plan |
| Nov 20 | Release plan: release candidate, then an aab and a testing release, two weeks of testing, then the Play Store | | plan |
| Nov 21 | Perchance connector for generative images (idea) | | not asked for yet; File > A. I. Image gen opens Image Donkey since 1.0.74 |

## Done (shipped in a release, not reported again)

Everything that was "pushed, not in a build" here on Nov 21 has shipped since: grips and the Layers title (1.0.73 onward), Color Lab grip lines (1.0.73), Save As height, Fill submenu, Recent Images, Auto Tone/Contrast/Color (1.0.73), lens flares and lightsabers (1.0.73), Quick Mask and Screen Mode (1.0.73), toolbox icon styles and button backgrounds (1.0.73 to 1.0.74). The aqua items were superseded: Aqua dropped Nov 25, buttons are style A pills (Nov 26). Release text without his first name: done in the repo notes; he edits the GitHub release text himself.

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
