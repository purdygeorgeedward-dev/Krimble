/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOABSTRACTCANVASRESOURCEINTERFACE_H
#define KOABSTRACTCANVASRESOURCEINTERFACE_H

#include <QObject>
#include <QSharedPointer>
#include "kritaflake_export.h"

class QVariant;

class KRITAFLAKE_EXPORT KoAbstractCanvasResourceInterface : public QObject
{
    Q_OBJECT
public:
    KoAbstractCanvasResourceInterface(int key, const QString debugTag = QString());

    virtual QVariant value() const = 0;

    virtual void setValue(const QVariant value) = 0;

    int key() const;

Q_SIGNALS:
    void sigResourceChangedExternal(int key, const QVariant &value);

private:
    int m_key = -1;
    QString m_debugTag;
};

typedef QSharedPointer<KoAbstractCanvasResourceInterface> KoAbstractCanvasResourceInterfaceSP;

#endif
