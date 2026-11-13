/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Krimble 2026-11-12 (task 42): George: "It would be nice to have hex selection snap to hex grid." and "lock the settings so it doesn't rotate or scale or change line thickness or the hex grid until you unlock it."
 */

#include "kis_tool_select_hexagon.h"

#include <QtMath>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <limits>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KisOptionCollectionWidget.h>

#include "kis_painter.h"
#include <brushengine/kis_paintop_registry.h>
#include "kis_selection_options.h"
#include "kis_canvas2.h"
#include "kis_pixel_selection.h"
#include "kis_selection_tool_helper.h"
#include <kis_default_bounds.h>

#include "KisViewManager.h"
#include "kis_selection_manager.h"
#include <kis_command_utils.h>
#include <kis_selection_filters.h>

namespace {

const qreal s_sqrt3 = 1.7320508075688772;

// The center of the hex cell that is nearest to a point, on the same lattice the hex overlay of View > Show > Hex draws (pointy-top: columns sqrt(3) x size apart, rows 1.5 x size
// apart, every second row shifted half a column; flat-top: the same turned a quarter turn). size = center to corner, in image pixels.
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

// the settings: the hex grid (group KrimbleHex, written by View > Show > Hex and Hex Size...) and this tool's own options (group KrimbleHexTool)
__KisToolSelectHexagonLocal::HexSettings __KisToolSelectHexagonLocal::readHexSettings()
{
    HexSettings s;
    KConfigGroup grid = KSharedConfig::openConfig()->group("KrimbleHex");
    KConfigGroup tool = KSharedConfig::openConfig()->group("KrimbleHexTool");
    s.gridSize = qMax(1.0, grid.readEntry("size", 100.0));
    s.gridFlat = grid.readEntry("flat", false);
    s.locked = grid.readEntry("locked", false);
    s.flatTop = tool.readEntry("flat", false);
    s.snap = tool.readEntry("snap", false);
    return s;
}

QPolygonF __KisToolSelectHexagonLocal::hexagonInRect(const QRectF &rect, bool flatTop)
{
    // the largest regular hexagon in the box: flat top is 2 R wide and sqrt(3) R high, pointed top is sqrt(3) R wide and 2 R high
    const qreal radius = flatTop ? qMin(rect.width() / 2.0, rect.height() / s_sqrt3)
                                 : qMin(rect.width() / s_sqrt3, rect.height() / 2.0);
    const QPointF center = rect.center();
    QPolygonF polygon;
    for (int i = 0; i < 6; ++i) {
        const qreal angle = qDegreesToRadians((flatTop ? 0.0 : 30.0) + 60.0 * i);
        polygon << QPointF(center.x() + radius * qCos(angle), center.y() + radius * qSin(angle));
    }
    return polygon;
}

// The options of the tool: the orientation (Pointed top or Flat top), Snap to the hex grid, and the Lock.
class KrimbleHexOptionsWidget : public QWidget
{
public:
    explicit KrimbleHexOptionsWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        m_orientation = new QComboBox(this);
        m_orientation->addItem(i18n("Pointed top"));
        m_orientation->addItem(i18n("Flat top"));
        m_snap = new QCheckBox(i18n("Snap to the hex grid"), this);
        m_lock = new QCheckBox(i18n("Lock hex settings"), this);
        m_note = new QLabel(this);
        m_note->setWordWrap(true);
        layout->addWidget(m_orientation);
        layout->addWidget(m_snap);
        layout->addWidget(m_lock);
        layout->addWidget(m_note);
        refresh();
        QObject::connect(m_orientation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
            KConfigGroup tool = KSharedConfig::openConfig()->group("KrimbleHexTool");
            tool.writeEntry("flat", index == 1);
            tool.sync();
            refresh();
        });
        QObject::connect(m_snap, &QCheckBox::toggled, this, [this](bool on) {
            KConfigGroup tool = KSharedConfig::openConfig()->group("KrimbleHexTool");
            tool.writeEntry("snap", on);
            tool.sync();
            refresh();
        });
        QObject::connect(m_lock, &QCheckBox::toggled, this, [this](bool on) {
            KConfigGroup grid = KSharedConfig::openConfig()->group("KrimbleHex");
            grid.writeEntry("locked", on);
            grid.sync();
            refresh();
        });
    }

    // reads the settings again (the lock may have been changed in View > Show > Lock Hex Settings, or by the other hex tool)
    void refresh()
    {
        const __KisToolSelectHexagonLocal::HexSettings s = __KisToolSelectHexagonLocal::readHexSettings();
        QSignalBlocker b1(m_orientation);
        QSignalBlocker b2(m_snap);
        QSignalBlocker b3(m_lock);
        m_orientation->setCurrentIndex(s.flatTop ? 1 : 0);
        m_snap->setChecked(s.snap);
        m_lock->setChecked(s.locked);
        m_orientation->setEnabled(!s.locked && !s.snap);   // with snapping the hexagon is the grid's own, so the orientation is the grid's
        m_snap->setEnabled(!s.locked);
        const QString gridText = i18n("Grid: %1, size %2 px (View > Show > Hex Size...)", s.gridFlat ? i18n("flat top") : i18n("pointed top"), qRound(s.gridSize));
        if (s.locked) {
            m_note->setText(i18n("Locked: no rotation or scaling; the size and orientation are fixed. ") + gridText);
        } else if (s.snap) {
            m_note->setText(i18n("The hexagon fills the grid cell you tap or drag from. ") + gridText);
        } else {
            m_note->setText(gridText);
        }
    }

