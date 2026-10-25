#!/usr/bin/env python3
"""
Krimble 2026-10-24: writes the retro palettes of krita/data/palettes (GIMP .gpl files), for Image > Mode > Indexed Color... (George: "8-bit 256-colour palettes (old days): where is the option?").
Run from the repository root:  python3 tools/palettes/make_retro_palettes.py
The values are the well known ones: the Macintosh 8-bit system palette (a 6 x 6 x 6 cube and four 10-step ramps), the VGA default 256 palette (16 CGA colors, 16 grays, and nine
24-hue groups, the DAC values being 6-bit), CGA, EGA, the Windows/VGA 16, the Commodore 64 (Pepto), Game Boy, Amiga Workbench.
"""
import os

OUT = "krita/data/palettes"

def write(filename, name, colors, columns=16, note=""):
    path = os.path.join(OUT, filename)
    with open(path, "w") as f:
        f.write("GIMP Palette\nName: %s\nColumns: %d\n#\n" % (name, columns))
        if note:
            f.write("# %s\n#\n" % note)
        for i, (r, g, b) in enumerate(colors):
            f.write("%3d %3d %3d\tIndex %d\n" % (r, g, b, i))
    print("%-28s %3d colors" % (filename, len(colors)))

def v6(v):   # a VGA 6-bit value to 8 bits
    return int(round(v * 255 / 63.0))

# --- Macintosh 8-bit system palette: the 216-color cube (white first), the ramps of red, green, blue and gray (10 steps each), and black last: 256
cube = [0xFF, 0xCC, 0x99, 0x66, 0x33, 0x00]
ramp = [0xEE, 0xDD, 0xBB, 0xAA, 0x88, 0x77, 0x55, 0x44, 0x22, 0x11]
mac = [(r, g, b) for r in cube for g in cube for b in cube if (r, g, b) != (0, 0, 0)]
mac += [(v, 0, 0) for v in ramp] + [(0, v, 0) for v in ramp] + [(0, 0, v) for v in ramp] + [(v, v, v) for v in ramp]
mac.append((0, 0, 0))
write("mac-8bit-256.gpl", "Macintosh 8-bit (256)", mac, note="The Macintosh system palette of 256 colors: 215 cube colors, 40 ramp colors and black.")

# --- VGA default 256-color palette (mode 13h)
cga = [(0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170), (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
       (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255), (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255)]
grays6 = [0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63]
vga = list(cga) + [(v6(g), v6(g), v6(g)) for g in grays6]
def hue_group(lo, steps):
    t = steps          # t[0] = lo ... t[4] = max
    hi = t[4]
    seq = [(lo, lo, hi), (t[1], lo, hi), (t[2], lo, hi), (t[3], lo, hi), (hi, lo, hi), (hi, lo, t[3]), (hi, lo, t[2]), (hi, lo, t[1]),
           (hi, lo, lo), (hi, t[1], lo), (hi, t[2], lo), (hi, t[3], lo), (hi, hi, lo), (t[3], hi, lo), (t[2], hi, lo), (t[1], hi, lo),
           (lo, hi, lo), (lo, hi, t[1]), (lo, hi, t[2]), (lo, hi, t[3]), (lo, hi, hi), (lo, t[3], hi), (lo, t[2], hi), (lo, t[1], hi)]
    return [(v6(r), v6(g), v6(b)) for r, g, b in seq]
for lo_steps in ([0, 16, 31, 47, 63], [31, 39, 47, 55, 63], [45, 49, 54, 58, 63],
                 [0, 7, 14, 21, 28], [14, 17, 21, 24, 28], [20, 22, 24, 26, 28],
                 [0, 4, 8, 12, 16], [8, 10, 12, 14, 16], [11, 12, 13, 15, 16]):
    vga += hue_group(lo_steps[0], lo_steps)
vga += [(0, 0, 0)] * (256 - len(vga))
write("vga-256.gpl", "VGA 256 (mode 13h)", vga, note="The default VGA palette of 256 colors: 16 CGA colors, 16 grays, 9 groups of 24 hues, 8 blacks.")

# --- CGA 16, Windows/VGA 16, EGA 64
write("cga-16.gpl", "CGA 16", cga, columns=8)
win16 = [(0, 0, 0), (128, 0, 0), (0, 128, 0), (128, 128, 0), (0, 0, 128), (128, 0, 128), (0, 128, 128), (192, 192, 192),
         (128, 128, 128), (255, 0, 0), (0, 255, 0), (255, 255, 0), (0, 0, 255), (255, 0, 255), (0, 255, 255), (255, 255, 255)]
write("windows-16.gpl", "Windows 16", win16, columns=8)
lv = [0x00, 0x55, 0xAA, 0xFF]
write("ega-64.gpl", "EGA 64", [(r, g, b) for r in lv for g in lv for b in lv], columns=8)
# --- others
c64 = ["000000", "FFFFFF", "68372B", "70A4B2", "6F3D86", "588D43", "352879", "B8C76F", "6F4F25", "433900", "9A6759", "444444", "6C6C6C", "9AD284", "6C5EB5", "959595"]
write("commodore-64-16.gpl", "Commodore 64", [tuple(int(h[i:i + 2], 16) for i in (0, 2, 4)) for h in c64], columns=8, note="The Pepto palette.")
gb = ["0F380F", "306230", "8BAC0F", "9BBC0F"]
write("game-boy-4.gpl", "Game Boy", [tuple(int(h[i:i + 2], 16) for i in (0, 2, 4)) for h in gb], columns=4)
amiga = ["0055AA", "FFFFFF", "000022", "FF8800"]
write("amiga-workbench-4.gpl", "Amiga Workbench", [tuple(int(h[i:i + 2], 16) for i in (0, 2, 4)) for h in amiga], columns=4)
write("grayscale-16.gpl", "Grayscale 16", [(v * 17, v * 17, v * 17) for v in range(16)], columns=8)
