/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KISEXPORTCHECKBASE_H
#define KISEXPORTCHECKBASE_H

#include <QString>

#include <kis_types.h>

#include "kritaimpex_export.h"

class KRITAIMPEX_EXPORT KisExportCheckBase
{
public:

    enum Level {
        SUPPORTED,
        PARTIALLY,
        UNSUPPORTED
    };

    KisExportCheckBase(const QString &id, Level level, const QString &customWarning = QString(), bool perLayerCheck = false);

    virtual ~KisExportCheckBase();

    virtual QString id() const;

    virtual bool checkNeeded(KisImageSP image) const = 0;

    virtual bool perLayerCheck() const;

    virtual Level check(KisImageSP image) const = 0;

    QString warning() const;

protected:

    QString m_id;
    Level m_level {UNSUPPORTED};
    QString m_warning;
    bool m_perLayerCheck {false};

};

class KRITAIMPEX_EXPORT KisExportCheckFactory
{
public:
    virtual KisExportCheckBase *create(KisExportCheckBase::Level level, const QString &customWarning = QString()) = 0;
    virtual ~KisExportCheckFactory() {}
    virtual QString id() const = 0;
};

#endif
