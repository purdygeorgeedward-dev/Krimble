/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIS_DOCUMENT_SECTION_VIEW_H
#define KIS_DOCUMENT_SECTION_VIEW_H

#include <QTreeView>
#include <QScroller>
#include <optional>

#include "kritalayerdocker_export.h"

class QStyleOptionViewItem;
class KisNodeModel;

class KRITALAYERDOCKER_EXPORT NodeView : public QTreeView
{
    Q_OBJECT
Q_SIGNALS:
    void contextMenuRequested(const QPoint &globalPos, const QModelIndex &index);
    void selectionChanged(const QModelIndexList &);
public:

    enum ColumnIndex {
        DEFAULT_COL = 0,
        VISIBILITY_COL = 1,
        SELECTED_COL = 2,
    };

    explicit NodeView(QWidget *parent = 0);
    ~NodeView() override;

    void setModel(QAbstractItemModel *model) override;
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent (QPaintEvent *event) override;
    void drawBranches(QPainter *painter, const QRect &rect,
                              const QModelIndex &index) const override;

    void dropEvent(QDropEvent *ev) override;

    void dragEnterEvent(QDragEnterEvent *e) override;

    void dragMoveEvent(QDragMoveEvent *ev) override;

    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

    void addPropertyActions(QMenu *menu, const QModelIndex &index);

    void updateNode(const QModelIndex &index);

    void toggleSolo(const QModelIndex &index);

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const override;
    QModelIndex indexAt(const QPoint &point) const override;
    bool viewportEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void showContextMenu(const QPoint &globalPos, const QModelIndex &index);
    void startDrag (Qt::DropActions supportedActions) override;
    QPixmap createDragPixmap() const;

    int cursorPageIndex() const;

public Q_SLOTS:
    void slotUpdateIcons();
    void slotScrollerStateChanged(QScroller::State state);
    void slotConfigurationChanged();

protected Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private Q_SLOTS:
    void slotActionToggled(bool on, const QPersistentModelIndex &index, int property);

private:

    bool isDragging() const;

    void setDraggingFlag(bool flag = true);

    void updateSelectedCheckboxColumn();

    std::optional<QModelIndex> getActiveItem();
    void setExclusiveActiveItem(QModelIndex index);

    bool m_draggingFlag;

    QStyleOptionViewItem optionForIndex(const QModelIndex &index) const;
    typedef KisNodeModel Model;
    class PropertyAction;
    class Private;
    Private* const d;
};

#endif
