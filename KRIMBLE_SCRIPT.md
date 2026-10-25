# Krimble Script

A very small script language for simple operations on many pictures. It is written in **Image > Batch Process...** (tick **Use a script instead**), or kept inside a saved **recipe**.

One command on each line. That is all there is: no variables, no loops, no brackets.

## The commands (15)

| Command | What it does |
|---|---|
| `resize longest N` / `resize width N` / `resize height N` | Scale so that the longest side, the width or the height is N pixels, keeping the shape. Add `grow` at the end to allow a picture to be made **bigger** (`resize width 3000 grow`). |
| `fit W H` | Scale to fit inside a box W by H pixels, keeping the shape. Add `grow` to allow making it bigger. |
| `crop square` | Cut the biggest square out of the middle. |
| `crop W H` | Cut a W by H piece out of the middle (never bigger than the picture). |
| `rotate 90` / `180` / `270` | Turn the picture clockwise by that many degrees (`-90` also works). |
| `flip h` / `flip v` | Mirror left-right (`h`) or top-bottom (`v`). |
| `gray` | Black and white. (`grey` works too.) |
| `brightness N` | Lighter (positive) or darker (negative), N from -100 to 100. |
| `contrast N` | More (positive) or less (negative) contrast, N from -100 to 100. |
| `flatten COLOR` | Put the picture on a solid color, so that nothing is transparent. COLOR is a name (`white`, `black`, `gray`, `red`, `green`, `blue`, `yellow`, `orange`, `purple`, `pink`, `brown`) or `#rrggbb`. |
| `format jpg [Q]` | Save as JPG, quality Q from 1 to 100 (90 if left out). Transparent parts become white. |
| `format png` | Save as PNG (keeps transparency). |
| `format webp [Q]` | Save as WebP, quality Q from 1 to 100 (85 if left out). **Keeps transparency.** |
| `format webp lossless` | Save as lossless WebP: every pixel as it was, transparency kept. |
| `name add TEXT` | Add `-TEXT` to the end of the name: `photo.png` becomes `photo-web.webp`. |
| `name prefix TEXT` | Put `TEXT` at the start of the name: `web-photo.webp`. |
| `folder NAME` | Save in `Pictures/Krimble/NAME` (the folder is `Batch` if there is no `folder` line). |
| `frames` | For an animated GIF or WebP: save every frame as its own numbered picture (`walk-001.png`, `walk-002.png`...) instead of one animation. |
| `fps N` | For an animation: show N frames each second (1 to 100), instead of the times the file has. |
| `loop N` | For an animation: play N times (0 for ever), instead of what the file says. |

(`format`, `name`, `folder`, `frames`, `fps` and `loop` say how the result is saved; they can be anywhere in the script and do not count as steps. The picture commands run in the order they are written.)

## Rules

- Commands and words can be in capital letters or small ones: `Resize Longest 1920` is fine.
- A **note** starts with `#` and a space: `# make them small`. A `#` followed straight away by letters or numbers is a color (`flatten #ff8800`), not a note.
- Empty lines are ignored.
- **A picture is never made bigger** unless the command ends with `grow`.
- **Without a `format` line the picture keeps its kind**: a JPG stays a JPG, a PNG a PNG, a WebP a (lossless) WebP, a GIF becomes a (lossless) WebP (a GIF cannot be written yet), anything else becomes a PNG.
- `TEXT` in `name` is made of letters, numbers, `-` and `_` (no spaces, dots or slashes). `NAME` in `folder` can also have spaces.
- At most **40** picture commands in a script.
- The originals are **never changed**: the results are new files.

## Examples

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

## Animated pictures

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

## When there is a mistake

The script is checked as you type, and the first mistake is shown under the box, with its line: for example `Line 3: "rezise" is not a Krimble Script command`. Nothing is run until the script is right. The **Help** button shows a short list of the commands.

## Recipes and scripts

- **Settings as script** writes the settings above the box as a script, to start from.
- **Save recipe...** keeps what you have, **with** the script if "Use a script instead" is ticked. A saved recipe is marked with a star in the list; **Delete** removes it.
- The built-in recipes are written in the same way (for example, "WebP for the web" is `resize longest 1920`, `format webp 85`, `name add web`).

## What it cannot do (yet)

- **An animated GIF cannot be written** yet (it can be read, and written as WebP). Animated PNG (APNG) is not read or written. A set of numbered pictures cannot be made into an animation yet.
- A GIF is read with Qt's own GIF reader. If this build of Krimble does not have it, GIF files are skipped and counted.
- Layered Krimble/Krita files and PSD files are not opened (only pictures Qt or libwebp can read: JPG, PNG, WebP, BMP and the like).
- No watermarks, no text, no choosing the picture's name by a pattern, no `if`.

## For the developers

The engine is `libs/ui/KrimbleBatch.cpp` (parser: `parseScript`; runner: `runProgram`); the window is `libs/ui/KrimbleBatchDialog.cpp`. They need only Qt and, for WebP, libwebp. The same text is inside the app, in **Help > Krimble Help**: after changing this file, run `python3 tools/help/make_help.py` (it rebuilds `krita/data/aboutdata/krimble-help.md` from this file and the parts in `tools/help/`). The tests are in `tools/batch-bench/` (`build_script_test.sh` runs 59 checks of the commands, the messages and the recipes; `build_engine.sh` and `build_dialog.sh` run the engine and the window on real pictures).
