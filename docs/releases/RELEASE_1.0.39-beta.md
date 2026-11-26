<!-- Backup of GitHub release v1.0.39-beta (Krimble v1.0.39-beta), published 2026-10-19T23:42:50Z, target 94c6c8ff94e362f52f66639b05a39dcb7292ac67. Copied word for word. -->

**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, a fork of Krita, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is free open source software made with the assistance of Claude and ChatGPT.

**NEW IN 1.0.39 BETA**

- **A new Content-Aware Fill engine, written for Krimble.** Edit > Fill > Content-Aware... now uses Krimble's own fill instead of Krita's Smart Patch. It does well on small removals (see Known Issues).

- **52 open-license font families** (OFL and Apache licensed) are bundled with the app, with their licenses, and credited on the About page.

- **Free Rotate turns with one finger.** Drag a finger around the layer to turn it; two fingers stay free for zooming the canvas.

- **The Window menu no longer gets stuck open:** it is not rebuilt while it is open, and open menus close when you come back to Krimble from another app.

- **The File > New list:** the two top icons (Custom Document and Create from Clipboard) are a little smaller, so they match the template pictures.

**KNOWN ISSUES**

- **Content-Aware Fill smears detail.** Small removals look fine; anything with texture (grass, brick, hair) comes out blurry. A sharper engine is being worked on.
- **The new fonts do not show in the Type tool's font list**, which still shows many foreign-language Noto fonts. Emoji do not display in the Type tool.
- **Free Rotate shows the angle but the picture does not turn until you apply.**
- **A tap on the canvas right after a crop undoes the crop.** Use another tool after cropping.
- **In Add selection mode, a stroke started near the selection's edge moves the selection** instead of adding to it.
- The toolbox resize zone gets in the way of selections near the toolbox. The toolbox can snap to an edge when it is not close to one, and when docked at the top it has empty space under the tools. Horizontal does not reshape a floating toolbox.
- Switching transform modes throws away the transform not yet applied. The transform mode buttons are small and do not show which one is chosen.
- The File > New icons are too big and their column is too wide.
- The Type tool makes one line of text only (no text boxes, no restyling part of the text).
- Switching to another app can restart Krimble. Krimble autosaves every 7 minutes and offers to recover your picture; if it happens a lot, set Krimble to Unrestricted in the phone's battery settings.

Fixes for most of these are already made for the next version.

This is a beta. Please report problems on the Issues page.

**Install:** download the APK below and open it on your phone (arm64 devices).
