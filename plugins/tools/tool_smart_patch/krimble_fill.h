// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <QRect>
#include <QString>
#include <kis_types.h>

namespace KrimbleFill {

enum Method {
    Sharp = 0,
    Smooth = 1,
    Classic = 2
};

int savedMethod();
void saveMethod(int method);

bool fill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, const QRect &bounds, int method);

QString lastStatus();

}
