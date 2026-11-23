/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2021 Alvin Wong <alvin@alvinhc.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDockWidgetTitleBar.h"
#include "KoDockWidgetTitleBar_p.h"
#include "KoDockWidgetTitleBarButton.h"

#include <KoIcon.h>
#include <kis_icon_utils.h>

#include <WidgetsDebug.h>
#include <klocalizedstring.h>

#include <QAbstractButton>
#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QtMath>
#include <QApplication>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QMouseEvent>
#include <QPainterPath>
#include <QApplication>

#include <KSqueezedTextLabel>

static inline bool hasFeature(const QDockWidget *dockwidget, QDockWidget::DockWidgetFeature feature)
{
    return (dockwidget->features() & feature) == feature;
}

constexpr int SPACING = 6;

KoDockWidgetTitleBar::KoDockWidgetTitleBar(QDockWidget* dockWidget)
        : QWidget(dockWidget), d(new Private(this))
{
    d->floatIcon = kisIcon("docker_float");
    d->floatButton = new KoDockWidgetTitleBarButton(this);
    d->floatButton->setIcon(d->floatIcon);
    connect(d->floatButton, SIGNAL(clicked()), SLOT(toggleFloating()));
    d->floatButton->setVisible(true);
    d->floatButton->setToolTip(i18nc("@info:tooltip", "Float Docker"));
    d->floatButton->setStyleSheet("border: 0");

    d->removeIcon = kisIcon("docker_close");
    d->closeButton = new KoDockWidgetTitleBarButton(this);
    d->closeButton->setIcon(d->removeIcon);
    connect(d->closeButton, SIGNAL(clicked()), dockWidget, SLOT(close()));
    d->closeButton->setVisible(true);
    d->closeButton->setToolTip(i18nc("@info:tooltip", "Close Docker"));   
    d->closeButton->setStyleSheet("border: 0"); // border makes the header busy looking (appears on some OSs)

    d->lockIcon = kisIcon("docker_lock_a");
    d->lockButton = new KoDockWidgetTitleBarButton(this);
    d->lockButton->setCheckable(true);
    d->lockButton->setIcon(d->lockIcon);
    connect(d->lockButton, SIGNAL(toggled(bool)), SLOT(setLocked(bool)));
    d->lockButton->setVisible(false);
    d->lockButton->setToolTip(i18nc("@info:tooltip", "Lock Docker"));
    d->lockButton->setStyleSheet("border: 0");

    d->updateButtonSizes();

    d->titleLabel = new KSqueezedTextLabel(this);
    d->titleLabel->setTextElideMode(Qt::ElideRight);
    d->titleLabel->setText(dockWidget->windowTitle());

    QHBoxLayout *layout = new QHBoxLayout(this);
    // layout->setContentsMargins(2, 0, 2, 0);
    {
        // the title 28 real pixels in from the left; the row tall enough for nine grab lines (9 lines on a 9 pixel pitch, 13 above and below)
        const qreal dpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
        layout->setContentsMargins(2 + qRound(28.0 / dpr), 0, 2, 0);
        setMinimumHeight(qCeil(101.0 / dpr));
    }
    layout->setSpacing(SPACING);
    layout->addWidget(d->titleLabel, 1);
    layout->addWidget(d->floatButton);
    layout->addWidget(d->closeButton);

    connect(dockWidget, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), SLOT(featuresChanged(QDockWidget::DockWidgetFeatures)));
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), SLOT(topLevelChanged(bool)));
    connect(dockWidget, SIGNAL(windowTitleChanged(const QString &)), SLOT(dockWidgetTitleChanged(const QString &)));

    d->featuresChanged(QDockWidget::NoDockWidgetFeatures);
}

KoDockWidgetTitleBar::~KoDockWidgetTitleBar()
{
    delete d;
}

void KoDockWidgetTitleBar::setTitleTextHidden(bool hidden)
{
    d->titleTextHidden = hidden;
    QDockWidget *dock = qobject_cast<QDockWidget*>(parentWidget());
    d->titleLabel->setVisible(true);
    // d->titleLabel->setText(hidden || !dock ? QString() : dock->windowTitle());
    d->titleLabel->setText(!d->titleOverride.isEmpty() ? d->titleOverride : (hidden || !dock ? QString() : dock->windowTitle()));
    update();
}

void KoDockWidgetTitleBar::setTitleOverride(const QString &text)
{
    if (d->titleOverride == text) return;
    d->titleOverride = text;
    setTitleTextHidden(d->titleTextHidden);
}

