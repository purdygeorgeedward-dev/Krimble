/*
 *  SPDX-FileCopyrightText: 2019 Anna Medonosova <anna.medonosova@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISUPDATERBASE_H
#define KISUPDATERBASE_H

#include <QObject>
#include <KisUpdaterStatus.h>

class QString;

class KisUpdaterBase : public QObject
{
    Q_OBJECT

public:
    KisUpdaterBase();

    virtual void checkForUpdate() = 0;

    virtual bool hasUpdateCapability() = 0;

    virtual void doUpdate() = 0;

Q_SIGNALS:
    void sigUpdateCheckStateChange(KisUpdaterStatus);

protected:
    KisUpdaterStatus m_updaterStatus;
};

#endif
