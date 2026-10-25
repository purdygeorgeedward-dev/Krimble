# Krimble Help

Krimble is a graphics editor for Android phones and tablets, made from Krita and changed to work well with a finger. This help is inside the app: **Help > Krimble Help**. Use the **Go to** list to jump to a part, and the **Find** box to search.

## Moving around

- The **Hand** tool is the tool Krimble starts with: drag a finger to move the picture. Pick another tool from the toolbox when you want to draw, select or edit.
- Every open picture has its own window. Drag a window by its title bar. The three buttons in the corner make it small, make it big, and close it.
- The bottom bar shows the picture's color kind and its size.

## Toolbox, panels and toolbars

- The **toolbox** can sit on a side of the screen, or be pulled off its edge and put anywhere. Held upright, it can use two columns so that more tools fit.
- **Panels** (Tool Options, Layers, Color and others) sit on the right. Each has a lock button, a button that makes it float, and a close button. A locked panel cannot be made to float: unlock it first.
- The **Window** menu has the commands for panels and toolbars: **Detach Panel** (makes a panel float), **Attach Panel**, **Show Right Panels** and **Hide All Panels**, **Stack Panels** (puts the floating panels in a neat pile), **Toolbars**, **Customize Toolbar...** and **Lock Toolbars**.

## Files

- **File > New** starts a picture of the size you choose. **File > Open** opens pictures; each opens in its own window.
- **File > Save** keeps your work as a Krimble/Krita file with layers. **File > Export** (or Save As) writes a flat picture: JPG, PNG, WebP and others.
- **WebP** keeps transparency. In its export window, **Lossless encoding** keeps every pixel; with it off, the quality slider sets the size.

## Type tool

Pick the **T** tool and tap the picture where the text should go. The text window has four tabs:

- **Text**: type here. Below the box: the **font** (with a star to keep it as a favorite), the **size** in points, and the **color** (your recent colors, and an eyedropper that takes the foreground color). Then **B**old, *I*talic, <u>U</u>nderline, ~~S~~trikethrough, and **Aa**, which changes the capitals: UPPERCASE, lowercase, Title Case, Sentence case. Then the **alignment** (left, center, right, justify) and the **spacing**: letter spacing, line spacing, and **kerning at the cursor** (the **Tighter** and **Looser** buttons move the two letters at the cursor closer or farther apart).
- **Font**: search the fonts by name; **Favorites only** shows just your starred ones.
- **Style**: the text color: recent colors, **More colors...**, and **Use the foreground color**.
- **Layout**: the **text box width**. **No box** (0) keeps each line as you typed it. In a box the words wrap at the box's width; you can also draw a box by dragging a finger on the picture.

A preview shows the text as you change it. **OK** puts the text on the picture; **Cancel** leaves it; **Reset** puts the settings back. Held sideways, the window uses two columns.

After the text is on the picture, a **frame** appears around it: drag a **corner** to make the text bigger or smaller, drag a **side** handle to change the width of the text box, drag **inside** the frame to move the text, and **tap inside** it to open the window again.

Emoji (as plain black shapes) can be used in text.

## The Image menu

- **Mode** changes the color kind. **Indexed Color...** limits a picture to a palette (see below).
- **Adjustments**: Quick Adjust, Brightness/Contrast, Levels, Curves, Vibrance, Hue/Saturation, Color Balance, Photo Filter, Invert, Posterize, Threshold, Gradient Map, Desaturate, Match Color.
- **Auto Tone**, **Auto Contrast**, **Auto Color**: one-tap fixes.
- **Image Size...**, **Canvas Size...**, **Image Rotation**, **Crop**, **Crop to Image Size**, **Crop to Layer**, **Crop to Selection**, **Shear...**, **Offset...**.
- **Properties...**, **Purge Unused Data**, **Split Image...**, **Wavelet Decompose...**, **Separate**.
- **Upscale...** makes a picture 2, 3 or 4 times larger.
- **Batch Process...** works on many pictures at once (see below).

## Erasing: painting with transparency

- The **Eraser** tool (in the toolbox, after Brush, Smudge and Soften) is the brush that paints **transparency** instead of color. It uses the brush you have picked: its size, softness and opacity. Pick the Brush tool again to paint with color.
- Another way to the same: the **eraser switch** of the brush toolbar (the Retouch toolbar has one too; turn a toolbar on in **Window > Toolbars**). The Eraser tool turns this switch on while it is the tool, and puts it back as it was when you change tools.
- For erasing that you can change your mind about, add a **transparency mask** to the layer (**Layer > Add Transparency Mask**) and paint on it: black hides, white shows.
- To clear a whole area: select it, then **Edit > Clear**.

## Selecting

