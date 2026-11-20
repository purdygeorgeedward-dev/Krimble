/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KritaVersionWrapper.h"

#include <kritaversion.h>
#include <kritagitversion.h>
#if defined(__has_include)
#  if __has_include(<krimble_build_stamp.h>)
#    include <krimble_build_stamp.h>
#  endif
#endif

QString KritaVersionWrapper::versionString(bool checkGit)
{
    QString kritaVersion = QStringLiteral(KRITA_VERSION_STRING);
    QString version = kritaVersion;

    if (checkGit) {
#ifdef KRIMBLE_BUILD_NUMBER
        QString suffix;
#  if defined(KRITA_BETA)
        suffix = QStringLiteral("-beta");
#  elif defined(KRITA_ALPHA)
        suffix = QStringLiteral("-alpha");
#  endif
        // "1.0" from "1.0.0-beta2"
        version = QStringLiteral("%1.%2%3").arg(kritaVersion.section(QLatin1Char('.'), 0, 1),
                                                QStringLiteral(KRIMBLE_BUILD_NUMBER),
                                                suffix);
        QString detail = QStringLiteral(KRIMBLE_BUILD_DATE);
#  ifdef KRIMBLE_BUILD_GIT
        detail = QStringLiteral("%1, git %2").arg(detail, QStringLiteral(KRIMBLE_BUILD_GIT));
#  elif defined(KRITA_GIT_SHA1_STRING)
        detail = QStringLiteral("%1, git %2").arg(detail, QStringLiteral(KRITA_GIT_SHA1_STRING));
#  endif
        version = QStringLiteral("%1 (%2)").arg(version, detail);
#else
        // (original behaviour)
#  ifdef KRITA_GIT_SHA1_STRING
        QString gitVersion = QStringLiteral(KRITA_GIT_SHA1_STRING);
        version = QStringLiteral("%1 (git %2)").arg(kritaVersion, gitVersion);
#  endif
#endif
    }
    return version;
}

bool KritaVersionWrapper::isDevelopersBuild()
{
#if defined(KRITA_STABLE)
    return false;
#else
    return true;
#endif
}
