# Krimble code comments, moved out of the code (2026-11-23)

On Nov 23, 2026 George asked for every comment to be removed from the code ("Explanations go in a separate dev log, not in code."). Before they were cut, every comment that mentioned Krimble or George, and every comment in a Krimble-named file, was copied here with its file and line number at that time. The code itself, with all its comments, is in git history before this commit.

## `libs/flake/KoToolFactoryBase.h`

- line 29: ///< Krimble: tools with an industry standard equivalent, ordered to match PS's toolbox

## `libs/flake/KoToolManager.cpp`

- line 606: // Krimble: the tool name is already the first line of Tool Options (Krimble Type showed its name twice)
- line 642: // Krimble 2026-11-22: one tool for the whole program. Krita keeps a tool per image, so going to another image (or coming back from the
- line 644: // tool in use": new images opened with the Healing Brush (George: "This is a major bug."). An image that becomes active now takes
- line 738: // Krimble: a new image keeps the tool the user has selected; Pan only when there is none yet (program launch)

## `libs/flake/KrimbleShapeModeRow.h`

- line 2: // Krimble 2026-10-30: the two vector tools are one toolbox button with a toggle. George (2026-10-30): "I think you should have left it. Edit shapes can be a toggle
- line 3: // or something." The toolbox button, with George's arrow picture, is the **Select Shapes** tool (the black arrow: moves, scales and rotates a whole vector
- line 4: // object); the **Edit Shapes** tool (the white arrow: the points and handles of an object) has no button of its own. Instead the Tool Options of each tool start
- line 5: // with this row of two buttons, **Whole object** and **Edit points**; pressing the other one switches to the other tool (one tap).
- line 38: // queued, so this button is not deleted inside its own click

## `libs/image/kis_image.h`

- line 238: /**
  * Krimble: optional customActionName lets callers (e.g. the Canvas Size
  * dialog) label this in the undo history as something other than the
  * generic "Resize Image" -- see KRIMBLE_CHANGES.md.
  */

## `libs/ui/KisApplication.cpp`

- line 416: /**
  * Automatically assign sRGB color space to all Krimble windows,
  * which are not marked with a special tag.
  */
- line 619: // Krimble 2026-11-23: the bundled fonts (95 files, about 13 MB) are no longer loaded at every start. They are loaded the first time
- line 621: // (George: "we don't need extended fonts until the text tool loads."). The log says how long the loading took.
- line 711: // krimbleLoadBundledFonts();   // at every start until 2026-11-23; now on first use (krimbleEnsureBundledFonts)

## `libs/ui/KisApplication.h`

- line 38: /// Krimble 2026-11-23: loads the fonts that come with Krimble, once, the first time they are needed (text tools, files that can hold text).

## `libs/ui/KisBrushColorWells.h`

- line 11: /**
  * KRIMBLE 2026-10-05: the foreground / background color wells (the same two-squares widget as at the bottom of the
  * toolbox) as a toolbar item named "brush_color_wells", so the "BrushOptions" toolbar in krita5.xmlgui can show them
  * (George: "I think it might be a good idea to have color wells on the brush toolbar").
  */

## `libs/ui/KisMainWindow.cpp`

- line 346: // Krimble: finger-sized scroll bars (48 real px) on the Recent Images page, in place of the thin default ones
- line 764: // Krimble 2026-11-23: it ran every 8 s for as long as the app ran, walking the whole window each time (George: "When should it shut
- line 847: // if (!floatCfg.readEntry<int>("krimble_toolbox_floated_once", 0)) {
- line 1283: // Krimble 2026-11-22: the images that are open are remembered, so that after Android ends Krimble in the background (the restarts) the
- line 1284: // next start opens them again. George: "The problem is wasted time restarting"; "Do them all in order as needed."
- line 1309: // the list is emptied before opening: an image that ends Krimble while loading is not tried again at every start
- line 1327: // when Krimble goes to the background, the list is written once more (a Save As changes a path)
- line 1334: // Krimble 2026-11-22: a newly opened image is fitted again each time its canvas gets bigger in its first seconds, until the user touches it.
- line 1343: // Krimble 2026-11-23: no time limit: the new image is fitted again on every canvas size change until the canvas is first touched
- line 1344: // (George, build 78: "Still opened a file tiny." / "Yes. Fix it.")
- line 1358: // Krimble 2026-11-22: the image opened blurry and turned around, and George: dragging or zooming "fixes itself after". Redraw calls
- line 1359: // do not help on Android after the window is hidden and shown at open (Krita's own slotFlashWindowHack, already in Krimble). So the
- line 1759: // Krimble 2026-11-23: a file that can hold text layers gets the bundled fonts before it is read, so its text is laid out with them
- line 2030: // Possible when file is moved/unmounted during Krimble session.
- line 2225: // Krimble (1.0.73 test, "It relaunched on switch again"): the log showed Android asked Krimble to finish (onDestroy, isFinishing=true)
- line 2228: // moves Krimble to the background (as Home does) and leaves it running.
- line 3208: // Krimble 2026-11-22: panel content had no margin, it touched the panel's left edge (George: "All the panels start their internal
- line 3210: // the toolbox is left out: its tool icons fill it edge to edge (George 2026-11-23: "we don't want a toolbox with a big margin")
- line 3211: // Color Lab (ColorSelectorNg) is left out too: its band and color areas are built to run edge to edge (George 2026-11-23:
- line 3212: // "Color Lab didn't need a margin added."). Margins are case by case (KRIMBLE_DECISIONS.md).
- line 3830: // connect(stackPanelsAction, &QAction::triggered, this, [this]() { krimbleStackPanels(this); });
- line 4325: // Krimble 2026-11-23: Help > Support Krimble... opens the Buy Me a Coffee page (George: "Help - support Krimble should go to the Buy me a Coffee page.")

## `libs/ui/KisMainWindow.h`

- line 256: /**
  * Krimble 2026-10-23: File > Revert: goes back to the saved file (closes the picture and opens it again), after asking when it has unsaved changes.
  */
- line 439: // Krimble: the Back button sends Krimble to the background instead of closing it (Android)

## `libs/ui/KisTypeOptionsBar.h`

- line 20: /**
  * KRIMBLE 2026-10-04: the "Type Options" toolbar, a horizontal bar of the most used text controls (font and
  * style, size, alignment) that is shown under the menu bar while the Type tool is active, like the options bar
  * of the industry-standard editors. The controls are widget actions in the normal action collection, so the
  * toolbar "TypeOptionsBar" in krita5.xmlgui (and "Customize Toolbar") can place and rearrange them. They
  * read and write through the same text properties interface (KoSvgTextPropertiesInterface) as the Text
  * Properties panel, so the two always agree.
  *
  * First version: Font and Style, Size, Align left / center / right. (Orientation, anti-aliasing, color, panel
  * button and cancel / confirm are not made yet.)
  */

## `libs/ui/KisView.cpp`

- line 204: // static bool s_krimbleOpening = false;   // removed 2026-11-23 with the put-back
- line 211: // if (s_krimbleAttaching || s_krimbleOpening) return;
- line 395: // Krimble 2026-11-23: removed. The put-back undid a tool picked in the first 1.5 s after opening; the real cause (a tool per image)
- line 397: // // Krimble 2026-11-22: new images opened with the Healing Brush (stamp) instead of the tool in use (George: "This is a major bug.").
- line 401: //     const QString wanted = s_krimbleLastTool.isEmpty() ? QStringLiteral("PanTool") : s_krimbleLastTool;
- line 402: //     s_krimbleOpening = true;
- line 406: //             qWarning() << "Krimble: tool on open was" << now << "- putting back" << wanted;
- line 409: //         if (last) s_krimbleOpening = false;

## `libs/ui/KisWelcomePageWidget.cpp`

- line 1031: // Krimble: no supporter page or banner under Recent Images: the space goes to the recent pictures

## `libs/ui/KrimbleBatch.cpp`

- line 1: /*
  * Krimble 2026-10-25: see KrimbleBatch.h.
  */
- line 88: // the WebP ones first (George works with WebP a lot), then the ones that were there before (Batch Process, 2026-10-12)
- line 140: // libwebp reads the picture with its transparency; Qt may not have a WebP reader on the phone
- line 162: // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 163: // the programs: what a recipe or a script does to a picture
- line 164: // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 211: // scaled in premultiplied form, so that the edges of a transparent picture do not get dark or light fringes
- line 228: // not premultiplied: the colors are changed, the transparency is not
- line 244: // a GIF cannot be written yet: a lossless WebP (animated, if the GIF is) is the closest
- line 319: // -255 to 255
- line 331: // a JPG has no transparency: what is still transparent becomes white
- line 345: // WebP: at most 16383 px on a side
- line 348: // not premultiplied: what libwebp wants
- line 363: // Qt: 100 is lossless
- line 376: // the older functions of the recipes, now written with the programs
- line 381: // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 382: // animated pictures
- line 383: // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 385: // frames x width x height: about 240 MB as pictures; more is refused (a phone has not the memory)
- line 405: // how long the frame just read is shown
- line 406: // 0 or 1 hundredth of a second: browsers show it for a tenth of a second
- line 478: // transparent
- line 479: // smaller files, slower: only for small animations
- line 492: // every frame must be as big as the first
- line 514: // no libwebp in this build: Qt cannot write an animated WebP
- line 571: // a note starts with # at the start of a line or after a space, and is followed by a space or the end of the line ("#ff0000" is a color, "# a note" is a note)

## `libs/ui/KrimbleBatch.h`

- line 1: /*
  * Krimble 2026-10-25: the engine of Batch Process: "recipes" (resize, format, quality, name), reading pictures (WebP included, with transparency), writing JPG, PNG and WebP
  * (lossless or lossy, with transparency). George: "I'm working a lot with webp. I would like to add this feature."
  * It uses Qt and, when it was built with it, libwebp (KRIMBLE_HAVE_WEBP). It has no Android code, so it can be tried on any machine (tools/batch-bench).
  */
- line 19: // FormatSame (a script without a format line): the picture keeps its kind
- line 24: // px, for the resize modes
- line 25: // a picture already smaller than the size is not made bigger
- line 27: // 1 to 100: JPG and lossy WebP
- line 28: // added to the name: "photo-suffix.webp"
- line 34: // PNG and both WebP
- line 35: // JPG and lossy WebP
- line 43: // Resize: a ResizeMode; Crop: 0 = square, 1 = W x H; Flip: 0 = horizontal, 1 = vertical
- line 44: // the number: size, width, degrees, amount
- line 45: // the second number: height
- line 46: // the picture may be made bigger
- line 47: // Flatten
- line 54: // "photo-web.webp"
- line 55: // "web-photo.webp"
- line 56: // the folder in Pictures/Krimble
- line 57: // an animation is written as separate numbered pictures ("frames")
- line 58: // an animation's speed in frames per second (0: each frame keeps its own time)
- line 59: // how many times an animation plays (0: for ever; -1: as the file said)
- line 62: // ---- animated pictures (GIF and WebP are read; WebP is written) ----
- line 70: // 0: for ever
- line 73: // Qt in this build has its GIF reader
- line 74: // an animated GIF or WebP; false if it cannot be read (a picture with one frame gives true and one frame)
- line 75: // format: FormatWebPLossless or FormatWebPLossy
- line 77: // error: "Line 3: ..." (empty program and no error for an empty script)
- line 79: // the recipe written as a script
- line 80: // FormatSame becomes JPG for a JPG, WebP (lossless) for a WebP, PNG for everything else
- line 81: // outputFormat: what resolveFormat said (if not given: the program's own, PNG when it is "same")
- line 83: // frameNumber from 1: "name-web-001.webp"
- line 86: // the short reference shown by the Help button
- line 92: // WebP can be written
- line 93: // null if it cannot be read (animated WebP and other things)
- line 94: // resize; a JPG is flattened onto white

## `libs/ui/KrimbleBatchDialog.cpp`

- line 1: /*
  * Krimble 2026-10-25: see KrimbleBatchDialog.h.
  */
- line 51: // the recipe and its settings (and the first line): in a scroll area, so that it fits a phone held sideways
- line 75: // resize
- line 92: // format and quality
- line 110: // the name
- line 150: // the run
- line 155: // shown while a run goes on
- line 169: // WebP cannot be written on this build: the two WebP formats are shown, but cannot be chosen
- line 216: // a star marks the user's own
- line 249: // the script is checked as it is typed: the first mistake is shown under it
- line 349: // one animated picture: every frame gets the steps of the program; then it is written as an animated WebP, or as separate numbered pictures ("frames"),
- line 350: // or, for JPG and PNG (which cannot move), as its first frame only
- line 410: // the window was closed (Android's back button) while a picture was processed
- line 413: // this build cannot write WebP
- line 414: // an animated GIF or WebP: kept as an animated WebP, or written frame by frame, or (JPG, PNG) its first frame
- line 422: // this build has no GIF reader

## `libs/ui/KrimbleBatchDialog.h`

- line 1: /*
  * Krimble 2026-10-25: the window of Batch Process: a list of recipes (the built-in ones and the ones saved by the user), the fields of the recipe (resize, format, quality, name),
  * "Save recipe...", and the run over the pictures chosen. It uses Qt widgets only: the choosing of pictures and the saving to the phone's Pictures folder (Android code) are
  * given to it as functions (KisMainWindow.cpp), so it can be tried on any machine (tools/batch-bench).
  */
- line 28: // pick: asks the user for the pictures (returns their paths); save: puts one finished file (a path in the cache, its mime type and name) where it belongs, true if it worked
- line 34: // what the fields (or the script) say now
- line 35: // switches to "use a script" with this text (for the bench)
- line 36: // the script, or the settings, as what is to be done
- line 37: // the run (used by the button, and by the bench)

## `libs/ui/KrimbleCmykEngine.h`

- line 30: // C, M, Y, K as 0 to 1
- line 32: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 33: // color math (D50, sRGB)
- line 51: // Y in percent (100 = a perfect white)
- line 73: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 74: // ink colors
- line 77: // the colors (XYZ, D50, 1.0 = a perfect white) of: 0 paper, 1 C, 2 M, 3 Y, 4 MY (red), 5 CY (green), 6 CM (blue), 7 CMY, 8 K (solid black ink on the paper)
- line 83: // the overprints made from the single inks: the reflectances multiply (the paper's color divided out)
- line 100: // the values of the industry standard Ink Colors dialog for SWOP (Coated): Y (percent), x, y of C, M, Y, MY, CY, CM, CMY, paper, K
- line 105: // ISO 12647-2:2004 (paper type 1 and 2), the characterization data FOGRA39: CIE Lab D50
- line 118: // the ISO 12647-2 / ICC printing condition PC4 (standard matte coated): paper and the colour aims of the 4 inks; the overprints are estimated
- line 134: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 135: // dot gain
- line 140: // false: the Standard curve from percent; true: the table below
- line 141: // the gain of a 50% dot
- line 143: // the printed dot (percent) for a plate dot of kPoints[i], for C, M, Y, K (used when curves is true)
- line 146: // The Standard curve: the gain of a dot of plate area a is gain50 * sin(pi * a)^0.8 (most at 50%, none at 0 and 100%). The shape was fitted to the Dot Gain Curves table of the industry standard Custom CMYK
- line 147: // dialog (a 23% curve: +10 at 10%, +16 at 20%, +21.5 at 30%, +23 at 50%, +15 at 80%, +8.5 at 90%; this gives +9, +15, +19, +23, +15, +9). The GRL formula of Viggiano,
- line 148: // gain = 2 * gain50 * sqrt(a * (1 - a)), is the older open one; it has a flat top near 90% that a solver cannot climb, so it is not used. The printed dot never reaches 100% before the plate does
- line 149: // (the curve is kept below a + (1 - a) * 0.98), which keeps it rising all the way.
- line 161: // a monotone cubic through (0, 0), the 13 points and (1, 1) (Fritsch and Carlson)
- line 187: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 188: // the profile: everything the Custom CMYK dialog sets
- line 191: // empty: made from the settings, like "SWOP (Coated), 20%, GCR, Medium"
- line 192: // a preset's name, or "Custom"
- line 195: // false: UCR
- line 196: // None, Light, Medium, Heavy, Maximum (GCR only)
- line 197: // percent
- line 198: // percent
- line 199: // undercolor addition, percent (GCR only)
- line 201: // when a plate of a CMYK palette swatch is edited, the ink limits hold
- line 262: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 263: // the engine
- line 271: // the 16 overprints: index bits 1 = C, 2 = M, 4 = Y, 8 = K
- line 272: /*CM*/
- line 272: /*Y*/
- line 272: /*CY*/
- line 272: /*MY*/
- line 272: /*CMY*/
- line 272: // bits (C=1, M=2, Y=4) to the ink set's index
- line 284: // the color (XYZ, D50) that the four plates (0 to 1) print: dot gain, then the Neugebauer equations with the Demichel weights
- line 301: // sRGB to plates
- line 306: // plates to sRGB
- line 315: // the ink limits as a guard for plates that are typed in: the black is capped at the black ink limit and the total at the total ink limit (the plate that was just changed gives way)
- line 327: // still over: C, M, Y give the same amount each (black stays)
- line 337: // the plates of a neutral ramp, from white (0) to black (1), for the Gray Ramp picture
- line 348: // the black generation: the FRACTION (0 to 1) of the gray component (the smallest of C, M, Y of the K = 0 separation) that black takes over. GCR starts at a gray level that the preset sets
- line 349: // (Light 40%, Medium 20%, Heavy 10%, Maximum from the start, as in the industry standard Black Generation presets) and rises to the full replacement at the darkest.
- line 362: // UCR: only dark, neutral colors get black ("skeleton" black); chromatic colors keep their C, M, Y
- line 367: // the K plate that is as dark as C, M, Y all at g (the "gray component" in K plate terms)
- line 395: // C, M, Y (least squares, Levenberg-Marquardt) so that the plates with the given K print the wanted color
- line 398: // the match is measured in CIE Lab (the error is the square of the color difference, delta E): the same error means the same visible difference in light and dark colors
- line 437: // C, M, Y for a chosen K: the Levenberg-Marquardt from three starting points (a guess that allows for the black, the hint the caller gives, and the middle), the best is kept
- line 458: // a first guess for C, M, Y when black is K: black dims every channel (the black ink's color over the paper's), so C, M, Y only have to make the wanted color divided by that
- line 482: // a first guess for C, M, Y from the wanted color
- line 500: // the K = 0 separation
- line 507: // how well can this color be matched at all (with the best black)? a scan of K from 0 to the black limit; the black may not make the match worse than that
- line 518: // delta E squared: a black generation may be at most about one delta E worse than the best match
- line 521: // the least black that matches (colors darker than C, M, Y can print need some black): found between the scan steps by bisection, so that it changes smoothly with the color
- line 522: // (before it was a multiple of one eighth of the black limit, which made the plates jump)
- line 538: // The wanted black: from the DARKNESS of the color (its lightness L* from the paper's to the darkest the inks print), a smooth curve from no black at the light end to the black limit at the
- line 539: // darkest; the black generation presets set where the black starts and how fast it rises. The gray component (the smallest of C, M, Y of the K = 0 separation, over the largest) says how much
- line 540: // of it a chromatic color gets, so vivid colors keep their C, M, Y. (Before, the black came from the gray component's plate level, which stops changing for colors darker than C, M, Y all solid.)
- line 553: // UCR: only dark, neutral colors get black
- line 564: // too much black for this color: the biggest K between the least black and the wanted that still matches
- line 579: // the total ink guard: the smallest black (not below the one above) at which the total is within the limit and the color still matches. A scan in 24 steps finds the interval, then a bisection
- line 580: // inside it finds the black (a continuous result; before it was the scan step itself, which made the plates jump). If no black matches, the best match within the limit is used.
- line 613: // black is at its limit and the inks are still over: C, M, Y are reduced equally
- line 620: // undercolor addition (GCR only): some C, M, Y put back under the black of neutral shadows
- line 630: // the version before 2026-11-11 (kept, not used)
- line 636: // the K = 0 separation
- line 643: // how well can this color be matched at all (with the best black)? a scan of K from 0 to the black limit; a black generation may not be worse than that match
- line 654: // delta E squared: a black generation may be at most about one delta E worse than the best match
- line 656: // the least black that matches (colors darker than CMY can be need some black)
- line 666: // too much black for this color: the biggest K between the least black and the wanted that still matches
- line 682: // the total ink guard
- line 700: // black is at its limit and the inks are still over: C, M, Y are reduced equally
- line 707: // undercolor addition (GCR only): some C, M, Y put back under the black of neutral shadows
- line 725: // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- line 726: // the working profile (the one the Color Lab converts with) and the saved profiles (files in the app's data folder)
- line 751: // the engine of the working profile (made again when the profile changes)

## `libs/ui/KrimbleCmykIcc.h`

- line 31: // ICC v2 16 bit Lab: L 0 to 100 is 0 to 0xFF00; a and b -128 to 127.996 is 0 to 0xFFFF (0 at 0x8000)
- line 37: // a lut16 (mft2) tag with identity input and output tables
- line 60: // Unicode language code and count
- line 61: // Macintosh script code and count
- line 74: // Builds the profile file. The main thread stays responsive (a progress box with Cancel); returns an empty array when cancelled.
- line 80: // A2B: CMYK to Lab (fast, in this thread)
- line 92: // B2A: Lab to CMYK (the separation, on all the cores)
- line 119: // the file
- line 127: // body: index into bodies
- line 139: // header (128 bytes)
- line 140: // size, filled in below
- line 141: // preferred CMM
- line 142: // version 2.4
- line 148: // platform, flags, manufacturer, model
- line 149: // attributes
- line 150: // rendering intent: perceptual
- line 151: // the illuminant (D50)
- line 152: // creator
- line 154: // tag table
- line 167: // builds the profile, saves it in the icc_profiles folder and adds it to the color management; returns the profile's name (empty when cancelled or failed)
- line 185: // a profile with this name may already be in use (the same settings were made before): it is in the folder, and is taken at the next start
- line 192: // namespace KrimbleCmykIcc

