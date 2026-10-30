# Krimble — Roadmap Notes

Status: **planning only** — not started. Captured here so they don't stay stuck in chat.

## George's priorities (2026-10-26, answering the "wish we could do it" list)

- **AI upscaling: a separate app is a MUST** ("I think a separate upscaler is a must."). The "Krimble Upscaler" app, not a feature inside Krimble. Not started.
- **Selective compression (Advanced JPEG Export): "would be awesome".** Wanted. Not started.
- **Quick Mask: wanted** ("in Photoshop it's very useful"; George was not sure how it works now). What exists now: **Select > Show Global Selection Mask** (and Add New Selection Mask / Convert to Selection Mask): the selection is shown as an ordinary grey mask *layer* in the Layers panel that can be painted on; there is no Photoshop-style toggle with a red overlay and a Q key. See the Quick Mask section below. Not started.
- **Fonts (browser, add your own, curated set): "future must".**
- **Auto Tone / Auto Contrast / Auto Color on curves:** George wrote "There are ways to edit images with curves. I would assume these" and the message ended there: waiting for the rest.
- **The small things: "Do all the small things."** Magnetic Selection bar (done, 320th entry), Match Color's exact selection shape and its two selection options, a Channels panel tap to load a channel as a selection, styling part of a text, Krimble's own smaller text-selection droplets (waits for the log numbers, 318th entry), Google Play (an app bundle, a closed test of 12 testers for 14 days; process, not code).
- **Linux build next** (George: "Next we should build for Linux. It's already part of the process."). The repository has Krita's own Linux packaging (`packaging/linux/appimage`, `build-tools/ci-scripts/build-appimage.py`, `.gitlab-ci.yml`); our server is ARM64, so a Linux x86_64 AppImage needs an x86_64 builder (a free GitHub Actions runner, since the policy is against spending money, not against Actions). Not started; the six tools hidden on the phone are shown by a desktop build (see "New Tools").
- **Android ABIs:** George asked: "we have the 8a builds for Android, but no 7a?" (arm64-v8a is built; armeabi-v7a, 32-bit, is not: `BUILD_ANDROID.md`, "Target ABI: arm64-v8a"). A 32-bit build needs its own dependency build, has about 3 GB of address space (the low-memory kills would be worse) and Google Play only requires 64-bit. **Decided 2026-10-26 (George, after the market size was looked up: roughly 5 to 10 percent of active devices run 32-bit ARM, mostly cheap low-RAM phones): "Okay, 7a is off the map."** No 32-bit build. Google Play Console ("Reach and devices") will show the real numbers for Krimble's own audience once it is published.

## Advanced JPEG Export Module

Goal: selective/regional compression instead of one global quality slider — get
files that look uncompressed at meaningfully smaller sizes, the way tools like
Guetzli, MozJPEG (trellis quantization), and XAT-style optimizers do.

**Core mechanism:** build a perceptual quality map across the image instead of
applying one quality level everywhere:

- **Edge/detail-heavy regions** (text, logos, sharp transitions) -> higher
  quality/lower compression -- artifacts are most visible here.
- **Flat/gradient regions** (backgrounds, sky, smooth fills) -> deceptive:
  hide detail loss well, but reveal banding easily, so these need careful
  handling rather than just "compress harder because there's nothing there."
- **Texture-heavy/busy regions** -> most tolerant of aggressive compression.

Quality map can be built from Sobel/Laplacian edge detection or local
variance per block; ideally validated with a perceptual metric (SSIM or
butteraugli) comparing candidate compression levels against the source and
picking the lowest quality that stays under a visible-difference threshold,
per region.

**Grayscale/B&W-specific behavior:** B&W images should get a more aggressive
default compression curve than color images. JPEG normally spends a large
share of its bit budget on chroma subsampling, which a grayscale image
doesn't need at all -- every bit goes to luminance/edge fidelity instead.
Combined with the regional map above, flat B&W regions (line art, text,
high-contrast logo/mascot work) can take much heavier quantization than
their color equivalents before anything perceptible changes.

**Implementation notes:** likely built on libjpeg-turbo, either via per-region
quantization table manipulation if exposed directly, or a tile-based
export-and-reassemble approach if not.

## New Tools

