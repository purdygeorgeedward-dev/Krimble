/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_INFOOBJECT_H
#define LIBKIS_INFOOBJECT_H

#include <QObject>
#include <kis_properties_configuration.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT InfoObject : public QObject
{
    Q_OBJECT

public:
    InfoObject(KisPropertiesConfigurationSP configuration);

    explicit InfoObject(QObject *parent = 0);
    ~InfoObject() override;

    bool operator==(const InfoObject &other) const;
    bool operator!=(const InfoObject &other) const;
    QMap<QString, QVariant> properties() const;

    void setProperties(QMap<QString, QVariant> propertyMap);

public Q_SLOTS:
    void setProperty(const QString &key, QVariant value);

    QVariant property(const QString &key);

private:

    friend class Filter;
    friend class Document;
    friend class Node;
    KisPropertiesConfigurationSP configuration() const;

    struct Private;
    Private *d;

};

#endif
