# Krimble — Roadmap Notes

Status: **planning only** — not started. Captured here so they don't stay stuck in chat.

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

- **Smudge tool** -- ✅ shipped (commit `7390d6b`). Dedicated toolbox tool
  wrapping Krita's existing colorsmudge paintop engine, own icon/shortcut/
  toolbox slot, auto-loads the "smudge" preset on activation.
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
