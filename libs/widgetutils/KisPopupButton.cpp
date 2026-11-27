/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisPopupButton.h"

#include <QPointer>
#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QScreen>
#include <QStyleOption>
#include <QStylePainter>
#include <QWindow>
#include <QMenu>
#include <QScrollArea>
#include <QScroller>
#include "KisKineticScroller.h"

#include "kis_global.h"
#include <kis_debug.h>

class KisPopupButtonFrame : public QFrame
{
    QHBoxLayout* frameLayout {0};

public:
    KisPopupButtonFrame(QWidget *parent, bool detach, bool popupIsMenu)
        : QFrame(parent)
    {
        setObjectName("KisPopupButtonFrame");
        setProperty("_kis_excludeFromLayoutThumbnail", true);
        frameLayout = new QHBoxLayout(this);
        frameLayout->setContentsMargins(0, 0, 0, 0);

        setDetached(detach, popupIsMenu);
    }

    void setDetached(bool detach, bool popupIsMenu)
    {
#if defined Q_OS_ANDROID || defined Q_OS_MACOS
        hide();
#endif

        destroy();

        if (detach) {
            setWindowFlags(Qt::Dialog);
            setFrameStyle(QFrame::NoFrame);
        }
        else {
            setWindowFlags(Qt::Popup);
            if (popupIsMenu) {
                setFrameStyle(QFrame::NoFrame);
            } else {
                setFrameStyle(QFrame::Box | QFrame::Plain);
            }
        }

        updateGeometry();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->matches(QKeySequence::Cancel)) {
            event->accept();
            hide();
        } else {
            QFrame::keyPressEvent(event);
        }
    }

    bool event(QEvent *e) override
    {
        if (e->type() == QEvent::Close) {
            e->ignore();
            hide();
            return true;
        }
        return QFrame::event(e);
    }
};

struct KisPopupButton::Private {
    Private()
    {}
    QPointer<KisPopupButtonFrame> frame;
    QPointer<QWidget> popupWidget;
    bool arrowVisible { true };
    bool isPopupDetached { false };
    bool isDetachedGeometrySet { false };
};

KisPopupButton::KisPopupButton(QWidget* parent)
        : QToolButton(parent)
        , m_d(new Private)
{
    setObjectName("KisPopupButton");
    connect(this, SIGNAL(released()), SLOT(showPopupWidget()));
}

KisPopupButton::~KisPopupButton()
{
    delete m_d->frame;
    delete m_d;
}

void KisPopupButton::setPopupWidgetDetached(bool detach)
{
    m_d->isPopupDetached = detach;
    if (m_d->frame) {
        bool wasVisible = isPopupWidgetVisible();
        m_d->frame->setDetached(detach, qobject_cast<QMenu *>(m_d->popupWidget));
        if (wasVisible) {
            setPopupWidgetVisible(true);
            if (detach) {
                m_d->isDetachedGeometrySet = true;
            }
            adjustPosition();
        }
    }
}

void KisPopupButton::setPopupWidget(QWidget* widget)
{
    if (widget) {
        delete m_d->frame;

        QMenu *menu = qobject_cast<QMenu *>(widget);
        m_d->frame = new KisPopupButtonFrame(this->window(), m_d->isPopupDetached, menu);
        m_d->frame->setWindowTitle(widget->windowTitle());

        m_d->popupWidget = widget;

#ifdef Q_OS_ANDROID
        if (!menu) {
            QScrollArea *scroll = new QScrollArea(m_d->frame);
            scroll->setFrameShape(QFrame::NoFrame);
            scroll->setWidgetResizable(true);
            scroll->setWidget(m_d->popupWidget);
            QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(scroll);
            Q_UNUSED(scroller);
            m_d->frame->layout()->addWidget(scroll);
        } else {
            m_d->frame->layout()->addWidget(m_d->popupWidget);
        }
#else
        m_d->frame->layout()->addWidget(m_d->popupWidget);
#endif

        if (menu) {
            connect(menu, &QMenu::aboutToHide, this, &KisPopupButton::slotMenuAboutToHide);
            connect(this, &KisPopupButton::sigPopupWidgetShown, menu, &QMenu::show);
        }
    }
}

void KisPopupButton::setPopupWidgetWidth(int w)
{
    m_d->frame->resize(w, m_d->frame->height());
}