void KoDockWidgetTitleBar::paintEvent(QPaintEvent*)
{
    QStylePainter p(this);

    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());

    int fw = q->isFloating() ? q->style()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, 0, q) : 0;
    int mw = q->style()->pixelMetric(QStyle::PM_DockWidgetTitleMargin, 0, q);

    QStyleOptionDockWidget titleOpt;
    titleOpt.initFrom(q);

    QSize lockButtonSize(0,0);
    if (d->lockButton->isVisible()) {
        lockButtonSize = d->lockButton->size();
    }

    // To improve the look with Fusion which has weird 13x15 button sizes
    int fusionTextOffset = 0;
    QRect styleTestRect = q->style()->subElementRect(QStyle::SE_DockWidgetFloatButton, &titleOpt, q);
    if (styleTestRect.width() < 16) {
        fusionTextOffset = d->lockButton->x();
    }

    titleOpt.rect = QRect(QPoint(fw + mw + lockButtonSize.width() + fusionTextOffset, 0),
                          QSize(geometry().width() - (fw * 2) -  mw - lockButtonSize.width(), geometry().height()));
    // We don't print the title text here. Instead, we have a QLabel.
    //   titleOpt.title = q->windowTitle();
    // FIXME: Maybe we just shouldn't use a QStylePainter at all?
    titleOpt.title = QString();
    titleOpt.closable = hasFeature(q, QDockWidget::DockWidgetClosable);
    titleOpt.floatable = hasFeature(q, QDockWidget::DockWidgetFloatable);
    p.drawControl(QStyle::CE_DockWidgetTitle, titleOpt);



    {
        const qreal dpr = devicePixelRatioF();
        int left = 12;
        // if (d->titleLabel && d->titleLabel->isVisible() && !d->titleTextHidden) left = d->titleLabel->geometry().left() + d->titleLabel->fontMetrics().horizontalAdvance(q->windowTitle()) + 16;
        // gaps measured from the mockup (docs/krimble-refs/2026-11-20_1943_MOCKUP-grips-and-layers-title.jpg): 36 real px after the title, 70 real px before the float button
        const qreal gapDpr = qMax<qreal>(1.0, devicePixelRatioF());
        // if (d->titleLabel && d->titleLabel->isVisible() && !d->titleTextHidden) left = d->titleLabel->geometry().left() + d->titleLabel->fontMetrics().horizontalAdvance(q->windowTitle()) + qRound(36.0 / gapDpr);
        // the grip starts after whatever title the row shows (the dock's own, or the tool's name in Tool Options): "Tool options title overlaps onto grab area."
        if (d->titleLabel && d->titleLabel->isVisible() && !d->titleLabel->text().isEmpty()) left = d->titleLabel->geometry().left() + d->titleLabel->fontMetrics().horizontalAdvance(d->titleLabel->text()) + qRound(36.0 / gapDpr);
        // Tool Options: the tool's name (krimbleToolOptionsName, in the panel) shows under this row on the phone; the grip starts after it
        // (George, 2026-11-22: "First move the grips over and out of the way." / "Now they're on top of the title.")
        if (q) {
            if (QLabel *toolName = q->findChild<QLabel*>(QStringLiteral("krimbleToolOptionsName"))) {
                if (toolName->isVisible() && !toolName->text().isEmpty()) {
                    const int nameLeft = toolName->mapTo(q, QPoint(0, 0)).x() - mapTo(q, QPoint(0, 0)).x();
                    const int nameRight = nameLeft + toolName->contentsRect().left() + 6 + toolName->fontMetrics().horizontalAdvance(toolName->text());
                    left = qMax(left, nameRight + qRound(36.0 / gapDpr));
                }
            }
        }
        int right = width() - 12;
        Q_FOREACH (QAbstractButton *b, QList<QAbstractButton*>() << d->floatButton << d->closeButton << d->lockButton) {
            // if (b && b->isVisible()) right = qMin(right, b->geometry().left() - 12);
            if (b && b->isVisible()) right = qMin(right, b->geometry().left() - qRound(70.0 / gapDpr));
        }
        if (right - left >= 24) {
            const qreal thick = 3.0 / dpr;
            const qreal gap = 9.0 / dpr;
            QColor c = palette().color(QPalette::WindowText);
            c.setAlpha(150);
            const qreal mid = height() / 2.0;
            // for (int i = -1; i <= 1; ++i) p.fillRect(QRectF(left, mid + i * gap - thick / 2, right - left, thick), c);
            // six lines (George, 2026-11-22: "The 6 line grip is nicer.")
            for (int i = 0; i < 6; ++i) p.fillRect(QRectF(left, mid + (i - 2.5) * gap - thick / 2, right - left, thick), c);
            // for (int i = -4; i <= 4; ++i) p.fillRect(QRectF(left, mid + i * gap - thick / 2, right - left, thick), c);   // nine lines
        }
    }
}

