/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_CURVE_WIDGET_P_H_
#define _KIS_CURVE_WIDGET_P_H_
#include <kis_cubic_curve.h>
#include <QApplication>
#include <QPalette>

enum enumState {
    ST_NORMAL,
    ST_DRAG
};

class Q_DECL_HIDDEN KisCurveWidget::Private
{

    KisCurveWidget *m_curveWidget {nullptr};

public:
    Private(KisCurveWidget *parent);

    int m_grab_point_index {-1};
    double m_grabOffsetX {0.0};
    double m_grabOffsetY {0.0};
    double m_grabOriginalX {0.0};
    double m_grabOriginalY {0.0};
    KisCubicCurvePoint m_draggedAwayPoint;
    int m_draggedAwayPointIndex {0};

    bool m_readOnlyMode {false};

    bool    m_splineDirty {false};
    KisCubicCurve m_curve;
    KisCurveWidget::PointConstrain m_globalPointConstrain {KisCurveWidget::PointConstrain_None};

    QPixmap m_pix;
    bool m_pixmapDirty {true};
    QPixmap *m_pixmapCache {nullptr};

    int m_handleSize {12};

    enumState m_state {enumState::ST_NORMAL};

    inline void setState(enumState st);
    inline enumState state() const;

    KisThreadSafeSignalCompressor m_modifiedSignalsCompressor;

    void setCurveModified(bool rewriteSpinBoxesValues = true);
    void setCurveRepaint();

    bool jumpOverExistingPoints(QPointF &pt, int skipIndex);

    void syncIOControls();

    int nearestPointInRange(QPointF pt, int wWidth, int wHeight) const;

    inline
    void drawGrid(QPainter &p, int wWidth, int wHeight);

    void applyGlobalPointConstrain();
};

KisCurveWidget::Private::Private(KisCurveWidget *parent)
    : m_modifiedSignalsCompressor(100, KisSignalCompressor::Mode::FIRST_INACTIVE)
{
    m_curveWidget = parent;
}

bool KisCurveWidget::Private::jumpOverExistingPoints(QPointF &pt, int skipIndex)
{
    Q_FOREACH (const KisCubicCurvePoint &it, m_curve.curvePoints()) {
        if (m_curve.curvePoints().indexOf(it) == skipIndex)
            continue;
        if (fabs(it.x() - pt.x()) < POINT_AREA) {
            pt.rx() = pt.x() >= it.x() ?
                      it.x() + POINT_AREA : it.x() - POINT_AREA;
        }
    }
    return (pt.x() >= 0 && pt.x() <= 1.);
}

int KisCurveWidget::Private::nearestPointInRange(QPointF pt, int wWidth, int wHeight) const
{
    double nearestDistanceSquared = 1000;
    int nearestIndex = -1;
    int i = 0;

    Q_FOREACH (const KisCubicCurvePoint &point, m_curve.curvePoints()) {
        double distanceSquared = (pt.x() - point.x()) *
                                 (pt.x() - point.x()) +
                                 (pt.y() - point.y()) *
                                 (pt.y() - point.y());

        if (distanceSquared < nearestDistanceSquared) {
            nearestIndex = i;
            nearestDistanceSquared = distanceSquared;
        }
        ++i;
    }

    if (nearestIndex >= 0) {

        QPointF distanceVector = QPointF((pt.x() - m_curve.curvePoints()[nearestIndex].x()) *(wWidth - 1),
                                        (pt.y() - m_curve.curvePoints()[nearestIndex].y()) *(wHeight - 1));

        if (distanceVector.x() > m_handleSize || distanceVector.y() > m_handleSize) {
            return -1;
        }

        double distanceInPixels = QLineF(distanceVector, QPointF(0, 0)).length();

        if (distanceInPixels <= m_handleSize) {
            return nearestIndex;
        }
    }

    return -1;
}

#define div2_round(x) (((x)+1)>>1)
#define div4_round(x) (((x)+2)>>2)

void KisCurveWidget::Private::drawGrid(QPainter &p, int wWidth, int wHeight)
{

    QPalette appPalette = QApplication::palette();

    p.setPen(QPen(appPalette.color(QPalette::Window), 1, Qt::SolidLine));
    p.drawLine(div4_round(wWidth), 0, div4_round(wWidth), wHeight);
    p.drawLine(div2_round(wWidth), 0, div2_round(wWidth), wHeight);
    p.drawLine(div4_round(3*wWidth), 0, div4_round(3*wWidth), wHeight);

    p.drawLine(0, div4_round(wHeight), wWidth, div4_round(wHeight));
    p.drawLine(0, div2_round(wHeight), wWidth, div2_round(wHeight));
    p.drawLine(0, div4_round(3*wHeight), wWidth, div4_round(3*wHeight));

}

void KisCurveWidget::Private::syncIOControls()
{
    Q_EMIT m_curveWidget->shouldSyncIOControls();
}

void KisCurveWidget::Private::setCurveModified(bool rewriteSpinBoxesValues)
{
    if (rewriteSpinBoxesValues) {
        syncIOControls();
    }
    m_splineDirty = true;
    m_curveWidget->update();
    Q_EMIT m_curveWidget->compressorShouldEmitModified();
}

void KisCurveWidget::Private::setCurveRepaint()
{
    m_curveWidget->update();
}

void KisCurveWidget::Private::setState(enumState st)
{
    m_state = st;
}

enumState KisCurveWidget::Private::state() const
{
    return m_state;
}

void KisCurveWidget::Private::applyGlobalPointConstrain()
{
    if (m_globalPointConstrain == PointConstrain_AlwaysCorner) {
        for (int i = 0; i < m_curve.curvePoints().size(); ++i) {
            m_curve.setPointAsCorner(i, true);
        }
    } else if (m_globalPointConstrain == PointConstrain_AlwaysSmooth) {
        for (int i = 0; i < m_curve.curvePoints().size(); ++i) {
            m_curve.setPointAsCorner(i, false);
        }
    }
}

#endif
