// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-19: the fill methods shared by Edit > Fill > Content-Aware... and the healing brush (Smart Patch tool), and the method chosen in the
// healing brush's Tool Options (saved, so the menu command uses the same one).
#pragma once
#include <QRect>
#include <QString>
#include <kis_types.h>

namespace KrimbleFill {

enum Method {
    Sharp = 0,     // Krimble's engine (2026-10-19): copies real pixels from the best matching patches, with a seamless edge
    Smooth = 1,    // Krimble's first engine (2026-10-18): a smooth colour with blended detail; softer, good on skin and skies
    Classic = 2    // Krita's own Smart Patch fill
};

int savedMethod();              // the method chosen in the Tool Options (Sharp when none was chosen yet)
void saveMethod(int method);

// Fills the pixels of imageDev that maskDev (an 8-bit alpha device, > 127 = fill) covers, with Sharp or Smooth, on the part of the picture around them
// (inside `bounds`). Returns false when it cannot (nothing to fill, a hole too big for the memory, the picture too small, or method Classic): the caller
// then uses Krita's fill.
bool fill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, const QRect &bounds, int method);

// Krimble 2026-10-20 (2nd): what the last fill() did, in words: "Sharp engine", "Smooth engine", or why it returned false (so that Krita's fill ran):
// "Classic method chosen", "nothing selected", "area too small", "area too big", "engine could not fill", "out of memory", "nothing filled".
QString lastStatus();

}