void KoDockWidgetTitleBar::resizeEvent(QResizeEvent*)
{
    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());

    if (q->isFloating() || (width() < (d->closeButton->width() + d->floatButton->width() + d->lockButton->width()) + 32)) {
        d->lockButton->setVisible(false);
    } else {
        d->lockButton->setVisible(false);
    }
}

void KoDockWidgetTitleBar::setLocked(bool locked)
{
    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());

    d->locked = locked;
    d->lockButton->blockSignals(true);
    d->lockButton->setChecked(locked);
    d->lockButton->blockSignals(false);

    if (locked) {
        d->features = q->features();
        q->setFeatures(QDockWidget::NoDockWidgetFeatures);
    }
    else {
        q->setFeatures(d->features);
    }

    q->toggleViewAction()->setEnabled(!locked);
    d->closeButton->setEnabled(!locked);
    d->floatButton->setEnabled(!locked);
    d->floatButton->setVisible(!locked);

    d->updateIcons();
    q->setProperty("Locked", locked);
}

void KoDockWidgetTitleBar::updateIcons()
{
    d->updateIcons();
}

void KoDockWidgetTitleBar::Private::toggleFloating()
{
    QDockWidget *q = qobject_cast<QDockWidget*>(thePublic->parentWidget());

    q->setFloating(!q->isFloating());
    updateIcons();
}

void KoDockWidgetTitleBar::Private::topLevelChanged(bool topLevel)
{
    lockButton->setEnabled(!topLevel);
    updateIcons();
}

void KoDockWidgetTitleBar::Private::featuresChanged(QDockWidget::DockWidgetFeatures)
{
    QDockWidget *q = qobject_cast<QDockWidget*>(thePublic->parentWidget());

    closeButton->setVisible(hasFeature(q, QDockWidget::DockWidgetClosable));
    floatButton->setVisible(hasFeature(q, QDockWidget::DockWidgetFloatable));

    updateButtonSizes();
    thePublic->resizeEvent(0);
}

void KoDockWidgetTitleBar::Private::dockWidgetTitleChanged(const QString &title)
{
    // titleLabel->setText(titleTextHidden ? QString() : title);
    titleLabel->setText(!titleOverride.isEmpty() ? titleOverride : (titleTextHidden ? QString() : title));
}


void KoDockWidgetTitleBar::Private::updateIcons()
{
    lockIcon = (!locked) ? kisIcon("docker_lock_a") : kisIcon("docker_lock_b");
    lockButton->setIcon(lockIcon);

    // this method gets called when switching themes, so update all of the themed icons now
    floatButton->setIcon(kisIcon("docker_float"));
    closeButton->setIcon(kisIcon("docker_close"));

    thePublic->resizeEvent(0);
}

void KoDockWidgetTitleBar::Private::updateButtonSizes()
{
    const QDockWidget *q = qobject_cast<QDockWidget*>(thePublic->parentWidget());

    const int fw = q->isFloating() ? q->style()->pixelMetric(QStyle::PM_DockWidgetFrameWidth, 0, q) : 0;

    QStyleOptionDockWidget opt;
    opt.initFrom(q);
    opt.rect = QRect(QPoint(fw, fw), QSize(thePublic->geometry().width() - (fw * 2), thePublic->geometry().height() - (fw * 2)));
    opt.title = q->windowTitle();
    // Originally it was:
    //   opt.closable = hasFeature(q, QDockWidget::DockWidgetClosable);
    // but I think we better just always pretend the close button is visible to
    // get the button size.
    opt.closable = true;
    opt.floatable = hasFeature(q, QDockWidget::DockWidgetFloatable);

    // Again, we just always use the size of the close button, so we don't
    // need to get the size of the float button...
    //   QRect floatRect = q->style()->subElementRect(QStyle::SE_DockWidgetFloatButton, &opt, q);
    const QRect closeRect = q->style()->subElementRect(QStyle::SE_DockWidgetCloseButton, &opt, q);
    QSize buttonSize = closeRect.size();
    if (buttonSize.width() < 16) {
        // To improve the look with Fusion which has weird 13x15 button sizes
        buttonSize = QSize(16, 16);
    } else if (buttonSize.width() != buttonSize.height()) {
        // Just make sure the button is square...
        buttonSize.setHeight(buttonSize.width());
    }

    floatButton->setFixedSize(buttonSize);
    closeButton->setFixedSize(buttonSize);
    lockButton->setFixedSize(buttonSize);
}

//have to include this because of Q_PRIVATE_SLOT
#include "moc_KoDockWidgetTitleBar.cpp"

