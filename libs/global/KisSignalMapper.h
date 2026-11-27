/****************************************************************************
**
** SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KFQF-Accepted-GPL OR LicenseRef-Qt-Commercial
**
****************************************************************************/

#ifndef KisSignalMapper_H
#define KisSignalMapper_H

#include <QtCore/qobject.h>
#include "kritaglobal_export.h"

#include <QScopedPointer>

class KRITAGLOBAL_EXPORT KisSignalMapper : public QObject
{
    Q_OBJECT
public:
    explicit KisSignalMapper(QObject *parent = nullptr);

    ~KisSignalMapper();

    void setMapping(QObject *sender, int id);

    void setMapping(QObject *sender, const QString &text);

    void setMapping(QObject *sender, QWidget *widget);

    void setMapping(QObject *sender, QObject *object);

    void removeMappings(QObject *sender);

    QObject *mapping(int id) const;

    QObject *mapping(const QString &text) const;

    QObject *mapping(QWidget *widget) const;

    QObject *mapping(QObject *object) const;

Q_SIGNALS:
    void mapped(int);

    void mapped(const QString &);

    void mapped(QWidget *);

    void mapped(QObject *);

public Q_SLOTS:
    void map();

    void map(QObject *sender);

private:

    class Private;
    QScopedPointer<Private> d;

    Q_DISABLE_COPY(KisSignalMapper)
    Q_PRIVATE_SLOT(d, void _q_senderDestroyed())
};

#endif
