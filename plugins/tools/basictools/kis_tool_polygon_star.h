/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Krimble 2026-11-13 (task 74): George: "hexagon and star tools - BUILD IMMEDIATELY." The Polygon and Star drawing tool: drag out a regular polygon (3 to 24 sides, a hexagon by default)
 *  or a star (3 to 24 points) with a line (thickness, color) and a fill. Flat top or pointed top. The hex grid of View > Show > Hex: Snap to it, and the shared Lock
 *  (no rotation, no scaling, no change of size or grid until unlocked). It is built like the Ellipse tool: the dragged box is the box of the shape; the line and fill
 *  options are the shape tools' own.
 */

#ifndef KIS_TOOL_POLYGON_STAR_H_
#define KIS_TOOL_POLYGON_STAR_H_

#include "kis_tool_shape.h"
#include "kis_types.h"
#include "KisToolPaintFactoryBase.h"
#include <kis_tool_rectangle_base.h>
#include <kis_icon.h>
#include <QPointer>
#include <QPolygonF>

class KoCanvasBase;

class KisToolPolygonStar : public KisToolRectangleBase
{
    Q_OBJECT

public:
    KisToolPolygonStar(KoCanvasBase *canvas);
    ~KisToolPolygonStar() override;

    QList<QPointer<QWidget> > createOptionWidgets() override;
    void activate(const QSet<KoShape*> &shapes) override;
    bool supportsPaintingAssistants() const override { return true; }

    struct Settings {
        int sides = 6;             // 3 to 24 (a hexagon by default)
        bool star = false;         // a star: 2 x sides points, every second one on the inner radius
        int innerPercent = 50;     // the inner radius of a star, % of the outer
        bool flatTop = false;      // flat top (an edge on top) or pointed top (a corner on top)
        bool snap = false;         // snap to the hex grid
        bool locked = false;       // the shared hex lock: no rotation, no scaling, the size and the grid are fixed
        qreal gridSize = 100.0;    // the hex grid (View > Show > Hex Size): center to corner, image pixels
        bool gridFlat = false;     // the hex grid's orientation
        bool fixedSize() const { return snap || locked; }
        bool effectiveFlat() const { return snap && sides == 6 && !star ? gridFlat : flatTop; }
    };
    static Settings readSettings();

    /// the shape fitted into the box, in the box's coordinates (a regular polygon or a star)
    static QPolygonF shapeInRect(const QRectF &rect, const Settings &s);

protected Q_SLOTS:
    void resetCursorStyle() override;

protected:
    void finishRect(const QRectF &rect, qreal roundCornersX, qreal roundCornersY) override;
    void paintRectangle(QPainter &gc, const QRectF &imageRect) override;
    QRectF createRect(const QPointF &start, const QPointF &end) override;
    bool showRoundCornersGUI() const override { return false; }
    qreal shapeRotationAngle();

private:
    QPointer<QWidget> m_options;
};

class KisToolPolygonStarFactory : public KisToolPaintFactoryBase
{
public:
    KisToolPolygonStarFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolPolygonStar") {
        setToolTip(i18n("Polygon and Star Tool"));
        setSection(ToolBoxSection::PSOrder);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
        setIconName(koIconNameCStr("krita_tool_shape_star"));
        setPriority(3);
        setToolBoxPriority(192);
    }

    ~KisToolPolygonStarFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override {
        return new KisToolPolygonStar(canvas);
    }
};

#endif // KIS_TOOL_POLYGON_STAR_H_
