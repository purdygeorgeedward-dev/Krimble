# Krimble — Phase 2: Creative Effects Pack (Planning)

Status: **not started** — deferred until primary dev work (core parity, adjustment panel, Content-Aware, Healing Brush, text tool) is complete.

Goal: build original, from-scratch implementations of standard, well-documented image-processing effect *categories* popularized by plugin packs like Alien Skin Eye Candy — not decompiled or ported proprietary code. Same approach as the Vibrance filter (MIT-licensed CeeJay.dk algorithm), matching *effect types*, not vendor source.

## Effect catalog

| Effect | Standard technique | Notes / controls to expose |
|---|---|---|
| **Glow / Glowing Aura** | Extract luminance or alpha mask → Gaussian blur → Screen or Add blend back onto original, tinted | Radius, intensity, color tint |
| **Metallic / Chrome** | Gradient map driven by surface normal or luminance + specular highlight pass (small reflected environment strip) | Base gradient, highlight sharpness/position |
| **Bevel / Emboss** | Pseudo-height map from alpha/luminance → derive normal map → light with directional light model | Depth, light angle, softness |
| **Drop Shadow** | Offset alpha silhouette → blur → composite behind | Distance, blur radius, opacity, color |
| **Motion Trail** | Directional blur with falloff; layered copies with decreasing opacity | Angle, length, falloff curve |
| **Lens Flare** | Hotspot/pivot-aligned flare elements (rings, streaks, halo) composited via Screen/Add | Position, brightness, lens type/preset — see KritaVFX (open source) as reference for structure, not copied code |
| **Bloom** | Threshold bright areas → blur → Add/Screen back | Threshold, intensity, radius |
| **Chromatic Aberration** | Per-channel radial offset (R/B shifted outward from center, G fixed) | Offset amount, falloff |
| **Lens Dirt** | Overlay a smudge/dust texture map, blended via Screen, modulated by scene brightness | Texture asset, intensity |
| **Light Saber** (added 2026-10-20, George) | A glowing energy blade drawn along a line, a path or a selection: a thin near-white core, a coloured inner glow (small blur radius), and a wide soft outer glow (large blur), composited with Screen/Add; optional faint flicker along the blade. A Glow variant: the same blur/Screen pieces, with a stroked line as the mask instead of the image's own alpha | Colour, core width, glow radius, intensity, blade length/angle (drag a line), flicker amount |

## Sequencing

1. Ship after: Content-Aware Fill (verified), Healing Brush (verified), Adjustments Panel, text tool rebuild.
2. Suggested build order: Drop Shadow → Glow → Light Saber (reuses Glow) → Bevel/Emboss (simplest math first, reuses shared blur/luminance utilities).
3. Chrome/Metallic, Lens Flare, Bloom, Chromatic Aberration, Lens Dirt as a second wave — share a common "render/composite" filter base class where possible.

## Licensing/attribution note

Reference implementations like KritaVFX (SockHungryClutz, open source) can inform *structure* (parameter sets, categories) but should not be copied wholesale without checking its license terms first.
