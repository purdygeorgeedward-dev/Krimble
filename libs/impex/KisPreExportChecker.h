/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISPREEXPORTCHECKER_H
#define KISPREEXPORTCHECKER_H

#include <kis_types.h>
#include "kritaimpex_export.h"

#include "KisExportCheckBase.h"

class KisExportConverterBase;

class KRITAIMPEX_EXPORT KisPreExportChecker
{
public:
    KisPreExportChecker();

    bool check(KisImageSP image, QMap<QString, KisExportCheckBase *> filterChecks);
    QStringList warnings() const;
    QStringList errors() const;

private:

    QStringList m_errors;
    QStringList m_warnings;
};

#endif