void KoDockWidgetTitleBar::contextMenuEvent(QContextMenuEvent *event)
{
    QDockWidget *dock = qobject_cast<QDockWidget*>(parentWidget());
    QMainWindow *mainWindow = dock ? qobject_cast<QMainWindow*>(dock->parentWidget()) : nullptr;
    if (!dock || !mainWindow || d->lockButton->isChecked()) {
        QWidget::contextMenuEvent(event);
        return;
    }
    const Qt::DockWidgetArea current = dock->isFloating() ? Qt::NoDockWidgetArea : mainWindow->dockWidgetArea(dock);
    QMenu menu(this);
    const struct { QString text; Qt::DockWidgetArea area; } entries[] = {
        {i18n("Dock Left"), Qt::LeftDockWidgetArea},
        {i18n("Dock Right"), Qt::RightDockWidgetArea},
        {i18n("Dock Top"), Qt::TopDockWidgetArea},
        {i18n("Dock Bottom"), Qt::BottomDockWidgetArea}
    };
    for (const auto &entry : entries) {
        QAction *action = menu.addAction(entry.text);
        action->setCheckable(true);
        action->setChecked(current == entry.area);
        action->setEnabled(true);
        const Qt::DockWidgetArea area = entry.area;
        // connect(action, &QAction::triggered, dock, [dock, mainWindow, area]() {
        //     mainWindow->addDockWidget(area, dock);
        //     dock->show();
        // });
        connect(action, &QAction::triggered, dock, [dock, mainWindow, area]() {
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            mainWindow->addDockWidget(area, dock);
            // addDockWidget alone leaves a floating panel floating; it has to be told to dock.
            if (dock->isFloating()) {
                dock->setFloating(false);
            }
            dock->show();
        });
    }
    menu.addSeparator();
    QAction *undock = menu.addAction(i18n("Undock"));
    undock->setCheckable(true);
    undock->setChecked(dock->isFloating());
    connect(undock, &QAction::triggered, dock, [dock]() { dock->setFloating(true); });

    menu.addSeparator();
    const QList<QDockWidget*> tabMates = mainWindow->tabifiedDockWidgets(dock);
    if (!dock->isFloating() && !tabMates.isEmpty()) {
        QDockWidget *mate = tabMates.first();
        QAction *stackBelow = menu.addAction(i18n("Stack Below %1", mate->windowTitle()));
        connect(stackBelow, &QAction::triggered, dock, [dock, mainWindow, mate]() {
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            mainWindow->splitDockWidget(mate, dock, Qt::Vertical);
            dock->show();
        });
    }
    QMenu *stackUnder = menu.addMenu(i18n("Stack Under"));
    bool anyOther = false;
    Q_FOREACH (QDockWidget *other, mainWindow->findChildren<QDockWidget*>()) {
        if (other == dock || other->isFloating() || !other->isVisible() || tabMates.contains(other)) continue;
        anyOther = true;
        QAction *under = stackUnder->addAction(other->windowTitle());
        connect(under, &QAction::triggered, dock, [dock, mainWindow, other]() {
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            mainWindow->splitDockWidget(other, dock, Qt::Vertical);
            if (dock->isFloating()) dock->setFloating(false);
            dock->show();
        });
    }
    stackUnder->setEnabled(anyOther);
    menu.exec(event->globalPos());
}

void KoDockWidgetTitleBar::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    return;
    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());
    if (!q || q->isFloating() || event->button() != Qt::LeftButton || !hasFeature(q, QDockWidget::DockWidgetMovable)) {
        event->ignore();
        return;
    }
    d->pressed = true;
    d->dragging = false;
    d->pressGlobal = event->globalPos();
    event->accept();
}

void KoDockWidgetTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());
    if (!q || !d->pressed) {
        event->ignore();
        return;
    }
    if (!d->dragging) {
        if ((event->globalPos() - d->pressGlobal).manhattanLength() < QApplication::startDragDistance()) {
            event->accept();
            return;
        }
        d->grabOffset = d->pressGlobal - q->mapToGlobal(QPoint(0, 0));
        q->setFloating(true);
        d->dragging = true;
        qApp->installEventFilter(this);   // the mouse may no longer be grabbed by this bar once the panel is its own window; follow the finger from the application
    }
    q->move(event->globalPos() - d->grabOffset);
    event->accept();
}

void KoDockWidgetTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (!d->pressed) {
        event->ignore();
        return;
    }
    d->pressed = false;
    if (d->dragging) {
        d->dragging = false;
        qApp->removeEventFilter(this);
    }
    event->accept();
}

bool KoDockWidgetTitleBar::eventFilter(QObject *watched, QEvent *event)
{
    if (d->dragging) {
        if (event->type() == QEvent::MouseMove) {
            QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());
            if (q) q->move(static_cast<QMouseEvent*>(event)->globalPos() - d->grabOffset);
        } else if (event->type() == QEvent::MouseButtonRelease) {
            d->pressed = false;
            d->dragging = false;
            qApp->removeEventFilter(this);
        }
    }
    return QWidget::eventFilter(watched, event);
}
