/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISSURFACECOLORMANAGEMENTINFO_H
#define KISSURFACECOLORMANAGEMENTINFO_H

#include <kritasurfacecolormanagementapi_export.h>

#include <QObject>
#include <QFuture>

class KRITASURFACECOLORMANAGEMENTAPI_EXPORT KisSurfaceColorManagementInfo : public QObject
{
    Q_OBJECT
public:
    KisSurfaceColorManagementInfo(QObject *parent = nullptr);
    virtual ~KisSurfaceColorManagementInfo();

    virtual bool surfaceColorManagedByOS() = 0;

    virtual QFuture<QString> debugReport() = 0;
};

#endif