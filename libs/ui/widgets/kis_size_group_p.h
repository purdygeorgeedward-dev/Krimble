/*
 *  SPDX-FileCopyrightText: 2013 Juan Palacios <jpalaciosdev@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISSIZEGROUPPRIVATE_H
#define KISSIZEGROUPPRIVATE_H

#include <QObject>
#include <QWidgetItem>
#include <QList>
#include <QSize>

#include "kis_size_group.h"

class QWidget;
class QEvent;
class QTimer;

class GroupItem;
class KisSizeGroupPrivate : public QObject
{
    Q_OBJECT

public:
    KisSizeGroupPrivate(KisSizeGroup *q_ptr, KisSizeGroup::mode mode, bool ignoreHidden);

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);

    void scheduleSizeUpdate();

    const QSize getSizeHint() const { return m_sizeHint; }

private Q_SLOTS:
    void updateSize();

public:
    KisSizeGroup* q;
    KisSizeGroup::mode m_mode;
    bool m_ignoreHidden;

private:
    QTimer* m_updateTimer;
    QList<GroupItem*> m_groupItems;
    QSize m_sizeHint;
};

class GroupItem : public QObject, public QWidgetItem
{
    Q_OBJECT

public:
    explicit GroupItem(QWidget* widget);
    ~GroupItem() override {}

    void setSize(const QSize &size) { m_size = size; }

    int getWidth() const { return m_size.width(); }
    void setWidth(int width) { m_size.setWidth(width); }

    int getHeight() const { return m_size.height(); }
    void setHeight(int height) { m_size.setHeight(height); }

    bool hidden() const { return m_hidden; }

    KisSizeGroupPrivate* getGroup() { return m_group; }
    void setGroup(KisSizeGroupPrivate* group) { m_group = group; }

    QSize sizeHint() const override;
    QSize minimumSize() const override;

    bool eventFilter(QObject*, QEvent *event) override;

private:
    bool m_hidden {false};
    QSize m_size;
    KisSizeGroupPrivate* m_group {0};
};

#endif
