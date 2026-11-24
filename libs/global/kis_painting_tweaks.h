/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINTING_TWEAKS_H
#define __KIS_PAINTING_TWEAKS_H

#include "kritaglobal_export.h"

#include <QPen>
#include <QBrush>

#include <QVector3D>
#include <QVector2D>

class QPainter;
class QRegion;
class QRect;
class QPen;

namespace KisPaintingTweaks {

    KRITAGLOBAL_EXPORT QRegion safeClipRegion(const QPainter &painter);

    KRITAGLOBAL_EXPORT QRect safeClipBoundingRect(const QPainter &painter);

    KRITAGLOBAL_EXPORT void initAntsPen(QPen *antsPen, QPen *outlinePen,
                                        int antLength = 4, int antSpace = 4,
                                        QColor black = QColor(Qt::black), QColor white = QColor(Qt::white));

    class KRITAGLOBAL_EXPORT PenBrushSaver
    {
    public:
        struct allow_noop_t { explicit allow_noop_t() = default; };
        static constexpr allow_noop_t	allow_noop { };

        PenBrushSaver(QPainter *painter);

        PenBrushSaver(QPainter *painter, const QPen &pen, const QBrush &brush);

        PenBrushSaver(QPainter *painter, const QPair<QPen, QBrush> &pair);

        PenBrushSaver(QPainter *painter, const QPair<QPen, QBrush> &pair, allow_noop_t);

        ~PenBrushSaver();

    private:
        PenBrushSaver(const PenBrushSaver &rhs) = delete;
        QPainter *m_painter;
        QPen m_pen;
        QBrush m_brush;
    };

    QColor KRITAGLOBAL_EXPORT blendColors(const QColor &c1, const QColor &c2, qreal r1);

    qreal KRITAGLOBAL_EXPORT luminosityCoarse(const QColor &c, bool sRGBtrc = true);

    qreal KRITAGLOBAL_EXPORT colorDifference(const QColor &c1, const QColor &c2);

    void KRITAGLOBAL_EXPORT dragColor(QColor *color, const QColor &baseColor, qreal threshold);

    inline void rectToVertices(QVector3D* vertices, const QRectF &rc)
    {
        vertices[0] = QVector3D(rc.left(),  rc.bottom(), 0.f);
        vertices[1] = QVector3D(rc.left(),  rc.top(),    0.f);
        vertices[2] = QVector3D(rc.right(), rc.bottom(), 0.f);
        vertices[3] = QVector3D(rc.left(),  rc.top(), 0.f);
        vertices[4] = QVector3D(rc.right(), rc.top(), 0.f);
        vertices[5] = QVector3D(rc.right(), rc.bottom(),    0.f);
    }

    inline void rectToTexCoords(QVector2D* texCoords, const QRectF &rc)
    {
        texCoords[0] = QVector2D(rc.left(), rc.bottom());
        texCoords[1] = QVector2D(rc.left(), rc.top());
        texCoords[2] = QVector2D(rc.right(), rc.bottom());
        texCoords[3] = QVector2D(rc.left(), rc.top());
        texCoords[4] = QVector2D(rc.right(), rc.top());
        texCoords[5] = QVector2D(rc.right(), rc.bottom());
    }
}

#endif
