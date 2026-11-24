/*
 *  kis_tool_transform.h - part of Krita
 *
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2010 Marc Pegon <pe.marc@free.fr>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_TRANSFORM_H_
#define KIS_TOOL_TRANSFORM_H_

#include <kis_icon.h>

#include <QPoint>
#include <QPointF>
#include <QVector2D>
#include <QVector3D>
#include <QButtonGroup>
#include <QPointer>

#include <QKeySequence>

#include <KoToolFactoryBase.h>

#include <kis_shape_selection.h>
#include <kis_undo_adapter.h>
#include <kis_types.h>
#include <flake/kis_node_shape.h>
#include <kis_tool.h>
#include <kis_canvas2.h>
#include <kis_action.h>

#include <KisToolPaintFactoryBase.h>

#include "tool_transform_args.h"
#include "KisToolChangesTracker.h"
#include "kis_tool_transform_config_widget.h"
#include "transform_transaction_properties.h"
#include "kis_signal_auto_connection.h"

#include "strokes/inplace_transform_stroke_strategy.h"

#include <kis_paint_device.h>
class QTouchEvent;
class KisTransformStrategyBase;
class KisWarpTransformStrategy;
class KisCageTransformStrategy;
class KisLiquifyTransformStrategy;
class KisFreeTransformStrategy;
class KisPerspectiveTransformStrategy;
class KisMeshTransformStrategy;

class KisToolTransform : public KisTool
{

    Q_OBJECT

    Q_PROPERTY(TransformToolMode transformMode READ transformMode WRITE setTransformMode NOTIFY transformModeChanged)

    Q_PROPERTY(double translateX READ translateX WRITE setTranslateX NOTIFY freeTransformChanged)
    Q_PROPERTY(double translateY READ translateY WRITE setTranslateY NOTIFY freeTransformChanged)

    Q_PROPERTY(double rotateX READ rotateX WRITE setRotateX NOTIFY freeTransformChanged)
    Q_PROPERTY(double rotateY READ rotateY WRITE setRotateY NOTIFY freeTransformChanged)
    Q_PROPERTY(double rotateZ READ rotateZ WRITE setRotateZ NOTIFY freeTransformChanged)

    Q_PROPERTY(double scaleX READ scaleX WRITE setScaleX NOTIFY freeTransformChanged)
    Q_PROPERTY(double scaleY READ scaleY WRITE setScaleY NOTIFY freeTransformChanged)

    Q_PROPERTY(double shearX READ shearX WRITE setShearX NOTIFY freeTransformChanged)
    Q_PROPERTY(double shearY READ shearY WRITE setShearY NOTIFY freeTransformChanged)

    Q_PROPERTY(WarpType warpType READ warpType WRITE setWarpType NOTIFY warpTransformChanged)
    Q_PROPERTY(double warpFlexibility READ warpFlexibility WRITE setWarpFlexibility NOTIFY warpTransformChanged)
    Q_PROPERTY(int warpPointDensity READ warpPointDensity WRITE setWarpPointDensity NOTIFY warpTransformChanged)

public:
    enum TransformToolMode {
        FreeTransformMode,
        WarpTransformMode,
        CageTransformMode,
        LiquifyTransformMode,
        PerspectiveTransformMode,
        MeshTransformMode
    };
    Q_ENUMS(TransformToolMode)

    enum WarpType {
        RigidWarpType,
        AffineWarpType,
        SimilitudeWarpType
    };
    Q_ENUMS(WarpType)

    KisToolTransform(KoCanvasBase * canvas);
    ~KisToolTransform() override;

    bool wantsAutoScroll() const override {
        return false;
    }

    QWidget* createOptionWidget() override;

    void mousePressEvent(KoPointerEvent *e) override;
    void mouseMoveEvent(KoPointerEvent *e) override;
    void mouseReleaseEvent(KoPointerEvent *e) override;
    void beginActionImpl(KoPointerEvent *event, bool usePrimaryAction, KisTool::AlternateAction action);

    QPointF krimbleTransformCenter() const { return m_currentArgs.transformedCenter(); }
    void continueActionImpl(KoPointerEvent *event, bool usePrimaryAction, KisTool::AlternateAction action);
    void endActionImpl(KoPointerEvent *event, bool usePrimaryAction, KisTool::AlternateAction action);
    QMenu* popupActionsMenu() override;

    void activatePrimaryAction() override;
    void deactivatePrimaryAction() override;
    void beginPrimaryAction(KoPointerEvent *event) override;
    void continuePrimaryAction(KoPointerEvent *event) override;
    void endPrimaryAction(KoPointerEvent *event) override;

    void activateAlternateAction(AlternateAction action) override;
    void deactivateAlternateAction(AlternateAction action) override;
    void beginAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void continueAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void endAlternateAction(KoPointerEvent *event, AlternateAction action) override;

    void paint(QPainter& gc, const KoViewConverter &converter) override;

    void newActivationWithExternalSource(KisPaintDeviceSP externalSource) override;

    void setNextActivationTransformMode(TransformToolMode mode);
    TransformToolMode transformMode() const;

    double translateX() const;
    double translateY() const;

    double rotateX() const;
    double rotateY() const;
    double rotateZ() const;

    double scaleX() const;
    double scaleY() const;

    double shearX() const;
    double shearY() const;

    WarpType warpType() const;
    double warpFlexibility() const;
    int warpPointDensity() const;

    static ToolTransformArgs::TransformMode toArgsMode(KisToolTransform::TransformToolMode toolMode);

public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    void applyTransform();

    void requestImageRecalculation();

    void krimbleRefreshHandles();

    void setTransformMode( KisToolTransform::TransformToolMode newMode );

    void setTranslateX(double translateX);
    void setTranslateY(double translateY);

    void setRotateX(double rotation);
    void setRotateY(double rotation);
    void setRotateZ(double rotation);

    void setScaleX(double scaleX);
    void setScaleY(double scaleY);

    void setShearX(double shearX);
    void setShearY(double shearY);

    void setWarpType(WarpType type);
    void setWarpFlexibility(double flexibility);
    void setWarpPointDensity(int density);

protected Q_SLOTS:
    void resetCursorStyle() override;
    void slotGlobalConfigChanged();

Q_SIGNALS:
    void transformModeChanged();
    void freeTransformChanged();
    void warpTransformChanged();

public Q_SLOTS:
    void requestUndoDuringStroke() override;
    void requestRedoDuringStroke() override;
    void requestStrokeEnd() override;
    void requestStrokeCancellation() override;
    void canvasUpdateRequested();
    void cursorOutlineUpdateRequested(const QPointF &imagePos);

    void updateOptionWidget();

    void resetRotationCenterButtonsRequested();
    void imageTooBigRequested(bool value);
    void convexHullCalculationRequested();
    void slotConvexHullCalculated(QPolygon hull, void *strokeStrategyCookie);

private:
    QScopedPointer<QMenu> m_contextMenu;

    void startStroke(ToolTransformArgs::TransformMode mode, bool forceReset);
    void endStroke();
    void cancelStroke();

private:
    void outlineChanged();
    void setFunctionalCursor();
    void setTransformFunction(QPointF mousePos, Qt::KeyboardModifiers modifiers);

    void commitChanges();

    void initTransformMode(ToolTransformArgs::TransformMode mode);
    void initGuiAfterTransformMode();

    void initThumbnailImage(KisPaintDeviceSP previewDevice);
    void updateApplyResetAvailability();

private:
    ToolTransformArgs m_currentArgs;

    KisPaintDeviceSP m_externalSourceForNextActivation;

    bool m_actuallyMoveWhileSelected {false};

    KisPaintDeviceSP m_selectedPortionCache;
    KisStrokeId m_strokeId;
    void *m_strokeStrategyCookie {0};
    bool m_currentlyUsingOverlayPreviewStyle {false};
    bool m_preferOverlayPreviewStyle {false};
    bool m_forceLodMode {false};

    QPainterPath m_selectionPath;

    KisToolTransformConfigWidget *m_optionsWidget {0};
    QPointer<KisCanvas2> m_canvas;

    TransformTransactionProperties m_transaction;
    KisToolChangesTracker m_changesTracker;
    KisSignalAutoConnectionsStore m_actionConnections;

    TransformToolMode nextActivationTransformMode {FreeTransformMode};

    KisAction* warpAction {0};
    KisAction* meshAction {0};
    KisAction* liquifyAction {0};
    KisAction* cageAction {0};
    KisAction* freeTransformAction {0};
    KisAction* perspectiveAction {0};
    KisAction* applyTransformation {0};
    KisAction* resetTransformation {0};

    KisAction* mirrorHorizontalAction {0};
    KisAction* mirrorVerticalAction {0};
    KisAction* rotateNinetyCWAction {0};
    KisAction* rotateNinetyCCWAction {0};
    KisAction* keepAspectRatioAction {0};

    QRectF m_refRect;

    QScopedPointer<KisWarpTransformStrategy> m_warpStrategy;
    QScopedPointer<KisCageTransformStrategy> m_cageStrategy;
    QScopedPointer<KisLiquifyTransformStrategy> m_liquifyStrategy;
    QScopedPointer<KisMeshTransformStrategy> m_meshStrategy;
    QScopedPointer<KisFreeTransformStrategy> m_freeStrategy;
    QScopedPointer<KisPerspectiveTransformStrategy> m_perspectiveStrategy;
    KisTransformStrategyBase* currentStrategy() const;

    QPainterPath m_cursorOutline;

    KisAsynchronousStrokeUpdateHelper m_asyncUpdateHelper;

private Q_SLOTS:
    void slotTrackerChangedConfig(KisToolChangesTrackerDataSP status);
    void slotUiChangedConfig(bool needsPreviewRecalculation);
    void slotApplyTransform();
    void slotResetTransform(ToolTransformArgs::TransformMode mode);
    void slotCancelTransform();
    void slotRestartTransform();
    void slotRestartAndContinueTransform();
    void slotEditingFinished();

    void slotMoveDiscreteUp();
    void slotMoveDiscreteUpMore();
    void slotMoveDiscreteDown();
    void slotMoveDiscreteDownMore();
    void slotMoveDiscreteLeft();
    void slotMoveDiscreteLeftMore();
    void slotMoveDiscreteRight();
    void slotMoveDiscreteRightMore();

    void slotIncreaseBrushSize();
    void slotDecreaseBrushSize();

    void slotTransactionGenerated(TransformTransactionProperties transaction, ToolTransformArgs args, void *strokeStrategyCookie);
    void slotPreviewDeviceGenerated(KisPaintDeviceSP device);

    void slotUpdateToWarpType();
    void slotUpdateToPerspectiveType();
    void slotUpdateToFreeTransformType();
    void slotUpdateToLiquifyType();
    void slotUpdateToMeshType();
    void slotUpdateToCageType();
};

class KisToolTransformFactory : public KisToolPaintFactoryBase
{

    Q_OBJECT

public:

    KisToolTransformFactory()
            : KisToolPaintFactoryBase("KisToolTransform") {
        setToolTip(i18n("Transform"));
        setSection(ToolBoxSection::PSOrder);
        setToolBoxPriority(240);
        setIconName(koIconNameCStr("krita_tool_transform"));
        setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
        setPriority(2);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolTransformFactory() override {}

    KoToolBase * createTool(KoCanvasBase *canvas) override {
        return new KisToolTransform(canvas);
    }

    QList<QAction *> createActionsImpl() override;

private:
    void activateSubtool(KisToolTransform::TransformToolMode mode);

private Q_SLOTS:
    void activateSubtoolFree();
    void activateSubtoolPerspective();
    void activateSubtoolWarp();
    void activateSubtoolCage();
    void activateSubtoolLiquify();
    void activateSubtoolMesh();
};

class KisToolFreeRotate : public KisToolTransform
{
    Q_OBJECT

public:
    explicit KisToolFreeRotate(KoCanvasBase *canvas);

public:
    void rotateByDegrees(double degrees);

protected:
    void beginPrimaryAction(KoPointerEvent *event) override;
    void continuePrimaryAction(KoPointerEvent *event) override;
    void endPrimaryAction(KoPointerEvent *event) override;

public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;

private:
    QPointer<QObject> m_touchFilter;
    double m_rotationRadians {0.0};
    bool m_dragging {false};
    double m_dragLastAngle {0.0};
    double pointerAngle(KoPointerEvent *event) const;
};

class KisToolFreeRotateFactory : public KisToolPaintFactoryBase
{
    Q_OBJECT

public:
    KisToolFreeRotateFactory()
            : KisToolPaintFactoryBase("KisToolFreeRotate") {
        setToolTip(i18n("Rotate"));
        setSection(ToolBoxSection::PSOrder);
        setToolBoxPriority(241);
        setIconName(koIconNameCStr("krimble-free-rotate"));
        setPriority(3);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolFreeRotateFactory() override {}

    KoToolBase * createTool(KoCanvasBase *canvas) override {
        return new KisToolFreeRotate(canvas);
    }
};

#endif