## `libs/ui/KrimbleCommonStyle.h`

- line 2: // Krimble 2026-11-20 (756th): George: "Do the other steps next." (the cleanup plan's "one style sheet"). Rules for size and spacing that every Krimble theme shares live here, once,
- line 3: // and are added after the theme's own sheet (thememanager.cpp), so a spacing fix can no longer reach one theme and miss another (as the 738th's menu room missed Krimble Painted).
- line 4: // The theme sheets (KrimbleVisualStyle.h, KrimbleVisualStylePainted.h) keep only their colors and looks. Check a change with tools/preview/krimble_preview.py before a build.

## `libs/ui/KrimbleCustomCmykDialog.h`

- line 2: // Krimble 2026-11-11: George: "I really want [industry standard] style interfaces for RGB to CMYK." and, of the pictures of the Custom CMYK, Dot Gain Curves, Ink Colors and Color Settings dialogs: "yes"
- line 3: // (start with the Custom CMYK dialog). The Custom CMYK dialog of the working CMYK profile, built on the conversion engine (KrimbleCmykEngine.h, changelog 450): name, ink colors (the presets),
- line 4: // dot gain (Standard with its percent, or Curves... for the 13 point table of each ink), separation type (GCR or UCR), black generation, black ink limit, total ink limit, UCA amount, black point
- line 5: // compensation, and a gray ramp graph that shows the plates the engine makes for a gray from white to black. Load... and Save... use the saved profiles of KrimbleCmykProfiles. The dialog is a
- line 6: // scroll area, so it fits a phone. Header only and without Q_OBJECT (everything is wired with lambdas), so no moc or CMake change is needed. NOT in this first version: the Ink Colors dialog (xyY or Lab
- line 7: // values of each ink; only the presets for now), the display control points with midpoints, and the camera made profile.
- line 44: // the gray ramp: the C, M, Y and K plates (percent, up) the engine makes for a gray from white (left) to black (right)
- line 75: // C, M, Y, K (K is light so that it shows on the dark graph)
- line 96: // the dot gain curve of one ink: the printed dot for a plate dot (a straight line is no gain)
- line 135: // Dot Gain Curves: the printed dot for a plate dot of 2, 4, 6, 8, 10, 20 ... 90 percent for each ink (All Same sets the four inks together). Returns true on OK.
- line 234: // ink options
- line 259: // separation options
- line 298: // the labels may wrap onto two lines, so that no label forces the dialog wider than the screen
- line 303: // the buttons: Load..., Save..., OK, Cancel
- line 319: // the gray ramp is made a moment after the last change (the engine solves 25 grays)
- line 330: // the profile to the widgets
- line 350: // the widgets to the profile (except the name)
- line 359: // something changed: the profile follows the widgets, the name follows the settings (unless it was typed), the gray ramp is made again

## `libs/ui/KrimbleHelpDialog.cpp`

- line 1: /*
  * Krimble 2026-10-25: see KrimbleHelpDialog.h.
  */
- line 37: // "Go to" the parts of the handbook (its "## " headings), and Find
- line 72: // as it is typed: from the start of the word being typed
- line 92: // the heading at the top of the window
- line 107: // from the other end

## `libs/ui/KrimbleHelpDialog.h`

- line 1: /*
  * Krimble 2026-10-25: the window of Help > Krimble Help. George: "Do we have a help file? We should." It shows the handbook (krita/data/aboutdata/krimble-help.md,
  * built by tools/help/make_help.py, compiled into the app by aboutdata.qrc) with a "Go to" list of its parts and a Find box. Qt widgets only (tools/help-bench).
  */
- line 17: // markdown: the text of the handbook (an empty text shows a short message)
- line 20: // the handbook from the app's resources (":/krimble-help.md")

## `libs/ui/KrimbleLogDialog.cpp`

- line 1: /*
  * Krimble 2026-10-25: see KrimbleLogDialog.h.
  */
- line 23: // a session starts at "SESSION:"; the first piece is whatever comes before the first one
- line 37: // "Last exit:" says why the run BEFORE this one ended (it is written near the top of this session)
- line 71: // the end of the log

## `libs/ui/KrimbleLogDialog.h`

- line 1: /*
  * Krimble 2026-10-25: Help > Show Krimble Log... George: "Krimble relaunches constantly on switching ... I want it to stop crashing." Krimble already writes a log (krita.log) that says, at the start of
  * every run, why the run before it ended ("Last exit: reason ..., importance ..., signal ...") and marks a run that did not end correctly ("KRITA DID NOT CLOSE CORRECTLY"), but nothing in the app
  * showed it. This window shows a summary of the last sessions (newest first), then the end of the log. Qt widgets only (tools/log-bench).
  */
- line 16: // logPath: the file krita.log. extraText: what the Android side knows (why the window was last destroyed); may be empty.
- line 19: // the summary of the sessions, newest first (also used by the bench)

## `libs/ui/KrimbleVisualStyle.h`

- line 2: // Krimble Android visual refresh. Kept separate from layout and touch handling.
- line 3: // Krimble 2026-11-06: George: "Do the gradient effects on the whole app." The ChatGPT sheet (kept as KrimbleVisualStyle_chatgpt_74828b7.h.txt) with every color and every
- line 4: // gradient replaced by values SAMPLED from George's mockup pictures (the Picker, Mixer and QuickAdjust mockups), and the same effects added to the main window's menu bar,
- line 5: // menus, tool bars, headers, progress bars and tool tips (the ChatGPT sheet only styled dialogs). The Claude sheet from before the revert is kept as
- line 6: // KrimbleVisualStyle_before_revert_2026-11-06.h.txt.
- line 7: // Krimble 2026-11-06 (4th): George: "blue selections should go to a deeper blue." Every selected-state gradient (tabs, checked buttons, list rows, menu items, check boxes)
- line 8: // was #0585CE, #016CAC, #015C96, #01548C, #0165A5 (sampled from the Mixer mockup's selected tab); it is now #0B87DD, #0867B8, #0652AD, #05429D (the bottom is the
- line 9: // mockup's OK button blue).

## `libs/ui/KrimbleVisualStylePainted.h`

- line 2: // Krimble Android visual refresh. Kept separate from layout and touch handling.
- line 3: // Krimble 2026-11-11: THIS IS THE PAINTED (GRADIENT) SHEET, the one the app used before the flat theme (KrimbleVisualStyle.h). George: "Can we put in the Krimble gradient theme as an option?"
- line 4: // It is now applied only while the theme "Krimble Painted" (krita/data/themes/KrimblePainted.colors) is chosen (thememanager.cpp); the function was krimbleVisualStyleSheet(), now
- line 5: // krimblePaintedStyleSheet(). The rest of this file is the sheet exactly as it was on 2026-11-10.
- line 6: // Krimble 2026-11-06: George: "Do the gradient effects on the whole app." The ChatGPT sheet (kept as KrimbleVisualStyle_chatgpt_74828b7.h.txt) with every color and every
- line 7: // gradient replaced by values SAMPLED from George's mockup pictures (the Picker, Mixer and QuickAdjust mockups), and the same effects added to the main window's menu bar,
- line 8: // menus, tool bars, headers, progress bars and tool tips (the ChatGPT sheet only styled dialogs). The Claude sheet from before the revert is kept as
- line 9: // KrimbleVisualStyle_before_revert_2026-11-06.h.txt.
- line 10: // Krimble 2026-11-06 (4th): George: "blue selections should go to a deeper blue." Every selected-state gradient (tabs, checked buttons, list rows, menu items, check boxes)
- line 11: // was #0585CE, #016CAC, #015C96, #01548C, #0165A5 (sampled from the Mixer mockup's selected tab); it is now #0B87DD, #0867B8, #0652AD, #05429D (the bottom is the
- line 12: // mockup's OK button blue).

## `libs/ui/imagesize/imagesize.cc`

- line 39: /**
  * Krimble: makes "Image Size" a genuinely replayable Actions-panel step.
  * WdgImageSize::getConfiguration() already captures everything needed;
  * this just reads it back and calls the same KisImageManager method the
  * menu action always called directly.
  */
- line 65: /**
  * Krimble: unlike WdgImageSize, DlgCanvasSize is already a full KoDialog
  * itself (anchor-icon grid, offset sync, aspect lock all live inside it),
  * not a plain content widget meant to be embedded in someone else's
  * dialog shell -- so rather than refactor that substantial existing logic
  * to fit KisOperationUIWidgetFactory<T>'s template shape, this small
  * custom factory just constructs/execs it exactly as
  * ImageSize::slotCanvasSize() always did, then reads its existing
  * accessors into the operation config. None of DlgCanvasSize's own code
  * is touched.
  */

## `libs/ui/imagesize/wdg_imagesize.h`

- line 37: /**
  * Krimble: constructor matching KisOperationUIWidgetFactory<T>'s
  * expected (parent, view, config) signature, so this same widget --
  * unchanged validation/unit/aspect-ratio logic -- can be registered as
  * a real KisOperation UI, making "Image Size" a genuinely replayable
  * Actions-panel step instead of a bare menu-item trigger. Delegates to
  * the constructor above, reading the current image's own dimensions
  * exactly as ImageSize::slotImageSize() used to do by hand.
  */

## `libs/ui/kis_action.h`

- line 99: /**
  * Krimble: the operation id set by setOperationID(), or an empty
  * string for a plain action. Lets the Actions docker tell whether an
  * action merely dispatches a KisOperation -- in which case the real,
  * parameterized step arrives separately via
  * KisActionManager::sigOperationRun and this trigger should not also
  * be recorded as a duplicate, non-replayable step.
  */

## `libs/ui/kis_action_manager.h`

- line 89: /**
  * Krimble: fired whenever a KisOperation is actually run, with its
  * full KisOperationConfiguration -- the operation equivalent of
  * KisFilterManager::sigFilterApplied. Operations that take parameters
  * (Grow/Shrink/Border/Feather/Smooth Selection) prompt for them via a
  * KisOperationUIFactory *before* reaching here, so by this point the
  * config carries the user's real chosen values, making the step
  * genuinely replayable rather than just "this menu item was clicked".
  * Used by the Actions docker to record parameterized operation steps.
  */

## `libs/ui/kis_filter_manager.h`

- line 52: /**
  * Krimble: fired every time a filter is actually applied, with its
  * full, exact configuration -- unlike QActionCollection::actionTriggered
  * (which only tells you a menu item was clicked, before any dialog the
  * user might fill in even opens), this fires with the real, final
  * parameters the user chose. Used by the Task Set docker to record
  * genuinely replayable filter/adjustment steps instead of just
  * "this action was triggered."
  */

## `libs/ui/kis_layer_manager.h`

- line 65: /**
  * Krimble addition: the industry standard-parity "Merge Visible" (Ctrl+Shift+E).
  * Not a separate merge implementation -- just triggers the same two
  * steps described in the Krita manual: select all visible layers,
  * then merge with layer below (mergeLayer() already handles merging
  * a multi-selection via KisImage::mergeMultipleLayers()).
  */

## `libs/ui/kis_statusbar.cc`

- line 158: // George: hidden by default

## `libs/ui/krimble/KrimbleBevelsAndTabs.inc`

- line 56: // the toolbox: drawn like the raised buttons, 7 real pixels on every side
- line 78: // p.setPen(Qt::NoPen);
- line 79: // p.fillRect(QRectF(0, 0, width(), edge), Qt::black);
- line 80: // p.fillRect(QRectF(0, height() - edge, width(), edge), Qt::black);
- line 99: // Krimble 2026-11-23: the menu bar has no bevel edges, in every theme; only its K is drawn (George: "The menu bar doesn't need edges.")
- line 118: // was QColor(150, 160, 208)
- line 121: // const QColor bottomTone = recessed ? QColor(214, 222, 250) : (krimbleToolBoxTarget ? QColor(12, 13, 24) : QColor(32, 34, 56));
- line 126: // the toolbox's right and bottom shadow: one more line
- line 128: // const bool allEdges = !dock || dock->isFloating();
- line 129: // 674th+: the docked toolbox has all four edges too
- line 159: // p.setBrush(rightTone);
- line 223: // still on the row (or near it)
- line 267: // the tab bars of the panel groups are direct children of the main window (the image tabs are not)
- line 315: // the panel follows the finger
- line 324: // half a tab row beyond the row, up or down
- line 335: // the title may carry a mnemonic (&) or be shortened: try the dock whose title contains the tab's text without them
- line 369: // The tab row, "Tab C": rounded top corners; the selected tab has the color of the panel under it, no line below it, and curves flaring out to the line under the row (inverted
- line 370: // curves); the other tabs are darker and closed off by that line. The colors come from the theme's palette (the panel is the Window color).
- line 378: // the spaces between the tabs: clearly darker than the tabs
- line 382: // r: top corner radius (larger = rounder)
- line 386: // the line under the row first (the top of the panel below); the selected tab's body is drawn over it with a gap
- line 387: // the other tabs first, the selected tab last
- line 388: // the line under all the tabs (drawn after the darker tab bodies, which would hide it); the selected tab then covers its part
- line 422: // no line under the selected tab: cover it with the panel's color (the gap)
- line 450: // the tab bars of the panel groups are direct children of the main window (the image tabs live inside the MDI area)
- line 501: // the panels and toolbars are drawn above it
- line 530: // the comments of the sheet hold old copies of the rule
- line 533: // the last rule wins
- line 578: // no rule found in the sheet: the palette's window color
- line 580: // the dock draws its own content over this

## `libs/ui/krimble/KrimbleDialogs.inc`

- line 27: // const qreal off = 8 + i * 7;
- line 68: // anything unusual: leave this grid as it is
- line 74: // rows that are exactly a label in the first column and a control (not another label) in the second
- line 117: // 24: the window's margins and a scroll bar
- line 210: // a vertical spacer
- line 255: // QLayout::totalMinimumSize() and totalHeightForWidth() already include the widget's own contents margins (measured in real Qt: a group box's 168 px), so none are added here
- line 258: // only a widget that really is smaller than that is raised: a widget that has its room is left alone
- line 351: // Keep a strip free at the top for a title bar that may be drawn outside the dialog's area.
- line 459: // in the style sheets' and layouts' own units
- line 501: // Watch the dialog's window for touches: a window sees ALL fingers, while a
- line 502: // widget only sees them if it accepts the first touch (which would stop one-finger
- line 503: // taps from reaching buttons).
- line 538: // QDialog *dialog = qobject_cast<QDialog*>(qvariant_cast<QObject*>(window->property("krimbleDialog")));
- line 557: // the dialog's contents do not react to the two-finger move
- line 562: // QTimer::singleShot(0, dialog, [moved]() { keepOnScreen(moved); });   (bounced a dialog back on screen: George, no)

## `libs/ui/krimble/KrimbleDockEdges.inc`

- line 19: // logical px: about 12 mm on the screen
- line 84: // setAttribute(Qt::WA_NoSystemBackground);          (the invisible versions)
- line 108: // the area covered by all docked panels on this side
- line 125: // else setGeometry(column.left() - sepWidth - 4, column.top(), sepWidth + 4 + 16, column.height());
- line 126: // Krimble 2026-11-23: the right panel's grab area a little wider, 4 more units on each side (George: "Just extend it a little.")
- line 144: // logical px the tab sticks out from the panel's edge
- line 145: // logical px wide touch zone (the tab plus room for a finger)
- line 146: // 70 -> 140 (2026-10-26)
- line 159: // how far down the angled end runs
- line 160: // the panel's edge
- line 161: // the tab's outer edge
- line 185: // QColor fill = palette().color(QPalette::Mid);   (the canvas's gray: it could not be seen)
- line 186: // fill.setAlpha(215);
- line 194: // how far the curve sticks out; the rest of the zone is only a bigger place to put a finger
- line 196: // the panel is on the left: the curve bulges to the right
- line 199: // the panels are on the right: the curve bulges to the left

## `libs/ui/krimble/KrimbleDockHelpers.inc`

- line 10: // Looked up by this name (see the topLevelChanged handler). This class has no
- line 11: // Q_OBJECT, so findChild<KisFloatingDockSizeGrip*>() would match any widget.
- line 45: // was i * 6 (26 px handle) and i * 8 (36 px handle)
- line 55: // Keeps the resize handle in the lower right corner of its floating panel.
- line 80: // Remembers the size a panel had while it was docked, so that it can be given
- line 81: // back when the panel is detached (used for the toolbox).
- line 115: // Set while this runs, so the "docked again" handler in createDockWidget() does not
- line 116: // start a second placement.
- line 119: // Left or right: the side of the main window the panel is nearest to. (Panels are
- line 120: // not put above or below the canvas.)
- line 128: // Panels already docked in that area, in their order along it.
- line 139: // The panel after which the moved panel goes: the last one whose middle lies before it.
- line 148: // A floating panel is only allowed in no area; allow all before docking it.
- line 154: // Goes before every panel already there: put the others after it, in order.

## `libs/ui/krimble/KrimbleFileAndHelpExtras.inc`

- line 42: // "Where is...?": every menu entry with its path, searchable; tapping one opens it (Claude's idea).
- line 46: // some menus (Window, recent files) fill themselves when they are about to be shown
- line 119: // familiar name, a word that is in the menu entry's name
- line 169: // "Share": the picture as it looks flattened, through Android's share sheet (Claude's idea; George: "Share out is great.").
- line 173: // KisImageSP image = win->viewManager() ? win->viewManager()->image() : KisImageSP();   (did not compile: weak and shared pointer in one ?:)
- line 258: // Qt's PNG quality: 0 is the smallest file, 100 the largest; the slider's 9 is the smallest file
- line 358: // "Batch", or the folder a script names
- line 410: // QObject::connect(start, &QPushButton::clicked, dialog, [win, dialog, combo, start, bar, status]() {   (the first version, without Cancel)
- line 430: // the window was closed (Android's back button) while a picture was processed
- line 434: // if (preset.width > 0 && image.width() > preset.width) {
- line 435: //     image = image.scaledToWidth(preset.width, Qt::SmoothTransformation);
- line 436: // }
- line 437: // preset.width is the longest side
- line 493: // -1 is RESULT_OK
- line 524: // (the receiver is not deleted here: a few bytes per photo are left; the camera app has finished with it)
- line 544: // read and write access to that address
- line 547: // no camera app
- line 555: // "Upscale...": makes the picture 2, 3 or 4 times larger with the resize method of your choice (Claude's idea; George: "Yes. Go for it.").
- line 556: // Lanczos3 is the default; the other methods are the ones of Image > Image Size. Optionally a light sharpen layer is put on top
- line 557: // (a layer, so it can be hidden or removed).
- line 597: // (the lambda is "mutable": it keeps its own copy of the image pointer and calls waitForDone() on it, which a const copy does not allow)
- line 634: // Adds "Share as JPG" and "Share as PNG" to the File menu, just above Quit (Claude's idea).
- line 675: // File > A. I. Image generation: opens George's Perchance image generator (Image Donkey) in the browser; the address is a setting
- line 676: // (krimblerc, [Krimble] ImageDonkeyUrl) so it can change ("Actually, file has much fewer items. Put it in File as \"A. I. Image generation\".")
- line 677: // QAction *imageDonkey = new QAction(i18n("A. I. Image generation"), file);
- line 678: // "Maybe shorten it to gen instead of generation since that will stretch out the menu."
- line 698: // "Send Feedback": opens a prefilled GitHub issue (Claude's idea).
- line 716: // The QA overlay: live numbers on the screen, for screenshots (Claude's idea).
- line 793: // Adds the three entries to the end of the Help menu each time it opens (once).

## `libs/ui/krimble/KrimbleGlowAndDocking.inc`

- line 6: // it has an effect already (this one, or another that is not ours)
- line 8: // the selection blue (#0b87dd)
- line 17: /*
  class KrimbleGlowFilter : public QObject
  {
  public:
  explicit KrimbleGlowFilter(QObject *parent) : QObject(parent) {}
  void hook(QAbstractButton *button)
  {
  if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
  if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;   // not check boxes or radio buttons
  if (QByteArray(button->metaObject()->className()) == "KoToolBoxButton") return;   // the toolbox selection is flat, no glow
  if (KoGroupButton *grouped = qobject_cast<KoGroupButton*>(button)) {
  if (grouped->groupPosition() != KoGroupButton::NoGroup) return;   // it paints the Aqua-style selection itself
  }
  button->setProperty("krimbleGlowHooked", true);
  QObject::connect(button, &QAbstractButton::toggled, button, [button](bool on) { krimbleSetButtonGlow(button, on); });
  krimbleSetButtonGlow(button, button->isChecked());
  }
  bool eventFilter(QObject *watched, QEvent *event) override
  {
  if (event->type() == QEvent::Polish) {
  if (QAbstractButton *button = qobject_cast<QAbstractButton*>(watched)) hook(button);
  }
  return false;
  }
  };
  
  */
