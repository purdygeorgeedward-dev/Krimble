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
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>

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
    d->lockButton->setVisible(true);
    d->lockButton->setToolTip(i18nc("@info:tooltip", "Lock Docker"));
    d->lockButton->setStyleSheet("border: 0");

    d->updateButtonSizes();

    d->titleLabel = new KSqueezedTextLabel(this);
    d->titleLabel->setTextElideMode(Qt::ElideRight);
    d->titleLabel->setText(dockWidget->windowTitle());

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(SPACING);
    layout->addWidget(d->lockButton);
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
    // Krimble 2026-10-18 (15th): George (1.0.37, c666301; screenshots of the Color and Tool Options panels in tabs): "8. Double titles in panels look weird. Fix?" A panel in a tab
    // group shows its name on its tab and again in its own title bar. The title bar keeps its buttons (lock, float, close); only the text is hidden.
    // d->titleLabel->setVisible(!hidden);
    // Krimble 2026-10-20: George (1.0.40, screenshots of the Color and Layers panels): "Gadgets on the panels should be on the far right." The label
    // is the part of the title bar that takes all the spare room (stretch 1), which pushes the float and close buttons to the right edge. Hiding the
    // label took that room away, and the lock, float and close buttons were spread evenly across the bar. The label now stays (it keeps its room) and
    // only its text is emptied while hidden; the buttons stay at the right edge.
    d->titleTextHidden = hidden;
    QDockWidget *dock = qobject_cast<QDockWidget*>(parentWidget());
    d->titleLabel->setVisible(true);
    d->titleLabel->setText(hidden || !dock ? QString() : dock->windowTitle());
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

    // Krimble 2026-10-15: George (1.0.36): "I can't tell where one panel ends and the next begins when they stack. They need a light line at the top
    // and a dark line at the bottom." A docked panel's top edge gets a light line, and just above it a dark line: that dark line is the bottom
    // edge of the panel above (the title bar is the only part of a panel this code draws), so every panel is lit at its top and shaded at its
    // bottom. The last panel of a column has no panel below it, so its bottom has no dark line. Floating panels are not changed.
    // Krimble 2026-10-17: George (1.0.37): "Giant mega lines again for bevels instead of one pixel." These 3 px lines are off; the one-pixel lines
    // are drawn by KrimbleBevelOverlay (KisMainWindow.cpp) on panels and toolbars.
    if (false && !q->isFloating()) {
        const qreal dpr = devicePixelRatioF();
        const int line = qMax(2, qRound(dpr));          // about 1 logical px each
        p.fillRect(QRect(0, 0, width(), line), QColor(18, 19, 34));                 // dark (the bottom of the panel above)
        p.fillRect(QRect(0, line, width(), line), QColor(110, 114, 156));           // light (the top of this panel)
    }
}

void KoDockWidgetTitleBar::resizeEvent(QResizeEvent*)
{
    QDockWidget *q = qobject_cast<QDockWidget*>(parentWidget());

    if (q->isFloating() || (width() < (d->closeButton->width() + d->floatButton->width() + d->lockButton->width()) + 32)) {
        d->lockButton->setVisible(false);
    } else {
        d->lockButton->setVisible(true);
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
    // titleLabel->setText(title);
    titleLabel->setText(titleTextHidden ? QString() : title);   // Krimble 2026-10-20: stays empty while the title text is hidden
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
    // KRIMBLE 2026-10-10: docking from a menu, no dragging: Dock Left / Right / Top / Bottom, and Undock (float).
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
        // action->setEnabled(dock->allowedAreas() & entry.area);
        // Krimble 2026-10-13: George: "Dock right from the long press menu did nothing." A floating panel's allowed areas may be
        // none or only some (floating panels are allowed in no area, or in the sides the finger was near while dragging), so the
        // choices were greyed out or ignored. A choice from this menu is always allowed.
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

    // Krimble 2026-10-18 (17th): George (1.0.37, c666301): "Could we have a stack command? Like the side panel would stack instead of overlaying tabs?" and "Add stack command."
    // A panel that is in a tab group can be taken out of it and put BELOW the panel it was tabbed with ("Stack Below"); and any panel, docked or floating, can be put
    // under another panel ("Stack Under" and the list of the other panels). Qt's splitDockWidget does it: the space of the other panel is split in two, up and down.
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
