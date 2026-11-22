**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is open source software made with the assistance of Claude and ChatGPT.

**NEW IN 1.0.65 BETA**

**Version numbers:** this is build 65. The last release was 1.0.58 beta.

**Painting**

- Painting no longer jolts: the canvas was being moved twice every time the screen laid itself out.
- A painting tool with no brush takes the built-in Krimble Brush.

**Toolbox**

- Krimble starts with the Hand tool, not the arrow.
- The tools are in a new order: Move, Marquee, Lasso, Magic Wand, Crop, Eyedropper, Bandaid, Brush, Stamp, Eraser, Gradient, Blur, Sharpen, Pen, Type, Arrow, Shapes, Hand, Zoom, Transform, UNDO.
- New Sharpen tool.
- Soften is named Blur, with Smudge in its flyout. The Bandaid and the Stamp each have their own button.
- The two type tools are named Krimble Type and Old Type.
- The Screen Mode button is gone (View > Screen Mode still works).
- Flyouts open after 250 ms.
- The Polygon and Star tool has its icon.

**Looks**

- Raised toolbox buttons work in every theme but Krimble Dark Gray.
- Tabbed windows: the selected tab and its page share one color, with no line between.
- Selected rows in lists and menus have the blue selection gradient.
- New ball bearing slider handles with a soft cast shadow.
- File Info has a new information icon.

**Menus and windows**

- The image window buttons are centered in the menu bar.
- With no image open, "File" starts in the same place as with one.
- Tabs for open images are the default again.
- The QuickAdjust toolbar is removed.

**Text**

- Industry-standard brand names are replaced with plain names throughout (for example "PSD image", "Soft Light (Standard)"). "Adobe RGB" stays.

**Known issues**

- The menu bar was made taller, which left a gap between a menu and its title.
- File sits too close to the K with an image open.
- The toolbox shadow is a black line.
- Foreground and background start dark gray.
- Crashes, including one on a simple fill; cause not found.

Found in testing this build:

- The menu bar's extra shadow lines disappeared, and a menu showed detached from its title.
- An odd number of tools made the two toolbox columns lopsided; Sharpen should be in the Soften/Blur flyout.
- The healing brush icon looked like a stamp.
- The toolbox would not stay docked at the bottom; it snapped to the side. Docking should be switchable on and off.
- Canvas Input settings: the selection overlapped the keyboard title area.
- File > New: the selection covered the icon.
- Recent images should be a bigger scrolling area with smaller previews.
- The Layers panel was a mess: black bars at the bottom, a useless search field, layer colors; the selected layer should be the selection blue and the others dark.
- The canvas background should be darker.
- The edge of the right panel was hard to move again.
- Save As was far too wide (more height was wanted, not width), and it did not remember a size the user set.
- Layers snapped into a column for no reason.
- OK did not close Quick Adjust.
- The little arrow buttons beside the image tabs were too thin.
- Some features were noticeably slower, and many files opened zoomed far out.
