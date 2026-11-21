**KRIMBLE**
Your Creative Companion

Krimble is a mobile-first graphics editing app for Android, with familiar industry-standard terminology, pro customizations, and touch screen oriented controls.

Krimble is open source software made with the assistance of Claude and ChatGPT.

**NEW IN THE NEXT BETA**

(Draft, kept up to date as changes are pushed after 1.0.72. The version number is set when the build is started. Known issues are filled in from the test results.)

**Auto Tone, Auto Contrast and Auto Color, rebuilt**

- These three were reported as doing nothing or just lightening the picture, and should work with curves. All three are rebuilt from published, open color-correction methods and work out a curve for each color channel from the picture itself. Transparent pixels are not counted.
- **Auto Tone** stretches red, green and blue each on its own, from the darkest 0.1% to the lightest 0.1%. This fixes washed-out pictures and can remove a color cast.
- **Auto Contrast** stretches red, green and blue together by the same amount: more contrast, no change of color.
- **Auto Color** finds the darkest and lightest colors in the picture and makes them true black and white, makes near-gray midtones truly gray without changing the overall brightness, and then gently lifts dull colors while leaving brightness and hue alone.

**Selections**

- The selected toolbox button, the selected button in button rows (such as Mode and Action in Tool Options), every selected item in Color Lab, and every other checked button now have a glossy Aqua-style blue highlight in place of the glow. The dull blue-green highlight on the Mode and Action buttons is gone.

**Color Lab**

- Grip lines on the black part of the Color Lab logo band, centered between the letters and the buttons, for grabbing and moving the panel.

**Panels and menus**

- Panel grips and the Layers panel title were changed to follow the mockup.
- Highlighting Fill in the Edit menu no longer squeezes the menu.
- The magenta box drawn by the Move tool is gone.

**Known issues**

- (from the test results)
