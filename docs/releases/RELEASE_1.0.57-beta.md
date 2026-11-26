<!-- Backup of GitHub release v1.0.57-beta (Krimble v1.0.57-beta), published 2026-11-13T09:52:49Z, target cdca33dad25f134b72dadc3fe41e8ec46abd5133. Copied word for word. -->

# Krimble 1.0.57-beta

**Your Creative Companion.** Android build for touchscreens, forked from Krita.
Built on GitHub Actions from commit `cdca33d` (November 12, 2026). Tag: `v1.0.57-beta`.

This build includes everything since 1.0.56-beta (November 11).

---

## Toolbox

- **Long-press groups.** Hold a tool that has a small triangle in its corner and a list opens next to it: **Marquee** (Rectangular, Elliptical, Hexagon), **Lasso** (Outline, Polygonal, Magnetic) and **Wand** (Contiguous, Similar). Tap a row to pick the tool.
- **Hexagon Selection** (new): drag out a regular hexagon, flat top or pointed top. **Snap to the hex grid** puts the hexagon on the nearest cell of the grid you set in View > Show > Hex, and a tap selects one cell. **Lock hex settings** keeps the size and orientation fixed (no rotating or scaling) until you unlock it.
- **Button Style** in the toolbox menu: Flat (default) or Raised.
- The selected-tool glow has sharp corners again.

## Look and feel

- **Krimble Blue-Gray** is the default theme again with the painted look: one-pixel light and dark lines on panels, tool bars and image windows, and menus that look recessed. Menu separators are a black line over a highlight line. The theme list is down to 10 themes.
- Menu bar items have their old height back.
- The lock buttons on panels are hidden.
- **QuickAdjust** is rebuilt as a grid of rows with a new header, new number box arrows, and the Preview switch on the bottom row.
- **View > Show > Hex** has line controls: **Hex Lines...** sets thickness, color and opacity, with a live preview. **Hex Size...** and **Lock Hex Settings** are in the same menu.

## JPEG

- **SmartCompression** (first step): a new tab in the JPEG save dialog. A level slider, three protected-area choices (Smart, the picture's selection, none) and a sensitivity slider. Blocks with detail are left alone; flat areas are compressed harder. A picture that is all gray is saved as a one-channel gray JPEG.

---

## Known issues

- The toolbox can start floating on the first run and lay out crookedly (fixed in the next build: it starts docked at the left with two columns).
- The SmartCompression logo and tab, and the JPEG dialog as a whole, are too big (fixed in the next build).
- The New dialog is too large and its hues clash (fixed in the next build).
- The menu bar starts without its lines, then changes (fixed in the next build).
- Color Lab zooms with the panel like a picture instead of reflowing (to be rebuilt).
- Channels panel: loading a channel as a selection and showing one channel alone are not done yet.
- Transparent text can export differently from how it looks on the canvas.
- The CMYK interface is partial.

## Install

Download `Krimble-1.0.57-beta-<date>-<time>.apk`, copy it to your phone and open it. Android may ask you to allow installs from your file manager or browser.

*Krimble is free software. Purdy Design fonts are free for non-commercial use.*
