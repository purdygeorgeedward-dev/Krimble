/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SWATCH_H
#define SWATCH_H

#include "ManagedColor.h"

#include "kritalibkis_export.h"
#include "libkis.h"

class KisSwatch;

class KRITALIBKIS_EXPORT Swatch : public QObject
{
    Q_OBJECT

private:
    friend class Palette;
    friend class PaletteView;
    Swatch(const KisSwatch &kisSwatch, QObject *parent = 0);
public:
    explicit Swatch(QObject *parent = 0);
    virtual ~Swatch();
    Swatch(const Swatch &rhs, QObject *parent = 0);
    Swatch &operator=(const Swatch &rhs);

    bool operator==(const Swatch &other) const;
    bool operator!=(const Swatch &other) const;

public Q_SLOTS:
    QString name() const;
    void setName(const QString &name);

    QString id() const;
    void setId(const QString &id);

    ManagedColor *color() const;
    void setColor(ManagedColor *color);

    bool spotColor() const;
    void setSpotColor(bool spotColor);

    bool isValid() const;

private:
    friend class Palette;
    KisSwatch kisSwatch() const;

    struct Private;
    Private *const d;
};

#endif
