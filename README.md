<div align="center">

<img src="https://krimble.org/assets/hero.jpg" alt="Krimble — Your Creative Companion" width="100%">

**A mobile-first graphics editor built from Krita for Android phones.**

[![Website](https://img.shields.io/badge/Website-krimble.org-ff7a00?style=for-the-badge)](https://krimble.org)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-white?style=for-the-badge&logo=gnu&logoColor=black)](LICENSE)
[![Platform](https://img.shields.io/badge/Android-Mobile_First-3DDC84?style=for-the-badge&logo=android&logoColor=white)](#build-targets)
[![Status](https://img.shields.io/badge/Status-1.0.0--beta2-orange?style=for-the-badge)](#development-roadmap)

[Website](https://krimble.org) · [Source](https://github.com/purdygeorgeedward-dev/krimble) · [Roadmap](#development-roadmap) · [Support Krimble](#support-krimble)

</div>

---

<p align="right"><img src="packaging/android/apk/ic_launcher-playstore.png" alt="Krimble icon" width="112"></p>

## What is Krimble?

Krimble is a **mobile-first fork of Krita** designed to be a graphics workhorse for Android phones rather than a paint/animation app squeezed onto a small screen.

The project focuses on the realities of touch input and limited screen space: removing unnecessary interface clutter, resizing and reorganizing controls, changing defaults that make more sense on a phone, improving touch targets, simplifying workflows, and adding graphics-editing tools that are useful for everyday creative work.

Krimble remains built on the extraordinary foundation of [Krita](https://invent.kde.org/graphics/krita), while deliberately taking a different direction for mobile use.

<div align="center">
<img src="https://krimble.org/assets/kimmy-phone.jpg" alt="Kimmy using Krimble on a phone" width="100%">
</div>

### What is mobile-first? 📱

Krimble is designed around **phones and touch** and reproducing a desktop workstation on a smaller display. The focus is not on painting, but on graphics editing tasks.

That means:

- touch-sized controls and resize handles
- menus and dialogs designed by screen space
- mobile-friendly defaults
- simplified tool and panel behavior
- industry-standard terminology
- workflows that do not assume a mouse or keyboard

---

## Why Krimble?

I am a lifelong graphics professional trained across traditional media and digital tools. On a phone, I do not want to paint. I want a practical graphics editor in the palm of my hand.

I found myself using Krita on my smartphone as a daily driver for graphics tasks, so I began modifying it for that purpose. Krimble is the resulting work-in-progress.

**Same job, but smaller.**

<div align="center">
<img src="https://krimble.org/assets/kimmy-cat-small.jpg" alt="Kimmy and her cat" width="360">
</div>

---

## WHAT KRIMBLE CHANGES 🪄

Krimble already includes changes to make the application more practical and familiar on mobile:

| Area | Krimble direction |
|---|---|
| **Navigation** | Pan/Hand as the default tool, touch-first movement, fewer accidental gestures |
| **Toolbox** | Reordered around familiar graphics-editing terminology |
| **Menus** | Simplified, renamed, and reorganized for mobile use |
| **Panels** | User-facing “docker” terminology replaced with **panel**; **Detach Panel** and **Attach Panel** in the Settings menu float any panel and dock it back; floating panels stay where dropped and have a corner resize handle |
| **Touch** | Larger crop handles, wider resize targets, mobile selection handles |
| **Transform** | Free Transform as the predictable default; accidental shear reduced |
| **Color** | User-friendly simple square color selector; improved Match Color |
| **Editing** | Healing Brush, Content-Aware Fill, improved adjustments and replayable Actions |
| **Branding** | Krimble identity, app labels, splash, icons, links, versioning and support graphics |
| **Shortcuts** | Large shortcut-alignment pass for familiar tool keys where practical |

---

## NEW CREATIVE TOOLS 🎨

<div align="center">
<img src="https://krimble.org/assets/holograms.jpg" alt="Kimmy exploring new Krimble tools" width="100%">
</div>

Krimble is expanding beyond interface adaptation into new graphics-editing capabilities.

Recent and active work includes:

- **Healing Brush** 🩹
- **Content-Aware Fill** 🫗
- **Vibrance color adjustment** 🌈
- **Upscaling** 🆙
- **advanced Color Match tool** 🦚
- improved graphics editing workflows
- replayable Actions with captured parameters
- additional restoration and editing tools

Some of these are already implemented; others are still in development.

---

## A GIFT FOR YOU! 🎁

<div align="center">
<img src="https://krimble.org/assets/a-gift-for-you.jpg" alt="Kimmy offering a gift — A gift for you!" width="100%">
</div>

Krimble is **free and open-source software**. 

That is more than a price. It means the program is shared with its source code under a free-software license so people can study it, modify it, improve it, build it, and share compatible versions.

Krimble is meant to be something creators can actually possess and work with — not temporary access to a service.

**Free software is the gift that keeps on giving.**

---

## WE LOVE A.I.! ❤️

<div align="center">
<img src="https://krimble.org/assets/kimmy-robot-ai.jpg" alt="Kimmy and the Krimble robot — We love A.I." width="100%">
</div>

Krimble does **not** have a blanket moratorium on artificial intelligence.

AI is treated as another class of creative technology: useful when it can make a real workflow better, inappropriate when it cannot, and always subject to experimentation, revision, and improvement.

The goal is not to turn Krimble into an “AI app.” The goal is to build a strong graphics editor and use new technology where it genuinely helps.

<div align="center">
<img src="https://krimble.org/assets/friend-robot-2.jpg" alt="Robot — I am your friend!" width="80%">
</div>


**Artificial Intelligence is like a brilliant child. Children sometimes make mistakes. That's what Daddy is there for.**

---

<div align="center">
<a href="https://www.buymeacoffee.com/GeorgeEdwardPurdy">
<img src="https://krimble.org/assets/dreams.jpg" alt="Whatever it takes to make your dreams come true." width="100%">
</a>
</div>

## DEVELOPMENT ROADMAP

Krimble has already completed a large restructuring pass. The detailed engineering log is preserved below, but collapsed so the README remains readable.

### CURRENT OPEN WORK 🚧

- Finish the text-selection **magnifier/loupe** for precise cursor placement.
- Extend two-finger movement selectively to additional floating windows where useful.
- Decide how replayable **Actions** should target layers across different documents.
- Complete and test planned **Upscaling**, **Vibrance**, and additional restoration/editing tools.
- Build and validate distribution targets.

<details>
<summary><strong>Completed and implemented work — 35 major roadmap items</strong></summary>

1. **Default tool changed to Pan/Hand.** Also fixed a separate path that forced Brush whenever a pixel layer was selected. 
2. **Smaller splash image** for mobile screens. 
3. **Selective two-finger movement** implemented for `KisDockerHud`, with touch-event handling designed not to interfere with ordinary single-finger controls.
4. **Tools menu removed** to make Settings easier to reach.
5. **Toolbox default order rebuilt** around a unified familiar graphics-tool sequence; stale factory shortcuts corrected.
6. **Automatic recovery dialog on startup disabled** for the mobile workflow.
7. **Preferences moved under Edit** and renamed appropriately.
8. **Menus reconfigured**, including Select/Layer terminology, Filter categories, and moving color-adjustment tools into Image > Adjustments.
9. **Rotation removed from pinch-zoom defaults.**
10. **Transform made safer for touch**, with shear-by-drag gated behind an explicit option; an uninitialized transform variable was also fixed.
11. **Industry-standard terminology pass**, including History, Navigator, Swatches (Palette), Actions, and Healing Brush.
12. **Snapping defaults to OFF.**
13. **200% zoom command added and tested.**
14. **Custom default toolbar created.**
15. **Brightness/Contrast dialog added and tested.**
16. **Touch resizing improved:** crop handles enlarged to mobile-scale targets and dock/panel resize separators widened.
17. **Text/Type workflow substantially rebuilt:** better menu, larger grab sensitivity, triple-tap paragraph selection, mobile selection handles, and floating Cut/Copy/Paste/Select All controls.
18. **Krimble splash branding created.**
19. **Oversized dialogs audited and reduced** for phone screens; one genuinely fixed-size blocker relaxed.
20. **Krimble app icon created and implemented** across required variants.
21. **Save-format list trimmed.**
22. **~200 hardcoded shortcuts aligned** where possible and the preferred shortcut scheme made default.
23. **Tool-selection keys reassigned** to familiar keys where possible: V, M, L, W, C, I, B, G, P, T, Z, H and related variants.
24. **User-facing “docker” terminology replaced with “panel.”**
25. **Version reset to Krimble `1.0.0-alpha1`.**
26. **Full user-facing Krita → Krimble branding pass**, while deliberately preserving internal compatibility identifiers and historical references where changing them would be harmful.
27. **Project links redirected** to Krimble resources, source, support, and placeholders for developing documentation/community pages.
28. **Panels no longer snap back into docking zones** after being dragged.
29. **Crop defaults to subtractive-only**, preventing accidental canvas growth.
30. **Default color selector changed** to square saturation/value plus hue strip for touch.
31. **Content-Aware Fill added**, using the existing PatchMatch inpainting engine with a selection-driven workflow.
32. **Match Color rebuilt** to use open documents/layers and provide Luminance, Color Intensity, Fade, and Neutralize controls.
33. **Transform always starts in Free Transform mode** unless deliberately changed in visible tool options.
34. **Actions now capture real parameters** for filters, selection modifications, and image operations rather than merely reopening empty dialogs.
35. **Dialogs are prevented from spawning above the toolbar** through a shared `KoDialog::showEvent()` correction.

</details>

<details>
<summary><strong>Bug fixes and corrections</strong></summary>

1. Fixed Move-tool behavior that could accidentally trigger whole-image movement from a touch long-press path.
2. Fixed the remaining hardcoded Brush activation in `KisView.cpp` so the Pan/Hand default is actually honored.
3. Replaced remaining “Next” launcher/adaptive icon assets that still contained the old Krita mark.
4. Corrected developer-build/version-flag handling; the current alpha status is deliberate under Krimble versioning.
5. Removed the hardcoded “Artwork by: Tyson Tan” splash credit from the Krimble-specific splash.
6. Removed a leftover invented legal-entity reference and replaced it with: **“The Krimble Project is committed to preserving Krimble as free software.”**
7. Removed references to a competing graphics application that had been inserted into the README by an AI tool.

</details>

---

## BUILD TARGET 🎯

Planned targets:

| Platform | Target |
|---|---|
| Android | arm64-v8a — Play Store |
| Android | arm64-v8a — F-Droid |
| Android | armeabi-v7a — Play Store |
| Android | armeabi-v7a — F-Droid |
| Linux | x86-64 |
| Windows | Desktop |
| iOS | Planned |
| macOS | Planned |

Android is the primary focus. Other platforms are intended to share the same Krimble workflow where practical.

---

## THANK YOU! 🙏

Krimble exists because of these. They are not connected to Krimble and have not endorsed it; we are simply grateful.

- <img src="thanks-icons/claude.svg" width="24" height="24" align="absmiddle"> **Claude** (Anthropic): helped design and write much of Krimble's code and documentation.
- <img src="thanks-icons/openai.svg" width="24" height="24" align="absmiddle"> **ChatGPT** (OpenAI): ideas, art direction and troubleshooting along the way.
- <img src="thanks-icons/oracle.svg" width="24" height="24" align="absmiddle"> **Oracle Cloud**: its free tier hosts the build server that compiles every release.
- <img src="thanks-icons/termux.svg" width="24" height="24" align="absmiddle"> **Termux**: the Android terminal that runs the whole build from a phone.
- <img src="thanks-icons/github.svg" width="24" height="24" align="absmiddle"> **GitHub**: hosts the code, the releases and the issue tracker. What would we do without it?

The logos belong to their owners and are shown only to say thank you.

---

## SUPPORT KRIMBLE 💰

<div align="center">
<a href="https://www.buymeacoffee.com/GeorgeEdwardPurdy">
<img src="https://krimble.org/assets/support-coffee.jpg" alt="Support Krimble" width="100%">
</a>
</div>

Krimble is free and open source, but development still takes time and resources.

If Krimble is useful to you, voluntary support helps fund continued development, testing, infrastructure, artwork, and future distribution.

### [☕ Support Krimble on Buy Me a Coffee](https://www.buymeacoffee.com/GeorgeEdwardPurdy)

You can also help by testing, reporting issues, improving documentation, contributing code, or simply sharing the project.

---

## Upstream Krita resources

Krimble is forked from Krita and continues to benefit from upstream Krita development.

- [Krita User Manual](https://docs.krita.org/en/user_manual.html)
- [Krita build instructions](https://docs.krita.org/en/untranslatable_pages/building_krita.html)
- [Krita developer guides and notes](https://docs.krita.org/en/untranslatable_pages.html)
- [Krita APIdox](https://api.kde.org/legacy/krita/html/index.html)
- [Krita Project Website](https://www.krita.org)
- [Upstream source](https://invent.kde.org/graphics/krita)

---

## License

Krimble, like Krita, is free software. The project as a whole is licensed under the **GNU General Public License, Version 3**. Individual files may use different compatible licenses.

See the repository's [LICENSES](LICENSES) directory for details.

---

<div align="center">

<img src="https://krimble.org/assets/thank-you.jpg" alt="Thank you from Krimble" width="430">

### Create. Edit. Enhance. Everywhere.

**Krimble — Your Creative Companion**

[krimble.org](https://krimble.org)

</div>

<div align="center">
<img src="https://krimble.org/assets/cat.jpg" alt="Kimmy's cat" width="360">
</div>
