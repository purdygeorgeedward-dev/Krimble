
#ifdef Q_OS_ANDROID
class KisFloatingDockSizeGrip : public QWidget
{
public:
    explicit KisFloatingDockSizeGrip(QWidget *parent)
        : QWidget(parent)
    {
        setFixedSize(20, 20);
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
            const qreal offset = i * 4.5;
            p.drawLine(QPointF(width() - 2.5 - offset, height() - 2.5), QPointF(width() - 2.5, height() - 2.5 - offset));
        }
    }

private:
    QPoint m_pressPos;
    QSize m_startSize;
};

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

static void krimbleAttachDockWhereItIs(QMainWindow *win, QDockWidget *dock, const QPoint &center)
{
    const QRect windowRect = win->geometry();
    if (windowRect.width() <= 0 || windowRect.height() <= 0) {
        dock->setFloating(false);
        return;
    }
    dock->setProperty("krimbleAttaching", true);

    const qreal fx = qreal(center.x() - windowRect.left()) / windowRect.width();
    const Qt::DockWidgetArea area = (fx < 0.5) ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea;

    const bool vertical = (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea);
    const Qt::Orientation stackOrientation = vertical ? Qt::Vertical : Qt::Horizontal;
    const int centerAlong = vertical ? center.y() : center.x();

    QList<QDockWidget*> docked;
    Q_FOREACH (QDockWidget *other, win->findChildren<QDockWidget*>()) {
        if (other != dock && other->isVisible() && !other->isFloating() && win->dockWidgetArea(other) == area) {
            docked.append(other);
        }
    }
    std::sort(docked.begin(), docked.end(), [vertical](QDockWidget *a, QDockWidget *b) {
        return vertical ? a->geometry().top() < b->geometry().top() : a->geometry().left() < b->geometry().left();
    });

    QDockWidget *after = nullptr;
    Q_FOREACH (QDockWidget *other, docked) {
        const QPoint otherCenter = other->geometry().center();
        if ((vertical ? otherCenter.y() : otherCenter.x()) < centerAlong) {
            after = other;
        }
    }

    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    win->addDockWidget(area, dock);
    if (after) {
        win->splitDockWidget(after, dock, stackOrientation);
    } else if (!docked.isEmpty()) {
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

static int krimbleRubberBandDefault(KisConfig &cfg)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(cfg);
    return 0;
#else
    return cfg.useOpenGL();
#endif
}

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
#endif
