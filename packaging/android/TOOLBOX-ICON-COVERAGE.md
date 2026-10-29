# Krimble toolbox icon replacement — completeness gate

The October 28 concept sheet depicts 34 icon positions (six complete rows
of five and a final row of four). The original Krimble toolbox screenshot
contains more tools, so **the sheet is not a complete one-to-one replacement**.

**Do not replace toolbox icons by their grid position.** The actual number
of columns, orientation and scale are user-configurable and are not stable
identifiers. Use each tool factory's stable tool ID or registered icon name.

The last row of the concept sheet contains: assistant/reference target,
measure/angle, reference pin and edit-path-nodes. It must not be treated as
the end of the registered toolbox list.

## Required installation process

1. Inventory all tools from `KoToolRegistry` at runtime on the target build,
   including plugins, their stable IDs, displayed names and icon names.
2. Build an exact mapping from registered icon names to approved new artwork.
   Review ambiguous art (the concept repeats or confuses some selection icons).
3. For every tool, either supply a replacement at its **existing resource
   name** or deliberately retain the original icon as a fallback. No tool is
   hidden, deleted, blacklisted or omitted because an illustration is missing.
4. Provide native vector SVG icons (or high-DPI raster variants), test
   legibility at 24, 32 and 48 logical pixels and verify on dark backgrounds.
5. Verify the bottom of the toolbox in 2-, 3- and 5-column arrangements,
   horizontal and vertical orientations, and with a scrolled toolbox.
6. Capture before/after icon inventory and reject a package if the set of
   registered tool IDs changes unexpectedly.

## Status

Icon concept reviewed; asset mapping and icon replacements **not yet
installed**. No visual-only mockup should be presented as an installed asset.

## Corrected six-tool sequence beneath Rotate (verified with app owner)

These are in their existing **toolbox order**, not the artwork sheet's guessed names:

| Position | Actual function | Artwork rule |
| --- | --- | --- |
| 1 | Colorize Mask Editing | Brush with sparkles / colorization strokes; **not** a sun or Light Adjustment |
| 2 | Enclose and Fill | Loop with partly filled circle; **not** a color wheel or Color Selector |
| 3 | Assistant Tool | Crossed perspective-guide lines; use `07_01_assistant_tool.png` |
| 4 | Ruler Tool | Angle / measurement triangle; use `07_02_measure.png` |
| 5 | Reference Images Tool | Pushpin; use `07_03_reference_images.png` |
| 6 | Path-Based Selection Tool | Dashed Bézier selection with handles; use `07_04_edit_path_nodes.png` as **path selection**, not a node-editor icon |

**Important:** `02_02_contiguous_select.png` is a magic wand (contiguous
color selection), not a colorize-mask brush; `06_04_light_adjustment.png`
is a sun icon and `06_05_color_selector.png` is a color wheel. Those
three must not silently substitute for the first two bottom tools.

Before runtime installation, verify every tool factory's registered
`setIconName(...)` against these intended meanings. Leave original
artwork in place for any mismatched or unavailable design.

The 34 uploaded PNGs were copied to this feature branch as **staged assets**;
copying files alone does not replace Qt's installed toolbar icons.

## Removed incorrect artwork (2026-10-28)

The two concept files `06_04_light_adjustment.png` (sun) and
`06_05_color_selector.png` (color wheel) have been **deleted from this
feature branch**. Neither represented the actual Colorize Mask Editing or
Enclose and Fill tool. The remaining staged set contains **32 PNGs**.
New drawings for those two tools are still needed; preserve their original
application icons until replacements are correctly identified.

## Toolbox inclusion decision (2026-10-28)

**Exclude only the six bottom tools after Rotate:** Colorize Mask Editing,
Enclose and Fill, Assistant Tool, Ruler Tool, Reference Images Tool,
and Path-Based Selection Tool. All other existing toolbox tools must
remain available in the build. Do not exclude tools merely because there
is no approved replacement image or because an artwork filename is wrong.
These six exclusions concern visible toolbox registration; do not delete
underlying tools, files or artwork without a separate request.

## Revision: retain two tools (2026-10-28)

User confirmed **Path-Based Selection** and **Assistant Tool** are to be retained. Their `KoToolRegistry` registration statements have been restored. Only these four remain excluded: Colorize Mask Editing, Enclose and Fill, Ruler/Measure, and Reference Images. This revision supersedes the earlier six-tool exclusion statement above.

## Final toolbox decision (2026-10-28)

**Retain all six bottom tools.** Restore Colorize Mask Editing, Enclose and Fill, Assistant, Ruler/Measure, Reference Images, and Path-Based Selection registration. All six registrations have been restored on the feature branch. The earlier exclusions are superseded. Two incorrectly assumed concept images (sun and color wheel) remain deleted; their correct replacements are pending. All other toolbox tools remain unchanged.
