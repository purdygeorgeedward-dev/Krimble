/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KISFILEUTILS_H
#define KISFILEUTILS_H

#include "kritaglobal_export.h"
#include <functional>

class QString;

namespace KritaUtils {

QString KRITAGLOBAL_EXPORT resolveAbsoluteFilePath(const QString &baseDir, const QString &filePath);

QString KRITAGLOBAL_EXPORT deduplicateFileName(const QString &fileName,
                                               const QString &separator,
                                               std::function<bool(QString)> fileAllowedCallback);
}

#endif
