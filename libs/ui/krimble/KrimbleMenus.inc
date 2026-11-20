// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

#ifdef Q_OS_ANDROID   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
// Krimble 2026-11-13 (task 67): George: "yes number 2" (the menu starts at the bottom of the selected name on the menu bar and its left edge lines up with the name; image 2 of
// menu_now_vs_wanted.png). A menu with two or more columns is put at the very top of the screen by Qt itself (QMenu::popup: `if (ncols > 1) pos.setY(screen.top() + desktopFrame)`), over the
// menu bar and wherever x ends up; the File and Window menus have several columns on a phone because they are as tall as the screen. This puts a menu of the menu bar where the user asked
// when it is shown: x = the left edge of its name's box, y = the bottom of that box (the bar's bottom minus the item's 1 logical px margin), but never so low that its bottom is off the
// screen and never off the left or right of the screen.
class KrimbleMenuPlacer : public QObject
{
public:
    // KrimbleMenuPlacer(QMenu *menu, QMenuBar *bar, QAction *nameAction) : QObject(menu), m_menu(menu), m_bar(bar), m_action(nameAction) { menu->installEventFilter(this); }
    // Krimble 2026-11-15 (709th): George (build 63): "Window menu still doesn't work right." The menu is moved down by the bar's height after Qt has laid its columns out for the whole screen
    // height, so the last entry of a full column was below the screen (Toolbars, in his screenshot). Before each opening the menu is told how much room to keep free (half the bar's height,
    // above and below: the style reads "krimbleMenuDeskFrame", see KisWideDockSeparatorStyle.cpp), so its columns are shorter by the bar's height and all of it is on the screen.
    KrimbleMenuPlacer(QMenu *menu, QMenuBar *bar, QAction *nameAction) : QObject(menu), m_menu(menu), m_bar(bar), m_action(nameAction)
    {
        menu->installEventFilter(this);
        menu->setProperty("krimbleBarMenu", true);   // Krimble 2026-11-17 (735th): no outline along its top (KrimbleBevelOverlay)
        //     menu->setProperty("krimbleMenuDeskFrame", qMax(8, (bar->height() + 1) / 2 + 2));
        // Krimble 2026-11-17 (735th): George (build 66): "Bottoms of menus are chopped off." The room kept free was only the menu bar's height, but the menu also starts below the phone's status
        // bar and ends above its navigation buttons, and Qt measures its columns against the whole screen. The room kept free is now everything from the top of the screen to the bottom of the
        // menu bar, plus what the system takes at the bottom of the screen, plus 4 units (shared out above and below, as the style reads it).
        auto krimbleFrame = [menu, bar]() {
            QScreen *screen = bar->screen() ? bar->screen() : QGuiApplication::primaryScreen();
            const QRect full = screen->geometry();
            const QRect avail = screen->availableGeometry();
            const int above = qMax(bar->height(), bar->mapToGlobal(QPoint(0, bar->height())).y() - full.top());
            const int below = qMax(0, full.bottom() - avail.bottom());
            menu->setProperty("krimbleMenuDeskFrame", qMax(8, (above + below + 1) / 2 + 2));
        };
        krimbleFrame();
        QObject::connect(menu, &QMenu::aboutToShow, menu, krimbleFrame);
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_menu && event->type() == QEvent::Show && m_bar && m_menu->isWindow()) {
            const qreal dpr = qMax<qreal>(1.0, m_menu->devicePixelRatioF());
            const QRect nameRect = m_bar->actionGeometry(m_action);
            // Krimble 2026-11-15 (700th): George (screenshot, Select menu): "It looks like menus are a little too high." The old line took dpr (about 3) LOGICAL pixels off the bar's bottom, which is about 9 real
            // screen pixels, so the menu's top lay over the bottom of the bar. The menu now starts exactly at the bottom of the bar.
            // Krimble 2026-11-17 (738th): George (build 67 screenshots): "Same line dividing menus from titles." The name's box ends a few pixels above the bar's lower edge, and that strip of the
            // bar showed between the box and the menu as a light line. The menu now starts right at the bottom of the name's box.
            QPoint wanted = m_bar->mapToGlobal(QPoint(nameRect.left(), qMin(m_bar->height(), nameRect.bottom() + 1)));
            Q_UNUSED(dpr);
            QScreen *screen = QGuiApplication::screenAt(wanted);
            if (!screen) screen = QGuiApplication::primaryScreen();
            const QRect avail = screen->availableGeometry();
            // Krimble 2026-11-15: George (1.0.59 test, item 3: "Window menu covers up title of menu!"): a menu nearly as tall as the screen (the Window menu is 1040 px of 1080 in landscape) was moved UP by the
            // lines above until it fitted, over the bar and its own title. The top of the menu stays at the bottom of the bar (its title stays visible); when the menu is too tall for the room below, its
            // lower part is below the screen edge (it is not moved up over the bar).
            wanted.setY(qMax(wanted.y(), avail.top()));
            wanted.setX(qMin(wanted.x(), avail.right() + 1 - m_menu->width()));
            wanted.setX(qMax(wanted.x(), avail.left()));
            if (m_menu->pos() != wanted) m_menu->move(wanted);
        }
        // Krimble 2026-11-17 (735th): the menu bar's edge is drawn without a line under an open menu's name; redraw it when a menu opens or closes
        if (watched == m_menu && m_bar && (event->type() == QEvent::Show || event->type() == QEvent::Hide)) {
            QPointer<QMenuBar> bar = m_bar;
            QTimer::singleShot(0, bar, [bar]() {
                if (!bar) return;
                if (QWidget *overlay = bar->findChild<QWidget*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) overlay->update();
            });
        }
        return false;
    }
