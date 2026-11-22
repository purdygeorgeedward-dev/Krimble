**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is open source software made with the assistance of Claude and ChatGPT.

**NEW IN 1.0.71 BETA**

**Selections**

- Every selection is one flat blue, the same as the menus: list rows, tabs, the selected layer, checked buttons, the New and Preferences lists and the selected Color Lab tab.
- Menus, tabs and lists are all the same hue.

**Toolbox**

- Tools at the right edge of the toolbox are easy to tap again (the edge grab area no longer reaches over them).
- The shadow on the right and bottom of the toolbox is one line thicker.

**Panels**

- Grab lines in the title rows are thin and fill the empty part of the row.
- The COLOR LAB band is Color Lab's grab area, docked or floating, with no lines drawn on it.
- Layers has one grab area (its title row), and its top rows keep their full height so they no longer squeeze together or overlap.

**Screen**

- Scrollbars are a little thicker.
- The status bar is a little shorter.

**Under the hood**

- Faster: dragging panels, the Color Lab number boxes and swatches, and Color History do less work.
- Styling shared by all Krimble themes is kept in one place.
- Unused files and code removed, the change notes inside the code removed, and more of the code split into smaller files. Nothing changes in how the app works.

**Known issues**

- Panels have thin margins.
- Krimble often restarts after switching to another app.

Found in testing this build:

- The orange K was right on top of the File menu title.
- Color Lab was easy to grab but would not dock again.
- The toolbox selection glowed; a brighter, flat blue was wanted, like the menus.
- Pictures still opened at a tiny size; the New window should start at a size based on the screen.
- Small report boxes on the canvas (sizes, zoom and the like) got in the way; they belong on the status bar.
- A gray stripe showed outside the toolbox shadow. Nothing may be drawn outside a panel's shadow.
- The toolbox bevel's shadow was just a black line; light and shadow edges should be the same thickness, like the raised buttons.
- The K on the image windows was too far from the corner.
- Krimble still restarted after switching to another app, stopping work in the middle of a workflow.
- The check mark in Layers was too close to the eye.
