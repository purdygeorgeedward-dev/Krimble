/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_H_
#define KIS_TOOL_H_

#include <QCursor>

#include <KoColor.h>
#include <KoToolBase.h>
#include <KoID.h>
#include <KoCanvasResourceProvider.h>
#include <KoPattern.h>
#include <KoAbstractGradient.h>

#include <kis_types.h>

#ifdef __GNUC__
#define WARN_WRONG_MODE(_mode) warnKrita << "Unexpected tool event has come to" << __func__ << "while being mode" << _mode << "!"
#else
#define WARN_WRONG_MODE(_mode) warnKrita << "Unexpected tool event has come while being mode" << _mode << "!"
#endif

#define CHECK_MODE_SANITY_OR_RETURN(_mode) if (mode() != _mode) { WARN_WRONG_MODE(mode()); return; }

class KoCanvasBase;
class KisFilterConfiguration;
class QPainter;
class QPainterPath;
class QPolygonF;
class KisOptimizedBrushOutline;

static const QString KRITA_TOOL_ACTIVATION_ID = "flake/always";

#include <kritaui_export.h>
class  KRITAUI_EXPORT KisTool : public KoToolBase
{
    Q_OBJECT

    Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)

public:
    enum { FLAG_USES_CUSTOM_PRESET=0x01, FLAG_USES_CUSTOM_COMPOSITEOP=0x02, FLAG_USES_CUSTOM_SIZE=0x04 };

    KisTool(KoCanvasBase * canvas, const QCursor & cursor);
    ~KisTool() override;

    virtual int flags() const { return 0; }

    void deleteSelection() override;

public:

    virtual void activatePrimaryAction();

    virtual void deactivatePrimaryAction();

    virtual void beginPrimaryAction(KoPointerEvent *event);

    virtual void continuePrimaryAction(KoPointerEvent *event);

    virtual void endPrimaryAction(KoPointerEvent *event);

    virtual void beginPrimaryDoubleClickAction(KoPointerEvent *event);

    virtual bool primaryActionSupportsHiResEvents() const;

    enum ToolAction {
        Primary,
        AlternateChangeSize,
        AlternateChangeSizeSnap,
        AlternateSampleFgNode,
        AlternateSampleBgNode,
        AlternateSampleFgImage,
        AlternateSampleBgImage,
        AlternateSecondary,
        AlternateThird,
        AlternateFourth,
        AlternateFifth,
        Alternate_NONE = 10000
    };

    enum AlternateAction {
        ChangeSize = AlternateChangeSize,
        ChangeSizeSnap = AlternateChangeSizeSnap,
        SampleFgNode = AlternateSampleFgNode,
        SampleBgNode = AlternateSampleBgNode,
        SampleFgImage = AlternateSampleFgImage,
        SampleBgImage = AlternateSampleBgImage,
        Secondary = AlternateSecondary,
        Third = AlternateThird,
        Fourth = AlternateFourth,
        Fifth = AlternateFifth,
        NONE = 10000
    };

    enum NodePaintAbility {
        VECTOR,
        CLONE,
        PAINT,
        UNPAINTABLE,
        MYPAINTBRUSH_UNPAINTABLE
    };
    Q_ENUMS(NodePaintAbility)

    static AlternateAction actionToAlternateAction(ToolAction action);

    virtual void activateAlternateAction(AlternateAction action);
    virtual void deactivateAlternateAction(AlternateAction action);

    virtual void beginAlternateAction(KoPointerEvent *event, AlternateAction action);
    virtual void continueAlternateAction(KoPointerEvent *event, AlternateAction action);
    virtual void endAlternateAction(KoPointerEvent *event, AlternateAction action);
    virtual void beginAlternateDoubleClickAction(KoPointerEvent *event, AlternateAction action);
    virtual bool alternateActionSupportsHiResEvents(AlternateAction action) const;
    virtual bool supportsPaintingAssistants() const;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void mouseTripleClickEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;

    bool isActive() const;

    KisTool::NodePaintAbility nodePaintAbility();

    virtual void newActivationWithExternalSource(KisPaintDeviceSP externalSource);

public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    void canvasResourceChanged(int key, const QVariant & res) override;
    virtual void updateSettingsViews();

Q_SIGNALS:
    void isActiveChanged(bool isActivated);

protected:
    friend class KisToolPaintingInformationBuilder;

    QPointF convertToPixelCoord(KoPointerEvent *e);
    QPointF convertToPixelCoord(const QPointF& pt);

    QPointF convertToPixelCoordAndAlignOnWidget(const QPointF& pt);

    QPointF convertToPixelCoordAndSnap(KoPointerEvent *e, const QPointF &offset = QPointF(), bool useModifiers = true);
    QPointF convertToPixelCoordAndSnap(const QPointF& pt, const QPointF &offset = QPointF());

protected:
    QPointF widgetCenterInWidgetPixels();
    QPointF convertDocumentToWidget(const QPointF& pt);

    QPoint convertToImagePixelCoordFloored(KoPointerEvent *e);

    QRectF convertToPt(const QRectF &rect);
    qreal convertToPt(qreal value);

    QPointF viewToPixel(const QPointF &viewCoord) const;
    QPointF pixelToView(const QPoint &pixelCoord) const;

    QPointF pixelToView(const QPointF &pixelCoord) const;

    QRectF pixelToView(const QRectF &pixelRect) const;

    QPainterPath pixelToView(const QPainterPath &pixelPath) const;

    KisOptimizedBrushOutline pixelToView(const KisOptimizedBrushOutline &path) const;

    QPolygonF pixelToView(const QPolygonF &pixelPolygon) const;

    void updateCanvasPixelRect(const QRectF &pixelRect);

    void updateCanvasViewRect(const QRectF &viewRect);

    QWidget* createOptionWidget() override;

    virtual bool listeningToModifiers();
    virtual void listenToModifiers(bool listen);

protected:
    KisImageWSP image() const;
    QCursor cursor() const;

    KisImageWSP currentImage();
    KoPatternSP currentPattern();
    KoAbstractGradientSP currentGradient();
    KisNodeSP currentNode() const;
    KisNodeList selectedNodes() const;
    KoColor currentFgColor();
    KoColor currentBgColor();
    KisPaintOpPresetSP currentPaintOpPreset();
    KisFilterConfigurationSP currentGenerator();

    void paintToolOutline(QPainter * painter, const KisOptimizedBrushOutline &path);

    bool nodeEditable();

    bool selectionEditable();

    bool overrideCursorIfNotEditable();

    bool blockUntilOperationsFinished();
    void blockUntilOperationsFinishedForced();

protected:
    enum ToolMode: int {
        HOVER_MODE,
        PAINT_MODE,
        SECONDARY_PAINT_MODE,
        MIRROR_AXIS_SETUP_MODE,
        GESTURE_MODE,
        PAN_MODE,
        OTHER,
        OTHER_1
    };

    virtual void setMode(ToolMode mode);
    virtual ToolMode mode() const;
    void setCursor(const QCursor &cursor);

protected Q_SLOTS:
    virtual void resetCursorStyle();

private:
    struct Private;
    Private* const d;
};

#endif