- line 44: // a checked button draws the Aqua-style selection (KrimbleAquaPaint.h) under its icon and text, in place of the halo glow
- line 47: // if (!button || !button->isCheckable() || !button->isChecked()) return false;
- line 49: // one Aqua: every plain push button (OK, Cancel, Create, Copy...) is painted like the checked tool button
- line 55: // keeps its own look (the menu arrow part)
- line 65: // krimblePaintAqua(p, r, qMin(r.width(), r.height()) * 0.10);
- line 66: // krimblePaintAqua(p, r, qMin(r.width(), r.height()) * 0.10, button->isDown() ? QColor(11, 135, 221).darker(135) : QColor(11, 135, 221));
- line 67: // a little darker while pressed
- line 94: // Krimble 2026-11-23: paints the Aqua on the buttons it is installed on. One is installed on each hooked button, so the work is per button,
- line 95: // not a look at every paint of every widget in the app (George: "Change this back to once per widget.").
- line 106: // the button's own checked background is not drawn
- line 114: // Krimble 2026-11-23: about one line of room under a menu's last item, bottom only (the menu's own bottom margin)
- line 128: // if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
- line 130: // plain push buttons get the Aqua too
- line 132: // only Qt's own push button: subclasses (color buttons and the like) and buttons with their own style sheet keep their look
- line 135: // not check boxes or radio buttons
- line 136: // the toolbox selection is flat, no glow
- line 138: // it paints the Aqua-style selection itself
- line 141: // QObject::connect(button, &QAbstractButton::toggled, button, [button](bool on) { krimbleSetButtonGlow(button, on); });
- line 142: // krimbleSetButtonGlow(button, button->isChecked());
- line 143: // a halo from before is taken off
- line 145: // this button's paints only
- line 152: // 868th, 875th: column lines and the cast shadow on every menu (KrimbleMenuColumnDividers.h)
- line 157: // Krimble 2026-11-23: room under the last item, bottom only (George: "The menus are chopped off short at the bottom
- line 158: // again."). Qt ignores padding-bottom on menus (tested in desktop Qt 5.15); the menu's own bottom margin works.
- line 159: // menu->setContentsMargins(0, 0, 0, 16);
- line 164: // Krimble 2026-11-23: a menu set up before this hook was listening gets its bottom room the first time it opens
- line 165: // (George, build 78: "ALL MENU BOTTOMS ARE STILL SHORT.")
- line 174: // once per widget: Polish comes once, when a widget is first shown; buttons made before the filter was installed are hooked by
- line 175: // krimbleInstallSelectionGlow. From the 840th to the 860th every paint of every widget in the app was looked at here.
- line 184: // Krimble 2026-11-23: menus that already exist (the menu bar's menus are set up before this hook) get their bottom room now
- line 199: // View > Show > Hex (a check mark) and Hex Size...; Layer > New > Hex Pattern...
- line 247: // task 41
- line 253: // George: "Hex overlay should be pinch zoom then drop with a tap."
- line 346: // the K is drawn below, pinned to the corner
- line 353: // empty columns at the left of the K picture, in real pixels
- line 361: // the visible K starts 17 real pixels from the window's left edge
- line 381: // The image tabs: Qt's K is replaced by an empty picture of the same size (so the names stay where they are) and the K is
- line 382: // drawn after the tab bar, its visible left edge 17 real pixels from each tab's left edge, the same as the image window title bar.
- line 411: // empty columns at the left of the K picture, in real pixels
- line 489: // not deliberate (yet): no side of the window (panels to tab with are added below)
- line 529: // const Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
- line 530: //                               : (nearest == toRight) ? Qt::RightDockWidgetArea
- line 531: //                               : (nearest == toTop) ? Qt::TopDockWidgetArea
- line 532: //                               : Qt::BottomDockWidgetArea;
- line 533: // window->addDockWidget(area, dock);
- line 560: // the docked panels, found once when the drag starts
- line 562: // since when

## `libs/ui/krimble/KrimbleHexGrid.inc`

- line 49: // center to corner, in image pixels
- line 52: // the color with its opacity (the alpha)
- line 53: // George: "Hex overlay should be pinch zoom then drop with a tap." While true, a pinch scales the hexes and a tap drops them.
- line 61: // too small to be of any use: the same rule as the grid
- line 65: // QPen pen(QColor(30, 30, 30, 210), 1.0);   // before task 41: always 1 pixel, always this color
- line 76: // a note while the pattern is being scaled
- line 96: // the small window that asks for the size, the orientation and (for the layer) the line thickness and color
- line 221: // task 41
- line 231: // task 41
- line 252: // George: "Hex overlay should be pinch zoom then drop with a tap." While the overlay is being adjusted, two fingers scale the hexes (the distance between
- line 253: // them against its last value), a short single tap that does not move drops them (the size is kept; the pattern then follows the canvas as usual), and
- line 254: // other touches on the canvas are held back so that the canvas does not zoom or paint at the same time.
- line 263: // George: "Yes" to: when the overlay is dropped with a tap, ask "Make this a layer?" (a transparent vector layer with the hexes at that size)
- line 343: // puts the hex overlay on the picture that is open now, if the hexes are on
- line 380: // a vector shape lives in points: image pixels per point is xRes()
- line 400: // percent of the outer circle
- line 401: // outer circle diameter in pixels; 0 = half of the picture's shorter side
- line 454: // a vector shape lives in points: image pixels per point is xRes() (as in the Hex Pattern layer)
- line 465: // the foreground color is the fill
- line 472: // no outline
- line 477: // so that it can be moved, rotated and its circles changed at once

## `libs/ui/krimble/KrimbleMenuBarAndQuickAdjust.inc`

- line 14: // Brightness: a sun
- line 23: // Contrast: a circle, the left half filled
- line 28: // Saturation: three overlapping drops of color
- line 35: // Hue: a ring of all the colors
- line 42: // Lightness: a square, the left half light
- line 48: // Vibrance: a four-point sparkle
- line 62: // the Preview switch of the drawing: a pill that says ON or OFF, with a round knob, and a blue glow when it is on
- line 142: // glow behind the letters
- line 148: // the letters
- line 161: // the slider line under the word, with its knob
- line 192: // the height follows the width, so the logo keeps its shape whatever layout it sits in
- line 255: // Qt lays the menus out as if the button were its own 16 px (it ignores a fixed size), so with a 28 px button "File" would sit under the K. Tested in real Qt: a small
- line 256: // empty item in front of the first menu pushes all the menus to the right (File starts 13 px later). If the menu bar is rebuilt and the item is lost, it is put back.
- line 265: // gap->setFixedSize(10, 28);
- line 266: // 4 px: the 22 px button leaves room for Qt's own 16 px reservation plus this small gap
- line 286: // {
- line 287: //     const QFontMetrics holderMetrics(bar->font());
- line 288: //     const int holderWidth = 2 + qMin(bar->height() - 4, qMax(14, holderMetrics.height())) + 8;
- line 289: //     QWidget *corner = bar->cornerWidget(Qt::TopLeftCorner);
- line 290: //     if (!corner) {
- line 291: //         corner = new KrimbleKHolder(bar);
- line 292: //         bar->setCornerWidget(corner, Qt::TopLeftCorner);
- line 293: //     }
- line 294: //     if (corner->objectName() == QLatin1String("krimbleKHolder")) {
- line 295: //         if (corner->width() != holderWidth || corner->minimumWidth() != holderWidth) corner->setFixedWidth(holderWidth);
- line 296: //         // shown unless Qt has put it aside for an image window's own button (then it is not the bar's corner widget and this code does not see it)
- line 297: //         if (!corner->isVisible() && bar->isVisible()) corner->show();
- line 298: //     }
- line 299: // }
- line 300: // {
- line 301: //     const QFontMetrics krimbleKMetrics(bar->font());
- line 302: //     const int krimbleKRoom = 2 + qMin(bar->height() - 4, qMax(14, krimbleKMetrics.height())) + 8;
- line 303: //     QAction *krimbleFirst = nullptr;
- line 304: //     Q_FOREACH (QAction *a, bar->actions()) {
- line 305: //         if (a->isVisible() && a->objectName() != QLatin1String("krimbleKSpacer") && !qobject_cast<QWidgetAction*>(a)) { krimbleFirst = a; break; }
- line 306: //     }
- line 307: //     if (krimbleFirst && bar->isVisible()) {
- line 308: //         const int have = bar->actionGeometry(krimbleFirst).left();
- line 309: //         const int cur = bar->property("krimblePadLeft").toInt();
- line 310: //         const int want = qBound(0, cur + krimbleKRoom - have, krimbleKRoom);
- line 311: //         if (have > 0 && want != cur) {
- line 312: //             bar->setProperty("krimblePadLeft", want);
- line 313: //             bar->setStyleSheet(QStringLiteral("QMenuBar { padding-left: %1px; }").arg(want));
- line 314: //         }
- line 315: //     }
- line 316: // }
- line 323: // QWidget *label = bar->cornerWidget(Qt::TopLeftCorner);
- line 324: // const bool krimbleHaveLabel = label && QByteArray(label->metaObject()->className()).endsWith("ControlLabel") && label->isVisible();
- line 325: // const QPoint target(1, krimbleHaveLabel ? qMax(0, (bar->height() - krimbleBarExtra - label->height()) / 2) : 0);
- line 326: // if (krimbleHaveLabel && label->pos() != target) label->move(target);
- line 327: // QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
- line 328: // QAction *first = nullptr;
- line 329: // Q_FOREACH (QAction *entry, bar->actions()) {
- line 330: //     if (entry == spacer || !entry->isVisible() || !entry->menu()) continue;
- line 331: //     first = entry;
- line 332: //     break;
- line 333: // }
- line 334: // if (!first) return;
- line 335: // const int krimbleKSize = qMin(bar->height() - 4, qMax(14, QFontMetrics(bar->font()).height()));
- line 336: // const int deficit = (2 + krimbleKSize + 8) - bar->actionGeometry(first).x();
- line 337: // if (deficit <= 0) return;
- line 338: // if (!spacer) {
- line 339: //     ensureSpacer(bar);
- line 340: //     spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
- line 341: // }
- line 342: // QWidget *gap = spacer ? spacer->defaultWidget() : nullptr;
- line 343: // if (!gap || gap->width() >= 90) return;
- line 344: // gap->setFixedSize(qMin(90, gap->width() + deficit), gap->height());
- line 345: // spacer->setData(gap->width());
- line 346: // bar->update();
- line 348: // a menu bar does not lay out a widget item, so the empty item never moved File
- line 365: // const int textStart = 2 + krimbleKSize + qRound(90.0 / qMax<qreal>(1.0, bar->devicePixelRatioF()));   // the K's right edge plus 90 real screen pixels
- line 366: // 70 real pixels: "File menu title could be a few pixels closer to the K."
- line 399: // 2026-11-11: the K in the corner, the menus clear of it
- line 402: // Qt moved the button: put it back in the corner
- line 405: // 2026-11-07: no Settings menu, also after the menu bar is rebuilt
- line 410: // the menu bar was rebuilt: put the spacer back in front
- line 412: // after the painting of the bar when the bar is painted (the Painted theme), or at once
- line 432: // Krimble 2026-11-23: no edge lines on the menu bar (George: "The menu bar doesn't need edges."):
- line 433: // painter.setPen(QColor(150, 165, 225, 120));
- line 434: // painter.drawLine(0, 0, bar->width(), 0);
- line 435: // painter.setPen(QColor(62, 74, 128, 200));
- line 436: // painter.drawLine(0, bar->height() - 2, bar->width(), bar->height() - 2);
- line 437: // painter.setPen(QColor(8, 11, 28, 255));
- line 438: // painter.drawLine(0, bar->height() - 1, bar->width(), bar->height() - 1);
- line 441: // the bar then draws its menu names on top as usual
- line 462: // layout->setSpacing(3);   // item 6 misread: George meant only the logo
- line 496: // the six pictures cut out of George's picture (krita/pics/svg/dark_krimble-quickadjust-icon-N.png), 38 logical px, sharp at the screen's resolution
- line 515: // "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 22px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
- line 516: // "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 22px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
- line 517: //
- line 522: // row->addWidget(m_numbers[i], 1, 1, Qt::AlignLeft | Qt::AlignTop);
- line 524: // layout->addLayout(row);
- line 534: // m_sliders[i]->setStyleSheet(QStringLiteral(
- line 535: //     "QSlider::groove:horizontal { height: 4px; border: 1px solid #535c80; border-radius: 3px; background: #20263f; }"
- line 536: //     "QSlider::sub-page:horizontal { height: 4px; border: 1px solid #146a9f; border-radius: 3px; background: #1d7cb2; }")
- line 537: //     + thinHandle);
- line 538: // m_sliders[i]->setStyleSheet(QStringLiteral(
- line 539: //     "QSlider::groove:horizontal { height: 2px; border: none; border-radius: 1px; background: #6f789c; }"
- line 540: //     "QSlider::sub-page:horizontal { height: 2px; border: none; border-radius: 1px; background: #2aa6e8; }")
- line 541: //     + thinHandle);
- line 576: // "QSlider::groove:horizontal { height: 9px; border: 1px solid #535c80; border-radius: 4px;"
- line 577: // "QSlider::groove:horizontal { height: 5px; border: 1px solid #535c80; border-radius: 4px;"
- line 697: // another picture: forget the layers of the old one
- line 759: // "Make QuickAdjust menu slightly smaller." (1.0.74 list, item 6): text 90%
- line 760: // item 6 misread (George, 2026-11-22: "I meant the logo, not the panel."): the text stays full size
- line 761: // if (smaller.pointSizeF() > 0) smaller.setPointSizeF(smaller.pointSizeF() * 0.9);
- line 762: // else if (smaller.pixelSize() > 0) smaller.setPixelSize(qMax(8, qRound(smaller.pixelSize() * 0.9)));
- line 783: // logoRow->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-banner2.png"), QStringLiteral("Krimble QuickAdjust"), dialog, qRound(520 / bannerDpr)), 1);
- line 784: // his large QuickAdjust logo ("use my larger logo instead of the tiny Krimble one"), left aligned
- line 785: // logoRow->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-logo.png"), QStringLiteral("QuickAdjust"), dialog, qRound(600 / bannerDpr)), 1);
- line 786: // George, 2026-11-22: "60% of current size." (600 real px wide -> 360)
- line 816: // The divider between Preview and Cancel is not used now that Preview has its own line.
- line 817: // QFrame *divider = new QFrame(dialog);
- line 818: // divider->setFixedSize(1, 36);
- line 819: // divider->setStyleSheet(QStringLiteral("background: #f0f1fa; border: none;"));
- line 820: // bottom->addWidget(divider);
- line 843: // closing the window another way (Android's back button) counts as Cancel
- line 848: // const int width = qMax(dialog->minimumSizeHint().width(), qMin(dialog->sizeHint().width(), int(avail.width() * 0.64)));   // item 6 misread
- line 858: // Opens (or shows and hides) the Quick Adjust panel. It starts floating, 62% of the screen wide, so it never has to fit next to the
- line 859: // toolbox, canvas and panels.

## `libs/ui/krimble/KrimbleMenuColumnDividers.h`

- line 1: /*
  * Krimble 2026-11-23: two additions to every menu (QMenu), installed by KrimbleGlowFilter when a menu is first polished.
  *
  * 868th: lines between the columns of a menu that is too tall for the screen and wraps into columns (the Panels menu).
  * George: "Need vertical dividers for menus that expand wide." / "Dark vertical lines." / "Add a hilight edge like the separator has.
  * Dark line and a light line." / "I can't see the dark line." Qt has no style sheet setting for this, so the lines are painted after the
  * menu's own paint. Only between columns: nothing at the top edge, where the menu meets its title
  * ("THE MENUS TOUCH THEIR TITLE AREAS WITH NO DIVIDING LINES IN BETWEEN EVER.").
  *
  * 876th: a barely visible light line down the left edge of every menu (only the left edge).
  *
  * 875th: a small soft cast shadow on the left, right and bottom of a menu, never on the top (the menu joins its title there).
  * George: "Can menus have a small shadow?" / "Can it be softer and a little bigger?" / "Add it. Keep it small."
  * The shadow is its own see-through window just behind the menu, so the menu itself (its size, its place, its submenus) is unchanged.
  */
- line 30: // a soft shadow on the left, right and bottom of 'menu'; nothing above its top edge.
- line 31: // Thin layers stacked from the outside in: darkest at the menu's edge, fading smoothly to nothing.
- line 47: // units around the menu (about 18 real px on the phone): "Keep it small."
- line 106: // the menu stays in front of its shadow
- line 115: // column left edge -> its right edge, and the span the items cover from top to bottom
- line 126: // if (columns.size() < 2) return false;   // one column: the menu is painted as always, nothing added
- line 127: // (876th: every menu is painted here now, for its faint left edge)
- line 130: // the menu's own paint first
- line 135: // 876th: a barely visible light line down the menu's left edge only, not around it.
- line 136: // George: "On the left edge of the menu add a barely perceptible light edge. Just barely and only the left edge. Not all around."
- line 138: // one unit, like the lines of the menu's horizontal separator
- line 141: // midway between the two columns
- line 142: // the dark line twice as wide as the light one: one unit of black did not show on the dark menu ("I can't see the dark line.")

## `libs/ui/krimble/KrimbleMenus.inc`

- line 51: // A submenu (Edit > Fill, Edit > Transform...) gets the same row height and the same room before and after the text
- line 52: // as the menu it opens from. Without this, Fill's submenu had no room at the left of its words and looked cramped.
- line 53: // The values are measured from the parent menu each time the submenu opens (the screen's density decides them).
- line 66: // on the phone the parent's words start 47 real px in, on 69 real px rows
- line 104: // still there from the last time: make sure it is the first one
- line 140: // George: "Minimum width for the toolbox should be one column." The toolbox is left out: its smallest width stays what its layout
- line 141: // gives (one column of icons).

## `libs/ui/krimble/KrimbleNumberFields.inc`

- line 10: // (no Q_OBJECT in this file, so a property marks the pad)
- line 88: // how long a finger must stay down before dragging changes the value
- line 89: // logical pixels of drag for one step
- line 90: // movement allowed during a tap or long press
- line 104: // Used by the number pad and the drag: changes the value the way each kind of field wants it.
- line 155: // moved before the long press: not a tap and not a drag, so nothing happens
- line 200: // enum State { Idle, Pending, Scrubbing, Cancelled };
- line 238: // very wide ranges move faster so that the whole range can be reached
- line 251: // dragging the width of the field changes the value by up to 100 (a percentage box moves over its whole 0 to 100; a bigger range moves 100 units; a small range such as 0 to 1 moves its whole range)

## `libs/ui/krimble/KrimblePanelsAndLayout.inc`

- line 20: // Sets the width of the right-hand panel column (clamped so the panels keep their smallest width
- line 21: // and the canvas keeps some room).
- line 142: // the darker line
- line 143: // the highlight beside it
- line 153: // Panels hidden: a slightly wider tab on the window's right edge with an arrow.
- line 190: // Hides (hide == true) or shows the right-hand panels. When hiding, the panels that were visible
- line 191: // are remembered so exactly those come back.
- line 206: // Nothing remembered (for example after a restart): show the usual three.
- line 221: // Keeps the side button's place and the menu text in step with what is on screen.
- line 261: // the same event, passed up to a parent
- line 360: // Drop Shadow
- line 366: // Inner Shadow
- line 369: // Outer Glow (white)
- line 373: // Inner Glow
- line 376: // Stroke, white, 4 px, outside
- line 382: // Stroke, black, 4 px, outside
- line 388: // Bevel and Emboss
- line 391: // Clear Style
- line 434: // Adds "Batch Process..." at the end of the Image menu, after a divider (once, when the menu first opens). It was at the end of the
- line 435: // File menu, below Exit (George: "I think it should be in the image menu, not below exit.").
- line 455: // moved to the File menu as "A. I. Image generation" ("Actually, file has much fewer items.")
- line 456: // // Image > Generate Image (Image Donkey)...: opens George's Perchance image generator in the browser; the address is a setting
- line 457: // // (krimblerc, [Krimble] ImageDonkeyUrl) so it can change ("Just use one or the other. It's just a url." / "Add it now and document it." / "Image menu")
- line 458: // QAction *imageDonkey = new QAction(i18n("Generate Image (Image Donkey)..."), imageMenu);
- line 459: // imageDonkey->setObjectName(QStringLiteral("krimbleImageDonkey"));
- line 460: // QObject::connect(imageDonkey, &QAction::triggered, win, []() {
- line 461: //     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
- line 462: //     QString address = group.readEntry("ImageDonkeyUrl", QString());
- line 463: //     if (address.trimmed().isEmpty()) {
- line 464: //         address = QStringLiteral("https://perchance.org/image-donkey");
- line 465: //         group.writeEntry("ImageDonkeyUrl", address);
- line 466: //         group.config()->sync();
- line 467: //     }
- line 468: //     QDesktopServices::openUrl(QUrl::fromUserInput(address.trimmed()));
- line 469: // });
- line 470: // imageMenu->insertAction(topFirst, imageDonkey);
- line 495: // if (text == QLatin1String("Image Rotation") && action->menu()) {
- line 496: // George is renaming it "Rotate"
- line 500: // QAction *freeRotate = new QAction(i18n("Free Rotate"), rotation);
- line 528: // Adds "Rotation Gadget" (a check mark) at the end of the View menu (once, when the menu first opens): turns the canvas rotation
- line 529: // dial in the status bar off and on (Claude's design; George: "I sometimes accidentally trigger the rotation gadget on the bottom.
- line 530: // Can we turn it off and on at will?").
- line 552: // (only the status bar's dial, not the angle selectors in the panels)
- line 560: // gadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_rotation_gadget", 1) != 0);   (shown by default)
- line 561: // George: hidden by default
- line 582: // Adds "Style Presets" right after "Layer Style..." in the Layer menu (once, when the menu first opens).
- line 618: // the screen itself, not the window: the window's size can change for a moment without the phone being turned
- line 630: // QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
- line 631: // Krimble 2026-11-22: closed panels came back (George: Layers "kept reopening... triggered by opening new image files").
- line 632: // This was the only code that reopens panels: it put the saved layout back whenever the window's shape flipped, and the
- line 633: // window's shape can flip for a moment when the app comes back from the file picker. Now only a real turn of the screen counts.
- line 636: // QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
- line 637: // Krimble 2026-11-22: each direction remembers its panels as they were last left (George: "Do them all in order as needed.").
- line 638: // The layout of the direction being left is kept; the new direction gets its own last layout of this run, or, the first
- line 639: // time it is turned to in this run, the saved startup layout.
- line 640: // Krimble 2026-11-23: turning the phone keeps the panels as they are (George: "You shouldn't have a completely different
- line 641: // layout with a rotate."). The per-direction layouts of the 857th are switched off:
- line 642: // {
- line 643: // KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
- line 644: // g.writeEntry(m_screenLandscape ? "portrait_last" : "landscape_last", win->saveState().toBase64());
- line 645: // g.sync();
- line 646: // }
- line 647: // const bool toLandscape = m_screenLandscape;
- line 648: // QTimer::singleShot(500, win, [win, toLandscape]() {
- line 649: // KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
- line 650: // const QByteArray last = g.readEntry(toLandscape ? "landscape_last" : "portrait_last", QByteArray());
- line 651: // if (!last.isEmpty()) win->restoreState(QByteArray::fromBase64(last));
- line 652: // else krimbleRestoreSavedLayout(win);
- line 653: // });
- line 680: // Watches the main window and keeps the side button and menu text up to date.
- line 697: // the canvas area changed size or was shown: its right edge moved