private:
    QPointer<QMenu> m_menu;
    QPointer<QMenuBar> m_bar;
    QPointer<QAction> m_action;
};

static void krimbleInstallMenuShortcutStripping(QMainWindow *win)
{
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QMenu *menu = top->menu();
        if (!menu || menu->property("krimbleShortcutsStripped").toBool()) continue;
        menu->setProperty("krimbleShortcutsStripped", true);
        new KrimbleMenuPlacer(menu, win->menuBar(), top);   // Krimble 2026-11-13 (task 67): the menu opens at the bottom of its name, left edges lined up
        QSharedPointer<KrimbleSavedShortcuts> saved(new KrimbleSavedShortcuts());
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu, saved]() {
            krimbleStripShortcuts(menu, saved.data(), 0);
        });
        QObject::connect(menu, &QMenu::aboutToHide, menu, [saved]() {
            for (int i = 0; i < saved->size(); ++i) {
                if (saved->at(i).first) saved->at(i).first->setShortcuts(saved->at(i).second);
            }
            saved->clear();
        });
        // Krimble 2026-10-15: George (1.0.36): "Got another stuck menu ... Can we move open menus down so we can still see the selected menu title?"
        // Qt puts every menu with two or more columns at the very top of the screen, over the menu bar, on purpose (QMenu::popup:
        // `if (ncols > 1) pos.setY(screen.top() + desktopFrame)`), so it cannot be moved down without cutting off its bottom items. George chose
        // (a): the menu's own name as a heading on its first line. Only for menus of 12 or more items (the others sit under the menu bar), and
        // added each time the menu is about to show (the Window menu is rebuilt then), after Krita's own handler has run.
        // Krimble 2026-10-18 (5th): George (1.0.37, fc126d8): "The menus, instead of just being right below the menubar so you can see the menu title, has an additional
        // title for some reason. We don't need that." The name heading of the 136th entry is off: the menus now open under the menu bar, so their own title is not
        // covered. The code is kept below, not compiled.
#if 0
        QString menuTitle = top->text();
        menuTitle.remove(QLatin1Char('&'));
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu, menuTitle]() {
            if (menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"))) {
                // still there from the last time: make sure it is the first one
                QAction *headingAction = menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"));
                if (menu->actions().value(0) == headingAction) return;
            }
            int itemCount = 0;
            Q_FOREACH (QAction *a, menu->actions()) {
                if (!a->isSeparator() && a->isVisible()) ++itemCount;
            }
            if (itemCount < 12) return;
            QWidgetAction *heading = menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"));
            QAction *divider = menu->findChild<QAction*>(QStringLiteral("krimbleMenuHeadingDivider"));
            if (!heading) {
                heading = new QWidgetAction(menu);
                heading->setObjectName(QStringLiteral("krimbleMenuHeading"));
                QLabel *label = new QLabel(menuTitle);
                label->setStyleSheet(QStringLiteral("QLabel { font-weight: bold; padding: 8px 14px; color: #e6e6e6; background-color: #373952; }"));
                heading->setDefaultWidget(label);
                divider = new QAction(menu);
                divider->setObjectName(QStringLiteral("krimbleMenuHeadingDivider"));
                divider->setSeparator(true);
            }
            menu->removeAction(heading);
            menu->removeAction(divider);
            QAction *firstAction = menu->actions().value(0);
            menu->insertAction(firstAction, heading);
            menu->insertAction(firstAction, divider);
        });
