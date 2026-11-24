/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONCALLIGRAPHYTOOL_H
#define KARBONCALLIGRAPHYTOOL_H

#include <KoToolBase.h>
#include <KoPathShape.h>
#include <QPainterPath>
#include <QPointer>

#include "KarbonCalligraphyOptionWidget.h"

class KoPathShape;
class KarbonCalligraphicShape;

class KarbonCalligraphyTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KarbonCalligraphyTool(KoCanvasBase *canvas);
    ~KarbonCalligraphyTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    QList<QPointer<QWidget>> createOptionWidgets() override;

    KisPopupWidgetInterface *popupWidget() override;

    void activate(const QSet<KoShape *> &shapes) override;
    void deactivate() override;

Q_SIGNALS:
    void pathSelectedChanged(bool selection);

private Q_SLOTS:
    void setUsePath(bool usePath);
    void setUsePressure(bool usePressure);
    void setUseAngle(bool useAngle);
    void setStrokeWidth(double width);
    void setThinning(double thinning);
    void setAngle(int angle);
    void setFixation(double fixation);
    void setCaps(double caps);
    void setMass(double mass);
    void setDrag(double drag);

    void updateSelectedPath();

private:
    void addPoint(KoPointerEvent *event);
    void setAngle(KoPointerEvent *event);
    QPointF calculateNewPoint(const QPointF &mousePos, QPointF *speed);
    qreal calculateWidth(qreal pressure);
    qreal calculateAngle(const QPointF &oldSpeed, const QPointF &newSpeed);

    QPointF m_lastPoint;
    KarbonCalligraphicShape *m_shape;

    bool m_deviceSupportsTilt;

    bool m_usePath;
    bool m_usePressure;
    bool m_useAngle;
    qreal m_strokeWidth;
    qreal m_lastWidth;
    qreal m_customAngle;
    qreal m_angle;
    qreal m_fixation;
    qreal m_thinning;
    qreal m_caps;
    qreal m_mass;
    qreal m_drag;

    KoPathShape *m_selectedPath;
    QPainterPath m_selectedPathOutline;
    qreal m_followPathPosition;
    bool m_endOfPath;
    QPointF m_lastMousePos;

    bool m_isDrawing;
    int m_pointCount;

    QPointF m_speed;

    KarbonCalligraphyOptionWidget *m_widget {0};
};

#endif
