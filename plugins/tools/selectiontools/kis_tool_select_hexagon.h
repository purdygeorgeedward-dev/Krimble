/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Krimble 2026-11-12 (task 42): George: "All I asked for was hexagon and star." and "I want to be able to lock that in for either the selection tool or the drawing tool ... There should be a snapping setting so it can snap to a hex grid the user sets." and "you should be able to lock the settings so it doesn't rotate or scale or change line thickness or the hex grid until you unlock it."
 *  The Hexagon Selection tool: drag out a regular hexagon (a selection), flat top or pointed top. It is built like the Rectangular Selection tool (the same drag, the same
 *  rotation, the same Replace / Add / Subtract / Intersect buttons): the dragged rectangle is the box of the hexagon.
 */

#ifndef KIS_TOOL_SELECT_HEXAGON_H_
#define KIS_TOOL_SELECT_HEXAGON_H_

#include "KisSelectionToolFactoryBase.h"
#include "kis_tool_rectangle_base.h"
#include <kis_tool_select_base.h>
#include "kis_selection_tool_config_widget_helper.h"
#include <kis_icon.h>
#include <QKeySequence>
#include <QPointer>
#include <QPolygonF>

class KrimbleHexOptionsWidget;

class __KisToolSelectHexagonLocal : public KisToolRectangleBase
{
    Q_OBJECT

public:
    __KisToolSelectHexagonLocal(KoCanvasBase *canvas);

    QList<QPointer<QWidget> > createOptionWidgets() override;
    void activate(const QSet<KoShape*> &shapes) override;

    static QPolygonF hexagonInRect(const QRectF &rect, bool flatTop);
    struct HexSettings {
        bool flatTop = false;
        bool snap = false;
        bool locked = false;
        qreal gridSize = 100.0;
        bool gridFlat = false;
        bool fixedSize() const { return snap || locked; }
        bool effectiveFlat() const { return snap ? gridFlat : flatTop; }
    };
    static HexSettings readHexSettings();

protected:
    void paintRectangle(QPainter &gc, const QRectF &imageRect) override;
    QRectF createRect(const QPointF &start, const QPointF &end) override;
    bool showRoundCornersGUI() const override { return false; }
    qreal hexRotationAngle();

private:
    QPointer<QWidget> m_hexOptions;
};

class KisToolSelectHexagon : public KisToolSelectBase<__KisToolSelectHexagonLocal>
{
    Q_OBJECT
public:
    KisToolSelectHexagon(KoCanvasBase* canvas);
    void resetCursorStyle() override;
private:
    void finishRect(const QRectF& rect, qreal roundCornersX, qreal roundCornersY) override;
    void beginShape() override;
    void endShape() override;
};

class KisToolSelectHexagonFactory : public KisSelectionToolFactoryBase
{

public:
    KisToolSelectHexagonFactory()
        : KisSelectionToolFactoryBase("KisToolSelectHexagon")
    {
        setToolTip(i18n("Hexagon Selection Tool"));
        setSection(ToolBoxSection::PSOrder);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
        setIconName(koIconNameCStr("tool_hexagon_selection"));
        setPriority(0);
        setToolBoxPriority(26);
    }

    ~KisToolSelectHexagonFactory() override {}

    KoToolBase * createTool(KoCanvasBase *canvas) override {
        return new KisToolSelectHexagon(canvas);
    }
};

#endif