- **Rectangle, Ellipse and Lasso** selections are made with one drag.
- **Polygonal Selection:** tap or drag to put down each point. The point stays where you lift your finger. To finish, drop a point on the **first point** (it rings when it is close enough), or tap **END**. While the polygon is being made, a bar at the bottom of the picture has three buttons: **Undo point** (takes back the last point), **END** (closes the polygon and makes the selection; it needs at least three points) and **Cancel** (throws the polygon away). The same three buttons are at the top of **Tool Options**. Changing to another tool also throws an unfinished polygon away.
- **Select > Deselect** and **Edit > Undo** work on a finished selection as on any other change.

## Content-Aware Fill

Select the part you want to remove (with any selection tool), then choose **Edit > Fill > Content-Aware...**. Krimble fills the hole from the picture around it. It works best on plain or repeating backgrounds; on very busy pictures the fill can look soft.

## Indexed Color and the retro palettes

**Image > Mode > Indexed Color...** limits a picture to a palette of colors, for pixel art and old-computer looks. Krimble comes with these palettes: Macintosh 8-bit (256 colors), VGA (256), CGA (16), Windows (16), EGA (64), Commodore 64, Game Boy, Amiga Workbench and Grayscale (16).

## Batch Process

**Image > Batch Process...** changes many pictures at once, and never changes the originals: the results are new files in **Pictures/Krimble/Batch** (or the folder a script names).

1. Pick a **recipe** from the list. The built-in recipes include WebP ones (lossless; quality 90; for the web; thumbnail) and JPG and PNG ones.
2. Change the settings if you like: **Size** (keep it, or set the longest side, the width or the height), **Format** (JPG, PNG, WebP lossless, WebP lossy), **Quality**, and a bit of **text to add to the end of the name** (`photo.png` becomes `photo-web.webp`).
3. **Save recipe...** keeps your settings under a name (marked with a star in the list); **Delete** removes one of yours.
4. **Choose Pictures and Start...**, pick the pictures, and wait for "Done". **Cancel** stops after the picture it is working on.

WebP is read and written **with its transparency**. An animated **GIF** or **WebP** stays an animation when it is saved as WebP (as JPG or PNG, only its first frame is saved). For more, such as saving every frame as its own picture, use a script.

For more than the settings can do, use a script.

## Krimble Script

A very small script language for simple operations on many pictures. It is written in **Image > Batch Process...** (tick **Use a script instead**), or kept inside a saved **recipe**.

One command on each line. That is all there is: no variables, no loops, no brackets.

### The commands (15)

- `resize longest N` / `resize width N` / `resize height N`: Scale so that the longest side, the width or the height is N pixels, keeping the shape. Add `grow` at the end to allow a picture to be made **bigger** (`resize width 3000 grow`).
- `fit W H`: Scale to fit inside a box W by H pixels, keeping the shape. Add `grow` to allow making it bigger.
- `crop square`: Cut the biggest square out of the middle.
- `crop W H`: Cut a W by H piece out of the middle (never bigger than the picture).
- `rotate 90` / `180` / `270`: Turn the picture clockwise by that many degrees (`-90` also works).
- `flip h` / `flip v`: Mirror left-right (`h`) or top-bottom (`v`).
- `gray`: Black and white. (`grey` works too.)
- `brightness N`: Lighter (positive) or darker (negative), N from -100 to 100.
- `contrast N`: More (positive) or less (negative) contrast, N from -100 to 100.
- `flatten COLOR`: Put the picture on a solid color, so that nothing is transparent. COLOR is a name (`white`, `black`, `gray`, `red`, `green`, `blue`, `yellow`, `orange`, `purple`, `pink`, `brown`) or `#rrggbb`.
- `format jpg [Q]`: Save as JPG, quality Q from 1 to 100 (90 if left out). Transparent parts become white.
- `format png`: Save as PNG (keeps transparency).
- `format webp [Q]`: Save as WebP, quality Q from 1 to 100 (85 if left out). **Keeps transparency.**
- `format webp lossless`: Save as lossless WebP: every pixel as it was, transparency kept.
- `name add TEXT`: Add `-TEXT` to the end of the name: `photo.png` becomes `photo-web.webp`.
- `name prefix TEXT`: Put `TEXT` at the start of the name: `web-photo.webp`.
- `folder NAME`: Save in `Pictures/Krimble/NAME` (the folder is `Batch` if there is no `folder` line).
- `frames`: For an animated GIF or WebP: save every frame as its own numbered picture (`walk-001.png`, `walk-002.png`...) instead of one animation.
- `fps N`: For an animation: show N frames each second (1 to 100), instead of the times the file has.
- `loop N`: For an animation: play N times (0 for ever), instead of what the file says.

(`format`, `name`, `folder`, `frames`, `fps` and `loop` say how the result is saved; they can be anywhere in the script and do not count as steps. The picture commands run in the order they are written.)

