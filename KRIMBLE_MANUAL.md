# Krimble Manual

**A beginner's guide to Krimble, the graphics editor for Android.**

Written 2026-11-16 for the build after 1.0.64 beta. Krimble is beta software: things still change between builds. The full reference is inside the app: **Help > Krimble Help**.

---

## 1. What Krimble is

Krimble is a graphics editor for Android phones and tablets: an image editor for photos, graphics and design work. It is **not a paint app**. It is made from Krita and changed to work with a finger.

What it is for:

- **Editing photos:** adjust light and color, crop, resize, retouch, remove things.
- **Cutting out and combining:** select parts of a picture, and build a picture from layers.
- **Design work:** text, shapes, gradients, layer styles.
- **Preparing files:** export to JPG, PNG and WebP, and convert many pictures at once.

It uses the familiar, industry-standard names: Marquee, Lasso, Magic Wand, Layers, Adjustments, Free Transform.

---

## 2. The screen

- **Menu bar** (top): File, Edit, Image, Layer, Select, Filter, View, Window, Help. The orange **K** sits at its left.
- **Toolbox** (left): the tools. One is always active and is shown lit.
- **Panels** (right): Tool Options, Layers, Color Lab and others, as tabs.
- **Picture windows** (middle): every open picture has its own window.
- **Bottom bar**: information about the picture.

Turn the phone sideways and everything rearranges for the wide screen.

---

## 3. Your first edit

1. **File > Open** opens a picture from your phone. (**File > New** starts an empty one of the size you choose.)
2. Krimble starts with the **Hand** tool, so a finger drag moves the picture and changes nothing by accident.
3. **Image > Quick Adjust...**: move the Brightness and Contrast sliders until the picture looks right, then **OK**.
4. Pick the **Crop** tool in the toolbox, drag the frame to the part you want, and confirm.
5. Changed your mind? Tap **UNDO** at the end of the toolbox.
6. **File > Export** writes the finished picture (JPG, PNG, WebP). **File > Save** keeps your work with its layers, to go on later.

---

## 4. Moving around the picture

- **Hand tool:** drag to move the picture.
- **Two fingers:** pinch to zoom in and out.
- **View > Zoom:** Zoom In, Zoom Out, 100%, Fit on Screen.
- **View > Rotation Gadget** and **View > Zoom Gadget** add a rotation control and a zoom slider to the bottom bar. They are off until you turn them on.
- **View > Canvas Rotation > Reset Rotation** puts a turned picture upright again.

---

## 5. The toolbox

**Tap** a button to use its tool. Some buttons hold several tools of the same kind; they have a small triangle in the corner. **Press and hold** such a button and a flyout opens beside it, listing its tools by picture and name. Tap one to pick it. Tap anywhere else to close the flyout.

| Button | What it does | In its flyout |
|---|---|---|
| Move | Moves a layer or a selection | |
| Marquee | Selects a rectangle | Elliptical, Hexagon |
| Lasso | Selects a freehand outline | Polygonal, Magnetic, Path-Based |
| Magic Wand | Selects an area of similar color | Select Similar |
| Crop | Cuts the picture down to a frame | |
| Eyedropper | Picks a color from the picture | Ruler, Reference Images, Assistant |
| Bandaid | Heals a marked spot from its surroundings | |
| Brush | Applies the foreground color by hand, for touch-ups and masks | |
| Stamp | Heals by copying texture from a source spot: tap **Set source** first | |
| Eraser | Rubs parts of a layer away to transparency | |
| Gradient | Fills with a blend of colors | Paint Bucket |
| Blur | Softens where you brush | Sharpen, Smudge (pushes pixels along) |
| Pen | Draws curves point by point | Polyline, Polygon, Pencil |
| Type (T) | Adds text | Krimble Type, Old Type |
| Arrow | Picks and moves vector shapes and text | |
| Rectangle | Draws a rectangle | Ellipse, Polygon and Star, Line |
| Hand | Moves the picture | |
| Zoom | Zooms in and out | |
| Transform | Scales, turns and warps a layer | Free Rotate |
| UNDO | Tap to undo. Press and hold for Redo | |

**Arranging the toolbox.** Press and hold the toolbox's handle (the bar with three lines) for its menu:

- **Dock Left / Right / Top / Bottom** puts it on that side. **Undock** makes it float; drag it by the handle.
- **Dock by Dragging** (on or off): when off, the toolbox and the panels stay wherever you let them go and never snap to an edge.
- **Columns** sets how many columns it uses.
- **Icon Size**, **Icon Style** (Full color or Silhouette) and **Button Style** (Flat or Raised) change its look.

---

## 6. Colors

- The two squares at the bottom of the toolbox are the **foreground** color (used by the brush, fills, shapes and text) and the **background** color. Krimble starts with white and black.
- Tap a square to choose its color. The small arrows swap the two. The small black-and-white squares put them back to black and white.
- The **Color Lab** panel is the full color picker.
- The **Eyedropper** tool takes a color from the picture.

---

## 7. Fixing a photo

