/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Krimble 2026-11-13 (task 74): the Polygon and Star drawing tool (see the header).
 */

#include "kis_tool_polygon_star.h"

#include <QtMath>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <limits>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KisOptionCollectionWidget.h>
#include <KoCanvasBase.h>
#include <KoPathShape.h>

#include <KisViewManager.h>
#include <canvas/kis_canvas2.h>
#include <kis_canvas_resource_provider.h>
#include <brushengine/kis_paintop_preset.h>

namespace {

const qreal s_sqrt3 = 1.7320508075688772;

// The center of the hex cell nearest to a point, on the lattice the hex overlay of View > Show > Hex draws (the same as the Hexagon Selection tool).
QPointF nearestHexCenter(const QPointF &p, qreal size, bool flatTop)
{
    QPointF best = p;
    qreal bestDistance = std::numeric_limits<qreal>::max();
    auto consider = [&](const QPointF &c) {
        const qreal dx = c.x() - p.x();
        const qreal dy = c.y() - p.y();
        const qreal d = dx * dx + dy * dy;
        if (d < bestDistance) {
            bestDistance = d;
            best = c;
        }
    };
    if (!flatTop) {
        const qreal colWidth = s_sqrt3 * size;
        const qreal rowHeight = 1.5 * size;
        const int row0 = qRound(p.y() / rowHeight);
        for (int row = row0 - 1; row <= row0 + 1; ++row) {
            const qreal shift = (row & 1) ? colWidth / 2.0 : 0.0;
            const int col0 = qRound((p.x() - shift) / colWidth);
            for (int col = col0 - 1; col <= col0 + 1; ++col) {
                consider(QPointF(col * colWidth + shift, row * rowHeight));
            }
        }
    } else {
        const qreal colWidth = 1.5 * size;
        const qreal rowHeight = s_sqrt3 * size;
        const int col0 = qRound(p.x() / colWidth);
        for (int col = col0 - 1; col <= col0 + 1; ++col) {
            const qreal shift = (col & 1) ? rowHeight / 2.0 : 0.0;
            const int row0 = qRound((p.y() - shift) / rowHeight);
            for (int row = row0 - 1; row <= row0 + 1; ++row) {
                consider(QPointF(col * colWidth, row * rowHeight + shift));
            }
        }
    }
    return best;
}

} // namespace

// the settings: the hex grid and the shared lock (group KrimbleHex, written by View > Show > Hex, Hex Size... and Lock Hex Settings) and this tool's own options (group KrimblePolyTool)
KisToolPolygonStar::Settings KisToolPolygonStar::readSettings()
{
    Settings s;
    KConfigGroup grid = KSharedConfig::openConfig()->group("KrimbleHex");
    KConfigGroup tool = KSharedConfig::openConfig()->group("KrimblePolyTool");
    s.gridSize = qMax(1.0, grid.readEntry("size", 100.0));
    s.gridFlat = grid.readEntry("flat", false);
    s.locked = grid.readEntry("locked", false);
    s.sides = qBound(3, tool.readEntry("sides", 6), 24);
    s.star = tool.readEntry("star", false);
    s.innerPercent = qBound(10, tool.readEntry("inner", 50), 95);
    s.flatTop = tool.readEntry("flat", false);
    s.snap = tool.readEntry("snap", false);
    return s;
}

QPolygonF KisToolPolygonStar::shapeInRect(const QRectF &rect, const Settings &s)
{
    // unit shape: pointed top starts with a corner straight up; flat top turns it half a step so an edge is on top (for a hexagon: corners at 0, 60, ... degrees)
    const int n = s.sides;
    const int count = s.star ? 2 * n : n;
    const qreal start = -M_PI / 2.0 + (s.flatTop ? M_PI / n : 0.0);
    const qreal step = 2.0 * M_PI / count;
    QPolygonF unit;
    qreal minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;
    for (int i = 0; i < count; ++i) {
        const qreal radius = (s.star && (i & 1)) ? s.innerPercent / 100.0 : 1.0;
        const QPointF p(radius * qCos(start + step * i), radius * qSin(start + step * i));
        unit << p;
        minX = qMin(minX, p.x()); maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y()); maxY = qMax(maxY, p.y());
    }
    const qreal bw = qMax(1e-6, maxX - minX);
    const qreal bh = qMax(1e-6, maxY - minY);
    const qreal scale = qMin(rect.width() / bw, rect.height() / bh);
    const QPointF unitCenter((minX + maxX) / 2.0, (minY + maxY) / 2.0);
    const QPointF center = rect.center();
    QPolygonF out;
    Q_FOREACH (const QPointF &p, unit) {
        out << QPointF(center.x() + (p.x() - unitCenter.x()) * scale, center.y() + (p.y() - unitCenter.y()) * scale);
    }
    return out;
}

