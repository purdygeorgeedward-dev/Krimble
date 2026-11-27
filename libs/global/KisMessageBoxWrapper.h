/*
 *  SPDX-FileCopyrightText: 2025 Halla Rempt <halla@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISMESSAGEBOXWRAPPER_H
#define KISMESSAGEBOXWRAPPER_H

class QMessageBox;
class QString;

#include "kritaglobal_export.h"

namespace KisMessageBoxWrapper {

KRITAGLOBAL_EXPORT int doNotAskAgainMessageBoxWrapper(QMessageBox *messageBox, const QString &identifier);

}

#endif