## `libs/ui/thememanager.cpp`

- line 203: // Krimble Light: the Krimble sheet made light. Saturated blues (the selection blue, Aqua) stay. Dark colors (backgrounds, edges, shadows)
- line 353: // if (!krimbleSheet.isEmpty()) krimbleSheet += krimbleCommonStyleSheet();
- line 359: // (KrimbleAquaPaint.h), drawn once into pictures, because a style sheet cannot draw it
- line 385: // Krimble 2026-11-23: 20% darker (George: "slightly darker", "Try 20%")
- line 388: // Krimble 2026-11-23: Krimble Light only: the selected panel tab is raised (white top and left edges), the unselected tabs are flat and darker,
- line 389: // and the selected tab casts a shadow on the tab beside it (George: "the selected tab looks pushed in", "Not black.",

## `libs/ui/tool/kis_tool_paint.cc`

- line 166: /* Krimble 2026-11-13 (task 77) */

## `libs/ui/toolbox/KoToolBox.cpp`

- line 154: // the Transform mode in use (the qApp property krimbleTransformMode, set by the Transform Tool Options)
- line 171: // (it ends with krimbleApplyGroups(): the tool groups)
- line 640: /*
  void KoToolBox::setupIconStyleMenu(QMenu *menu)
  {
  QActionGroup *styleGroup = new QActionGroup(menu);
  const bool color = KrimbleToolboxIcons::colorMode();
  const bool gray = KrimbleToolboxIcons::grayMode();
  QAction *colorAction = menu->addAction(i18n("Full color"));
  QAction *grayAction = menu->addAction(i18n("Black and white"));
  QAction *clearAction = menu->addAction(i18n("Silhouette"));
  for (QAction *action : {colorAction, grayAction, clearAction}) {
  action->setActionGroup(styleGroup);
  action->setCheckable(true);
  }
  colorAction->setChecked(color && !gray);
  grayAction->setChecked(gray);
  clearAction->setChecked(!color);
  auto refresh = [this]() {
  Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) {
  button->krimbleRefreshIcon();
  }
  };
  connect(colorAction, &QAction::triggered, this, [refresh]() { KrimbleToolboxIcons::setColorMode(true); refresh(); });
  connect(grayAction, &QAction::triggered, this, [refresh]() { KrimbleToolboxIcons::setGrayMode(); refresh(); });
  connect(clearAction, &QAction::triggered, this, [refresh]() { KrimbleToolboxIcons::setColorMode(false); refresh(); });
  }
  */
- line 675: // {i18n("Color"), KrimbleToolboxIcons::Color},   // removed: "Remove the color option completely"
- line 676: // {i18n("Classic"), KrimbleToolboxIcons::Classic},   // removed: "Lose the \"classic\" version. It's not great."
- line 677: // {i18n("Photorealistic"), KrimbleToolboxIcons::Photorealistic},   // held back: the pictures are not in the app ("We should hold back the photorealistic high resolution buttons.")

## `libs/ui/toolbox/KoToolBoxButton.cpp`

- line 99: // setIcon(KrimbleToolboxIcons::icon(m_toolAction->iconName()));
- line 201: // const bool krimbleLight = krimbleWindow.lightness() > 150;
- line 202: // const QColor face = krimbleLight ? krimbleWindow.darker(108) : krimbleWindow.lighter(150);
- line 244: // krimblePaintAqua(painter, QRectF(rect()).adjusted(s * 0.12, s * 0.12, -s * 0.12, -s * 0.12), s * 0.07);   // the Aqua-style selection
- line 245: // krimblePaintAqua(painter, QRectF(rect()), s * 0.18);   // the Aqua-style selection fills the whole tile, rounder corners
- line 246: // George 2026-11-22: "I probably went too far making it big. Maybe shave off a bit." 5% of the tile off each side
- line 247: // krimblePaintAqua(painter, QRectF(rect()).adjusted(s * 0.05, s * 0.05, -s * 0.05, -s * 0.05), s * 0.16);
- line 248: // George 2026-11-23 (869th): "Let's just do flat color on toolbox selections. Save the aqua for the buttons." / "Flat is more professional."
- line 266: //         painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelGlow, [alpha](qreal v) { return QColor(28, 128, 235, qMin(255, int(alpha * (0.25 + 0.9 * v)))); })), qMax<qreal>(1.0, bw * 0.012)));
- line 267: //         krimbleMiterPen(painter);   // task 37: a pen's corners are cut off (bevel join) unless it says miter; that is what looked rounded
- line 293: //         painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [alpha](qreal v) { return QColor(36, 150, 248, qMax(0, qMin(255, int(alpha * (0.8 + 0.9 * (v - 0.55)))))); })), qMax<qreal>(1.0, bw * 0.011)));
- line 294: //         krimbleMiterPen(painter);   // task 37
- line 298: //     painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) { return QColor(80, 200, 255, qMin(255, int(255 * v))); })), qMax<qreal>(3.0, bw * 0.022)));
- line 300: //     krimbleMiterPen(painter);   // task 37
- line 303: //     painter.setPen(QPen(QBrush(krimbleSelConical(centre, krimbleSelOutline, [](qreal v) {
- line 308: //     krimbleMiterPen(painter);   // task 37
- line 324: // Krimble 2026-11-23: in the light theme the selected tool's icon is white on the selection blue (George: "#0B87DD is a good

## `libs/ui/toolbox/KoToolBoxDocker.cpp`

- line 97: // Raised bevel strip, edge to edge, flush with the top of the toolbox, five lines inside (Claude 2026-11-22, George: "Apply it.").
- line 112: // Krimble 2026-11-23 (866th): the white top and left bevel lines are off. George: "The giant white line at the top and top left of the toolbar has to go."
- line 354: // Krimble: the two simple gadget icons, drawn in code in the toolbox icon color.
- line 379: // 2026-11-22 drafts approved (George: "The icons. Yes. Push."):
- line 436: // Krimble: the two gadgets under the color swatches, as in the industry-standard toolbox
- line 437: // (docs/krimble-refs/2026-11-21_0428_ps-toolbox-bottom-quickmask-screenmode.jpg): Quick Mask on the left,
- line 446: // quickMask->setIcon(krimbleGadgetIcon(true));
- line 450: // 870th: George, 2026-11-23: "too big. They get too close to the edges." / "Those two smaller icons are better."
- line 455: // screenMode->setIcon(krimbleGadgetIcon(false));
- line 459: // 870th: George, 2026-11-23: "too big. They get too close to the edges." / "Those two smaller icons are better."
- line 633: // const bool has = property("krimbleToolBoxEdges").toBool();
- line 638: //     setProperty("krimbleToolBoxEdges", true);
- line 643: //     setProperty("krimbleToolBoxEdges", false);

## `libs/ui/toolbox/KrimbleToolFlyout.h`

- line 2: // Krimble 2026-11-12 (task 38): George: "I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection."
- line 3: // The flyout of a tool group: a small popup beside the toolbox button that was long pressed, one big row per tool of the group (picture and name, the tool in use marked).
- line 4: // A tap on a row picks that tool (its own, hidden toolbox button is clicked, so the tool changes exactly as if it had been tapped), a tap anywhere else only closes the popup:
- line 5: // it is a Qt popup, which closes on a press outside it, and the press is not handed on to what is underneath (WA_NoMouseReplay), so a miss changes nothing.
- line 32: // "QToolButton { color: #ffffff; font-size: 15px; padding: 4px 14px 4px 8px; border: none; background: transparent; text-align: left; }"
- line 35: // "QToolButton:checked { background: #0b2f66; }"));
- line 37: // the row in use: the flat selection blue (a checked row was painted Aqua)
- line 43: // the tool tip may carry markup
- line 48: // QToolButton *row = new QToolButton(this);
- line 49: // row->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
- line 50: // row->setIcon(member->icon());
- line 51: // row->setIconSize(member->iconSize());
- line 52: // row->setText(name);
- line 53: // row->setCheckable(true);
- line 54: // row->setChecked(member == current);
- line 55: // // row->setMinimumHeight(qMax(44, member->iconSize().height() * 3 / 2 + 14));
- line 56: // row->setMinimumHeight(qMax(member->height(), member->iconSize().height() + 6));
- line 57: // row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
- line 58: // layout->addWidget(row);
- line 59: // // a tap: close the flyout, then click the member's own button (it may be hidden: click() works on a hidden button)
- line 60: // QObject::connect(row, &QToolButton::clicked, this, [this, member]() {
- line 61: //     close();
- line 62: //     member->click();
- line 63: // });
- line 64: // a tap: close the flyout, then click the member's own button (it may be hidden: click() works on a hidden button)
- line 69: // one row: picture and name; the row in use is marked in the flat selection blue (not a checked button, which is painted Aqua)
- line 87: // beside the button: to the right of it, or to the left when there is no room, kept inside the screen

## `libs/ui/toolbox/KrimbleToolboxIcons.h`

- line 2: // Krimble 2026-10-30: the toolbox icons come in two styles, and the user switches between them (George: "I want the user to be able to switch between the full
- line 3: // color themed toolbox icons and your clear icons you made for the toolbox."): **Full color**, the picture icons drawn by George (32 PNGs in the resource
- line 4: // prefix :/krimble-toolbox/, one per tool), and **Silhouette**, the line icons made for Krimble (what the toolbox showed before; George calls them silhouette icons). The choice is the key
- line 5: // Krimble/ToolboxColorIcons (full color is the default). A tool with no picture icon (any tool not in the table below) always shows its own icon. (Dodge, Burn, Colorize Mask Editing and Enclose and Fill
- line 6: // are not in the toolbox at all, see the 357th entry.) The table maps a tool's icon name (what its factory gives to setIconName) to the picture file.
- line 51: // { QStringLiteral("shape_handling"),             QStringLiteral("04_04_shape_select") },   // (the 358th entry put it on Edit Shapes; George: "I think you should have left it.")
- line 69: // // A third style, Black and white: the silhouette icons drawn in black. The key Krimble/ToolboxGrayIcons is on for it
- line 70: // // (ToolboxColorIcons is on with it; grayMode() is checked first).
- line 71: // inline bool colorMode()
- line 72: // {
- line 73: //     return KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxColorIcons", false);
- line 74: // }
- line 76: // inline bool grayMode()
- line 77: // {
- line 78: //     return colorMode() && KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxGrayIcons", false);
- line 79: // }
- line 81: // inline void setColorMode(bool on)
- line 82: // {
- line 83: //     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
- line 84: //     group.writeEntry("ToolboxColorIcons", on);
- line 85: //     group.writeEntry("ToolboxGrayIcons", false);
- line 86: //     group.config()->sync();
- line 87: // }
- line 89: // inline void setGrayMode()
- line 90: // {
- line 91: //     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
- line 92: //     group.writeEntry("ToolboxColorIcons", true);
- line 93: //     group.writeEntry("ToolboxGrayIcons", true);
- line 94: //     group.config()->sync();
- line 95: // }
- line 97: // Icon styles: White (the silhouette icons as they are), Black (the silhouettes drawn in black), Grayscale (the picture icons
- line 98: // in shades of gray), Color (the picture icons). Key Krimble/ToolboxIconStyle; an older setting (ToolboxColorIcons / ToolboxGrayIcons) is read once.
- line 99: // enum IconStyle { White = 0, Color = 1, Black = 2, Grayscale = 3, Classic = 4 };
- line 100: // Photorealistic: George's photo pictures (krita/pics/krimble-toolbox-photo, full size in docs/krimble-icon-sets/photorealistic);
- line 101: // a tool without one is drawn as in Classic.
- line 104: // Classic: as in the classic industry-standard toolbar (docs/krimble-refs/2026-11-21_0436_ps-toolbar-labeled-grayscale-icons.jpg),
- line 105: // tools that are shapes and lines are black line art (the silhouettes in black); tools that are objects are shaded gray art
- line 106: // (the picture icons in grayscale).
- line 131: // the Color style was removed ("Remove the color option completely"): a saved Color choice shows White
- line 132: // inline int iconStyle() { return iconStyleCache() == Color ? int(White) : iconStyleCache(); }
- line 133: // Color and Classic were removed ("Remove the color option completely"; "Lose the \"classic\" version. It's not great."): Color shows as White, Classic as Grayscale
- line 138: // Photorealistic is held back (its pictures are not in the app): anyone who picked it sees White
- line 152: // the picture icons are shown in Color and Grayscale
- line 153: // inline bool colorMode() { return iconStyle() == Color || iconStyle() == Grayscale || iconStyle() == Classic; }
- line 155: // (name kept: the Black style)
- line 159: // recolors another icon at whatever size is asked for: 0 = black silhouette, 1 = shades of gray
- line 160: // (fixed-size copies can come out tiny on the phone's high-density screen)
- line 201: // a picture in shades of gray (the same light and dark, the transparency kept)
- line 204: // the earlier version, fixed-size copies
- line 244: // The Light gray switch: the toolbox buttons get a light gray face (flat or raised), like the classic light gray toolbox. Key Krimble/ToolboxLightGray.
- line 245: /*
  inline bool &lightGrayCache()
  {
  static bool on = KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxLightGray", false);
  return on;
  }
  
  inline bool lightGray()
  {
  return lightGrayCache();
  }
  
  inline void setLightGray(bool on)
  {
  lightGrayCache() = on;
  KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
  group.writeEntry("ToolboxLightGray", on);
  group.config()->sync();
  }
  
  inline QColor lightGrayFace()
  {
  return QColor(198, 198, 202);
  }
  */
- line 270: // Button background: 0 = the panel's own (default), 1 = light gray, 2 = white, 3 = dark gray, 4 = glow (a dark tile with a teal glow,
- line 271: // docs/krimble-refs/2026-11-21_1245_glow-background-sample.png: "Let's add that as a standard option, the pretty background."). Key Krimble/ToolboxButtonFace (the older ToolboxLightGray is read once).
- line 282: // inline int buttonFace() { return buttonFaceCache(); }
- line 283: // Glow (4) is dropped ("Glow background is dropped."): anyone who had picked it sees the panel background
- line 294: // inline bool lightGray() { return buttonFace() != 0; }   // a light background (light gray or white)
- line 295: // a background of its own (light gray, white or dark gray)
- line 296: // a light background (light gray or white)
- line 301: // return buttonFace() == 2 ? QColor(250, 250, 252) : QColor(198, 198, 202);
- line 303: // the glow tile's average, for anything that needs one color
- line 309: // the Glow background: a dark navy tile, lit from the middle with teal, rounded a little
- line 328: // the photorealistic picture for a tool (or a toolbox button such as "08_02_quick_mask"), or a null icon
- line 348: // the icon for a tool: the picture icon in the Full color style (when the tool has one), the program's own icon otherwise
- line 349: // the silhouette icon drawn in black (its shape and transparency kept): the Black and white style
- line 352: // the earlier version, fixed-size copies
- line 369: // draws another icon larger in its square, at whatever size is asked for (fixed-size copies came out tiny on the
- line 370: // phone's high-density screen: the eyedropper "disappeared" in 1.0.73)
- line 410: // the earlier version: fixed-size copies (too small on the phone)
- line 433: // the eyedropper is drawn small in its square: it is shown 20% larger ("Our eyedropper could be a little bigger.")
- line 434: // Krimble 2026-11-23: the eyedropper's own SVG files are drawn 20% larger instead; the wrapper had no icon name, so a theme change
- line 435: // skipped it and it stayed light gray in Krimble Light (George: "I want to know why that one tool is borked." / "Yes.")
- line 445: // if (iconStyle() == Classic) {
- line 446: // a tool with no photo is drawn as in Classic
- line 451: // White icons are only shown on a dark background: on light gray or white they are drawn in black ("White should only appear on dark backgrounds.")
- line 454: // Black and white: the silhouettes in black ("the problem with those black and white icons is they're just the color ones desaturated. I'd like to see the silhouettes in black.")
- line 466: // Quick Mask and Screen Mode: the photo in the Photorealistic style, their own drawn icon otherwise
- line 467: // (kept in the button's "krimbleDrawnIcon" property when the button is made)
- line 472: // button->setIcon(!photo.isNull() ? photo : button->property("krimbleDrawnIcon").value<QIcon>());
- line 473: // in the Black style (and on light gray or white buttons) they are drawn black like the tools ("Make UNDO black for black toolbox style.")
- line 487: // namespace KrimbleToolboxIcons

## `libs/ui/widgets/gradient/KisStopGradientEditor.cpp`

- line 33: // Krimble: a large box showing the gradient being edited, running top to bottom ("I want a linear preview in the editor where it shows

## `libs/ui/widgets/gradient/KisStopGradientSlider.h`

- line 79: // Krimble: steady touch editing ("Can you adjust the timings so it's stable?"): a stop moves only after a deliberate drag,

## `libs/ui/widgets/kis_tool_button.cpp`

- line 25: // Krimble 2026-11-23: a button without a menu is a plain button everywhere (the Layers + had a dead strip where its menu used to be)

## `libs/widgets/KoDockWidgetTitleBar.cpp`

- line 86: // started under this row (George, 2026-11-22: "Get them off each other.")
- line 88: // Krimble 2026-11-23 (867th): never for Color Lab. Its logo band takes the place of this row (the row is set to height 0), and the strut
- line 89: // kept a whole empty row above the band (George: "Color Lab was perfect before and now it's ruined." / "NEVER LET ANYTHING MESS IT UP AGAIN.")
- line 180: // gaps measured from the mockup (docs/krimble-refs/2026-11-20_1943_MOCKUP-grips-and-layers-title.jpg): 36 real px after the title, 70 real px before the float button
- line 185: // Tool Options: the tool's name (krimbleToolOptionsName, in the panel) shows under this row on the phone; the grip starts after it
- line 186: // (George, 2026-11-22: "First move the grips over and out of the way." / "Now they're on top of the title.")
- line 209: // six lines (George, 2026-11-22: "The 6 line grip is nicer.")

## `libs/widgets/KoDockWidgetTitleBar.h`

- line 33: /// Krimble: a title shown in the title row in place of the dock's own (the Tool Options docker shows the tool's name),
- line 36: /// Krimble 2026-11-22: a text for the title row while the panel's own title is hidden (the panel is in a tab group and the tab
- line 43: ///< Krimble 2026-10-10: Dock Left/Right/Top/Bottom, Undock

## `libs/widgets/KoDockWidgetTitleBar_p.h`

- line 42: // Krimble: see setTitleOverride()
- line 43: // Krimble: see setTabbedTitle()

## `libs/widgets/KoToolDocker.cpp`

- line 64: // Transform and its Rotate mode: "Transform \u2013 <mode>" (the mode in use is the qApp property krimbleTransformMode, set by the Transform options)
- line 194: // Krimble 2026-11-22: sliders in Tool Options could not be dragged with a finger (George: "Can't move this tiny little \"magnetism\" control."):
- line 248: // Krimble 2026-11-22: 28 real px on the left like the panel titles, 12 on the right (the options touched the panel's edges)

## `libs/widgetutils/KisWideDockSeparatorStyle.cpp`

- line 35: // no gap between a docked panel and the image area: the panel's shadow is its outer edge (KRIMBLE_DESIGN_RULES.md)

## `libs/widgetutils/KisWideDockSeparatorStyle.h`

- line 11: /**
  * Krimble: a proxy style that doubles the pixel metric governing the width
  * of dock widget (panel) separators. On a touchscreen, the default width
  * most desktop styles use (a handful of pixels, sized for a mouse cursor) is
  * too small to reliably grab with a finger to resize a docked panel. Qt ties
  * the visible separator width and its resize hit-test/grab zone to the same
  * single pixel metric, so there's no way to widen only the grab zone without
  * also widening what's drawn on screen -- this doubles both together.
  */

