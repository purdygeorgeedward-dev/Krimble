/*
 *  SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISUSAGELOGGER_H
#define KISUSAGELOGGER_H

#include <QString>
#include <QScopedPointer>

#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KisUsageLogger
{

public:

    KisUsageLogger();
    ~KisUsageLogger();

    static void initialize();
    static void close();

    static QString basicSystemInfo();

    static void writeLocaleSysInfo();

    static void log(const QString &message);

    static void writeSysInfo(const QString &message);

    static void writeHeader();

    static QString screenInformation();

private:

    void rotateLog();

    Q_DISABLE_COPY(KisUsageLogger)

    struct Private;
    const QScopedPointer<Private> d;

    static const QString s_sectionHeader;
    static const int s_maxLogs {20};

};

#endif