// The options of the tool: Sides, Star and its inner size, Pointed or Flat top, Snap to the hex grid, and the shared Lock. (The line and fill options are the shape tools' own.)
class KrimblePolyOptionsWidget : public QWidget
{
public:
    explicit KrimblePolyOptionsWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        QFormLayout *form = new QFormLayout;
        m_sides = new QSpinBox(this);
        m_sides->setRange(3, 24);
        m_inner = new QSpinBox(this);
        m_inner->setRange(10, 95);
        m_inner->setSuffix(QStringLiteral(" %"));
        form->addRow(i18n("Sides / points:"), m_sides);
        form->addRow(i18n("Star inner size:"), m_inner);
        layout->addLayout(form);
        m_star = new QCheckBox(i18n("Star"), this);
        m_orientation = new QComboBox(this);
        m_orientation->addItem(i18n("Pointed top"));
        m_orientation->addItem(i18n("Flat top"));
        m_snap = new QCheckBox(i18n("Snap to the hex grid"), this);
        m_lock = new QCheckBox(i18n("Lock hex settings"), this);
        m_note = new QLabel(this);
        m_note->setWordWrap(true);
        layout->addWidget(m_star);
        layout->addWidget(m_orientation);
        layout->addWidget(m_snap);
        layout->addWidget(m_lock);
        layout->addWidget(m_note);
        refresh();
        auto toolWrite = [](const char *key, const QVariant &v) {
            KConfigGroup tool = KSharedConfig::openConfig()->group("KrimblePolyTool");
            tool.writeEntry(key, v);
            tool.sync();
        };
        QObject::connect(m_sides, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, toolWrite](int v) { toolWrite("sides", v); refresh(); });
        QObject::connect(m_inner, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, toolWrite](int v) { toolWrite("inner", v); refresh(); });
        QObject::connect(m_star, &QCheckBox::toggled, this, [this, toolWrite](bool on) { toolWrite("star", on); refresh(); });
        QObject::connect(m_orientation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, toolWrite](int index) { toolWrite("flat", index == 1); refresh(); });
        QObject::connect(m_snap, &QCheckBox::toggled, this, [this, toolWrite](bool on) { toolWrite("snap", on); refresh(); });
        QObject::connect(m_lock, &QCheckBox::toggled, this, [this](bool on) {
            KConfigGroup grid = KSharedConfig::openConfig()->group("KrimbleHex");
            grid.writeEntry("locked", on);
            grid.sync();
            refresh();
        });
    }

    // reads the settings again (the lock may have been changed in View > Show > Lock Hex Settings or in the Hexagon Selection tool)
    void refresh()
    {
        const KisToolPolygonStar::Settings s = KisToolPolygonStar::readSettings();
        QSignalBlocker b1(m_sides);
        QSignalBlocker b2(m_inner);
        QSignalBlocker b3(m_star);
        QSignalBlocker b4(m_orientation);
        QSignalBlocker b5(m_snap);
        QSignalBlocker b6(m_lock);
        m_sides->setValue(s.sides);
        m_inner->setValue(s.innerPercent);
        m_star->setChecked(s.star);
        m_orientation->setCurrentIndex(s.flatTop ? 1 : 0);
        m_snap->setChecked(s.snap);
        m_lock->setChecked(s.locked);
        m_sides->setEnabled(!s.locked);
        m_inner->setEnabled(!s.locked && s.star);
        m_star->setEnabled(!s.locked);
        m_orientation->setEnabled(!s.locked);
        m_snap->setEnabled(!s.locked);
        const QString gridText = i18n("Grid: %1, size %2 px (View > Show > Hex Size...)", s.gridFlat ? i18n("flat top") : i18n("pointed top"), qRound(s.gridSize));
        if (s.locked) {
            m_note->setText(i18n("Locked: no rotation or scaling; the shape, its size and its orientation are fixed. ") + gridText);
        } else if (s.snap) {
            m_note->setText(i18n("The shape sits on the hex cell you tap or drag from. ") + gridText);
        } else {
            m_note->setText(gridText);
        }
    }

private:
    QSpinBox *m_sides {nullptr};
    QSpinBox *m_inner {nullptr};
    QCheckBox *m_star {nullptr};
    QComboBox *m_orientation {nullptr};
    QCheckBox *m_snap {nullptr};
    QCheckBox *m_lock {nullptr};
    QLabel *m_note {nullptr};
};

KisToolPolygonStar::KisToolPolygonStar(KoCanvasBase *canvas)
    : KisToolRectangleBase(canvas, KisToolRectangleBase::PAINT, KisCursor::loadWithSize("tool_ellipse_cursor.svg", 32, 32, 6, 6))
{
    setObjectName("tool_polygon_star");
    setSupportOutline(true);
    setIsOpacityPresetMode(true);

    KisCanvas2 *kritaCanvas = dynamic_cast<KisCanvas2*>(canvas);
    if (kritaCanvas) {
        connect(kritaCanvas->viewManager()->canvasResourceProvider(), SIGNAL(sigEffectiveCompositeOpChanged()), SLOT(resetCursorStyle()));
    }
}

