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