private:
    QComboBox *m_orientation {nullptr};
    QCheckBox *m_snap {nullptr};
    QCheckBox *m_lock {nullptr};
    QLabel *m_note {nullptr};
};

__KisToolSelectHexagonLocal::__KisToolSelectHexagonLocal(KoCanvasBase * canvas)
    : KisToolRectangleBase(canvas, KisToolRectangleBase::SELECT,
                           KisCursor::loadWithSize("tool_rectangular_selection_cursor.svg", 32, 32, 6, 6))
{
    setObjectName("tool_select_hexagon");
}

QList<QPointer<QWidget> > __KisToolSelectHexagonLocal::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgetsList = KisToolRectangleBase::createOptionWidgets();
    KrimbleHexOptionsWidget *hexWidget = new KrimbleHexOptionsWidget();
    m_hexOptions = hexWidget;
    if (widgetsList.size() > 0 && dynamic_cast<KisOptionCollectionWidget *>(widgetsList.first().data())) {
        KisOptionCollectionWidget *baseOptions = dynamic_cast<KisOptionCollectionWidget *>(widgetsList.first().data());
        KisOptionCollectionWidgetWithHeader *section = new KisOptionCollectionWidgetWithHeader(i18n("Hexagon"));
        section->appendWidget("hexagonWidget", hexWidget);
        baseOptions->appendWidget("sectionHexagon", section);
    } else {
        hexWidget->setContentsMargins(10, 10, 10, 10);
        widgetsList.append(hexWidget);
    }
    return widgetsList;
}

void __KisToolSelectHexagonLocal::activate(const QSet<KoShape*> &shapes)
{
    KisToolRectangleBase::activate(shapes);
    if (KrimbleHexOptionsWidget *w = dynamic_cast<KrimbleHexOptionsWidget*>(m_hexOptions.data())) {
        w->refresh();
    }
}

// Locked or snapping: no rotation (the hexagon sits on the grid as it is).
qreal __KisToolSelectHexagonLocal::hexRotationAngle()
{
    return readHexSettings().fixedSize() ? 0.0 : getRotationAngle();
}

// The box of the hexagon. Free: the dragged box (with the tool's rotation). Snapping or locked: the box of one hex cell of the grid (the grid's size), centered on the nearest cell
// when snapping, or on the point where the finger went down when only locked.
QRectF __KisToolSelectHexagonLocal::createRect(const QPointF &start, const QPointF &end)
{
    const HexSettings s = readHexSettings();
    if (!s.fixedSize()) {
        return KisToolRectangleBase::createRect(start, end);
    }
    const bool flat = s.effectiveFlat();
    const qreal radius = s.gridSize;
    const QPointF center = s.snap ? nearestHexCenter(start, radius, flat) : start;
    const qreal w = flat ? 2.0 * radius : s_sqrt3 * radius;
    const qreal h = flat ? s_sqrt3 * radius : 2.0 * radius;
    return QRectF(center.x() - w / 2.0, center.y() - h / 2.0, w, h);
}

void __KisToolSelectHexagonLocal::paintRectangle(QPainter &gc, const QRectF &imageRect)
{
    KIS_ASSERT_RECOVER_RETURN(canvas());
    const HexSettings s = readHexSettings();
    const QPolygonF hexImage = hexagonInRect(imageRect, s.effectiveFlat());
    QPolygonF hexView;
    Q_FOREACH (const QPointF &p, hexImage) {
        hexView << pixelToView(p);
    }
    QPainterPath path;
    path.addPolygon(hexView);
    path.closeSubpath();
    getRotatedPath(path, pixelToView(imageRect).center(), hexRotationAngle());
    path.addPath(drawX(pixelToView(m_dragStart)));
    path.addPath(drawX(pixelToView(m_dragCenter)));
    paintToolOutline(&gc, path);
}