## `libs/widgetutils/KoGroupButton.cpp`

- line 122: // krimblePaintAqua(painter, QRectF(rect()).adjusted(1, 1, -1, -1), qMin(width(), height()) * 0.10);

## `libs/widgetutils/KrimbleAquaPaint.h`

- line 2: // Krimble: the Aqua-style selection look (reference: docs/krimble-refs/2026-11-20_2311_macos-aqua-gradient-reference.png,
- line 3: // approved demo: docs/krimble-refs/2026-11-20_2312_APPROVED-aqua-gradient-demo.png), in the selection blue #0b87dd.
- line 4: // Four layers: a blue body darkest at the top edge and deep through the middle, a bright glow rising from the bottom,
- line 5: // a thin darker rim, and a white gloss over the top half that fades as it goes down.
- line 22: // A glossy Aqua rim for outlines: light at the top, the selection blue in the middle, deep at the bottom.
- line 44: // 1. the body
- line 49: // g.setColorAt(0.85, krimbleAquaMix(base, white, 0.35));
- line 50: // g.setColorAt(1.00, krimbleAquaMix(base, white, 0.50));
- line 51: // darker ("too light ... White disappears over it")
- line 58: // 2. the glow rising from the bottom (an ellipse 1.1 x the width and the height, centered just under the bottom edge)
- line 62: // glow.setColorAt(0.0, QColor(c.red(), c.green(), c.blue(), 140));
- line 63: // glow.setColorAt(0.5, QColor(c.red(), c.green(), c.blue(), 50));
- line 75: // 3. the rim
- line 81: // half of the pen is clipped away: a rim of 'rim' inside the edge
- line 85: // // 4. the gloss over the top half
- line 86: // {
- line 87: //     const QRectF gr(r.left() + r.width() * 0.07, r.top() + r.height() * 0.05, r.width() * 0.86, r.height() * 0.45);
- line 88: //     const qreal gRadius = qMin(radius * 0.85, gr.height() / 2.0);
- line 89: //     QPainterPath gloss;
- line 90: //     gloss.addRoundedRect(gr, gRadius, gRadius);
- line 91: //     QLinearGradient gg(gr.topLeft(), gr.bottomLeft());
- line 92: //     // gg.setColorAt(0.0, QColor(255, 255, 255, 217));
- line 93: //     // gg.setColorAt(1.0, QColor(255, 255, 255, 31));
- line 94: //     gg.setColorAt(0.0, QColor(255, 255, 255, 150));
- line 95: //     gg.setColorAt(1.0, QColor(255, 255, 255, 16));
- line 96: //     p.fillPath(gloss, gg);
- line 97: // }
- line 98: // 3b. dark curved corners at the bottom: only the two corners, each the wedge outside a round curve (like the shine's top corners)
- line 119: // 4. the gloss over the top half: rounded top corners, its sides curving inward as it goes down, so its bottom edge is narrower
- line 120: //    the sizes come from the height, so a wide button keeps narrow sides
- line 126: // the rounded top corner
- line 127: // how far each side comes in at the bottom
- line 139: // visible to its bottom edge, so the shape shows

## `packaging/android/apk/src/org/krimble/android/ConfigsManager.java`

- line 36: /**
  * Copies the default configurations from assets to INTERNAL_STORAGE
  * on first run of the app.
  */
- line 78: // Copying assets can take quite a while, so we show a spinner dialog
- line 79: // during it. The functions relating to that are all synchronized,
- line 80: // which means they take a mutex so that we don't end up with a race.
- line 114: // Don't remove this function, it exists because it's synchronized! We
- line 115: // have to take a lock to avoid races with the UI thread.
- line 120: // We might already be done copying the assets. This function is
- line 121: // synchronized!
- line 131: // This dialog usually appears and disappears very quickly, so
- line 132: // having animations looks pretty wonky. Turn those off.
- line 140: // By default, the alert dialog is full-width. This clamps the
- line 141: // dialog bounds to the spinner so that it looks sensible.
- line 151: // We may not have a dialog if we copied stuff fast enough. Again, note
- line 152: // this function is synchronized!
- line 171: // no assets inside, so a file
- line 191: /**
  * Asset manager API doesn't like trailing `/` at the end and
  * some other things so we need this for proper path
  * @param args file names or directories
  * @return sanitized path
  */
- line 212: // if last character is '/', then delete it
- line 232: // use the same directory structure
- line 256: /**
  * Returns base path for example, in /data/foo it would
  * return /data
  * @param path to be sanitized
  * @return base path
  */

## `packaging/android/apk/src/org/krimble/android/DocumentSaverService.java`

- line 141: // This can happen if the application has been unloaded and only
- line 142: // this saver thread is left dangling. Since the application is
- line 143: // gone, there's no documents left to save, so just bail out.
- line 153: // Promote this to a so-called foreground service. This will keep
- line 154: // the service running even when the application isn't in the
- line 155: // foreground. For further foreground/background confusion, my
- line 156: // Samsung tablet shows the notification for this service under a
- line 157: // (pretty well-hidden) section called "background activities".
- line 170: // No matter if foregrounding succeeded or not, we spawn a thread to
- line 171: // do the saving on. That will take care of un-foregrounding the
- line 172: // service and nulling the member variable once it's finished.
- line 205: // This can be async, because foreground service are promised to run beyond process death.
- line 206: // This has to be async, because we have to handle cancelling action.
- line 211: // if saving finished a long time ago, we will have to kill the app
- line 216: // We wait on Main thread. When we don't do this and use Background service, if the
- line 217: // Activity is killed the Service is automatically destroyed without waiting for threads
- line 218: // to start/finish.
- line 225: // without this Android will think we crashed
- line 228: // TODO: Think about atomicity, which Qt doesn't support for Android
- line 244: // We cannot let the process sit around when the activity is
- line 245: // destroyed. QtActivity depends on lot of statics. We also can't
- line 246: // exit (libc) using Qt's methods because they crash.

## `packaging/android/apk/src/org/krimble/android/DonationDialog.java`

- line 33: // How many days to wait between automatically showing the support options on the splash screen.
- line 190: // Automatically show the supporter page on the splash screen if enough time has elapsed
- line 191: // since the last time it has shown. However, we never automatically show it on first
- line 192: // startup and also never show it two times in a row.
- line 220: // If no products are available (e.g. because the user doesn't have a phone with Google
- line 221: // services on it or this is a Krita Next build), we show donation options instead.
- line 268: // Adjust which elements are visible depending on whether this
- line 269: // is the splash screen or summoned by the user. If this is
- line 270: // the splash screen, it may show or hide some elements
- line 271: // depending on if the application is still starting up or not.
- line 273: // boolean showHeaderButton = mPage == PAGE_SPLASH && mProductDetails != null
- line 274: //         && !mAnyProductsOwned && !mFirstStartup;
- line 275: // Krimble 2026-11-23: no supporter button on the startup splash (George: "get rid of the Show support options pink banner on load")

## `packaging/android/apk/src/org/krimble/android/DonationHelper.java`

- line 39: // Note that for testing this stuff, your Google account needs to be added to
- line 40: // the "License Testers" in the Play Console and you need to test with a
- line 41: // "krita.org" package, not "krita.org.debug". To change the package name, edit
- line 42: // build.gradle and comment out the line `applicationIdSuffix ".debug"`.
- line 47: // Keep this in sync with the enum clas State in KisAndroidDonations.h!
- line 183: // Pending means the payment is still processing and could
- line 184: // conceivably be declined in the future. We'll just treat this
- line 185: // as a successful purchase, it's not worth the extra code.
- line 190: // Ostensibly there's only an UNSPECIFIED_STATE left.
- line 207: /////////////////////////////////////////////////////////////////////
- line 208: // Only uncomment for development, do not let this get released!
- line 209: // This will "consume" the product, meaning the user will not have
- line 210: // it anymore and it will become available for purchase again.
- line 211: /////////////////////////////////////////////////////////////////////
- line 212: // Log.w(TAG, "Developer consuming purchase " + purchase.getPurchaseToken());
- line 213: // mBillingClient.consumeAsync(
- line 214: //     com.android.billingclient.api.ConsumeParams.newBuilder()
- line 215: //         .setPurchaseToken(purchase.getPurchaseToken())
- line 216: //         .build(),
- line 217: //     new com.android.billingclient.api.ConsumeResponseListener() {
- line 218: //         @Override
- line 219: //         public void onConsumeResponse(BillingResult billingResult, String purchaseToken) {
- line 220: //             Log.d(TAG, "Developer consumed '" + purchaseToken + "':" + billingResult);
- line 221: //             refreshPurchases();
- line 222: //         }
- line 223: //     });

## `packaging/android/apk/src/org/krimble/android/DonationOffer.java`

- line 105: // There's a litany of options you can set on products, with multiple plans, special offers
- line 106: // and all sorts of guff. We don't currently intend to use any of that, but we gotta pick a
- line 107: // single offer out of the list *somehow*. So it does this by checking the offer tags (which
- line 108: // can be configured in the Play Console), which is just a p followed by a number, which can
- line 109: // be negative. The offer with the lowest number wins, default priority is 1000.
- line 124: // Doesn't currently handle any discounts or anything, just shows the price.
- line 129: // Fallback, just say "buy" with no pricing information.
- line 135: // We currently only intend to have monthly subscriptions, so the code is only set up to say
- line 136: // "subscribe for price/month" on the button. It can't deal with multiple phases or other
- line 137: // cadences, so this code weeds those out so that the button just says "subscribe" instead.
- line 141: // Billing periods are in ISO8601 duration format, P1M is one month.
- line 149: // Fallback, just say "subscribe" with no pricing information.

## `packaging/android/apk/src/org/krimble/android/DonationQuery.java`

- line 17: // Wrapper around the awful interface of the Android billing client. Some undocumented issues:
- line 18: // * You can't query both one-time and subscription products in one query, it errors.
- line 19: // * Querying for details about products that the user already owns also errors.
- line 20: // * Can't query for subscriptions that contain dashes in their IDs, raises invalid ID errors.
- line 21: // * Can't query for subscriptions that contain underscores in their IDs, just doesn't return them.
- line 22: //
- line 23: // Also, this class does all of its queries sequentially. That is intentional, I don't trust the
- line 24: // billing client to actually be capable of performing two queries at once, considering the numerous
- line 25: // undocumented failure modes it has. It's fine this way.
- line 47: // Part 1: Query for products the user has purchased. We must always do this because inquiring
- line 48: // about products that the user already owns raises an (undocumented) error. We also have to do
- line 49: // this in two queries, because the API doesn't allow both one-time products and subscriptions.
- line 85: // Part 2: Query information about available products, avoiding the forbidden query of
- line 86: // already-owned ones. Even though the API seems to allow it, attempting to query both one-time
- line 87: // and subscription product raises an (undocumented) error.
- line 141: // Part 3: Report back.

## `packaging/android/apk/src/org/krimble/android/KeepAliveService.java`

- line 20: // Keeps Krimble running while it is open. Without it, Android counts Krimble as an idle app as soon as another app is
- line 21: // in front, and its low-memory killer ends it first (the "Last exit: LOW_MEMORY, CACHED" lines in Help > Show Krimble
- line 22: // Log...). A foreground service makes Android count Krimble as in use, so it is not ended to free memory.
- line 29: // what happened when it was last asked to start (shown in Help > Show Krimble Log...)
- line 36: // NotificationChannel channel = new NotificationChannel(CHANNEL_ID, "Krimble is open", NotificationManager.IMPORTANCE_MIN);
- line 50: // return builder.setContentTitle("Krimble is open")

## `packaging/android/apk/src/org/krimble/android/KisSupporterProduct.java`

- line 9: // Keep in sync with KisSupporterProduct.h!

## `packaging/android/apk/src/org/krimble/android/MainActivity.java`

- line 60: // we have to do this before loading main()
- line 64: // this will be passed as a command line argument to main()
- line 115: // unlike onCreate where we did this before, this method is called several times throughout the
- line 116: // lifecycle of our app, but we intend to run this method only once (and in "Foreground").
- line 119: // Full-screening the application here instead of after the main window is shown avoids
- line 120: // some ugly flicker as the Qt UI resizes itself.
- line 122: // Keep the service started so in an unfortunate case where we're not allowed to start a
- line 123: // foreground service, we can try to continue without it.
- line 158: // onPause() _is_ called when the app starts. If the native lib
- line 159: // isn't loaded, it crashes.
- line 187: // The service is already running, so maybe try saving without trying to put it in
- line 188: // foreground. According to docs we should have a couple of minutes of runtime.
- line 192: // We may not be allowed to start a background service either,
- line 193: // probably because onPause is called on an already-paused
- line 194: // application that is beyond the "couple of minutes" cutoff.
- line 201: // Docs say: this method will not be called if the activity's hosting process
- line 202: // is killed. This means, for us that the service has been stopped.
- line 221: // if (!JNIWrappers.hasMainWindowLoaded()) {
- line 223: // back button was pressed during splash screen, letting this
- line 224: // propagate leaves native side in an undefined state. So, it's
- line 225: // best we finish the activity here.
- line 235: // We manually pass these events to the QPA Android because,
- line 236: // android doesn't send events of type other than SOURCE_CLASS_POINTER
- line 237: // to the view which was just tapped. So, this view will never get to
- line 238: // QtSurface, because it doesn't claim focus.
- line 390: // Krimble 2026-11-22: George: "I keep installing Krimble and I don't remember to set it to unrestricted battery use."
- line 391: // Once per install (the flag is cleared with the app's data), a few seconds after the start, Android's own question
- line 392: // "Let app always run in background?" is shown. Allow sets Krimble to unrestricted battery use.
- line 463: // whether the keep-alive service is running now, and what happened when it was asked to start
- line 482: // return "Keep-alive service: running now: "

## `packaging/android/apk/src/org/krimble/android/ScalingDialog.java`

- line 44: // Nothing.

## `packaging/android/apk/src/org/krimble/android/ShareHelper.java`

- line 77: // Removes the empty entry made by prepareCameraUri when the photo was cancelled.

## `packaging/android/apk/src/org/krimble/android/VideoEncoder.java`

- line 50: // Keep these formats in sync with KisAndroidMediaEncoderRunnable!
- line 54: // These statuses too!
- line 745: // Should be the default, but let's set it if supported.
- line 758: // Note that this doesn't necessarily actually result in a track with
- line 759: // the given parameters! See the notes in the C++ code about only using
- line 760: // known good parameters to actually get something useful here.
- line 908: // These are the built-in Android software encoders.

## `plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`

- line 40: // Krimble 2026-11-23: George: "If I tear off ColorLab it should snap to the size of the content." / "It should not allow to be stretched out past

## `plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.h`

- line 29: // Krimble 2026-11-23: torn-off Color Lab: snaps to its content, which is also its largest size; smallest = the fewest palette rows

## `plugins/dockers/advancedcolorselector/kis_color_selector_ng_docker_widget.cpp`

- line 129: // Krimble 2026-11-23: the content always starts at the left edge: a narrow window cuts off the right side, never the left
- line 130: // (George: "You just can't let ColorLab chop off the left edge of its own window.")

## `plugins/dockers/advancedcolorselector/krimble_color_canvas.h`

- line 2: // Krimble 2026-11-08: the Color panel's Picker page, built from the MEASUREMENTS of the mockup (color_panel_measurements.md; George: "Measure everything for proportions. Don't
- line 3: // wing it. Sizing and scale matter." and "Remember this app asks the user to scale it as a percentage. Issues of scale are not a problem."). The canvas is the mockup's card:
- line 4: // 1178 x 1100 design units (the mockup's own pixels). Every part is placed at its measured rectangle times the scale (the panel's width / 1178), so the proportions are exact at any
- line 5: // size, and the symbols are the mockup's own (krimble_color_symbols.h). The color square, hue strip, number boxes, HEX box, color boxes, recent colors and palette cells are real
- line 6: // widgets of krimble_color_studio.h; the canvas paints the frames, wells, labels and symbols under and around them.
- line 31: // the card's height with 4 palette rows
- line 32: // one palette row (206 units for 4 rows)
- line 33: // rows added under the 4 when the panel is taller (up to 14 rows of 19: 266 swatches)
- line 46: // the parts the color studio hands over (all children of this canvas)
- line 56: // buttons made here (transparent: the canvas paints them)
- line 63: // 2026-11-08: the palette name box (opens the list of palettes)
- line 64: // rename the palette
- line 65: // duplicate the palette
- line 66: // New, Open, Import, Export, Delete
- line 67: // 2026-11-08: George: "you better be able to scroll through 256 swatches." The palette area shows 4 rows of 19; the scroll bar, the wheel
- line 68: // and the arrows move it by rows
- line 69: // the first visible row
- line 116: // the up and down parts of the six number boxes: call once, after hsv[] and rgb[] are set
- line 136: // the sizes of the color boxes' own frames follow the scale (the style sheet text is made here)
- line 145: // return QStringLiteral("background:%1;border:%2px solid #e6ecfa;border-radius:%3px;")
- line 146: //     .arg(c.name()).arg(qMax(2, qRound(3 * s))).arg(qMax(3, qRound(14 * s)));
- line 193: // the frames' sizes follow the scale
- line 206: // if (paletteCells) layoutCells(*paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2);
- line 227: // the card: a smooth gradient (measured #283248, #242A3F, #1B2235), a dark outline and a light line along the top that fades toward both ends
- line 250: // the mode drop-down (HSV or HSL) and the eyedropper button (the Picker page only: the other pages are widgets over this part)
- line 266: // the number boxes: H S V, then R G B, with their labels and units
- line 275: // the arrow section: the right 38 units
- line 299: // the two thin dividers of the value column, HEX with its copy button
- line 308: // the Picker page's own painting
- line 311: // Recent Colors: a divider above and below, the label, the dark well with the ten cells (empty places are dark), the chevron of the mockup is left out
- line 316: // the label is as wide as the mockup's (170 units for "Recent Colors")
- line 322: // paintEmptyCells(p, recentCells, 10, 1, QRectF(428, 663, 658, 55), 7.0, 0.0, QColor("#1a1f33"), 5 * s);
- line 323: // 2026-11-09: square corners
- line 325: // the palette row: grid symbol, the name, the add and remove buttons; then the palette well
- line 326: // the mockup's palette row: a raised frame with the grid symbol and "Palette:", joined to the raised name box
- line 333: // paintField(p, r(215, 761, 536, 64), 10 * s);
- line 339: // 2026-11-08: the rest of the mockup's palette row: the chevron of the name box, a divider, the pencil, duplicate and "..." buttons
- line 350: // paintEmptyCells(p, paletteCells, 19, 4, QRectF(29, 850, 1070, 206), 3.5, 4.2, QColor("#0f1325"), 3 * s, paletteScroll);
- line 351: // 2026-11-09: square corners
- line 352: // the scroll bar of the palette (the mockup's: up and down arrows and a thumb), measured: 39 units wide beside the well
- line 369: // the scroll bar: the thumb's place (its length is the part of the palette that shows)

## `plugins/dockers/advancedcolorselector/krimble_color_history.h`

- line 2: // Krimble 2026-11-08: the History page of the Color panel, from the History mockup (George: "make the new tabs work just like the mockups"). Every color that becomes the foreground
- line 3: // color is recorded with the tool that was active and the time; the list shows them (newest first) with a pin to keep a color in the Pinned Colors, the options group duplicates, show
- line 4: // the tool and show the time, a limit, and Save History as Palette and Clear History. Measured in the mockup (the card scaled to 1178 units wide, 1062 tall).
- line 92: // record a color that became the foreground color (a color equal to the newest entry only updates its time when duplicates are grouped)
- line 115: // the name of the tool for the list ("KritaShape/KisToolBrush" is "Brush")
- line 131: // a color chosen here becomes the foreground color
- line 132: // a palette was made from the history
- line 205: // the card
- line 227: // 2026-11-08: the left column's text is one size (measured: that text is 286 units wide)
- line 233: // the start of every fit below: the fit makes it smaller, to the width measured in the mockup
- line 234: // ---- the left group
- line 301: // ---- the right group: the title, the filter, the list or grid
- line 318: // the list
- line 329: // if (current) { selectedDone = true; paintSelected(p, row, 12 * s, s); }
- line 330: // a selection: flat blue
- line 334: // measured: 99 units wide
- line 337: // measured: 186 units wide
- line 368: // the scroll bar at the right of the list
- line 416: // a double tap on a pinned color unpins it
- line 451: // p.setPen(QPen(QColor("#0b87dd"), qMax<qreal>(2.5, 4.5 * s), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
- line 452: // p.setPen(QPen(krimbleAquaRimBrush(box), qMax<qreal>(2.5, 4.5 * s), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));   // Aqua tic
- line 453: // a selection mark: flat bluek
- line 463: // the options
- line 476: // the pinned colors
- line 479: // the three buttons
- line 483: // the filter and the view buttons
- line 487: // the list: a tap on the pin toggles it, a tap elsewhere in a row sets the color

## `plugins/dockers/advancedcolorselector/krimble_color_icons.h`

