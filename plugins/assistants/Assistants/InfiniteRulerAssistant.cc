/*
 * SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2010 Geoffry Song <goffrie@gmail.com>
 * SPDX-FileCopyrightText: 2014 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 * SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 * SPDX-FileCopyrightText: 2022 Julian Schmidt <julisch1107@web.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InfiniteRulerAssistant.h"

#include "kis_debug.h"
#include <klocalizedstring.h>

#include <QPainter>
#include <QPainterPath>
#include <QTransform>

#include <kis_canvas2.h>
#include <kis_coordinates_converter.h>
#include <kis_algebra_2d.h>

#include <math.h>

InfiniteRulerAssistant::InfiniteRulerAssistant()
    : RulerAssistant("infinite ruler", i18n("Infinite Ruler assistant"))
{
}

InfiniteRulerAssistant::InfiniteRulerAssistant(const InfiniteRulerAssistant &rhs, QMap<KisPaintingAssistantHandleSP, KisPaintingAssistantHandleSP> &handleMap)
    : RulerAssistant(rhs, handleMap)
{
}

KisPaintingAssistantSP InfiniteRulerAssistant::clone(QMap<KisPaintingAssistantHandleSP, KisPaintingAssistantHandleSP> &handleMap) const
{
    return KisPaintingAssistantSP(new InfiniteRulerAssistant(*this, handleMap));
}

QPointF InfiniteRulerAssistant::project(const QPointF& pt, const QPointF& strokeBegin, const bool checkForInitialMovement, qreal moveThresholdPt)
{
    Q_ASSERT(isAssistantComplete());
    qreal dx = pt.x() - strokeBegin.x();
    qreal dy = pt.y() - strokeBegin.y();
    if (checkForInitialMovement && KisAlgebra2D::norm(QPointF(dx, dy)) < moveThresholdPt) {
        return strokeBegin;
    }

    QLineF snapLine = QLineF(*handles()[0], *handles()[1]);

        dx = snapLine.dx();
        dy = snapLine.dy();
    const qreal
        dx2 = dx * dx,
        dy2 = dy * dy,
        invsqrlen = 1.0 / (dx2 + dy2);
    QPointF r(dx2 * pt.x() + dy2 * snapLine.x1() + dx * dy * (pt.y() - snapLine.y1()),
              dx2 * snapLine.y1() + dy2 * pt.y() + dx * dy * (pt.x() - snapLine.x1()));
    r *= invsqrlen;
    return r;
}

QPointF InfiniteRulerAssistant::adjustPosition(const QPointF& pt, const QPointF& strokeBegin, const bool , qreal moveThresholdPt)
{
    return project(pt, strokeBegin, true, moveThresholdPt);
}

void InfiniteRulerAssistant::adjustLine(QPointF &point, QPointF &strokeBegin)
{

    point = project(point, strokeBegin, false, 0.0);
    strokeBegin = project(strokeBegin, strokeBegin, false, 0.0);
}

void InfiniteRulerAssistant::drawSubdivisions(QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *displayRenderInterface) {
    if (subdivisions() == 0) {
        return;
    }

    QTransform document2widget = converter->documentToWidgetTransform();

    QPointF p1 = document2widget.map(*handles()[0]);
    QPointF p2 = document2widget.map(*handles()[1]);

    const qreal scale = 16.0 / 2;
    const qreal minorScale = scale / 2;
    const QRectF clipping = QRectF(gc.viewport()).adjusted(-scale, -scale, scale, scale);
    const qreal threshold = 3.0;

    QPointF delta = p2 - p1;
    qreal length = sqrt(KisPaintingAssistant::norm2(delta));
    qreal stepsize = length / subdivisions();

    if (stepsize >= threshold) {
        QPointF normal = QPointF(delta.y(), -delta.x());
        normal /= length;

        ClippingResult res = clipLineParametric(QLineF(p1, p2), clipping);
        if (!res.intersects) {
            return;
        }
        int istart = (int) floor(res.tmin * subdivisions());
        int iend =   (int)  ceil(res.tmax * subdivisions());

        QPainterPath path;
        QPainterPath highlight;

        for (int ii = istart; ii < iend; ++ii) {
            QPointF pos = p1 + delta * ((qreal)ii / subdivisions());
            if (0 <= ii && ii < subdivisions()) {
                continue;
            }
            if (ii != subdivisions()) {
                if (ii % subdivisions() == 0) {
                    highlight.moveTo(pos - normal * scale);
                    highlight.lineTo(pos + normal * scale);
                } else {
                    path.moveTo(pos - normal * scale);
                    path.lineTo(pos + normal * scale);
                }
            }

            if (stepsize / minorSubdivisions() < threshold)
                continue;
            for (int jj = 1; jj < minorSubdivisions(); ++jj) {
                QPointF mpos = pos + delta * ((qreal)jj / (subdivisions() * minorSubdivisions()));

                path.moveTo(mpos - normal * minorScale);
                path.lineTo(mpos + normal * minorScale);
            }
        }

        drawPath(gc, highlight, displayRenderInterface);
        drawPreview(gc, path, displayRenderInterface);
    }
}

void InfiniteRulerAssistant::drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter* converter, const KoColorDisplayRendererInterface *displayRendererInterface, bool cached, KisCanvas2* canvas, bool assistantVisible, bool previewVisible)
{
    gc.save();
    gc.resetTransform();

    if (isAssistantComplete() && isSnappingActive() && previewVisible) {

        QTransform initialTransform = converter->documentToWidgetTransform();
        QLineF snapLine = QLineF(initialTransform.map(*handles()[0]), initialTransform.map(*handles()[1]));
        QRect viewport = gc.viewport();
        KisAlgebra2D::intersectLineRect(snapLine, viewport, true);

        QPainterPath path;
        path.moveTo(snapLine.p1());
        path.lineTo(snapLine.p2());
        drawPreview(gc, path, displayRendererInterface);

        if (subdivisions() > 0) {
            drawSubdivisions(gc, converter, displayRendererInterface);
        }
    }

    gc.restore();

    RulerAssistant::drawAssistant(gc, updateRect, converter, displayRendererInterface, cached, canvas, assistantVisible, previewVisible);
}

InfiniteRulerAssistant::ClippingResult InfiniteRulerAssistant::clipLineParametric(QLineF line, QRectF rect, bool extendFirst, bool extendSecond) {
    double dx = line.x2() - line.x1();
    double dy = line.y2() - line.y1();

    double q1 = line.x1() - rect.x();
    double q2 = rect.x() + rect.width() - line.x1();
    double q3 = line.y1() - rect.y();
    double q4 = rect.y() + rect.height() - line.y1();

    QVector<double> p = QVector<double>({-dx, dx, -dy, dy});
    QVector<double> q = QVector<double>({q1, q2, q3, q4});

    double tmin = extendFirst ? -std::numeric_limits<double>::infinity() : 0.0;
    double tmax = extendSecond ? +std::numeric_limits<double>::infinity() : 1.0;

    for (int i = 0; i < p.length(); i++) {

        if (p[i] == 0 && q[i] < 0) {
            return ClippingResult{false, 0, 0};

        } else if (p[i] < 0) {
            double t = q[i] / p[i];
            if (t > tmin) {
                tmin = t;
            }

        } else if (p[i] > 0) {
            double t = q[i] / p[i];
            if (t < tmax) {
                tmax = t;
            }
        }
    }

    return ClippingResult{tmin < tmax, tmin, tmax};
}

QPointF InfiniteRulerAssistant::getDefaultEditorPosition() const
{
    return (*handles()[0]);
}

bool InfiniteRulerAssistant::isAssistantComplete() const
{
    return handles().size() >= 2;
}

InfiniteRulerAssistantFactory::InfiniteRulerAssistantFactory() = default;

InfiniteRulerAssistantFactory::~InfiniteRulerAssistantFactory() = default;

QString InfiniteRulerAssistantFactory::id() const
{
    return "infinite ruler";
}

QString InfiniteRulerAssistantFactory::name() const
{
    return i18n("Infinite Ruler");
}

KisPaintingAssistant* InfiniteRulerAssistantFactory::createPaintingAssistant() const
{
    return new InfiniteRulerAssistant;
}