- **Image > Quick Adjust...**: six sliders in one window: Brightness, Contrast, Saturation, Hue, Lightness, Vibrance. The **Preview** switch shows before and after.
- **Image > Auto Tone**, **Auto Contrast**, **Auto Color**: one-tap fixes.
- **Image > Adjustments**: the full set: Levels, Curves, Hue/Saturation, Color Balance and more.
- **Crop tool**: drag the frame, then confirm.
- **Image > Image Size...** makes the picture smaller or larger. **Image > Canvas Size...** adds or removes room around it.
- **Image > Image Rotation**: turn or flip the whole picture.
- **Image > Upscale...**: makes a picture 2, 3 or 4 times larger.
- **Filter** menu: Blur, Sharpen, Stylize and others.
- **Filter > Render > Lens Flare...**: adds a camera lens flare: choose the type, drag the light on the picture with a finger (or use Light across and Light down), and set brightness, rays, ghosts, ring and color.

---

## 8. Selecting

A selection limits what you do next to one part of the picture.

- **Marquee**, **Elliptical**: one drag.
- **Lasso**: draw around the part with a finger.
- **Polygonal Lasso**: tap each corner. Finish by tapping the first point again, or **END** on the bar at the bottom.
- **Magic Wand**: tap an area; everything of similar color joining it is selected.
- **Select > All**, **Deselect**, **Inverse** (selects everything else), **Feather...** (softens the edge).

With a selection made, an adjustment or a fill touches only that part. You can also **Edit > Cut** or **Copy** it, fill it (**Edit > Fill**), or remove something from the picture with **Edit > Fill > Content-Aware...**, which fills the hole from the picture around it.

---
## 9. Layers

Layers are sheets stacked on top of each other: a photo, a cut-out placed over it, a line of text on top. Change one without touching the others.

- The **Layers** panel lists them, top sheet first. Tap a layer to work on it. The **eye** hides or shows it.
- **Layer > New > Layer** adds an empty one. **Layer > Duplicate Layer...** copies one.
- **Layer > Merge Down** joins a layer with the one under it. **Layer > Flatten Image** joins them all.
- **Layer > New > Layer Mask** adds a mask: brush black on it to hide parts of the layer, white to show them again. Nothing is erased for good.
- **Layer > Layer Style...** adds effects such as shadows and glows.

---
## 10. Text

1. Pick the **T** tool and tap the picture where the text should go.
2. Type in the text window. Choose the font, size and color; **B**, *I* and underline; alignment and spacing.
3. **OK** puts the text on the picture.

Afterward a frame surrounds the text: drag a corner to resize, drag inside to move, tap inside to edit it again.

---
## 11. Retouching and hand tools

These work by hand, directly on the picture. The brush toolbar has the size and opacity sliders; turn toolbars on in **Window > Toolbars**.

- **Bandaid:** mark a blemish and it is healed from its surroundings.
- **Stamp:** tap **Set source** on a clean spot, then brush over the flaw; the texture is copied across.
- **Blur:** softens only where you brush. In its flyout: **Sharpen** (sharpens where you brush) and **Smudge** (pushes pixels along).
- **Eraser:** rubs parts of a layer away to transparency. For erasing you can take back later, use a layer mask instead.
- **Brush:** applies the foreground color by hand: for touch-ups, and for working on masks.
- **Gradient** and **Paint Bucket** (same button): fill a layer or a selection with a blend or a flat color.
- To clear a whole area: select it, then **Edit > Clear**.

---

## 12. Saving and sharing

- **File > Save**: a Krimble/Krita file. Keeps layers. Use this for work in progress.
- **File > Export**: a flat picture. **JPG** for photos, **PNG** for sharp graphics and transparency, **WebP** for small files with transparency.
- **File > Share...**: sends the picture to another app, as JPG or PNG.
- **Image > Batch Process...**: resizes or converts many pictures at once. The originals are never changed.

---

## 13. Panels and looks

- **Window > Panels** turns each panel on or off.
- **Window > Detach Panel** makes a panel float. **Attach Panel** puts it back.
- **Window > Hide All Panels** clears the screen for the picture.
- **Window > Themes** changes the colors of the whole app: Krimble Blue-Gray, Krimble Dark Gray, Krimble Painted, Breeze Dark, Breeze Light, Breeze High Contrast.
- **Edit > Preferences...** has all the settings.

---

## 14. If something goes wrong

- **UNDO** in the toolbox, or **Edit > Undo**, takes back the last step.
- A panel is gone: **Window > Panels**.
- The toolbox is in a bad place: press and hold its handle, then **Dock Left**.
- Settings in a mess: **Help > Reset Preferences...**
- Krimble closed by itself: open it again, go to **Help > Show Krimble Log...** and send a screenshot of it with **Help > Report a Bug**.

---

## 15. More

- **Help > Krimble Help**: the full reference, with a search box.
- Website: <https://krimble.org>
- Source code: <https://github.com/purdygeorgeedward-dev/krimble>

Krimble is open source software made with the assistance of Claude and ChatGPT.
