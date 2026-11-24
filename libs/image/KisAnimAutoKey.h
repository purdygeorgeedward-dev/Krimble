/*
 *  SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_ANIM_AUTOKEY_H
#define _KIS_ANIM_AUTOKEY_H

#include "kritaimage_export.h"
#include <QFlags>
#include <QMetaType>

template<class T>
class KisSharedPtr;

class KisPaintDevice;
typedef KisSharedPtr<KisPaintDevice> KisPaintDeviceSP;

class KUndo2Command;

namespace KisAutoKey {
enum Mode {
    NONE,
    BLANK,
    DUPLICATE
};

KRITAIMAGE_EXPORT Mode activeMode();
KRITAIMAGE_EXPORT void testingSetActiveMode(Mode mode);

enum AutoCreateKeyframeFlag {
    None = 0x0,
    AllowBlankMode = 0x1,
    SupportsLod = 0x2
};

Q_DECLARE_FLAGS(AutoCreateKeyframeFlags, AutoCreateKeyframeFlag)

KRITAIMAGE_EXPORT
KUndo2Command* tryAutoCreateDuplicatedFrame(KisPaintDeviceSP device, AutoCreateKeyframeFlags flags = None);

}

Q_DECLARE_OPERATORS_FOR_FLAGS(KisAutoKey::AutoCreateKeyframeFlags)
Q_DECLARE_METATYPE(KisAutoKey::Mode)

#endif