- **Desktop builds (Linux, Windows, macOS): show the six tools hidden on the phone** -- decided 2026-10-26 (George: "When we build the desktop version for Linux, Windows, and MacOS we will enable those. For mobile, comment them out."). On Android the toolbox has no button for: Colorize Mask Editing, Enclose and Fill, Assistant, Ruler (Measure), Reference Images and Path-Based Selection (the list `krimbleHiddenOnMobile` in `libs/ui/toolbox/KoToolBox.cpp`, inside `#ifdef Q_OS_ANDROID`: a desktop build shows them with no change). The tools themselves are still built and registered.
- **Smudge tool** -- ✅ shipped (commit `7390d6b`). Dedicated toolbox tool
  wrapping Krita's existing colorsmudge paintop engine, own icon/shortcut/
  toolbox slot, auto-loads the "smudge" preset on activation.
- **Eraser tool** -- ✅ written 2026-10-25 (290th entry; not compiled or tried on a phone yet). Dedicated toolbox tool after Soften: the Brush tool with the eraser switch turned on while it is active; it paints transparency with the current brush. Pencil, Path Selection and Clone Stamp icons are still waiting in `krita/pics/tools/krimble-future-icons`.
- **Soften tool** -- not started. Photoshop's Blur tool equivalent: a
  localized blur/softening brush for softening edges/skin/detail in specific
  areas without running a full-image filter. Krita's `filterop` paintop
  engine (paint-with-any-filter) already exists and can drive this --
  the work is presetting it to Gaussian Blur with sane defaults and giving
  it its own toolbox entry, same pattern as Smudge.

## AI Upscaling Tool

Status: **not started**. Chosen approach below, after evaluating licensing.

**Model source: OpenCV's `dnn_superres` module** (opencv_contrib, Apache 2.0
license as of OpenCV 4.5+). Official project (built via Google Summer of
Code), not a personal/research repo -- both the module code and its
pretrained models are covered under OpenCV's own license terms. This was
chosen specifically because most independent FSRCNN/ESPCN reimplementations
found elsewhere have no stated license at all, making them unusable as-is.

Four models are available, covering a real quality/speed/size spectrum:

| Model | Size | Speed | Quality |
|---|---|---|---|
| ESPCN | ~100 KB | Fastest | Lowest of the four, still well above bicubic |
| FSRCNN | Small (light-weight, "-s" variant even smaller) | Fast | Decent, good speed/quality balance |
| LapSRN | Medium | Medium | Multi-scale (progressive 2x/4x/8x via Laplacian pyramid) |
| EDSR | ~38.5 MB quantized (150 MB original) | Slowest | Highest quality of the four |