KisToolPolygonStar::~KisToolPolygonStar()
{
}

void KisToolPolygonStar::resetCursorStyle()
{
    KisToolRectangleBase::resetCursorStyle();
    overrideCursorIfNotEditable();
}

QList<QPointer<QWidget> > KisToolPolygonStar::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgetsList = KisToolRectangleBase::createOptionWidgets();
    KrimblePolyOptionsWidget *polyWidget = new KrimblePolyOptionsWidget();
    m_options = polyWidget;
    if (widgetsList.size() > 0 && dynamic_cast<KisOptionCollectionWidget *>(widgetsList.first().data())) {
        KisOptionCollectionWidget *baseOptions = dynamic_cast<KisOptionCollectionWidget *>(widgetsList.first().data());
        KisOptionCollectionWidgetWithHeader *section = new KisOptionCollectionWidgetWithHeader(i18n("Polygon and Star"));
        section->appendWidget("polygonStarWidget", polyWidget);
        baseOptions->appendWidget("sectionPolygonStar", section);
    } else {
        polyWidget->setContentsMargins(10, 10, 10, 10);
        widgetsList.append(polyWidget);
    }
    return widgetsList;
}

void KisToolPolygonStar::activate(const QSet<KoShape*> &shapes)
{
    KisToolRectangleBase::activate(shapes);
    if (KrimblePolyOptionsWidget *w = dynamic_cast<KrimblePolyOptionsWidget*>(m_options.data())) {
        w->refresh();
    }
}

// Locked or snapping: no rotation (the shape sits on the grid as it is).
qreal KisToolPolygonStar::shapeRotationAngle()
{
    return readSettings().fixedSize() ? 0.0 : getRotationAngle();
}

// The box of the shape. Free: the dragged box. Snapping or locked: the box of one hex cell of the grid (the grid's size), centered on the nearest cell when snapping,
// or on the point where the finger went down when only locked.
QRectF KisToolPolygonStar::createRect(const QPointF &start, const QPointF &end)
{
    const Settings s = readSettings();
    if (!s.fixedSize()) {
        return KisToolRectangleBase::createRect(start, end);
    }
    const bool flat = s.gridFlat;
    const qreal radius = s.gridSize;
    const QPointF center = s.snap ? nearestHexCenter(start, radius, flat) : start;
    const qreal w = flat ? 2.0 * radius : s_sqrt3 * radius;
    const qreal h = flat ? s_sqrt3 * radius : 2.0 * radius;
    return QRectF(center.x() - w / 2.0, center.y() - h / 2.0, w, h);
}

void KisToolPolygonStar::paintRectangle(QPainter &gc, const QRectF &imageRect)
{
    KIS_ASSERT_RECOVER_RETURN(canvas());
    const Settings s = readSettings();
    const QPolygonF shapeImage = shapeInRect(imageRect, s);
    QPolygonF shapeView;
    Q_FOREACH (const QPointF &p, shapeImage) {
        shapeView << pixelToView(p);
    }
    QPainterPath path;
    path.addPolygon(shapeView);
    path.closeSubpath();
    getRotatedPath(path, pixelToView(imageRect).center(), shapeRotationAngle());
    path.addPath(drawX(pixelToView(m_dragStart)));
    path.addPath(drawX(pixelToView(m_dragCenter)));
    paintToolOutline(&gc, path);
}

void KisToolPolygonStar::finishRect(const QRectF &rect, qreal roundCornersX, qreal roundCornersY)
{
    Q_UNUSED(roundCornersX);
    Q_UNUSED(roundCornersY);

    if (rect.isEmpty())
        return;

    const Settings s = readSettings();
    const QRectF box = rect.normalized();

    QPainterPath path;
    path.addPolygon(shapeInRect(box, s));
    path.closeSubpath();
    getRotatedPath(path, box.center(), shapeRotationAngle());

    // a path shape in document points; addPathShape() paints it on a paint layer (line and fill from the tool options) or adds it to a vector layer
    KoPathShape *shape = new KoPathShape();
    const QPolygonF polygon = path.toFillPolygon();
    bool first = true;
    for (int i = 0; i < polygon.size(); ++i) {
        // toFillPolygon repeats the first point at the end
        if (i == polygon.size() - 1 && polygon.at(i) == polygon.first()) break;
        const QPointF pt = convertToPt(QRectF(polygon.at(i), QSizeF(1.0, 1.0))).topLeft();
        if (first) { shape->moveTo(pt); first = false; }
        else shape->lineTo(pt);
    }
    shape->close();
    shape->normalize();
    addPathShape(shape, kundo2_i18n(s.star ? "Draw Star" : "Draw Polygon"));
}