- line 2: // Krimble 2026-11-08: the colored icons of the Color panel (the color models and the rows of the Values page), cut out of the mockups with the distance from the button color as transparency.
- line 15: // 58 x 55
- line 16: // 62 x 56
- line 17: // 53 x 55
- line 18: // 62 x 52
- line 19: // 47 x 54
- line 20: // 40 x 32
- line 21: // 32 x 30
- line 22: // 37 x 38
- line 23: // 36 x 35
- line 24: // 42 x 40
- line 25: // 34 x 38
- line 26: // 31 x 30
- line 27: // 31 x 30
- line 28: // 33 x 34
- line 29: // 32 x 32
- line 30: // 40 x 40
- line 31: // 39 x 38
- line 32: // 29 x 29
- line 33: // 40 x 39
- line 34: // 48 x 30
- line 35: // 48 x 33
- line 36: // 48 x 29
- line 37: // 48 x 30
- line 38: // 48 x 30
- line 39: // 48 x 33
- line 40: // 48 x 33
- line 41: // 48 x 39
- line 42: // 60 x 55
- line 43: // 60 x 54
- line 44: // 54 x 43
- line 45: // 56 x 55

## `plugins/dockers/advancedcolorselector/krimble_color_logo.h`

- line 2: // Krimble 2026-11-08: the Color Lab logo (George's: colorlab-3.webp, the complete one; colorlab-3.png is cut off at the right edge of the B and has white bars), as a PNG 900 x 211
- line 3: // (a picture Qt always reads: the WebP reader is a plugin that may not be in the Android build). The picture has its own black background; it is not transparent.

## `plugins/dockers/advancedcolorselector/krimble_color_pages.h`

- line 2: // Krimble 2026-11-08: the Sliders and Values pages of the Color panel, from the new mockups (George: "make the new tabs work just like the mockups"). The measurements are the mockups'
- line 3: // pixels converted to the card's 1178 design units (the card of each mockup scaled to the same width), see color_pages_measurements.md. Both pages are drawn at the same scale as
- line 4: // the Picker page (krimble_color_paint.h) and share its bottom part (the foreground and background colors, recent colors, the palette).
- line 20: // the color the panel is working on: opaque RGB, the alpha channel apart, and the hue to keep when a color is gray (a gray has no hue)
- line 28: // the channels of each color model: 0 RGB, 1 HSV, 2 HSL, 3 CMYK, 4 Grayscale
- line 78: // the color after channel i of the model is set to v (the other channels as they are)
- line 107: // the color at fraction t (0 to 1) along the track of channel i. RGB tracks are the mockup's: the channel's own color fading to a pale tint of it (measured: #F80302 to #FED2D8 for R,
- line 108: // #09E80C to #D0FED8 for G, #1010F9 to #A8AAFC for B); the others show what the channel does (hue: the rainbow; saturation: gray to the color; value and lightness: dark to light;
- line 109: // the inks: white to the ink; gray: black to white)
- line 128: // a channel bar: a pill track filled with the colors the channel runs through (left to right), a glossy ball
- line 132: // the color at 0 to 1 along the track (alpha included for the alpha track)
- line 135: // the ball's own color when it is not the track's color at the ball (the RGB bars have the channel's color: the mockup's red, green and blue balls)
- line 167: // the ball
- line 190: // Sliders page: the color model buttons at the left; at the right the channel sliders with their number boxes, then the color preview with HEX, copy and the alpha preview
- line 195: // a new opaque color from a slider or a box
- line 238: // the channels and the alpha
- line 240: // read the state into the bars, boxes and HEX
- line 274: // the mockup's magenta ball
- line 336: // the two groups
- line 345: // the model buttons
- line 357: // the rows: the label, then the box frame (the bar and the number are widgets above)
- line 368: // the divider, Color Preview and its line, the preview parts
- line 386: // the alpha preview: a checker with the color (with its alpha) in the middle and a white frame
- line 419: // the nearest named color (the SVG color names) with "(custom)" when it is not exactly that color; and the web safe color (each channel a multiple of 51)
- line 440: // Values page: the color preview with its alpha at the left; at the right every color model as number boxes (RGB, HSV, HSL, CMYK), HEX, the color's name and its web safe color
- line 455: // the boxes: RGB 0 to 2, HSV 3 to 5, HSL 6 to 8, CMYK 9 to 12, alpha 13 (0 to 255) and 14 (0 to 100)
- line 516: // the narrow boxes (the inks, the alpha) have smaller text so three digits fit
- line 568: // the help mark of the group: a circle with a question mark
- line 573: // the preview and the alpha
- line 604: // the model rows
- line 633: // HEX
- line 637: // Color Info
- line 667: // the box of channel c of a model row (the mockup's measured places)
- line 695: // the tab strip above the card: Picker, Sliders, Values, Palette, History (measured in the Picker mockup: five tabs 220 units wide every 231, 75 tall; the selected one 235 x 82 with the
- line 696: // cyan outline and glow of every selected item)
- line 705: // (not Fixed: a fixed height would be the size hint's, not the height the width asks for)
- line 724: // the unselected tabs first, then the selected one over its neighbors
- line 757: // QRectF tabRect(int i, bool selected) const
- line 758: // {
- line 759: //     const double x = 31.0 + 231.0 * i;
- line 760: //     return selected ? r(x - 7.5, 0, 235, 82) : r(x, 3, 220, 75);
- line 761: // }
- line 762: // 2026-11-08: George: "The tabs should fill the horizontal space." Five equal tabs from the left margin to the right margin (24 units each side, the line the logo's left edge is on),
- line 763: // 8 units apart; the selected one is 5 units wider at each side.
- line 783: // for (int i = -1; i <= 1; ++i) p.fillRect(QRectF(0, mid + i * gap - thick / 2, width(), thick), QColor(150, 158, 190));
- line 784: // the same grip lines as the panel title rows (3 real px lines on a 9 real px pitch, the title-row color), as many as fit the band, up to nine
- line 801: // "QToolButton:pressed { background: #0b87dd; }");
- line 834: // int l = int(width() * 0.62), r = m_float->x() - qRound(12 * scale());
- line 835: // const int most = int(120.0 / dpr);
- line 836: // if (r - l > most) { const int extra = (r - l) - most; l += extra / 2; r -= extra - extra / 2; }
- line 837: // Q_UNUSED(most);
- line 838: // m_lines->setGeometry(l, 0, qMax(0, r - l), height());
- line 839: // m_lines->setVisible(false);
- line 840: // grip lines on the black part of the logo band, centered between the end of the letters and the float button
- line 841: // The logo is drawn h = 1.2 x the band height tall, w = h x 900 / 211 wide, from x = 24 units - 0.0514 w; its letters end at 851 / 900 of the picture.
- line 878: // the panel's docking helper (KrimbleDockTolerance, on the panel) sees this drag as if the panel's own title row moved it,
- line 879: // so a drag to the window's edge docks Color Lab again
- line 922: // the picture has black space around the letters (they are about half its height): it is drawn larger than the band and clipped to it, so the letters fill the band
- line 926: // p.drawPixmap(QRectF((width() - w) / 2, (height() - h) / 2, w, h), logo, QRectF(logo.rect()));   // centered: against George's rule (2026-11-08)
- line 927: // George's rule, 2026-11-08: "Do not center title logos. THIS IS A RULE. LEFT ALIGN." The left edge of the C is on the same line as the left edge of the first tab (23.5 units); the
- line 928: // picture has about 5.1% of black space at its left, which is taken off.

## `plugins/dockers/advancedcolorselector/krimble_color_paint.h`

- line 2: // Krimble 2026-11-08: the shared base of the pages of the Color panel (Picker, Sliders, Values, Palette, History): a page is a card of 1178 design units wide (the mockups' card, scaled
- line 3: // to the same width) that places its parts at the measured rectangles times the scale (the width / 1178), and paints with the same frames, wells, lines and symbols.
- line 18: // sets a widget's own style sheet only when it changes (setting one, even the same text, makes Qt restyle the widget)
- line 27: // the card's width in design units
- line 52: // the largest font (up to the base size) in which the text is not wider than the width
- line 80: // a raised button frame: a gradient from lighter to darker, a dark outline and a light line along the inside top
- line 101: // a dark inset field (number boxes, HEX, the name of the palette): dark at the top, a lighter outline, a light line along the bottom edge
- line 112: // p.setPen(QPen(QColor("#4a557b"), 1.3));
- line 113: // 2026-11-08: the rim measured in the Sliders mockup is #6175B0
- line 118: // a recessed well (recent colors, palette): very dark, a dark outline and a light line along the bottom edge
- line 137: // a dark raised button (the model buttons of the Sliders page, measured): a bright line along the top, a lighter line along the bottom, dark in between
- line 159: // a number box with its arrow section (the right 38 units): the field, a lighter section with a divider, and the up and down arrows
- line 180: // a divider of the groups (measured in the Sliders mockup: a bright line with a dark one under it)
- line 188: // a checker pattern (transparency): two grays, square cells, clipped to a rounded rectangle
- line 205: // a titled group: a frame a little darker than the card, a bright line along its top edge, a lighter left edge
- line 228: // the look of a selected item: a deep blue fill, a bright cyan outline and a glow (measured in the mockups' selected buttons and tabs)
- line 231: // the Aqua-style selection, the same as the toolbox and every checked button
- line 234: // for (int i = 6; i >= 1; --i) {
- line 235: //     p.setPen(QPen(QColor(11, 135, 221, 20 + (6 - i) * 9), 2.0));
- line 236: //     p.setBrush(Qt::NoBrush);
- line 237: //     p.drawRoundedRect(rect.adjusted(-i * 1.1 * s, -i * 1.1 * s, i * 1.1 * s, i * 1.1 * s), radius + i * s, radius + i * s);
- line 238: // }
- line 239: // const QRectF b = rect.adjusted(0.5, 0.5, -0.5, -0.5);
- line 240: // p.setPen(Qt::NoPen);
- line 241: // p.setBrush(QColor("#0b87dd"));
- line 242: // p.drawRoundedRect(b, radius, radius);
- line 243: // p.setBrush(Qt::NoBrush);
- line 244: // p.setPen(QPen(QColor("#0b87dd"), 2.4 * qMax<qreal>(0.5, s * 2)));
- line 245: // p.drawRoundedRect(b, radius, radius);
- line 247: // a pill that keeps the color and the glow of the item it shows (the preview boxes): a colored rounded square with the cyan outline and its glow
- line 259: // p.drawRoundedRect(b, radius, radius);
- line 261: // the icon pictures (colored, from krimble_color_icons.h) are drawn at the mockup's own size times the scale

## `plugins/dockers/advancedcolorselector/krimble_color_palette_actions.h`

- line 2: // Krimble 2026-11-08: the palette commands of the Color panel (New, Open, Import, Export, Duplicate, Rename, Delete), shared by the Palette page and the palette row of the Picker,
- line 22: // 2026-11-08: George: "palettes stored as CMYK color values (4 channels, commonly called plates in printing)": a new palette is an RGB palette or a CMYK palette
- line 61: // exporting changes no palette

## `plugins/dockers/advancedcolorselector/krimble_color_palette_page.h`

- line 2: // Krimble 2026-11-08: the Palette page of the Color panel, from the Palette mockup (George: "make the new tabs work just like the mockups"), measured and scaled to the card's 1178
- line 3: // units (1098 tall). It works on the named palettes (krimble_color_palettes.h): choose a palette, make a New one, Open, Import, Export, Duplicate, Rename or Delete it; the info shows
- line 4: // its name, colors and format; the selected color can be changed (index, HEX, R G B); the swatch tools pick from the image, replace the swatch with the foreground color, reload the
- line 5: // palette from its file, or change the number of columns; the grid shows the swatches (tap one to use it, drag to scroll); the recent colors are below.
- line 19: // a swatch chosen here becomes the foreground color
- line 20: // the active palette or its colors changed
- line 21: // the eyedropper of the swatch tools
- line 22: // the recent colors (the panel's list)
- line 27: // 2026-11-08: the four plates of a CMYK palette's selected swatch
- line 76: // read the active palette from its file
- line 94: // choose a swatch (by index); set the foreground color when asked
- line 164: // titles
- line 166: // the buttons, the info and the small labels
- line 170: // ---- the left group
- line 252: // ---- the grid group
- line 291: // p.setPen(QPen(QColor("#0b87dd"), qMax<qreal>(2.0, 3.2 * s)));
- line 292: // p.setPen(QPen(krimbleAquaRimBrush(cell), qMax<qreal>(2.0, 3.2 * s)));   // Aqua rim on the chosen swatch
- line 293: // the chosen swatch: flat selection blue
- line 299: // ---- the recent colors
- line 379: // a CMYK palette's swatch is edited with its four plates, an RGB palette's with R, G, B
- line 390: // change the selected swatch's color and save the palette
- line 396: // 2026-11-08: through the palette: a CMYK palette keeps the plates (made from this color)
- line 404: // 2026-11-08: the four plates (percent) of the selected swatch of a CMYK palette
- line 426: // the palette chooser
- line 438: // the seven commands
- line 454: // the swatch tools: pick from the image, replace the swatch with the foreground color, reload from the file, the number of columns
- line 478: // the recent colors
- line 481: // the grid

## `plugins/dockers/advancedcolorselector/krimble_color_palettes.h`

- line 3: // other programs read and write) in the program's data folder, with a name, a number of columns and its colors. The Palette page and the palette row of the Picker, Sliders and
- line 4: // Values pages use them: the drop-down lists the names, the plus adds the foreground color to the active palette, and New, Open, Import, Export, Duplicate, Rename and Delete work on
- line 5: // the files. The Mixer panel's "add to palette" adds to the active palette too.
- line 19: // the four plates of a CMYK color, in percent (0 to 100)
- line 34: // always filled: the RGB value of each swatch (for a CMYK palette: made from its plates)
- line 36: // 2026-11-08: George: "I want to be able to edit RGB and CMYK palettes too." and "palettes stored as CMYK color values (4 channels, commonly called plates in printing)". A palette is
- line 37: // an RGB palette (each swatch is R, G, B) or a CMYK palette (each swatch is its four plates C, M, Y, K in percent, which is what is stored and edited; the RGB value is made from them).
- line 39: // for a CMYK palette: the plates of each swatch (the same size as colors)
- line 75: // a CMYK palette has its plates for every swatch (made from the RGB value where a file lacked them), and its RGB values come from the plates
- line 88: // the folder of the palette files (tests can set another)
- line 148: // the file name may differ from the name inside (a palette copied in): look through the files
- line 162: // a CMYK palette is a GIMP palette too: every swatch has its RGB value (the line other programs read) and, as a comment, its four plates (which this program reads and edits)
- line 219: // 2026-11-08: was "My Palette"
- line 223: // add a color to the active palette (a color already in it is not added twice)
- line 235: // a text that changes when the active palette's name or colors change (the panel reloads when it does)
- line 274: // setActive(mine.name);
- line 275: // 2026-11-08: George: "palette should default to 256 colors." The active palette on the first run is the 256 color one.

## `plugins/dockers/advancedcolorselector/krimble_color_studio.h`

- line 45: // the eyedropper button switches to the color sampler tool
- line 47: // the named palettes (GPL files)
- line 48: // the Palette tab
- line 49: // the History tab
- line 92: // Suppress clicked() after the long-press context menu without
- line 93: // leaving the underlying QPushButton in a pressed state.
- line 110: // Hue/saturation/value picker. The painting is local to the widget; edits are
- line 111: // committed through the same canvas resource manager as Krita's other selectors.
- line 181: /**
  * Independent mixing scratchpad: its raster is never connected to the
  * document's projection or undo stack. Color picking is the only operation
  * that can explicitly send a sampled color back to Color Studio.
  */
- line 192: // Qt synthesizes mouse strokes
- line 232: // if (m_paint.size()!=event->size()) {
- line 233: //     QImage newImage(event->size(),QImage::Format_ARGB32_Premultiplied);
- line 234: //     newImage.fill(Qt::transparent);
- line 235: //     if (!m_paint.isNull()) {
- line 236: //         QPainter painter(&newImage);
- line 237: //         painter.drawImage(0,0,m_paint);
- line 238: //     }
- line 239: //     m_paint=newImage;
- line 240: // }
- line 291: // the picture has one pixel per screen pixel now: the logical point is scaled to it (before it was one to one)
- line 311: // Default oil-bristle simulation: individual semi-transparent
- line 312: // strands create ridges and broken coverage as colors overlap.
- line 313: // This is local QPainter rendering, not a Krita brush preset.
- line 341: // each stamp is light; the stamps along the line add up to the chosen opacity
- line 389: // p.setPen(QPen(QColor("#8498bd"), 1));
- line 390: // p.drawRoundedRect(strip, 5, 5);
- line 392: // a pill: fully rounded ends (the mockup's)
- line 394: // p.setBrush(Qt::NoBrush);
- line 395: // p.setPen(QPen(Qt::black, 3));
- line 396: // p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
- line 397: // p.setPen(QPen(Qt::white, 2));
- line 398: // p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
- line 399: // the handle of the mockup: a white pill outline, 28% as tall as the strip is wide, reaching 4 units beyond the strip at each side
- line 437: // the height follows the width, so the logo keeps its shape whatever layout it sits in
- line 521: // the Palette tab and the History tab
- line 530: // every color that becomes the foreground color is recorded in the history, with the tool and the time, when it has stayed for a moment (not while a slider is dragged)
- line 536: // it must be what the canvas has as the edited color (committed), not just what the picker shows
- line 856: // 2026-11-08: no cap of 76 any more: the palette area scrolls (George: "you better be able to scroll through 256 swatches")
- line 862: // Keep the controls in sync when another docker or the eyedropper changes
- line 863: // the foreground color. Only poll while the Studio is visible.
- line 882: // from here a color that changes is recorded in the history
- line 913: // Read the existing document foreground color on canvas switch.
- line 918: // Krimble 2026-11-23: the height the panel's scroll area needs to show the card with this many palette rows under the first 4
- line 919: // (0 = the fewest rows, 10 = all of them), at the card's present width; -1 if it can't tell yet. Used by the torn-off Color Lab's size limits.
- line 929: // When the panel is taller than the picker needs, the palette under it gets more rows (up to 14 rows of 19),
- line 930: // and fewer again when the panel gets shorter. The panel's scroll area decides how much room there is.
- line 975: // m_hsv[i]->setValue(i==0 ? qMax(0,c.hsvHue()) :
- line 976: //                    qRound((i==1 ? c.hsvSaturation() : c.value())*100.0/255.0));
- line 1091: // 1, not 0: a minimum of 0 means "use the button's own minimum", which is wider
- line 1092: // the cells grow with the panel, up to 20 px (was 26)
- line 1095: // button->setStyleSheet(QStringLiteral(
- line 1096: //     "background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
- line 1110: // 1, not 0: a minimum of 0 means "use the button's own minimum", which is wider
- line 1111: // the cells grow with the panel, up to 20 px (was 26)
- line 1149: // button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #05060f;border-radius:5px;").arg(color.name()));
- line 1150: // 2026-11-09: square corners
- line 1158: // ? QStringLiteral("background:%1;border:2px solid #0b87dd;border-radius:0px;").arg(color.name())
- line 1169: // the selected swatch has the bright outline of a selected item; it is the one the trash button removes
- line 1175: // only the two cells that changed are restyled
- line 1188: // 2026-11-08: through the palette, so the plates of a CMYK palette stay with their swatches
- line 1240: // 2026-11-08: RGB or CMYK (the plates of each swatch are m_cmyk)

## `plugins/dockers/advancedcolorselector/krimble_color_symbols.h`

- line 2: // Krimble 2026-11-08: the symbols of the Color panel, cut out of the Picker mockup itself (George: "Is it that hard to copy these elements?") as white shapes on a transparent
- line 3: // background, each cropped to the shape; the sizes are the mockup's own pixels (the card is 1178 px wide). Made with the brightness of each symbol as its transparency.
- line 16: // 45 x 46
- line 17: // 37 x 39
- line 18: // 44 x 41
- line 19: // 40 x 40
- line 20: // 38 x 43
- line 21: // 39 x 39
- line 22: // 18 x 11
- line 23: // 23 x 15
- line 24: // 18 x 11
- line 25: // 44 x 44
- line 26: // 43 x 44
- line 27: // 37 x 9
- line 28: // 24 x 15
- line 29: // 20 x 33
- line 30: // 23 x 15
- line 31: // 23 x 15

## `plugins/dockers/advancedcolorselector/krimble_dpaint_dock.h`

- line 2: // Krimble 2026-11-07: the DPaint panel. George: "unified DPaint style with pixelated black icons for tools. Retro nostalgia." (gray bevelled buttons, black pixel icons, the way the
- line 3: // classic toolbox looked), "Build it the way it feels right. We can tweak it after a build to see it working." One panel (Window menu: DPaint) with: the tools in two columns
- line 4: // (switching to the tool and showing which one is active), UNDO / redo / CLR / select all, the color indicator (the background color as a box with the foreground color as a dot in
- line 5: // it: tap the dot for the foreground color, tap the box for the background color, long press to swap), and a palette of 256 colors shown as pages of 4 x 8 wells with arrows:
- line 6: // tap a well for the foreground color; RANGE then the first and the last well fills the wells between them with a blend (BACK undoes the last change); EDIT then a well changes
- line 7: // that well's color. The palette is saved with the program's settings. Everything is drawn here (no picture files).
- line 385: // a 16 x 16 icon (# is a black pixel) drawn in the middle of an area, as large as a whole number of pixels per icon pixel allows
- line 402: // the bevel: light edges at the top and the left, dark at the bottom and the right (swapped when pressed), a 2 px edge
- line 423: // a gray bevelled button with a black pixel icon, or with a few letters; square when it has an icon
- line 456: // the color indicator: the background color as a box, the foreground color as a dot in the middle of it
- line 479: // an inset edge: dark at the top and the left, light at the bottom and the right
- line 492: // the swap already happened
- line 506: // one page of the palette: 4 columns by 8 rows of touching wells with dark lines between them
- line 516: // 4 x 8 wells, each as tall as wide
- line 559: // the gray of the classic toolbox
- line 562: // tools, two columns: the tool id and its pixel icon
- line 595: // the actions: UNDO, redo, CLR, select all
- line 613: // the color indicator
- line 636: // the palette: a page of 4 x 8 wells and the page arrows
- line 652: // RANGE, EDIT, BACK
- line 698: // follows the colors and the active tool changed elsewhere
- line 764: // the wells between the first and the last become a straight blend of the two (in RGB)
- line 778: // 256 colors: 16 grays, 16 bright colors, then 14 hues of 16 lightnesses; saved with the program's settings

