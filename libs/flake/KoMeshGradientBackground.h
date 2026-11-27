/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KOMESHGRADIENTBACKGROUND_H
#define KOMESHGRADIENTBACKGROUND_H

#include "KoShapeBackground.h"
#include <QSharedDataPointer>
#include "SvgMeshGradient.h"

class KRITAFLAKE_EXPORT KoMeshGradientBackground : public KoShapeBackground
{
public:
    KoMeshGradientBackground(const SvgMeshGradient *gradient, const QTransform &matrix = QTransform());
    ~KoMeshGradientBackground();

    KoMeshGradientBackground(const KoMeshGradientBackground &);
    KoMeshGradientBackground& operator=(const KoMeshGradientBackground &);

    void paint(QPainter &painter, const QPainterPath &fillPath) const override;

    bool compareTo(const KoShapeBackground *other) const override;

    SvgMeshGradient* gradient();
    QTransform transform();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
