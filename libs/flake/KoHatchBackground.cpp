/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoHatchBackground.h"

#include <KoShapeSavingContext.h>

#include <KoXmlNS.h>
#include <KoUnit.h>

#include <FlakeDebug.h>

#include <QColor>
#include <QString>
#include <QPainter>
#include <QPainterPath>

class KoHatchBackground::Private : public QSharedData
{
public:
    Private()
        : QSharedData()
        , angle(0.0)
        , distance(1.0)
        , style(KoHatchBackground::Single)
    {}

    QColor lineColor;
    int angle;
    qreal distance;
    KoHatchBackground::HatchStyle style;
    QString name;
};

KoHatchBackground::KoHatchBackground()
    : KoColorBackground()
    , d(new Private)
{
}

KoHatchBackground::~KoHatchBackground()
{
}

void KoHatchBackground::paint(QPainter &painter, const QPainterPath &fillPath) const
{
    if (color().isValid()) {
        KoColorBackground::paint(painter, fillPath);
    }

    const QRectF targetRect = fillPath.boundingRect();
    painter.save();
    painter.setClipPath(fillPath);
    QPen pen(d->lineColor);
    pen.setWidthF(0.5);
    painter.setPen(pen);
    QVector<QLineF> lines;

    const int angleOffset[] = {-90, 0, -45 };
    int loops = (d->style == Single) ? 1 : (d->style == Double) ? 2 : 3;

    for (int i = 0; i < loops; ++i) {
        int angle = d->angle - angleOffset[i];
        qreal cosAngle = ::cos(angle/180.0*M_PI);
        if (qAbs(cosAngle) > 0.00001) {
            qreal xDiff = tan(angle/180.0*M_PI) * targetRect.height();
            qreal xOffset = qAbs(d->distance / cosAngle);

            qreal xStart = 0;
            while (-xDiff < xStart) {
                xStart -= xOffset;
            }

            qreal xEndOffset = 0;
            if (xDiff < 0) {
                while (xDiff < -xEndOffset) {
                    xEndOffset += xOffset;
                }
            }
            lines.reserve(lines.size() + int((targetRect.width() + xEndOffset - xStart) / xOffset) + 1);
            for (qreal x = xStart; x < targetRect.width() + xEndOffset; x += xOffset) {
                lines.append(QLineF(x, 0, x + xDiff, targetRect.height()));
            }
        }
        else {
            lines.reserve(lines.size() + int(targetRect.height()/d->distance) + 1);
            for (qreal y = 0; y < targetRect.height(); y += d->distance) {
                lines.append(QLineF(0, y, targetRect.width(), y));
            }
        }
    }

    painter.drawLines(lines);
    painter.restore();
}
