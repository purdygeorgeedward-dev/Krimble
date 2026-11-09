**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is free open source software made with the assistance of Claude and ChatGPT.

**NEW IN 1.0.55 BETA**

**Version numbers:** every build now gets its own number. This is build 55; the last release was 1.0.44. Builds 45 to 54 were test builds and were never released.

**Color Lab (the Color panel, rebuilt from the mockups)**

- **The Color panel is now Color Lab**, with the Color Lab logo at the top and five tabs that fill the width: **Picker, Sliders, Values, Palette and History**.
- **Picker:** a color square and hue strip, H S V boxes (switch them to HSL in the drop-down) and R G B boxes with arrows, HEX with a copy button, an eyedropper button, the foreground and background colors with swap arrows, ten recent colors, and the palette row.
- **Sliders:** choose RGB, HSV, HSL, CMYK or Grayscale and drag a bar for each channel, with number boxes, a color preview and HEX.
- **Alpha (transparency):** an **A slider** on the Sliders tab and Alpha boxes (0 to 255 and 0 to 100%) with a bar on the Values tab set the alpha channel of the foreground color. A color you pick keeps its alpha, and a semi-transparent color from another panel shows its alpha here.
- **Values:** every color model as number boxes (RGB, HSV, HSL, CMYK), HEX with copy, the name of the nearest named color, and the web safe color.
- **Palette:** named palettes, saved as files in the GIMP palette format (.gpl, which many programs read). **New, Open, Import, Export, Duplicate, Rename and Delete.** Palette info, the selected color (change it with index, HEX or R G B), swatch tools (pick from the image, replace with the foreground color, reload, number of columns), and a numbered grid. The plus on the palette row adds the foreground color to the active palette; the trash removes the selected swatch.
- **256 colors by default, and scrolling:** the default palette is **Krimble 256** (16 by 16). The palette area on the Picker, Sliders and Values tabs shows 4 rows and scrolls through any number of swatches, with a scroll bar (up and down arrows and a thumb) or the mouse wheel. A palette has no size limit.
- **CMYK palettes:** **New Palette** asks for RGB or CMYK. A CMYK palette stores the four plates of each swatch (C, M, Y and K in percent), and the Palette tab edits them with C, M, Y and K boxes. Its file is still a GIMP palette (.gpl).
- **History:** every foreground color with the tool that was active and the time, **pinned colors**, options (group duplicates, show the tool, show the time, a limit), a filter by tool, list or grid view, **Save History as Palette** and **Clear History**.
- **Mixer panel:** its "add to palette" adds the color to the active palette.
- **HSV Triangle Color Picker:** the classic color selector (ring, triangle, shade selectors and patches) is back as its own panel, in the Window menu.

**A new look**

- **New look all through the app:** long smooth gradients on the windows, panels and status bar, one blue hue everywhere, lighter panels with darker recessed lists for more contrast, white text, and rounder buttons.
- **Menu bar** with a lighter middle and darker ends, and a glowing pill on the name you touch. **Progress bars** shade smoothly from left to right, with square ends.
- **Selected items glow:** the selected tool in the toolbox, tab, list entry and OK button have a bright cyan outline, and the selected tool has a soft glow. Check boxes show a real check mark and are smaller.
- The Settings menu is gone: Preferences is under Edit, Full Screen is under View > Screen Mode, Customize Toolbar is under Window.
- **Toolbox:** tools with new icons, in Full color or Silhouette (switch it in the toolbox menu). The Gradient tool icon is a plain gradient. **UNDO** is a tool button at the end of the toolbox, with its own icon: tap to undo, long press for Redo.
- **Tool names** no longer end with the word "Tool", and the Hand tool is called **Pan**.
- **Messages** (the little notices that say something is done) now appear at the bottom left of the picture instead of over it.
- **Preferences:** your own pictures for the pages, and the whole keyboard on Keyboard Shortcuts.

**New and improved tools**

- **QuickAdjust** (first in the Image menu): Brightness, Contrast, Saturation, Hue, Lightness and Vibrance on one screen, a Preview switch, and **Keep as layers**.
- **Transform:** the tool is just called Transform, and the name at the top of Tool Options follows what it is doing (Scale, Position, Rotate, Shear, Perspective, Warp, Cage, Liquify, Mesh). Scale is uniform by default and no longer rotates by accident. The Position, Rotate, Scale and Shear buttons in Tool Options choose what a drag does. In Scale mode, a touch outside the handles moves the selection.
- **Type tool:** every new text starts on its own layer, so two texts no longer move together.
- **Line tool:** a Thickness setting.
- **Brushes panel:** Size, Opacity and Flow above the presets, and the name of the current brush.
- **Layer > New > Star...**
- **Image > Batch Process** and **Krimble Script** (from 1.0.44) are unchanged.

**Panels and windows**

- **File > New:** OK and Cancel are always visible at the bottom right, and the description box is shorter.
- **Panels** dock only when you deliberately drag one to a side. **Window > Stack Panels** also stacks panels that are floating.
- **Layers panel:** more empty space above and in front of the Layer Type drop-down, so the top of the panel is easier to grab without opening it.
- **Save As:** the file type list opens taller. Several windows keep their labels beside their fields.
- **Preferences:** a safety rule keeps rows from being pressed together inside group boxes.

**DPaint panel** (Window > DPaint, first version)

- Tools in two columns with black pixel icons on gray bevelled buttons, UNDO, redo, CLR and select all.
- The color indicator: the background color as a box with the foreground color as a dot (tap the dot or the box to pick; long press swaps them).
- A palette of 256 colors in pages of 4 x 8 wells. **RANGE**, then the first and the last well, fills the wells between them with a blend. **EDIT** changes a well. **BACK** undoes the last change.

**Known issues in this beta**

- Tearing off the toolbox can change it to one column and then to a horizontal bar.

**Install:** download the APK from this page, allow installs from this source, and open it. Beta builds are signed with the Krimble release key.
