/*
 *  SPDX-FileCopyrightText: 2015 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_DOCKWIDGETFACTORY_H
#define LIBKIS_DOCKWIDGETFACTORY_H

#include <QString>
#include <KoDockFactoryBase.h>

#include "kritalibkis_export.h"

class KRITALIBKIS_EXPORT DockWidgetFactoryBase : public KoDockFactoryBase
{
public:
    DockWidgetFactoryBase(const QString& _id, DockPosition _dockPosition);
    ~DockWidgetFactoryBase() override;
    QString id() const override;
    DockPosition defaultDockPosition() const override;
private:
    QString m_id;
    DockPosition m_dockPosition;
};

#endif
