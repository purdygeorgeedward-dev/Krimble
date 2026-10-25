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

WebP is read and written **with its transparency**. (Animated WebP and GIF files are not read yet.)

For more than the settings can do, use a script.

