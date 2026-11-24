/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_COLOR_MANAGER_H
#define KIS_COLOR_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

#include "kritacolor_export.h"
class KRITACOLOR_EXPORT KisColorManager : public QObject
{
    Q_OBJECT

public:
    explicit KisColorManager();
    ~KisColorManager() override;

    enum DeviceType {
        screen,
        printer,
        camera,
        scanner
    };

    QString deviceName(const QString &id);

    QStringList devices(DeviceType type = screen) const;

    QByteArray displayProfile(const QString &device, int profile = 0) const;

    static KisColorManager *instance();

Q_SIGNALS:

    void changed(const QString device);

public Q_SLOTS:

private:

    KisColorManager(const KisColorManager&);
    KisColorManager operator=(const KisColorManager&);

    class Private;
    const Private *const d;
};

#endif
