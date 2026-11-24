/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_CANVAS_H
#define KIS_CANVAS_H

#include <QObject>
#include <QWidget>
#include <QSize>
#include <QString>

#include <KoConfig.h>
#include <KoColorConversionTransformation.h>
#include <KoCanvasBase.h>
#include <kritaui_export.h>
#include <kis_types.h>
#include <KoPointerEvent.h>

#include "opengl/kis_opengl.h"

#include "kis_ui_types.h"
#include "kis_coordinates_converter.h"
#include "kis_canvas_decoration.h"
#include "kis_painting_assistants_decoration.h"
#include "input/KisInputActionGroup.h"
#include "KisReferenceImagesDecoration.h"
#include "KisWraparoundAxis.h"

class KoToolProxy;
class KisDisplayConfig;

class KisViewManager;
class KisFavoriteResourceManager;
class KisDisplayFilter;
class KisDisplayColorConverter;
struct KisExposureGammaCorrectionInterface;
class KisView;
class KisInputManager;
class KisCanvasAnimationState;
class KisShapeController;
class KisCoordinatesConverter;
class KoViewConverter;
class KisAbstractCanvasWidget;
class KisPopupPalette;

class KRITAUI_EXPORT KisCanvas2 : public KoCanvasBase
{

    Q_OBJECT

public:

    KisCanvas2(KisCoordinatesConverter *coordConverter, KoCanvasResourceProvider *resourceManager, KisMainWindow *mainWindow, KisView *view, KoShapeControllerBase *sc);

    ~KisCanvas2() override;

    void disconnectCanvasObserver(QObject *object) override;

public:

    bool canvasIsOpenGL() const override;

    KisOpenGL::FilterMode openGLFilterMode() const;

    void gridSize(QPointF *offset, QSizeF *spacing) const override;

    bool snapToGrid() const override;

    void addCommand(KUndo2Command *command) override;

    QPoint documentOrigin() const override;
    QPoint documentOffset() const;

    KoShapeManager * shapeManager() const override;

    KoSelectedShapesProxy *selectedShapesProxy() const override;

    KoShapeManager *globalShapeManager() const;

    KoShape *currentShapeManagerOwnerShape() const override;
    void setCurrentShapeManagerOwnerShape(KoShape *source) override;

    KoShapeManager *localShapeManager() const;

    void updateCanvas(const QRectF& rc) override;

    const KisCoordinatesConverter* coordinatesConverter() const;
    const KoViewConverter *viewConverter() const override;
    KoViewConverter *viewConverter() override;

    KoColorDisplayRendererInterface* displayRendererInterface() const override;

    QWidget* canvasWidget() override;

    const QWidget* canvasWidget() const override;

    KoUnit unit() const override;

    KoToolProxy* toolProxy() const override;

    KisImageWSP currentImage() const;

    KisInputManager* globalInputManager() const;

    KisPaintingAssistantsDecorationSP paintingAssistantsDecoration() const;
    KisReferenceImagesDecorationSP referenceImagesDecoration() const;

public:

    KisImageWSP image() const;
    KisViewManager* viewManager() const;
    QPointer<KisView> imageView() const;

    void addDecoration(KisCanvasDecorationSP deco);
    KisCanvasDecorationSP decoration(const QString& id) const;

    void setDisplayFilter(QSharedPointer<KisDisplayFilter> displayFilter);
    QSharedPointer<KisDisplayFilter> displayFilter() const;

    KisDisplayColorConverter *displayColorConverter() const;
    KisExposureGammaCorrectionInterface* exposureGammaCorrectionInterface() const;

    void fetchProofingOptions();
    void updateProofingState();
    KisProofingConfigurationSP proofingConfiguration() const;

    void setProofingConfigUpdated(bool updated);

    bool proofingConfigUpdated();

    void setCursor(const QCursor &cursor) override;
    KisAnimationFrameCacheSP frameCache() const;
    KisCanvasAnimationState *animationState() const;
    void refetchDataFromImage();

    QRect regionOfInterest() const;

    void setRenderingLimit(const QRect &rc);

    QRect renderingLimit() const;

    KisPopupPalette* popupPalette();

    KisInputActionGroupsMaskInterface::SharedInterface inputActionGroupsMaskInterface();

    QString colorManagementReport() const;

Q_SIGNALS:
    void sigCanvasEngineChanged();

    void sigCanvasCacheUpdated();
    void sigContinueResizeImage(qint32 w, qint32 h);

    void sigCanvasStateChanged();

    void updateCanvasRequested(const QRect &rc);

    void sigRegionOfInterestChanged(const QRect &roi);

public Q_SLOTS:

    void updateCanvas();

    void updateCanvasProjection(const QRectF &docRect);
    void updateCanvasDecorations();
    void updateCanvasDecorations(const QRectF &docRect);
    void updateCanvasToolOutlineDoc(const QRectF &docRect);
    void updateCanvasToolOutlineWdg(const QRect &widgetRect);
    void updateCanvasScene();

    void startResizingImage();
    void finishResizingImage(qint32 w, qint32 h);

    qreal rotationAngle() const;
    bool xAxisMirrored() const;
    bool yAxisMirrored() const;
    void slotSoftProofing();
    void slotGamutCheck();
    void slotChangeGlobalProofingConfig();
    void slotChangeProofingConfig();
    void slotPopupPaletteRequestedZoomChange(int zoom);

    void channelSelectionChanged();

    void startUpdateInPatches(const QRect &imageRect);

    void slotTrySwitchShapeManager();

    void slotConfigChanged();

    void slotScreenChanged(QScreen *screen);

private Q_SLOTS:

    void startUpdateCanvasProjection(const QRect & rc);
    void updateCanvasProjection();

    void slotBeginUpdatesBatch();
    void slotEndUpdatesBatch();
    void slotSetLodUpdatesBlocked(bool value);

    void slotEffectiveZoomChanged(qreal newZoom);
    void slotCanvasStateChanged();

    void slotSelectionChanged();

    void slotDoCanvasUpdate();

    void bootstrapFinished();

    void slotUpdateRegionOfInterest();
    void slotUpdateReferencesBounds();

    void slotImageColorSpaceChanged();

public:
    void setWrapAroundViewingMode(bool value);
    bool wrapAroundViewingMode() const;

    void setWrapAroundViewingModeAxis(WrapAroundAxis value);
    WrapAroundAxis wrapAroundViewingModeAxis() const;

    void setLodPreferredInCanvas(bool value);
    bool lodPreferredInCanvas() const;

    void initializeImage();
    void disconnectImage();

    void setFavoriteResourceManager(KisFavoriteResourceManager* favoriteResourceManager);

private:
    Q_DISABLE_COPY(KisCanvas2)

    void requestCanvasUpdateMaybeCompressed();
    void connectCurrentCanvas();
    void createCanvas(bool useOpenGL);
    void createQPainterCanvas();
    void createOpenGLCanvas();
    void updateCanvasWidgetImpl(const QRect &rc = QRect());
    void setCanvasWidget(KisAbstractCanvasWidget *widget);
    void resetCanvas(bool useOpenGL);
    void slotSurfaceFormatChanged(const KisDisplayConfig &config);

    void notifyLevelOfDetailChange();

    void setup();

    void initializeFpsDecoration();

private:
    friend class KisView;
    class KisCanvas2Private;
    KisCanvas2Private * const m_d;
};

#endif
