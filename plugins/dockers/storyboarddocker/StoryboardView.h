/*
  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STORYBOARD_VIEW_H
#define STORYBOARD_VIEW_H

#include <QListView>

#include <kritastoryboarddocker_export.h>

class QStyleOptionViewItem;
class StoryboardModel;

class KRITASTORYBOARDDOCKER_EXPORT StoryboardView : public QListView
{
    Q_OBJECT
public:
    explicit StoryboardView(QWidget *parent = 0);
    ~StoryboardView() override;

    void paintEvent(QPaintEvent *event) override;
    QRect visualRect(const QModelIndex &index) const override;
    QModelIndex indexAt(const QPoint &point) const override;
    void setItemOrientation(Qt::Orientation orientation);

    Qt::Orientation itemOrientation();

    bool commentIsVisible() const;

    bool thumbnailIsVisible() const;

    void setCommentVisibility(bool value);

    void setThumbnailVisibility(bool value);

    void setCurrentItem(int frame);

    void mouseReleaseEvent(QMouseEvent *event) override;

    QSize sizeHint() const override;

private Q_SLOTS:
    void slotContextMenuRequested(const QPoint &);

    void slotItemClicked(const QModelIndex &clicked);

private:
    Qt::Orientation m_itemOrientation;
    bool m_commentIsVisible;
    bool m_thumbnailIsVisible;
};

#endif
