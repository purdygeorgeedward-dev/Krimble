/* This file is part of the KDE project
 * Copyright 2010 (C) Boudewijn Rempt <boud@valdyas.org>
 * Copyright 2011 (C) Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_SCRATCH_PAD_H
#define KIS_SCRATCH_PAD_H

#include <QImage>
#include <QWidget>
#include <QRect>

#include <KoColor.h>

#include <brushengine/kis_paintop_preset.h>
#include <kis_types.h>
#include <kis_gradient_painter.h>
#include <kritaui_export.h>
#include <KisDisplayConfig.h>
#include <KisMultiSurfaceStateManager.h>

class QColor;

class KoPointerEvent;

class KisCanvasResourceProvider;
class KisUpdateScheduler;
class KisUndoStore;
class KisPostExecutionUndoAdapter;
class KisScratchPadEventFilter;
class KisPaintingInformationBuilder;
class KisToolFreehandHelper;
class KisNodeGraphListener;
class KisScreenMigrationTracker;

class KRITAUI_EXPORT KisScratchPad : public QWidget
{

    Q_OBJECT

public:
    void setupScratchPad(KisCanvasResourceProvider* resourceProvider,
                         const QColor &defaultColor);

    KisScratchPad(QWidget *parent = 0);
    ~KisScratchPad() override;

    void setCutoutOverlayRect(const QRect&rc);

    void setModeManually(bool value);

    void setModeType(QString modeName);

    bool canvasZoomLink();

    void setCanvasZoomLink(bool value);

    bool setScale(qreal scaleX, qreal scaleY);

    qreal scaleX();
    qreal scaleY();

    void scaleToFit();
    void scaleReset();

    void panTo(qint32 x, qint32 y);

    void panCenter();

    QImage cutoutOverlay() const;

    void imageUpdated(const QRect &rect);

    QRect imageBounds() const;
    QRect viewportBounds() const;

    QRect contentBounds() const;

    void wheelDelta(QWheelEvent *event);
    void pointerPress(KoPointerEvent *event);
    void pointerRelease(KoPointerEvent *event);
    void pointerMove(KoPointerEvent *event);
    void resetState();

public Q_SLOTS:
    void fillDefault();
    void fillPattern(QTransform transform);
    void fillGradient(const QPoint &gradientVectorStart,
                      const QPoint &gradientVectorEnd,
                      KisGradientPainter::enumGradientShape gradientShape,
                      KisGradientPainter::enumGradientRepeat gradientRepeat,
                      bool reverseGradient,
                      bool dither);
    void fillGradient();
    void fillBackground();
    void fillForeground();
    void fillTransparent();

    void setFillColor(QColor newColor);

    void fillLayer(bool fullContent);
    void fillDocument();
    void fillDocument(bool fullContent);

    void setPresetImage(const QImage& image);

    void paintPresetImage();

    void paintCustomImage(const QImage & loadedImage);

    void loadScratchpadImage(QImage image);

    QImage copyScratchpadImageData();

Q_SIGNALS:
    void scaleChanged(qreal scale);

    void contentChanged();

    void viewportChanged(const QRect rect);

private Q_SLOTS:
    void slotScreenChanged(QScreen *screen);
    void setOnScreenResolution(qreal scaleX, qreal scaleY);
    void slotUpdateCanvas(const QRect &rect);
    void slotConfigChanged();

Q_SIGNALS:
    void colorSelected(const KoColor& color);
    void sigUpdateCanvas(const QRect &rect);

protected:
    void paintEvent ( QPaintEvent * event ) override;
    void resizeEvent( QResizeEvent *event) override;

private:
    void beginStroke(KoPointerEvent *event);
    void doStroke(KoPointerEvent *event);
    void endStroke(KoPointerEvent *event);

    void beginPan(KoPointerEvent *event);
    void doPan(KoPointerEvent *event);
    void endPan(KoPointerEvent *event);

    void sample(KoPointerEvent *event);

    void updateTransformations();

    bool setScaleImpl(qreal scaleX, qreal scaleY);

    void updateViewport();
    bool updateViewportImpl();

    void assignNewSurfaceState(const KisMultiSurfaceStateManager::State &newState);

    QTransform documentToWidget() const;
    QTransform widgetToDocument() const;

    friend class KisScratchPadPaintingInformationBuilder;

private:
    friend KisScratchPadEventFilter;

    void resetWheelDelta();

private:
    enum Mode {
        PAINTING,
        HOVERING,
        PANNING,
        SAMPLING
    };

    Mode modeFromButton(Qt::MouseButton button) const;

private:

    KoColor m_defaultColor;
    Mode m_toolMode;
    bool isModeManuallySet;
    bool isMouseDown;
    bool m_linkCanvasZoomLevel;

    qreal m_canvasScaleX;
    qreal m_canvasScaleY;

    qreal m_scratchpadScaleX;
    qreal m_scratchpadScaleY;

    int m_accumulatedMouseDelta;

    QRect m_viewport;

    KisPaintLayerSP m_paintLayer;
    const KoColorProfile* m_displayProfile;
    KisDisplayConfig m_displayConfig;
    QCursor m_cursor;
    QCursor m_colorSamplerCursor;
    QRect m_cutoutOverlay;
    QBrush m_checkBrush;
    KisCanvasResourceProvider* m_resourceProvider;

    KisUpdateScheduler *m_updateScheduler;
    KisUndoStore *m_undoStore;
    KisPostExecutionUndoAdapter *m_undoAdapter;
    KisNodeGraphListener *m_nodeListener;
    KisScratchPadEventFilter *m_eventFilter;

    QScopedPointer<KisToolFreehandHelper> m_helper;
    KisPaintingInformationBuilder *m_infoBuilder;

    QTransform m_scaleTransform;
    QTransform m_translateTransform;

    QPointF m_panDocPoint;
    int m_scaleBorderWidth;

    QImage m_presetImage;
    QScopedPointer<KisScreenMigrationTracker> m_screenMigrationTracker;
    KisMultiSurfaceStateManager m_multiSurfaceStateManager;
    KisMultiSurfaceStateManager::State m_multiSurfaceState;
};

#endif
