# Krimble Script

A very small script language for simple operations on many pictures. It is written in **Image > Batch Process...** (tick **Use a script instead**), or kept inside a saved **recipe**.

One command on each line. That is all there is: no variables, no loops, no brackets.

## The commands (12)

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

(`format`, `name` and `folder` say how the result is saved; they can be anywhere in the script and do not count as steps. The picture commands run in the order they are written.)

## Rules

- Commands and words can be in capital letters or small ones: `Resize Longest 1920` is fine.
- A **note** starts with `#` and a space: `# make them small`. A `#` followed straight away by letters or numbers is a color (`flatten #ff8800`), not a note.
- Empty lines are ignored.
- **A picture is never made bigger** unless the command ends with `grow`.
- **Without a `format` line the picture keeps its kind**: a JPG stays a JPG, a PNG a PNG, a WebP a (lossless) WebP, anything else becomes a PNG.
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

## When there is a mistake

The script is checked as you type, and the first mistake is shown under the box, with its line: for example `Line 3: "rezise" is not a Krimble Script command`. Nothing is run until the script is right. The **Help** button shows a short list of the commands.

## Recipes and scripts

- **Settings as script** writes the settings above the box as a script, to start from.
- **Save recipe...** keeps what you have, **with** the script if "Use a script instead" is ticked. A saved recipe is marked with a star in the list; **Delete** removes it.
- The built-in recipes are written in the same way (for example, "WebP for the web" is `resize longest 1920`, `format webp 85`, `name add web`).

## What it cannot do (yet)

- Animated WebP files cannot be read (they are skipped and counted).
- Layered Krimble/Krita files and PSD files are not opened (only pictures Qt or libwebp can read: JPG, PNG, WebP, BMP and the like).
- No watermarks, no text, no choosing the picture's name by a pattern, no `if`.

## For the developers

The engine is `libs/ui/KrimbleBatch.cpp` (parser: `parseScript`; runner: `runProgram`); the window is `libs/ui/KrimbleBatchDialog.cpp`. They need only Qt and, for WebP, libwebp. The tests are in `tools/batch-bench/` (`build_script_test.sh` runs 59 checks of the commands, the messages and the recipes; `build_engine.sh` and `build_dialog.sh` run the engine and the window on real pictures).