## `plugins/dockers/advancedcolorselector/krimble_mixer_dock.h`

- line 2: // Krimble 2026-10-30: the Mixer panel. George: "I specifically asked for a color mixing scratch pad. That's why there's a mockup of that. It should be a
- line 3: // panel." and, asked whether it should stay in Color Studio or be its own panel: "I think it is a good idea for a panel." The mixing pad that was the
- line 4: // Mixer tab of Color Studio (krimble_color_studio.h) is a panel of its own, "Mixer", opened from the Window menu. The pad is independent of the picture:
- line 5: // its raster is never part of the document or its undo stack. It paints with the foreground color; Sample sets the foreground color from the pad;
- line 6: // + Palette adds the foreground color to the Color panel's saved swatches.
- line 35: // the buttons of this panel have less padding, so a row of four fits a docked panel (3 px above and below, 5 px at the sides)
- line 47: // button->setMinimumHeight(38);
- line 61: // mixLayout->addWidget(m_pad, 1);
- line 62: // The pad now paints with Krita's own brush engine and the current brush preset (Krita's scratch pad, the one in the brush editor):
- line 63: // "Just use the built in Krita brush engine. They put most of their efforts into it." The home-made pad is kept, hidden.
- line 72: // auto *brushControls = new QHBoxLayout;
- line 73: // brushControls->addWidget(new QLabel(tr("Size"), panel));
- line 74: // auto *brushSize = new QSpinBox(panel);
- line 75: // brushSize->setRange(1, 100); brushSize->setValue(18); brushSize->setMinimumHeight(38);
- line 76: // brushControls->addWidget(brushSize);
- line 77: // brushControls->addWidget(new QLabel(tr("Opacity"), panel));
- line 78: // auto *brushOpacity = new QSpinBox(panel);
- line 79: // brushOpacity->setRange(1, 100); brushOpacity->setValue(85); brushOpacity->setMinimumHeight(38);
- line 80: // brushOpacity->setSuffix(QStringLiteral("%"));
- line 81: // brushControls->addWidget(brushOpacity);
- line 82: // mixLayout->addLayout(brushControls);
- line 89: // 12 px extra: the style adds some padding
- line 107: // the real brush uses the current preset's own size and opacity (set in Tool Options or the brush editor): these rows are hidden
- line 130: // Paint, Move (pan the pad), Sample. Erase is now Move: the real pad erases with the brush's own eraser mode.
- line 152: // Krita's scratch pad has no undo of its own: Undo and Redo are hidden; Clear wipes the real pad to white
- line 155: // + Palette: the foreground color joins the Color panel's saved swatches (the same saved list; the Color panel shows it within a moment)
- line 160: // QSettings settings;
- line 163: // Sample: the sampled color becomes the foreground color
- line 168: // the pad paints with the foreground color; it follows changes made elsewhere (only while the panel is visible)
- line 183: // the real pad is made the first time there is a view: it needs the view's brush and color resources
- line 205: // Krita's own scratch pad: the real brush engine

## `plugins/dockers/layerdocker/LayerBox.cpp`

- line 172: // explicit KrimbleLongPressOpen(QComboBox *combo, int delayMs = 500) : QObject(combo), m_combo(combo)
- line 173: // Krimble 2026-11-23: 400 ms (George: "Set it to 400ms.")
- line 221: // George: "If all else fails make the buttons smaller." One row of normal buttons (icons 18 px) if they fit; else one row of smaller ones (14 px) if those fit;
- line 368: // Krimble 2026-11-23: a bold "Type:" label left of the blend drop-down, about 15% bigger than its text
- line 369: // (George: "Please put the word "Type:" next to the layer type selector", "too small and should be bold", "Better. Push it.")
- line 374: // if (krimbleTypeFont.pointSizeF() > 0) krimbleTypeFont.setPointSizeF(krimbleTypeFont.pointSizeF() * 1.15);
- line 376: // else if (krimbleTypeFont.pixelSize() > 0) krimbleTypeFont.setPixelSize(qRound(krimbleTypeFont.pixelSize() * 1.15));

## `plugins/dockers/layerdocker/NodeDelegate.cpp`

- line 130: // if (qApp->property("krimbleSelectionGradient").toBool()) {
- line 135: // krimblePaintAqua(*p, QRectF(option.rect), 0.0);   // the Aqua-style selection on the selected layer row

## `plugins/dockers/tasksetdocker/taskset_resource.h`

- line 40: /**
  * Krimble: one record per recorded step. Exactly one of `action`,
  * `filterId`, or `operationId` is set; `configXml` carries the
  * serialized parameters for the filter/operation cases.
  */

## `plugins/dockers/tasksetdocker/tasksetmodel.h`

- line 18: /**
  * Krimble: a step is either a plain action trigger (the original,
  * Krita-native behaviour -- fine for parameterless commands like Flatten
  * Image or a tool switch), or a real filter application captured with its
  * exact parameters via KisFilterManager::sigFilterApplied -- the
  * industry standard actions equivalent case, correctly replayable without
  * reopening an empty dialog the way a plain trigger of a filter's menu
  * action would.
  */

## `plugins/extensions/colorrange/colorrange.cc`

- line 94: /**
  * Krimble: Select > Similar, as in the industry standard.
  * Adds to the selection every pixel of the active layer whose colour is
  * within the Magic Wand's threshold of a colour inside the current selection.
  * Colours are kept in a 64 x 64 x 64 grid (4 levels per cell), grown by the
  * threshold per channel, then every pixel of the picture is looked up.
  */

## `plugins/extensions/rotateimage/rotateimage.cc`

- line 32: /**
  * Krimble: DlgRotateImage is already a full KoDialog (its own preview/lock
  * logic), same situation as DlgCanvasSize -- a small custom factory
  * wrapping it as-is, exactly as slotRotateImage() always did, rather than
  * refactoring its internals to fit the widget-template shape.
  */

## `plugins/filters/colors/KrimbleLensFlare.h`

- line 30: // the light, in picture pixels
- line 31: // the center of the picture
- line 32: // the picture's diagonal
- line 33: // 0 .. 2
- line 34: // 0 .. 2
- line 35: // 0 .. 2
- line 36: // 0 .. 2
- line 37: // -1 (cool) .. 1 (warm)
- line 66: // the tint: warm by default, cool for the cinematic type; Warmth moves it
- line 70: // 0 = cool, 1 = warm
- line 83: // angle bins: the rays near each angle, so each pixel looks at one or two rays only
- line 112: // the light added at a pixel (each channel 0 .. 1 after clamping)
- line 157: // a six-sided disc
- line 194: // namespace KrimbleLensFlare

## `plugins/filters/colorsfilters/colorsfilters.cpp`

- line 99: // Krimble: on an RGB picture, one stretch for R, G and B together (KrimbleAutoAdjust): more contrast, no color shift.

## `plugins/filters/colorsfilters/kis_perchannel_filter.cpp`

- line 58: // Krimble: an Auto button beside Reset. It works out a curve for red, green and blue from the picture
- line 59: // (the same as Image > Auto Color, without its saturation lift: KrimbleAutoAdjust.h) and puts the curves here,

## `plugins/filters/fastcolortransfer/kis_wdg_fastcolortransfer.h`

- line 18: /**
  * Krimble: reworked into the industry standard's Match Color dialog shape -- pick a
  * source from currently open documents (not a file loaded from disk, which
  * is what this used to require), a layer within it (or Merged), selection-
  * scoping checkboxes, Neutralize, and Luminance/Color Intensity/Fade
  * sliders. The underlying statistical color-transfer math is unchanged
  * (see fastcolortransfer.cpp) -- this only changes how the reference
  * statistics get gathered and how strongly the result gets applied.
  */

## `plugins/filters/levelfilter/KisAutoLevelsFilters.cpp`

- line 58: // Krimble: the curves are now worked out by KrimbleAutoAdjust (Auto Tone: each channel stretched on its own;

## `plugins/filters/levelfilter/KrimbleAutoAdjust.h`

- line 41: // 16-bit values / 64
- line 43: // the 16-bit RGBA layout of Krita (blue first)
- line 47: // the value (0..1) under which 'fraction' of the counted values lie
- line 80: // The curve of each channel (R, G, B): y = ((x - black) / (white - black)) ^ power, clipped to 0..1.
- line 91: // 1. histograms
- line 109: // 2. black and white point of each channel
- line 121: // the dark and light colors: the average color of the darkest and of the lightest 0.5% of the pixels
- line 133: // otherwise the 0.1% points of the channel stay
- line 137: // a flat channel is left alone
- line 141: // 3. Auto Color: the near-neutral midtones made neutral, at their own average brightness
- line 186: // 4. the curves
- line 196: // 5. apply (with the saturation lift for Auto Color)
- line 213: // dull colors get more, bright colors less
- line 225: // namespace KrimbleAutoAdjust

## `plugins/impex/jpeg/KrimbleSmartCompression.h`

- line 2: // Krimble 2026-11-12 (task 24a): SmartCompression: the Smart Selection and the level.
- line 3: //
- line 4: // 1. Smart Selection (detailMask): a first protected area made from the picture itself: the 8 x 8 pixel blocks that carry detail (edges, text, texture) are protected, the flat
- line 5: //    ones (sky, a plain panel, an empty background) are not. George: "But you start with the smart selection as the mask." The user then edits it (the red film).
- line 6: // 2. The level (requantize): the JPEG is made as usual at the user's quality. Then every 8 x 8 block that is NOT protected gets coarser: its AC coefficients are rounded to multiples
- line 7: //    of a step that grows with the level and with the frequency (the finest detail goes first), with a dead zone (small values become 0). The protected blocks are not touched
- line 8: //    (their coefficients are copied as they are). The result is an ordinary JPEG (the same quantization tables, the huffman tables are optimized again): it opens everywhere.
- line 9: //    No pixels are decoded or encoded again, so there is no second loss.
- line 32: // jpeg: a JPEG file in memory. mask: width * height bytes (above 127 = protected). level: 0..100 (0 = nothing is changed). Returns false on an error (out is then not valid).
- line 54: // the exif, the profile, the xmp... (APP0, the JFIF header, is written again by the writer)
- line 58: // integral image of the mask: how many protected pixels are in any rectangle
- line 67: // [x0, x1) x [y0, y1), clamped to the picture
- line 78: // how many picture pixels one pixel of this component covers
- line 80: // the colors take more than the brightness
- line 90: // any protected pixel under the block: the block is left as it is
- line 92: // k = 0 is the block's average (DC): left alone
- line 96: // dead zone: values below 60 percent of a step become 0
- line 112: // the writer makes its own Adobe marker for a CMYK picture; the one that was read is not written again
- line 126: // namespace KrimbleSmart

## `plugins/impex/jpeg/KrimbleSmartDetail.h`

- line 2: // Krimble 2026-11-13 (task 52): the Smart Selection's analysis, on its own (no JPEG library needed), so that the SmartCompression panel and the JPEG writer use the same code.
- line 3: // The 8 x 8 pixel blocks that carry detail (edges, text, texture) are protected; the flat ones (sky, a plain panel, an empty background) are not.
- line 14: // lum: width * height luminance bytes. sensitivity 0..100 (more = more is protected). Returns width * height bytes: 255 = protected, 0 = not.
- line 35: // the threshold: 50 percent sensitivity protects blocks whose average step to the neighbour pixels is more than 6 gray levels; 100 percent more than 1.5; 0 percent more than 24
- line 40: // a block next to a detailed block is protected too (an edge's ringing reaches into the next block)
- line 57: // namespace KrimbleSmart

## `plugins/impex/jpeg/KrimbleSmartPreview.h`

- line 2: // Krimble 2026-11-13 (task 55): the SmartCompression panel's preview. Makes a JPEG of a picture in memory (the same settings as the save: quality, 1x1 color, gray, progressive) and reads a JPEG back, so the
- line 3: // panel can show how large the result will be and what it looks like, without writing a file. Only libjpeg.
- line 16: // rgb: width * height * 3 bytes (R, G, B). Returns false on an error.
- line 45: // 2x2 (smallest) or 1x1 (best), like the Advanced tab
- line 70: // Reads a JPEG into 3 bytes per pixel (R, G, B; a gray JPEG becomes gray RGB).
- line 99: // namespace KrimbleSmart

## `plugins/impex/jpeg/kis_jpeg_converter.cc`

- line 731: // Krimble 2026-11-13 (task 49): George (build 57): "SmartCompression logo is too big. Too much explanation in the panel. Doesn't look like XAT. Jpeg save dialog is enormous."

## `plugins/tools/basictools/KrimbleEmbeddedPreset.h`

- line 26: // Returns the embedded preset for the given brush engine id ("paintbrush", "colorsmudge",
- line 27: // "filter", ...), or a null pointer if it cannot be loaded. Each call returns a new copy.

## `plugins/tools/basictools/kis_tool_brush.cc`

- line 82: /* Krimble 2026-11-13 (task 77) */
- line 625: // Krimble 2026-11-22: the panel jumped when Snap to Assistants was checked (the rows below it appeared). The rows always show and are
- line 626: // greyed out until it is checked, so nothing moves (George: "The panel shifts around when the check box is checked.").

## `plugins/tools/svgtexttool/KrimbleTextDialog.cpp`

- line 1: /*
  * Krimble 2026-10-24: see KrimbleTextDialog.h.
  */
- line 43: // four lines of text, left, centered, right or both edges
- line 64: // an eyedropper, drawn
- line 94: // the preview: a widget that paints itself, so that it always uses its present width
- line 102: // painter, width, height
- line 139: // namespace
- line 158: // a phone held sideways is only about 340 px high: the Text tab is then in two columns (the words and the preview on the left, the controls on the right)
- line 161: // for the bench
- line 167: // the title: a T badge, the name, a close button
- line 194: // sideways there is no room for the title row (the Cancel button closes the window)
- line 205: // every page of the tabs scrolls, so that the window also fits a phone held sideways (about 340 px high)
- line 215: // ---- the Text tab: everything for ordinary use, on one page ----
- line 223: // (1.0.42: the typing box was given the color emoji fonts as extra families, and the space between words came out with the wide space of one of them: "Jezzy     english".
- line 224: // The box now uses the font as it is; the system's own fallback draws an emoji.)
- line 226: // tp->addWidget(m_edit);
- line 227: // tp->addWidget(separator(textPage));
- line 228: // (the words, the controls and the preview are put in the page after they are made, below)
- line 239: // Font + favorite star
- line 242: // every family except the phone's own script and system fonts (as before: Noto, SEC, New SEC, Samsung, AndroidClock); each drawn in its own font
- line 259: // not as wide as the longest family name
- line 271: // Size, Color (swatch, recent colors in its menu), eyedropper (the foreground color)
- line 288: // grid->addWidget(new QLabel(i18n("Size"), textPage), row, 0);
- line 289: // grid->addWidget(m_sizeBox, row, 1);
- line 290: // grid->addWidget(new QLabel(i18n("Color"), textPage), row, 2);
- line 291: // grid->addWidget(m_colorButton, row, 3);
- line 292: // grid->addWidget(m_dropperButton, row, 4);
- line 293: // On the phone (about 340 px wide) Size and Color in one row made the window wider than the screen; they are two rows (George's picture has them in one).
- line 303: // B I U S Aa
- line 336: // Alignment
- line 359: // Spacing: letters, lines; Kerning at the cursor
- line 397: // the preview
- line 422: // not in a scroll area of its own: the right column scrolls
- line 426: // ---- the Font tab: a long list of the fonts, searched, with the favorites ----
- line 449: // ---- the Style tab: colors ----
- line 466: // swatches->addWidget(swatch, i / 6, i % 6);
- line 467: // four a row: six made the window wider than a phone
- line 484: // ---- the Layout tab: the text box ----
- line 503: // ---- the buttons at the bottom ----
- line 533: // ---- the connections ----
- line 535: // the kerning follows the typing: characters typed or deleted move the kerning after them
- line 574: // the height the Text tab wants (the scroll area itself asks for less), but never more than the screen has
- line 717: // between two letters only
- line 723: // between two letters only
- line 740: // Title Case
- line 741: // Sentence case
- line 753: // the same number of characters: the kerning stays where it is
- line 761: // the words in the font, size (at most 40 pt, so that it fits), bold, italic, underline, strike-out, alignment, line spacing and color that are chosen, changing as they are chosen, drawn by
- line 762: // hand so that the tracking and the kerning show: each line is drawn in pieces between kerned letters (the font's own kerning still applies inside a piece). On a dark background when
- line 763: // the color is light, so that it can be seen.
- line 768: // everything the picture needs is taken now (not at the time of painting)
- line 799: // one pass measures the widest line, the second draws; a line wider than the preview makes the whole picture smaller, so that nothing is cut off
- line 814: // text, gap before it (px)

## `plugins/tools/svgtexttool/KrimbleTextDialog.h`

- line 1: /*
  * Krimble 2026-10-24: the window of the Type tool, redesigned after George's picture of what it should be ("Text tool UI sucks. Here's what it is and what it should be.").
  * It depends on Qt widgets and i18n only, so it can be built and looked at on its own (tools/textdlg-bench).
  */
- line 28: // what the window shows and gives back; the Type tool keeps one of these as "what was chosen last time"
- line 31: // pt
- line 36: // 0 left, 1 center, 2 right, 3 justified
- line 37: // pt, for the whole text
- line 38: // percent
- line 40: // pt, an extra gap before the character at each position
- line 41: // document units; 0 = no text box
- line 51: // for the bench: 0 Text, 1 Font, 2 Style, 3 Layout
- line 95: // a KrimblePreviewWidget (KrimbleTextDialog.cpp)

## `plugins/tools/svgtexttool/KrimbleTextTool.cpp`

- line 1: /*
  * Krimble 2026-10-18: see KrimbleTextTool.h.
  */
- line 60: // what was chosen last time is what the window starts with next time
- line 68: // 0 left, 1 center, 2 right (3 justified: Krimble 2026-10-24)
- line 123: // svg += QStringLiteral("<tspan>%1</tspan>").arg(line);
- line 272: // the kerning follows the typing: characters typed or deleted move the kerning after them
- line 304: // the preview: the words in the font, size (at most 40 pt, so that it fits), bold, italic, alignment and color that are chosen, changing as they are chosen. On a dark
- line 305: // background when the color is light, so that it can be seen.
- line 344: // the pieces and the width of the line
- line 345: // text, gap before it (px)
- line 373: // QObject::connect(fontBox, &QFontComboBox::currentFontChanged, &dialog, [&]() { updatePreview(); });
- line 382: // between two letters only
- line 406: // s_family = fontBox->currentFont().family();
- line 487: // the line break added just below comes first
- line 555: // corners: size
- line 556: // sides: width of the text box
- line 603: // a drag, not a shaky tap
- line 623: // a tap inside the frame: the window opens, as it did before the frame was there
- line 637: // the window opens after this event is finished
- line 644: // the bundled fonts are loaded the first time a text tool is chosen
- line 658: // the shape may have been taken away (undo, a deleted layer) since it was chosen
- line 671: // a margin of 40 view pixels, for the handles and the line
- line 676: // which handle (or the inside of the frame) is at the point; the handles are tested in view pixels so that a finger can hit them
- line 697: // the frame while a handle is dragged by delta (document units)
- line 713: // a corner: the text is made bigger or smaller, in proportion; the opposite corner stays
- line 727: // the text is changed to fit the frame that was dragged
- line 741: // the words and the look of the text as it is now (this sets what was chosen last time, s_family and the others, to the text's own)
- line 744: // the fill is not written into the svg: it is the shape's own
- line 749: // a text box of this width (text that had no box gets one)
- line 760: // the edge or corner opposite to the handle stays where it was: the text is moved by what it moved
- line 775: // is there text where the finger was? (the boxes of the shapes, within about 14 view px of the tap)
- line 781: // the top one first
- line 795: // the words and the look are changed on the shape that is there (one undo step)
- line 796: // KUndo2Command *editCommand = new KUndo2Command(i18n("Edit text"));   // compile error (1.0.38 build): the name of an undo command is a KUndo2MagicString
- line 823: // the fill is also set on the shape itself, as the old tool does (properties do not write their fills to the svg)

## `plugins/tools/svgtexttool/KrimbleTextTool.h`

- line 1: /*
  * Krimble 2026-10-18: the new Type tool, designed from scratch (George: "The existing type tool is garbage. I'm letting you design the new one from scratch. Just
  * get rid of this crap version."). Tap the canvas with it and an ordinary window opens: a text box to type in, the font, the size, bold, italic, the alignment and
  * the color. OK puts the text on the picture as vector text (a KoSvgTextShape, in a vector layer). There is no editing on the canvas: no cursor, no handles, and
  * nothing for Android's own text selection controls to fight with. The old tool (SvgTextTool) is no longer registered; its code is left in the plugin, unused.
  */
