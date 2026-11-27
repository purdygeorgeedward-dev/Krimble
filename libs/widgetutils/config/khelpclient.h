/*  This file is part of the KDE libraries
 *  SPDX-FileCopyrightText: 2012 David Faure <faure@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef KHELPCLIENT_H
#define KHELPCLIENT_H

#include "kritawidgetutils_export.h"
#include <QString>

namespace KHelpClient
{
KRITAWIDGETUTILS_EXPORT void invokeHelp(const QString &anchor = QString(), const QString &appname = QString());
}

#endif

