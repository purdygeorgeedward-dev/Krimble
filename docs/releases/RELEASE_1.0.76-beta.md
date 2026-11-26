<!-- Backup of GitHub release v1.0.76-beta (Krimble v1.0.76-beta), published 2026-11-23T19:35:06Z, target release-1.0.76. Copied word for word. -->

<!-- Release notes for v1.0.76-beta, written 2026-11-23 12:25 MST for George to post on GitHub (this session is not allowed to create or edit GitHub releases). Target branch: release-1.0.76. -->

**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is open source software made with the assistance of Claude and ChatGPT.

**NEW IN 1.0.76 BETA**

**Fixes from the 1.0.75 test**

- Menus look as they did in 1.0.74: no outline, no extra space inside, and an open menu joins its title with no line between them.
- Color Lab is back exactly as it was: no empty row above its logo band, nothing pushed to the side.
- The toolbox grip has no white edge.
- Quick Mask and Screen Mode icons are smaller.
- Focused boxes and drop-downs have a plain blue edge, and text, number and drop-down boxes have square corners, so their edges meet cleanly.

**Look**

- The selected tool in the toolbox is a flat blue square. The Aqua look stays on buttons.
- Menus have a small, soft shadow on the sides and bottom, and a barely visible light edge on the left.
- Menus that spread into several columns (Panels) have a divider between columns.

**Panels**

- The animation panels are back: Animation Timeline, Animation Curves and Onion Skins.
- Five panels are left out: Digital Colors Mixer, Specific Color Selector, Gamut Masks, Recorder and Touch Panel. Color Lab, Mixer and the HSV Triangle Color Picker cover the color jobs.

**Faster and smaller**

- The bundled fonts load the first time a text tool is used instead of at every start.
- A background pass over the window now stops 30 seconds after start instead of running for as long as the app runs.
- The Aqua buttons are set up once per button instead of being checked on every redraw.
- Third-party library documentation and an unused Qt style are no longer packed into the app.

**Known issues**

- Still open from the 1.0.75 test: in portrait the window buttons overlap the menu titles; the strip for dragging the toolbox wider cannot be grabbed; the Quick Adjust window is narrow; pressed toggle buttons turn gray.
- The menu shadow is new and untested on Android: if Android does not show it as see-through, it will appear as a dark band around menus.
