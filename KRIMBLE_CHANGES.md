# Krimble Changes

This file tracks fixes and changes made in this fork (Krimble) on top of
upstream Krita, specifically for the Android build. It is not exhaustive
history — it's a running log of notable bugs found and fixed, so future
work (by anyone, including an AI assistant with no memory of past
sessions) can see what's already been addressed and why, without having
to rediscover it from scratch.

## Standing rules for this fork

- **Design priority is mobile-first, then industry-standard parity
  second.** Mobile UI/UX constraints and conventions take precedence
  over matching desktop industry-standard tools exactly, where the two
  conflict (see e.g. the transform-mode-as-separate-tool decision,
  which departs from the industry-standard single-tool-with-modifier-
  keys model because modifier-key combinations don't translate to
  touch input).
- **Never write the name of the proprietary industry-standard tool
  this fork targets parity with, anywhere public-facing** — code
  comments, this file, commit messages, or any other note. Use the
  phrase "industry standard" instead. This has been stated more than
  once because it did not reliably get preserved as a written rule
  before; writing it here is the fix for that.
- **Never delete menu/action XML entries.** Comment them out (`<!-- -->`)
  instead. A deletion buried in an unrelated commit is silent and
  unrecoverable without git archaeology (see the Close All incident
  below); a comment is visible in the file itself and easy to restore.
- **Industry-standard feature parity — same features, same menu
  locations — is the primary UI goal** for this fork, subordinate to
  mobile-first design per above.
- **Verify actual behavior before cataloging an industry-standard
  feature as missing or existing.** Some features exist via a different
  mechanism or multi-step sequence in Krita (e.g. Merge Visible =
  Select Visible Layers + Merge with Layer Below) rather than a single
  equivalently-named action.
- **Ask before making codebase changes.** Investigating is fine
  unprompted, and so is writing/updating documentation. Only actual
  code/config edits, commits, and pushes need explicit go-ahead first.
- **Document every change twice**: inline code comments explaining the
  change, AND an entry in this file. Every time, not just sometimes.

## Open bug list

Reported as a numbered list; numbers are kept stable across sessions so
they can be referenced directly. Descriptions below are the *corrected*
versions after clarification — several were initially misread, and the
wrong initial readings are noted so they aren't repeated.

1. ~~Krita icon instead of Krimble icon~~ — **Fixed**, commit
   `a4a5b53`. Adaptive-icon foreground was the stock Krita paintbrush;
   replaced with Krimble's own icon.
2. ~~Brush presets open on load~~ — **Likely fixed**, commit `a73387e`,
   pending your on-device confirmation. See the 2026-09-04 changelog
   entry above.
3. **UI-scale-on-startup dialog missing.** A dialog with a percentage
   slider for setting the *interface* (UI) scale — not canvas zoom —
   used to appear on first launch in a previous Krimble build, then
   disappeared with no error or explanation. **Fixed** — see the
   2026-09-03 "UI-scale-on-startup dialog silently missing" entry
   below. (Initially misread as being about a canvas zoom-fit prompt;
   it is not.)
4. **Brightness/Contrast doesn't reach full black/white.** Specifically:
   pushing *Contrast* to maximum on a grayscale image should threshold
   it to pure black-and-white, and didn't. **Fixed** — see the
   2026-09-03 "Contrast slider couldn't reach pure black/white at
   maximum" entry below. Note this is a *different* bug from the
   earlier LcmsColorSpace.h Lab-round-trip fix (`b1eb52e`) — that one
   fixed a color-space clipping issue; this one fixed the contrast
   curve's math not being steep enough at max value. Both were real,
   separate bugs in the same feature.
5. **Resize handles hard to grab, including on the Transform tool.**
   A previous attempt was made to enlarge the hit-target size of
   resize handles (windows and Transform tool) and had no effect.
   Not yet investigated — the prior attempt itself needs to be found
   (git history search for handle/resize/grab-related commits turned
   up nothing in the visible history) and understood before trying
   again.
6. **No way to set UI zoom scale.** Same root cause as #3 above — this
   is not about canvas zoom. **Fixed** together with #3.
7. ~~Window snap is broken~~ — **Likely fixed**, commit `c50f03a`,
   pending your on-device confirmation. See the 2026-09-04 changelog
   entry above. (Initially misread as a "docked layout doesn't persist
   across sessions" issue; it is not — it's about the snapping
   behavior itself failing.)
8. ~~Transform tool does nothing~~ — **Fixed**, commit `05d1fc1`.
   `activateSubtool()` was ignoring the requested mode entirely; each
   Edit/Filter Transform menu item now correctly activates the mode
   it's labeled with. Also removed Free Transform's ability to
   silently blend into Perspective or Skew, per later direction that
   each mode should be a fully separate tool.
9. **Massive rendering delay during brush painting.** Investigated —
   ruled out the most likely causes: OpenGL is enabled by default
   (`useOpenGL()` defaults to "auto", not disabled), and there are
   zero Krimble-authored changes anywhere in the core paint/brush/image
   engine (`libs/image`, `libs/brush`, `plugins/paintops`). Unlike other
   fixes this session, this doesn't appear to be a Krimble-introduced
   regression sitting in a findable spot — likely needs on-device
   profiling/reproduction rather than further static code reading.
10. **Type tool does nothing.** Investigated — no confident root cause
    found. Checked `nodeEditable()` (generic node-lock check, not
    text-specific) and `SvgCreateTextStrategy.cpp` (no early-return
    guards, no Krimble markers). Substantial mobile-specific rework
    already exists in this tool (draggable selection handles,
    triple-tap-to-select-paragraph, floating action bar), which makes
    a simple obvious bug less likely. Needs on-device reproduction to
    diagnose further — ties to the in-progress "Type tool rework part
    1" commit mentioned previously, not reachable in this shallow
    clone's history to verify further.
11. **Cartoon mascot in support screen.** User is addressing this one
    themselves — not part of Claude's task list.
12. ~~Tool docker panels render as unmanageable narrow column shapes~~
    — **Two separate contributing bugs found and fixed**, commits
    `c50f03a` and `af46ff2`. (Described as "vertical stripes"
    initially, corrected to "unmanageable column shapes/proportions".)
    `c50f03a` (2026-09-04) fixed a global docking-mechanism bug in
    `KisMainWindow.cpp` — a self-contradicting `setAllowedAreas()` call
    was breaking dock/snap layout for every panel. `af46ff2`
    (2026-09-10) separately fixed 4 dockers (Advanced Color Selector,
    History, Histogram, SVG Symbol Collection) that all declared
    `defaultDockPosition() = DockRight`, causing them to pile into the
    same narrow column on fresh installs. Both plausibly contributed to
    the same visible symptom; pending your on-device confirmation that
    panels now render correctly with both fixes applied.
13. ~~File dialogs open at roughly half the size they should be by
    default~~ — **Fixed**, commit `4016d2b`. See the 2026-09-04
    changelog entry above.
14. ~~Dialog windows don't remember their size/position after being
    manually resized~~ — **Fixed**, commit `c27416b`. See the
    2026-09-04 changelog entry above.
15. ~~Convert to Profile is not available from the Edit menu~~ —
    **Fixed**, commit `623f3c5`. Placed the existing
    `imagecolorspaceconversion` action in Edit as well.
16. ~~Keyboard shortcut customization is not available from the Edit
    menu~~ — **Fixed**, commit `623f3c5`. Placed the existing
    `options_configure_keybinding` action, which had no menu placement
    anywhere before this.
17. ~~Toolbar customization is not available from the Edit menu~~ —
    **Fixed**, commit `623f3c5`. Placed the existing
    `options_configure_toolbars` action in Edit as well (it was
    already in Settings).
18. **Assign Profile (reinterpret pixels under a different color
    profile without converting them) does not exist anywhere in the
    app.** Not yet investigated whether Krita's color-management code
    has an underlying "assign without convert" operation to wrap.
19. ~~Purge (manually free cached memory...) does not exist as a menu
    command~~ — **Not actually missing.** `purge_unused_image_data`
    already exists and is already placed in the Image menu (matches
    what's visible in the reference Krimble screenshots). No work
    needed; this was a cataloging error, not a real gap.
20. ~~Step Forward / Step Backward... do not exist~~ — **Not actually
    missing.** These exist in the industry standard only because its
    plain Undo (Ctrl+Z) is a toggle (press again to redo the same
    thing back), a legacy quirk Krita doesn't have. Confirmed in
    KisMainWindow::undo() — it's an unconditional, repeatable stack
    walk, not a toggle. Krita's existing Undo/Redo already provide
    exactly this functionality. Building separate actions would just
    duplicate Undo/Redo.

## 2026-09-02 — File/Edit/View toolbar items silently deleted, restored

**File:** `krita/krita5.xmlgui`

`file_close`, `file_close_all`, `file_quit` (File toolbar/menu) and
`toggle-selection-overlay-mode`, `show-global-selection-mask`,
`view_show_canvas_only`, `fullscreen` (View toolbar) were all deleted in
commit `718e83878` ("Update krita5.xmlgui", 2026-08-24, George Edward
Purdy). That commit bundled these deletions together with unrelated
cleanup across three different toolbar blocks in one generic-message
commit, with no indication any of it was intentional — it has the
signature of accidental collateral damage from a broader toolbar-trim
pass, not a deliberate decision to drop Close All specifically.

All of the above still had valid, registered actions (nothing else
referenced them, but they weren't gone from the codebase) and have been
restored, with comments explaining why, per the no-delete rule above.

## Missing industry-standard-parity features (not just menu entries — the underlying feature doesn't exist)

Found while restoring the toolbar deletions above: two groups of actions
were referenced in the pre-`718e83878` menu but no longer exist
*anywhere* in the codebase — not in any `.action` file, not registered
via `createAction()` in any `.cpp`. These aren't menu-restoration jobs;
the feature itself needs to be built:

- **File > Print / Print One Copy** (`file_print`, `file_print_preview`)
  — the industry standard has both under File. No print implementation currently
  exists in this codebase at all.
- **Edit > Find and Replace** (`edit_find`, `edit_find_next`,
  `edit_find_prev`, `edit_replace`) — the industry standard has Find and Replace Text
  under Edit. No find/replace implementation currently exists in this
  codebase at all (this would apply to text layers/the text tool,
  Krita's closest equivalent to the industry standard's text-focused find/replace).

Left commented out in `krita5.xmlgui` at their original menu locations so
the intended placement isn't lost, pending a full menu-by-menu industry-standard
parity audit (comparing every menu against
`PHOTOSHOP_27_MENUS.TXT`/`Adobephotoshopshortcutkeyspdf.pdf` in the
project files) to catalog anything else in the same situation.



**Commit:** `b1eb52e`
**File:** `plugins/color/lcms2engine/LcmsColorSpace.h`

`createBrightnessContrastAdjustment()` built its LittleCMS device link
tagged as `cmsSigLabData` and chained it between two copies of the
working ICC profile (`RGB -> "Lab" -> RGB`, via
`cmsCreateMultiprofileTransform`). Tagging the link as Lab makes
LittleCMS perform a *real* colorimetric conversion into and out of Lab,
which is subject to the profile's rendering intent and gamut mapping.
That round-trip could keep true black (0,0,0) and true white (255,255,255)
from mapping back to themselves exactly, which is what caused the
Brightness/Contrast filter to not reach full black/white even at extreme
slider values.

The filter's own curve math (in
`plugins/filters/colorsfilters/kis_brightness_contrast_filter.cpp`)
already clamps to a full 0.0–1.0 range and was never the problem — the
bug was purely in how the resulting curve got applied at the LittleCMS
level.

**Fix:** build the device link using the color space's own signature
(`this->colorSpaceSignature()`) and apply it with a single-profile
`cmsCreateTransform`, exactly the same pattern already used correctly a
few lines down in `createPerChannelAdjustment()` in the same file. No
Lab conversion, no rendering-intent-dependent round trip — the curve is
applied directly to the working color space's own channels, so its own
clamping is what determines the output range.

## 2026-09-02 — Play Store icon for "Next"/debug flavor still showed stock Krita paintbrush

**Commit:** `a1b59e8`

An earlier fix (`a4a5b53`) corrected the adaptive-icon foreground and all
mipmap-density launcher icons to the Krimble gear/K mark, but missed
`packaging/android/apk/ic_launcher_next-playstore.png` — the Play Store
listing image for the debug/"next" build flavor, which was still the
original Krita paintbrush + git-branch badge artwork. Replaced with the
same Krimble gear/K image used for the main flavor's Play Store icon.

## 2026-09-02 — Debug/"Next" flavor icon made visually distinct

**Commit:** `dae8061`

The debug ("Next") build flavor's launcher icon was identical to the
release flavor's icon (same gear/K artwork, no visual distinction),
making it hard to tell debug and release installs apart on-device.
Generated a distinct variant: the same Krimble gear/K mark desaturated
to a grayscale/steel tone, with an orange circular "N" badge overlaid in
the bottom-right corner. Applied across all mipmap densities
(`mdpi` through `xxxhdpi`), both square and round variants, and the
debug flavor's Play Store PNG.

## 2026-09-03 — Added real "Merge Visible" action (industry-standard parity, Ctrl+Shift+E)

**Commit:** `e1a7e5a`
**Files:** `libs/ui/kis_layer_manager.h`, `libs/ui/kis_layer_manager.cc`,
`krita/krita.action`, `krita/krita5.xmlgui`

The industry standard's Layer menu has a dedicated "Merge Visible" command
(Ctrl+Shift+E) between Merge Down and Flatten Image. Krita has always
had the equivalent capability, but only as a two-step manual sequence
(per the Krita manual): Layer ▸ Select ▸ Visible Layers, then Layer ▸
Merge with Layer Below. No single menu item triggered both steps.

Added a new `merge_visible_layers` action to `KisLayerManager` that
does exactly that sequence and nothing else — it calls
`KisNodeManager::selectVisibleNodes()` then the existing
`mergeLayer()` slot, which already handles merging a multi-node
selection via `KisImage::mergeMultipleLayers()`. No new merge logic
was written.

**Bonus fix:** while placing the new action, found that the Layer
menu's existing `flatten_layer` entry was mislabeled "Merge Visible"
in `krita5.xmlgui`, even though `flatten_layer` only flattens the
single active layer — it has nothing to do with visible-layer merging.
Relabeled that entry to its correct text, "Flatten Layer", and inserted
the new `merge_visible_layers` action in the correct industry-standard-parity
position (between Merge Down and Flatten Image).

**Shortcut:** the industry standard uses Ctrl+Shift+E for Merge Visible and has no
default shortcut for Flatten Image. Krita had Ctrl+Shift+E assigned to
`flatten_image`. Moved the shortcut from `flatten_image` to the new
`merge_visible_layers` action to match the industry standard's defaults.

## 2026-09-03 — Exposed transform submodes as separate industry-standard-parity menu items

**Commit:** `c7630d4`
**File:** `krita/krita5.xmlgui`

The industry standard splits transformation into distinct menu commands: Edit ▸
Free Transform, Edit ▸ Transform ▸ Perspective/Warp/etc., Edit ▸ Puppet
Warp, and Filter ▸ Liquify. Krita has a single unified Transform tool
that switches between these modes on the fly from its own tool options
— functionally equivalent, but it meant none of these had a dedicated,
discoverable menu entry, and switching modes inside one tool felt
inconsistent with how the rest of the app is organized.

Investigated the transform tool's implementation
(`plugins/tools/tool_transform2/`) and found Krita already ships
separate, fully-functional actions for each submode —
`KisToolTransformFree`, `KisToolTransformPerspective`,
`KisToolTransformWarp`, `KisToolTransformCage`,
`KisToolTransformLiquify`, `KisToolTransformMesh` — each wired to its
own `activateSubtoolXxx()` slot in `kis_tool_transform.cc`. They were
just never placed anywhere except the toolbox flyout and the
transform tool's own right-click context menu, so nobody could find
them from the menu bar.

Added menu entries for these existing actions, no new tool code:

- **Edit ▸ Puppet Warp** → `KisToolTransformCage` (Krita's cage
  transform is the closest existing equivalent to the industry standard's
  mesh-pin-based Puppet Warp)
- **Edit ▸ Free Transform** → `KisToolTransformFree`
- **Edit ▸ Transform ▸ Perspective** → `KisToolTransformPerspective`
- **Edit ▸ Transform ▸ Warp** → `KisToolTransformWarp`
- **Filter ▸ Liquify...** → `KisToolTransformLiquify`

All five inserted at their corresponding industry-standard menu positions.
The industry standard's Transform submenu also has Scale/Rotate/Skew/Distort/
Flip/Rotate 180°, but those are modifier-key interactions within
Krita's Free Transform mode rather than separate tool activations, so
they don't get their own menu entries — Free Transform already covers
that functionality once opened.

## 2026-09-03 — Filled out Edit > Transform to the industry standard's full list, matched exact wording

**Commits:** `4a49e0d`, `55de2a2` (`a7d3e31` fixed an XML comment syntax
mistake introduced by `55de2a2`)
**File:** `krita/krita5.xmlgui`

Added the remaining industry-standard Edit ▸ Transform items — Scale, Rotate,
Skew, Rotate 180°, Rotate 90° Clockwise, Rotate 90° Counter Clockwise,
Flip Horizontal, Flip Vertical — joining Perspective and Warp already
there. All ten map to existing, distinct, already-functional Krita
actions (`layersize`, `rotatelayer`, `shearlayer`, `rotateLayer180`,
`rotateLayerCW90`, `rotateLayerCCW90`, `mirrorNodeX`, `mirrorNodeY`,
plus the two `KisToolTransform` submode actions from the previous
entry) — no new code, just menu placement.

Verified none of the 8 underlying one-shot actions are restricted to a
specific layer type: no `setExcludedNodeTypes()` calls, and their
`activationFlags` cover layers, shape layers, transparency masks, and
both selection types. Also traced each one's actual C++ implementation
and confirmed they behave like the industry standard's Edit ▸ Transform: if a pixel
selection is active, the operation is constrained to
`selection->selectedExactRect()`; otherwise it falls back to the whole
active layer's bounds (`KisImage::rotateImpl`, `KisNodeManager::
mirrorNodes`, `ImageSize::slotLayerSize`, `ShearImage::slotShearLayer`
all confirmed).

"Distort" is deliberately left out: Krita has no separate constrained
distort-only mode, only unconstrained corner-drag inside Free
Transform, so a menu item here would just be a duplicate label for the
same interaction as Free Transform, not real distinct functionality.

Text matches the industry standard's exact wording (no ellipsis on Scale/Rotate/
Skew) even though those three open a numeric dialog in Krita rather
than the industry standard's live on-canvas drag — same command, different
interaction model, not worth a misleading label difference.

## 2026-09-03 — Reordered Layer menu so Merge Down/Merge Visible/Flatten Image are adjacent

**Commit:** `c41569f`
**File:** `krita/krita5.xmlgui`

Confirmed against real industry-standard Layer menu screenshots: Merge Down,
Merge Visible, and Flatten Image sit as an uninterrupted triplet with
nothing between them, and the industry standard has no "Flatten Layer" or "Merge
Shape Layers" concept at all.

Krimble had three Krita-specific extras (`merge_selected_layers`,
`flatten_layer`, `merge_all_shape_layers`) interspersed inside that
triplet, breaking the exact adjacency. Moved all three to their own
group directly after the industry-standard-parity triplet instead, so the
triplet itself matches the industry standard exactly and the Krita-only extras are
clearly set apart as bonus functionality.

## 2026-09-03 — Contrast slider couldn't reach pure black/white at maximum

**Commit:** `19062f3`
**File:** `plugins/filters/colorsfilters/kis_brightness_contrast_filter.cpp`

Bug report: pushing Contrast to maximum on a grayscale image should
produce a pure black-and-white result, and didn't.

This file is Krimble/"Krita Mobile"-authored, not inherited from
upstream Krita. Its contrast curve used `contrastFactor = 1.0 +
contrast`, a plain linear scale that caps out at a slope of 2.0 even
at contrast=+100. That only clips pixels already below 25% gray or
above 75% gray to full black/white — the middle 50% of the tonal range
could never reach pure black/white no matter how far the slider was
pushed. Verified numerically: at max contrast, a pixel at 45% gray
only reached 40% output, not 0%.

Replaced the positive-contrast side with the industry standard's legacy contrast
formula (`factor = 1 / (1 - contrast)`), whose slope diverges toward
infinity as contrast approaches its maximum — that divergence is what
actually produces the posterize-to-black/white look. Handled the true
maximum (contrast = +100) as an explicit hard threshold at the 50%
midpoint rather than relying on floating-point infinity, guaranteeing
exact 0.0/1.0 output. Negative contrast (flattening toward gray) is
unchanged. Confirmed the fix numerically against the old formula
before committing.

## 2026-09-03 — UI-scale-on-startup dialog silently missing (bug items 3/6)

**Commit:** `913eb00`
**Files:** `krita/main.cc`, `libs/ui/dialogs/kis_dlg_preferences.cc`

Bug report: a dialog with a percentage slider for setting the
interface (UI) scale used to appear on first launch in a previous
Krimble build, then disappeared entirely in a later build with no
error or explanation. Both that startup dialog and the Settings >
Interface Scale menu item were confirmed missing.

Traced the full chain: `KisApplication::start()` calls
`KisAndroidDonations::showDonationDialog(true)`, through JNI to
`MainActivity.showDonationDialogInternal()`; on dismiss that fires
`JNIWrappers.onSplashDialogDismissed()`, which comes back into C++ as
`KisAndroidScaling::slotSplashDialogDismissed()`, calling
`maybeShowDialog(true)`. That entire path was intact and unconditional
— not the actual problem.

Root cause was one level deeper: `KisAndroidScaling::isSupported()`
(which gates the Settings menu item's creation in `KisMainWindow.cpp`)
and `maybeShowDialog()`'s early-return both depend on
`isHighDpiScalingEnabled()`, which only ends up true if the
"EnableHiDPI" `kritadisplayrc` key is true. That key defaulted to
`false` in `main.cc`, so on any install without a pre-existing config,
the entire interface-scaling subsystem (menu item + startup dialog)
never got a valid primary screen to work with and silently never
appeared — no crash, no log, just absent.

Changed the default to `true` in both `main.cc` (actual startup
behavior) and `kis_dlg_preferences.cc` (so the Preferences checkbox
reflects the same default, rather than showing unchecked while the
feature is actually on). `androidScalingAskOnStartup` already defaulted
to true and wasn't part of the problem.

## 2026-09-03 — Assigned industry-standard-parity shortcuts to Transform submodes

**Commit:** `27404fb`
**File:** `plugins/tools/tool_transform2/KisToolTransform.action`

Cross-referenced the industry-standard shortcuts reference: Ctrl+T is
always Free Transform there, and Liquify has its own dedicated
shortcut (Ctrl+Shift+X), but Perspective, Warp, and Puppet Warp have
no dedicated shortcut at all — reached only via menu (or a
modifier-drag inside Free Transform, which this fork deliberately
doesn't replicate). Mesh has no equivalent in the reference at all.

Moved Ctrl+T from the generic `KisToolTransform` action (activates the
Transform tool in whatever mode it last used) to
`KisToolTransformFree` specifically, so it matches the reference's
"Ctrl+T always means Free Transform" behavior. Assigned Ctrl+Shift+X
to `KisToolTransformLiquify`. Left Perspective, Warp, Cage, and Mesh
with no shortcut, matching the reference. Checked for conflicts with
existing shortcuts before assigning; none found.

## 2026-09-03 — Added Convert to Profile, Configure Shortcuts, Customize Toolbar to Edit menu

**Commit:** `623f3c5`
**File:** `krita/krita5.xmlgui`

Bug items 15–17. All three reuse existing, already-registered,
already-functional actions — no new code. `imagecolorspaceconversion`
already worked in the Image and Layer menus; `options_configure_toolbars`
already worked in Settings. `options_configure_keybinding` had no menu
placement anywhere before this. Placed together directly before
Preferences, matching the industry standard's grouping of these
app-config items at the end of the Edit menu.

## 2026-09-04 — Implemented Vibrance filter (real algorithm, not a wrapper)

**Commit:** `375a647`
**New files:** `plugins/filters/colorsfilters/kis_vibrance_filter.h/.cpp`, `wdg_vibrance.ui`
**Modified:** `colorsfilters.cpp`, `CMakeLists.txt` (registration only)

No equivalent existed in Krita. The industry standard's own Vibrance
formula is proprietary and undocumented (confirmed via research). Uses
the well-known open-source "vibrance" algorithm by CeeJay.dk instead —
MIT licensed, distributed via SweetFX/ReShade/GShade for over a decade.
A different, simpler implementation of the same idea, not a
byte-for-byte match to any commercial product.

Per pixel: luma via Rec. 709 weights, saturation as
`max(R,G,B) - min(R,G,B)`, blend each channel toward/away from luma by
`1 + vibrance*(1-saturation)` — strongest effect on muted pixels,
fading out as saturation approaches 1. Verified numerically before
implementation.

Implemented as a plain `KisFilter` with direct pixel iteration
(`KisSequentialIteratorProgress`) and `KoColorSpace::toRgbA16`/
`fromRgbA16` — the same conversion functions the existing Match Color
filter uses for Lab — rather than the heavier templated
`KoColorTransformation` pattern HSV Adjustment uses. No changes needed
to `plugins/color/colorspaceextensions/`, `krita5.xmlgui`, or any
`.action` file — self-registers into the existing Adjustments category.

## 2026-09-04 — Fixed docker snap/rendering (setAllowedAreas self-contradiction)

**Commit:** `c50f03a`
**File:** `libs/ui/KisMainWindow.cpp`

Bug items 7 and 12. Every dock panel had
`setAllowedAreas(Qt::NoDockWidgetArea)` called on it, then
`addDockWidget(side, dockWidget)` immediately placed that same widget
into `side` — a direct self-contradiction fed to Qt's dock-layout
engine (the widget was just told it's allowed in zero areas, then
force-placed into one anyway).

Strong candidate for both: window snap being broken (a dragged panel
declaring zero allowed areas can't be dropped back into any dock zone),
and docker panels rendering as unmanageable narrow columns (plausible
symptom of Qt's internal dock-layout width allocation misbehaving for
a widget that structurally shouldn't be docked anywhere, yet is).

The original intent (per the replaced comment) was to stop a dragged
panel from auto-snapping back into a dock zone — reasonable goal,
wrong API, broke docking far more broadly than intended. Commented
out rather than deleted. `QDockWidget` defaults to
`Qt::AllDockWidgetAreas` when `setAllowedAreas()` is never called,
restoring normal dock/undock/snap behavior.

## 2026-09-04 — Replaced the actual About KDE dialog artwork

**Commit:** `addd1f4` (uploaded directly, not via this session's usual commit flow)
**File:** `libs/widgetutils/xmlgui/aboutkde.png` (150 × 250)

Earlier this session, Claude mistakenly identified and replaced
`krita/pics/Breeze-light/light_kde.svg` /
`krita/pics/Breeze-dark/dark_kde.svg` — the small icon shown next to
"About KDE" in the Help menu — believing that was the graphic in
question. It was not. The actual content shown inside the About KDE
dialog itself comes from a completely separate file,
`libs/widgetutils/xmlgui/aboutkde.png`, sitting alongside
`kaboutkdedialog_p.cpp`/`.h`. That file already contained genuine
Konqi (KDE's real dragon mascot) artwork before any of this session's
changes — confirmed directly by viewing it.

User corrected both mistakes directly: reverted `light_kde.svg` /
`dark_kde.svg` back to the original stock vector icon (undoing
Claude's incorrect edit), and replaced `aboutkde.png` with new dragon
artwork. Both changes are the current, correct state.

## 2026-09-04 — Simplified Default workspace to ToolBox, Tool Options, Layers only

**Commit:** `a73387e`
**File:** `krita/data/workspaces/Default.kws`

Bug item 2 (brush presets opening unexpectedly on load). This file's
docker layout is a binary Qt `QMainWindow::saveState()` blob, not
human-readable per-docker flags — confirmed by decoding it, finding
all ~90 known docker object names (`ToolBox`, `sharedtooldocker`/Tool
Options, `KisLayerBox`/Layers, `PresetDocker`/brush presets, and
dozens more) embedded as UTF-16BE strings.

Replaced the `<state>` CDATA block only (nothing else in the file
touched) with a freshly-generated blob built via Qt's own `saveState()`
API: every known docker object name recreated, only ToolBox (left)
and Tool Options + Layers (right, stacked vertically) set visible,
everything else explicitly hidden.

Verified via round-trip test before writing: restored the generated
blob into a completely separate, freshly-built `QMainWindow` with the
same dockers and confirmed exactly `{ToolBox, sharedtooldocker,
KisLayerBox}` come back visible and nothing else — `restoreState()`
reported success. An earlier attempt at this same generation failed
silently (`restoreState()` returned false, zero dockers visible) until
`.show()` was called on the `QMainWindow` before
`saveState()`/`restoreState()`.

Per project direction: workspaces are meant to be customized by the
user afterward, so the default should be minimal — not because Krimble
avoids brush-related features.

## 2026-09-04 — Fixed file dialog default size and size persistence

**Commits:** `4016d2b` (item 13), `c27416b` (item 14)
**File:** `libs/widgetutils/KoFileDialog.cpp`

**Item 13:** the `QInputDialog` used on Android to pick a file format
before the native save picker opens had no explicit size at all, so it
fell back to Qt's tiny default (built for a one-line text prompt, not
a scrollable format list on a touchscreen). Sized to 85%/60% of the
available screen instead of a fixed pixel value, so it's reasonable
across different device sizes. There was already a comment
acknowledging the list was hard to scroll on touch, partially
addressed (`UseListViewForComboBoxItems`), but the overall dialog size
was never fixed.

**Item 14:** `KoFileDialog` (Save As, Open, Import, Export) never
remembered a size the user manually resized it to — every reopen reset
to the default. Reused the exact same `KConfigGroup` ("File Dialogs")
+ `dialogName` pattern already used by `getUsedDir()`/`saveUsedDir()`
for remembering the last-used directory. Geometry restored once in
`createFileDialog()` before the dialog is shown; saved via the
`finished()` signal so it's captured regardless of which `exec()` call
site is used, and regardless of accept/cancel.

## 2026-09-06 — Fixed crop tool false-undo on short strokes

**Files:** `plugins/tools/tool_crop/kis_tool_crop.h`,
`plugins/tools/tool_crop/kis_tool_crop.cc`

Any short stroke or incidental tap with the crop tool active could
silently undo the user's last operation and replace the crop
selection with an old one. Root cause: `endPrimaryAction()`'s
`haveValidRect` check ("was a real crop rectangle drawn, or just a
tap?") reused `m_handleSize` as its threshold. `m_handleSize` was
previously bumped from 13 to 44 (see item 4 above) to make resize
handles reliably grabbable on touch -- correct for that purpose, but
it silently raised the "is there a rect" threshold to 44px too. Any
stroke shorter than that in either dimension then fell through to
`tryContinueLastCropAction()`, which -- if the last undo-stack command
was a previous crop -- calls `undoLastCommand()` to reopen it. On
touch, that fallback fired on essentially any incidental short touch.

Fix: added a separate `m_minimumCropSize` (13, the original mouse-era
value) used only for the `haveValidRect` check, decoupled from
`m_handleSize`'s touch-target sizing. Handles stay easy to grab; short
strokes no longer trigger an unintended undo.

## 2026-09-06 — Shifted default theme from neutral gray to blue-gray

**File:** `krita/data/themes/KritaDark.colors`

Replaced the neutral gray UI base (`71,71,71` on Window/Complementary,
and matching values on Button/Tooltip/View/WM) with a blue-gray
sampled directly from a reference color (#373952 / RGB 55,57,82,
"Lavender Blue") using a color-picker tool. Every background value in
the theme was shifted by the same delta (dR -16, dG -14, dB +11) from
its old neutral-gray value, so the existing lightness relationships
between sections (Button vs. Window vs. Tooltip, etc.) are preserved
-- only the hue changes. Selection/accent blue (83,114,142) and all
foreground/text colors are unchanged.

Also fixed a pre-existing mismatch: `KisApplication.cpp` and
`KisMainWindow.cpp` default to theme name "Krimble dark", but this
file's [General] Name/ColorScheme were still "Krita dark", so the
lookup likely never matched on a fresh install. Renamed both to
"Krimble dark" to match.

## 2026-09-06 — Padded crop handle hit-test to reduce near-miss resets

**Files:** `plugins/tools/tool_crop/kis_tool_crop.h`,
`plugins/tools/tool_crop/kis_tool_crop.cc`

A tap that missed a crop handle's 44px hit box even slightly -- hard
to avoid on a small touchscreen -- fell through to
`mouseOnHandleType == None`, which `beginPrimaryAction()` treats as
"discard the existing crop selection and start a brand new one at
this point." Any near-miss looked like the tool randomly restarting.

Fix: added `m_handleHitPadding` (16px), applied only inside
`mouseOnHandle()`'s hit-test via `.adjusted(-pad,-pad,pad,pad)` on
each handle rect -- giving each handle an effective ~76px grab zone
while leaving the drawn/visible handle size at 44px, matching the same
decoupled-threshold pattern as `m_minimumCropSize` above.

## 2026-09-06 — Temporarily disabled Type tool (bug #9, random self-activation)

**File:** `plugins/tools/svgtexttool/Plugin.cpp`

Type tool was randomly self-activating in the toolbox, disrupting
other work in progress. Root cause not yet confirmed -- ruled out
KoToolManager::attachCanvas()'s lowest-priority auto-select (that path
only scans ToolBoxSection::Main, and the Type tool is registered under
ToolBoxSection::PSOrder alongside Brush, so it isn't a candidate
there). Some other auto-switch path is the likely culprit.

Commented out the one line in Plugin.cpp that registers
SvgTextToolFactory with KoToolRegistry. The plugin still builds and
loads; the tool simply never enters the registry, so it can't appear
in the toolbox or be activated by anything. Fully reversible --
uncomment that line once the actual root cause is found and fixed.

## 2026-09-06 — Added dedicated Smudge tool to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/light_krita_tool_smudge.svg` (new)
- `krita/pics/tools/SVG/16/dark_krita_tool_smudge.svg` (new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_smudge.h` (new)
- `plugins/tools/basictools/kis_tool_smudge.cc` (new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

Krita's colorsmudge paintop engine (plugins/paintops/colorsmudge/)
already existed and is more capable than Photoshop's Smudge tool, but
was only reachable by manually finding a smudge preset in the brush
picker while using the generic Freehand tool -- no dedicated toolbox
entry.

Added KisToolSmudge (subclasses KisToolBrush, reusing its option
widget) and KisToolSmudgeFactory, registered in default_tools.cc
alongside Brush. On activate(), it looks up the bundled "smudge"
preset via KisResourceModel(ResourceType::PaintOpPresets) and applies
it through the canvas's KisCanvasResourceProvider, so the tool
smudges immediately with no manual preset selection. Own icon (16px
SVG, dark/light), own toolbox slot (priority 21, directly after Brush
at 20), own shortcut (R, previously unused).

## 2026-09-06 — Added FG/BG color swap widget to bottom of toolbox

**Files:** `libs/ui/toolbox/KoToolBoxDocker_p.h`,
`libs/ui/toolbox/KoToolBoxDocker.cpp`

Photoshop's toolbox has the foreground/background color swatches (with
swap and reset-to-black/white controls) built into the bottom of the
tool column itself. Krimble already has this widget (KoDualColorButton,
swap/reset built in) but only in the classic toolbar
(kis_control_frame.cpp).

Wrapped the toolbox's existing tool-button scroll area in a container
widget (QVBoxLayout) so a second KoDualColorButton instance could be
appended below it, matching Photoshop's layout. Construction/wiring
deferred to setViewManager() since that's the first point a real
KisViewManager (and canvasResourceProvider) is available -- mirrors
the classic toolbar's connection pattern exactly (FG/BG signals both
directions, display renderer updates via a new slotUpdateDisplayRenderer,
color-space-change reconnection).

Kept in both locations (classic toolbar + toolbox) per project
decision -- this is an addition, not a relocation.

## 2026-09-06 — Wired up Android holiday splash screen (Dec 1-26)

**Files:** `libs/ui/kis_splash_screen.cpp`, `krita/data/splash/splash-android.qrc`,
`krita/data/splash/logo_splash_holidays.png`

Krita's holiday splash feature was a real, maintained feature 2015-2018,
then explicitly disabled upstream in 2021 ("Dummy out the holidays splash
for now") and never re-added -- confirmed via upstream commit history.
It was never implemented for Android at all, even when active on desktop
-- `kis_splash_screen.cpp`'s Android branch went straight to `hd.jpg`
unconditionally, no date check ever existed there.

Replaced the placeholder `logo_splash_holidays.png` with new artwork,
registered it in `splash-android.qrc` (was previously missing from the
Android build entirely -- only in the desktop-only `splash.qrc`), and
added a date check to the Android branch of `getImageSource()`: active
Dec 1 through Dec 26 inclusive, switching to `:/splash/holiday.png`.
Desktop's holiday splash remains disabled (upstream's `#if 0` block,
untouched) -- this only covers Android, Krimble's actual target.

## 2026-09-06 — Added Screen Mode toggle to bottom of toolbox

**File:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

Third of Photoshop's three bottom-of-toolbox items (FG/BG swatches,
Quick Mask, Screen Mode) -- this one didn't need a new feature, just a
toolbox entry point. Reused the existing `view_show_canvas_only` action
(already checkable, already bound to Tab) via a QToolButton with
setDefaultAction(), added below the color swap widget in the same
container layout.

