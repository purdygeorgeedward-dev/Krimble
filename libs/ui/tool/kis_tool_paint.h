/*
 *  SPDX-FileCopyrightText: 2003 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_PAINT_H_
#define KIS_TOOL_PAINT_H_

#include "kis_tool.h"
#include "kis_tool_utils.h"

#include <QGridLayout>
#include <QPainterPath>
#include <QCheckBox>

#include <KoCanvasResourceProvider.h>
#include <KoToolBase.h>
#include <resources/KoAbstractGradient.h>

#include <kritaui_export.h>

#include <kis_types.h>
#include <kis_image.h>
#include <brushengine/kis_paintop_settings.h>
#include <resources/KoPattern.h>
#include <KisOptimizedBrushOutline.h>
#include "KisAsyncColorSamplerHelper.h"

class QGridLayout;
class KoCompositeOp;
class KoCanvasBase;

class KRITAUI_EXPORT KisToolPaint : public KisTool
{

    Q_OBJECT

public:
    KisToolPaint(KoCanvasBase *canvas, const QCursor &cursor);
    ~KisToolPaint() override;
    int flags() const override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;

    KisPopupWidgetInterface* popupWidget() override;

protected:

    void setMode(ToolMode mode) override;

    void canvasResourceChanged(int key, const QVariant &v) override;

    void paint(QPainter &gc, const KoViewConverter &converter) override;

    void activatePrimaryAction() override;
    void deactivatePrimaryAction() override;

    void activateAlternateAction(AlternateAction action) override;
    void deactivateAlternateAction(AlternateAction action) override;

    void beginAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void continueAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void endAlternateAction(KoPointerEvent *event, AlternateAction action) override;

    virtual void requestUpdateOutline(const QPointF &outlineDocPoint, const KoPointerEvent *event);

    void setSupportOutline(bool supportOutline) {
        m_supportOutline = supportOutline;
    }

    virtual KisOptimizedBrushOutline getOutlinePath(const QPointF &documentPos,
                                                    const KoPointerEvent *event,
                                                    KisPaintOpSettings::OutlineMode outlineMode);

protected:
    bool isOutlineEnabled() const;
    void setOutlineEnabled(bool enabled);
    bool isOutlineVisible() const;
    void setOutlineVisible(bool visible);

    bool isEraser() const;

    void addOptionWidgetLayout(QLayout *layout);

    virtual void addOptionWidgetOption(QWidget *control, QWidget *label = nullptr);

    void showControl(QWidget *control, bool value);
    void enableControl(QWidget *control, bool value);

    QWidget * createOptionWidget() override;

    virtual QString quickHelp() const {
        return QString();
    }

public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;

private Q_SLOTS:

    void slotColorPickerRequestedCursor(const QCursor &cursor);
    void slotColorPickerRequestedCursorReset();
    void slotColorPickerRequestedOutlineUpdate();

    void slotPopupQuickHelp();

    void increaseBrushSize();
    void decreaseBrushSize();
    void showBrushSize();

    void rotateBrushTipClockwise();
    void rotateBrushTipClockwisePrecise();
    void rotateBrushTipCounterClockwise();
    void rotateBrushTipCounterClockwisePrecise();

protected:
    bool m_paintOutline {false};
    QPointF m_outlineDocPoint;
    KisOptimizedBrushOutline m_currentOutline;
    QRectF m_oldOutlineRect;
    QRectF m_oldColorPreviewUpdateRect;

private:
    KisOptimizedBrushOutline tryFixBrushOutline(const KisOptimizedBrushOutline &originalOutline);
    bool isSamplingAction(AlternateAction action);
private:

    bool m_specialHoverModifier {false};
    QGridLayout *m_optionsWidgetLayout {nullptr};

    bool m_supportOutline {false};

    bool m_isOutlineEnabled;
    bool m_isOutlineVisible;
    KisToolUtils::StandardBrushSizes m_standardBrushSizes;

    KisPaintOpPresetSP m_oldPreset;
    qreal m_oldOpacity {1.0};
    bool m_oldPresetIsDirty {false};
    int m_oldPresetVersion {-1};
    KisAsyncColorSamplerHelper m_colorSamplerHelper;

    void tryRestoreOpacitySnapshot();

    struct Private;
    QScopedPointer<Private> m_d;

Q_SIGNALS:
    void sigPaintingFinished();
};

#endif

