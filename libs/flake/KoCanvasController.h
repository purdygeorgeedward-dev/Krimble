/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007-2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2007-2008 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCANVASCONTROLLER_H
#define KOCANVASCONTROLLER_H

#include "kritaflake_export.h"
#include <QObject>

#include <QSize>
#include <QPoint>
#include <QPointF>
#include <QPointer>

#include <KoZoomState.h>

class KisKActionCollection;
class QRect;
class QRectF;

class KoShape;
class KoCanvasBase;
class KoCanvasControllerProxyObject;
class KoViewTransformStillPoint;

class KRITAFLAKE_EXPORT KoCanvasController
{
public:

    QPointer<KoCanvasControllerProxyObject> proxyObject;

    explicit KoCanvasController(KisKActionCollection* actionCollection);
    virtual ~KoCanvasController();

public:

    virtual void setCanvas(KoCanvasBase *canvas) = 0;

    virtual KoCanvasBase *canvas() const;

    virtual void ensureVisibleDoc(const QRectF &docRect, bool smooth) = 0;

    virtual void zoomIn(const KoViewTransformStillPoint &stillPoint) = 0;
    virtual void zoomIn() = 0;

    virtual void zoomOut(const KoViewTransformStillPoint &stillPoint) = 0;
    virtual void zoomOut() = 0;

    virtual void zoomTo(const QRect &rect) = 0;

    virtual void setZoom(KoZoomMode::Mode mode, qreal zoom) = 0;

    virtual void setPreferredCenter(const QPointF &viewPoint) = 0;

    virtual QPointF preferredCenter() const = 0;

    virtual void pan(const QPoint &distance) = 0;

    virtual void panUp() = 0;

    virtual void panDown() = 0;

    virtual void panLeft() = 0;

    virtual void panRight() = 0;

    virtual QPoint scrollBarValue() const = 0;

    virtual void setScrollBarValue(const QPoint &value) = 0;

    virtual void resetScrollBars() = 0;

    KisKActionCollection* actionCollection() const;

    virtual QPointF currentCursorPosition() const = 0;

    virtual KoZoomState zoomState() const = 0;

protected:
    void setDocumentOffset(const QPoint &offset);

private:
    class Private;
    Private * const d;
};

class KRITAFLAKE_EXPORT KoCanvasControllerProxyObject : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(KoCanvasControllerProxyObject)
public:
    explicit KoCanvasControllerProxyObject(KoCanvasController *canvasController, QObject *parent = 0);

public:

    void emitCanvasRemoved(KoCanvasController *canvasController) { Q_EMIT canvasRemoved(canvasController); }
    void emitCanvasSet(KoCanvasController *canvasController) { Q_EMIT canvasSet(canvasController); }
    void emitCanvasOffsetChanged() { Q_EMIT canvasOffsetChanged(); }
    void emitCanvasMousePositionChanged(const QPoint &position) { Q_EMIT canvasMousePositionChanged(position); }
    void emitDocumentMousePositionChanged(const QPointF &position) { Q_EMIT documentMousePositionChanged(position); }
    void emitSizeChanged(const QSize &size) { Q_EMIT sizeChanged(size); }
    void emitMoveDocumentOffset(const QPointF &oldOffset, const QPointF &newOffset) { Q_EMIT moveDocumentOffset(oldOffset, newOffset); }
    void emitEffectiveZoomChanged(qreal zoom) { Q_EMIT effectiveZoomChanged(zoom); }
    void emitZoomStateChanged(const KoZoomState &zoomState) { Q_EMIT zoomStateChanged(zoomState); }
    void emitDocumentRectInWidgetPixelsChanged(const QRectF &documentRectInWidgetPixels) { Q_EMIT documentRectInWidgetPixelsChanged(documentRectInWidgetPixels); }
    void emitDocumentRotationChanged(qreal angle) { Q_EMIT documentRotationChanged(angle); }
    void emitDocumentMirrorStatusChanged(bool mirrorX, bool mirrorY) { Q_EMIT documentMirrorStatusChanged(mirrorX, mirrorY); }
    void emitCanvasStateChanged() { Q_EMIT canvasStateChanged(); }

    KoCanvasController *canvasController() const { return m_canvasController; }

Q_SIGNALS:
    void canvasRemoved(KoCanvasController *canvasController);

    void canvasSet(KoCanvasController *canvasController);

    void canvasOffsetChanged();

    void canvasMousePositionChanged(const QPoint &position);

    void documentMousePositionChanged(const QPointF &position);

    void sizeChanged(const QSize &size);

    void moveDocumentOffset(const QPointF &oldOffset, const QPointF &newOffset);

    void effectiveZoomChanged(qreal zoom);

    void zoomStateChanged(const KoZoomState &zoomState);

    void documentRectInWidgetPixelsChanged(const QRectF &documentRectInWidgetPixels);

    void documentRotationChanged(qreal angle);
    void documentMirrorStatusChanged(bool mirrorX, bool mirrorY);

    void canvasStateChanged();

private:
    KoCanvasController *m_canvasController;
};

#endif