### Rules

- Commands and words can be in capital letters or small ones: `Resize Longest 1920` is fine.
- A **note** starts with `#` and a space: `# make them small`. A `#` followed straight away by letters or numbers is a color (`flatten #ff8800`), not a note.
- Empty lines are ignored.
- **A picture is never made bigger** unless the command ends with `grow`.
- **Without a `format` line the picture keeps its kind**: a JPG stays a JPG, a PNG a PNG, a WebP a (lossless) WebP, a GIF becomes a (lossless) WebP (a GIF cannot be written yet), anything else becomes a PNG.
- `TEXT` in `name` is made of letters, numbers, `-` and `_` (no spaces, dots or slashes). `NAME` in `folder` can also have spaces.
- At most **40** picture commands in a script.
- The originals are **never changed**: the results are new files.

### Examples

**Pictures for a web page, as WebP**
```
resize longest 1920
format webp 85
name add web
```

**Transparent PNG pictures to lossless WebP, nothing else changed**
```
format webp lossless
```

**Square thumbnails for a gallery**
```
crop square
resize width 400
format webp 80
name add thumb
folder Thumbnails
```

**Black and white copies**
```
gray
contrast 20
name add bw
```

**A transparent logo on white, as a JPG for a form that wants JPG**
```
flatten white
format jpg 95
name add jpg
```

**Scans that came sideways**
```
rotate 90
resize longest 2000
format png
```

### Animated pictures

Animated **GIF** and animated **WebP** files are read, frame by frame, with each frame's time and the transparency.

- **Saved as WebP, the animation stays an animation** (`format webp 85` or `format webp lossless`; and with no `format` line a GIF becomes a lossless animated WebP). The file is much smaller than the GIF.
- **Saved as JPG or PNG, only the first frame is saved**, because those cannot move. To get every frame, add `frames`.
- **`frames`** writes each frame as its own picture, in whatever format is set: `walk-001.png`, `walk-002.png`... (with `name add` and `folder` as usual). This is the way to take an animation to a game that wants separate frames, such as Wesnoth.
- **All the picture commands work on every frame**: `resize`, `crop`, `rotate`, `flip`, `gray`, `brightness`, `contrast`, `flatten`.
- **`fps` and `loop`** change the speed and the number of plays. A GIF frame with a time of 0.01 second or less is shown for 0.1 second, as web browsers do.
- A very big animation is skipped: at most 2000 frames, and about 60 million pixels in all (the frames times the width times the height).

Two examples:

**An animated GIF to a small animated WebP, half the size**
```
resize width 128
format webp 80
name add small
```

**The frames of an animation, as pictures for a game**
```
format png
frames
name add walk
folder Frames
```

How small? Measured on two test animations with flat colors and transparency (a painted sprite will be bigger): a 72 by 72 bouncing ball of 10 frames is 9.9 KB as a GIF and **1.8 KB** as a lossless WebP; a 256 by 256 picture of 20 frames is 24.6 KB as a GIF and **12.3 KB** as a lossless WebP.

### When there is a mistake

The script is checked as you type, and the first mistake is shown under the box, with its line: for example `Line 3: "rezise" is not a Krimble Script command`. Nothing is run until the script is right. The **Help** button shows a short list of the commands.

### Recipes and scripts

- **Settings as script** writes the settings above the box as a script, to start from.
- **Save recipe...** keeps what you have, **with** the script if "Use a script instead" is ticked. A saved recipe is marked with a star in the list; **Delete** removes it.
- The built-in recipes are written in the same way (for example, "WebP for the web" is `resize longest 1920`, `format webp 85`, `name add web`).

### What it cannot do (yet)

- **An animated GIF cannot be written** yet (it can be read, and written as WebP). Animated PNG (APNG) is not read or written. A set of numbered pictures cannot be made into an animation yet.
- A GIF is read with Qt's own GIF reader. If this build of Krimble does not have it, GIF files are skipped and counted.
- Layered Krimble/Krita files and PSD files are not opened (only pictures Qt or libwebp can read: JPG, PNG, WebP, BMP and the like).
- No watermarks, no text, no choosing the picture's name by a pattern, no `if`.

## Help menu, settings and support

- **Help > Report a Bug**, **Feature Request**, **Krimble Website** and **Krimble Forum** open the Krimble pages in your browser: <https://krimble.org>.
- **Help > Presets...** and **Resource Libraries...** manage brushes, patterns, palettes and other resources.
- **Help > Author Profile...** sets the name that is kept in the pictures you make.
- **Help > Reset Preferences...** puts all the settings back as they were. **Help > Reset Tool Options on Start** makes every tool start with its default options.
- **Help > About** shows who made Krimble, and thanks everyone it was built with.
- The program is open source: <https://github.com/purdygeorgeedward-dev/krimble>.
