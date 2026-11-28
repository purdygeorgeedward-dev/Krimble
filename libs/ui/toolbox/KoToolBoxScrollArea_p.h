/*
 * SPDX-FileCopyrightText: 2018 Alvin Wong <alvinhochun@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_TOOLBOX_SCROLL_AREA_H
#define KO_TOOLBOX_SCROLL_AREA_H

#include "KoToolBox_p.h"
#include "KoToolBoxLayout_p.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QStyleOption>
#include <QToolButton>
#include <KisKineticScroller.h>

class KoToolBoxScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    KoToolBoxScrollArea(KoToolBox *toolBox, QWidget *parent)
        : QScrollArea(parent)
        , m_toolBox(toolBox)
        , m_orientation(Qt::Vertical)
        , m_scrollPrev(new QToolButton(this))
        , m_scrollNext(new QToolButton(this))
    {
        setFrameShape(QFrame::NoFrame);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_toolBox->setOrientation(m_orientation);
        setWidget(m_toolBox);

        m_scrollPrev->setAutoRepeat(true);
        m_scrollPrev->setAutoFillBackground(true);
        m_scrollPrev->setFocusPolicy(Qt::NoFocus);
        connect(m_scrollPrev, &QToolButton::clicked, this, &KoToolBoxScrollArea::doScrollPrev);
        m_scrollNext->setAutoRepeat(true);
        m_scrollNext->setAutoFillBackground(true);
        m_scrollNext->setFocusPolicy(Qt::NoFocus);
        connect(m_scrollNext, &QToolButton::clicked, this, &KoToolBoxScrollArea::doScrollNext);
        // These are for filtering the mouse wheel events:
        m_scrollPrev->installEventFilter(this);
        m_scrollNext->installEventFilter(this);

        m_scrollPrev->hide();
        m_scrollNext->hide();
    }

    void setOrientation(Qt::Orientation orientation)
    {
        m_orientation = orientation;
        m_toolBox->setOrientation(orientation);
        layoutItems();
    }

    void setCompact(bool state) {
        m_toolBox->setCompact(state);
        layoutItems();
    }

    Qt::Orientation orientation() const
    {
        return m_orientation;
    }

    QSize minimumSizeHint() const override
    {
        return m_toolBox->minimumSizeHint();
    }

    QSize sizeHint() const override
    {
        return m_toolBox->sizeHint();
    }

public Q_SLOTS:

protected:
    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::LayoutRequest) {
            // LayoutRequest can be triggered by icon changes, so resize the toolbox
            layoutItems();
            // The toolbox might have changed the sizeHint and minimumSizeHint
            updateGeometry();
        }
        return QScrollArea::event(event);
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        // The toolbuttons eat the wheel events, so we filter them for our use.
        if ((watched == m_scrollPrev || watched == m_scrollNext) && event->type() == QEvent::Wheel) {
            wheelEvent(static_cast<QWheelEvent *>(event));
            return true;
        }
        return QScrollArea::eventFilter(watched, event);
    }

    void resizeEvent(QResizeEvent *event) override
    {
        layoutItems();
        QScrollArea::resizeEvent(event);
        updateScrollButtons();
    }

    void wheelEvent(QWheelEvent *event) override
    {
        event->ignore();
    }

    void scrollContentsBy(int dx, int dy) override
    {
        QScrollArea::scrollContentsBy(dx, dy);
        updateScrollButtons();
    }

private Q_SLOTS:
    void doScrollPrev()
    {
        if (m_orientation == Qt::Vertical) {
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
        } else {
            horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
        }
    }

    void doScrollNext()
    {
        if (m_orientation == Qt::Vertical) {
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
        } else {
            horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
        }
    }

private:
    void layoutItems()
    {
        const KoToolBoxLayout *l = m_toolBox->toolBoxLayout();
        QSize newSize = viewport()->size();
        if (m_orientation == Qt::Vertical) {
            newSize.setHeight(l->heightForWidth(newSize.width()));
        } else {
            newSize.setWidth(l->widthForHeight(newSize.height()));
        }
        m_toolBox->resize(newSize);

        updateScrollButtons();
    }

    void updateScrollButtons()
    {
        m_scrollPrev->hide();
        m_scrollNext->hide();
    }

    KoToolBox *m_toolBox;
    Qt::Orientation m_orientation;

    QToolButton *m_scrollPrev;
    QToolButton *m_scrollNext;
};

#endif // KO_TOOLBOX_SCROLL_AREA_H
