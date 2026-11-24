/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef OVERVIEWWIDGET_H
#define OVERVIEWWIDGET_H
#include <QObject>
#include <QWidget>
#include <QPixmap>

#include "KisWidgetWithIdleTask.h"

#include <kis_canvas2.h>

class KisSignalCompressor;
class KoCanvasBase;

class OverviewWidget : public KisWidgetWithIdleTask<QWidget>
{
    Q_OBJECT

public:
    OverviewWidget(QWidget * parent = 0);

    ~OverviewWidget() override;

    void setCanvas(KisCanvas2 *canvas) override;

    inline bool isDragging() const
    {
        return m_dragging;
    }

public Q_SLOTS:
    void startUpdateCanvasProjection();
    void updateThumbnail(QImage pixmap);
    void slotThemeChanged();

Q_SIGNALS:
    void signalDraggingStarted();
    void signalDraggingFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void recalculatePreviewDimensions();
    KisIdleTasksManager::TaskGuard registerIdleTask(KisCanvas2 *canvas) override;
    void clearCachedState() override;
    bool isPixelArt();

    QPointF calculatePreviewOrigin(QSize previewSize);
    QTransform canvasToPreviewTransform();
    QTransform previewToCanvasTransform();
    QPolygonF previewPolygon();

    qreal m_previewScale {1.0};
    QPixmap m_oldPixmap;
    QPixmap m_pixmap;

    QPointF m_previewOrigin;
    QSize m_previewSize {QSize(100, 100)};

    bool m_dragging {false};
    QPointF m_lastPos {QPointF(0, 0)};

    QColor m_outlineColor;
};

#endif
