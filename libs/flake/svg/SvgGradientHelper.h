/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGGRADIENTHELPER_H
#define SVGGRADIENTHELPER_H

#include <KoFlakeCoordinateSystem.h>
#include <QTransform>
#include <QGradient>
#include <SvgMeshGradient.h>

class SvgGradientHelper
{
public:
    SvgGradientHelper();
    ~SvgGradientHelper();
    SvgGradientHelper(const SvgGradientHelper &other);

    void setGradientUnits(KoFlake::CoordinateSystem units);
    KoFlake::CoordinateSystem gradientUnits() const;

    void setGradient(QGradient * g);
    QGradient * gradient() const;

    void setMeshGradient(SvgMeshGradient* g);
    QScopedPointer<SvgMeshGradient>& meshgradient();

    bool isMeshGradient() const;

    QTransform transform() const;
    void setTransform(const QTransform &transform);

    SvgGradientHelper & operator = (const SvgGradientHelper & rhs);

    QGradient * adjustedGradient(const QRectF &bound) const;

    static QGradient *convertGradient(const QGradient * originalGradient, const QTransform &userToRelativeTransform, const QRectF &size);

    QGradient::Spread spreadMode() const;
    void setSpreadMode(const QGradient::Spread &spreadMode);

private:

    QScopedPointer<QGradient> m_gradient;
    QScopedPointer<SvgMeshGradient> m_meshgradient;
    KoFlake::CoordinateSystem m_gradientUnits;
    QTransform m_gradientTransform;
};

#endif