KisToolSelectHexagon::KisToolSelectHexagon(KoCanvasBase *canvas):
    KisToolSelectBase<__KisToolSelectHexagonLocal>(canvas, i18n("Hexagon Selection"))
{}

void KisToolSelectHexagon::finishRect(const QRectF& rect, qreal roundCornersX, qreal roundCornersY)
{
    Q_UNUSED(roundCornersX);
    Q_UNUSED(roundCornersY);
    KisCanvas2 * kisCanvas = dynamic_cast<KisCanvas2*>(canvas());
    if (!kisCanvas)
        return;

    KisSelectionToolHelper helper(kisCanvas, kundo2_i18n("Select Hexagon"));

    QRect rc(rect.normalized().toRect());

    if (helper.tryDeselectCurrentSelection(pixelToView(rc), selectionAction())) {
        return;
    }

    if (helper.canShortcutToNoop(rc, selectionAction())) {
        return;
    }

    if (!rc.isValid()) {
        return;
    }

    // always a pixel selection (the hexagon is not a vector shape)
    KisProcessingApplicator applicator(currentImage(),
                                       currentNode(),
                                       KisProcessingApplicator::NONE,
                                       KisImageSignalVector(),
                                       kundo2_i18n("Select Hexagon"));

    KisPixelSelectionSP tmpSel =
        new KisPixelSelection(new KisDefaultBounds(currentImage()));

    const bool antiAlias = antiAliasSelection();
    const int grow = growSelection();
    const int feather = featherSelection();

    QPainterPath path;
    path.addPolygon(hexagonInRect(rect.normalized(), readHexSettings().effectiveFlat()));
    path.closeSubpath();
    getRotatedPath(path, rect.normalized().center(), hexRotationAngle());

    KUndo2Command *cmd = new KisCommandUtils::LambdaCommand(
        [tmpSel, antiAlias, grow, feather, path]() mutable
        -> KUndo2Command * {
            KisPainter painter(tmpSel);
            painter.setPaintColor(KoColor(Qt::black, tmpSel->colorSpace()));
            painter.setAntiAliasPolygonFill(antiAlias && feather == 0);
            painter.setFillStyle(KisPainter::FillStyleForegroundColor);
            painter.setStrokeStyle(KisPainter::StrokeStyleNone);

            painter.paintPainterPath(path);

            if (grow > 0) {
                KisGrowSelectionFilter biggy(grow, grow);
                biggy.process(tmpSel,
                              tmpSel->selectedRect().adjusted(-grow, -grow, grow, grow));
            } else if (grow < 0) {
                KisShrinkSelectionFilter tiny(-grow, -grow, false);
                tiny.process(tmpSel, tmpSel->selectedRect());
            }
            if (feather > 0) {
                KisFeatherSelectionFilter feathery(feather);
                feathery.process(tmpSel,
                                 tmpSel->selectedRect().adjusted(-feather, -feather, feather, feather));
            }

            if (grow == 0 && feather == 0) {
                tmpSel->setOutlineCache(path);
            } else {
                tmpSel->invalidateOutlineCache();
            }

            return 0;
        });

    applicator.applyCommand(cmd, KisStrokeJobData::SEQUENTIAL);
    helper.selectPixelSelection(applicator, tmpSel, selectionAction());
    applicator.end();
}

void KisToolSelectHexagon::beginShape()
{
    beginSelectInteraction();
}

void KisToolSelectHexagon::endShape()
{
    endSelectInteraction();
}

void KisToolSelectHexagon::resetCursorStyle()
{
    if (selectionAction() == SELECTION_ADD) {
        useCursor(KisCursor::loadWithSize("tool_rectangular_selection_cursor_add.svg", 32, 32, 6, 6));
    } else if (selectionAction() == SELECTION_SUBTRACT) {
        useCursor(KisCursor::loadWithSize("tool_rectangular_selection_cursor_sub.svg", 32, 32, 6, 6));
    } else if (selectionAction() == SELECTION_INTERSECT) {
        useCursor(KisCursor::loadWithSize("tool_rectangular_selection_cursor_inter.svg", 32, 32, 6, 6));
    } else if (selectionAction() == SELECTION_SYMMETRICDIFFERENCE) {
        useCursor(KisCursor::loadWithSize("tool_rectangular_selection_cursor_symdiff.svg", 32, 32, 6, 6));
    } else {
        KisToolSelectBase<__KisToolSelectHexagonLocal>::resetCursorStyle();
    }
}