#endif
    }
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13 (6th): George: "There is a minimum width on the side panel. There should not be." and "Eliminate all minimum widths."
// Qt's own code (qdockwidget.cpp, `QDockWidgetItem::minimumSize` and `QDockWidgetLayout::minimumSize`) gives a docked panel as its smallest
// width the smallest width of the widget inside it (or of its title bar, if that is wider) plus the frame; the main window cannot make it
// narrower. A widget that has a minimum width of its own set (greater than 0) is measured by that number and not by what it asks for
// (qSmartMinSize), so the panels' content and title bars, and the canvas area, are given a minimum of 1 px. Whatever does not fit is cut
// off instead of forcing the panel wide. Done for every panel, the toolbox included, and again every 2 s for panels made later.
static void krimbleRemoveMinimumWidths(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        // George: "Minimum width for the toolbox should be one column." The toolbox is left out: its smallest width stays what its layout
        // gives (one column of icons).
        if (dock->objectName() == QLatin1String("ToolBox")) continue;
        if (QWidget *content = dock->widget()) {
            if (content->minimumWidth() != 1) content->setMinimumWidth(1);
        }
        if (QWidget *title = dock->titleBarWidget()) {
            if (title->minimumWidth() != 1) title->setMinimumWidth(1);
        }
    }
    if (QWidget *central = win->centralWidget()) {
        if (central->minimumWidth() != 1) central->setMinimumWidth(1);
    }
    // Krimble 2026-10-17: George (1.0.37): "it's too easy to accidentally grab the layer type selection drop-down list when grabbing the edge. Is
    // there a way to scoot that selection area away from the edge so that layer menu doesn't appear so often by accident?" The Layers panel's
    // contents (its blending-mode drop-down is the first thing on the left) are moved 16 logical px (about 54 on the screen) away from the panel's
    // left edge, where the finger lands when it looks for the edge. Done once per panel.
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() != QLatin1String("KisLayerBox")) continue;
        QWidget *content = dock->widget();
        if (!content || content->property("krimbleEdgeMargin").toBool()) continue;
        content->setProperty("krimbleEdgeMargin", true);
        // Krimble 2026-10-18: George (1.0.37): "6. Layers panel has a big strip on the side instead of just moving the layer type selector over."
        // Only the row with the blending-mode drop-down gets the 16 px space in front of it; the rest of the panel is as it was.
        QComboBox *blendCombo = content->findChild<QComboBox*>();
        if (blendCombo) {
            QBoxLayout *rowLayout = nullptr;
            std::function<QBoxLayout*(QLayout*)> findRow = [&](QLayout *layout) -> QBoxLayout* {
                if (!layout) return nullptr;
                for (int i = 0; i < layout->count(); ++i) {
                    QLayoutItem *item = layout->itemAt(i);
                    if (item->widget() == blendCombo) return qobject_cast<QBoxLayout*>(layout);
                    if (QLayout *child = item->layout()) {
                        if (QBoxLayout *found = findRow(child)) return found;
                    }
                }
                return nullptr;
            };
            rowLayout = findRow(content->layout());
            if (rowLayout && (rowLayout->direction() == QBoxLayout::LeftToRight)) {
                // Krimble 2026-11-07: George (screenshot of the Color panel over the Layers panel): "Still having a hard time selecting the top of the layers menu instead of changing the
                // layer type." (the Layer Type drop-down is the first thing at the top of the Layers panel; the finger looking for the top edge of the panel lands on it). 24 px
                // (was 16) of space in front of it, and 20 px of empty strip ABOVE the whole panel's contents, so the top of the panel is a safe place to touch.
                rowLayout->insertSpacing(0, 24);
            }
            QMargins topMargins = content->contentsMargins();
            topMargins.setTop(topMargins.top() + 20);
            content->setContentsMargins(topMargins);
        }
    }
}
#endif

