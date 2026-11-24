/*
   SPDX-FileCopyrightText: 2006-2007 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_PROPERTIES_H
#define _KO_PROPERTIES_H

#include <QString>
#include <QMap>
#include <QVariant>
#include "kritawidgetutils_export.h"

class QDomElement;

class KRITAWIDGETUTILS_EXPORT KoProperties
{
public:

    KoProperties();

    KoProperties(const KoProperties &other);

    ~KoProperties();

public:

    void load(const QDomElement &root);

    bool load(const QString &string);

    QMapIterator<QString, QVariant> propertyIterator() const;

    bool isEmpty() const;

    QString store(const QString &root) const;

    void save(QDomElement &root) const;

    void setProperty(const QString &name, const QVariant &value);

    bool property(const QString &name, QVariant &value) const;

    QVariant property(const QString &name) const;

    int intProperty(const QString &name, int defaultValue = 0) const;

    qreal doubleProperty(const QString &name, qreal defaultValue = 0.0) const;

    bool boolProperty(const QString &name, bool defaultValue = false) const;

    QString stringProperty(const QString &name, const QString &defaultValue = QString()) const;

    bool contains(const QString &key) const;

    QVariant value(const QString &key) const;

    bool operator==(const KoProperties &other) const;

private:

    class Private;
    Private * const d;
};

#endif
