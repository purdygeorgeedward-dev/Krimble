// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-02: resize handle for floating panels (see the
// topLevelChanged handler in createDockWidget()). A visible corner handle with
// a large touch target that resizes its floating panel from the lower right:
// the left and top edges stay where they are.
// (It replaces a QSizeGrip subclass. QSizeGrip works out which corner it is in
// from where it sits in the window: on a narrow panel such as the toolbox its
// 36 pixel width put it in the left half, so it moved the lower left corner.)
class KisFloatingDockSizeGrip : public QWidget
{
public:
    explicit KisFloatingDockSizeGrip(QWidget *parent)
        : QWidget(parent)
    {
        // KRIMBLE 2026-10-04: smaller, was 36 x 36.
        // setFixedSize(36, 36);
        // KRIMBLE 2026-10-04 (2nd pass): 20 px, was 26 (and 36 before that).
        // setFixedSize(26, 26);
        setFixedSize(20, 20);
        // Looked up by this name (see the topLevelChanged handler). This class has no
        // Q_OBJECT, so findChild<KisFloatingDockSizeGrip*>() would match any widget.
        setObjectName(QStringLiteral("krimbleResizeHandle"));
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_pressPos = event->globalPos();
            m_startSize = window()->size();
            event->accept();
        } else {
            event->ignore();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton) {
            const QPoint delta = event->globalPos() - m_pressPos;
            window()->resize(qMax(1, m_startSize.width() + delta.x()),
                             qMax(1, m_startSize.height() + delta.y()));
            event->accept();
        }
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QPen pen(palette().color(QPalette::WindowText));
        pen.setWidthF(2.0);
        p.setPen(pen);
        for (int i = 1; i <= 3; ++i) {
            const qreal offset = i * 4.5;     // was i * 6 (26 px handle) and i * 8 (36 px handle)
            p.drawLine(QPointF(width() - 2.5 - offset, height() - 2.5), QPointF(width() - 2.5, height() - 2.5 - offset));
        }
    }

private:
    QPoint m_pressPos;
    QSize m_startSize;
};

// Keeps the resize handle in the lower right corner of its floating panel.
class KisFloatingDockGripPlacer : public QObject
{
public:
    KisFloatingDockGripPlacer(QWidget *dock, QWidget *grip)
        : QObject(dock)
        , m_dock(dock)
        , m_grip(grip)
    {
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
            m_grip->move(m_dock->width() - m_grip->width(), m_dock->height() - m_grip->height());
            m_grip->raise();
        }
        return false;
    }

private:
    QWidget *m_dock;
    QWidget *m_grip;
};

// Remembers the size a panel had while it was docked, so that it can be given
// back when the panel is detached (used for the toolbox).
class KisDockedSizeTracker : public QObject
{
public:
    explicit KisDockedSizeTracker(QDockWidget *dock)
        : QObject(dock)
        , m_dock(dock)
    {
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && event->type() == QEvent::Resize && !m_dock->isFloating()) {
            m_dock->setProperty("krimbleDockedSize", m_dock->size());
        }
        // KRIMBLE 2026-10-02: remember where a floating panel was put (centre, in screen
        // coordinates), so that its own float button can dock it there too.
        if (watched == m_dock && m_dock->isFloating()
                && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) {
            m_dock->setProperty("krimbleFloatingCenter", m_dock->geometry().center());
        }
        return false;
    }

private:
    QDockWidget *m_dock;
};
#endif

// KRIMBLE 2026-10-02: used by Settings > Attach Panel.
// Docks a floating panel where the user put it: the dock area nearest to the panel's
// current position, and, in that area, between the panels above and below it.
static void krimbleAttachDockWhereItIs(QMainWindow *win, QDockWidget *dock, const QPoint &center)
{
    const QRect windowRect = win->geometry();
    if (windowRect.width() <= 0 || windowRect.height() <= 0) {
        dock->setFloating(false);
        return;
    }
    // Set while this runs, so the "docked again" handler in createDockWidget() does not
    // start a second placement.
    dock->setProperty("krimbleAttaching", true);

    // Left or right: the side of the main window the panel is nearest to. (Panels are
    // not put above or below the canvas.)
    const qreal fx = qreal(center.x() - windowRect.left()) / windowRect.width();
    const Qt::DockWidgetArea area = (fx < 0.5) ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea;

    const bool vertical = (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea);
    const Qt::Orientation stackOrientation = vertical ? Qt::Vertical : Qt::Horizontal;
    const int centerAlong = vertical ? center.y() : center.x();

    // Panels already docked in that area, in their order along it.
    QList<QDockWidget*> docked;
    Q_FOREACH (QDockWidget *other, win->findChildren<QDockWidget*>()) {
        if (other != dock && other->isVisible() && !other->isFloating() && win->dockWidgetArea(other) == area) {
            docked.append(other);
        }
    }
    std::sort(docked.begin(), docked.end(), [vertical](QDockWidget *a, QDockWidget *b) {
        return vertical ? a->geometry().top() < b->geometry().top() : a->geometry().left() < b->geometry().left();
    });

    // The panel after which the moved panel goes: the last one whose middle lies before it.
    QDockWidget *after = nullptr;
    Q_FOREACH (QDockWidget *other, docked) {
        const QPoint otherCenter = other->geometry().center();
        if ((vertical ? otherCenter.y() : otherCenter.x()) < centerAlong) {
            after = other;
        }
    }

    // A floating panel is only allowed in no area; allow all before docking it.
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    win->addDockWidget(area, dock);
    if (after) {
        win->splitDockWidget(after, dock, stackOrientation);
    } else if (!docked.isEmpty()) {
        // Goes before every panel already there: put the others after it, in order.
        QDockWidget *previous = dock;
        Q_FOREACH (QDockWidget *other, docked) {
            win->splitDockWidget(previous, other, stackOrientation);
            previous = other;
        }
    }
    dock->setFloating(false);
    dock->show();
    dock->setProperty("krimbleAttaching", false);
    dock->setProperty("krimbleFloatingCenter", QVariant());
}