**Target approach (matches the "Samsung Gallery" bar discussed):** don't
depend on the full OpenCV library on-device. Instead, take the ESPCN or
FSRCNN pretrained `.pb` model out of `dnn_superres`, convert it to
`.tflite`, and run it through TensorFlow Lite's **NNAPI delegate** --
this is what lets inference actually use the phone's NPU/DSP/GPU instead of
running CPU-bound, which is the real reason Samsung Gallery's on-device
upscaling feels fast. EDSR/LapSRN are logged here for future
higher-quality/slower options (e.g. an explicit "high quality, takes
longer" mode) but ESPCN/FSRCNN are the realistic default for a responsive
in-app tool.

**Still needed before this is buildable:**
- Add TensorFlow Lite to the deps toolchain (nothing ML-related is currently
  linked at all -- confirmed via full search of CMakeLists and
  krita-deps-management).
- Convert the chosen `.pb` model to `.tflite` and verify NNAPI delegate
  support for its specific ops.
- Tiling pipeline for images larger than the model's expected input size.
- Tool/dialog UI.

**Separately evaluated, not chosen as the primary model:** Real-ESRGAN
(xinntao/Real-ESRGAN, BSD-3-Clause, confirmed clean license) -- GAN-based,
better detail hallucination than the dnn_superres models, but heavier/slower
and a bigger integration lift. Worth revisiting later as a higher-quality
optional mode once the ESPCN/FSRCNN path is working.

## Quick Mask

Status: **not started** -- confirmed genuinely missing (searched the entire
codebase under every naming variant: quickmask, quick_mask, QuickMask --
zero hits). Not a relocation/UI-parity case like Channels or Select Opaque;
this needs building from scratch.

**What it is:** paint a selection into existence instead of using
marquee/lasso/wand. Toggle on (Q key in Photoshop) -> canvas shows a
semi-transparent red overlay (rubylith-style) over everything NOT selected
-> paint in black/white/gray with any normal brush (black removes from
selection, white adds, gray = partial/feathered) -> toggle off -> the
painted result becomes a real active selection.

**Closest existing Krita infrastructure to build on:**
- Krita's Selection Masks (grayscale mask layers) are conceptually the
  same underlying data model a Quick Mask needs -- a paintable grayscale
  layer that converts to/from a real selection.
- The Colorize Mask tool (libs/image/lazybrush/kis_colorize_mask.cpp)
  proves the "special temporary paintable overlay tied to a toggle state"
  pattern already exists in this codebase, just for a different purpose.

**Rough implementation shape:**
1. Toggle action that creates a temporary paintable grayscale device
   representing the mask, seeded from the current selection (or fully
   selected if none exists).
2. Canvas rendering: composite that grayscale device as a semi-transparent
   red overlay over the normal image while active (inverse of the
   grayscale value = overlay opacity, matching the rubylith convention).
3. While active, redirect normal paint tools to paint into the mask device
   instead of the image (grayscale in, not full color).
4. On toggle-off: convert the grayscale mask device into a real
   KisSelection (threshold/gradient -> selection, feathered where gray),
   apply it as the active selection, discard the temporary device.
5. Toolbox button (bottom of toolbox, alongside the FG/BG swap widget and
   Screen Mode) + Q shortcut, matching Photoshop's actual entry points.

Likely a genuinely medium-sized feature -- more than a UI wrapper, less
than the AI upscaler. Worth scoping properly (time estimate, step-by-step
plan) when it's next in line for actual implementation.

## Gradient tool: synchronous UI freeze on large images

Status: **diagnosed, not fixed**. plugins/tools/basictools/kis_tool_gradient.cc
has its own TODO comment confirming it: "the gradient tool is still not in
strokes" -- unlike most tools, it never got migrated to Krita's async
stroke architecture (background thread, progressive updates).

endPrimaryAction() runs the full gradient computation synchronously on the
UI thread across the paint device's full bounds when the drag is released
-- not backgrounded, not incremental. On a large image on phone-class
hardware this is a real blocking pause, which reads as "did this even
work?" followed by the result suddenly appearing once the computation
finishes.

Real fix is porting the tool to strokes (matching how modern Krita tools
run paint operations on a background stroke thread) -- a genuine refactor,
not a quick patch. Needs proper scoping (time estimate, step-by-step plan)
before starting, same as the AI upscaler and Quick Mask.

## Channels docker: Ctrl+click-to-load-selection

Status: **scoped, not started**. Corrects an earlier wrong assessment of this
docker (originally logged as "no thumbnails, no isolation" -- both were
already fully implemented; see channelmodel.cpp's Qt::DecorationRole handling
and rowActivated()'s isolate-on-double-click). Only genuinely missing piece,
confirmed by searching for any Ctrl/modifier handling in the docker (none
found): Photoshop's Ctrl+click-a-channel-thumbnail -> load that channel's
data as a selection.

**Why it's real work, not a UI wrapper:** unlike Select Opaque (which loads
a layer's existing alpha channel -- already implemented, just needs a
better UI entry point), this needs to extract a *specific color channel's*
raw values across the image and convert them into a new selection mask.
No existing utility does this in the codebase.

**Rough implementation shape:**
1. Add a mouse event filter/override on the channel table view to detect
   Ctrl+click on a row (distinct from the existing plain-click checkbox
   toggle and double-click isolate).
2. Iterate the root layer's projection device (KisRandomConstAccessorSP or
   similar) and read the specific channel's byte value per pixel via
   KoChannelInfo::pos()/size() into a new KisPixelSelection.
3. Apply as the active selection via the same KisSelectionManager path
   Select Opaque already uses.

Deliberately not attempted blind in this session -- pixel-iteration code
like this needs to be verified against an actual build, not just read
through, given how the initial assessment of this same docker turned out
to be wrong on both counts checked.

