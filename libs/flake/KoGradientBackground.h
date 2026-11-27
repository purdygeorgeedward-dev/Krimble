/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOGRADIENTBACKGROUND_H
#define KOGRADIENTBACKGROUND_H

#include "KoShapeBackground.h"
#include "kritaflake_export.h"

#include <QTransform>
#include <QSharedDataPointer>

class QGradient;

class KRITAFLAKE_EXPORT KoGradientBackground : public KoShapeBackground
{
public:
    explicit KoGradientBackground(QGradient *gradient, const QTransform &matrix = QTransform());

    explicit KoGradientBackground(const QGradient &gradient, const QTransform &matrix = QTransform());

    ~KoGradientBackground() override;

    KoGradientBackground(const KoGradientBackground &);
    KoGradientBackground &operator=(const KoGradientBackground &);

    bool compareTo(const KoShapeBackground *other) const override;

    void setTransform(const QTransform &matrix);

    QTransform transform() const;

    void setGradient(const QGradient &gradient);

    const QGradient *gradient() const;

    void paint(QPainter &painter, const QPainterPath &fillPath) const override;
private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