static void krimbleAttachDockWhereItIs(QMainWindow *win, QDockWidget *dock)
{
    krimbleAttachDockWhereItIs(win, dock, dock->geometry().center());
}

// BEGIN KRIMBLE RIGHT PANELS
// KRIMBLE 2026-10-03: one big, easy-to-press side button for the right-hand panels.
//  - a quick TAP hides all the panels on the right-hand side (the toolbox on the left is not
//    touched); while they are hidden the same button sits at the right edge of the window and a
//    tap shows them again;
//  - a LONG PRESS (hold about 0.35 s) turns it into a drag handle that resizes the panels' width
//    (a quick sideways swipe of more than 16 px does the same).
// It is a real widget laid over the middle of the thin bar between the canvas and the panels, so
// it does not depend on Qt's own bar. The same thing is in Settings > "Hide Right Panels".
// (The first version, commit eeb51e1, watched taps on Qt's own bar instead; replaced.)

// Krimble 2026-10-11: the default for "show only an outline while a floating image window is dragged" (mdi_rubberband).
// On Android the picture is shown while dragging (George); elsewhere it follows OpenGL as before.
static int krimbleRubberBandDefault(KisConfig &cfg)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(cfg);
    return 0;
#else
    return cfg.useOpenGL();
#endif
}

// Krimble 2026-10-12: the toolbars in the top area stack, one on each row, instead of lining up in one row with the rest hidden
// behind the ">>" button (George). A toolbar break is put in front of every top toolbar except the first one.
static void krimbleStackToolBars(QMainWindow *win)
{
    QList<QToolBar*> top;
    Q_FOREACH (QToolBar *toolBar, win->findChildren<QToolBar*>()) {
        if (toolBar->parentWidget() == win && win->toolBarArea(toolBar) == Qt::TopToolBarArea && !toolBar->isHidden()) {
            top.append(toolBar);
        }
    }
    std::sort(top.begin(), top.end(), [](QToolBar *a, QToolBar *b) {
        if (a->geometry().y() != b->geometry().y()) return a->geometry().y() < b->geometry().y();
        return a->geometry().x() < b->geometry().x();
    });
    for (int i = 1; i < top.size(); ++i) {
        if (!win->toolBarBreak(top[i])) {
            win->insertToolBarBreak(top[i]);
        }
    }
}

#ifdef Q_OS_ANDROID
// Krimble 2026-10-12: George: "Menus are still way too wide." Qt adds a column for the longest keyboard shortcut to every menu
// after it has measured the items, so the style cannot take it away. The only way is for the actions not to show their shortcut
// (a touch screen has no keyboard). Done for every action in every menu, again every 2 s for menus filled later.
static void krimbleHideMenuShortcuts(QMenu *menu, int depth)
{
    if (!menu || depth > 6) return;
    Q_FOREACH (QAction *action, menu->actions()) {
        if (action->isShortcutVisibleInContextMenu()) {
            action->setShortcutVisibleInContextMenu(false);
        }
        if (action->menu()) krimbleHideMenuShortcuts(action->menu(), depth + 1);
    }
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13 (4th): THE REAL CAUSE OF THE WIDE MENUS (found by reading Qt 5.15's own source, `QMenuPrivate::updateActionRects`).
// For a menu opened from the MENU BAR, Qt always measures the keyboard shortcut of every item and adds a column as wide as the longest
// one ("max_column_width += tabWidth"). The switch `setShortcutVisibleInContextMenu(false)` (used since 2026-10-12) only matters for
// right-click menus, so it did nothing here, and no style can remove that column after the fact. The only way is for the items not to have a
// shortcut at the moment the menu is measured. So the shortcuts of all items of a menu and its submenus are taken off just before the
// menu is shown (`aboutToShow` comes before the measuring) and put back as soon as it closes (`aboutToHide`). They still work on a keyboard.
typedef QList<QPair<QPointer<QAction>, QList<QKeySequence> > > KrimbleSavedShortcuts;
static void krimbleStripShortcuts(QMenu *menu, KrimbleSavedShortcuts *saved, int depth)
{
    if (!menu || depth > 6) return;
    Q_FOREACH (QAction *action, menu->actions()) {
        const QList<QKeySequence> keys = action->shortcuts();
        if (!keys.isEmpty()) {
            saved->append(qMakePair(QPointer<QAction>(action), keys));
            action->setShortcuts(QList<QKeySequence>());
        }
        if (action->menu()) krimbleStripShortcuts(action->menu(), saved, depth + 1);
    }
}
#endif   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
