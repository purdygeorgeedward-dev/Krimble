/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSVGTEXTPROPERTIESINTERFACE_H
#define KOSVGTEXTPROPERTIESINTERFACE_H

#include <QObject>
#include <KoSvgTextProperties.h>
#include <kritaflake_export.h>

class KRITAFLAKE_EXPORT KoSvgTextPropertiesInterface : public QObject
{
    Q_OBJECT
public:
    explicit KoSvgTextPropertiesInterface(QObject *parent = nullptr): QObject(parent){}

    virtual QList<KoSvgTextProperties> getSelectedProperties() = 0;

    virtual QList<KoSvgTextProperties> getCharacterProperties()  = 0;

    virtual KoSvgTextProperties getInheritedProperties() = 0;

    virtual void setPropertiesOnSelected(KoSvgTextProperties properties, QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>()) = 0;

    virtual void setCharacterPropertiesOnSelected(KoSvgTextProperties properties, QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>()) = 0;

    virtual bool spanSelection() = 0;

    virtual bool characterPropertiesEnabled() = 0;
Q_SIGNALS:
    void textSelectionChanged();
    void textCharacterSelectionChanged();
};

#endif