Icon overridden locally to `view-fullscreen` (the action's own icon,
`document-new`, is a placeholder) -- kritamenu.action itself untouched.

Quick Mask remains the one genuinely unbuilt item of the three (logged
separately in KRIMBLE_ROADMAP.md).

## 2026-09-07 — Added dedicated Soften tool to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/light_krita_tool_soften.svg` (new)
- `krita/pics/tools/SVG/16/dark_krita_tool_soften.svg` (new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_soften.h` (new)
- `plugins/tools/basictools/kis_tool_soften.cc` (new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

Second of the two roadmap tools (KRIMBLE_ROADMAP.md), same pattern as
Smudge: Krita's filterop paintop engine (plugins/paintops/filterop/)
already lets you paint with any filter applied per-stroke, and already
ships a Gaussian-Blur-configured preset (plugins/paintops/defaultpresets/filter.kpp,
internal resource name "DFP") -- but with no dedicated toolbox entry,
matching Photoshop's Blur tool.

Added KisToolSoften (subclasses KisToolBrush) and KisToolSoftenFactory,
registered in default_tools.cc directly after Smudge. On activate(),
looks up the "DFP" preset via KisResourceModel(ResourceType::PaintOpPresets)
and applies it through KisCanvasResourceProvider -- identical mechanism
to KisToolSmudge, just a different preset name. Own icon (water-droplet
shape, Photoshop's Blur tool convention), toolbox priority 22 (directly
after Smudge at 21), shortcut U (previously unused).

## 2026-09-07 — Added touch-friendly edge/corner resize to all KoDialog dialogs

**Files:** `libs/widgets/KoDialog.h`, `libs/widgets/KoDialog_p.h`,
`libs/widgets/KoDialog.cpp`

Krita's dialogs had no custom resize handling at all -- no QSizeGrip,
no margin constant anywhere -- relying entirely on whatever the
platform provides natively for frameless windows, which isn't
something reliably touch-friendly (or possibly not present at all) on
Android, unlike desktop window managers.

Added a generous (20px) invisible resize margin along every edge of
every KoDialog-derived dialog, implemented via mousePressEvent/
mouseMoveEvent/mouseReleaseEvent overrides on the shared KoDialog base
class -- every dialog in the app gets this for free, no per-dialog
changes needed. Supports all 8 drag directions (4 edges + 4 corners),
respects the dialog's existing minimumSize()/maximumSize(), and adds
desktop cursor-hover feedback (harmless no-op on touch input) via
setMouseTracking(true).

Same decoupled-margin-from-visible-size pattern used for the crop
tool's handle hit-test padding and m_minimumCropSize earlier this
session.

## 2026-09-07 — Added dedicated Dodge and Burn tools to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/{light,dark}_krita_tool_{dodge,burn}.svg` (4 new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_{dodge,burn}.{h,cc}` (4 new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

A third Smudge/Soften-pattern win, found after initially (incorrectly)
concluding Dodge/Burn had no existing engine to reuse. Correction: Krita's
core compositing engine already has fully-working "dodge" and "burn"
composite ops (COMPOSITE_DODGE/COMPOSITE_BURN in KoCompositeOpRegistry.h)
usable by any brush -- no bundled preset needed, unlike Smudge/Soften.

Different mechanism from Smudge/Soften since there's no preset to look
up by name: on activate(), both tools clone the standard default brush
preset ("defaultPreset", plugins/paintops/defaultpresets/paintbrush.kpp)
via KisPaintOpPreset::clone() and override CompositeOp via
KisPaintOpSettings::setProperty() before applying. Cloning is required --
modifying the shared cached preset in place would corrupt the default
brush everywhere else it's used.

Dodge: icon (hollow circle, PS convention), shortcut O (matches PS's
actual Dodge shortcut), priority 23. Burn: icon (filled circle), shortcut
K (arbitrary -- PS groups Burn with Dodge in one flyout key, Krimble
gives it a separate toolbox slot instead), priority 24.

## 2026-09-07 — Fixed Dodge/Burn to build up continuously while dragging

**Files:** `plugins/tools/basictools/kis_tool_dodge.cc`,
`plugins/tools/basictools/kis_tool_burn.cc`

The cloned base preset defaults to WASH mode (PaintOpAction=2), which
applies a stroke's opacity once, flattened at release -- not while
dragging back and forth over the same spot. Real Dodge/Burn darken or
lighten continuously the longer you paint over an area within a single
stroke, which requires BUILDUP mode (PaintOpAction=1).

Both tools now set PaintOpAction=1 alongside their CompositeOp override.

## 2026-09-10 — Fixed cramped/unusable docker panels on mobile (bug item 12)

**Files:** `plugins/dockers/advancedcolorselector/colorselectorng.cpp`,
`plugins/dockers/historydocker/History.cpp`,
`plugins/dockers/histogram/histogramdocker.cpp`,
`plugins/dockers/svgcollectiondocker/SvgSymbolCollectionDocker.h`

Four secondary dockers (Advanced Color Selector, History, Histogram, SVG
Symbol Collection) all declared `defaultDockPosition() = DockRight`,
meaning on any fresh install or Reset Configuration they all fought to
auto-dock into the same narrow right-side column simultaneously --
producing the unusable vertical-stripe panels reported in bug item 12.

Changed all four to `DockMinimized`: they still exist and can be opened
manually, but no longer auto-expand into the right column on load.
Complements the earlier Default workspace simplification (`a73387e`),
which controls saved-layout state -- this fixes the class-level fallback
used when no saved layout applies.

## 2026-09-16 — Fixed build break in KoToolBoxDocker (missing
kactioncollection.h)

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

The "Canvas Only" toolbox button added for the Photoshop-parity toolbox
layout calls `viewManager->actionCollection()->action(...)`, but
`KisKActionCollection` is only forward-declared by the headers this
file already includes (e.g. `KoToolManager.h`) -- never given a full
definition. Compiler rejected the `->action(...)` call with "member
access into incomplete type 'KisKActionCollection'".

Added `#include <kactioncollection.h>`, matching the pattern every
other file calling `actionCollection()->action(...)` already follows
(`KisMainWindow.cpp`, `KisViewManager.cpp`, `kis_node_manager.cpp`).

## 2026-09-15 — Fixed build break in KoFileDialog geometry-persistence
connect() (bug item 14 follow-up)

**Files:** `libs/widgetutils/KoFileDialog.cpp`

The `connect(d->fileDialog, &QDialog::finished, ...)` call added for bug
item 14 (dialog geometry persistence) passed `d->fileDialog` -- a
`QScopedPointer<KisPreviewFileDialog>` -- directly as the sender, which
doesn't convert to the `QObject*` `connect()` needs. Compiler rejected
every overload with "no known conversion from
'QScopedPointer<KisPreviewFileDialog>' to ... 'const QDialog *'".

Changed to `d->fileDialog.get()`, matching the `.get()` pattern already
used for the other `connect()` calls in `createFileDialog()`.

**Follow-up (same commit-day):** the line above the `connect()`,
`QPointer<QFileDialog> dialogPtr = d->fileDialog;`, had the identical
bug and was missed on the first pass -- same `QScopedPointer` ->
`QObject*`-family conversion failure, this time against `QPointer`'s
constructor. Changed to `d->fileDialog.get()` as well.

## 2026-09-13 — Renamed Palettize to Indexed Color, moved to Image >
Mode, added Grayscale and Black and White palettes

**Files:**
- `plugins/filters/palettize/palettize.cpp`
- `plugins/filters/palettize/palettize.action`
- `libs/ui/kis_filter_manager.cc`
- `krita/krita5.xmlgui`
- `krita/data/palettes/grayscale.gpl` (new)
- `krita/data/palettes/black-and-white.gpl` (new)
- `krita/data/palettes/CMakeLists.txt`

Renamed the "Palettize" filter to "Indexed Color" (i18n string in the
filter constructor, plus matching strings in the .action file so the
Keyboard Shortcuts dialog stays consistent) and moved it from Filter >
Adjust to Image > Mode, matching industry-standard menu placement.
Filters are auto-inserted into their category's Filter-menu submenu in
code (`KisFilterManager::insertFilter`), not xmlgui, so this required a
one-filter exception there: the action is still created (so xmlgui can
place it in Image > Mode and enable/disable state still tracks via
`filters2Action`), only the line that adds it to the Filter-menu
`KActionMenu` is skipped for `palettize` specifically.

Not a true colorspace/document-mode change -- Krita has no indexed
colorspace at all (confirmed: nothing registers one). This filter
repaints pixel colors to match a chosen saved palette while the image
stays full RGB/Lab; it does not reduce the document to paletted storage.
Relevant for anyone chasing actual 8-bit indexed export later: of the
formats checked, GIF is the only one that's genuinely paletted by
format spec; BMP export (`plugins/impex/qimageio/kis_qimageio_export.cpp`)
always calls `convertToQImage()` which produces full ARGB regardless of
source, so indexed-color output never survives to a BMP file; PCX has
no import/export plugin at all.

Also added two new bundled palette resources (`.gpl`, same GIMP Palette
format as the existing `web.gpl`/`ps.gpl`) so the Indexed Color picker's
built-in options get closer to industry-standard parity: `grayscale.gpl`
(256 entries, R=G=B=n for n=0..255) and `black-and-white.gpl` (2 entries:
pure black, pure white). Web (`web.gpl`, 216 colors) and a classic
industry-standard default swatch set (`ps.gpl`, 131 colors) already
existed and needed no changes. Deliberately skipped the legacy System
(Mac OS) / System (Windows) 256-color palettes from that dialog -- low
value today. No code changes needed to surface the two new files in the
picker: it pulls from `ResourceType::Palettes`, the same global resource
pool as everything else, driven purely by the install list in
`krita/data/palettes/CMakeLists.txt`.

## 2026-09-13 — Padded Transform tool resize-handle grab radius, enlarged
aspect-lock button (bug #5)

**Files:**
- `plugins/tools/tool_transform2/kis_transform_utils.h`
- `plugins/tools/tool_transform2/kis_transform_utils.cpp`
- `libs/widgets/KoAspectButton.cpp`

Two related touch-target fixes reported together as "resize handles hard
to grab" (bug item 5).

**Transform tool.** Free Transform's corner/edge scale handles,
Perspective's handles, and Mesh transform's control-point/node/segment
hit-testing all route through one function,
`KisTransformUtils::effectiveHandleGrabRadius()`, which converted the
raw 8px `handleRadius` constant straight into a hit-test radius with
zero padding -- visual size and touch tolerance were the same number.
(There's already a separate, larger `handleVisualRadius` (12px) used
for drawing in Free Transform/Perspective; mesh transform draws using
half of the *grab* constant instead, so the raw `handleRadius` value
itself was left untouched to avoid changing mesh's drawn handle size.)
Added `handleGrabPadding` (30px), applied only inside
`effectiveHandleGrabRadius()`, bringing the effective grab radius to
38px / 76px diameter -- matching the touch-target size already
established for the crop tool's handle padding fix. Rotation handles
use a separate, untouched function (`effectiveRotationHandleGrabRadius`)
-- out of scope, since the report was specifically about resize.

**Aspect-ratio lock button.** `KoAspectButton` (shared by the Image
Size, Canvas Size, and Layer Size dialogs' lock-proportions toggle) had
a hardcoded `setFixedSize(19, 34)` / `setIconSize(QSize(9, 24))` with no
DPI or UI-scale awareness at all -- became especially hard to hit with
the UI scaled down. Doubled both to 38x68 / 18x48, same proportions
(kept narrow-tall since it spans two grid rows as a bracket between the
Width/Height fields, not squared off).

## 2026-09-14 — Krimble identity in About dialog, working Help menu links

**Files:**
- `krita/main.cc`
- `libs/widgetutils/xmlgui/khelpmenu.cpp`
- `krita/kritamenu.action`
- `libs/ui/KisMainWindow.h`
- `libs/ui/KisMainWindow.cpp`
- `krita/krita5.xmlgui`

The `KAboutData` block in `main.cc` was still upstream Krita's identity
verbatim -- description ("Krita is the full-featured digital art
studio"), copyright, and homepage (krita.org) all unchanged since the
fork started. Updated description to Krimble's, copyright now credits
Krita upstream plus Purdy Design's modifications, homepage points at
krimble.org. Internal app-id string (`"krita"`) and `organizationDomain`
left untouched -- both affect on-disk config paths and weren't part of
what was asked.

**Report Bug was invisible, and would have opened the wrong place.**
`setBugAddress()` had never been called, so `KisKHelpMenu` never
instantiated the Report Bug action at all -- it wasn't just missing from
the menu, the QAction object itself didn't exist. Added
`setBugAddress()`. Investigating the actual `reportBug()` slot turned up
a second, more important problem: the function is a
`#ifdef KRITA_STABLE`/`#else` split, and this build has `KRITA_STABLE`
undefined (`KRITA_ALPHA` is set in the top-level `CMakeLists.txt`, which
suppresses it) -- meaning the live branch was never the simple URL-open,
it was `KisKBugReport`, a full dialog that submits to
`https://bugs.kde.org/enter_bug.cgi`. Report Bug would have sent users
to file issues against upstream KDE Krita, not this fork. Collapsed the
function to unconditionally open the forum's Bug Reports tag
(`https://forum.krimble.org/t/bug-reports`); old code commented out
(`#if 0`), not deleted, per standing rule.

**Three new Help menu items**, none of which had any prior framework
hook (unlike Report Bug, which at least had partial KHelpMenu
scaffolding): Feature Request, Krimble Website, Krimble Forum. Each is a
plain `KisAction` wired straight to `QDesktopServices::openUrl()` with a
fixed URL -- new `.action` declarations in `kritamenu.action` (Help
category), new slots declared in `KisMainWindow.h`, created/connected in
`KisMainWindow::createActions()`, defined next to `showAboutApplication()`.

Help menu is now: Handbook, separator, Report Bug / Feature Request /
Krimble Website / Krimble Forum, separator, About Krimble.

Fixed build error in `KoToolBoxDocker.cpp`: `KisKActionCollection` was
only forward-declared (via `KoToolManager.h`), but the file calls
`viewManager->actionCollection()->action(...)`, which needs the complete
type. Added `#include <kactioncollection.h>`.

Rebranded the Android package identity from `org.krita` to `org.krimble`:
manifest package + activity name (root + debug/next flavors), `build.gradle`
namespace, moved 13 Java files from `src/org/krita/android/` to
`src/org/krimble/android/` with updated package declarations, 7 JNI native
symbols in `KisAndroidScaling.cpp`/`KisAndroidDonations.cpp` renamed to match
(`Java_org_krita_android_*` -> `Java_org_krimble_android_*`), 8 `import
org.krita.R;` statements, one notification channel ID string, and the
ProGuard keep-rule. This is what installs on the device and what would show
in a Play Store listing. Left unchanged: `android.app.lib_name` meta-data
(still "krita" — tied to the CMake target/`.so` name, a separate,
not-yet-decided rename), MIME type strings (`x-krita*`, for `.kra` file
compatibility).

## 2026-09-19 — Fixed 6 remaining stale `org/krita/android` JNI class-path strings

The earlier full package rebrand (`org.krita` → `org.krimble`, Java file
moves, native symbol renames) missed a different mechanism: reflection-
style JNI calls that pass the class path as a **string literal**, not a
compiled symbol. `KisAndroidUtils.cpp` (x2), `KisLongPressEventFilter.cpp`,
`KisKineticScroller.cpp`, and `KisAndroidMediaEncoderRunnable.cpp` (x2)
were still calling `QJniObject::callStaticMethod("org/krita/android/...",
...)` against class paths that no longer exist post-rebrand — these would
throw `ClassNotFoundException` at runtime. `KisLongPressEventFilter` and
`KisKineticScroller` run during normal window/widget init, not some rare
path. Commits `612b8e2` (first batch found, in `KisAndroidDonations.cpp`,
turned out to already be part of the full rebrand) and `21952b8` (the
actual 6 remaining refs above).

## 2026-09-19 — Replaced launcher icon (K + paw mark), all densities + debug/next variant

Adaptive-icon foreground (`ic_launcher.webp` / `ic_launcher_round.webp`)
replaced at all 5 mipmap densities (mdpi–xxxhdpi) with the new K+paw
logo. Source: a transparent PNG with correct adaptive-icon safe-zone
padding (a same-design JPG variant was rejected — baked-in white
background, no transparency, tighter crop). Commit `888a590`. The
`ic_launcher_next`/`_round` (nightly/debug flavor) variant was still
carrying the old Krita mark — updated separately in commit `4bf581c` for
consistency.

## 2026-09-20 — Added release signing config

`assembleRelease` had no `signingConfigs` block at all — would produce an
unsigned build, not installable anywhere. Added one to `build.gradle`:
keystore path resolved from `$HOME/krimble-release.jks` (must exist on
whichever machine actually runs the build — the release keystore itself
is generated once via `keytool` and lives outside the repo, never
committed), passwords read from `KRIMBLE_KEYSTORE_PASSWORD` /
`KRIMBLE_KEY_PASSWORD` env vars set in that machine's shell profile.
Added `*.jks`/`*.keystore` to `.gitignore` so the keystore file can never
land in the repo by accident. Commit `4c7c698`.

Caught one bug in the fix itself before it shipped: the first draft used
a local Groovy variable named `keyPassword`, which collides with the
Gradle DSL setter of the same name — `keyPassword keyPassword` would
have tried to call the string value as a method and broken the build.
Renamed the local var to `keyPass` before committing.

## 2026-09-25 — Two build fixes pushed from server; build pipeline corrected

Two source fixes had been sitting uncommitted on the build server. Pushed:

- `CMakeLists.txt`: added `find_package(Threads REQUIRED)` just before the
  WebP lookup in the optional-dependencies section.
- `plugins/filters/colorsfilters/kis_vibrance_filter.cpp`: added
  `#include <KoUpdater.h>` — the type was only forward-declared, so the
  progress-update calls failed to compile.

Both carry inline comments.

Build pipeline findings (full procedure now in `BUILD_ANDROID.md`):

- The build server is **ARM64** (Oracle Ampere). NDK 27.3's host tools are
  x86_64 and run under emulation there. The emulated `ld.lld` segfaults,
  which surfaced as a misleading CMake error: "Host compiler must support
  64-bit std::atomic!". Fix: the NDK's `lld` was swapped for Ubuntu's
  native `lld-18` (server-side only, not a repo change).
- Hand-patching `build.gradle` / hand-running cmake was abandoned. The
  build now follows Krita's own CI recipe in
  `build-tools/ci-scripts/android.yml`.
- `androiddeployqt` silently drops all Qt QML modules when `_install` is
  inside the source tree. Earlier hand-rolled builds had exactly that
  layout — likely cause of the earlier crash-on-launch APK. Builds now go
  to `~/kwd`, outside the source tree.

## 2026-09-26 — First clean native build via the official CI pipeline

Native build (step 2 in `BUILD_ANDROID.md`) completed clean on commit
`bf2f7b3`: started ~00:56, finished ~08:30 server time. All Krimble libs
(`libkrita_arm64-v8a.so` etc.) and the `qml` folder landed in
`~/kwd/krita/_install`. First build on the server using Krita's own CI
recipe and the native ARM linker.

Also corrected `BUILD_ANDROID.md`: the lld-swap undo command no longer
works on this server (the swap was run twice and overwrote the Intel
backup). Old undo line commented out; re-run guard added.

APK packaging (step 3) not yet run.

## 2026-09-26 — Fixed the weeks-long "no launcher icon" bug

Root cause found: `ic_launcher_foreground.xml` (and the `_next` variant),
the foreground layer of the adaptive launcher icon, had
`android:src="@mipmap/ic_launcher"` — but on Android 8+ that name
resolves to the adaptive-icon XML itself (`mipmap-anydpi-v26/ic_launcher.xml`),
which requires this very file to render its foreground. A self-reference.
The icon renderer fails silently on the loop, leaving a blank icon.
Confirmed by installing a built release APK on a Samsung Galaxy A26
(Android 16) and finding no icon on the install prompt or home screen.

Fix: added `res/drawable-nodpi/ic_launcher_fg.webp` and
`ic_launcher_next_fg.webp` (copies of the existing xxxhdpi artwork,
given their own resource names), and pointed both foreground XML files
at those instead. No more self-reference.

Files:
- `packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp` (new)
- `packaging/android/apk/res/drawable-nodpi/ic_launcher_next_fg.webp` (new)
- `packaging/android/apk/res/drawable/ic_launcher_foreground.xml`
- `packaging/android/apk/res/drawable/ic_launcher_next_foreground.xml`

Not yet re-verified with a fresh build/install — next step.

## 2026-09-30 — Launcher icon: rainbow background removed, K enlarged

Two problems seen on a Samsung Galaxy A26 after installing the
beta2 release APK built from `5b84bcd`: the icon showed the K+paw
over the Krita rainbow gradient instead of white, and the K was
small.

Background: all four adaptive-icon files pointed at
`@drawable/ic_launcher_background` (or `ic_launcher_next_background`),
which resolves to the drawable XML that holds the Krita rainbow
gradient. A white `<color>` with the same name already existed in
`res/values/` but was never used. Each file now points at
`@color/...` (white). The old `<background>` line is kept as a
comment. The rainbow drawable files are untouched.

K size: `ic_launcher_fg.webp` and `ic_launcher_next_fg.webp` are
192x192 px images. The foreground XMLs used `gravity="center"`, which
places a bitmap at its own pixel size without scaling, so the K
covered only about 26% of the icon width on a high-density screen.
Both foreground XMLs now wrap the bitmap in a 12dp `<inset>` with
`gravity="fill"`, which scales it up to about 45% of the icon width.
The old `<bitmap>` element is kept as a comment.

Note: the 192px source art will look slightly soft when scaled up.
A larger K+paw source image would fix that.

Files (all under `packaging/android/apk/res/`):
- `mipmap-anydpi-v26/ic_launcher.xml`
- `mipmap-anydpi-v26/ic_launcher_round.xml`
- `mipmap-anydpi-v26/ic_launcher_next.xml`
- `mipmap-anydpi-v26/ic_launcher_next_round.xml`
- `drawable/ic_launcher_foreground.xml`
- `drawable/ic_launcher_next_foreground.xml`

Not yet verified with a fresh build and install.

## 2026-10-01 — Renamed "Advanced Color Selector" docker to "Color Selector"

**File:** `plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`

The docker title (also used as its tab label) changed from "Advanced
Color Selector" to "Color Selector". The old `setWindowTitle` line is
kept as a comment. The Wide Gamut Color Selector docker keeps its name.

Not changed: the same name still appears in the dropdown in
`kis_color_selector_settings.cpp` (line 43). Not yet verified with a
fresh build and install.

## 2026-10-01 — Hue/Saturation naming; filter dialog width cap

**Hue/Saturation rename.** The existing HSV filter (already under
Image > Adjustments, shortcut Ctrl+U) is now named to match the
industry-standard name. Files:
- `plugins/filters/colorsfilters/kis_hsv_adjustment_filter.cpp` -- menu
  text "&HSV Adjustment..." -> "&Hue/Saturation..." (old line commented).
- `plugins/filters/colorsfilters/kis_hsv_adjustment_filter.h` -- display
  name "HSV/HSL Adjustment" -> "Hue/Saturation" (old line commented).
  The internal id `hsvadjustment` is unchanged.
Decision: Hue and Saturation sliders are NOT added to Brightness/Contrast,
since this filter already provides them.

**Filter dialog width cap.** File: `libs/ui/dialogs/kis_dlg_filter.cpp`.
After the saved geometry is restored, if the dialog is wider than half
the screen's available width it is resized to exactly half. Applies to
every filter dialog. Height unchanged. Cause of the original width not
confirmed (saved geometry vs. content size hint). Not yet verified with a
fresh build and install; the dialog's later `adjustSize()` call may
override it.

**Not done:** Brightness/Contrast dialog title not drawn (cause
unknown); color selector docker half-size (no change made).

## 2026-10-01 — Filter dialog bottom row split into two rows (real cause of width)

**File:** `libs/ui/forms/wdgfilterdialog.ui`

Brightness/Contrast and HSV (Hue/Saturation) dialogs were wider than the
screen on Android. Measured from a screenshot: the bottom row held 6
widgets (gallery toggle, Preview, Multiframe, Create Filter Mask, Cancel,
OK) and needed about 1370px minimum, so no sizing code could shrink the
dialog below it. This applies to every filter dialog.

The row is now two rows: gallery toggle, Preview and Multiframe on the
first; Create Filter Mask, Cancel and OK on the second. Widget names and
connections are unchanged. The original row is kept as an XML comment.
The half-screen width cap added earlier today in `kis_dlg_filter.cpp`
stays; it could not take effect while this row set the minimum width.

Remaining known width source: the preset row (preset dropdown, Use last
preset, Edit Presets, XML) in `wdgfilterselector.ui`, about 650px
minimum. Not changed. Not yet verified with a fresh build and install.

## 2026-10-01 — Color Selector height cap; Android filter dialog title label; settings dropdown rename

**Color Selector docker height.** File:
`plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`.
The docker content is now capped at 42% of the screen's shorter side
(about 454px on a 1080px-wide portrait screen). Before, it took about
910px in portrait (measured from a screenshot), so this is roughly half.
Landscape is unaffected. Width is not capped separately: the selector
scales with its height. The 42% figure is an estimate and may need
tuning after a test on the device. Not yet built or installed.

**Filter dialog title label (Android only).** File:
`libs/ui/dialogs/kis_dlg_filter.cpp`. The dialog title ("Filter:
Brightness/Contrast...") is set in code but Android does not draw it,
leaving a blank white strip. Added a bold, centered label at the top of
the dialog that shows the filter name, wrapped in `#ifdef Q_OS_ANDROID`
so desktop builds are unchanged. It updates through `setDialogTitle()`.
Not yet built or installed. This code was not compiled before pushing.

**Settings dropdown.** File:
`plugins/dockers/advancedcolorselector/kis_color_selector_settings.cpp`.
The color docker settings dropdown entry "Advanced Color Selector" is
now "Color Selector" (old line commented). Index-based, so nothing else
depends on the text.

## 2026-10-01 — Version string alpha1 -> beta2 (splash and About)

**File:** `CMakeLists.txt` (both `KRITA_VERSION_STRING` lines, Qt5 and Qt6
branches). The splash and About dialog read this string and still said
"1.0.0-alpha1"; only the Gradle `versionName` (APK filename) had been
bumped to beta2. Old lines kept as comments.

Deliberately NOT changed: `KRITA_ALPHA 1` stays set. Switching it to
`KRITA_BETA` would change `BRANDING` from "Next" to "Beta" and swap the
icon and splash asset sets already customised for the "Next" variant.

Needs a CMake reconfigure on the next build (`make` does this
automatically). The git hash on the splash also updates then. Not yet
built or verified.

## 2026-10-01 — Krimble marked as Beta (CMake flag, README badge)

**Files:** `CMakeLists.txt`, `README.md`.
- `CMakeLists.txt`: `KRITA_ALPHA` is now `KRITA_BETA` (old line commented).
  To stop that from switching `BRANDING` from "Next" to "Beta" (which would
  swap the customised icon and splash assets), `BRANDING` is set to "Next"
  when not given on the command line. The version string was already bumped
  to `1.0.0-beta2` earlier today.
- `README.md`: Status badge text `1.0.0--alpha1` -> `1.0.0--beta2`.
  History line 180 ("Version reset to alpha1") left as-is; it is history.

Not yet built or verified. The next `make` re-runs CMake automatically.

## 2026-10-01 — Default toolbox icon size 32 -> 16 (16 x 16)

**File:** `libs/ui/toolbox/KoToolBox.cpp`

On Android the default toolbox icon size was a fixed 32. It is now 16
(16 x 16), matching the "16x16" entry already in the toolbox's icon size
menu. Old line commented out. Only the default changes: an icon size
already saved on a device (`KoToolBox/iconSize` in the config) still wins,
so existing installs keep their saved size until it is reset to
"Default" from the toolbox context menu. Fresh installs get 16.
Not yet built or verified.

## 2026-10-02 — Photoshop-style Image > Adjustments menu; Auto Tone, Auto Color, Photo Filter

Per project direction (reversal of the earlier "move the whole Adjust
submenu" approach): the auto-generated Adjust submenu is back in the
Filter menu, and Image > Adjustments is now a hand-built menu.

**`krita/krita5.xmlgui` (no comments added to the file, per direction)**
- Filter menu: `adjust_filters` (auto-generated "Adjust" submenu) placed
  first among the filter categories, after Liquify.
- Image menu: the single `adjust_filters` entry replaced by an
  "Adjustments" submenu listing individual filters in this order:
  Brightness/Contrast, Levels, Curves (label for `perchannel`) | Vibrance,
  Hue/Saturation, Color Balance, Photo Filter | Invert, Posterize,
  Threshold, Gradient Map | Desaturate, Match Color.
  Directly below it: Auto Tone, Auto Contrast, Auto Color.
- Not in the Adjustments menu (still available in Filter > Adjust and the
  Filter Gallery): Dodge, Burn, Slope/Offset/Power, Cross-channel curves,
  Normalize, Index Colors.
- Actions are referenced as `krita_filter_<id>`; an action name that does
  not exist is ignored by the menu loader.

**New filters (no new algorithms)**
- `plugins/filters/levelfilter/KisAutoLevelsFilters.{h,cpp}` (registered in
  `KisLevelsFilterPlugin.cpp`, added to that plugin's `CMakeLists.txt`):
  - Auto Tone (`autotone`): per-channel contrast stretch using the
    existing auto levels engine (`KisAutoLevels::adjustPerChannelContrast`),
    0.1% clipping each end, midtones untouched.
  - Auto Color (`autocolor`): the same, plus midtone neutralization: each
    channel's mean is moved to 50% gray at full strength.
  - Both apply the result through the existing Levels transformation.
    RGB images only (same limit as the "auto levels for all channels" button
    in the Levels dialog); other color models are left unchanged.
  - Clipping, offset and target values are the defaults of the Levels
    auto dialog; they are our choices and are not claimed to match any other
    program numerically.
- `plugins/filters/colors/KisFilterPhotoFilter.{h,cpp}` (registered in
  `colors.cpp`, added to that plugin's `CMakeLists.txt`): Photo Filter
  (`photofilter`) with Color, Density and Preserve Luminosity. It reuses the
  Fast Color Overlay blend: Color blend mode when Preserve Luminosity is on,
  Multiply when off. Defaults: warm orange (236,138,0), density 25%, preserve
  luminosity on. Blend modes are approximations of a photo filter, not a
  copy of any other program's math.

**What was and was not verified**
- Verified: both new `.cpp` files and both plugin registration files pass a
  compiler syntax-only check against the real Krita headers (generated
  config/export/moc headers stubbed). The auto levels math was reproduced on a
  synthetic low-contrast color-cast image: Auto Tone stretches every channel to
  the full range; Auto Color additionally brings the channel means to about
  50%.
- NOT verified: a real build, linking, running the filters in the app, the
  menu layout on a device, and the `Curves...` label override. The first
  server build after this commit may still surface errors.

**Not done (need new algorithms):** Exposure, Black & White, Channel Mixer,
Equalize, Shadows/Highlights, Replace Color, Selective Color, HDR Toning,
Color Lookup.

### Resulting Image menu (2026-10-02)

```
Image
  Mode                       >
  Adjustments                >
      Brightness/Contrast...
      Levels...                      Ctrl+L
      Curves...                      Ctrl+M
      ---------------------------
      Vibrance...
      Hue/Saturation...              Ctrl+U
      Color Balance...               Ctrl+B
      Photo Filter...                (new)
      ---------------------------
      Invert                         Ctrl+I
      Posterize...
      Threshold...
      Gradient Map...
      ---------------------------
      Desaturate...                  Ctrl+Shift+U
      Match Color...
  ---------------------------
  Auto Tone                          (new)
  Auto Contrast
  Auto Color                         (new)
  ---------------------------
  Image Size...                      (unchanged below this line)
```

Shortcuts are the ones already assigned to each filter in code; no
shortcut was added or changed. Auto Tone, Auto Contrast, Auto Color and
Photo Filter have none.

## 2026-10-02 — Default workspace repaired: only ToolBox, Tool Options and Layers on startup

**File:** `krita/data/workspaces/Default.kws`

**Cause found.** Commit `a73387e` (2026-09-05, "Simplify Default workspace
to ToolBox, Tool Options, Layers only") saved the panel layout (`<state>`,
a base64 string) with every letter converted to upper case: 0 lower-case
characters in 7,752, against about 1,500 lower-case in every earlier version.
Base64 is case-sensitive, so the string decoded to garbage, Qt refused to
restore it, and the app silently fell back to its built-in panel defaults.
That is why Color Selector and Brush Presets kept appearing at startup and the
Sept 4 simplification never took effect on a device.

**Fix.** The `<state>` is replaced with a valid one that contains exactly three
visible panels: ToolBox on the left; Tool Options (`sharedtooldocker`) above
Layers (`KisLayerBox`) on the right. The other 90 dockers in the file are
present and hidden, so they can still be opened from Settings > Dockers.
The rest of the file (settings, thumbnail) is the last valid version from
`62a77b2` (2026-08-24). The broken state from `a73387e` is not kept in the
file; recover it from that commit if ever needed.

**How the new state was made and checked.** A small Qt program built a main
window with a dock widget for every name found in the Aug 24 state, set the
layout above, and called `saveState()`. The result starts with the correct
Qt magic bytes, restores successfully into a fresh window, and in that window
exactly ToolBox, Tool Options and Layers are visible (90 of 93 hidden).

**Not verified:** on a real device. Installs that already have a copy of the
old workspace file in their app data may keep using it until the app data is
cleared; not tested. Dock widths are Qt defaults for a 1080 px-wide screen and
may need tuning.

## 2026-10-02 — Theme fallback and Themes menu check (first-launch gray theme)

**File:** `libs/ui/thememanager.cpp` (two small changes, old lines commented out)

Symptom: on a fresh install the first launch showed the plain gray default
palette instead of "Krimble dark"; the second launch was correct.

Two leftovers from renaming the theme were found in `ThemeManager`:
1. `currentThemeName()` fell back to `"Krita dark"` when no name was known.
   No theme file has that name any more, so the palette lookup found nothing and
   the app kept Qt's gray default. Now falls back to `"Krimble dark"`.
2. `populateThemeMenu()` decided which Themes-menu entry to check by calling
   `currentThemeName()` on a menu group that had just been created with nothing
   checked. That call returned the fallback name, so no entry was checked, and
   any later palette refresh used the fallback. It now compares against the theme
   name that was requested (default `"Krimble dark"`).

Confidence: these are real bugs and the fixes are correct, but I have NOT
confirmed they are the whole cause of the first-launch gray theme; I could not
reproduce it. Not compiled (the syntax-check setup could not resolve
`kstandardshortcut.h` for this file; the change only uses QString/QStringLiteral
already used in the file). Test: clear app data, install, first launch.

## 2026-10-02 — File > New dialog narrowed on Android

**Files:** `libs/ui/KisOpenPane.cpp`, `libs/ui/widgets/kis_custom_image_widget.cc`
(both changes wrapped in `#ifdef Q_OS_ANDROID`; desktop builds unchanged)

Symptom: the New Image dialog was wider than a portrait screen (its left list
was cut off) and, in landscape, the Width / Height / Resolution fields, their
unit dropdowns and the Predefined dropdown stretched across a lot of empty
space.

- `KisOpenPane.cpp`: the left list (Custom Document, templates...) gets a
  maximum width of 24% of the screen width. Longer names are elided. On a
  landscape screen the cap is larger than the list's natural width, so
  landscape is unchanged.
- `kis_custom_image_widget.cc`: maximum widths (in the app's scaled pixels) of
  90 for the width, height and resolution fields, 95 for their unit dropdowns,
  and 130 for the Predefined dropdown. They were about 123 each before. Only
  maximums were added, nothing can become smaller than it needs to be.

Both files pass a compiler syntax-only check against the real Krita and KDE
headers (generated headers stubbed). Not verified: a real build, and how the
dialog looks on a device. The numbers are estimates taken from a screenshot and
may need tuning.

Also re-checked with the same syntax-only compile: `libs/ui/thememanager.cpp`
(theme fix from earlier today) now passes; it could not be checked before.

## 2026-10-02 — Settings > Panels > Detach Panel

**File:** `libs/ui/KisMainWindow.cpp` (after the panel list is built; no
`krita5.xmlgui` change, the item is added to the code-generated Panels menu)

New submenu at the bottom of Settings > Panels, "Detach Panel". It lists the
panels that are currently shown and docked (hidden panels and panels that are
already floating are not listed). Choosing one makes it a free-floating window.
The list is rebuilt each time the submenu opens. If no panel is docked it shows a
disabled "No docked panels" entry.

A locked panel (lock icon in its title bar) cannot float, so choosing it
unlocks it first, then detaches it.

**Why Tool Options was greyed out in Settings > Panels:** that is Krita's
built-in panel lock, not a bug. While a panel's lock is on, its entry in the
Panels list is disabled and its close/float buttons are hidden. Tapping the lock
icon in the panel's title bar turns the lock off and re-enables the entry.
Nothing was changed for this; the lock still works as before.

**Verified:** the menu logic was run in a small standalone Qt program using
stand-ins for the two title-bar classes (copying their real lock behaviour): only
visible docked panels are listed, a locked panel is unlocked and floated, it
disappears from the list afterwards, and the empty case is handled.
**Not verified:** `KisMainWindow.cpp` as a whole could not be syntax-checked here
(it needs the external `lager` library headers, which are not available), the
real build, and floating panels on a real Android device.

## 2026-10-02 — Floating panels on touch: stay detached, easier to grab, Attach Panel

**File:** `libs/ui/KisMainWindow.cpp` (the first part is Android only,
`#ifdef Q_OS_ANDROID`)

Feedback from device testing: floating panels re-docked too eagerly when
dropped, and were hard to grab with touch.

- **Stay detached.** When a panel floats it is now allowed in no dock area
  (`setAllowedAreas(Qt::NoDockWidgetArea)`), so dropping it over a dock area
  does not snap it back. It is docked again through its float button or the new
  Settings > Panels > Attach Panel item. Its allowed areas are restored when it is
  docked. The change is made only from the `topLevelChanged` signal, i.e. after
  the panel was already added to a dock area. (The 2026-09-04 attempt set
  the allowed areas before `addDockWidget()` and broke docking; this does not.)
- **Easier to grab.** While floating, the panel's title bar has a minimum
  height of 40 (app-scaled pixels; docked panels are unchanged).
- **Attach Panel.** New submenu under Settings > Panels, below Detach Panel. Lists
  floating panels; choosing one docks it back into its previous area.

**Verified:** in a standalone Qt program (offscreen): the allowed-areas change
and the taller title bar happen on floating, `setFloating(false)` still docks a
panel back into its previous area even while its allowed areas are empty, and
the allowed areas and title height are restored afterwards.
**Not verified:** on a device. In particular, whether Qt on Android honours the
empty allowed areas while a floating panel is being dragged (this is how Qt
decides whether a drop docks the panel), and whether 40 is the right height.
The offscreen test cannot simulate dragging. `KisMainWindow.cpp` still cannot be
syntax-checked here (missing external `lager` headers).

## 2026-10-02 — Floating panels: title bar overlap fixed, default size, resize handle

**File:** `libs/ui/KisMainWindow.cpp` (Android only, `#ifdef Q_OS_ANDROID`).
Replaces part of the 2026-10-02 "stay detached" change after device testing.

**Bug found on device:** floating panels showed their title text and buttons on
top of the panel's content (Layers header over the blend mode box, Color
Selector title over the hue bar). Cause: I made the title bar taller by setting
a minimum height. The dock layout only reserves the title bar's size hint, so the
taller title bar was drawn over the content below it. Reproduced in a standalone
Qt test: title bar 80 px tall, content starting at 34 px.

**Fix:** the extra title height is now added as padding in the title bar's own
layout (14 on top and bottom while floating; removed when docked again), which
the dock layout does reserve. Same test: title bar and content no longer overlap.

**New:**
- Default size. A panel that starts floating is capped to 60% x 75% of the
  screen's shorter side (a panel already smaller stays as it is).
- Resize handle. A visible 36 x 36 corner handle (three diagonal lines) in the
  lower right of each floating panel; hidden when the panel is docked. It uses
  Qt's size grip, so the panel is resized by dragging the handle.

**Verified (standalone Qt test, offscreen):** no overlap; the default size is
applied; the handle sits in the lower right corner and resizes the panel when
dragged (650x800 dragged by -100 px wide gave 550 wide; the height result was
limited by the test screen); docking again removes the padding and hides the
handle. **Not verified:** on a device, touch dragging of the handle, and the size
and padding numbers (14, 36, 60%, 75%) are first guesses.

## 2026-10-02 — File > New still too wide: Profile dropdown minimum width found

**Files:** `libs/ui/widgets/kis_color_space_selector.cc` (Android only),
`libs/ui/KisOpenPane.cpp`

Device test of the earlier narrowing (ff26aa9) showed the fields were narrower
but the dialog was still wider than a portrait screen (left list cut off).

**Cause found:** `libs/ui/forms/wdgcolorspaceselector.ui` gives the Profile
dropdown (`cmbProfile`) a hard minimum width of 300. On the test phone the app
scales by about 2.5, so that is about 750 screen pixels, matching the width of the
dropdown in the screenshot (745 px). It set the minimum width of the whole Custom
Document page on its own.

**Fix:** on Android only, the dropdown's minimum width is set to 100 in the
selector's constructor. The dropdown is a squeezed combo box that elides long
profile names, so nothing is lost. The same selector is used by other dialogs
(for example image properties and color conversion), which also get narrower on
Android. The left list cap in `KisOpenPane.cpp` was lowered from 24% to 22% of the
screen width.

**Estimate, not measured:** with a 2.5 scale, a portrait screen is about 432 wide
in app units. The left list (22%) is about 95; the page should now need roughly
290, so the total is about 385. The 290 is an estimate from the other rows, not a
measurement. **Verified:** both files pass a compiler syntax-only check. **Not
verified:** the real dialog on a device.

## 2026-10-02 — Detach Panel and Attach Panel moved under "Panels" in the Settings menu

**Files:** `libs/ui/KisMainWindow.cpp`, `krita/krita5.xmlgui` (George approved
this edit to the file; no comments were added to it)

Device feedback: the Settings > Panels list already has far too many entries, so
Detach Panel and Attach Panel should not be at the bottom of it.

- `KisMainWindow.cpp`: both are now `KActionMenu` actions registered in the
  action collection as `settings_detach_panel_menu` and
  `settings_attach_panel_menu`, instead of submenus added to the Panels list.
  Their contents and behaviour are unchanged. The old lines that added them to
  the Panels list are commented out.
- `krita5.xmlgui`: two `<Action>` entries added in the Settings menu, directly
  after the `settings_dockers_menu` ("Panels") entry.

**Caveat:** `KisMainWindow` loads a locally customized `krita5.xmlgui` from the
app's data folder if one exists, and that file replaces the built-in one. A phone
with such a file will not show the two new entries until that local copy is
removed or updated.

**Verified:** `krita5.xmlgui` is well-formed XML. **Not verified:** the menu on a
device; `KisMainWindow.cpp` cannot be syntax-checked here (missing external
`lager` headers).

## 2026-10-02 — Default workspace applied once on first launch (Brush Presets / Color Selector at startup)

**File:** `libs/ui/KisApplication.cpp` (right after the existing `--workspace`
handling in `KisApplication::start()`)

Device test with cleared app data showed Brush Presets and Color Selector still
visible, even after `Default.kws` was repaired (9f951f5). Reading the startup
code showed why: Krita applies a workspace only when asked (command line option,
canvas-only mode, Reset All Settings). On a normal first launch no workspace is
applied, so the panels come from each panel's built-in default, and the repaired
`Default.kws` was never used.

**Fix:** on the first launch of an installation (no `DefaultWorkspaceApplied`
entry in the `Krimble` config group), the workspace named "Default" is applied
once and the entry is written. Later launches keep whatever layout the user
left, so panels the user opens or moves are not reset. Skipped when a workspace
was given on the command line.

**Limits:** it needs the repaired `Default.kws` to be the "Default" workspace in
the app's resource database, which is true on a fresh install (clear app data).
An install over an older one keeps the old workspace and is not changed by this.
Settings > Reset All Settings clears the flag along with the other settings, so
the layout is applied again after a reset.

**Verified:** no compile error in the added lines (the syntax-only check of the
whole file reports unrelated errors from stubbed generated headers). **Not
verified:** on a device.

## 2026-10-02 — Detach Panel / Attach Panel: summary, README entry, toolbox keeps its size

This entry summarizes the feature built across the 2026-10-02 entries above
(86f1474, cad9232, 9a6c1b0, c71726a) and records two further changes.

**What the feature is**
- Settings > **Detach Panel** (below "Panels"): lists the panels that are shown
  and docked; choosing one makes it a free-floating window. A locked panel is
  unlocked first.
- Settings > **Attach Panel**: lists the floating panels; choosing one docks it
  back into its previous area.
- A panel that floats stays where it is dropped (it is allowed in no dock area
  while floating), has extra title bar padding for touch, is capped to a default
  size (60% x 75% of the screen's shorter side) and has a resize handle in its
  lower right corner. Android only, except the two menu items.
- Code: `libs/ui/KisMainWindow.cpp`; menu placement: `krita/krita5.xmlgui`.

**README:** the "Panels" row of the "What Krimble changes" table now mentions
Detach Panel, Attach Panel, stay-where-dropped and the resize handle.

**Toolbox keeps its size when detached** (`libs/ui/KisMainWindow.cpp`, Android
only). Device feedback: the toolbox did not keep its dimensions when detached. The
size every panel has while docked is now remembered, and the toolbox is given its
docked size back when it floats instead of being capped to the default size.
The cause of the size loss is not confirmed: in a standalone Qt test (including a
scroll-area based panel like the toolbox) a panel keeps its size when floated, so
this change restores the size explicitly rather than fixing a known cause.
**Verified:** the size tracking and restore in a standalone Qt test. **Not
verified:** on a device with the real toolbox; whether the toolbox then keeps its
vertical layout.

## 2026-10-02 — Six panel plugins no longer built (George's edit, commit 563cc24)

**File:** `plugins/dockers/CMakeLists.txt` (the `add_subdirectory` lines were
commented out, not deleted)

To make the app leaner, these panel plugins are no longer compiled or packaged:
- Artistic Color Selector (`artisticcolorselector`)
- Animation panels: Animation Timeline, Animation Curves, Onion Skins (`animation`)
- Brush Preset History (`presethistory`)
- Log Viewer (`logdocker`)
- Storyboard (`storyboarddocker`)
- Wide Gamut Color Selector (`widegamutcolorselector`)

They no longer appear in Settings > Panels, which also shortens that list.

**Checked (read-only):** nothing else in the build refers to these plugin targets
(a search of all CMake files, scripts and CI files found no references outside the
six directories), so the build should still configure and link.

**Important for the server build:** commenting a plugin out does not delete the
copy already installed. The old plugin libraries (`kritaartisticcolorselector`,
`kritaanimationdocker`, `kritapresethistory`, `kritalogdocker`,
`kritastoryboarddocker`, `kritawgcolorselector`, each `_arm64-v8a.so`) stay in
`_install/lib`, and packaging would still put them in the APK, so the panels would
keep showing. They need to be deleted from `_install/lib` before packaging.

## 2026-10-02 — Splash images are JPG; the old PNGs are no longer packaged

**Files:** `krita/data/splash/splash-android.qrc`, `krita/data/splash/splash.qrc`,
`libs/ui/kis_splash_screen.cpp`; new files `krita/data/splash/logo_splash.jpg` and
`krita/data/splash/logo_splash_holidays.jpg` (added by George, commit 2c76225)

The new splash images have no transparency, so they are saved as JPG: 379 KB and
387 KB (1536 x 1024, RGB, no alpha), against about 2.1 MB and 2.4 MB for the PNGs.

- Both resource lists now embed the two JPGs. The old PNG lines are kept as XML
  comments in each file. Android uses `splash-android.qrc` (aliases `hd.jpg` and
  `holiday.jpg`); the desktop list `splash.qrc` uses `0.jpg` and `1.jpg`.
- `kis_splash_screen.cpp` loads the new alias names. The old lines are commented out.
- The PNG files stay in the repository (George wants to keep them) but nothing
  references them any more, so they are not built into the app. A search of the
  code, resource and build files found no other reference.
- The December 1-26 holiday splash is now landscape (it was portrait); the splash
  code scales any image to a fixed height and keeps its aspect ratio.
- The splash window still draws its own gray background, visible only in the bar
  under the image that holds "Loading resources...".

**Verified:** both resource files are well-formed XML; the splash code passes a
compiler syntax-only check; Qt reads both JPGs (1536 x 1024, no alpha channel).
**Not verified:** a real build, and that the Android build includes Qt's JPEG
image plugin (upstream Krita's Android build used a JPEG splash, which suggests
it does). Check after the build that the splash shows.

## 2026-10-02 — Brush Presets still at startup (real cause), panel renamed "Brushes", Attach docks where the panel is

**Files:** `libs/ui/KisMainWindow.cpp`,
`plugins/dockers/presetdocker/presetdocker_dock.cpp`

**1. Brush Presets and Color Selector still showed after the first new file, even with the first-launch workspace (d250bcc).**
Cause: while the welcome page is shown the panels are hidden and the layout from
before hiding is kept in `dockerStateBeforeHiding`; it is put back when a document
opens. The first-launch workspace was applied while the welcome page was up and
left that saved layout untouched, so the first new file brought back the old
layout. Reproduced in a standalone Qt test: without the fix the old panels return
after the first new file, with it only ToolBox, Tool Options and Layers do.
Fix: `KisMainWindow::restoreWorkspace()` now, when no document is open, calls
`toggleDockersVisibility(false, true)` right after restoring the workspace, which
captures the workspace layout and hides the panels again as the welcome page does.

**2. Panel renamed "Brush Presets" -> "Brushes"** (`presetdocker_dock.cpp`; old
line commented out). It also changes the name in Settings > Panels.

**3. Settings > Attach Panel now docks where the panel was put**, instead of back
into its old place. The panel goes to the nearer side of the window (left or right,
by the position of the panel's centre) and, on that side, between the panels above
and below it. While detached it still does not snap anywhere. The old behaviour is
kept in a comment. Panels are not put above or below the canvas.
Verified in a standalone Qt test with five drop positions: left/right side chosen
by position; top, middle or bottom of the side order correctly among three docked
panels; the panel is docked afterwards and its allowed areas are restored.
Docking via the panel's own float button still returns it to its old place.

**Not verified:** a real build and a device. `KisMainWindow.cpp` cannot be
syntax-checked here (missing external `lager` headers); the new code was tested as
a standalone copy.

## 2026-10-02 — Resize handle no longer covers the Layers Delete button; smaller color picker

**Files:** `libs/ui/KisMainWindow.cpp` (Android only),
`libs/widgets/KisDlgInternalColorSelector.cpp` (Android only)

**Resize handle over the Delete button.** On a floating Layers panel the corner
handle sat on top of the Delete (trash) button. Instead of moving the button, the
handle now has its own strip: while a panel floats, a 36-high strip is reserved
under its content (via the panel's content margins) and the handle sits in that
strip. When the panel is docked again the strip is removed. This applies to every
floating panel, so no panel loses anything under the handle. Verified in a
standalone Qt test: the content shrinks by exactly the strip height while floating
and the margin returns to 0 when docked. The Delete button was not moved.

**Color picker too big.** The dialog opened by tapping a color well (for example
the foreground/background colors in the toolbox) is designed at 505 x 490. On
Android it now opens at 80% of that, never more than 90% of the screen; the layout
still enforces any minimum it needs. The 80% is a first guess from "a little too big".

**Verified:** `KisDlgInternalColorSelector.cpp` passes a compiler syntax-only
check; the strip logic was tested as a standalone copy. **Not verified:** a real
build or a device; `KisMainWindow.cpp` cannot be syntax-checked here.

## 2026-10-02 — Four panels renamed to match industry-standard names

**Files (title lines only, old lines kept as comments):**
`plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`,
`plugins/dockers/compositiondocker/compositiondocker_dock.cpp`,
`plugins/dockers/snapshotdocker/SnapshotDocker.cpp`,
`libs/ui/toolbox/KoToolBoxDocker.cpp`

| Was | Now |
|---|---|
| Color Selector | **Color** |
| Compositions | **Layer Comps** |
| Snapshot Panel | **Snapshots** |
| Toolbox | **Tools** |

Together with "Brush Presets" -> "Brushes" (765acdb). The names also change in
Settings > Panels. Saved panel layouts refer to panels by an internal name, not by
title, so they are not affected, and no code compares against the old titles
(a search found none).

**Kept on purpose (George's decision):** "Swatches (Palette)", because it is more
explanatory. **Not changed:** Tool Options, Text Properties, Grid and Guides.
The Color Selector entry in the color settings dropdown
(`kis_color_selector_settings.cpp`) still says "Color Selector".

**Verified:** all four files pass a compiler syntax-only check. **Not verified:**
a real build or a device.

## 2026-10-02 — Resize handle moved the wrong corner on the toolbox

**File:** `libs/ui/KisMainWindow.cpp` (Android only)

Device feedback: the handle in the lower right corner of the floating toolbox
moved the lower left corner.

**Cause (reproduced in a standalone Qt test):** the handle was a `QSizeGrip`. Qt
decides which corner a size grip controls from where it sits in the window: if
its left edge is in the left half of the window it acts as a lower left grip. The
toolbox is narrow, so the 36 pixel wide handle was in the left half. Test: on a
panel 70 wide, dragging the handle right moved the left edge from 302 to 310
and shrank the panel; on a 300 wide panel it behaved correctly.

**Fix:** the handle is now a plain widget that resizes its panel from the lower
right: the left and top edges never move; dragging by (+60, +80) adds 60 to the
width and 80 to the height. Same test, both widths: left edge fixed, right edge
and bottom edge move as dragged. Look, size (36 x 36) and position are unchanged.
The old `QSizeGrip` include is commented out.

**Verified:** standalone Qt test (wide and narrow panel). **Not verified:** a
device; `KisMainWindow.cpp` cannot be syntax-checked here.

## 2026-10-02 — Edit > Preferences window far wider than the screen

**File:** `libs/ui/dialogs/kis_dlg_preferences.cc` (Android only)

Device feedback: Preferences opened as a very wide window.

**Cause found:** the form of the General page (`libs/ui/forms/wdggeneralsettings.ui`)
gives its top widget a hard minimum width of 552. At the phone's app scale (about 2.5)
that is about 1380 screen pixels, and a window cannot be narrower than its widest
page. The page already scrolls inside (it has a scroll area), so it does not need that
minimum.

**Fix:** (1) the General page's minimum width is set to 0. (2) After the dialog is
built it is resized to no more than 95% of the screen width and 90% of the screen
height (the smaller of that and its natural size).

**Checked:** other pages' forms have only smaller hard minimum widths (the tablet
pressure curve 200, brush preview 320 and scratch pad 250 on a brush settings form
that may not be part of this dialog). **Not verified:** that every page now fits;
another page may still force it wider. **Not verified:** a build or a device. The file
could not be syntax-checked here (it includes headers from the external `lager`
library); the added lines are plain Qt calls. If Preferences is still too wide, a
screenshot of the page that is too wide will show which form to fix next.

## 2026-10-02 — Type tool re-enabled (George's decision); cause of bug #9 still unknown

**File:** `plugins/tools/svgtexttool/Plugin.cpp`

Re-enabled the line that registers the Type tool (`SvgTextToolFactory`) with
`KoToolRegistry`, which had been commented out since 2026-09-06 (bug #9). The
old comment and the old commented line are kept above the new line. With the tool
registered again, the Type tool appears in the toolbox and all 18 items in the
Type menu work again; they had nothing to act on while the tool was off.

**Bug #9 is NOT fixed.** The Type tool was randomly activating itself, and the
reason was never found. This session's read-only search of the automatic tool
switching code found no cause: `KoToolManager::preferredToolForSelection`
(used by `KisShapeController::setInitialShapeForCanvas` and
`DefaultTool::explicitUserStrokeEndRequest`) picks the lowest-priority-number
tool whose shape list matches the selected shapes, and the Type tool matches only
text shapes (plus `flake/always`, which no shape uses); the "no active tool"
fallback only looks at the Main section, and the Type tool is in `PSOrder`. The
Type tool's own priority (1) and section (`PSOrder`) are Krimble settings (upstream
Krita uses priority 5 and the Vector section); I did not change them.

If it activates by itself again, please note what was being done (which tool was
active, what was tapped, whether a text layer existed) so it can be traced.
**Not verified:** a build or a device; the file could not be syntax-checked here (the
Qt QML headers are not installed in the sandbox). The change is one uncommented line.

## 2026-10-02 — Resize handles missing on every panel (bug from cfa2343) fixed

**File:** `libs/ui/KisMainWindow.cpp` (Android only)

Device feedback on the 19:05 build: the sizing handles on all floating panels
were missing.

**Cause (my bug, from cfa2343):** when the handle stopped being a `QSizeGrip`
subclass and became a plain `QWidget`, the code that checks "does this panel
already have a handle?" (`findChild<KisFloatingDockSizeGrip*>()`) stopped
working. The class has no `Q_OBJECT`, so Qt's type check matches any widget.
It returned the first child of the panel (in a standalone test: the panel's own
title button), the code concluded a handle existed, and no handle was ever
created. It also moved and showed that other widget in the lower right corner.
Reproduced in a standalone Qt test.

**Fix:** the handle sets an object name (`krimbleResizeHandle`) and is looked up
by that name among the panel's direct children. Same test: no handle -> nothing
found; after creating one -> found.

**Verified:** the lookup in a standalone Qt test. **Not verified:** a build or a
device; `KisMainWindow.cpp` cannot be syntax-checked here. Lesson: a standalone
test covered the handle's resizing but not this lookup; the next test should
create the handle through the same code path as the app.

## 2026-10-02 — A panel's own float button now docks it where it was put

**File:** `libs/ui/KisMainWindow.cpp` (the tracking is Android only)

Device feedback: detached panels "fly away" instead of snapping to the nearest
zone. Settings > Attach Panel already docked a panel where it was put (765acdb),
but the float button in the panel's own title bar still used Qt's default, which
sends the panel back to its old place. That is the likely path that was used.

**Fix:** the app remembers the centre of a floating panel as it is moved. When a
panel is docked again by any route other than Attach Panel, it is docked with the
same placement (nearer side, left or right; between the panels above and below).
Attach Panel sets a flag while it runs so it is not handled twice; the remembered
position is cleared after use. While floating, panels still do not snap anywhere.

**Verified (standalone Qt test, same logic):** a panel that started at the end of
the right column and was put near the top docks first; put in the middle docks
between B and C; put near the left docks on the left; the menu path gives the same
result as before. **Not verified:** a build or a device; `KisMainWindow.cpp` cannot
be syntax-checked here. Dragging a floating panel onto a dock area still does
nothing by design.

## 2026-10-02 — In-app window and tab icon: orange K + paw instead of the rainbow K

**Files:** `krita/pics/branding/Next/sc-apps-krita.svgz` (replaced);
`krita/pics/branding/Next/sc-apps-krita-rainbow-original.svgz` (the old rainbow icon,
kept, not referenced by anything)

Device feedback: the document tab and window icon was still the old rainbow K.
That icon is `krita-branding`, loaded in `KisApplication.cpp` (line 243) and used
for the document tabs (`KisMainWindow.cpp`). It comes from `branding.qrc`, which maps
`krita-branding.svgz` to `sc-apps-krita.svgz`. The Android launcher icon had been
replaced earlier (Sept 19) but this in-app icon had not.

The new `sc-apps-krita.svgz` is an SVG that embeds the orange K + paw art (taken from
`packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp`, cropped to the art with
a margin, scaled to 512 x 512, transparent background). Rendered with Qt at 16, 32,
64 and 256 pixels it is clear at every size. The file is 101 KB, against 281 KB for the
old one.

**Limits:** the source art is only 192 pixels across, so very large uses (above about
256 pixels) will be slightly soft; a vector or larger version of the art would fix
that. The desktop PNG icons in the same folder (`16-apps-krita.png` ... `1024-apps-krita.png`)
and the Play Store icon `packaging/android/apk/ic_launcher-playstore.png` (still the
old rainbow gear K at 512 x 512) were NOT changed; the PNGs are not used by the Android
app, but the Play Store icon will need replacing before any Play Store upload.
**Not verified:** a build or a device.

## 2026-10-02 — Smudge, Soften, Dodge, Burn painted plain black (cause found); stale gear-K icons removed

### 1. Smudge / Soften / Dodge / Burn tools

**Files:** `plugins/tools/basictools/kis_tool_{smudge,soften,dodge,burn}.cc`, new
`plugins/tools/basictools/KrimbleEmbeddedPreset.h`

Device feedback: all four tools just paint black, like the freehand brush with a
different icon.

**Cause:** each tool looked its brush preset up by name in the resource database
(`KisResourceModel::resourcesForName`: "smudge", "DFP", "defaultPreset"). Those presets
are built into the app (`plugins/paintops/defaultpresets`, embedded as
`:/presets/<engine>.kpp`); they are not database resources, so the lookup found nothing
and the tools quietly kept the current brush. (The property names the Dodge/Burn
tools set, `CompositeOp` and `PaintOpAction`, do exist in the engine, so those were fine.)

**Fix:** a helper (`krimbleLoadEmbeddedPreset`) loads the embedded preset directly, the
same way `KisPaintopBox::defaultPreset()` does. Smudge uses the Color Smudge engine
(`colorsmudge`), Soften the filter brush ("DFP": gaussian blur, `filter`), Dodge and Burn
the default brush (`paintbrush`) with dodge/burn blending and build-up as before. The old
lookup code is kept as comments in each file.
**Verified:** all four files pass a compiler syntax-only check; the preset names and engine
ids were read from the embedded `.kpp` files. **Not verified:** a build or the tools
actually smudging, softening, dodging or burning on a device.

### 2. Stale icons

At George's instruction the old metallic gear-K icons (with the rainbow ring or the
orange "N" badge) were removed or replaced:
- Replaced with the orange K + paw: `krita/pics/branding/Next/{16,22,24,32,48,64,128,256,512,1024}-apps-krita.png`,
  `krita/pics/branding/Next/krita.ico`, and the Play Store icons
  `packaging/android/apk/ic_launcher-playstore.png` and `ic_launcher_next-playstore.png`
  (white square, K + paw centred). Large sizes are re-sharpened from the 192 pixel launcher art;
  a bigger original would still be better.
- Deleted: the unused branding sets `krita/pics/branding/default`, `Plus` and `Beta`; the
  Next variant's macOS-only `krita.icon` assets; `krita/pics/branding/krita.ico`;
  `krita/pics/branding/generate_icons.sh` (it generated the old icons); and the rainbow
  copy `sc-apps-krita-rainbow-original.svgz` added earlier today.
The build only uses `krita/pics/branding/Next` (BRANDING is forced to Next); a search of the
CMake, script, CI and resource files found no other reference to the deleted files.
**Not changed (still the old art):** the Windows installer assets in `packaging/windows/msix/pkg/Assets`
(45 files) and the macOS icon in `packaging/macos`; neither is used by the Android build.

## 2026-10-03 — Support notice that opens at start now goes to Buy Me a Coffee, not Krita

**Files:** `packaging/android/apk/src/org/krimble/android/DonationProductView.java`,
`packaging/android/apk/res/values/strings.xml`

George pointed out that the support notice shown when the app starts sent people to
Krita instead of his Buy Me a Coffee campaign.

**Found:** the start-up "Support Krimble" dialog (Java, Android) shows two fallback
buttons when Google Play billing is not available. "Join the Fund" opened
`https://fund.krita.org/` and "Donate" opened `https://krita.org/en/donations`; the
Donate text also described a PayPal donation on Krita's page.

**Changed:** the fallback list is now one product, "Donate", that opens
`https://www.buymeacoffee.com/GeorgeEdwardPurdy` (the same link the Start page and the
splash already used). Its description now reads "Help Krimble's continued development
with a donation through Buy Me a Coffee." The old two-button list and the old wording are
kept as comments. Strings file is well-formed XML.

**Already correct (checked):** the Start page "Support Krimble" link and the splash
screen link both already point to Buy Me a Coffee; the README does too.

**Still open, not changed:**
- The Google Play billing products (Supporter Subscription, one-time Supporter Pack,
  supporter badge) and the Settings menu entries "Manage Supporter Benefits..." and
  "Manage Subscriptions..." are Krita's Google Play purchase flow. Their product ids belong
  to Krita's Play Console account, so they cannot work for Krimble's own Play account. Decide
  whether to hide them for now.
- The one-time pack is still titled "Krimble 1.0.0-alpha1 Supporter Pack" and promises
  resource bundles (brushes, "Digital Atelier") that Krimble does not sell.
**Not verified:** a build or a device.

## 2026-10-03 — Toolbox resize bar and panel edge could not be grabbed: system back gesture excluded at the screen edges (theory, not confirmed)

**File:** `packaging/android/apk/src/org/krimble/android/MainActivity.java` (new method
`installEdgeGestureExclusion`, called at the end of `onCreate`; Android 10 and up)

Device feedback: the bar that resizes the toolbox, the frame edge of the panels, and
dialog edges "can no longer be grabbed", even with the phone's magnifier on.

**Theory:** Android's back gesture takes any touch that starts at the left or right screen
edge (about 30 to 40 dp, depending on the phone's gesture sensitivity setting). Since
the toolbox became a single narrow column (16 px icons, 2026-10-01), its resize bar sits
inside the left edge zone, and the panels' outer edge is in the right edge zone, so the
system takes those touches. The wider two-column toolbox kept the bar further in. Nothing
in the app's own event code was changed in a way that could explain the loss of both.

**Change:** a 64 dp wide strip at the middle of each side is excluded from system
gestures (`setSystemGestureExclusionRects`, updated whenever the window is laid out). The
system allows at most 200 dp of height per edge, so only the middle 200 dp of each edge
can be grabbed; the rest of the edge still triggers the back gesture. Java syntax checked
with a parser only.

**How to test the theory without a build:** Settings > Display > Navigation bar > switch to
"Buttons" (3-button navigation). If the bars can be grabbed then, the theory is right.
**Not verified:** a build or a device. If it is wrong, the cause is somewhere else.

## 2026-10-03 — Stale Windows and macOS icon art deleted (George's instruction)

**Deleted:** all 51 files in `packaging/windows/msix/pkg/Assets` (Windows Store tile and file
icons: Square44/71/150/310, Wide310x150, StoreLogo, fileicon, in all scales) and
`packaging/macos/KritaIcon.icns` (the macOS disk image volume icon). All were the old Krita art.
Nothing in the Android build uses them (the CMake, script and CI files searched). The Windows
and macOS packaging scripts still mention these paths (`build_msix.py` and
`macos-apptodmg.py` copy them), so building Windows or macOS packages will need new icons
first. **Not verified:** a build.

## 2026-10-03 — Build stamp: the version shown by the app now changes with every build

**Files:** new `libs/version/KrimbleBuildStamp.cmake`; `libs/version/CMakeLists.txt`;
`libs/version/KritaVersionWrapper.cpp`

George asked why the version always reads "1.0.0 beta 2" when a new build number is made
every build. Reason: the version text is a fixed string in `CMakeLists.txt`
(`KRITA_VERSION_STRING`), and the git hash shown next to it is only refreshed when CMake
reconfigures, which does not happen on most rebuilds (that is why the splash kept showing the
hash `3a607b4` and `38d3800` on newer builds).

**Change:** a small CMake script runs on every build (target `krimble_build_stamp`, part of
the normal build) and writes `krimble_build_stamp.h` with the build time as `YYMMDD-HHMM` in
Mountain time (the same clock as the APK file names) and the short hash of the checked-out
commit. `KritaVersionWrapper::versionString(true)`, the text shown in the splash screen, logs,
crash and bug reports, now reads for example
**`1.0.0-beta2 build 261003-1905 (git 76c7150)`**.
`versionString(false)`, the plain version used inside saved files and the resource database,
is not changed. If the generated header is missing the old format is used.

**Cost:** each build recompiles one small file and relinks one small library.
**Verified:** the script run here gives the right Mountain time and git hash and leaves the git
part out when there is no checkout; the version text was compiled and run with stand-in headers.
**Not verified:** a real build, and how the longer text fits on the splash screen (about 42
characters instead of 26; it is drawn right-aligned over the image). The CMake file change
makes the next build reconfigure CMake once.

### Update 2026-10-03: build number is now a counter (replaces the date-time format above)

George's decision: instead of the build time, add one to a number on every new build.
`KrimbleBuildStamp.cmake` now keeps a counter in `~/krimble-build-number.txt` (home folder of
whoever builds, so it survives a clean build folder). Each build adds one; if the file is
missing it starts at 1. To start from another number, write the number BEFORE the one wanted
into that file (for example `echo 9 > ~/krimble-build-number.txt` makes the next build 10).
The app now shows **`1.0.<counter>-beta (<Month><Day>, git <hash>)`**, for example
`1.0.58-beta (Oct3, git 76c7150)`. The "-beta" comes from the beta flag in `CMakeLists.txt`.
The plain internal version (`1.0.0-beta2`, used in saved files and the resource database)
is unchanged.
**Verified:** three runs of the script gave 1, 2, 3; a start value of 9 gave 10; the text was
compiled and run with stand-in headers. **Not verified:** a real build.
**Note:** the counter goes up on every `make` run, including one that is repeated after a
failed build. CORRECTION 2026-10-03: the packaging step ALSO adds one, because it rebuilds the project first (the first packaging try failed, so one APK took three tries and showed 4, not 1). The number in the app and the number in the APK file name stay the same, so they always match. Suggested APK name:
`Krimble-Beta2-Oct3-1447-b58.apk` (the last part is the counter).

## 2026-10-03 — Dialogs that opened above the screen are now kept on screen (Android)

**File:** `libs/ui/KisApplication.cpp` (class `KrimbleDialogKeeper`, Android only)

Device feedback: new dialogs sometimes pop up partly above the top of the screen, so their
title bar and edges cannot be grabbed.

**Change:** an app-wide event filter watches every top-level dialog. After it is shown, and again
after it is resized, the dialog is made no larger than the usable screen area and moved so its
top-left corner is on screen (checked after the current event, once the dialog has its final
size and place). Dialogs that are already fine are not touched.
**Verified (standalone Qt test, same code, 800 x 600 test screen):** a dialog asked for at y -180
ended at y 2; one at x -120 ended at x 2; one past the bottom-right moved fully inside; a
1500 x 1200 dialog became 800 x 600; a dialog already inside stayed where it was.
**Not verified:** a build or a device; `KisApplication.cpp` could not be syntax-checked as a whole in
the sandbox (the checker lacks some brush headers); the new class compiled and ran on its own.

## 2026-10-03 — Hide / Show Right Panels (menu) and tap-the-bar behaviour (Android)

**Files:** `libs/ui/KisMainWindow.cpp` (helper code marked BEGIN/END KRIMBLE RIGHT PANELS, and
the action registered next to Attach Panel), `krita/krita5.xmlgui` (one new action line, added
with George's permission)

George's idea: collapse the whole right-hand panel column without closing the toolbox; a tap on
the sizing bar hides/unhides the side panels, a drag still resizes.

**Menu:** Settings > "Hide Right Panels" (becomes "Show Right Panels" while they are hidden),
placed under Attach Panel. It hides every docked panel in the right-hand area and remembers which
ones, so exactly those come back. The toolbox (left side) is never touched. After a restart, when
nothing is remembered, "Show Right Panels" shows Tool Options, Layers and Color.
**Tap on the bar (Android only):** a press and release without moving (under 12 px) on the bar
between the canvas and the right panels hides them. A drag is not touched, so it resizes as
before. The event is never consumed.
**Handle:** while the right panels are hidden, a small handle (40 x 180) sits at the right edge of
the window, vertically centred; a tap on it shows the panels again. It disappears as soon as any
right panel is visible (also when one is shown by hand from Settings > Panels).
**Verified (standalone Qt test, the same code, a main window with a toolbox on the left and three
panels on the right):** tap on the bar hid the three right panels and left the toolbox; the handle
appeared; a tap on the handle brought all three back; a 60 px drag on the bar did nothing; a tap
on the canvas did nothing; the menu action toggled and its text changed; showing one panel by hand
hid the handle; with nothing remembered, "Show" brought back the usual three.
**Not verified:** a build or a device; `KisMainWindow.cpp` as a whole cannot be syntax-checked in
the sandbox (the extracted code compiled with the translation call stubbed). Whether Qt's own
bar-drag on a phone still works is the open problem from Oct 2; this change does not touch it.
`krita5.xmlgui` checked as well-formed XML.

### Update 2026-10-03: the side button replaces the tap-on-Qt's-bar idea (George: "long press is drag, a tap is short")

**File:** `libs/ui/KisMainWindow.cpp` (block marked KRIMBLE RIGHT PANELS; Android only)

George wanted the slender dotted button on the side made easier to press: a short tap should
hide/show the side panels, a long press should drag (resize). The first version (`eeb51e1`) only
watched taps on Qt's own thin bar, which was already hard to grab on the phone, so it is replaced.

**Now:** a real button, 52 x 170 px, laid over the bar between the canvas and the right-hand
panels, halfway down the panels. Quick tap (under 16 px of movement): hides all the right-hand
panels (the toolbox is not touched). While they are hidden the same button sits at the right edge
with an arrow; a tap shows them again. Long press (hold 0.35 s) turns it into a drag handle (it
turns brighter); moving the finger sideways then changes the width of the panel column (left =
wider). A quick sideways swipe of more than 16 px starts the drag without waiting. A long press
without moving does nothing. Dragging from the edge button while the panels are hidden shows them
first. The menu entry Settings > Hide/Show Right Panels is unchanged and works on every platform.
**Verified (standalone Qt test, the same code, realistic finger positions):** quick tap hides and
shows; long press + 90 px left grew the column as far as the layout allows (the test window was
too small for the full 90); long press + 120 px right shrank it by exactly 120; a quick 40 px swipe
widened it by exactly 40; long press without movement changed nothing; dragging from the hidden
edge button showed the panels and resized them.
**Not verified:** a build or a device (feel of the 0.35 s hold, the button size, whether it covers
something you need at the left edge of the panels); `KisMainWindow.cpp` cannot be syntax-checked
as a whole in the sandbox.

## 2026-10-03 — Seven tools removed from the top of the toolbox so Move is the first button

**Files:** `plugins/tools/karbonplugins/tools/KarbonToolsPlugin.cpp`,
`plugins/tools/tool_knife/ToolKnife.cpp`, `plugins/tools/tool_dyna/tool_dyna.cpp`,
`plugins/tools/tool_polyline/tool_polyline.cc`, `plugins/tools/basictools/default_tools.cc`,
`libs/ui/toolbox/KoToolBox.cpp`

George: the first tool should be Move; the group above the industry-standard tools should go.
That group was two toolbox sections: "Main" (Select Shapes, Calligraphy, Comic Panel Editing)
and "Shape" (Dynamic Brush, Polyline, Multibrush, Pencil).

**Changed:** the registration line of Calligraphy, Comic Panel Editing, Dynamic Brush, Polyline,
Multibrush and Pencil is commented out (same method as the Type tool in September); they no
longer exist in the app, so they have no toolbox button, tool options or shortcuts. To bring one
back, uncomment its line. **Select Shapes** (`InteractionTool`) is NOT unregistered: the tool
manager picks it as the default tool when a document opens and other code switches to it. Instead
`KoToolBox::addButton` skips it, so it has no button. The toolbox layout already skips an empty
section, so no stray divider line is expected above Move.
**Checked:** other code only mentions these tool ids as text (a list in `kis_node_manager.cpp`
and the recorder's tool list) or in `krita5.xmlgui` toolbar line 1084 (`KritaShape/KisToolMultiBrush`),
which will simply find no action; no menu depends on them.
**Syntax:** the five plugin files pass the compiler check; `KoToolBox.cpp` stops at a build-generated
`moc_` file that does not exist in the sandbox (not caused by this change).
**Not verified:** a build or a device. **Not changed:** the order of the remaining groups (the
industry-standard block now comes first; the Krita-only groups still sit after Zoom).

## 2026-10-03 — Six new toolbox icons that read like the industry-standard tools

**Files (same names, light and dark versions each, 12 files) in `krita/pics/tools/SVG/16/`:**
`krita_tool_move` (Move: pointer arrow plus the four-way cross), `tool_outline_selection`
(Lasso: loop with a rope tail), `krita_draw_path` (Pen: pen nib), `krita_tool_smart_patch`
(Healing Brush: adhesive bandage), `krita_tool_smudge` (Smudge: pointing finger),
`shape_handling` (Direct Selection: hollow arrow).

George: toolbox icons should look like the tool in the industry-standard editor where Krita's did
not. These are original drawings, not copies of that editor's artwork; they use the same
well-known symbols. 16 x 16 grid, round caps, strokes 1.2 to 1.5 so they stay readable at
16 pixels; light theme version #d2d2d2, dark theme version #373737, the same colours as the
neighbouring icons. George approved the look from an on-screen preview (72, 32 and 16 pixels).
The old icons are in git history (the commit before this one). Note that these icon names are
shared: the same file is used wherever the app shows that icon (for example the Lasso icon also
appears for the outline selection action in menus).
**Checked:** all 12 files are well-formed XML and render in the preview. **Not verified:** a build
or a device (how they look on the real toolbox).

### Update 2026-10-03: Move icon changed to the plain four-way cross

George compared sample images of the industry-standard toolbox: the current versions use a plain
four-way arrow cross (plus with an arrowhead on each end) for the Move tool; only older versions
used a pointer arrow with a small cross. He chose the plain version. `light_krita_tool_move.svg` and
`dark_krita_tool_move.svg` now draw it (rounded caps and joins, stroke 1.5, same colours as before).
The other five icons from the entry above are unchanged. **Verified:** both files are well-formed
XML; the drawing was approved from an on-screen preview. **Not verified:** a build or a device.

## 2026-10-03 — New rounded icons for all the toolbox tools (George approved the on-screen preview)

**Files:** 52 icon files in `krita/pics/tools/SVG/16/` (light and dark version of each of 26 tools,
same file names as before, so nothing in the code changes); plus `krita/pics/tools/krimble-future-icons/`
(8 files and a README, not part of the build).

Tools redrawn: Move (plain four-way cross), Rectangular and Elliptical Marquee, Lasso, Polygonal Lasso,
Magnetic Lasso (horseshoe magnet), Magic Wand, Select Similar, Crop, Eyedropper, Healing Brush, Brush,
Smudge, Blur (Soften), Dodge, Burn, Gradient, Paint Bucket, Pen, Direct Selection (Edit Shapes), Type,
Rectangle, Ellipse, Line, Polygon, Hand, Zoom. All are original drawings in one style: 16 x 16 grid,
round caps and joins, strokes 1.2 to 1.5 so they stay readable at 16 pixels. Light theme version is
#d2d2d2, dark theme version #373737 (the colours of the icons they replace). They use the familiar
symbols for each tool and are not copies of any other program's artwork.
**Kept for later, not in the build:** Pencil, Path Selection, Eraser, Clone Stamp (in
`krimble-future-icons`), for when those tools are added.
**Note:** an icon name is used wherever the app shows that icon, so some menus that use the same icon
(for example the selection tools in the Select menu) will show the new drawing too.
**Verified:** every file is well-formed XML; the 27 light icons were rendered from the written files at
32 and 16 pixels and checked. **Not verified:** a build or a device.

## 2026-10-04 — Side button appeared over the toolbox instead of on the bar between the canvas and the panels

**File:** `libs/ui/KisMainWindow.cpp` (block marked KRIMBLE RIGHT PANELS, Android)

Device feedback on build b4: the new side button sat over the toolbox (top left), not on the bar by the
right-hand panels. Its drag did resize the panel column, so the button itself worked.

**Cause:** the button's place was worked out when the window asked for a layout, which happens BEFORE
the window has actually positioned its panels. The panels still had empty positions (left edge 0), so
the button was put at the left edge of the window. In my earlier test the layout had already settled,
so it looked right there.
**Fix:** every change (window layout, resize, show; and a move, resize, show or hide of any right-hand
panel) now schedules the update for one step later, after the layout is done. The right-hand panels are
watched directly so the button follows them. The button is also a little smaller (44 x 150 px).
**Verified (standalone Qt test, same code):** the button stays on the bar, centred on the panels' left edge
and well clear of the toolbox, at the start, after the panels are narrowed, and after the window is
widened and narrowed; tap, long press, swipe and the edge button behave as before. **Not verified:** a
build or a device.

### Update 2026-10-04: the side button is a thin sliver again (George: "a tiny thin sliver on the edge of the panel")

**File:** `libs/ui/KisMainWindow.cpp` (block marked KRIMBLE RIGHT PANELS, Android)

The first version was a big 52 x 170 pill, and the 44 x 150 one after it was still too big. Now only a
**6 px wide, 90 px tall rounded sliver** with three small dots is drawn, flush against the left edge of the
right-hand panels (a little over 2 mm on a phone). The touch area behind it is **22 x 140**: 18 px on the
canvas side of the panels' edge and 4 px over it, so the sliver is easy to press while the panels' own
content is barely covered. While the panels are hidden it becomes a slightly wider tab (14 x 90) with an
arrow on the right edge of the window. Behaviour is unchanged: tap hides/shows the right panels, long press
or a sideways swipe resizes.
**Verified (standalone Qt test, same code):** it stays on the bar after the panels and the window change
size, clear of the toolbox; tap, long press, swipe and the edge tab work; the drawn shape was rendered and
looked at. **Not verified:** a build or a device (how easy it is to press).

## 2026-10-04 — Dialogs: top strip kept free, and two-finger drag moves any dialog (Android)

**File:** `libs/ui/KisApplication.cpp` (class `KrimbleDialogKeeper`, extended)

George: the New file window still opened above the top of the screen so it could not be grabbed, and asked
for the two-finger window drag that had been planned. (Neither the plan nor the code is in my own notes; the
unmerged branch `move/two-finger-window-mover` holds an early version: a file in `src/ui/`, a folder that does
not exist in this project, never added to the build. That idea is rebuilt here.)

**1. Top strip.** A dialog is kept at least 48 px below the top of the usable screen area (or lower, if
the window's own title bar is taller). Before, only the dialog's own area was kept on screen; a title bar
drawn just above it could still be off the top.
**2. Two-finger drag.** Put two fingers anywhere on a dialog and drag: the whole dialog moves with the
middle point between the fingers. One finger behaves exactly as before (taps still reach buttons). When the
fingers lift, the dialog is brought back on screen if it was dragged too far. The touches are watched on the
dialog's window, which sees every finger; a widget only sees them if it accepts the first touch, which
would have stopped one-finger taps from reaching buttons. (My first attempt did it that way and failed in test.)
**Verified (standalone Qt test, same code, 800 x 600 test screen):** a dialog asked for at y -180 or y 0 ended
at y 48; one bigger than the screen was cut to fit; a two-finger drag of (+100, +50) moved the dialog by exactly
that; a drag far above the top came back to y 48 when the fingers lifted; a one-finger touch was not taken.
**Not verified:** a build or a device (real touch events on the phone; whether the title bar sits above or
inside the dialog's area). `KisApplication.cpp` as a whole cannot be syntax-checked in the sandbox.

## 2026-10-04 — Attach Panels on/off replaces Detach/Attach; floating panels: no empty bands, smaller handle, smaller minimum size

**File:** `libs/ui/KisMainWindow.cpp` (Android parts as before; the menu item works everywhere)

George (build b4): Attach Panel > Layers made the Layers panel vanish; Detach and Attach had become
unpredictable; he asked for a single on/off item that restores the standard attach behaviour. Also: floating
panels had big empty bands at the top and bottom, the Layers panel could not be made small enough, and the
corner handle was too big.

**Attach Panels (Settings menu, tick box).** Replaces the Detach Panel and Attach Panel submenus (the code of
both is kept, commented or hidden; the menu entry keeps the old action name, so `krita5.xmlgui` is unchanged
and the hidden Detach Panel line simply shows nothing). ON (default) = the standard behaviour: a floating
panel dragged to the edge of the window docks there. OFF = floating panels never snap anywhere. The choice is
remembered (`Krimble` group, key `AttachPanels`) and applies at once to panels that are already floating. A panel
is detached with its own float button. The float button is back to the standard behaviour (the "dock where it
was put" code from 7945b55 is switched off, `if (false && ...)`, the code stays), which is what removes the
unpredictable parts.
**Empty bands:** the extra 14 px added above and below the title bar of a floating panel is now 0 (old value kept in a comment).
**Corner handle:** 36 px -> 26 px, and the strip reserved under the panel for it 36 -> 26.
**Smaller minimum size:** while a panel floats its content gets a minimum of 150 x 120 and its layout stops
forcing the larger one; docked again, the old minimum and layout setting are restored. Test with a panel whose
buttons force 318 x 299: floating it can be resized to 200 x 160 and its minimum hint is 150 x 120; docked
again the minimum is 318 x 299. With very small sizes some controls will be cut off (clipped), not squeezed.
**Verified (standalone Qt test):** the minimum size change and restore, and the allowed areas for ON (15 = can
dock) and OFF (0 = never docks). **Not verified:** a build or a device (touch-dragging a floating panel to the
edge, how the clipped small Layers panel looks). `KisMainWindow.cpp` cannot be syntax-checked as a whole here.

### Update 2026-10-04: Magnetic Lasso icon redrawn as a horseshoe magnet

George did not recognise the U-shaped magnet. `light_tool_magnetic_selection.svg` and
`dark_tool_magnetic_selection.svg` now draw a classic horseshoe magnet: an arch that curves over the top and
is open at the bottom, a lighter body (55% opacity) and solid bright pole tips. George chose this version
(option D of three) from an on-screen preview. Same file names, so nothing in the code changes.
**Verified:** both files are well-formed XML; the drawing was rendered at 96, 32 and 16 pixels. **Not verified:**
a build or a device.

## 2026-10-04 — Brushes panel switched off in the build (George's decision)

**File:** `plugins/dockers/CMakeLists.txt` (line 8: `# add_subdirectory(presetdocker)`, same method as the six panels
switched off on 2026-10-02)

The Brushes panel (the Brush Presets docker) kept opening by itself when a file was opened, even after the app data
was cleared; code review found nothing that shows it, and the first-launch layout does not contain it. George chose to
leave it out of the build. The plugin source is untouched; to bring the panel back, remove the `#` and rebuild.
**What is lost:** the Brushes panel and its list of brush presets. Not checked: which other way there is on the phone to
pick a brush preset (for example a preset button in the toolbar); check on the device.
**Build note:** switching a plugin off does not remove its library from the install folder; delete
`libkritapresetdocker*` from `~/kwd/krita/_install/lib` (and any copy under `lib/kritaplugins`) before packaging so it
is not in the APK. CMake reconfigures once on the next build.
**Not verified:** a build or a device.

### Update 2026-10-04 (2nd pass): corner handle and the top and bottom of floating panels smaller again

George: the corner handles are too big and the top and bottom edges of the windows are way too big.
**Changed:** corner handle 26 -> 20 px (it was 36), and the strip reserved for it under the panel 26 -> 20 (it was 36);
the default size of a panel that floats 60% x 75% -> 50% x 50% of the screen's shorter side, so there is less unused
space around the content (earlier this pass the extra title bar padding of 14 px above and below was already set to 0).
The old values are kept in comments. **Verified:** the handle was drawn at phone scale next to the old sizes.
**Not verified:** a build or a device. **Not changed:** the title bar's own height (set by its buttons); if the top is
still too tall after the next build, that is the next place to cut.

## 2026-10-04 — Detach / Attach features switched off; panels behave as before them

**File:** `libs/ui/KisMainWindow.cpp`

George (build b4): the Attach feature threw panels to the opposite side of the screen; "we should just disable that
feature and restore their previous behavior. All I really wanted was tolerances adjusted."
**Changed:** the "Attach Panels" menu item (which replaced the Attach Panel / Detach Panel submenus the day before)
is hidden and disabled, and its effect on the panels is removed: a panel's allowed dock areas are always "all", floating
or not, as before the 2026-10-02 Attach/Detach work. The Detach Panel submenu was already hidden; the "dock where it was
put" code on the float button stays switched off. The old code is all still there, commented or hidden. The other
changes to floating panels stay (corner handle, no extra title padding, smaller minimum and default size).
**Not changed:** `krita5.xmlgui` (the menu lines are still there; the actions are hidden, so they show nothing).
**Not verified:** a build or a device. **Open question for George:** which tolerance he wants adjusted (see the reply).

### Fix 2026-10-04: build error in KisApplication.cpp (missing QWindow include on Android)

The first build with the dialog two-finger drag stopped with 6 errors "member access into incomplete type 'QWindow'"
in `libs/ui/KisApplication.cpp`: the QWindow header was only included in the Windows part of the file. Added
`#include <QWindow>` to the Android include block. A syntax check of the file no longer reports QWindow; the
remaining messages (KisGbrBrush and similar) are the sandbox lacking brush headers and are not caused by this change.

## 2026-10-04 — Menu labels from krita5.xmlgui now show; sliver no longer drawn over the toolbox

**File:** `libs/ui/KisMainWindow.cpp`

George (build b8): "There is still a sizing gadget on top of the toolbox. Get rid of that thing." and "this is not the File menu
from my most recent edits. It says Quit instead of Exit."

**1. Menu labels.** The `<text>` that krita5.xmlgui gives to an `<Action>` was never used: an action's label comes from its own
definition (the `.action` files or the code), so edits like Quit -> Exit did not show. A comparison found 157 actions whose
label in krita5.xmlgui differs from their definition (most of George's Photoshop-style names, e.g. "Layer via Copy",
"Crop to Selection", "Exit"). New code (`krimbleApplyXmlGuiLabels`) reads the loaded menu file and sets each action's label
to the `<text>` given for it. It runs once the window is built, whenever the active view changes, and when a tool is chosen
(tool actions appear later). From now on a label typed in krita5.xmlgui is the label in the app. Excluded: "Hide/Show Right
Panels" (its text changes by itself) and the hidden Attach/Detach items. Mnemonics (&) are only present if typed in the file.
**2. The sliver over the toolbox.** The button was placed from the left edge of the panels, but a panel hidden behind a tab (or
not yet laid out) has an old position with its left edge at 0, which pulled the button over the toolbox. Now only panels that
lie to the right of the middle of the canvas area are used, and the toolbox is never counted as a right-hand panel; if there is
no usable panel the button is hidden.
**Verified (standalone Qt test, same code):** file_quit became "Exit", an action without a label in the file kept its text, the
Right Panels item kept its own text; with a tabbed panel in the test window the button stayed on the bar and clear of the
toolbox. **Not verified:** a build or a device; `KisMainWindow.cpp` cannot be syntax-checked as a whole here (the signal
connection `changedTool(KoCanvasController*)` was checked against the header). Note that George's 157 label edits will now all
appear, including ones he may not have meant for the running app.

## 2026-10-04 — Side button (sliver) switched off

**File:** `libs/ui/KisMainWindow.cpp`

George: "Just get rid of it. The sidebar gadget is just causing problems. It's a failure." The button that sat at the
edge of the right-hand panels (tap = hide/show, long press = resize) is no longer created: the block that makes it is
turned off with `#if 0` (it was `#ifdef Q_OS_ANDROID`; change it back to bring the button back). Nothing is drawn at the
panels' edge any more. Kept: the menu item Settings > Hide Right Panels / Show Right Panels, and the code for the button.
George also reported that the floating panels are much better in the same build: the smaller corner handle, and they snap
into and out of place in a more natural way.
**Not verified:** a build or a device. (The preprocessor lines of the file balance.)

## 2026-10-04 — Side sliver back, now hung on the canvas area (George: "I want to test it. I know Ps users will want it")

**File:** `libs/ui/KisMainWindow.cpp` (Android)

George asked whether the sliver could be made to always sit on the left edge of the right-hand panels, and said he wants
to test it (including the mode where the pictures are floating windows). The button is switched on again (`#ifdef Q_OS_ANDROID`)
with a new way of placing it: it is a child of the canvas area (`d->mdiArea`) and sits at the area's right edge, which is
where the right-hand panels begin (or the screen edge while they are hidden). Its place comes from the canvas area's own size and
is refreshed whenever that area is resized or shown, or the window lays out, so it no longer depends on where the panels are
reported to be (the cause of the earlier "over the toolbox" failure). On the welcome page, where the canvas area is not shown,
the sliver is not shown. Same look and behaviour: a 6 x 90 px sliver, 22 x 140 touch area; tap hides/shows the right panels,
long press or a sideways swipe resizes them. The old placement code is kept as comments.
**Verified (standalone Qt test, same code, with a welcome page and a picture area in a stack):** the sliver stayed at the left
edge of the panels with the window widened, with the panels narrowed, in floating picture-window mode and while the panels were
hidden and shown again; it was hidden on the welcome page and never near the toolbox. **Not verified:** a build or a device.
In floating-window mode the sliver lies above a picture window dragged to that edge and covers a strip about 22 px wide.

## 2026-10-04 — Preferences window far too big: every page now scrolls (Android); sliver switched off again

**Files:** `libs/ui/dialogs/kis_dlg_preferences.cc` (Android), `libs/ui/KisMainWindow.cpp`

**Sliver:** George: "No. Comment out the sliver. I'll worry about it later." The block that creates it is `#if 0` again
(change it to `#ifdef Q_OS_ANDROID` to bring it back); the new canvas-area placement code stays in the file.

**Preferences window.** George (build b8, screenshots): "The Preferences window is waaaaaaaay too big." The 2026-10-02 fix
(General page minimum width 0, and the window capped to the screen) was not enough: the window still came out wider than the
screen, because the pages themselves (the General page with its row of eight tabs and long check box texts, the long labels of
the page list, other pages) each ask for more room than a phone screen has, and a window cannot be smaller than the biggest page.
**Fix:** every page is put inside a scroll area (`krimblePrefsPage`, Android only; other platforms use the page as it is): the
11 pages (General, Keyboard Shortcuts, Canvas Input Settings, Display, Color Management, Performance, Tablet settings,
Canvas-only settings, Pop-up Palette, Author, and the pages added from the preference set registry). A page too big for the window now scrolls
instead of making the window grow. The earlier lines are kept as comments next to each page.
**Verified (standalone Qt test with a page of eight tabs and long check box texts):** used as it is, the dialog's minimum width
was 875; inside a scroll area it was 91, and the dialog could be resized to 300 x 400 with the page scrolling.
**Not verified:** a build or a device; the file cannot be syntax-checked as a whole in the sandbox (it needs the external lager
headers). The page list on the left (icons and long labels) is not changed; if the window is still too wide, that is next.

## 2026-10-04 — Default Multiple Document Mode is now Subwindows (floating picture windows)

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/dialogs/kis_dlg_preferences.cc`

George: "I want the default multiple document mode to be sub Windows" (Settings > Configure > General > Window > Multiple
Document Mode > Subwindows). The default of the setting `mdi_viewmode` was tabs in three places; now it is
`QMdiArea::SubWindowView` in all three: where the main window reads it (`KisMainWindow.cpp`), where the Preferences window
shows it, and where "Restore Defaults" sets it (`kis_dlg_preferences.cc`). The old lines are kept as comments. The option
list order in the settings form (Subwindows first, Tabs second) matches the value, so the Preferences window will show "Subwindows".
**Note:** this only changes the default. A phone that already has the setting saved keeps its saved choice; George clears app
data before each test, so a fresh start uses the new default. **Not verified:** a build or a device (how a new picture opens
as a floating window on the phone).

### Update 2026-10-04: Magic Wand icon redrawn

George: the wand "looks like a tube you keep a toothbrush in"; more sparkles; taper the sparkle end. The icon
(`light_tool_contiguous_selection.svg` and `dark_tool_contiguous_selection.svg`, same names) is now a slim wand that
tapers from a round handle to a fine point, with a small cluster of sparkles (four star shapes, the largest about 2 units,
and three tiny dots) at the tip, plus two trailing dots at the top left. George chose it step by step from on-screen previews
("Very nice"). **Verified:** both files are well-formed XML; rendered at 96, 32 and 16 pixels. **Not verified:** a build or a device.

## 2026-10-04 — View menu: Canvas Rotation and Mirror View submenus

**File:** `krita/krita5.xmlgui` (edited at George's request: "Can you make those two submenus in the View menu for me")

The six loose canvas actions in the View menu were moved into two submenus; nothing was added or removed, each action still
appears exactly once, and the file is still well-formed XML.
- **View > Canvas Rotation:** Clockwise (`rotate_canvas_right`), Counterclockwise (`rotate_canvas_left`), a separator,
  Reset Rotation (`reset_canvas_rotation`).
- **View > Mirror View:** Mirror Canvas (`mirror_canvas`), Around Cursor (`mirror_canvas_around_cursor`),
  Around Canvas (`mirror_canvas_around_canvas`).
They sit where the loose entries were: after Proof Colors and Gamut Warning, before Reset Display. The short item labels
(like the ones in Snap To) show in the app after the next build, because of the label fix of 2026-10-04.
**Not verified:** a build or a device.

### Update 2026-10-04: View menu: Screen Mode and Show submenus (George: "Yes")

**File:** `krita/krita5.xmlgui` (edited at George's request)

Eight loose View menu items were moved into two submenus, as in the industry-standard View menu. Every action still appears
exactly as often as before (323 action entries before and after, the same set; the file is well-formed XML; no comments added).
- **View > Screen Mode:** Full Screen (`fullscreen`), Canvas Only (`view_show_canvas_only`), Detached Canvas (`view_detached_canvas`).
  Placed right after the Zoom submenu.
- **View > Show:** Guides (`view_show_guides`), Grid (`view_grid`), Pixel Grid (`view_pixel_grid`), Reference Images
  (`view_toggle_reference_images`), Rulers Track Mouse (`rulers_track_mouse`). Placed after Lock Guides, before Snap To.
Left at the top level: Show Status Bar, Rulers, Lock Guides, Proof Colors, Gamut Warning, Reset Display, the Wrap Around items,
Level of Detail Mode, Show Painting Assistants, Show Assistant Previews, Palette and Refresh. Short labels show in the app after
the next build. **Not verified:** a build or a device.

## 2026-10-04 — Detach Panel and Attach Panel menus are back

**File:** `libs/ui/KisMainWindow.cpp`

George (after testing build b12: "So far it looks really good, but I think I want the detach and attach panels menus too").
- **Settings > Detach Panel:** visible again (the line that hid it is commented out). It lists the docked panels; choosing one
  unlocks it if needed and makes it float, as before.
- **Settings > Attach Panel:** a submenu again (the "Attach Panels" on/off tick box is switched off with `#if 0`, code kept). It
  lists the panels that are floating; choosing one docks it with the STANDARD behaviour, back into its previous dock area
  (`setFloating(false)`). The older "dock where it was put" version (nearest side, between the panels above and below) is NOT used:
  it threw panels to the opposite side and made Layers vanish. The commented copy of it stays in the file.
Both menus use the existing action names, so `krita5.xmlgui` is unchanged (it still has the two entries).
**Verified (standalone Qt test):** the Attach Panel list showed a floating "Layers" panel; choosing it docked it back into the right
area where it started and the list then said "No floating panels". **Not verified:** a build or a device; the file cannot be
syntax-checked as a whole in the sandbox.

## 2026-10-04 — Default Multiple Document Mode back to tabs

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/dialogs/kis_dlg_preferences.cc`

George (after testing b12): "I think the sub Windows are a little wonky. Go back to tabs by default." The three default
places changed earlier the same day (see the entry "Default Multiple Document Mode is now Subwindows") are tabs again
(`QMdiArea::TabbedView`); the Subwindows lines are kept as comments. Subwindows stays a choice in Settings > Configure >
General > Window > Multiple Document Mode. **Not verified:** a build or a device.

## 2026-10-04 — Toolbox docks only close to a side of the window (docking tolerance)

**File:** `libs/ui/KisMainWindow.cpp` (Android; new class `KrimbleDockTolerance`)

George (after testing b12, which "looks really good so far"): "I had a bit of an issue with the toolbox wanting a bit too desperately
to dock." While the floating toolbox is dragged, it may now dock in a side of the window only when the finger is within 36 px
(logical; about 90 px on the Galaxy A26 screen, roughly 7 mm) of that side. Anywhere else it stays a floating window. This is done by
changing the dock areas the toolbox is allowed in as the finger moves (left side near the left edge, right near the right edge, and
likewise top and bottom); when the finger is lifted, all areas are allowed again. Only the toolbox is affected (found by its name
"ToolBox"); a docked toolbox and all other panels behave as before. The distance is `KrimbleDockTolerance::Distance`.
**Verified (standalone Qt test, same code):** finger in the middle or 100 px from a side: no dock area allowed; 20 px from the left side:
left only; 20 px from the right side: right only; top-left corner: left and top; after lifting the finger: all areas again; a docked
toolbox untouched. **Not verified:** a build or a device (whether it feels right while dragging by touch; the distance may need tuning).

## 2026-10-04 — "Font Families" is now just "Fonts" in the interface

**Files:** `libs/ui/KisApplication.cpp`, `libs/widgetutils/kis_font_family_combo_box.cpp`

George: "Font families in Krita is a LIE. IT'S JUST FONTS." The only two places where the user sees the word "family" were changed:
the name of the font resource type in the resource manager, "Font Families" -> "Fonts", and the tooltip of the font box in the text
tool, "Font Family" -> "Font". The old lines are kept as comments. Names inside the code (`KoFontFamily`, `FontFamilies`, the
resource folder) are not user-visible and are unchanged. **Not changed:** how the font box groups fonts (it still pairs a font box
with a style box); making it a flat list is a larger change and has not been requested in detail yet. **Not verified:** a build or a device.

### Fix 2026-10-04 (2nd): the File menu still said "Quit" on the phone (build b12)

**File:** `libs/ui/KisMainWindow.cpp` (`krimbleApplyXmlGuiLabels`)

George's screenshot of build b12 showed File > Quit although krita5.xmlgui says Exit for `file_quit`, while other labels from the menu
file (for example View > Rulers) did show. The first version of the label code only looked at actions that are children of the
window, but actions belong to the action collection, which is not necessarily below the window. Now it also uses the actions that
are really in the menus (`menu->actions()`: the objects that are drawn), and it runs once more right before any menu is shown
(`QMenu::aboutToShow`, connected after start-up). **Verified (standalone Qt test):** an action owned outside the window and shown in a
menu changed from "&Quit" to "Exit". **Not verified:** a build or a device.

## 2026-10-04 — Number fields on Android: Krimble's number pad on tap, long press + drag to change, arrows decorative

**File:** `libs/ui/KisApplication.cpp` (Android; new classes `KrimbleNumberPad` and `KrimbleNumberFields`)

George: tapping a number brought up Cut/Copy/Paste over the view, so numbers could not be typed; the up/down arrows were too small to tap;
"I want to be able to long press, then drag up and down to change a numeric value." and "Make the up down arrows decorative but actually do
the drag. 1 and 2." Cause: the bar-style fields (`kis_slider_spin_box_p.h`, `startEditing()`) select all their text when editing starts, and plain
number boxes use the normal text editing; on Android both bring up the system selection handles and the Cut/Copy/Paste bar.
**Now, for every number field** (all QSpinBox / QDoubleSpinBox, including the bar-style ones such as Opacity and Font Size):
- **Short tap:** Krimble's own number pad opens (big buttons 0-9, minus, decimal point, backspace, Clear, Cancel, OK). The system text editing,
  handles and Cut/Copy/Paste bar are never used. What is typed replaces the value; it is limited to the field's range; Cancel changes nothing.
- **Long press (350 ms), then drag up or down:** the value changes (one step per 9 logical px; up raises it); wide ranges move faster
  (x5 above 500 steps, x20 above 2000); a small tip shows the value; lift the finger to stop. Moving before the long press does nothing.
- **Arrows:** still drawn, but decorative: touching them does the same as touching the field.
The old way of sliding a bar-style field sideways with the finger is replaced on Android by this (every touch on a number field is handled
here). The numbers are the constants at the top of `KrimbleNumberFields` (`LongPressMs`, `MovePixels`, `TapSlop`). Not touched: the text
tool's on-canvas text (the "bubbles"), other platforms, text fields that are not number fields.
**Verified (standalone Qt test, same code):** long press + drag up 45 px moved 50 -> 55 and down below the start -> 45; a decimal box moved in
its own step (1.5 -> 2.5 with step 0.25); a short tap opened the pad without changing the value; typing 7 2 OK gave 72; 999 in a 0-100 field gave
100; 3.75 worked in a decimal box; Cancel kept the value; a swipe before the long press did nothing. A compile problem (a cast needing a Qt macro)
was found and fixed in the test. **Not verified:** a build or a device (how it feels under a finger, and the bar-style classes `KisSliderSpinBox` /
`KisDoubleSliderSpinBox` themselves, which the test could not include).

### Update 2026-10-04: "Font Family" label in the Text Properties panel is now "Font"

**File:** `plugins/dockers/textproperties/qml/FontFamily.qml`

The earlier wording change ("Font Families" -> "Fonts", commit d0aa690) missed the label that George actually sees in the Text Properties panel,
which lives in this QML file (my search had covered C++ and form files only). The panel label "Font Family" is now "Font"; the old line is
kept as a comment. The label "Font Style" is unchanged. **Not verified:** a build or a device.

## 2026-10-04 — Text Properties panel, step 1: Character and Paragraph in the order of the reference panels

**Files:** `plugins/dockers/textproperties/qml/TextProperties.qml`, `TextPropertyBaseList.qml`, and the property files `FontKerning.qml`,
`LetterSpacing.qml`, `BaselineShift.qml`, `TextDecoration.qml`, `TextTransform.qml`, `Language.qml`, `TextRendering.qml`, `TextIndent.qml`

George sent four reference screenshots of the industry-standard Character, Paragraph, Character Styles and Paragraph Styles panels and the Type
options bar and said "We need to match this structure"; step 1 of the agreed order (re-layout with what exists today):
- **Tabs:** Character, Paragraph, Preset (was Paragraph, Character, Preset). The Character list is first in the stack to match.
- **Order of the groups** (all 32 kept; the old order is in a comment): Character tab: Font, Font Style, Font Size, Line Height, Font Kerning,
  Letter Spacing, Font Size Adjust, Word Spacing, Baseline Shift, Text Decoration, Text Transform, the OpenType groups (Capitals, Position,
  Ligatures, Numeric, East-Asian, Features), Language, Text Rendering (anti-aliasing). Paragraph tab: Text Align, Text Indent, Direction,
  Writing Mode, Text Area, then the line and word breaking options.
- **Moved to the Character tab:** Language (was both) and Text Rendering (was Paragraph), as in the reference Character panel.
- **Shown by default** (before only Font, Font Style, Font Size, Line Height and Text Align were): Font Kerning, Letter Spacing, Baseline Shift,
  Text Decoration, Text Transform, Language, Text Rendering, Text Indent. The user can still hide groups with "Add Property".
**Verified:** all edited files pass the QML syntax checker and every group is still in the list. **Not verified:** a build or a device (how the
panel looks and fits). **Not done yet (later steps):** two controls side by side in one row, the icon row of faux bold/italic, caps, super/subscript,
underline and strikethrough, the scale boxes, color, space before and after, hyphenation, separate Character Styles and Paragraph Styles
lists, the Type options bar.

## 2026-10-04 — "Type Options" toolbar (first version): font and style, size, align left / center / right

**Files:** new `libs/ui/KisTypeOptionsBar.h` / `.cpp`; `libs/ui/KisTextPropertiesManager.h` / `.cpp` (new signal `sigInterfaceChanged`);
`libs/ui/KisViewManager.cpp` (creates the bar); `libs/ui/CMakeLists.txt`; `krita/krita5.xmlgui` (new block `TypeOptionsBar`, added at George's request:
"We have custom toolbars, so it should be possible to get close", then "Yes" to the block)

A horizontal bar of the most used text controls under the menu bar, shown only while the Type tool is active, like the options bar of the
industry-standard editors (George's reference screenshots). The controls are widget actions in the normal action collection (like the brush
controls), so the toolbar can be rearranged with Customize Toolbar. They read and write through the same text properties interface as the Text
Properties panel (`KoSvgTextPropertiesInterface`), so the panel and the bar agree, and everything goes through the tool's own undo handling.
**In this first version:** `type_font` (font and style boxes), `type_size` (in points, using the document's resolution), `type_align_left`,
`type_align_center`, `type_align_right` (physical left / center / right). The font box sets family, weight and italic. The bar is hidden (actions and
toolbar) unless the Type tool is active; the toolbar overflow button "»" holds whatever does not fit on a narrow screen.
**Not made yet** (from the block George approved): orientation, anti-aliasing, color, Text Properties panel button, cancel and confirm. The
block in `krita5.xmlgui` lists only the controls that exist; the others get added to it as they are made.
**Observed, not changed:** the existing shortcut table in `SvgTextShortCuts.cpp` maps "svg_align_right" to AlignStart and "svg_align_left" to
AlignEnd, which reads reversed for left-to-right text. The new bar uses AlignLeft / AlignRight (which are defined as the physical sides).
**Verified:** the weight mapping numbers (Qt weight -> CSS weight) and the menu file (well-formed); the syntax checker reported no problem located in
the new file (it could not follow every header). **Not verified:** a compile of the library, a build or a device. This is new code in the main
library, so a compile error on the server is possible; how it looks and fits on the phone is untested.

### Update 2026-10-05: toolbox hand (Pan) icon is solid

George asked to see a solid hand for the toolbox and chose option A of three from an on-screen preview: four slim fingers and a thumb sweeping to the lower left,
filled instead of outlined. `light_tool_pan.svg` and `dark_tool_pan.svg` (same names, so nothing in the code changes). **Verified:** both files are well-formed XML;
rendered at 80, 32 and 16 pixels. **Not verified:** a build or a device.

### Update 2026-10-05: toolbox Pen icon is a solid nib with a split point

George asked for a solid pen, then "a little pointier", then "the end should stick out more" and to "get that shape on the end, the point" (a metal nib). He chose option A4
from an on-screen preview: a tilted solid nib with a round collar, a round breather hole, a slit that runs through the very end so the tip is two small rounded tines,
and a long narrow point that reaches the edge of the icon. `light_krita_draw_path.svg` and `dark_krita_draw_path.svg` (same names, so nothing in the code changes).
**Verified:** both files are well-formed XML; rendered at 88, 32 and 16 pixels. **Not verified:** a build or a device.

### Update 2026-10-05: toolbox Smudge (pointing finger) icon is solid

George asked for the pointing-finger icon to be solid like the hand and pen. After a redrawn hand (rejected: lower hand too small) he asked for the existing outline simply filled in,
then "Too thick", and chose S3: the same paths as the old outline icon, filled with no edge line (so the shape is the centre line of the old outline), plus a small filler that closes
the notch where the finger meets the hand. `light_krita_tool_smudge.svg` and `dark_krita_tool_smudge.svg` (same names, so nothing in the code changes). **Verified:** both files are
well-formed XML; rendered at 72, 32 and 16 pixels. **Not verified:** a build or a device.

### Update 2026-10-05: toolbox arrow (Edit Shapes) icon is solid

George asked for "the little arrow icon" to be solid too and chose F2: the old outline arrow's own path, filled, with a thin 0.6 edge line (F1, with no edge, was the slimmer alternative).
`light_shape_handling.svg` and `dark_shape_handling.svg` (same names, so nothing in the code changes). Note: the Select Shapes arrow in the same toolbox style is also solid, so the
hollow/solid pair is gone. **Verified:** both files are well-formed XML; rendered at 72, 32 and 16 pixels. **Not verified:** a build or a device.

### Fix 2026-10-05: link error in the Type Options toolbar (undefined symbol KoSvgText::parseFontStyle)

The build of 2026-10-05 compiled everything but stopped at the link step: `undefined symbol: KoSvgText::parseFontStyle(QString const&)`. The function exists in the flake library but
is not exported, so the main library could not call it. `KisTypeOptionsBar.cpp` now builds the value directly (`KoSvgText::CssFontStyleData(QFont::StyleItalic / StyleNormal)`, an inline
type), as the library's own code does elsewhere. The old call is kept as a comment. **Not verified:** the link (needs a build); no other undefined symbol was reported.

### Update 2026-10-05: toolbox Brush icon redrawn (long-handled brush, hollow metal sleeve, original hair)

George asked for the brush end to be solid, then for the handle to taper (thick next to the bristles, thin at the far end), the hair on the end of the handle, "like the long ones" in the photos of
real paintbrushes, a long thin metal sleeve drawn as an outline that tapers toward the hair, and his original curved hair shape only moved and made smaller and centred on the stick. He chose option
Z1 ("Z1 is close to perfect", then "Z1 is still best so far" after two teardrop-shaped alternatives, D1 and D2, which were not used). `light_krita_tool_freehand.svg` and
`dark_krita_tool_freehand.svg` (same names, so nothing in the code changes). The icon is one filled shape (handle, hollow sleeve and hair, with a cut-out for the sleeve), made as plain polygons so it
looks the same in every renderer (checked in two). **Verified:** both files are well-formed XML; rendered at 16, 32 and 88 pixels. **Not verified:** a build or a device.

### Update 2026-10-05 (2nd): toolbox Brush icon, final proportions (K4)

George asked for the metal sleeve to be a little smaller with nothing else shrunk (K1/K2 shown), picked K1, then asked to move the hair down a little and lengthen the sleeve a little (K3/K4 shown) and chose
K4: the sleeve is shorter and thinner than Z1 (half-width 0.68 -> 0.47, wall 0.28), runs from 7.9 to 12.2 along the brush so it meets the hair, and the hair (original curved shape at 62%, centred on the stick) sits
0.8 further down the brush. Handle and hair shape and size are unchanged. Replaces the Z1 version saved earlier today. `light_krita_tool_freehand.svg` and `dark_krita_tool_freehand.svg`
(same names). **Verified:** both files are well-formed XML; rendered at 16, 32 and 72 pixels. **Not verified:** a build or a device.

## 2026-10-05 — Brushes panel back, and a "Brush Options" toolbar

**Files:** `plugins/dockers/CMakeLists.txt`, `krita/krita5.xmlgui` (edited with George's permission: "Yes")

George, after testing build b17: "We may have to bring back the brushes. I was trying to use brushes and had no controls for size or softness or other properties."
When the Brushes panel (the `presetdocker` plugin) was switched off on 2026-10-03 the only brush controls left on screen went with it: the brush controls (presets button, brush settings
editor, size / opacity / flow sliders, blend mode, mirror tools) exist as toolbar widget actions in `KisPaintopBox`, but no toolbar in `krita5.xmlgui` listed them.
- **Brushes panel:** `add_subdirectory(presetdocker)` is active again (the earlier decision to leave it out is reversed; the comment above the line says so).
- **Brush Options toolbar:** new block `BrushOptions` in `krita5.xmlgui`, next to the Type Options toolbar: `show_brush_presets`, `show_brush_editor`, `brushslider1`, `brushslider2`, `brushslider3`,
  `composite_actions`, `mirror_actions` (all names exist in the code and appear once in the file). Size, opacity and flow are number fields, so the number pad and long-press drag apply to them.
  Edge softness is in the brush settings editor (Brush Tip) for now; quick softness minus / plus buttons are a later step.
**Build note:** the plugin comes back, so the next build reconfigures CMake and builds the plugin; `libkritapresetdocker*` will be in the APK again (the APK check line that wants none for it must be dropped).
**Not verified:** a build or a device (that the toolbar shows the controls, and how they fit).

## 2026-10-05 (2nd) — Color wells on the Brush Options toolbar; the toolbox's color wells sit right under the tools

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`; new `libs/ui/KisBrushColorWells.h` / `.cpp`; `libs/ui/KisViewManager.cpp`; `libs/ui/CMakeLists.txt`; `krita/krita5.xmlgui`

George: "I think it might be a good idea to have color wells on the brush toolbar. Also the ones under the toolbox seem to end up way down low instead of right under the toolbox."
- **Toolbox:** the tool grid had stretch 1 in the panel's layout, so it took all the height and pushed the foreground/background color wells and the screen mode button to the very bottom.
  It now has stretch 0 (its natural height), the wells and the button are inserted right after it, and a stretch after them takes the spare space. The old lines are kept as comments.
- **Brush Options toolbar:** a new toolbar item `brush_color_wells` (same two-squares widget and the same connections as the toolbox wells; 40 x 40 px) is the first item of the `BrushOptions` block in `krita5.xmlgui`.
**Verified:** the syntax checker reported no error located in the new file; the menu file is well-formed. **Not verified:** a build or a device (that the wells sit under the tools at every panel size,
and how the toolbar looks). New code in the main library: a compile or link error on the server is possible.

## 2026-10-05 (3rd) — Side button (sliver) switched on again

**File:** `libs/ui/KisMainWindow.cpp` (Android)

George, after testing build b17 (where the side button is off): "Side panel gadget does nothing. Not resize. Not collapse." and "At one point that side gadget was working."
The thin side button is switched on again (the `#if 0` around its creation is `#ifdef Q_OS_ANDROID` again; the `#if 0` line is kept as a comment). It is the canvas-area version of 2026-10-04
(a child of the canvas area, placed from that area's own size, which is where the right-hand panels begin; hidden on the Welcome page): tap = hide / show the right panels; long press or a sideways
swipe = resize them. In build b4 its resize had been confirmed working; the problems were its position and size, which this version changes. **Not verified on a device:** this version has been
tested only in a standalone Qt test, never on the phone. The dotted Qt bar between the canvas and the panels is a separate, still unsolved problem.

## 2026-10-06 — Krita wordmark removed; no logo on the splash / About screen

**Files:** `libs/ui/kis_splash_screen.cpp`, `krita/data/splash/splash.qrc`, `krita/data/splash/splash-android.qrc`, `krita/data/splash/banner.svg` (deleted)

George (about the About window): "About KRITA?" then "Remove the Krita logo completely from the source and keep that paw icon out of that screen."
The splash screen (also the About tab, which reuses it) drew two logo files on top of the picture: the orange K with the paw (`krita-branding.svgz`) and the white "KRITA" wordmark (`splash/banner.svg`).
- `banner.svg` (the Krita wordmark) is deleted from the source and its line in both `.qrc` files is commented out.
- Neither logo is created any more: the code that built them is kept as comments, and the loading text label takes the place it had before (just below where the logo and banner were).
- `krita-branding.svgz` itself stays in the project (it is still used as the window icon); it is only no longer drawn on this screen.
**Verified:** the syntax checker reports no error in `kis_splash_screen.cpp`; both `.qrc` files are well-formed. **Not verified:** a build or a device (that the start-up splash and the About tab look right without the logo).

## 2026-10-06 (2nd) — Toolbox keeps its columns through a screen rotation, and snaps to whole icon columns

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`

George: "The toolbox reverts to a single column when the display rotates. It should maintain its size. Also, why not snap to multiples of tool icon columns on scale?" ("Fix the toolbox.")
- **Remembered columns:** the number of icon columns (default 2, 1 to 4) is kept in the settings (`krimble/ToolBoxColumns`) and applied once after start.
- **Rotation:** a filter on the main window notices the resize caused by a rotation; once the window has settled (300 ms) the toolbox width is set back to the remembered number of columns (icons x columns + the panel's own frame).
  Resizes that happen during the rotation are not taken as the user's choice.
- **Snap:** when the docked toolbox (left or right side) is resized by hand, 250 ms after the resize stops (and the finger is up) it snaps to the nearest whole number of columns and remembers it.
- Only for a toolbox docked on the left or right side; a floating one is left alone.
**Verified:** the syntax checker reports no error in the toolbox files. **Not verified:** a build or a device: how it behaves during a real rotation on Android is untested (the order of the window and panel resizes is the risk).

## 2026-10-06 (3rd) — Pop-up windows fit the screen and scroll (Android)

**File:** `libs/widgetutils/KisPopupButton.cpp`

George (testing b19): "Presets opens another massive unusable mega-menu. Reduce it. Analyze for other mega menus that need to be shrunk."
The brush Presets pop-up (and every other pop-up opened from a pop-up button) was as large as its contents, more than a phone screen. On Android:
- the pop-up's content is placed in a scroll area with finger scrolling (the same kinetic scroller the app uses elsewhere), and
- the pop-up is capped to 60 % x 85 % of the screen in landscape (92 % x 65 % in portrait); whatever does not fit scrolls.
Menus (`QMenu`) and every other platform keep the old behaviour (the old line is kept as a comment).
**Pop-ups that use this code (so all are covered):** brush presets, brush settings editor, gradient chooser and editor, pattern / widget choosers, panel HUD, color set widget, resource item choosers, storage chooser, file-format export options.
**Analysis only, nothing changed:** the biggest top-level menus (items counted from `krita5.xmlgui`, nested ones included): Layer 85, View 54, Image 42, Edit 35, Type 23; the Filter menu is filled at run time. Qt scrolls a menu that is taller than the screen.
**Verified:** the syntax checker reports no error in `KisPopupButton.cpp`. **Not verified:** a build or a device (how the Presets pop-up looks and scrolls; whether the capped size is comfortable).

## 2026-10-06 (4th) — Settings menu removed: its items moved to Window and Help

**Files:** `krita/krita5.xmlgui` (edited with George's explicit instruction), `libs/ui/KisMainWindow.cpp`

George: "Move everything below Presets in the Settings menu to the Help menu. Everything above that move to Window menu. Then remove Settings menu."
- **Help menu** (in `krita5.xmlgui`, after the website / forum entries, before About), in the old order: Presets..., Resource Libraries..., (supporter bundles), separator, (donations, subscriptions), separator, Author Profile..., separator, Reset Preferences...
  (Presets... itself went with the group below it, so Presets and Resource Libraries stay together.)
- **Window menu** (built in code, `KisMainWindow::updateWindowMenu`; it is not in the menu file) now ends with: separator, Interface Scale..., separator, Themes, Styles, Language..., separator, Customize Toolbar..., Toolbars (new submenu with a
  check box per toolbar: it replaces the automatic toolbar list the Settings menu got from a merge point), Lock Toolbars, separator, Toggle Panels, Detach Panel, Attach Panel, Hide Right Panels. "Panels" was already in Window.
- **Settings menu:** the empty menu element is removed from the menu file. No action entry was deleted: every moved action appears exactly once (checked). The texts the Settings menu showed are set on the actions in the code.
**Verified:** the menu file is well-formed; braces and parentheses of the new code are balanced. **Not verified:** a build (KisMainWindow.cpp cannot be compiled in my test setup, so a compile error is possible) or a device.

## 2026-10-06 (5th) — The former Settings items above Presets go in the View menu, not Window

**Files:** `krita/krita5.xmlgui` (George's instruction), `libs/ui/KisMainWindow.cpp`

George: "If Window is constructed on the fly, use View menu instead." The Window menu is built in code (it is cleared and refilled every time it opens), so the 4th entry above is changed:
- **View menu** (end of the menu, after a separator), in the old order: Interface Scale..., separator, Themes, Styles, Language..., separator, the automatic toolbar list (the merge point comes back here, so the code-built "Toolbars" submenu is not needed), Lock Toolbars,
  separator, Toggle Panels, Detach Panel, Attach Panel, Hide Right Panels. The action entries are the original ones with their texts.
- **Customize Toolbar...** already exists in the Edit menu, so it is not repeated in View (it appears once, in Edit).
- **Panels** (the submenu) stays in the Window menu, where the code already puts it; it is not repeated in View.
- **Window menu code:** the block added in the 4th entry is switched off with `#if 0` (kept, not deleted). The Help part (everything from Presets down) is unchanged. The Settings menu stays removed.
**Verified:** the menu file is well-formed; each moved action appears once in the file. **Not verified:** a build or a device.

## 2026-10-06 (6th) — Menu labels: repetition removed (21 labels)

**File:** `krita/krita5.xmlgui` (text only; George: "Yes, eliminating repetition is a good idea.")

Where a submenu name already says the verb or the object, the item no longer repeats it. Action name: old text -> new text:

- `selectopaque_add`: "Add to Selection" -> "Add"
- `selectopaque_subtract`: "Subtract from Selection" -> "Subtract"
- `selectopaque_intersect`: "Intersect with Selection" -> "Intersect"
- `mirrorNodeX`: "Mirror Layer Horizontally" -> "Flip Horizontal"
- `mirrorNodeY`: "Mirror Layer Vertically" -> "Flip Vertical"
- `mirrorAllNodesX`: "Mirror All Layers Horizontally" -> "Flip Horizontal"
- `mirrorAllNodesY`: "Mirror All Layers Vertically" -> "Flip Vertical"
- `scaleAllLayers`: "Scale All Layers..." -> "Scale..."
- `shearAllLayers`: "Shear All Layers..." -> "Shear..."
- `rotateAllLayers`: "Rotate All Layers..." -> "Angle..."
- `rotateAllLayersCW90`: "Rotate All Layers 90° CW" -> "90° Right"
- `rotateAllLayersCCW90`: "Rotate All Layers 90° CCW" -> "90° Left"
- `rotateAllLayers180`: "Rotate All Layers 180°" -> "180°"
- `convert_to_transparency_mask`: "Convert to Transparency Mask" -> "Transparency Mask"
- `convert_to_filter_mask`: "Convert to Filter Mask" -> "Filter Mask"
- `convert_to_selection_mask`: "Convert to Selection Mask" -> "Selection Mask"
- `convert_to_file_layer`: "Convert to File Layer" -> "File Layer"
- `convert_group_to_animated`: "Convert to Animated Layer" -> "Animated Layer"
- `layercolorspaceconversion`: "Convert Layer to Profile..." -> "Color Profile..."
- `split_alpha_into_mask`: "Split Alpha into Mask" -> "Into Mask"
- `split_alpha_save_merged`: "Split Alpha and Save Merged" -> "Save Merged"

Not changed on purpose: items whose action also appears in the Edit > Transform menu (Rotate Layer, 90 deg, 180 deg), because one action has one label everywhere; the other suggestions from the list (Open as Copy, Export As, the Select > Convert group, Type, Image, Help, the paint-only View items) wait for George.
**Verified:** the menu file is well-formed and each of the 21 changes matched exactly one entry. **Not verified:** a build or a device.

## 2026-10-06 (7th) — More menu labels, and a Select > Convert submenu

**File:** `krita/krita5.xmlgui` (George's choices from the suggestion list)

- file_import_file: (Open existing Document as Untitled Document...) -> Open as Untitled...
- file_export_advanced: Export.../Export Options -> Export Options...
- Select: four Convert items moved into a new Convert submenu (convert_to_vector_selection: To Vector Selection; convert_to_raster_selection: To Raster Selection; convert_shapes_to_vector_selection: Shapes to Vector Selection; convert_selection_to_shape: Selection to Shape)
- edit_selection: Edit in Quick Mask Mode -> QuickMask Edit
- resizeimagetolayer: Crop to Current Layer -> Crop to Layer

**Select > Convert:** the four items that began with "Convert" are now in a submenu named Convert, so their names no longer repeat it.
**Not changed (waiting for George):** Purge Unused Image Data (Unused is a safety word: "Purge Image Data" would sound like it deletes the image), Remove Character Transforms, Flip Canvas Horizontal / Vertical, Edit Layer Metadata..., the Layer > Convert terms, the Help items.
**Verified:** the menu file is well-formed; each changed action appears once. **Not verified:** a build or a device (a new submenu in the Select menu).

## 2026-10-06 (8th) — Select > Select Opaque: Boolean terms

**File:** `krita/krita5.xmlgui` (George: "9. Can be changed to what you just said. A pro knows these terms." and "I'm trying to find ways to make menus more slender.")
- `selectopaque_add`: "Add" -> "Union"
- `selectopaque_subtract`: "Subtract" -> "Difference"
- `selectopaque_intersect`: "Intersect" -> "Intersection"
(The earlier text was "Add to Selection" / "Subtract from Selection" / "Intersect with Selection"; see the 6th entry.) These combine the opaque-pixel selection with the current selection as Boolean set operations.
**Verified:** the menu file is well-formed and each change matched exactly one entry. **Not verified:** a build or a device.

## 2026-10-06 (9th) — Menu labels: the remaining proposals applied (10 labels)

**File:** `krita/krita5.xmlgui` (George: "Apply your other best use suggestions.")

Action name: old text -> new text:

- `convert_to_transparency_mask`: "Transparency Mask" -> "To Transparency Mask"
- `convert_to_filter_mask`: "Filter Mask" -> "To Filter Mask"
- `convert_to_selection_mask`: "Selection Mask" -> "To Selection Mask"
- `convert_to_file_layer`: "File Layer" -> "To File Layer"
- `convert_group_to_animated`: "Animated Layer" -> "To Animated Layer"
- `svg_remove_transforms_from_range`: "Remove Character Transforms" -> "Reset Letter Positions"
- `purge_unused_image_data`: "Purge Unused Image Data" -> "Purge Unused Data"
- `mirrorImageHorizontal`: "Flip Canvas Horizontal" -> "Flip Horizontal"
- `mirrorImageVertical`: "Flip Canvas Vertical" -> "Flip Vertical"
- `EditLayerMetaData`: "Edit Layer Metadata..." -> "Edit Metadata..."

Kept the word "Unused" in the purge item on purpose ("Purge Image Data" would sound like it deletes the image). "Reset Letter Positions" clears the shifts and rotations set by hand on single letters of the selected text.
Not changed on purpose: "Show Global Selection Mask" (the word Global tells it apart from the Selection Mask layer type), the shared rotate-layer items (one action, one label in two menus), the Help items and the paint-only View items (George: leave / undecided).
**Verified:** the menu file is well-formed and each of the 10 changes matched exactly one entry. **Not verified:** a build or a device.

## 2026-10-06 (10th) — Side panel: a full-height resize grip, no collapse button; collapse by closeness to the side

**File:** `libs/ui/KisMainWindow.cpp` (Android)

George: "I think the added collapse button on the side panel is less useful than actually making the scale gadget reliable. it could just collapse on close proximity to the side."
The thin side button (which already resized the right-hand panels reliably in build b4) is now the resize grip, since Qt's own dotted bar between the canvas and the panels has not been grabbable on the phone since 2026-10-02 and its cause was never found.
- **Shape:** a full-height strip, 36 px wide, along the right edge of the canvas area (where the panels begin), with a dark line and a light line beside it (the separator made visible), a few dots in the middle, and a faint band while it is held.
  It covers the 36 px of the canvas next to the panels: a stroke cannot start there.
- **Resize:** press and drag sideways (the drag starts after 8 px, no long press); left = wider.
- **No collapse button:** a tap does not collapse the panels any more. A tap, or a drag, opens them when they are hidden.
- **Collapse by closeness to the side:** when a drag ends within 56 px of the window's right edge, or with the panels narrower than 120 px, the right-hand panels hide; the grip then sits on the window's right edge and a drag from it opens them again.
**Verified:** braces and parentheses balance (the file as a whole already had one unmatched brace character before this change, in a string or comment). **Not verified:** a build (KisMainWindow.cpp cannot be compiled in my test setup) or a device. The dotted Qt bar itself is still unfixed.

## 2026-10-06 (11th) — Build fix: the commented-out banner line in the splash resource lists

**Files:** `krita/data/splash/splash.qrc`, `krita/data/splash/splash-android.qrc`

Build `build-install16` (and `17`) stopped at 80%: "No rule to make target .../splash/banner.svg, needed by krita/qrc_splash-android.cpp". CMake reads a `.qrc` list with a plain text search, so the old `file` entry for `banner.svg`
that I had kept inside an XML comment (7th change, "comment out, don't delete") was still read as a real entry, and the file no longer exists. The comment now describes the removal in words only and no longer contains the `file` tag.
**Verified:** both files are well-formed and contain no `file` entry naming `banner.svg`. **Not verified:** the build.

## 2026-10-06 (12th) — Server helper scripts (short, fixed build commands)

**Files:** `tools/server/kb-env.sh`, `kb-build.sh`, `kb-package.sh`, `kb-status.sh`, `kb-apk.sh`; `BUILD_ANDROID.md`

George: "You send me the same goddamned things over and over. It's confusing. You need to make these stupid things clearer." The long commands (the settings block, the build, the packaging, the checks) were pasted again and again and a double paste started two packaging runs at once.
The commands are now fixed scripts on the server; every step is one short name that never changes (`~/kb-build.sh`, `~/kb-status.sh`, `~/kb-package.sh`, `~/kb-apk.sh`), each prints plain words, and the build and packaging scripts cannot be started twice by mistake. **Verified:** all five scripts pass a syntax check (`bash -n`). **Not verified:** a run on the server.

## 2026-10-06 (13th) — Fixes after testing b26: no number pad, no "No Text" menu, About Krimble, edge strip off

**Files:** `libs/ui/KisApplication.cpp`, `libs/widgetutils/xmlgui/ktoolbarhandler.cpp`, `krita/krita5.xmlgui`, `libs/ui/KisMainWindow.cpp`

George tested b26 and sent a long list. These four are fixed:
- **Big number pad:** "GIGANTIC NUMERIC KEYPAD APPEARS!! DO NOT WANT!!" A short tap on a number field no longer opens Krimble's number pad; the tap is handed back to the field (the press and release are sent to it again). Long press and drag still change the value. The pad's code stays in the file as a comment.
- **"No Text" menu:** the toolbar-list handler of the menu library had the removed Settings menu written into it, so KXmlGui made a new menu called "settings" without a text. It now points at the View menu.
- **Help menu:** the entry reads "About Krimble".
- **Edge strip on the side panel:** switched off again (`#if 0`). It was an unwanted extra gadget, did not resize, and (full height from the top) covered the X of the document tab, which is why the X stopped working.
**Not fixed yet (from the same list):** the toolbox and side panel cannot be resized (the dotted bar); the toolbox shows a corner gadget and a minimum width at start, and its color wells float; panels cannot be stacked (they become tabs); the toolbox went back to one column when the side panel was collapsed from the menu; number drag works on Width but not Height; the rotation gadget at the bottom; the New Image dialog is too big; the whole UI disappeared once with no way back; the Brush icon is too small; the hand-with-a-slash icon; dashes on the selection-tool icons (the lasso looks like a speech bubble); Tool Options should name the active tool.
**Verified:** `KisApplication.cpp` passes the syntax check; the menu file is well-formed. **Not verified:** a build or a device.

## 2026-10-07 — Floating toolbox: no corner gadget, no minimum width, two edge strips

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`, `libs/ui/KisMainWindow.cpp`

George (he floated the toolbox on purpose): "I made the toolbox float. I don't want it to have a corner sizing gadget." Earlier: "It now has a min width."
- **No corner gadget** on the floating toolbox: the shared floating-panel code (`KisMainWindow.cpp`) skips the handle, the reserved bottom strip and the 150 x 120 minimum for the panel named ToolBox. Other floating panels are unchanged.
- **Two edge strips** instead (inside a 24 px margin the floating toolbox gets, so they cover no tool icon): the right strip changes the width in whole icon columns (1 to 4, remembered in the same setting as the docked toolbox), the bottom strip changes the height (at least 160 px).
  Each strip shows a dark line with a light line beside it and three dots.
**Verified:** the syntax checker reports no error in the toolbox files; the brace count of `KisMainWindow.cpp` has the same one-off difference as before. **Not verified:** a build or a device (whether a finger can grab the 24 px strips on a floating window).

## 2026-10-07 (2nd) — Toolbox Brush icon: bolder, fills the icon (BR1)

**Files:** `krita/pics/tools/SVG/16/light_krita_tool_freehand.svg`, `dark_krita_tool_freehand.svg`

George (testing b26): "Brush works now, but icon looks bad. Super tiny." The K4 brush was a thin line inside the 16 px box. Two bolder versions were shown (BR1, BR2); George: "Br1 is better."
BR1 has a thicker handle, a thicker hollow metal sleeve and a bigger tuft of hair (the original curved shape, centred on the stick), and uses the whole diagonal of the box. Replaces the K4 version of 2026-10-05.
**Verified:** both files are well-formed XML; drawn at 72, 32 and 16 px. **Not verified:** a build or a device.

## 2026-10-07 (3rd) — Dashes on the lasso icons

**Files:** `krita/pics/tools/SVG/16/light_` and `dark_` `tool_outline_selection.svg`, `tool_polygonal_selection.svg`, `tool_magnetic_selection.svg`

George (testing b26): "Lasso looks like a word balloon. Needs dashes. All selection tools need dashes." The rectangle, ellipse and "similar color" selection icons were already dashed.
- **Lasso:** the loop is dashed, the little tail stays solid.
- **Polygonal lasso:** the polygon is dashed, the tail stays solid.
- **Magnetic lasso:** the horseshoe magnet is a little smaller and has a dashed line under it (the edge it follows).
**Verified:** all six files are well-formed XML; drawn at 80 px. **Not verified:** a build or a device (legibility at 16 px).
**Not done:** the Burn icon (hand with a ring of finger and thumb): George rejected every version so far; he sent a photo of his own hand and the original tool menu as the reference.

## 2026-10-07 (4th) — Burn tool icon: the darkroom hand

**Files:** `krita/pics/tools/SVG/16/light_krita_tool_burn.svg`, `dark_krita_tool_burn.svg`

George (testing b26): the old Burn icon, a raised-palm outline, was "the hand outline with a slash through it ... no idea". He explained that Burn is the hand shaped as in the darkroom technique (a pinch or claw that lets light through a small opening), sent a photo of his own hand and the original tool menu as the reference, and rejected several drawings of mine (an OK-sign hand, library icons, a curled hand), one of them because it was "cut off" with a hard edge.
The new icon is the outline of his own hand from the photo: side view, thumb and finger meeting with a diamond-shaped opening, the back of the hand arching over, the arm ending in a round shape (no hard cut). Solid, like the other icons. George: "W2 is fine."
(No Adobe artwork was used or traced.)
**Verified:** both files are well-formed XML; drawn at 16, 32 and 110 px. **Not verified:** a build or a device.

## 2026-10-09 — New launcher icon: white K+paw on orange

**Files:** `packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp`, `ic_launcher_next_fg.webp`; `res/values/ic_launcher_background.xml`, `ic_launcher_next_background.xml`; all `res/mipmap-*/ic_launcher*.webp` (mdpi, hdpi, xhdpi, xxhdpi, xxxhdpi; main, next, round, next_round); `ic_launcher-playstore.png`, `ic_launcher_next-playstore.png`

George supplied a new icon: white K and paw on solid orange (#F96301).
- **Foreground:** white K+paw with the orange removed (transparent), so the themed (monochrome) icon stays a K+paw shape and not a solid square.
- **Background color:** #FFFFFF → #F96301 (old line commented out).
- **Legacy mipmaps:** full orange icon; round files circle-masked.
- **Play Store PNGs:** full orange icon, 512 px.
- The main and "next" variants use the same image. Adaptive icon XMLs and the 12dp inset are unchanged.
**Verified:** foreground recomposited on orange matches the source (max pixel difference 2/255); file sizes and modes checked. **Not verified:** a build or a device.

## 2026-10-09 (2nd) — Google Play AAB: API 36, AAB script, size reduction

**Files (repo):** `packaging/android/apk/build.gradle` (targetSdkVersion 35 → 36), `build-tools/ci-scripts/krimble-build-aab.py` (new)
**Server only, not in the repo:** `~/aab-build.py` (now copied into the repo as above); the temporary CMake strip hook (removed again); any other uncommitted edits to the server's `build.gradle` (not yet captured)

From two ChatGPT session transcripts George supplied on 2026-10-09 (work done 2026-10-08). Not re-verified on the server.

**1. ARM64 only**
- `KDECI_ANDROID_ABI=arm64-v8a`, `KDECI_WORKDIR_PATH=$HOME/kwd`, built from `~/kwd/krita`.

**2. Target API 35 → 36**
- Done on the server with `sed -i 's/targetSdkVersion 35/targetSdkVersion 36/'`. Now also in the repo (2026-10-09). `compileSdk` stays 35.

**3. AAB script**
- `~/aab-build.py` = `build-android-package.py` with `assembleRelease` → `bundleRelease` and output match `*.apk` → `*.aab`. Now `build-tools/ci-scripts/krimble-build-aab.py`.
- Run: `cd ~/kwd/krita && python3 <script> --package-type release`
- Result: `krimble-arm64-v8a-1.0.0-beta2-release.aab`, 271,313,916 bytes (258.8 MiB), `BUILD SUCCESSFUL`.

**4. Size reduction: native debug symbols**
- Cause: debug information in the native libraries, not debug code. C++ was already built with `-O3 -DNDEBUG`.
- Largest file, `lib_kritalcmsengine_arm64-v8a.so`: 219,082,456 → 23,180,648 bytes (89.4% smaller).
- Test: `llvm-strip --strip-debug` on a copy of that library, about 209 MB → about 31 MB.
- Tried: a CMake hook in `~/krimble/packaging/android/apk/assets/ECM/toolchain/ECMAndroidDeployQt.cmake` plus a helper `strip-android-libs.cmake`. Removed afterward with `sed -i '/strip-android-libs\.cmake/d'` and `rm`. That path is not in the repo.
- Final fix: Gradle's `stripReleaseDebugSymbols` task. Which change made it start working is not recorded here.
- Clean rebuild: `python3 ~/krimble/build-tools/ci-scripts/build-android-package.py --package-type release`
- APK: about 509 MB → 165,553,065 bytes (157.8 MiB), about 67% smaller.
- The 157.8 MiB figure is the APK. The 258.8 MiB figure is the AAB. They are different outputs. Whether the AAB had stripping applied is not known.
- `build.gradle` line 263 has `ndkVersion "22.1.7171670"`. Its comment says a mismatched NDK version makes AGP fail to strip native libraries. Not changed.

**5. Google Play (internal testing track)**
- The size check and the target-API check passed.
- Rejected: version code `5050400` was already used. The transcript does not say this was resolved. `versionName` is still `1.0.0-beta2` in the repo.

**Debug-bulk hunt, from the server's ~/.bash_history (not in the repo):** the search for what bloated the package began with removing stale NDK references. In `~/krimble/env` and `~/.bashrc`, NDK 30.0.16248370 was replaced with 27.3.13750724. In the server's `build.gradle`, `ndkVersion` 22.1.7171670 was replaced with 27.3.13750724. `~/krimble` was searched for leftover r22 and r30 strings. Before each packaging run, the Gradle daemon was stopped and `~/kwd/krita/_build/krita_build_apk` was deleted so Gradle regenerated it. No command deleting the old r22 or r30 NDK folders was found. After this: the `llvm-strip` test on `lib_kritalcmsengine`, a search of the packaging scripts for where libraries are copied, the temporary CMake strip helper (run, then deleted), and the Gradle `stripReleaseDebugSymbols` result.
**Timeline (unconfirmed):** ChatGPT's recovered records place the AAB work on 2026-10-07 and the stripping work around 2026-10-07/08. An earlier ChatGPT summary said 2026-10-08. The GitHub release 1.0.29b (158 MB, stripped) is dated 2026-10-07. Recovered times: AAB packaging began ~12:32 UTC on 2026-10-07; the androiddeployqt / ECMAndroidDeployQt.cmake investigation ~19:43 MDT; the stripReleaseDebugSymbols result (lib_kritalcmsengine 219 MB → 23.2 MB) ~21:18 MDT. The "2026-10-08" dates elsewhere in this entry are unconfirmed.
**Not verified:** a build or a device. Repo changes here are the API 36 value and the new script only.

## 2026-10-09 (3rd) — build.gradle: versionRelease 1, ndkVersion 27.3.13750724, versionName 1.0.29-beta

**Files:** `packaging/android/apk/build.gradle`

Found by comparing the server's uncommitted `git diff` to the repo. These server-side edits were never committed.
- `versionRelease` 0 → 1. Version code becomes 5050401. Google Play rejected 5050400 as already used. `versionName` is set separately (see below).
- `ndkVersion` "22.1.7171670" → "27.3.13750724". The server's 2026-10-08 builds, which stripped native libraries successfully, used this value. The repo's old comment says a mismatched NDK version makes AGP fail to strip every native library, so this edit is the likely cause of the APK drop from about 509 MB to 157.8 MiB. The transcripts do not confirm this.
- `versionName` "1.0.0-beta2" → "1.0.29-beta", matching the release published 2026-10-07 (`krimble-arm64-v8a-1.0.29-beta-release.apk`, tag `1.0.29b`, commit `f01a17e`, 158 MB). The name had not changed through many earlier builds.
- The older comment above `ndkVersion` (claims r22b is installed) is now outdated and left in place.
- The repo's `build.gradle` now has all 3 server-side edits (API 36, versionRelease, ndkVersion), plus the new `versionName`.
**Not captured:** the server's modified `.kde-ci.yml` (its diff was not provided). **Not verified:** a build or a device.

## 2026-10-09 (4th) — build.gradle: versionName and version code follow the build counter

**Files:** `packaging/android/apk/build.gradle`

`versionName` and `versionRelease` were hardcoded, so the APK's version never changed between builds and a fixed `versionRelease` would repeat Google Play's duplicate-version-code rejection on every upload.
- `build.gradle` now reads `~/krimble-build-number.txt` (the counter `libs/version/KrimbleBuildStamp.cmake` adds 1 to on every make).
- `versionName` = `1.0.<counter>-beta`. `versionRelease` = the counter, so the version code rises with every build (counter 37 → 5050437).
- If the file is missing or not a number, the old fixed values apply (`1.0.29-beta`, `versionRelease` 1).
- Past counter 99 the version code runs into the `versionMinor` digits but still rises.
- Replaces the fixed values from the 3rd entry above. Old lines are commented out.
- `kb-apk.sh` still writes `Krimble-Beta2-` into copied file names. Unchanged.
**Not verified:** a Gradle run, a build, or a device.

## 2026-10-09 (5th) — Launcher icon replaced again: larger K+paw

**Files:** `packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp`, `ic_launcher_next_fg.webp`; all `res/mipmap-*/ic_launcher*.webp` (mdpi, hdpi, xhdpi, xxhdpi, xxxhdpi; main, next, round, next_round); `ic_launcher-playstore.png`, `ic_launcher_next-playstore.png`

Replaces the 2026-10-09 icon. On George's phone the first version's K and paw sat too small in the orange. The new file (white K+paw on #F96301, 1024 × 1024) has a larger mark.
- Same method as the first icon: foreground = white K+paw with the orange removed; legacy mipmaps = full orange icon, round files circle-masked; Play Store PNGs 512 px. Background color XMLs unchanged (#F96301).
- Fit check against the adaptive-icon mask at the current 12 dp inset: farthest point of the K is 32.0 dp from center (safe zone 33 dp); the paw's right toe is 34.8 dp (mask edge 36 dp). Nothing is clipped. The toe is outside the safe zone by about 1.8 dp, intended: the paw is off center.
**Verified:** foreground recomposited on orange matches the source (max pixel difference 2/255); file sizes and modes. **Not verified:** how it looks on a device.

## 2026-10-09 (6th) — New-document dialog: scrolls, fits the screen, narrower entry boxes (Android)

**Files:** `libs/ui/KisOpenPane.cpp`, `libs/ui/dialogs/KisDlgCreateNewDocument.cpp`, `libs/ui/widgets/kis_custom_image_widget.cc`

The File > New dialog's minimum size (about 1415 × 1375 px) was bigger than the screen in both orientations: the left side was off-screen in portrait, and Create/Cancel were off the bottom in landscape.
- Each custom page (Custom Document, Create from Clipboard) is wrapped in a scroll area with touch dragging, so everything, Create and Cancel included, is reachable by scrolling.
- The dialog opens no wider than 95% and no taller than 90% of the screen.
- The width, height and resolution boxes, their unit dropdowns and the Predefined dropdown are about 20% narrower (maximum widths 90/95/130 → 72/76/104). Old lines commented out.
- All Android only (`Q_OS_ANDROID`).
**Not verified:** a compile, a build, or a device. The white bands around the dialog in the screenshots are not addressed.

## 2026-10-09 (7th) — Toolbox: columns choice menu, default 1 column

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`

Screen rotation switched the toolbox from 1 column to 2. Cause: no column count was ever saved (the size handle can't be dragged on touch), so each rotation applied the built-in default, 2.
- Default is now 1 column (was 2), so rotation keeps 1.
- New "Columns" section in the toolbox's right-click menu: 1, 2, 3 or 4 columns. The choice is saved ("krimble/ToolBoxColumns") and kept through rotation. The current count is ticked.
- Old default lines commented out.
**Not verified:** a compile, a build, or a device. The sizing handles (hard to grab on touch) are not fixed by this entry.

## 2026-10-09 (8th) — Preferences: narrow left list with wrapped titles (Android)

**Files:** `libs/ui/dialogs/kis_dlg_preferences.cc`

The left list of the Preferences window was about 31% of the screen wide, sized by its longest one-line title ("Canvas Input Settings").
- On Android the list is drawn by a new `KrimblePrefsListDelegate`: icon on top, title wrapped under it, fixed width of 20% of the screen's short side.
- It is used only if the list is found and has one row per page. Otherwise nothing changes.
- Added includes: `QApplication`, `QStyledItemDelegate`, `QListView`, `QPainter`.
**Not verified:** a compile, a build, or a device.

## 2026-10-09 (9th) — APK file name follows the build counter

**Files:** `tools/server/kb-apk.sh`

The copied APK was named `Krimble-Beta2-<date>-b<counter>.apk`. `Beta2` was hardcoded, so the name disagreed with the app's own version (`1.0.<counter>-beta`).
- The name is now `Krimble-1.0.<counter>-beta-<date>.apk`, for example `Krimble-1.0.42-beta-Oct9-2202.apk`. Date and time are still Mountain time.
- The old line is commented out.
- The server's `~/kb-apk.sh` is a separate copy and must be refreshed from the repo: `cp ~/krimble/tools/server/kb-apk.sh ~/kb-apk.sh`.
**Not verified:** a run on the server.

## 2026-10-09 (10th) — Version number is set by hand

**Files:** `packaging/android/apk/build.gradle`, `tools/server/kb-apk.sh`

George wants no automation on version numbers. The counter had also grown on `make` and packaging runs that were never released (28 on 2026-10-07, 42 on 2026-10-10 UTC).
- `build.gradle`: one hand-set number, `krimbleBuildNumber` (now 30). `versionName` = `1.0.<number>-beta`; `versionRelease` = the number, so the version code is 5050400 + the number. Change that one number for each release build. Google Play needs a higher number for every upload. The counter-file reading is commented out. Replaces the counter-based values from the 4th entry.
- `kb-apk.sh`: the APK copy is named `Krimble-<version from the APK's own file name>-<date>.apk`, for example `Krimble-1.0.30-beta-Oct9-2230.apk`. Replaces the 9th entry's counter-based name.
- The build counter inside the app (splash, logs) is unchanged and is now separate from the version number.
- Server: the `~/kb-*.sh` files are copies and must be refreshed from the repo.

## 2026-10-09 (11th) — BUILD_ANDROID.md: version routine, size-fix verification, server gotchas (docs only)

**Files:** `BUILD_ANDROID.md`

The 2026-10-09 section of `BUILD_ANDROID.md` still described the counter-based version and the old APK names after the version number was changed to hand-set (10th entry).
- Section 2 rewritten: the hand-set number and what it controls, the Play Store rule (each upload needs a higher number), the routine where Claude asks for the number before each build, why the counter was dropped, the counter explained as separate, the new APK file names.
- Section 3: verified that two fresh builds with no strip hook came out at about 157.8 MiB, so no hook is needed. Why Gradle's stripping started working stays marked unconfirmed.
- Section 6: `.kde-ci.yml` is generated output; the build.gradle pull conflict is resolved; new gotchas (`kb-status.sh` can show an old result; packaging deletes `_packaging`).
- New section 7: the release-build routine, step by step.

## 2026-10-10 (12th) — Toolbox: no more 4-column limit; wider grab strip between docks (Android)

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.cpp`

The toolbox could not be stretched past 4 icon columns, so a wide, horizontal toolbox was impossible, and the strips between the docks were too thin to grab by finger.
- The most columns is now 40 (was 4) in all three places that capped it: the saved-columns read, the floating-toolbox edge drag, and the docked snap. The "Columns" menu still offers 1 to 4; wider is by dragging. Old lines commented out.
- On Android the strip between docks (toolbox, canvas, right panels) is at least 20 px (was Qt's default doubled), the same touch size as the dialog edges. Each strip takes 20 px of width.
**Not verified:** a build or a device.

## 2026-10-10 (13th) — Toolbox: color wells grow with the toolbox width

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

The foreground/background color wells at the bottom of the toolbox were fixed at 28 × 28 px, about one icon wide.
- They now grow with the toolbox width: 28 px at 1 column (as before), about 50 px at 2 columns, up to 72 px when wider. Set in `resizeEvent`.
- The wells in the brush toolbar (`KisBrushColorWells.cpp`, 40 × 40) are unchanged.
**Not verified:** a build or a device.

## 2026-10-10 (14th) — Panels and toolbox: dock menu on long-press

**Files:** `libs/widgets/KoDockWidgetTitleBar.cpp`, `libs/widgets/KoDockWidgetTitleBar.h`, `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`

Panels had to be dragged to be docked. Now a long-press opens a menu: Dock Left, Dock Right, Dock Top, Dock Bottom, Undock. The panel's current place is ticked; places it may not go are greyed.
- Panels: long-press on the title bar. Applies to every panel that uses the shared title bar (about 30). A locked panel shows no menu.
- Toolbox: a new "Dock" section in its existing long-press menu, below Columns.
- Not covered: the Animation Timeline and Animation Curves, which have their own title bars. The bottom corners are not changed.
**Not verified:** a build or a device.

## 2026-10-10 — Version 31

`krimbleBuildNumber` 30 → 31 in `build.gradle`: version `1.0.31-beta`, version code 5050431.

## 2026-10-10 (15th) — App shows the hand-set version number, not the build counter

**Files:** `libs/version/KrimbleBuildStamp.cmake`

On launch 1.0.31 showed "1.0.44". The app built its version from the old build counter, which kept rising on every make and packaging run; the hand-set number only reached the APK's Android version.
- `KrimbleBuildStamp.cmake` now reads `krimbleBuildNumber` from `packaging/android/apk/build.gradle`. The app shows `1.0.<number>-beta (<date>, git <hash>)`, the same number as the APK's versionName.
- The counter code is commented out. `~/krimble-build-number.txt` is no longer updated or read.
- A version change now needs `kb-build.sh` (about 2 minutes) before `kb-package.sh`.
**Not verified:** a build or a device.

## 2026-10-10 (16th) — Dock grab strip drawn as three dots only (Android)

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.h`

The widened strip between docks (20, from the 12th entry) made it possible to grab the toolbox and side panels, but Qt draws a line and a band in the whole strip, which looked like a big border around the toolbox.
- On Android the strip is now drawn as three small dots in its middle, with no line or band. The grab width is unchanged.
**Not verified:** a build or a device.

## 2026-10-10 (17th) — Window menu: panel, toolbar and appearance controls moved from View

**Files:** `krita/krita5.xmlgui`, `libs/ui/KisMainWindow.cpp`

George wanted the panel controls in the Window menu, not View, and the toolbar controls and Themes and Styles with them, because the View menu had too much in it.
- Window now ends with: Toggle Panels, Detach Panel, Attach Panel, Hide Right Panels, then Toolbars and Lock Toolbars, then Themes and Styles.
- The Toolbars list is built by `updateWindowMenu()` each time the submenu opens, instead of by the KDE merge point in View.
- The same entries are commented out of the View menu in `krita5.xmlgui`. Interface Scale and Language stay in View.
**Not verified:** a build or a device.

## 2026-10-10 (18th) — Dock separators: black line with a highlight line (Android)

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`

The 16th entry's three dots were invisible. The strip between docks is now a black line (2 px) with a translucent white highlight line (2 px) beside it, centered in the wide strip: to the right of the black line on a vertical strip, below it on a horizontal one. The grab width is unchanged.
**Not verified:** a build or a device.

## 2026-10-10 (19th) — Fix: build error in the Window menu code

**Files:** `libs/ui/KisMainWindow.cpp`

The 17th entry's Toolbars list used `KToolBar`, a class that does not exist in this program (it is `KisToolBar`). The native build stopped with 6 errors on lines 3945 and 3946. One name changed; old line commented out.
**Not verified:** a build.

## 2026-10-10 (20th) — Window menu: View's items moved in, Panels grouped with the panel items

**Files:** `krita/krita5.xmlgui`, `libs/ui/KisMainWindow.cpp`

George wanted fewer items in View. Interface Scale, Themes, Styles, Language, Toolbars and Lock Toolbars move to Window, in their old View order and separators. Panels ▸ moves down so it sits directly on top of Toggle Panels, Detach Panel, Attach Panel and Hide Right Panels with no separator. The pushed block (17th entry) and the two original Panels lines are kept under `#if 0` or commented out.
**Not verified:** a build or a device.

## 2026-10-10 (21st) — Fix: separator lines were on the dock strips; now on the menu dividers. No shortcut text in menus.

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.h`

The 18th entry drew a black line with a highlight line on the dock strips (toolbox, canvas, panels). George meant the divider lines between menu items, which were invisible.
- Dock strips: back to three small dots. Grab width unchanged. The line code is kept under `#if 0`.
- Menu dividers (Android): a black line (2 px) with a highlight line (2 px) below it; divider height at least 8. Titled section dividers (for example "Icon Size") keep their default look.
- Menus show no keyboard shortcut text and reserve no column for it on Android (no keyboard on a touch screen), so menus are as wide as their longest label. Qt works out the column width before it asks the style; if that includes the shortcut width, the narrowing will not show.
**Not verified:** a build or a device.

## 2026-10-10 (22nd) — Toolbox: default 2 columns; a rotation can no longer change the saved column count

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`

The toolbox went from 1 column in portrait to 7 in landscape on rotation: a late resize after the rotation could be saved as the user's column choice.
- The width is only snapped and saved after a resize made with a finger or mouse down (a drag), or from the Columns menu. `snapToColumns` also does nothing during a rotation or while the code is setting the width itself.
- The default is now 2 columns (was 1). Old lines commented out.
**Not verified:** a build or a device.

## 2026-10-10 (23rd) — No graphics on dock strips; 1-pixel menu dividers; Toolbars out of View; Interface Scale... back in View

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/widgetutils/xmlgui/ktoolbarhandler.cpp`, `krita/kritamenu_android.action`, `krita/krita5.xmlgui`, `libs/ui/KisMainWindow.cpp`

QA of build aea9f3c.
- Dock strips draw nothing (the dots were ugly); width unchanged (20), so they are invisible but grabbable. The dots code is kept under `#if 0`.
- Menu dividers: one device pixel black line and one device pixel highlight line (the 2-logical-pixel lines were about 5 device pixels). Divider slot 6 (was 8).
- "Toolbars Shown" kept appearing in View because KDE's toolbar handler plugs it there; the plug call is commented out. The Window menu's Toolbars submenu is unchanged.
- Interface Scale: renamed "Interface Scale..." (was "Change Interface Scale...", the dots because it opens a dialog), moved back to View between Zoom and Screen Mode, removed from Window.
- George's rule: when an item moves between menus it is moved, not commented out. The leftover commented-out copies in `krita5.xmlgui` of the items moved to Window (Themes and Styles, Language, toolbar and panel items) were deleted.
**Not verified:** a build or a device.

## 2026-10-10 (24th) — Toolbox: one continuous block of tools, no groups

**Files:** `libs/ui/toolbox/KoToolBox.cpp`

George did not like the grouping in the toolbox (separator lines and a new row for each group). Every tool now goes into the one "main" section. The old order is kept by adding the old group (main, dynamic, then by its leading digit) to the tool's priority. Old lines commented out.
**Not verified:** a build or a device.

## 2026-10-10 (25th) — Dock strips: the original tiny dots are back

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`

The 23rd entry made the dock strips draw nothing. George said the original tiny dots were visually right and only hard to grab. The 20 px strip stays (the grab area). It now draws three dots, 6 device pixels across and 25 apart (measured from the original look), and no line or band. The earlier larger dots were about 3 times too big. Old versions kept under `#if 0`.
**Not verified:** a build or a device.

## 2026-10-10 (27th) — New splash image

**Files:** `krita/data/splash/logo_splash.jpg`

George's new splash graphic (the old one did not look like the user interface). 1536 × 1024 JPG, 251,466 bytes (was 379,456). The version text keeps its position under the top-right wordmark. The holiday splash is unchanged.
**Not verified:** how it looks on a device.

## 2026-10-10 (26th) — Dragging the side panel strip to the screen edge shuts the right panels

**Files:** `libs/ui/KisMainWindow.cpp`

The strip between the canvas and the right panels stops at the panels' minimum width. Now, if the finger lets go within 56 px of the right edge of the window after pressing on that strip, the right panels hide (same function as Hide Right Panels, which brings them back). No gadget is drawn; the logic of the parked side handle (`#if 0`, 2026-10-07) is reused.
**Not verified:** a build or a device.

## 2026-10-10 (28th) — Dock strips 14 wide (was 20)

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`

The toolbox could not be widened in portrait. Nothing caps its width; Qt can only widen it by taking room from the canvas or the right panels, and the two 20-wide strips between the docks used about 100 device pixels. The strips are now at least 14 wide (about 36 device pixels, still easy to catch), which gives back about 32 device pixels. Old line commented out. The side panel's real minimum width is not confirmed yet.
**Not verified:** a build or a device.

## 2026-10-10 (29th) — Save As dialogs fit the screen (Android)

**Files:** `libs/widgetutils/KoFileDialog.cpp`

QA of build 117207e: the Save As file dialog was bigger than the screen in portrait. It restores the size it had last time, which can be a landscape size. On Android the file dialog and the format picker are cut down to 95% x 90% of the screen and centered each time they are shown.
**Not verified:** a build or a device.

## 2026-10-10 (30th) — Floating toolbox: invisible edges, no stripe, no empty margin

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

QA: the floating toolbox showed a dark/light stripe, dots and about 62 device pixels of empty space on its right and bottom edges (the 24 px edge margin from 2026-10-07). The edges are now invisible 10 px touch zones laid over the toolbox's own edge, with no margin, stripe or dots. Resizing a floating toolbox still works by dragging its right or bottom edge. Old lines kept under `#if 0` or commented out.
**Not verified:** a build or a device.

## 2026-10-10 (31st) — Toolbox: color wells cling to the bottom of the tools

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

QA: when the toolbox was wider, the color wells sat far below the last tool. The tool area kept the height it had at a narrower width. In `resizeEvent`, a moment after the layout settles, the tool area's maximum height is set to the height the tools need at the current width (vertical layout only), so the wells follow directly under the tools.
**Not verified:** a build or a device.

## 2026-10-10 (32nd) — Toolbox: the saved column count is put back after any resize that is not a finger

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

QA: in portrait the toolbox fell back to one column and would not widen. A width change with no finger down (a workspace or layout restore, a rotation) now starts the restore timer, which puts the saved column count back. Only a drag with a finger or the Columns menu can change the saved count (22nd entry). If the screen has no room for the saved count the width stays as the layout allows.
**Not verified:** a build or a device.

## 2026-10-11 (33rd) — Window menu: Tabs/Windows switch and "Show Contents While Dragging" at the top

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/dialogs/kis_dlg_preferences.cc`

George wanted a quick switch between tabbed and floating image windows that is not buried in Preferences, and a toggle for showing the picture while a floating window is dragged.
- First item of the Window menu: "Switch to Windows" (while in tabs) or "Switch to Tabs" (while in windows). It writes the same `mdi_viewmode` setting as Preferences and applies at once. Switching to windows cascades the pictures, so they do not stay maximized. Tile and Cascade already appear in the Window menu in window mode.
- Second item: "Show Contents While Dragging" (checkable). It writes `mdi_rubberband` (checked = 0 = the picture is shown while dragging).
- On Android the default for `mdi_rubberband` is now 0 (the picture is shown while dragging); it was "outline only" because it followed OpenGL. Changed in both places `KisMainWindow.cpp` reads it and in the Preferences checkbox. Old lines commented out.
- Tabs stay the default view mode (window mode was tried as the default on 2026-10-04 and went back, "a little wonky").
**Not verified:** a build or a device.

## 2026-10-11 (34th) — Floating windows are the default view mode again

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/dialogs/kis_dlg_preferences.cc`

George wants floating image windows as the default again "just so people know it's there"; the Window menu's first item (33rd entry) switches to tabs and back in one tap. The default for `mdi_viewmode` is now floating windows (`SubWindowView`) in `configChanged()`, in the Window menu switch and in the Preferences combo box. A stored setting is not touched: anyone who already chose tabs keeps tabs. Window mode was made the default on 2026-10-04 and reverted the same day ("a little wonky"); this is a second try. Old lines commented out.
**Not verified:** a build or a device.

## 2026-10-11 (35th) — Dock strips: the original look with the bigger grab area; wells and screen mode button right under the tools

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/ui/toolbox/KoToolBoxLayout_p.h`, `libs/ui/toolbox/KoToolBoxDocker.cpp`

QA of build 117207e: George wanted the original default look of the toolbox and side panel edges, with only the grabbing improved; and the color wells and screen mode button, which sat in a box of their own at the bottom of the panel and scrolled separately, to stack right under the tools like the tool icons do.
- Dock strips: the strip stays wide (14), but what is drawn is the base style's own handle in a strip of its original width, centered in the wide one. This replaces the tiny-dots drawing, which is kept under `#if 0`.
- Wells and screen mode button: they are now one box (the "trailing widget") inside the toolbox's own layout (`KoToolBoxLayout::setTrailingWidget`), placed right after the last row of tools (after the last column in horizontal layout) and scrolling with the tools. The old lines that put them in the container below the tool area are commented out. The 31st entry's cap on the tool area's height stays; it now includes the wells.
**Not verified:** a build or a device.

## 2026-10-11 (36th) — Toolbox drag handle shows light horizontal lines

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

The empty bar at the top of the toolbox (its title bar, used to drag it) did not look like something to grab. It is now a `KrimbleToolBoxGrip`: the same raised label with three light horizontal lines (2 device pixels thick, 6 apart, 45% of the bar's width) centered on it. The old label line is commented out.
(The blank edge on the right and bottom of a floating toolbox is a separate item: it was removed in the 30th entry.)
**Not verified:** a build or a device.

## 2026-10-11 — Version 32

`krimbleBuildNumber` 31 → 32 in `build.gradle`: version `1.0.32-beta`, version code 5050432.

## 2026-10-11 (37th) — QA of build 32, first batch: Window menu, popups on rotation, real window glyphs, Toolbox name

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.h`, `libs/ui/toolbox/KoToolBoxDocker.cpp`

- Window menu: new "Hide All Panels" next to "Hide Right Panels" (it reads "Show All Panels" while they are hidden). Themes, Styles and Switch Application Language moved to the end of the Window menu, after the panel items. Old lines commented out.
- Rotation left the Window menu stuck on screen. On Android any open menu is closed when the main window changes size (`KrimbleClosePopupsOnResizeFilter`).
- The close button of a floating image window was not an X. On Android the title bar buttons (close, minimize, maximize, restore) are drawn by the app's style (`standardIcon`): an X, a line, a square, two squares, in the text color. This also covers the buttons that appear at the right end of the menu bar for a maximized image.
- The toolbox panel is called "Toolbox" again (it was "Tools" since 2026-10-02). Old line commented out.
**Not verified:** a build or a device.

## 2026-10-11 (38th) — Toolbox: the Columns choice always works

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

QA of build 32: the Columns choice (7th entry) did nothing. `applyColumns()` returned at its first line unless the toolbox was docked at the left or right; George's toolbox was floating.
- Tools laid out in a column (the usual way): the number is the number of columns and sets the width, for a floating toolbox and for one docked at the left or right.
- Tools laid out in a row (a wide toolbox, or docked at the top or bottom): the number is the number of rows and sets the height.
- The first version of the function is kept in comments.
**Not verified:** a build or a device.

## 2026-10-11 (39th) — Two-finger drag: dialogs no longer bounce back; floating panels get it too

**Files:** `libs/ui/KisApplication.cpp`

QA of build 32: after a two-finger drag the dialog bounced back on screen, which made its bottom buttons hard to reach (a dialog taller than the screen).
- Dialogs: after a two-finger drag they stay where they were let go, as long as at least 160 px of them are on screen (`keepReachable`). They are still sized and placed inside the screen when they open.
- Floating panels, the toolbox included, now take the two-finger move too, and bounce back inside the screen when let go (`keepOnScreen`). Their windows are watched by the same code as the dialogs.
- Old lines kept in comments.
**Not verified:** a build or a device.

## 2026-10-11 (40th) — Floating toolbox: bounce at the screen edges; never taller than its contents

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`

QA of build 32: the toolbox was easy to strand off screen and was larger than its contents.
- A floating toolbox that is let go partly off the screen bounces back inside it (350 ms after the last move, once the finger is up; a strip of 48 px at the top is kept free).
- The toolbox's maximum height is the height of its tools, the wells and the title bar at its current width (set with the tool area's own maximum in `resizeEvent`).
**Not verified:** a build or a device.

## 2026-10-11 (41st) — Toolbars menu: Reset Toolbars

**Files:** `libs/ui/KisMainWindow.cpp`

QA of build 32: a toolbar (Back to Basics) was stuck vertical and could not be moved or resized. Window > Toolbars now ends with "Reset Toolbars": every toolbar is un-floated, put back in the top area, made horizontal and shown, as `krita5.xmlgui` defines them (all five are `position="top"`).
**Not verified:** a build or a device.

## 2026-10-11 (42nd) — Menu bar entry let go after a rotation

**Files:** `libs/ui/KisMainWindow.cpp`

QA: after a rotation the Window menu was "stuck" (George is not sure whether the open menu or the pressed button). The 37th entry closes open menus when the main window changes size; this adds letting go of the menu bar entry too (`QMenuBar::setActiveAction(nullptr)`), so a pressed "Window" button cannot stay highlighted.
**Not verified:** a build or a device.

## 2026-10-11 (43rd) — Toolbox width in portrait; "Switch to Floating Windows"

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`, `libs/ui/KisMainWindow.cpp`

QA of 1.0.32 (portrait): the docked toolbox was far wider than its columns (one column of tools beside a wide empty bar), the column count seemed to do nothing, and with the toolbox that wide there was no room left to move the strips beside it.
- The toolbox's preferred width (`sizeHint`) is the width its saved column count needs, so the dock layout starts from the right size.
- `applyColumns()` (docked left or right) also pins the width for 250 ms after `resizeDocks`, then lets go, so the dock layout has to use the width and a drag can still change it afterwards.
- The cause is not confirmed on a device. If a content minimum keeps the toolbox wider, the pin has no effect.
- The first Window menu item reads "Switch to Floating Windows" (it read "Switch to Windows"); old line commented out.
**Not verified:** a build or a device.

## 2026-10-11 (44th) — Dodge and Burn: a slow, cumulative lighten and darken

**Files:** `plugins/tools/basictools/kis_tool_dodge.cc`, `kis_tool_dodge.h`, `kis_tool_burn.cc`, `kis_tool_burn.h`

QA of 1.0.32: on a white canvas Dodge made only a faint line and Burn an instant black one, and neither built up slowly. Both used the color dodge / color burn composite ops with the foreground color, which do not lighten or darken what is there the way the industry-standard tools do.
- Dodge now uses the linear dodge op (`COMPOSITE_LINEAR_DODGE`) and Burn the linear burn op (`COMPOSITE_LINEAR_BURN`). Painted with white (Dodge) or black (Burn), each brush dab moves the pixel a small step toward white or black, so the effect builds up the more you paint over a spot. Build-up mode stays on.
- Opacity 100% and flow 4% per dab to start with; both can be changed in the tool options.
- While Dodge or Burn is the active tool, the foreground color is white or black. `deactivate()` gives the user's foreground color back. The wells will change when you pick the tool, and change back when you leave it.
- The old lines are kept in comments.
- Not done: Photoshop's range choice (shadows, midtones, highlights).
**Not verified:** a build or a device. The 4% flow is a starting guess.

## 2026-10-11 (45th) — Type tool: keyboard comes back on re-edit; Type Setting mode starts off

**Files:** `plugins/tools/svgtexttool/SvgTextTool.cpp`

QA of 1.0.32: going back to edit a text did not bring the on-screen keyboard back, and a circle with dashes and handles showed beside the text.
- The circle, red dashes and dashed line are the handles of Type Setting mode (baseline and kerning), drawn while that mode is on. The mode now starts switched off every time the text tool starts (`activate()`); it can still be switched on in the tool options. The cause of it being on is not known.
- Nothing in the text tool ever asked Android to show the keyboard; it came up only when text was created. On Android, after a tap that leaves a text selected, the keyboard is shown (`QGuiApplication::inputMethod()->show()` after the release).
**Not verified:** a build or a device.

## 2026-10-11 (46th) — A corner sizing grip on every dialog without a touch resize of its own

**Files:** `libs/ui/KisApplication.cpp`

George wanted the bottom-corner sizing gadget on more dialogs (Preferences was the example). The code that already watches every dialog (`KrimbleDialogKeeper`) now adds a `KrimbleResizeGrip` to each top-level dialog when it is shown: a 44 px square at the bottom right with three diagonal lines; dragging it resizes the dialog (not smaller than its minimum). It is repositioned when the dialog changes size.
- Not added to the 43 dialogs built on `KoDialog` (they have their own touch edges and corner), to message boxes, or to progress dialogs.
- Covers the dialogs built straight on `QDialog` (New Document, Filter, Switch Language, the recorder dialogs, and the others listed in the 2026-10-11 survey), Preferences and the metadata editor, and Qt's file dialogs.
**Not verified:** a build or a device.

## 2026-10-11 (47th) — About window: the splash image is scaled to fit the screen

**Files:** `libs/ui/kis_splash_screen.cpp`

QA: the About Krimble window was far bigger than the screen. Its first tab holds the splash image at a fixed 320 px high, about 480 px wide, wider than a phone in portrait, and the window cannot be smaller than that. On Android the About splash is now scaled so it is at most 80% of the screen width (aspect ratio kept), and the banner and margin values that follow from its height are worked out again. The loading splash is unchanged. The old lines are kept in comments.
Not done: `kis_about_application.cpp` still sets its minimum size from its size hint; if the window is still too big, the next step is a scrolling area.
**Not verified:** a build or a device.

## 2026-10-12 (48th) — New Document window: no scroll area, thinner left list with the text under the icons

**Files:** `libs/ui/KisOpenPane.cpp`, `libs/ui/widgets/kis_custom_image_widget.cc`

George: the New Document window is too big, the scroll area (2026-10-09, 6th entry) made the Create button harder to find, and the left list could be thinner with the text under the icons.
- The scroll area around the Custom Document and Create from Clipboard pages is switched off again (kept under `#if 0`).
- The left list uses `KrimbleIconAboveTextDelegate` on Android: icon on top, text centered under it and wrapped. Its maximum width is 20% of the screen width (was 22%).
- The long sentence "This document will be ... pixels by ... pixels in ..." (`lblDocumentInfo`) did not wrap and made the Custom Document page wider than a phone. It wraps now.
- Not done: other things on that page may still be wider than the screen. A screenshot of the next build will show.
**Not verified:** a build or a device.

## 2026-10-12 (49th) — Top toolbars stack; New Document margins

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/KisOpenPane.cpp`

- George: the toolbars should stack instead of lining up in one row. On Android, 2.5 s after the main window starts (once the saved layout is back), and after "Reset Toolbars", a toolbar break is put in front of every toolbar in the top area except the first (`krimbleStackToolBars`), so each toolbar gets its own row. The cost is vertical space.
- George: the New Document window wastes a lot of space. On Android its outer margins and spacing are 4 px (they were the dialog's defaults).
**Not verified:** a build or a device.

## 2026-10-12 (50th) — Help menu: "Where is...?", "Send Feedback" and the "QA Overlay"

**Files:** `libs/ui/KisMainWindow.cpp`

Three entries are added to the end of the Help menu (each time it opens, once), from the same timer that stacks the toolbars (Android).
- **Where is...?** (Claude's idea; George: "Yes."). A small window with a search box and a list of every menu entry with its path (for example "Image > Crop"). Typing filters the list; tapping an entry closes the window and opens that entry. Menus that fill themselves when opened (Window, recent files) are asked to fill first.
- **Send Feedback** (Claude's idea; George: "Yes."). Opens a prefilled new issue on GitHub in the browser, with the Krimble version, the Android version, the screen size and density and the window size.
- **QA Overlay** (Claude's idea; George: "Yes. Like the web developer extensions for browsers."). A checkable item. While on, a green-on-black note at the top left updates twice a second with: portrait or landscape, the window size and density, the central area, every visible panel (size, minimum width, maximum width, area or floating), every visible toolbar and, if a dialog is in front, its size, its minimum size and the 8 widgets with the widest minimum size. Sizes are logical pixels; multiply by the density for device pixels. It exists so that one screenshot gives exact numbers.
**Not verified:** a build or a device. Not done yet: the cheat sheet.

## 2026-10-12 (51st) — Size presets for social media, phone and print (Claude's idea)

**Files:** `krita/data/predefined_image_sizes/` (13 new `.predefinedimage` files and `CMakeLists.txt`)

Claude's idea ("Size presets: social posts, stories, A4 at 300 dpi"); George: "Brilliant idea! Yes!". They appear in the "Predefined" list of the Custom Document page of the New Document window. Names, sizes (pixels, 72 ppi unless noted):
- Social: Instagram Post 1080x1080, Instagram Portrait 1080x1350, Story / Reel 1080x1920, YouTube Thumbnail 1280x720, X Post 1600x900, X Header 1500x500, Facebook Cover 820x312, LinkedIn Banner 1584x396, Profile Picture 400x400.
- Phone Wallpaper 1080x2400, Album Cover 3000x3000.
- Business Card 3.5x2 in at 300 ppi (1050x600), Postcard 4x6 in at 300 ppi (1800x1200).
A4 and the other paper sizes at 300 ppi already existed. The sizes are the commonly used ones; the platforms change theirs, so the list may need updating.
**Not verified:** a build or a device.

## 2026-10-12 (52nd) — Help menu: Cheat Sheet (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`

Claude's idea ("a cheat sheet in Help: industry-standard name next to Krimble name"); George: "Cheat sheet- YES."
- A new Help entry, "Cheat Sheet", opens a list: the name people know from other graphics editors, an arrow, and where it is in Krimble (for example "Crop -> Image > Crop"). About 37 names: crop and image sizes, layers and merging, transforms, selections, the adjustments (brightness/contrast, levels, curves, hue/saturation, color balance, vibrance), blur, sharpen and noise filters, color mode, fill and stroke, undo, copy merged, paste as a new image, preferences.
- The paths are looked up in Krimble's real menus when the sheet opens, so they cannot go out of date; a name whose entry is not found (renamed or missing) is simply left out. Tapping a line closes the sheet and opens that entry.
- The list of names and search words is in `krimbleShowCheatSheet`; names can be added there.
**Not verified:** a build or a device.

## 2026-10-12 (53rd) — File menu: Share as JPG / Share as PNG (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`, `packaging/android/apk/src/org/krimble/android/ShareHelper.java` (new)

Claude's idea ("Open and share through Android"); George: "Share out is great." (Opening images from other apps already works: Krimble is in the "Open with" list.)
- Two new entries just above Quit in the File menu (added when the menu first opens): "Share as JPG..." and "Share as PNG...". They take the open image as it looks flattened (all layers, as shown), save it, and open Android's share sheet: messages, email, social apps, cloud storage, nearby devices. JPG is flattened onto white and saved at quality 92; PNG keeps transparency.
- `ShareHelper.shareFile` (Java) puts the picture into the phone's **Pictures/Krimble** folder through the MediaStore, so it also shows in the Gallery, and shares that. This needs no extra Android permission or library. It needs Android 10 or newer; on an older phone a message says so.
- Not done: sharing the layered Krimble/Krita file itself, and choosing a size before sharing.
**Not verified:** a build or a device. This is the first Java code added by Claude's ideas; if the packaging step fails, the error will name `ShareHelper.java`.

## 2026-10-12 (54th) — Window menu: Mirror Layout (Left-Handed) (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`

Claude's idea ("left-hand and right-hand layouts: mirror the toolbox and panels"); George: "Mirror layout - smart idea. Yes".
- A new checkable item in the Window menu, after "Hide All Panels": "Mirror Layout (Left-Handed)". One tap swaps the sides: every panel docked on the left (the toolbox) moves to the right, every panel docked on the right moves to the left. Their order top to bottom is kept, and panels that share tabs (Color and Tool Options) stay together. The item shows a check while the toolbox is on the right; tapping it again swaps back.
- Nothing extra is saved: Krimble already saves and restores the window layout when it closes and starts.
- Floating panels are not moved. The "Hide Right Panels" item and the drag-to-the-edge shortcut still mean "the panels on the right side", so in the mirrored layout they act on the toolbox side.
**Not verified:** a build or a device.

## 2026-10-12 (55th) — Panel edges: no more wide strip; an invisible touch zone inside each panel

**Files:** `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/ui/KisMainWindow.cpp`

George (looking at a screenshot of the edge between the toolbox and the canvas): "I want to make sure the gristle along this edge is gone. Should be a usable invisible grab zone there, not more UI fat."
- The strip between the panels and the canvas (14 px, drawn with the base style's handle) is the "fat". On Android it is the base style's own thin strip again, and it draws nothing. The old drawing code is kept after a `return`, not deleted. This undoes the 28th, 35th and 25th entries' wide strip and the dots.
- The touch zone is now `KrimbleDockEdgeGrab`: an invisible 14 px widget laid over the inner edge of every docked panel (right edge of a panel on the left, left edge of a panel on the right). Dragging it resizes that side with `resizeDocks`. It draws nothing and takes no layout room. A 2 s timer makes sure panels added later get one; mirroring the layout moves them to the right side.
- The drag-to-the-edge shortcut (26th entry) looks for a press up to 20 px inside the panel's edge now.
- The three dots are gone (George asked for an invisible zone). If they should come back as a hint, they can be drawn on the grab zone.
**Not verified:** a build or a device.

## 2026-10-12 (56th) — Panel edge touch zone moved to the outer side

**Files:** `libs/ui/KisMainWindow.cpp`

George: "Not the inner edge. That overlaps with the tools." The 55th entry put the invisible 14 px zone inside each panel. It is now a child of the main window and lies on the outer side of the panel's edge: for the toolbox, the first 14 px of the canvas to its right; for the panels on the right, the 14 px of the canvas to their left. It follows the panel when the panel moves or changes size. The inner-edge version is kept in a comment.
- Cost: on the right side the zone covers part of the canvas's vertical scrollbar (the scrollbar is about 30 px wide; about 14 px of it is taken).
**Not verified:** a build or a device.

## 2026-10-12 (57th) — Brush icon fills its square; long labels in dialogs wrap

**Files:** `krita/pics/tools/SVG/16/dark_krita_tool_freehand.svg`, `krita/pics/tools/SVG/16/light_krita_tool_freehand.svg`, `libs/ui/KisApplication.cpp`

- George: "Brush icon is too small. Enlarge to the corners." The drawing covered only x 3.0 to 15.4 and y 0.6 to 11.9 of its 16 x 16 box. Its `viewBox` is now `2.7 -0.25 13 13` (the drawing's area plus a small margin) in both the dark and the light file, so it fills the square like the other tool icons. The paths are unchanged; the old `viewBox` is noted in a comment in each file.
- George: "Jpg save dialog is 3x width of the screen in portrait mode." The cause I found in the layout file: two long sentences in the JPEG options (134 and 181 characters, rich text) that do not wrap, so the dialog was as wide as the longest sentence. When any dialog opens on Android, labels longer than 40 characters are set to wrap, and the dialog is re-measured (`KrimbleDialogKeeper`). This also helps other dialogs with long labels.
- Not confirmed: whether the long labels are the only wide thing in that dialog. The QA overlay will list the widest widgets.
**Not verified:** a build or a device.

## 2026-10-12 (58th) — Panel edges: a small handle in the middle instead of a zone along the whole edge

**Files:** `libs/ui/KisMainWindow.cpp`

George: "The touch zone doesn't have to be the whole edge of a panel or toolbox. It could be a small slightly protruding handle in the middle."
- `KrimbleDockEdgeGrab` is now one small handle, 18 x 80 px, in the middle of each side's edge: a rounded tab with three grip lines, on the canvas side of the edge (it protrudes slightly onto the canvas). It is centered on the whole column of docked panels on that side, and only the topmost panel of a side has it; the others hide theirs. Dragging it resizes that side as before.
- This replaces the invisible full-height zone of the 55th and 56th entries (kept in comments). The thin strip drawn by the style stays invisible.
- Cost: the handle covers an 18 x 80 px piece of the canvas next to each side (on the right, part of the vertical scrollbar).
**Not verified:** a build or a device.

## 2026-10-12 (59th) — New Document: the section icons are in color

**Files:** 20 pictures in `krita/data/templates/animation/`, `comics/`, `design/`, `dslr/` and `texture/` (every `light_template_*.png` and `dark_template_*.png`)

George asked whether dialog icons have to be line art (no: they can be full color) and agreed to color for the New Document section icons.
- One color per section, in the same shape and line style as before: Animation violet, Comics amber, Design blue, DSLR coral, Texture mint. The `light_` pictures (used on dark themes like Krimble dark) are bright versions; the `dark_` pictures (used on light themes) are the same colors, deeper. Only the color changed: the drawing, transparency and size are the same.
- Not changed: the "Custom Document" and "Create from Clipboard" icons. They are the program's general "new document" and "new tab" icons, used in other places too; coloring them would color those places as well. They would need their own icon names.
- Where else: these pictures are also the icons of the templates inside each section.
**Not verified:** a build or a device. Checked on a preview image (dark background) before pushing.

## 2026-10-12 (60th) — Window menu: Layout Presets (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`

Claude's idea ("Workspace presets: Retouch, Illustration, Layout and Quick Edit, one tap each"); George: "Better workspaces! Yes. The Krita ones suck."
- A new submenu in the Window menu, after "Mirror Layout (Left-Handed)": "Layout Presets". One tap shows the panels that job needs and hides the other docked panels; the toolbox is always kept and floating panels are left alone.
  - **Quick Edit:** Layers, Tool Options.
  - **Retouch:** Layers, Tool Options, Color, Overview.
  - **Illustration:** Layers, Color, Brushes, Tool Options.
  - **Design:** Layers, Tool Options, Undo History.
  - **Focus (toolbox only):** nothing but the toolbox.
- Panels are matched by their English title; a panel that is not in the list for a preset is hidden. Hidden panels come back through Window > Panels or another preset.
- This does not replace Krita's own saved workspaces (the Workspace submenu); it sits beside them. They can be replaced later if these prove better.
- The panel lists are a first guess at what each job needs; they are in `krimbleApplyPanelPreset`'s caller and are easy to change.
**Not verified:** a build or a device.

## 2026-10-12 (61st) — Ten extra toolbars (Claude's ideas)

**Files:** `krita/krita5.xmlgui`

Claude's ideas ("custom toolbars"), George: "Do all of it." Ten new toolbars. All start **hidden**, so nothing changes until they are turned on in Window > Toolbars:
- **History:** Undo, Redo, Cut, Copy, Paste, Clear.
- **Selection:** All, Deselect, Reselect, Inverse, Color Range, Grow, Contract, Feather, Smooth.
- **Layers:** Add Paint Layer, Add Group Layer, Duplicate, Merge Down, Merge Visible, Flatten Image, Add Transparency Mask, Add Filter Mask, Layer Style.
- **View:** Fit on Screen, 100%, Zoom In, Zoom Out, Rotate View left and right, Reset Rotation, Mirror View, Rulers, Grid, Pixel Grid.
- **Retouch:** Dodge, Burn, Smudge, Soften, Color Sampler, Eraser.
- **Photo Edit:** Crop tool, Crop, Rotate Image 90° left and right and 180°, Flip Horizontal and Vertical, Image Size, Canvas Size.
- **Transform:** Free Transform, Warp, Perspective, Cage, Liquify, Mirror Layers Horizontally and Vertically.
- **Quick Adjust:** Brightness/Contrast, Levels, Color Adjustment curves, HSV Adjustment, Vibrance, Color Balance, Invert.
- **Save and Export:** Save, Save As, Export As.
- **Filters:** Apply Filter Again, Apply Filter Again (Reprompt), Filter Gallery.
- Every button is an action that already exists (names checked against the program's action files and menus; the four Retouch tool names were found in the tool code). Not included: a Text bar (the "Type Options" bar already exists), a Share bar (the Share entries are added to the File menu in code, not as actions), and "My Toolbar" (Custom toolbar 1 and 2 already are that).
- The custom toolbar improvements (bigger grab handle, tools in columns) are not done; they are still last.
**Not verified:** a build or a device. If a button is missing or blank the action name is wrong or the tool is not loaded.

## 2026-10-12 — Version 33

`krimbleBuildNumber` 32 → 33 in `build.gradle`: version `1.0.33-beta`, version code 5050433.

## 2026-10-12 (62nd) — Two compile errors in `KisMainWindow.cpp` (build 33)

**Files:** `libs/ui/KisMainWindow.cpp`

The first compile of the week's work stopped on two lines of my own code:
- Share (`krimbleShareImage`): `cond ? weak pointer : shared pointer` is ambiguous. Now an `if` and an assignment. Old line kept in a comment.
- Reset Toolbars: `KisToolBar` has no `setFloating`. The line is commented out; removing and re-adding the toolbar docks it again anyway.
**Not verified:** a rebuild.

## 2026-10-12 (63rd) — Dialog corner grip no longer covers the buttons

**Files:** `libs/ui/KisApplication.cpp`

QA of build 33 (George: "Sizing gadget covers the button. No bueno."): the 44 px corner grip sat on top of the Create button of the New Document window. The grip is 26 px now, and the first time a dialog gets a grip, a strip as tall as the grip is added under its contents (bottom margin of its layout, once), so the grip sits below the buttons. Cost: each such dialog is 26 px taller. Old values kept in comments.
(Also from the same test: the colored section icons are liked.)
**Not verified:** a build or a device.

## 2026-10-12 (64th) — QA of build 33: menus without the shortcut column, smaller handle, floating toolbox on first run

**Files:** `libs/ui/KisMainWindow.cpp`

- George: "Menus are still way too wide." The 6th/21st-entry fix in the style could not work: Qt adds a column for the longest keyboard shortcut to every menu after measuring the items. The actions are now told not to show their shortcut in menus (`setShortcutVisibleInContextMenu(false)`) for every action in every menu, at start and every 2 s (menus filled later). Menus should be about as wide as their longest name.
- George: "Handle on toolbox is too big." The panel edge handle (58th entry) is 12 x 48 px (was 18 x 80). Old values in comments.
- George: "Handle ... still does not work in portrait/tall aspect. Maybe start with toolbox floating by default." The first time the app runs (once, remembered in the setting `krimble_toolbox_floated_once`), 2.5 s after start, the toolbox is made floating at the top left, 30% of the screen wide and 70% high. After that the saved layout decides, so the user can dock it again. The portrait failure of the handle is not fixed: in portrait the docked toolbox, the canvas and the right panels have no room to give each other.
**Not verified:** a build or a device.

## 2026-10-12 (65th) — Preferences page icons in color (temporary)

**Files:** 16 icon files in `krita/pics/svg/` (`light_` and `dark_` versions of `config-general`, `config-keyboard`, `config-canvas-input`, `config-display`, `config-performance`, `config-tablet`, `config-canvas-only`, `user-identity`)

George: "Add colors for now." (Earlier he said he will draw his own icons; these are a stand-in, like the 59th entry. His icons replace them by file name when they are ready.)
- One color per page, same drawing as before: General blue, Keyboard Shortcuts violet, Canvas Input amber, Display mint, Performance orange, Tablet pink, Canvas-only lime, Author coral. The `light_` files (dark themes) are the bright versions, the `dark_` files (light themes) the deeper ones. Each file had a single gray; only that color changed.
- Not changed: Color Management and Pop-up Palette (already in color), and the other pages' icons.
**Not verified:** a build or a device. Checked on a preview image first.

## 2026-10-12 (66th) — From the test of 1.0.33: Cheat Sheet and Mirror Layout removed; fewer toolbars; Close All Toolbars

**Files:** `libs/ui/KisMainWindow.cpp`, `krita/krita5.xmlgui`

George's test of 1.0.33: Share, QA Overlay, Where is..., Layout Presets and the new toolbars work.
- **Cheat Sheet removed** (it crashed the app when a line was chosen). The Help entry is commented out; `krimbleShowCheatSheet` is kept for later.
- **Mirror Layout removed** (it corrupted the display). The Window menu item is commented out; `krimbleMirrorLayout` is kept.
- **Fewer toolbars:** Transform, Filters and Save and Export are removed (commented out in `krita5.xmlgui`). Seven remain: History, Selection, Layers, View, Retouch, Photo Edit, Quick Adjust.
- **Close All Toolbars:** new item in Window > Toolbars, above Reset Toolbars. It hides every toolbar.
- **Found with the QA overlay (a screenshot, landscape, density 3.375):** the Layers panel's minimum width is 258 logical px, which is about 870 device px. Color is 70 and the toolbox 28. That is why portrait has no room: the Layers panel alone needs most of the screen's width. Not fixed yet.
**Not verified:** a build or a device.

## 2026-10-12 (67th) — New Document window: height follows the page

**Files:** `libs/ui/dialogs/KisDlgCreateNewDocument.cpp`

George (with a screenshot, an arrow pointing at the empty space): "New dialog is too tall." The window's height was the size hint of the whole dialog, which the tall left list (icon over text) pushed up, leaving a big empty gap in the Custom Document page. The height is now the Custom Document page's own size hint plus 70 px for the frame, never more than before (the dialog's size hint, at most 90% of the screen). The old lines are kept in comments.
(Same screenshot set: all the new toolbars turned on at once stack as intended, but fill the screen; that is why four were removed.)
**Not verified:** a build or a device. The 70 px is an estimate of the frame.

## 2026-10-12 (68th) — Layers panel can be narrower (Android)

**Files:** `plugins/dockers/layerdocker/LayerBox.cpp`

The QA overlay showed the Layers panel needs at least 258 logical px (about 870 device px at density 3.375), most of a portrait screen, so the toolbox, canvas and other panels had no room (the cause of the portrait toolbox and handle problems). The six buttons under the layer list (add, duplicate, lower, raise, properties, delete) were the widest row. On Android their icons are 18 px (were 22), their size policy is "ignored" horizontally so they share the row evenly, and each may be as narrow as 30 px. Expected minimum width of the panel: about 190 px (about 640 device px at 3.375), if nothing else in the panel is wider. Not confirmed: the QA overlay on the next build will show the new minimum, and the widest other part if it is still wide.
**Not verified:** a build or a device.

## 2026-10-12 (69th) — New Document: the Predefined size list is wider

**Files:** `libs/ui/widgets/kis_custom_image_widget.cc`

George's test of 1.0.33: Dodge/Burn look ok, the Type tool, the new size presets, Columns, rotation and Save As all work. "The little button could be a little wider." The "Predefined" drop-down's maximum width is 170 px (was 104), so the new size names fit better. Old line kept in a comment.
**Not verified:** a build or a device.

## 2026-10-12 (70th) — Window menu: Quick Adjust (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`

Claude's idea ("A Quick Adjust panel: sliders ... working on a layer without destroying it"); George: "Build quick adjust. It's such a useful feature. Be careful of the width."
- New Window menu item "Quick Adjust" (next to Layout Presets) shows or hides a small panel. It starts **floating**, 62% of the screen wide, so it never has to fit beside the toolbox, canvas and panels; the two-finger move and the bounce-back work on it. One slider per row (nothing side by side), so it can be made narrow.
- Six sliders: **Brightness** and **Contrast** (-100 to 100), **Saturation**, **Hue** (-180 to 180) and **Lightness** (-100 to 100), **Vibrance** (-100 to 100). A Reset button sets them all to 0.
- Non-destructive: the first time a slider of a group leaves 0, an adjustment layer is added above the active layer: "Quick Adjust: Brightness/Contrast", "Quick Adjust: Color" (hue, saturation, lightness) or "Quick Adjust: Vibrance". Moving the slider edits that layer's filter (90 ms after the last move). The layers can be hidden, deleted or opened in the Layers panel like any other. Switching to another picture starts fresh layers there.
- Uses the program's own filters (`brightnesscontrast`, `hsvadjustment`, `vibrance`) and the same layer-adding code as the Layers menu (with undo for the added layer; slider edits themselves are not on the undo list).
- Not done: Exposure, Highlights, Shadows and Sharpness (no matching filter was used), and a "bake into the layer" button.
**Not verified:** a build or a device. The filter ids and property names (`brightness`, `contrast`, `h`, `s`, `v`, `vibrance`) were taken from the filter code; if a slider does nothing, one of them is wrong.

## 2026-10-12 (71st) — README: a "Thank you" section with icons

**Files:** `README.md`, `thanks-icons/` (5 icons and a `README.md`)

George: "Add readme thank yous and icons, please. They deserve our thanks. Actually add GitHub to that list too."
- New README section "Thank you", just above "Support Krimble": Claude (Anthropic), ChatGPT (OpenAI), Oracle Cloud, Termux and GitHub, one line each, with a 24 px icon. The wording thanks them; it says they are not connected to Krimble and have not endorsed it, and that the logos belong to their owners.
- Icons in `thanks-icons/`: `claude.svg`, `openai.svg`, `oracle.svg`, `github.svg` are glyphs from Simple Icons (CC0) in each brand's colour on a white rounded square (OpenAI and Oracle from Simple Icons release 13.0.0, since later releases dropped them); `termux.svg` is the Termux launcher icon from its repository (GPLv3), unchanged. Sources and licenses are in `thanks-icons/README.md`.
- Not checked: each company's own rules for using its logo. If one objects, delete its icon file and its `<img>` tag; the text can stay.
**Not verified:** how the README looks on GitHub. Checked the icons on a preview image.

## 2026-10-12 (72nd) — Bounce-back only on the toolbox

**Files:** `libs/ui/KisApplication.cpp`

George: "if a window has two finger it doesn't need bounce back. We can dump bounce back except on the toolbox." After a two-finger drag, floating panels other than the toolbox (Layers, Color, Quick Adjust and the others) no longer bounce back inside the screen; like dialogs, they stay where they were let go, as long as at least 160 px of them is on screen. The toolbox still bounces back after a two-finger drag, and after a one-finger move (`KoToolBoxDocker::moveEvent`, 40th entry). The old call is kept in a comment-free branch of the same lambda (the panel branch now uses `keepReachable`).
**Not verified:** a build or a device.

## 2026-10-12 (73rd) — File menu: Batch Process... with presets (Claude's idea)

**Files:** `libs/ui/KisMainWindow.cpp`, `packaging/android/apk/src/org/krimble/android/ShareHelper.java`

Claude's idea ("Batch resize and export"; "Premade batch scripts"); George: "Premade batch scripts are a great idea. Yes!" and "Add batch presets."
- New File menu item "Batch Process..." (above Quit, with the Share items). A small window: pick a preset, tap "Choose Pictures and Start...", pick one or more pictures, and each is processed in turn with a progress bar and a result line ("Done: 5 saved in Pictures/Krimble/Batch, 0 skipped").
- Presets: **Resize to 1080 px wide (JPG)**, **Resize to 1920 px wide (JPG)**, **Small for messages: 1280 px (JPG 80%)**, **Thumbnail 256 px wide (PNG)**, **Convert to JPG (same size)**, **Convert to PNG (same size)**. Pictures narrower than the target are not enlarged. JPG output is flattened onto white. Output names are `<name>-<suffix>.jpg/.png`.
- Results go to the phone's **Pictures/Krimble/Batch** folder (new Java function `ShareHelper.saveToPictures`, MediaStore, Android 10 or newer). The originals are not changed.
- Works on pictures Qt can read (JPG, PNG, WebP, BMP); not on layered Krimble/Krita files or PSD.
- Not done: custom presets, watermarks, renaming.
**Not verified:** a build or a device. If picking pictures from the Gallery gives a file Qt cannot open, the picture is skipped and counted.

## 2026-10-12 (74th) — Batch presets use the longest side

**Files:** `libs/ui/KisMainWindow.cpp`

George: "Why 1080 wide? It's usually a height from 1080p." The batch presets' size is now the picture's longest side, so it works whichever way up the picture is: **HD 1080p: longest side 1920 px (JPG)** (a landscape picture becomes 1920 x 1080), **Social: longest side 1080 px (JPG)**, **Messages: longest side 1280 px (JPG 80%)**, **Thumbnail: longest side 256 px (PNG)**; the two convert presets are unchanged. Output names end in `-hd`, `-social`, `-msg`, `-thumb`. Pictures already smaller than the target are not enlarged. The first version is kept in a comment.
**Not verified:** a build or a device.

## 2026-10-12 (75th) — Layer Style Presets and Startup Layouts

**Files:** `libs/ui/KisMainWindow.cpp`

**Layer Style Presets** (Claude's idea; George: "Layer Styles - sounds like Alien Skin plugins. I love it. Yes."): a new submenu "Style Presets" in the Layer menu, right after "Layer Style...". One tap puts a ready-made style on the active layer, with undo, using the program's own layer style code: **Drop Shadow** (12 px distance, 14 px size, 70%), **Inner Shadow**, **Outer Glow** (white), **Inner Glow**, **Stroke (white, 4 px)**, **Stroke (black, 4 px)**, **Bevel and Emboss**, **Clear Style** (switches the whole style off). A preset adds to the layer's existing style; the full Layer Style window can fine-tune it. Not done: color overlay, gradient or pattern presets, and a way to save your own.

**Startup Layouts** (George's idea, Claude's design; George: "Yes"): two new Window menu items after Layout Presets.
- **Save Layout as Startup:** saves how the panels, toolbars and toolbox are arranged for the screen's current orientation (portrait or landscape), in the settings group `KrimbleLayouts`. A message says which one was saved.
- At start (2.5 s after launch) and 0.5 s after each screen rotation, the layout saved for that orientation is put back (if there is one).
- **Reset Layout:** forgets both saved layouts and returns to the layout the app first started with (kept as `default` the first time the app runs, after the toolbox is made floating).
- The layout is Qt's own window state (`saveState` / `restoreState`), the same mechanism Krita uses when it closes and opens.
**Not verified:** a build or a device. Restoring a layout right after a rotation interacts with the toolbox width code; if it misbehaves, "Reset Layout" is the way back.

## 2026-10-12 (76th) — Documentation (BUILD_ANDROID.md section 8) and version 34

**Files:** `BUILD_ANDROID.md`, `packaging/android/apk/build.gradle`

George: "Document everything and start build."
- `BUILD_ANDROID.md`: a new section 8 with the list of builds and tags (and the rule that a tag sits on the commit the APK was built from), the build routine in six steps, the lessons learned (6 error lines only, UTC against MDT, the order of `kb-package.sh` and `kb-apk.sh`, `ShareHelper.java` compiled at packaging, uploading the APK from the server), a table of every feature added since 1.0.31 with its main file and changelog entry, what was removed after testing, and the known open issues. A note at the start of section 2 says the version counter is retired.
- Version: `krimbleBuildNumber` 33 → 34 (`1.0.34-beta`, version code 5050434).
**Not verified:** a build.

## 2026-10-12 (77th) — Brushes panel hidden on first run; the app icon in the README

**Files:** `libs/ui/KisMainWindow.cpp`, `README.md`

George's test of 1.0.33 (the Edit, Select, Filter, View and Window menus still wide, the toolbox handle still big, brushes still loaded): those are the 1.0.33 behaviors; the menu and handle fixes are in the code since 1.0.33 (64th entry) and are in 1.0.34.
- **Brushes panel off by default:** the first time the app runs (once, setting `krimble_default_panels_done`), 2.5 s after start, the Brushes panel is hidden. It comes back through Window > Panels. It is hidden before the "default" layout for Reset Layout is kept, so Reset Layout does not bring it back.
- **README:** the app icon (the K with the paw, `packaging/android/apk/ic_launcher-playstore.png`, 512 px) is shown at 112 px at the very top, centered, above the hero picture. (George had edited the README on GitHub in the meantime: three commits, 13:31 to 13:48 MDT; this was added on top of his version.)
**Not verified:** a build. The icon is checked on a preview.

## 2026-10-12 (78th) — README: the icon moved above "What is Krimble?" and made smaller

**Files:** `README.md`

George: "Make the icon smaller and put it above What is Krimble? Not at the top." The app icon (77th entry) is no longer at the top of the README; it sits directly above the "What is Krimble?" heading, 64 px wide (was 112).

## 2026-10-12 (79th) — README: the icon is left-aligned explicitly

**Files:** `README.md`

George: "And left justify." The icon above "What is Krimble?" is wrapped in `<p align="left">` so it is left-aligned whatever the page's other settings.

## 2026-10-12 (80th) — README: the icon is big again and right-aligned

**Files:** `README.md`

George: "No, make it big again and right justify." The icon above "What is Krimble?" is 112 px wide (as in the 77th entry) and right-aligned (`<p align="right">`).

## 2026-10-12 (81st) — README: the icon is centered

**Files:** `README.md`

George: "Ok center it." The icon above "What is Krimble?" is centered (`<p align="center">`), 112 px wide.

## 2026-10-12 (82nd) — README: section headings back to normal case

**Files:** `README.md`

George: "Screen readers are the important issue. Put them back to normal case." Nine headings that had been set in ALL CAPS (commit `6fe7ad7`, "Update section headers to uppercase") are in sentence case again, with the wording they had before: What Krimble changes, New creative tools, A gift for you!, We love A.I.!, Development roadmap, Current open work, Build targets, Thank you!, Support Krimble. The emojis added in `9f37b6b` and the "!" are kept. The anchors the page links to (`#development-roadmap`, `#build-targets`, `#support-krimble`) work again for the same reason: "Build targets" was "BUILD TARGET" and the badge links to `#build-targets`.
(Emojis in headings are read aloud by screen readers too, by name; they were left because only the case was asked about.)

## 2026-10-12 (83rd) — Compile error in `KisMainWindow.cpp` (build 34)

**Files:** `libs/ui/KisMainWindow.cpp`

The first compile of the Quick Adjust, Batch, Layer Style Presets and Startup Layout code stopped on one error: "member access into incomplete type 'KisSelection'" (`kis_shared_ptr.h:201`). Quick Adjust creates an adjustment layer with an empty `KisSelectionSP()`, and that needs the full `KisSelection` class. Added `#include <kis_selection.h>`.
**Not verified:** a rebuild.

## 2026-10-12 (84th) — Toolbox height, handle curve, menu width, Preferences list (after the 1.0.34 test)

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`, `libs/ui/toolbox/KoToolBoxDocker_p.h`, `libs/ui/KisMainWindow.cpp`, `libs/widgetutils/KisWideDockSeparatorStyle.cpp`, `libs/ui/KisApplication.cpp`

George's test of 1.0.34: "Handle on toolbox is still too big. Should just be a slight curve outward. Toolbox is too tall. Should not have empty space inside the box. Menus are still too wide. ... That column of colored icons in the dialog is still too wide." (Also: "Brush icon looks great"; after floating and changing the columns setting the toolbox scales fine.)
- **Toolbox height:** a new `fitHeightToContents()` cuts a floating toolbox down to the height of its tools (the layout's height for the current width, plus the title bar and the margins). It runs 350 ms after the toolbox becomes floating (including the first start) and 450 ms after a change of columns. Capped at 95% of the screen height.
- **Handle:** the panel-edge handle is 7 x 40 px (was 12 x 48) and is drawn as a gentle bulge out from the panel's edge (a curve, no grip lines, bulging toward the canvas), instead of a rounded tab with three lines. The old drawing is kept in a comment.
- **Menus:** the width of a menu item is capped at its text plus 60 px (the left margin and the arrow), whatever the base style adds. Before, the Layer menu's first column was about 730 device px wide for text about 280 px wide. If text is clipped, the 60 px is too small.
- **Preferences:** the list of pages (the colored icons) on the left is at most 24% of the screen width (`KPageListView`, set when a window dialog is shown).
- **Not changed (asked about):** "Brushes still open by default": in the 1.0.34 screenshots the Brushes panel was not shown, so what opens is not yet known. "Portrait mode toolbox doesn't resize (docked), landscape does": not changed in this entry; the toolbox now starts floating and floating resizes correctly.
**Not verified:** a build or a device.

## 2026-10-12 (85th) — The toolbox does not bounce off the bottom

**Files:** `libs/ui/KisApplication.cpp`, `libs/ui/toolbox/KoToolBoxDocker.cpp`

George: "I think the toolbox shouldn't bounce off the bottom. Only the top or sides." A floating toolbox that is let go partly below the bottom of the screen now stays there. Left, right and top still push it back (the top keeps 48 px free for the menu bar), and at least 120 px of it must stay on screen so it cannot be lost. Done in both places that bounced it: after a two-finger drag (new `keepOnScreenSidesAndTop` in `KisApplication.cpp`) and after a one-finger move (`KoToolBoxDocker::moveEvent`). It is no longer shortened to the screen's height by the bounce. Dialogs and other panels are unchanged (they do not bounce).
**Not verified:** a build or a device.

## 2026-10-12 (86th) — New Document fits a portrait screen

**Files:** `libs/ui/KisOpenPane.cpp`, `libs/ui/widgets/kis_custom_image_widget.cc`

George's 1.0.34 screenshots of the New Document window in portrait: the colored-icon list on the left took about half the screen (about 570 of 1080 px), and with the size fields on the right the window was about two screens wide (Create and Cancel off to the right). The "column of colored icons in the dialog is still too wide" in the 84th entry was this list; the 84th entry's fix was for the Preferences list, a different window.
- **Left list:** on a screen narrower than 450 logical px (portrait) it is exactly 22% of the screen's width (`setFixedWidth`, and the minimum width is cleared), whatever its natural width. In landscape the 20% cap stays. The text under each icon wraps.
- **Predefined list:** the wider list from the 69th entry (170 px) is for landscape only; in portrait it is 104 px again, so the right side is about 700 px, not 1000.
- Expected in portrait (1080 px): list about 240 px + size fields about 700 px + margins, which fits.
**Not verified:** a build or a device. If the window is still wider than the screen, the next suspect is the white band and the color group's width.

## 2026-10-12 (87th) — View menu: Rotation Gadget on/off (Claude's design)

**Files:** `libs/ui/KisMainWindow.cpp`, `libs/ui/kis_statusbar.cc`

George: "I sometimes accidentally trigger the rotation gadget on the bottom. Can we turn it off and on at will?" The gadget is the canvas rotation dial with the angle number in the status bar.
- New check item **View > Rotation Gadget** (at the end of the View menu, after a divider). Unchecked hides the dial; checked shows it again, right away. The choice is remembered (setting `krimble_show_rotation_gadget`, default on).
- `KisStatusBar` shows the dial when a picture is opened only if the setting allows it (the old unconditional `setVisible(true)` is kept in a comment).
- The canvas can still be rotated other ways (View > Canvas Rotation, the Rotate tool); only the dial is hidden.
**Not verified:** a build or a device. If View > Rotation Gadget is missing, the View menu was rebuilt by the program; the item is added again each time the menu opens.
