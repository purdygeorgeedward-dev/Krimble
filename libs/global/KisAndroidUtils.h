/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef __KISANDROIDUTILS_H_
#define __KISANDROIDUTILS_H_

#include <QString>
#include <kritaglobal_export.h>

class QTemporaryFile;

namespace KisAndroidUtils
{

KRITAGLOBAL_EXPORT void performInitialSetup();

KRITAGLOBAL_EXPORT bool looksLikeXiaomiDevice();

KRITAGLOBAL_EXPORT bool isLowMemoryKillReportSupported();

KRITAGLOBAL_EXPORT void clearJniException(const QString &location);

KRITAGLOBAL_EXPORT bool isInFullScreen();

KRITAGLOBAL_EXPORT void setFullScreen(bool fullScreen);

KRITAGLOBAL_EXPORT bool
copyFile(const QString &inputPath, const QString &outputPath, QString *outErrorMessage = nullptr);

KRITAGLOBAL_EXPORT bool
copyFileToTemporary(const QString &inputPath, QTemporaryFile &outputFile, QString *outErrorMessage = nullptr);

}

#endif
