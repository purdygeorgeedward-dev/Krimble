/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007-2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2007-2008 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCANVASCONTROLLERWIDGET_H
#define KOCANVASCONTROLLERWIDGET_H

#include "kritaui_export.h"

#include <optional>
#include <QAbstractScrollArea>
#include <QPointer>
#include "KoCanvasController.h"
#include <KoZoomMode.h>

class KoShape;
class KoCanvasBase;
class KoCanvasSupervisor;
class KisCanvasState;
class KoViewTransformStillPoint;

class KRITAUI_EXPORT KoCanvasControllerWidget : public QAbstractScrollArea, public KoCanvasController
{
    Q_OBJECT
public:

    explicit KoCanvasControllerWidget(KisKActionCollection * actionCollection, KoCanvasSupervisor *observerProvider, QWidget *parent = 0);
    ~KoCanvasControllerWidget() override;

    void scrollContentsBy(int dx, int dy) override;

    virtual void activate();

    void setCanvas(KoCanvasBase *canvas) override;

    KoCanvasBase *canvas() const override;

    virtual void changeCanvasWidget(QWidget *widget);

    void setToolOptionWidgets(const QList<QPointer<QWidget> > &widgets);

    void zoomTo(const QRect &rect) override;

    void setZoom(KoZoomMode::Mode mode, qreal zoom) override;
    void setZoom(KoZoomMode::Mode mode, qreal zoom, qreal resolutionX, qreal resolutionY);

    void setZoom(KoZoomMode::Mode mode, qreal zoom, const KoViewTransformStillPoint &stillPoint);
    void setZoom(KoZoomMode::Mode mode, qreal zoom, qreal resolutionX, qreal resolutionY, const std::optional<KoViewTransformStillPoint> &docStillPoint);

    void pan(const QPoint &distance) override;

    virtual void panUp() override;
    virtual void panDown() override;
    virtual void panLeft() override;
    virtual void panRight() override;

    QPoint scrollBarValue() const override;

    void setScrollBarValue(const QPoint &value) override;

    virtual KisCanvasState canvasState() const = 0;

    class Private;
    KoCanvasControllerWidget::Private *priv();

protected:
    friend class KisZoomAndPanTest;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *resizeEvent) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    bool focusNextPrevChild(bool next) override;
    bool viewportEvent(QEvent *event) override;

    virtual void updateCanvasOffsetInternal(const QPointF &newOffset) = 0;
    virtual void updateCanvasWidgetSizeInternal(const QSize &newSize, qreal devicePixelRatio) = 0;
    virtual void updateCanvasZoomInternal(KoZoomMode::Mode mode, qreal zoom, qreal resolutionX, qreal resolutionY, const std::optional<KoViewTransformStillPoint> &docStillPoint) = 0;
    virtual void zoomToInternal(const QRect &viewRect) = 0;

    void emitSignals(const KisCanvasState &oldState, const KisCanvasState &newState);

private:
    Q_PRIVATE_SLOT(d, void activate())

    Private * const d;
};

#endif
