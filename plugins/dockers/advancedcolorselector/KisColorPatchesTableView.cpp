/*
 * SPDX-FileCopyrightText: 2023 Sharaf Zaman <shzam@sdf.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KisColorPatchesTableView.h"

#include <QCoreApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QMouseEvent>

#include <utility>

#include "kis_color_patches.h"

class KisColorPatchesTableDelegate : public QStyledItemDelegate
{
public:
    KisColorPatchesTableDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

void KisColorPatchesTableDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    const QColor color = index.data(Qt::UserRole).value<QColor>();

    if (color.isValid()) {
        painter->fillRect(option.rect, color);
    }
}

struct KisColorPatchesTableView::Private {
    QScopedPointer<QStandardItemModel> model;
    QList<KoColor> colorPatches;
    const QString configPrefix;
    KisColorPatches::Direction direction{KisColorPatches::Horizontal};
    int numRows {0};
    int numCols {0};
    int patchWidth {1};
    int patchHeight {1};
    int patchCount {1};
    bool wasScrollingDisabled {false};

    Private(QString configPrefix)
        : configPrefix(std::move(configPrefix))
    {
    }
};

KisColorPatchesTableView::KisColorPatchesTableView(const QString &configPrefix, QWidget *parent)
    : QTableView(parent)
    , m_d(new KisColorPatchesTableView::Private(configPrefix))
{
    setShowGrid(false);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::NoSelection);
    setStyleSheet("QTableView{ border: 0px}");
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    reloadWidgetConfig();

    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(this);
    if (scroller) {
        QScrollerProperties props;
        props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                              QScrollerProperties::OvershootAlwaysOff);
        props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                              QScrollerProperties::OvershootAlwaysOff);
        scroller->setScrollerProperties(props);
        connect(scroller,
                SIGNAL(stateChanged(QScroller::State)),
                this,
                SLOT(slotScrollerStateChanged(QScroller::State)));
    }
    reloadWidgetConfig();
}

KisColorPatchesTableView::~KisColorPatchesTableView()
{
}

void KisColorPatchesTableView::setColors(const QList<KoColor> &colors)
{
    m_d->colorPatches.clear();
    m_d->colorPatches = colors;
    redraw();
}

QList<KoColor> KisColorPatchesTableView::colors() const
{
    return m_d->colorPatches;
}

void KisColorPatchesTableView::reloadWidgetConfig()
{
    const KConfigGroup cfg = KSharedConfig::openConfig()->group("advancedColorSelector");

    m_d->numCols = cfg.readEntry(m_d->configPrefix + "NumCols", 1);
    m_d->numRows = cfg.readEntry(m_d->configPrefix + "NumRows", 1);

    m_d->patchWidth = cfg.readEntry(m_d->configPrefix + "Width", 20);
    m_d->patchHeight = cfg.readEntry(m_d->configPrefix + "Height", 20);
    if (cfg.readEntry(m_d->configPrefix + "Alignment", false)) {
        m_d->direction = KisColorPatches::Vertical;
    } else {
        m_d->direction = KisColorPatches::Horizontal;
    }
    m_d->patchCount = cfg.readEntry(m_d->configPrefix + "Count", 15);
    const bool allowScrolling = cfg.readEntry(m_d->configPrefix + "Scrolling", true);
    if (!allowScrolling) {
        QScroller::scroller(this)->ungrabGesture(this);
        horizontalScrollBar()->setEnabled(false);
        verticalScrollBar()->setEnabled(false);
        m_d->wasScrollingDisabled = true;
    } else if (m_d->wasScrollingDisabled) {
        QScroller *scroller = QScroller::scroller(this);
        scroller->grabGesture(this, KisKineticScroller::getConfiguredGestureType());
        horizontalScrollBar()->setEnabled(true);
        verticalScrollBar()->setEnabled(true);
        m_d->wasScrollingDisabled = false;
    }

    m_d->model.reset(new QStandardItemModel(m_d->numRows, m_d->numCols, this));

    if (m_d->direction == KisColorPatches::Vertical) {
        setMinimumWidth((m_d->patchWidth + 0) * m_d->numCols);
        setMaximumWidth((m_d->patchWidth + 0) * m_d->numCols);

        setMinimumHeight(m_d->patchHeight);
        setMaximumHeight(QWIDGETSIZE_MAX);

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    } else {
        setMinimumHeight((m_d->patchHeight + 0) * m_d->numRows);
        setMaximumHeight((m_d->patchHeight + 0) * m_d->numRows);

        setMinimumWidth(m_d->patchWidth);
        setMaximumWidth(QWIDGETSIZE_MAX);

        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    }

    verticalHeader()->setMinimumSectionSize(m_d->patchHeight);
    verticalHeader()->setMaximumSectionSize(m_d->patchHeight);
    verticalHeader()->setDefaultSectionSize(m_d->patchHeight);
    horizontalHeader()->setMinimumSectionSize(m_d->patchWidth);
    horizontalHeader()->setMaximumSectionSize(m_d->patchWidth);
    horizontalHeader()->setDefaultSectionSize(m_d->patchWidth);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    verticalHeader()->hide();
    horizontalHeader()->hide();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setModel(m_d->model.data());
    setItemDelegate(new KisColorPatchesTableDelegate(this));

    redraw();
}

QSize KisColorPatchesTableView::cellSize() const
{
    return QSize(m_d->patchWidth, m_d->patchHeight);
}

int KisColorPatchesTableView::patchCount() const
{
    return m_d->patchCount;
}

void KisColorPatchesTableView::addColorPatch(const KoColor &color)
{
    m_d->colorPatches.removeAll(color);
    m_d->colorPatches.prepend(color);

    if (m_d->colorPatches.size() > 200) {
        m_d->colorPatches.pop_back();
    }

    redraw();
}

void KisColorPatchesTableView::wheelEvent(QWheelEvent *event)
{
    if (m_d->direction == KisColorPatches::Horizontal) {
        bool horizontal = qAbs(event->angleDelta().x()) > qAbs(event->angleDelta().y());
        if (!horizontal) {
            QPoint modifiedAngleDelta = {event->angleDelta().y(), event->angleDelta().x()};
            QWheelEvent modifiedEvent(event->position(),
                                      event->globalPosition(),
                                      event->pixelDelta(),
                                      modifiedAngleDelta,
                                      event->buttons(),
                                      event->modifiers(),
                                      event->phase(),
                                      event->inverted());
            horizontalScrollBar()->event(&modifiedEvent);
            event->accept();
            return;
        }
    }
    QTableView::wheelEvent(event);
}

void KisColorPatchesTableView::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void KisColorPatchesTableView::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

void KisColorPatchesTableView::redraw()
{
    m_d->model->clear();

    if (m_d->colorPatches.isEmpty()) {
        return;
    }

    int linearIndex = 0;
    const int limit = ((m_d->direction == KisColorPatches::Vertical) ? m_d->numCols : m_d->numRows);
    int pointer = 0;
    int index = 1 % limit;

    Q_FOREACH (const KoColor &color, m_d->colorPatches) {
        if (linearIndex > m_d->patchCount) {
            break;
        }
        if (index == 0) {
            pointer++;
            if (m_d->direction == KisColorPatches::Vertical) {
                m_d->model->insertRow(pointer);
            } else {
                m_d->model->insertColumn(pointer);
            }
        }
        QStandardItem *item = new QStandardItem;
        item->setData(QVariant(), Qt::DisplayRole);
        item->setData(color.toQColor(), Qt::UserRole);
        if (m_d->direction == KisColorPatches::Vertical) {
            m_d->model->setItem(pointer, index, item);
        } else {
            m_d->model->setItem(index, pointer, item);
        }
        index = (index + 1) % limit;
        linearIndex++;
    }
}

boost::optional<KoColor> KisColorPatchesTableView::colorPatchAt(const QPoint &pos) const
{
    const QModelIndex index = indexAt(mapFromGlobal(pos));
    if (!index.isValid()) {
        return boost::none;
    }
    int linearIndex = 0;
    if (m_d->direction == KisColorPatches::Vertical) {
        linearIndex = index.row() * m_d->model->columnCount() + index.column();
    } else {
        linearIndex = index.column() * m_d->model->rowCount() + index.row();
    }
    linearIndex -= 1;

    if (linearIndex < 0 || linearIndex >= m_d->colorPatches.size()) {
        return boost::none;
    }
    return m_d->colorPatches[linearIndex];
}
