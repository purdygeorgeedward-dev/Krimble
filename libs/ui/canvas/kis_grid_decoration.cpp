/*
 * This file is part of Krita
 *
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_grid_decoration.h"

#include <QPainter>
#include <QPen>
#include <QtMath>
#include <klocalizedstring.h>

#include <KoUnit.h>
#include <KoColorDisplayRendererInterface.h>
#include <kis_canvas2.h>

#include "kis_grid_config.h"
#include "kis_coordinates_converter.h"

struct KisGridDecoration::Private
{
    KisGridConfig config;
};

KisGridDecoration::KisGridDecoration(KisView* parent)
    : KisCanvasDecoration("grid", parent),
      m_d(new Private)
{
    setPriority(0);
}

KisGridDecoration::~KisGridDecoration()
{

}

void KisGridDecoration::setGridConfig(const KisGridConfig &config)
{
    m_d->config = config;
}

void KisGridDecoration::drawDecoration(QPainter& gc, const QRectF& updateArea, const KisCoordinatesConverter* converter, KisCanvas2* canvas)
{
    if (!m_d->config.showGrid()) return;

    Q_UNUSED(canvas);

    QTransform transform = converter->imageToWidgetTransform();

    const qreal scale = KoUnit::approxTransformScale(transform);
    const int minWidgetSize = 3;
    const int effectiveSize = qMin(m_d->config.spacing().x(), m_d->config.spacing().y());

    int scaleCoeff = 1;
    quint32 subdivision = m_d->config.subdivision();

    while (qFloor(scale * scaleCoeff * effectiveSize) <= minWidgetSize) {
        if (subdivision > 1) {
            scaleCoeff = subdivision;
            subdivision = 1;
        } else {
            scaleCoeff *= 2;
        }

        if (scaleCoeff > 32768) {
            qWarning() << "WARNING: Grid Scale Coeff is too high! That is surely a bug!";
            return;
        }
    }

    QPen mainPen = m_d->config.penMain();
    QPen subdivisionPen = m_d->config.penSubdivision();

    mainPen.setColor(canvas->displayRendererInterface()->convertColorToDisplayColorSpace(KoColor(mainPen.color(), KoColorSpaceRegistry::instance()->rgb8())));
    subdivisionPen.setColor(canvas->displayRendererInterface()->convertColorToDisplayColorSpace(KoColor(subdivisionPen.color(), KoColorSpaceRegistry::instance()->rgb8())));

    gc.save();
    gc.setTransform(transform);

    const QRect imageRectInImagePixels = converter->imageRectInImagePixels();
    const QRectF updateRectInImagePixels =
        converter->documentToImage(updateArea) &
        imageRectInImagePixels;

    KisGridConfig::GridType gridType = m_d->config.gridType();

    if (gridType == KisGridConfig::GRID_RECTANGULAR) {
        gc.setRenderHints(QPainter::Antialiasing, false);
        qreal x1, y1, x2, y2;
        updateRectInImagePixels.getCoords(&x1, &y1, &x2, &y2);

        x2++;
        y2++;

        if (m_d->config.xSpacingActive()) {
            int offset = 0;
            if (m_d->config.offsetActive()) {
                offset = m_d->config.offset().x();
            }
            const int step = scaleCoeff * m_d->config.spacing().x();
            const int lineIndexFirst = qCeil((x1 - offset) / step);
            const int lineIndexLast = qFloor((x2 - offset) / step);

            if (mainPen.style() != Qt::SolidLine) {
                mainPen.setDashOffset(y1 * scale);
            }
            if (subdivisionPen.style() != Qt::SolidLine) {
                subdivisionPen.setDashOffset(y1 * scale);
            }

            for (int i = lineIndexFirst; i <= lineIndexLast; i++) {
                int w = offset + i * step;

                gc.setPen(i % subdivision == 0 ? mainPen : subdivisionPen);
                gc.drawLine(QPointF(w, y1),QPointF(w, qMin(y2, qreal(imageRectInImagePixels.bottom() + 1))));
            }
        }

        if (m_d->config.ySpacingActive()) {
            int offset = 0;
            if (m_d->config.offsetActive()) {
                offset = m_d->config.offset().y();
            }
            const int step = scaleCoeff * m_d->config.spacing().y();
            const int lineIndexFirst = qCeil((y1 - offset) / step);
            const int lineIndexLast = qFloor((y2 - offset) / step);

            if (mainPen.style() != Qt::SolidLine) {
                mainPen.setDashOffset(x1 * scale);
            }
            if (subdivisionPen.style() != Qt::SolidLine) {
                subdivisionPen.setDashOffset(x1 * scale);
            }

            for (int i = lineIndexFirst; i <= lineIndexLast; i++) {
                int w = offset + i * step;

                gc.setPen(i % subdivision == 0 ? mainPen : subdivisionPen);
                gc.drawLine(QPointF(x1, w),QPointF(qMin(x2, qreal(imageRectInImagePixels.right() + 1)), w));
            }
        }
    } else if (gridType == KisGridConfig::GRID_ISOMETRIC_LEGACY)  {
        qreal x1, y1, x2, y2;

        QRectF trueImageRect = converter->imageRectInImagePixels();
        trueImageRect.getCoords(&x1, &y1, &x2, &y2);

        x2++;
        y2++;

        int offset = 0;
        int offsetY = 0;
        if (m_d->config.offsetActive()) {
            offset = m_d->config.offset().x();
            offsetY = m_d->config.offset().y();
        }
        const int cellSpacing = m_d->config.cellSpacing();

        gc.setClipping(true);
        gc.setClipRect(updateRectInImagePixels, Qt::IntersectClip);

        if (m_d->config.angleLeftActive()) {
            const qreal gridXAngle = m_d->config.angleLeft();
            const qreal bottomRightOfImageY = y2;
            qreal finalY = 0.0;

            qreal correctedAngleSpacing = cellSpacing;
            if (gridXAngle > 0.0) {
                correctedAngleSpacing = cellSpacing / qCos(qDegreesToRadians(gridXAngle));
                gc.setRenderHints(QPainter::Antialiasing, true);
            } else {
                gc.setRenderHints(QPainter::Antialiasing, false);
            }

            qreal counter = qFloor((-(offset + offsetY)) / correctedAngleSpacing);

            while (finalY < bottomRightOfImageY) {

                const qreal w = (counter * correctedAngleSpacing) + offsetY + offset;
                gc.setPen(mainPen);

                const qreal startingY = w;
                const qreal horizontalDistance = x2;

                const qreal length2 = qTan(qDegreesToRadians(gridXAngle)) * x2;

                finalY = startingY - length2;

                gc.drawLine(QPointF(x1, w), QPointF(horizontalDistance, finalY));

                counter = counter + 1.0;
            }
        }

        if (m_d->config.angleRightActive()) {
            const qreal gridXAngle = m_d->config.angleRight();
            const qreal bottomLeftOfImageY = y2;

            qreal correctedAngleSpacing = cellSpacing;
            if (gridXAngle > 0.0) {
                correctedAngleSpacing = cellSpacing / qCos(qDegreesToRadians(gridXAngle));
                gc.setRenderHints(QPainter::Antialiasing, true);
            } else {
                gc.setRenderHints(QPainter::Antialiasing, false);
            }

            const qreal horizontalDistance = x2;
            const qreal length2 = qTan(qDegreesToRadians(gridXAngle)) * horizontalDistance;

            const qreal yLower = 0.0;
            const qreal yHigher = yLower - length2;

            const qreal yLeftFirst = qCeil(yHigher / correctedAngleSpacing) * correctedAngleSpacing;
            const qreal additionalOffset = yLeftFirst - yHigher;
            qreal finalY = 0.0;
            qreal counter = qFloor((-(offsetY - offset)) / correctedAngleSpacing);

            while (finalY < bottomLeftOfImageY) {

                const qreal w = (counter * correctedAngleSpacing) + offsetY - offset + additionalOffset;
                gc.setPen(mainPen);

                const qreal startingY = w;

                finalY = startingY - length2;

                gc.drawLine(QPointF(x2, w), QPointF(0.0, finalY));

                counter = counter + 1.0;
            }
        }
    } else if (gridType == KisGridConfig::GRID_ISOMETRIC)  {
        qreal x1, y1, xRight, yBottom;

        QRectF trueImageRect = converter->imageRectInImagePixels();
        trueImageRect.getCoords(&x1, &y1, &xRight, &yBottom);

        xRight++;
        yBottom++;

        const KisGridConfig::TrigoCache trigoCache = m_d->config.trigoCache();

        int offsetX = 0;
        int offsetY = 0;
        if (m_d->config.offsetActive()) {
            offsetX = trigoCache.correctedAngleRightOffsetX;
            offsetY = m_d->config.offset().y();
        }

        gc.setClipping(true);
        gc.setClipRect(updateRectInImagePixels, Qt::IntersectClip);

        if (trigoCache.correctedAngleLeftCellSize > 0 && m_d->config.angleLeftActive()) {
            if (m_d->config.angleLeft() > 0.0) {
                gc.setRenderHints(QPainter::Antialiasing, true);
            } else {
                gc.setRenderHints(QPainter::Antialiasing, false);
            }

            const qreal length2 = trigoCache.tanAngleLeft * xRight;

            const qreal yLower = 0.0;
            const qreal yHigher = yLower - length2;

            const qreal yLeftFirst = qCeil(yHigher / trigoCache.correctedAngleLeftCellSize) * trigoCache.correctedAngleLeftCellSize;
            const qreal additionalOffset = yLeftFirst - yHigher;

            qreal finalY = 0.0;

            int subdivisionIndex = qCeil(yHigher / trigoCache.correctedAngleLeftCellSize);

            qreal startingY = yLeftFirst + offsetY + offsetX + additionalOffset;

            while (finalY < yBottom) {
                finalY = startingY - length2;

                gc.setPen(qAbs(subdivisionIndex) % subdivision == 0 ? mainPen : subdivisionPen);
                gc.drawLine(QPointF(0.0, startingY), QPointF(xRight, finalY));

                subdivisionIndex++;
                startingY += trigoCache.correctedAngleLeftCellSize;
            }
        }

        if (trigoCache.correctedAngleRightCellSize > 0 && m_d->config.angleRightActive()) {
            if (m_d->config.angleRight() > 0.0) {
                gc.setRenderHints(QPainter::Antialiasing, true);
            } else {
                gc.setRenderHints(QPainter::Antialiasing, false);
            }

            const qreal length2 = trigoCache.tanAngleRight * xRight;

            const qreal yLower = 0.0;
            const qreal yHigher = yLower - length2;

            const qreal yLeftFirst = qCeil(yHigher / trigoCache.correctedAngleRightCellSize) * trigoCache.correctedAngleRightCellSize;
            const qreal additionalOffset = yLeftFirst - yHigher;

            qreal finalY = 0.0;

            int subdivisionIndex = qCeil(yHigher / trigoCache.correctedAngleRightCellSize);

            qreal startingY = yLeftFirst + offsetY - offsetX + additionalOffset;

            while (finalY < yBottom) {
                finalY = startingY - length2;

                gc.setPen(qAbs(subdivisionIndex) % subdivision == 0 ? mainPen : subdivisionPen);
                gc.drawLine(QPointF(xRight, startingY), QPointF(0.0, finalY));

                subdivisionIndex++;
                startingY += trigoCache.correctedAngleRightCellSize;
            }
        }

        if (trigoCache.verticalSpace > 0) {
            QPen verticalPen = m_d->config.penVertical();
            if (verticalPen.style() != Qt::SolidLine) {
                verticalPen.setDashOffset(updateRectInImagePixels.top() * scale);
            }
            gc.setPen(verticalPen);
            gc.setRenderHints(QPainter::Antialiasing, false);

            int offset = 0;
            if (m_d->config.offsetActive()) {
                offset = m_d->config.offset().x();
            }

            qreal pX = (xRight - x1)/(2*trigoCache.verticalSpace);
            pX = offset + (pX - qFloor(pX)) * trigoCache.verticalSpace;
            pX = pX - trigoCache.verticalSpace * qFloor(pX/trigoCache.verticalSpace);

            while(pX <= updateRectInImagePixels.right()) {
                if (pX > updateRectInImagePixels.left()) {
                    gc.drawLine(QPointF(pX, updateRectInImagePixels.top()),QPointF(pX, qMin(yBottom, qreal(updateRectInImagePixels.bottom() + 1))));
                }
                pX += trigoCache.verticalSpace;
            }
        }
    }

    gc.restore();
}
