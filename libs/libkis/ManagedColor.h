/*
 * SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MANAGEDCOLOR_H
#define MANAGEDCOLOR_H

#include <QObject>
#include <QVector>
#include <QScopedPointer>

#include "kritalibkis_export.h"
#include "libkis.h"

class KoColor;

class KRITALIBKIS_EXPORT ManagedColor : public QObject
{
    Q_OBJECT
public:
    explicit ManagedColor(QObject *parent = 0);
    ManagedColor(const QString &colorModel, const QString &colorDepth, const QString &colorProfile, QObject *parent = 0);
    ManagedColor(KoColor color, QObject *parent = 0);
    ~ManagedColor() override;

    bool operator==(const ManagedColor &other) const;

    QColor colorForCanvas(Canvas *canvas) const;

    static ManagedColor *fromQColor(const QColor &qcolor, Canvas *canvas = 0);

    QString colorDepth() const;

    QString colorModel() const;

    QString colorProfile() const;

    bool setColorProfile(const QString &colorProfile);

    bool setColorSpace(const QString &colorModel, const QString &colorDepth, const QString &colorProfile);

    QVector<float> components() const;

    QVector<float> componentsOrdered() const;

    void setComponents(const QVector<float> &values);

    QString toXML() const;

    void fromXML(const QString &xml);

    QString toQString();

private:

    friend class View;
    friend class PaletteView;
    friend class Swatch;
    friend class ColorizeMask;

    KoColor color() const;

    struct Private;
    const QScopedPointer<Private> d;

};

#endif