## Fonts: browse, add, and a curated default set (George's idea, 2026-10-13)

George: "add fonts to your list. I want a system built in for browsing fonts and adding them and it would be nice to curate a default font set."

**Parts, in the order they would be built:**
1. **Browse.** A font browser (a panel or a window, and the same list inside the Type tool's font choice): every font drawn in its own face, a sample line the user can edit, a search box, favorites, and filters by kind (text, display, script, monospace). Big rows that suit a finger.
2. **Add.** "Add Font..." chooses font files (`.ttf`, `.otf`, `.ttc`), or a folder, or a `.zip`, with Android's file picker. The files are copied into Krimble's own fonts folder and registered when the app starts (`QFontDatabase::addApplicationFont`), so they survive restarts. "Remove" takes one out. The font's name, style and license text (from the font file itself) are shown before it is added.
3. **Curated default set.** A hand-picked group of **open-license** fonts (SIL Open Font License or Apache) shipped with the app, so there is a good choice on every phone. About 25 to 40 families covering clean text, headline/display, script and handwriting, monospace, and a few decorative ones. Each family adds about 100 to 500 KB to the app; a budget of about 10 MB is suggested. The license file of every shipped font is kept in the repository.

**Open questions for George:** the picks (Claude can propose a list with the license of each; George decides); whether the browser is a panel, a window, or both; whether fonts used in a document should be saved with it (not planned); the state of the Type tool (it was switched off on 2026-09 for a self-activation bug and is in the toolbox again; it should be re-tested before this is built).
**Status:** not started. Nothing in the code yet.

## AI upscaling: Real-ESRGAN and others (George, 2026-10-13: "Definitely put it on the list. Real-ESRGAN etc.")

Goal: Image > Upscale... gets AI methods next to the classic ones (Lanczos3 and the others, built 2026-10-13, entry 110), for much better fine detail (hair, text edges, textures) on photos and drawings.

**Candidates (licenses to be read and confirmed from the projects themselves before anything is bundled):**
- **Real-ESRGAN** with the **ncnn + Vulkan** runtime (ncnn is Tencent's inference library). The command-line port `Real-ESRGAN-ncnn-vulkan` is listed by one Linux packager under "MIT BSD". The usual route to Android. Best for photos.
- From memory, not yet checked: **waifu2x** and **Real-CUGAN** (MIT; for drawings and anime), **SwinIR** (Apache-2.0), small models in **OpenCV's super-resolution module** (EDSR, ESPCN, FSRCNN, LapSRN; licenses per model).
- Samsung Gallery's "Remaster" (AI upscaling for photos of 3 megapixels or less) is proprietary; its model is not published and cannot be used.

**What it costs:** a new native library to build for arm64 (ncnn, with Vulkan), model files of a few to tens of MB shipped or downloaded on first use, slow runs on a phone (large pictures must be done in tiles, with a progress bar and Cancel), and a license check of **both the code and the trained model files** (their terms and training data can differ). Krimble is GPL; MIT, BSD and Apache-2.0 fit with it.

**Order:** 1. choose the model(s) and read their licenses; 2. build ncnn for arm64 inside the existing build; 3. a first working "AI x2" and "AI x4" in the Upscale window with tiles, progress and Cancel; 4. more models (drawings, photos, denoise) if wanted.
**Status:** not started. Nothing in the code yet.

### Option for AI upscaling: a small separate app, "Krimble Upscaler" (George's idea, 2026-10-13: "Maybe we could release a small accessory app called Krimble Upscaler with a very small local model.")

**For:** the main app stays small (it is already about 165 MB) and is not put at risk by a new native library; the upscaler can be built, tested and updated on its own; it could stand as a product of its own on the store; a small model keeps it quick to download.
**Against:** two apps to build, sign and keep up to date; a second store listing with its own review (the 16 KB page-size rule applies to it too); a small model gives less detail than the full Real-ESRGAN models.
**How the two would work together:** Image > Upscale... in Krimble would offer "AI (Krimble Upscaler)", send the picture to the accessory app through Android's share mechanism (the same kind of hand-off as Take Photo, entry 109) and open the result when it comes back. If the accessory is not installed, Krimble opens its store page.
**Other ways to the same end, for comparison:** one app that downloads the model the first time it is used (the app stays small, one listing, but a download is needed); Play Feature Delivery (needs the app bundle format, not yet built).
**Status:** an option, not decided, not started. The first step is the same for all: pick a model and read its license (see the list above).

## Animation and game sprites (proposed; George, 2026-10-25)

Status: **proposed. Nothing in this section is started.** George: "Animated GIF seems like a must for an alleged animation tool. We should have a whole category of proposed future features related to animation, especially for making game sprites." He also wants simple looping animations for Wesnoth, "as long as they're not huge".

### Where it stands today (read in the code on 2026-10-25; none of it tried on a phone)

- **Animated WebP:** the WebP import and export have animation code: import reads each frame onto the timeline; export writes the frames as an animated WebP. Not tried on a phone.
- **GIF:** GIF import reads **only the first frame** (no animation code found). A GIF export exists (not checked for animation).
- **Video (MPEG, MP4):** cannot be imported; the code says video import needs ffmpeg, which Android does not have. There is no render-to-video either.
- **Batch Process and Krimble Script:** cannot read animated pictures (an animated WebP is skipped and counted).
- **Wesnoth-Mobile:** a static WebP works (the game looks for `name.webp` when `name.png` or `name.jpg` is not there). An animated GIF or WebP would show only its first frame: the picture loader is SDL3_image's `IMG_Load`, and no use of its animated-image loader was found in `picture.cpp`. Wesnoth makes animations from **lists of separate frames, each with a duration** (its own animation class and its data files), not from animated picture files.

### The must-haves (George: "a must")

1. **Animated GIF import:** every frame, each frame's delay, and transparency, onto the timeline.
2. **Animated GIF export:** a good palette (with dithering choice), transparency, loop count, and a **size estimate** before saving.
3. **Animated WebP:** try it on the phone, then make it reliable (import, export, lossless or lossy, with transparency).
4. **Batch Process and Krimble Script that read and write animated GIF and WebP**, so that a folder of GIFs can become animated WebP (and back) without the timeline. **Built on 2026-10-25 (288th), not tried on a phone:** animated GIF and WebP are read, animated WebP is written (lossless or lossy, with transparency, times and loop count kept). **Not built:** writing an animated GIF.
5. **Frames out and in:** write an animation as numbered pictures (`walk-001.webp`, `walk-002.webp`...), and make an animation from a folder of numbered pictures. **Frames out: built on 2026-10-25 (288th)** (the `frames` command). **Frames in: not built.**
6. **APNG** (animated PNG) import and export.

### Making game sprites

- **Animation preview window:** frames per second, loop mode (forward, ping-pong, once), whole-number zoom without blur (nearest neighbor, 1 to 8 times), a choice of background.
- **Onion skin controls:** how many frames before and after, and their tint.
- **Frame work:** a time for each frame, duplicate, reverse, copy a range, remove frames that are the same as the one before.
- **Tags:** named ranges of frames (idle, walk, run, attack, die) and directions (a sprite that faces 4 or 8 ways).
- **Pixel-art helpers:** a pencil that gives clean one-pixel lines ("pixel perfect"), whole-number scaling, a locked palette (the retro palettes are already in), **palette cycling** (color animation, as on the Amiga), mirror drawing, a tile preview that repeats the picture to check seams.
- **Grids and templates:** a **hex grid** and a **72 by 72 hex tile** template for Wesnoth; a preview of Wesnoth's team colors.
- **Anchors and hitboxes:** a pivot point and a box for each frame (or for each tag), kept with the animation.
- **Trim and unify:** cut the empty edges of the frames, and make all frames one size, for a sheet.

### Sprite sheets and game engines

- **Sprite sheet export:** a grid or a packed sheet, spacing between frames, power-of-two size, with a data file (JSON or XML) for Godot, Unity, Tiled, GameMaker and Aseprite-style readers.
- **Sprite sheet import:** cut a sheet into frames by a grid, or by the transparent gaps.
- **Wesnoth:** write the frames and a ready-to-paste piece of Wesnoth data-file text (the frame list with the durations). Later, a change in Wesnoth-Mobile so that it can read an animated WebP directly (SDL3_image has a loader for animated pictures that the game does not use yet).

### Video

- **MP4 or MPEG import through Android's own video decoder** (no ffmpeg): choose the time range and frames per second, and a size limit; then save as animated GIF or WebP, or as frames.

### Keeping animations small

- A **size estimate** before saving, and in Batch Process; a warning over a size the user sets (for a game: a budget in KB for each animation).
- Ways to make one smaller: fewer colors, fewer frames, a smaller picture, drop duplicate frames, a lower WebP quality. For scale: a 72 by 72 tile with 10 frames of lossy WebP with transparency should be tens of KB (to be measured, not yet).

### Krimble Script for animation (proposed commands)

`frames` (write the pictures one by one), `sheet cols N` (make a sprite sheet), `fps N`, `loop N`, `reverse`, `trim`, `delay N`; and a script that works on a whole folder of GIFs.

### A possible order (Claude's suggestion, not George's decision)

1. Batch Process and Krimble Script reading and writing animated GIF and WebP (smallest step; it needs no timeline, and the engine and its tests are already in place). **Done on 2026-10-25, except writing a GIF.**
2. Animated GIF import and export on the timeline.
3. The preview window and frame tags.
4. Sprite sheets, and the Wesnoth, Godot and Unity exports.
5. Video import.

## Interface refresh, color panels and shape options (George, 2026-10-30; proposed, not built unless it says so)

Branch `feature/krimble-ui-refresh`. Entries 324 to 332 are done on the branch (see `KRIMBLE_STATUS.md`).

### Color panels (approved, not started)
1. **Color docker on one page, no tabs**: picker (SV square and hue slider), values (RGB/HSV/HEX), recent colors, palette. The classic Krita selector is hidden.
2. **The SV square gradient-painted** (a horizontal white-to-hue gradient and a vertical transparent-to-black gradient layered in `paintEvent`; no per-pixel loop), and it **stretches as far as the user wants** (the old dialog's square does not scale; this was the reason for a new one).
3. **Mixing and brush test pad as its own panel** (George: "I specifically asked for a color mixing scratch pad ... It should be a panel."). It reads the foreground color; its raster is never part of the picture.
4. The mockups ChatGPT drew (tabs, brush test settings, blend mode, pad backgrounds) were **too ambitious**; keep what the list above says.

### Shape options (on hold)
- **Rounded Box**: the Rectangle tool already has rounded corners in code (`m_roundCornersX/Y`); a preset or a radius field in its options. Easy.
- **Tail option** on Rectangle, Ellipse and Rounded Box: Tail checkbox and Tail Width in Tool Options; with Tail on, the shape stays unpainted until a tap near the outline sets the attach point and a drag or tap sets the tip; the shape and tail are painted as one joined outline; a tap far from the shape skips the tail. Raster first; vector mode later.
- **Speech Balloon** is this tail option, not a tool. **Star** is the menu item (done on the branch, 331st entry).
- **No new toolbox tools** (George, 2026-10-30).

### Icons and highlights (approved, not started)
- Wire the 32 icons; Rotate icon first (the old one has corner lines without arrows). Glow for the selected tool; Color/Silver switch; smaller sizes by script. See `packaging/android/TOOLBOX-ICON-COVERAGE.md`.

### Transform and panels
- Scale and Rotate are modes of Transform (done on the branch). Open: should dragging inside the box still move in Scale.
- Panel docking only on deliberate intent: **already built** (`KrimbleDockTolerance`, Settings > Attach Panels, Attach Panel and Detach Panel). Open: the Brushes panel still attached eagerly; waiting for what it attached to. A dwell timer (about half a second near an edge) is an idea, not built.

### Update 2026-10-30, 12:55 PM MT
Color panels 1, 2 and 3 above are **done on the branch** (entries 341 and 342, untested): one page, gradient-painted SV square that grows both ways, the Mixer as its own panel. Panel docking on a deliberate approach is done as a 350 ms dwell (338th). Icons: wiring, glow, Color/Silver switch and smaller sizes are **not started**. Shape options stay on hold.

### Update 2026-10-30, 2:55 PM MT
Icons: the 32 pictures are wired with a **Full color / Clear** switch in the toolbox menu, and checked buttons have a blue glow (348th and 349th entries, untested). Not done: smaller sizes by script; pictures for Dodge, Burn, Colorize Mask Editing and Enclose and Fill.
