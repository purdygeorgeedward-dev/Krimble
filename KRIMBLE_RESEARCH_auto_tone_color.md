# Research: Auto Tone, Auto Contrast, Auto Color (Nov 20, 2026, 23:33 MST)

Asked for: "Research these features online. Look for open functional algorithms." (after "These should be based on curve profiles.", Oct 26, and "Auto Color should default to sweetening the colour saturation without changing the overall tone", Oct 23)

## What the industry-standard program does (published descriptions)
- Its auto options are four algorithms: Enhance Monochromatic Contrast, Enhance Per Channel Contrast, Find Dark & Light Colors, Enhance Brightness and Contrast (the last is machine learning, not reproducible).
- Per Channel Contrast (Auto Tone / Auto Levels): stretches R, G and B separately; default clip 0.1%; can remove or add a color cast.
- Monochromatic Contrast (Auto Contrast): one stretch on all channels together, so no color shift.
- Find Dark & Light Colors (Auto Color): finds the average darkest and lightest pixels and uses them as shadow and highlight colors while minimizing clipping; by default neutralizes midtones to RGB 128 gray and clips 0.5%.
- Snap Neutral Midtones: finds an average midtone color close to neutral and adjusts the gamma of each channel to make it neutral.
- The result of each is a curve (or levels) per channel, which is why it can be put into Curves and adjusted.

Sources: Adobe help (quoted on community.adobe.com), astropix.com (Photoshop for Astrophotographers), creativepro.com, issuu (Andrew Haysom).

## Open algorithms found
1. **Simplest Color Balance** (Limare, Lisani, Morel, Petro, Sbert; IPOL 2011, peer-reviewed, source code published): per channel, saturate a small percentage of the darkest pixels to 0 and the brightest to 255, stretch the rest with an affine map. = Auto Tone. There is also a variant on the intensity axis applied equally to R, G, B (no color cast) = Auto Contrast. https://www.ipol.im/pub/art/2011/llmps-scb
2. **GIMP White Balance**: per channel stretch discarding the 0.05% tails. **GIMP Color Enhance**: stretches the saturation range in HSV without changing brightness or hue (= the saturation "sweetening" for Auto Color). **GIMP Levels Auto** (gimp_levels_config_stretch).
3. **ImageMagick** -auto-level (stretch min/max) and -auto-gamma (gamma = log(mean)/log(0.5) per channel: the midtone part).
4. **darktable levels, automatic mode**: black, gray and white points as percentiles of the histogram.
5. **Neutral gamma** (Kodak patent 7068853, expired-era method, described publicly): neutral = (r+g+b)/3; gamma from the mean of near-neutral pixels.

## License note
Krita/Krimble is GPL-2.0-or-later. GIMP is GPL-3.0: copying its code would force GPL-3.0. Algorithms are not copyrighted, so Krimble's version is written new from these descriptions, not copied.

## Proposed Krimble implementation (all produce curves, shown in Curves so they can be adjusted)
- **Auto Tone**: per channel black/white points at 0.1% clip (Simplest Color Balance per channel), then a mild midtone gamma per channel toward the overall mean.
- **Auto Contrast**: black/white points from the luminance (Rec. 709) histogram at 0.1% clip, one identical curve for R, G, B: no color shift.
- **Auto Color**: dark and light colors = the average of the darkest and lightest 0.5% (by luminance), mapped to black and white per channel; Snap Neutral Midtones: average of near-neutral midtone pixels (low chroma, luminance 25 to 75%), per-channel gamma so it lands on 128 gray; then a gentle saturation lift that keeps brightness and hue (Color Enhance / Vibrance style, weighted toward less-saturated colors).
- Transparent pixels never count (309th fix).