void KisPopupButton::showPopupWidget()
{
    if (m_d->popupWidget && !m_d->frame->isVisible()) {
        setPopupWidgetVisible(true);
        if (!m_d->isPopupDetached) {
            adjustPosition();
        }
    } else {
        hidePopupWidget();
    }
}

void KisPopupButton::hidePopupWidget()
{
    setPopupWidgetVisible(false);
}

void KisPopupButton::setPopupWidgetVisible(bool visible)
{
    if (m_d->popupWidget) {
        if (visible) {
            if (m_d->isPopupDetached) {
                m_d->popupWidget->adjustSize();
                m_d->frame->adjustSize();
                adjustPosition();
            }
            m_d->frame->raise();
            m_d->frame->show();
            m_d->frame->activateWindow();
            Q_EMIT sigPopupWidgetShown();
        } else {
            m_d->frame->setVisible(false);
        }
    }
}

bool KisPopupButton::isPopupWidgetVisible()
{
    return m_d->popupWidget && m_d->frame->isVisible();
}

void KisPopupButton::paintEvent ( QPaintEvent * event  )
{
    QToolButton::paintEvent(event);
    if (m_d->arrowVisible) {
        paintPopupArrow();
    }
}

void KisPopupButton::paintPopupArrow()
{
    QStylePainter p(this);
    QStyleOption option;
    option.rect = QRect(rect().right() - 15, rect().bottom() - 15, 14, 14);
    option.palette = palette();
    option.palette.setBrush(QPalette::ButtonText, Qt::black);
    option.state = QStyle::State_Enabled;
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawPrimitive(QStyle::PE_IndicatorArrowDown, option);
}

void KisPopupButton::adjustPosition()
{
    QScreen *currentScreen = nullptr;
    auto getCurrentScreen = [this, &currentScreen] {
        if (!currentScreen) {
            QWindow *mainWinHandle = window()->windowHandle();
            if (mainWinHandle) {
                currentScreen = mainWinHandle->screen();
            } else {
                currentScreen = QApplication::primaryScreen();
            }
        }
        return currentScreen;
    };

    if (!m_d->isPopupDetached || !m_d->isDetachedGeometrySet) {
        QWindow *winHandle = m_d->frame->windowHandle();
        if (winHandle) {
            winHandle->setScreen(getCurrentScreen());
        }
    }

    QPoint pos = this->isVisible() ? this->mapToGlobal(QPoint(0, this->size().height())) : QCursor().pos();
    QSize popSize = m_d->popupWidget->size();
    QRect popupRect(pos, popSize);

    QScreen *screen = [this, &getCurrentScreen]() {
        QWindow *winHandle = m_d->frame->windowHandle();
        if (winHandle && winHandle->screen()) {
            return winHandle->screen();
        }
        return getCurrentScreen();
    }();
    QRect screenRect = screen->availableGeometry();
#ifdef Q_OS_ANDROID
    if (m_d->popupWidget && !qobject_cast<QMenu *>(m_d->popupWidget)) {
        const bool landscape = screenRect.width() > screenRect.height();
        const QSize maxSize(int(screenRect.width() * (landscape ? 0.60 : 0.92)),
                            int(screenRect.height() * (landscape ? 0.85 : 0.65)));
        popSize = m_d->frame->sizeHint().boundedTo(maxSize);
        popupRect.setSize(popSize);
    }
#endif
    if (m_d->isPopupDetached) {
        if (m_d->isDetachedGeometrySet) {
            popupRect.moveTo(m_d->frame->geometry().topLeft());
        } else {
            popupRect.moveTo(this->window()->geometry().center() - QRect(QPoint(0, 0), popSize).center());
            m_d->isDetachedGeometrySet = true;
        }
    }
    popupRect = kisEnsureInRect(popupRect, screenRect);

    m_d->frame->setGeometry(popupRect);
}

void KisPopupButton::setArrowVisible (bool v)
{
    if (v) {
        m_d->arrowVisible = true;
    } else {
        m_d->arrowVisible = false;
    }
}

void KisPopupButton::slotMenuAboutToHide()
{
    if (m_d->popupWidget && m_d->frame->isVisible()) {
        m_d->popupWidget->show();
    }
}
