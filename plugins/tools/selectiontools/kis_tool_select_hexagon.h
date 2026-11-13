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

    /// the regular hexagon that fits in the box (flat top: a flat edge on top and bottom; pointed top: a point on top and bottom)
    static QPolygonF hexagonInRect(const QRectF &rect, bool flatTop);
    /// the settings the tool works with now, from the program's settings (the hex grid of View > Show > Hex and the tool's own options)
    struct HexSettings {
        bool flatTop = false;      // the orientation of the hexagon
        bool snap = false;         // snap to the hex grid
        bool locked = false;       // the settings are locked: no rotation, no scaling, the size and the orientation of the grid
        qreal gridSize = 100.0;    // the hex grid: center to corner, image pixels
        bool gridFlat = false;     // the hex grid's orientation
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

#endif // KIS_TOOL_SELECT_HEXAGON_H_
