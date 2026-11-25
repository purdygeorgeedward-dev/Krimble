
#ifdef Q_OS_ANDROID
class KrimbleMenuPlacer : public QObject
{
public:
    KrimbleMenuPlacer(QMenu *menu, QMenuBar *bar, QAction *nameAction) : QObject(menu), m_menu(menu), m_bar(bar), m_action(nameAction)
    {
        menu->installEventFilter(this);
        menu->setProperty("krimbleBarMenu", true);
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
            QPoint wanted = m_bar->mapToGlobal(QPoint(nameRect.left(), qMin(m_bar->height(), nameRect.bottom() + 1) - qRound(6.0 / dpr)));
            QScreen *screen = QGuiApplication::screenAt(wanted);
            if (!screen) screen = QGuiApplication::primaryScreen();
            const QRect avail = screen->availableGeometry();
            wanted.setY(qMax(wanted.y(), avail.top()));
            wanted.setX(qMin(wanted.x(), avail.right() + 1 - m_menu->width()));
            wanted.setX(qMax(wanted.x(), avail.left()));
            if (m_menu->pos() != wanted) m_menu->move(wanted);
        }
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

static void krimbleMatchSubmenu(QMenu *parent, QMenu *sub)
{
    if (!parent || !sub) return;
    QAction *sample = nullptr;
    Q_FOREACH (QAction *a, parent->actions()) {
        if (a->isVisible() && !a->isSeparator() && !a->menu()) { sample = a; break; }
    }
    if (!sample) return;
    const int rowH = parent->actionGeometry(sample).height();
    if (rowH <= 0) return;
    const int textH = sub->fontMetrics().height();
    const int vertical = qMax(0, (rowH - textH) / 2);
    const int left = qRound(rowH * 0.68);
    const QString sheet = QStringLiteral("QMenu::item { padding: %1px %2px %3px %4px; } QMenu::item:selected { color: #ffffff; background: #0b87dd; }")
                              .arg(vertical).arg(left).arg(rowH - textH - vertical).arg(left);
    if (sub->styleSheet() != sheet) sub->setStyleSheet(sheet);
}

static void krimbleInstallMenuShortcutStripping(QMainWindow *win)
{
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QMenu *menu = top->menu();
        if (!menu || menu->property("krimbleShortcutsStripped").toBool()) continue;
        menu->setProperty("krimbleShortcutsStripped", true);
        new KrimbleMenuPlacer(menu, win->menuBar(), top);
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu]() {
            Q_FOREACH (QAction *a, menu->actions()) {
                if (QMenu *sub = a->menu()) {
                    if (sub->property("krimbleSubmenuMatched").toBool()) continue;
                    sub->setProperty("krimbleSubmenuMatched", true);
                    QPointer<QMenu> parentMenu = menu;
                    QObject::connect(sub, &QMenu::aboutToShow, sub, [parentMenu, sub]() { krimbleMatchSubmenu(parentMenu, sub); });
                }
            }
        });
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
#if 0
        QString menuTitle = top->text();
        menuTitle.remove(QLatin1Char('&'));
        QObject::connect(menu, &QMenu::aboutToShow, menu, [menu, menuTitle]() {
            if (menu->findChild<QWidgetAction*>(QStringLiteral("krimbleMenuHeading"))) {
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
static void krimbleRemoveMinimumWidths(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
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
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() != QLatin1String("KisLayerBox")) continue;
        QWidget *content = dock->widget();
        if (!content || content->property("krimbleEdgeMargin").toBool()) continue;
        content->setProperty("krimbleEdgeMargin", true);
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
                rowLayout->insertSpacing(0, 24);
            }
            QMargins topMargins = content->contentsMargins();
            topMargins.setTop(topMargins.top() + 20);
            content->setContentsMargins(topMargins);
        }
    }
}
#endif