- line 33: // the frame when a handle was pressed (document coordinates)
- line 34: // the frame while it is dragged
- line 57: // KRIMBLE_TEXT_TOOL_H

## `plugins/tools/svgtexttool/SvgAdjustSelectionHandleStrategy.h`

- line 12: /**
  * Krimble: dragging one of the two mobile-style selection-endpoint handles
  * (see SvgTextCursor::selectionHandlePos/hitTestSelectionHandle) to adjust
  * an existing selection after the fact, moving only the grabbed end and
  * leaving the other end fixed -- unlike SvgSelectTextStrategy, which always
  * starts a brand new selection from a fresh tap.
  */

## `plugins/tools/svgtexttool/SvgTextCursor.h`

- line 149: /**
  * Krimble: mobile-style draggable selection handles, one at each end of
  * the current selection, independently adjustable after the fact (like
  * the "teardrop" handles every phone keyboard shows), separate from the
  * type-setting handles above.
  */

## `plugins/tools/svgtexttool/SvgTextQuickActionBar.h`

- line 9: /**
  * Krimble: a small floating toolbar (Cut/Copy/Paste/Select All) shown near
  * an active text selection, the same pattern every mobile OS text field
  * uses instead of requiring a menu-bar trip for basic clipboard actions.
  * Purely a dumb button strip -- SvgTextTool owns showing/hiding/positioning
  * it and connects its signals to the actual cursor operations.
  */

## `plugins/tools/svgtexttool/SvgTextTool.cpp`

- line 187: // Krimble 2026-11-23: the bundled fonts are loaded the first time a text tool is chosen

## `plugins/tools/svgtexttool/SvgTextTool.h`

- line 135: /**
  * @brief updateQuickActionBar
  * Krimble: shows/hides/repositions the floating Cut/Copy/Paste/Select
  * All bar based on whether there's currently a text selection.
  */

## `plugins/tools/tool_smart_patch/content_aware_fill_plugin.h`

- line 20: /**
  * Krimble: Content-Aware Fill. Registered here rather than as a
  * KisFilter subclass because KisFilter::processImpl() never receives
  * the real KisSelectionSP -- only the final composite-back step does,
  * via the non-virtual, non-overridable KisFilter::process() wrapper --
  * so a KisFilter-based version can't see the actual (possibly
  * irregular) selection shape to inform the inpaint algorithm's own
  * fill/source decisions, only get the correct final clipped output.
  * This action gets the real selection directly from the view manager
  * instead.
  */

## `plugins/tools/tool_smart_patch/krimble_fill.cpp`

- line 2: // Krimble 2026-10-19: moved here from content_aware_fill_plugin.cpp (krimbleContentAwareFill, 2026-10-18 and 19) so that the healing brush can use it too,
- line 3: // with a choice of method (George: "Make the different types options in the tool options panel.").
- line 74: // the selection is an 8-bit alpha device: one byte per pixel
- line 96: // the result, in the colour space of the layer, and only the replaced pixels copied

## `plugins/tools/tool_smart_patch/krimble_fill.h`

- line 2: // Krimble 2026-10-19: the fill methods shared by Edit > Fill > Content-Aware... and the healing brush (Smart Patch tool), and the method chosen in the
- line 3: // healing brush's Tool Options (saved, so the menu command uses the same one).
- line 14: // Krita's own Smart Patch fill
- line 17: // the method chosen in the Tool Options (Sharp when none was chosen yet)
- line 20: // Fills the pixels of imageDev that maskDev (an 8-bit alpha device, > 127 = fill) covers, with Sharp or Smooth, on the part of the picture around them
- line 21: // (inside `bounds`). Returns false when it cannot (nothing to fill, a hole too big for the memory, the picture too small, or method Classic): the caller
- line 22: // then uses Krita's fill.

## `plugins/tools/tool_smart_patch/krimble_inpaint.cpp`

- line 18: // the picture: known pixels are the truth, hole pixels are the current estimate
- line 19: // 1 = hole
- line 20: // summed-area table of the hole, (w+1) * (h+1)
- line 21: // the nearest-neighbour field: for each pixel, the centre of the source patch
- line 22: // the mean squared difference of the match
- line 23: // 1 = this pixel's patch overlaps the hole: it has a match to find
- line 24: // centres of the valid source patches (fully inside the picture and fully known), as y * w + x
- line 25: // the window the sources may be taken from (around the hole): inclusive centre coordinates
- line 26: // the picture split in two: the smooth colour (a box blur) and the detail (the picture minus the smooth colour)
- line 27: // the smooth colour of the hole: what the surroundings of the hole say it should be (re-solved at every level)
- line 30: // runs f(a, b) on `threads` pieces of [0, n)
- line 58: // inclusive rectangle, inside the picture
- line 71: // squared difference between the patch around (x, y) (clamped at the borders) and the source patch around (sx, sy); stops when it passes `cutoff`
- line 72: // how much the difference of the smooth colours of two patches counts next to the difference of their detail
- line 73: // the weight of a pixel inside a voting patch falls off with its distance from the centre of the patch (in pixels)
- line 74: // the detail of the hole may be raised at most this many times to match the detail around it
- line 75: // the source window reaches this many times the shorter side of the hole beyond the hole
- line 76: // the sharpness of the vote on the coarse levels (the fine levels use Options::sharpness)
- line 77: // the radius of the box blur that separates the smooth colour from the detail
- line 78: // the smooth colour of the hole is relaxed this many times at every level
- line 80: // the picture split in the smooth colour (a box blur of radius g_blurRadius) and the detail
- line 89: // horizontal running sum with the border repeated
- line 112: // Seamless correction (membrane interpolation, as in seamless cloning): the estimate that came from the level above is kept, with all its detail, but its mismatch with the
- line 113: // known pixels at the edge of the hole is measured and spread smoothly into the hole (a Laplace solution), and subtracted. The colours at the edge are then right, and the
- line 114: // middle of a big hole keeps what the coarser level made of it.
- line 167: // replaced by correctBoundary(); kept for reference
- line 168: // the hole is relaxed toward the smooth solution (Laplace) with the known pixels of THIS level as the edge (not used any more: correctBoundary keeps the detail)
- line 201: // The difference of the DETAIL of the two patches (the picture minus its smooth colour), plus, with a small weight, the difference of their smooth colour at the centre. The detail
- line 202: // does not depend on the colour, so a patch of dark texture matches a patch of bright texture; the colour of the fill comes from the surroundings of the hole, not from the source.
- line 227: // the first fill of the smallest level: the hole is filled from its edge inward with the mean of the neighbours already filled
- line 262: // exemplar-based first stage (Criminisi et al. 2004): tried, copies wrong chunks on big holes; kept for reference
- line 263: // Exemplar-based filling (Criminisi, Perez, Toyama 2004): the hole is filled from its edge inward, one patch at a time. At each step the pixel of the edge with the highest priority
- line 264: // (many known pixels around it, and an image edge that runs into the hole) is chosen, its patch is matched against the known picture using only the pixels that are known, and the
- line 265: // unknown pixels are copied from the best match. Real pixels are copied, never averaged, so the result is sharp and the edges continue.
- line 278: // the pixel of the edge with the highest priority
- line 284: // not on the edge
- line 285: // confidence: the mean confidence of the patch
- line 294: // the gradient where the pixel and its 8 neighbours are all known
- line 305: // the direction of the edge of the hole (the normal): from the known mask
- line 311: // the isophote is perpendicular to the gradient; D = |isophote . normal|
- line 315: // temporary: for tests
- line 321: // the known pixels of the patch (these decide the match)
- line 330: // the best match among the sources
- line 359: // The smooth first guess: the hole is the solution of the Laplace equation with the known pixels around it as the boundary (every hole pixel is the mean of its four neighbours).
- line 360: // That is what a stretched rubber sheet does: the colours at the nearest edges count the most, so a long thin hole is not averaged over its whole length.
- line 363: // a first guess to start from
- line 372: // Gauss-Seidel with over-relaxation, in both directions of the sweep
- line 388: // successive over-relaxation
- line 401: // the window the sources are taken from: the box of the hole, grown by `margin`. Far-away patches that happen to have a similar colour are what makes a bad fill.
- line 409: // a pixel is a target when a patch centred on it touches the hole
- line 445: // one PatchMatch pass over the targets: propagation from the two neighbours already visited, then a random search of shrinking size
- line 446: // one PatchMatch pass over the rows [rowA, rowB) of the level; several bands run at the same time (a band reads the matches of the neighbouring band while they are being
- line 447: // changed: that is how PatchMatch is usually parallelised, a stale value only costs a missed chance)
- line 457: // propagation
- line 467: // random search around the best so far
- line 493: // the vote: every pixel of the hole takes the weighted mean of what the matches of all the patches that cover it say it should be
- line 498: // the spread of the weights: from the distribution of the match distances
- line 505: // the weight of every match, and the weight of a pixel inside a patch (the centre counts more)
- line 514: // Gathering: the pixel t asks every patch that covers it. The pixels are independent of each other, so the rows can be done at the same time.
- line 515: // the voted detail
- line 516: // the energy of the detail of the copied pixels, before the averaging
- line 548: // Averaging many patches lowers the contrast of the detail (opposite strokes cancel). The energy of the detail of the copied pixels BEFORE they are averaged is compared with the
- line 549: // energy of the average, and the detail of the hole is raised by the typical ratio (the median, so that no single pixel decides), up to g_gainMax. It can only give back what the copied
- line 550: // patches had: a smooth source stays smooth, and nothing is invented from the strong edges around the hole.
- line 565: // the smooth colour of the guide plus the voted detail
- line 612: // namespace
- line 627: // level 0: the picture and the hole (grown by `expand` pixels)
- line 658: // the number of levels: the hole must be small (about 12 px across) at the smallest one, and the picture big enough to find patches in
- line 664: // from the smallest level to the full size
- line 669: // filled below, by exemplarFill, once the sources are known
- line 671: // the estimate of the level above, enlarged, fills the hole here; the known pixels stay the truth
- line 680: // the margin of the source window, in pixels of this level: 1.5 times the size of the hole, at least 40 px at full size
- line 681: // (the shorter side of the hole decides, so that a long thin hole, a line of text for instance, does not search half the picture)
- line 686: // the first estimate: the smooth (Laplace) solution, or the exemplar fill
- line 689: // what the level above made of the hole, with its detail, made to meet the edge of this level
- line 692: // the smooth colour of the estimate: the detail is voted on top of it
- line 715: // Seed the detail: one vote with equal weights, from the matches as they came (random on the smallest level, the matches of the level above on the others). Without it the estimate
- line 716: // has no detail, the matching finds only smooth patches for it, and the fill stays a smooth blur.
- line 729: // the matches were measured against the old estimate: measure again against the one the vote makes
- line 740: // write the hole back (only the pixels the caller asked for, not the pixels of the grown edge unless they were in the hole)
- line 756: // w * h * c
- line 757: // w * h, 1 = to be filled
- line 758: // w * h, 1 = a patch centred here lies wholly in the known picture (a valid source)
- line 759: // w * h, 1 = the patch centred here touches the hole (a target)
- line 760: // the match of every target
- line 761: // its distance
- line 762: // the valid sources, as indices
- line 763: // w * h: how much a pixel counts when patches are compared (known pixels 1; the guessed hole less)
- line 764: // w * h: index into the transform table
- line 765: // w * h * c
- line 766: // w * h: Chebyshev distance to the nearest hole pixel or the edge of the picture
- line 767: // (w + 1) * (h + 1) * c: sums of the picture (whole levels), for the mean colour of any window (the colour shift)
- line 773: // integral image of the hole, to test whole windows at once
- line 782: // inclusive, clamped
- line 802: // how much the guessed hole counts in the comparisons: little while the guess is rough (the known picture around the hole decides what is copied), more as it improves
- line 808: // the distance between the patch around target (tx, ty) and the patch around source (sx, sy); pixels of the target patch outside the picture are skipped
- line 853: // one PatchMatch pass over rows [ya, yb): propagation from the two neighbours already visited, then a random search of shrinking size around the current match
- line 886: // The hole is rebuilt from the matches. `winner` false: every pixel is the weighted mean of what the patches that cover it say (weights from the match quality); used while the
- line 887: // matches are still being found. `winner` true: every pixel is taken from the one covering patch with the best match (sharp; the final rebuild).
- line 913: // the best match wins; a patch centred nearer the pixel wins a tie
- line 929: // The final rebuild with the seam taken away. Every hole pixel is taken from its best covering patch (real pixels), and the same patches also predict the known pixels
- line 930: // in a thin ring around the hole. Where a prediction differs from the real pixel (a copied piece is a little lighter, darker or of another hue than this spot), that
- line 931: // difference is spread smoothly into the hole (a membrane, as in seamless cloning) and added, so the copied pixels meet the picture with no visible edge. Only a smooth
- line 932: // colour shift is added: the copied detail stays as sharp as it was.
- line 942: // the hole and the ring of known pixels whose patches touch it
- line 952: // ring pixels (known): only patches CENTRED IN THE HOLE count. A patch centred on a known pixel near the hole is matched mostly by its known part
- line 953: // and predicts the ring almost exactly, which would hide the seam; the patches centred in the hole are the ones whose pixels were copied.
- line 966: // the offset: fixed on the ring (real - predicted), unknown in the hole
- line 983: // the hole pixels take their best patch
- line 986: // the membrane: first an onion-peel guess, then over-relaxation (each hole value toward the mean of its four neighbours)
- line 1024: // the outer edge of the picture: free (no pull)
- line 1068: // the first guess at the smallest size: the hole is filled from its edge inward, each pixel the mean of its neighbours that are already known or filled (onion peel)
- line 1099: // namespace
- line 1135: // the levels: down until the hole is at most about 24 px across (or the picture gets too small to find patches in)
- line 1157: // the matches of the smaller size, carried up (doubled), each checked; the hole is then rebuilt from THIS size's own pixels
- line 1174: // a first rebuild of the hole from the carried matches (the distances are not known yet: all equal)
- line 1179: // the guessed hole counts little at the smallest size (its first guess is only a peeled average), more on the bigger sizes (their guess came from real patches)
- line 1205: // source offset = [a b; c d] * patch offset; reach: how far the patch reaches; cost: the penalty factor
- line 1228: // index 0 is the plain copy
- line 1231: // the distance (Chebyshev) of every pixel to the nearest hole pixel or the edge of the picture: a transformed source patch must lie wholly in the known picture
- line 1241: // the edge of the picture
- line 1281: // the value the match of target q gives to the pixel at offset (dx, dy) from q (the transformed source, plus the colour shift). `nearest`: the source pixel nearest
- line 1282: // to the transformed spot, not a blend of four (the final rebuild: a blend of four neighbours softens every turned or resized copy)
- line 1301: // the largest colour shift, per channel (of 255)
- line 1302: // what a shift costs, next to the difference it removes
- line 1304: // the sums of the picture, so that the mean colour of any window takes four reads (rebuilt whenever the hole is rebuilt)
- line 1308: // 32-bit integers (4 bytes a channel): at most 255 * 4 M pixels, below 2^31
- line 1329: // the distance between the target patch around (tx, ty) and the source patch at (sx, sy) copied with transform `ti`, after the colour shift (returned in `bias`):
- line 1330: // the shift is the difference of the two patches' mean colours (the source's over its window at its own size), limited to kMaxBias; the patches are then
- line 1331: // compared pixel by pixel (stopping early once past `cutoff`), and a large shift costs a little. Each transform costs its factor.
- line 1391: // propagation: the neighbour's match, moved by one step in the neighbour's transform
- line 1403: // random search: around the current match, shrinking, with the same transform; and a few other transforms at the current spot
- line 1428: // the picture (the hole) has changed since the last sums
- line 1441: // the hole rebuilt from the matches: the weighted mean of what the covering patches say (`winner` false), or the value of the one best covering patch (`winner` true)
- line 1483: // the final rebuild (each hole pixel from its best covering patch) with the seam taken away (see pFinalSeamless: the same, with the transformed, colour-shifted patches)
- line 1608: // namespace
- line 1728: // w * h * c
- line 1729: // 1 = to be filled
- line 1730: // 1 = the patch centred here touches the hole
- line 1731: // the match of every target
- line 1732: // its distance
- line 1733: // its transform (index into the table)
- line 1734: // its colour shift, per channel (w * h * c)
- line 1735: // Chebyshev distance to the nearest hole pixel or the picture's edge (capped at 65535)
- line 1736: // window sums of the picture ((w + 1) * (h + 1) * c)
- line 1737: // how much a hole pixel counts in the comparisons (known pixels count 1)
- line 1738: // how many valid sources there are (plain copies)
- line 1974: // the hole rebuilt as the weighted mean of what the covering patches say; only the hole's pixels are kept aside (not a copy of the whole picture)
- line 2013: // the final rebuild with the seam taken away (see qFinalSeamless), worked only inside the hole's box (and the ring around it)
- line 2024: // index in the box
- line 2093: // box indices of the hole's pixels, by colour
- line 2184: // a random valid source (a plain copy), found by trying random spots
- line 2206: // the patch used for matching
- line 2209: // the layer: hole pixels with a known 4-neighbour (if none has one, the nearest remaining pixel is taken so that the loop ends)
- line 2235: // clearly worse, stop early
- line 2245: // the matches of the filled neighbours, shifted by the offset between the pixels
- line 2253: // (a version that took half of the candidates from near the hole, as the industry standard's sampling window does, was tried and made the fill hazier on the cup and spoon
- line 2254: // pictures of tools/inpaint-bench: the nearby colours pull the match towards a smooth look; not used.)
- line 2266: // take the layer in: copy the middle pixel of the best patch
- line 2278: // namespace
- line 2283: // the 16-bit match positions
- line 2345: // while (levels < std::max(opt.maxLevels, 10) && (holeDim >> (levels - 1)) > 24 && (std::min(w, h) >> levels) >= 8 * (2 * r + 1)) ++levels;
- line 2361: // a first guess for every pixel, replaced where the ring fill finds a match
- line 2390: // the smaller size is no longer needed: its memory goes back
- line 2408: // not needed by the bigger size
- line 2420: // namespace KrimbleInpaint

## `plugins/tools/tool_smart_patch/krimble_inpaint.h`

- line 2: // Krimble content-aware fill: a multi-scale PatchMatch inpainting engine (no Qt, no Krita types: plain pixel buffers, so it can be tested on its own).
- line 10: // patches are (2r+1) x (2r+1): 7 x 7
- line 11: // the hole is grown by this many pixels (halos of the selection edge), at full size
- line 12: // at most this many pyramid levels
- line 13: // random seed (the same input gives the same result)
- line 14: // 0 = as many as the device has (Krimble 2026-10-20: no longer at most 4)
- line 15: // called now and then with 0..1; return false to stop (inpaint() then returns false and changes nothing)
- line 16: // higher = sharper and more "copied" (a few sources win), lower = smoother (many sources are averaged)
- line 19: // pixels: w * h * channels, 8 bit, interleaved (RGB or RGBA); hole: w * h, non-zero = to be filled.
- line 20: // The hole pixels of `pixels` are replaced. Returns false when there is nothing to copy from (the whole picture is hole) or on bad arguments.
- line 21: //
- line 22: // filledHole (optional, w * h): set to 1 for every pixel that was replaced (the hole, grown by Options::expand), so that the caller can write back only those.

## `plugins/tools/tool_transform2/kis_tool_transform_config_widget.cpp`

- line 624: // if (QLabel *label = window->findChild<QLabel *>(QStringLiteral("krimbleToolOptionsName"))) label->setText(title);
- line 657: // if (m_krimbleRotateButton && KoToolManager::instance()->activeToolId() == QLatin1String("KisToolFreeRotate")) {
- line 658: //     m_krimbleRotateButton->setChecked(true);
- line 1584: // Krimble: the mode in use, for the toolbox's Transform button (its picture) and its flyout (the row in use)

## `plugins/tools/tool_transform2/kis_tool_transform_config_widget.h`

- line 141: // a mode chosen in the toolbox flyout (qApp property krimbleTransformModeRequest)

## `tools/batch-bench/engine_test.cpp`

- line 1: // Runs every built-in recipe of KrimbleBatch on the pictures given and writes the results; prints what came out.

## `tools/batch-bench/script_test.cpp`

- line 1: // Tests Krimble Script: every command, the errors, and that every built-in recipe written as a script means the same as the recipe.

## `tools/cmyk-engine-test/cmyk_engine_test.cpp`

- line 1: // Krimble 2026-11-11: a standalone check of the CMYK engine (libs/ui/KrimbleCmykEngine.h). Needs only Qt5 Gui (apt-get install -y qtbase5-dev).

## `tools/help-bench/main.cpp`

- line 1: // Draws the Krimble Help window offscreen, as a phone would show it, and writes pictures of it.
- line 2: // Usage: help_dialog OUTDIR WIDTH HEIGHT krimble-help.md [part to go to] [word to find]

## `tools/log-bench/main.cpp`

- line 1: // Draws the Krimble Log window offscreen. Usage: log_dialog OUT.png WIDTH HEIGHT krita.log ["extra text"]

## `tools/textdlg-bench/klocalizedstring.h`

- line 1: // A stand-in for KDE's i18n(), only so that KrimbleTextDialog.cpp can be built here without KDE.

## `tools/textdlg-bench/main.cpp`

- line 1: // Builds the Type tool window (KrimbleTextDialog) on its own, with a dark palette like Krimble's, and saves pictures of its tabs.
