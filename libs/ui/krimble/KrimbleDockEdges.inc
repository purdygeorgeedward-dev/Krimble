
#ifdef Q_OS_ANDROID
class KrimbleCanvasOnlyExit : public QWidget
{
public:
    KrimbleCanvasOnlyExit(QMainWindow *win, QAction *canvasOnly) : QWidget(win), m_win(win), m_action(canvasOnly)
    {
        win->installEventFilter(this);
        QObject::connect(canvasOnly, &QAction::toggled, this, [this](bool) { reposition(); });
        hide();
        reposition();
    }
    void reposition()
    {
        if (!m_action || !m_action->isChecked()) {
            hide();
            return;
        }
        const int size = 36;     // logical px: about 12 mm on the screen
        setGeometry(m_win->width() - size - 4, 4, size, size);
        show();
        raise();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
            reposition();
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(QPen(Qt::black, 3.5, Qt::SolidLine, Qt::RoundCap));
        const int m = int(width() * 0.28);
        p.drawLine(m, m, width() - m, height() - m);
        p.drawLine(width() - m, m, m, height() - m);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        if (rect().contains(e->pos()) && m_action) {
            m_action->setChecked(false);
        }
        e->accept();
    }

private:
    QPointer<QMainWindow> m_win;
    QPointer<QAction> m_action;
};

static void krimbleInstallCanvasOnlyExit(KisMainWindow *win)
{
    if (!win->viewManager() || win->findChild<QWidget*>(QStringLiteral("krimbleCanvasOnlyExit"), Qt::FindDirectChildrenOnly)) return;
    QAction *canvasOnly = win->viewManager()->actionCollection()->action(QStringLiteral("view_show_canvas_only"));
    if (!canvasOnly) return;
    KrimbleCanvasOnlyExit *exitX = new KrimbleCanvasOnlyExit(win, canvasOnly);
    exitX->setObjectName(QStringLiteral("krimbleCanvasOnlyExit"));
}
#endif

#ifdef Q_OS_ANDROID
static bool krimbleSelectionToolActive()
{
    const QString id = KoToolManager::instance()->activeToolId();
    return id.startsWith(QLatin1String("KisToolSelect")) || id == QLatin1String("KisToolCrop");
}
#endif

#ifdef Q_OS_ANDROID
class KrimbleDockEdgeGrab : public QWidget
{
public:
    KrimbleDockEdgeGrab(QMainWindow *win, QDockWidget *dock) : QWidget(win), m_win(win), m_dock(dock)
    {
        setCursor(Qt::SizeHorCursor);
        // setAttribute(Qt::WA_NoSystemBackground);          (the invisible versions)
        dock->installEventFilter(this);
        reposition();
    }
    void reposition()
    {
        if (!m_dock || m_dock->isFloating() || !m_dock->isVisible()) {
            m_reason = QStringLiteral("hidden: panel floating or not visible");
            hide();
            return;
        }
        if (krimbleSelectionToolActive()) {
            m_reason = QStringLiteral("hidden: crop or selection tool active");
            hide();
            return;
        }
        const Qt::DockWidgetArea area = m_win->dockWidgetArea(m_dock);
        if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) {
            m_reason = QStringLiteral("hidden: panel is not on the left or right (area %1)").arg(int(area));
            hide();
            return;
        }
        const int w = 22;
        const int h = 64;
        QRect column;                       // the area covered by all docked panels on this side
        bool topmost = true;
        Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
            if (other->isFloating() || !other->isVisible() || m_win->dockWidgetArea(other) != area) continue;
            column = column.isNull() ? other->geometry() : column.united(other->geometry());
            if (other->geometry().top() < m_dock->geometry().top()) topmost = false;
        }
        if (!topmost || column.isNull()) {
            m_reason = column.isNull() ? QStringLiteral("hidden: no visible panels on this side") : QStringLiteral("hidden: another panel on this side is higher");
            hide();
            return;
        }
        const int stripWidth = 30;
        Q_UNUSED(stripWidth);
        {
            const int sepWidth = qMax(4, m_win->style()->pixelMetric(QStyle::PM_DockWidgetSeparatorExtent, nullptr, m_win));
            if (area == Qt::LeftDockWidgetArea) setGeometry(column.right() + 1, column.top(), sepWidth + 8, column.height());
            // else setGeometry(column.left() - sepWidth - 4, column.top(), sepWidth + 4 + 16, column.height());
            // Krimble 2026-11-23: the right panel's grab area a little wider, 4 more units on each side (George: "Just extend it a little.")
            else setGeometry(column.left() - sepWidth - 8, column.top(), sepWidth + 8 + 20, column.height());
        }
        show();
        raise();
        m_reason = QStringLiteral("shown at x%1 y%2, %3x%4").arg(x()).arg(y()).arg(width()).arg(height());
    }
    QString status() const { return QStringLiteral("edge tab %1: %2").arg(m_dock ? m_dock->objectName() : QString(), m_reason); }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dock && (event->type() == QEvent::Resize || event->type() == QEvent::Show
                                  || event->type() == QEvent::Move || event->type() == QEvent::ParentChange)) {
            reposition();
        }
        return false;
    }

protected:
    static constexpr int kTabDepth = 10;    // logical px the tab sticks out from the panel's edge
    static constexpr int kTabZone = 20;     // logical px wide touch zone (the tab plus room for a finger)
    static constexpr int kTabHeight = 140;  // 70 -> 140 (2026-10-26)

    static constexpr bool kTabVisible = false;

    void paintEvent(QPaintEvent *) override
    {
        if (!kTabVisible) return;
        {
            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing, true);
            const bool leftArea = m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea;
            const qreal h = height();
            const qreal d = kTabDepth;
            const qreal slope = d * 1.6;                      // how far down the angled end runs
            const qreal x0 = leftArea ? 0 : width();          // the panel's edge
            const qreal x1 = leftArea ? d : width() - d;      // the tab's outer edge
            QPainterPath tab;
            tab.moveTo(x0, 0);
            tab.lineTo(x1, slope);
            tab.lineTo(x1, h - slope);
            tab.lineTo(x0, h);
            tab.closeSubpath();
            QColor edgeColor = m_dock->palette().color(QPalette::WindowText);
            edgeColor.setAlpha(140);
            p.setPen(leftArea ? QPen(Qt::NoPen) : QPen(edgeColor, 1.2));
            p.setBrush(m_dock->palette().color(QPalette::Window));
            p.drawPath(tab);
            QColor dots = m_dock->palette().color(QPalette::WindowText);
            dots.setAlpha(170);
            QPen pen(dots, 1.6);
            pen.setStyle(Qt::DotLine);
            pen.setCapStyle(Qt::RoundCap);
            p.setPen(pen);
            const qreal mid = (x0 + x1) / 2.0;
            p.drawLine(QPointF(mid, slope + 4), QPointF(mid, h - slope - 4));
            return;
        }
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        // QColor fill = palette().color(QPalette::Mid);   (the canvas's gray: it could not be seen)
        // fill.setAlpha(215);
        QColor fill = m_dock->palette().color(QPalette::Window);
        QColor edge = m_dock->palette().color(QPalette::WindowText);
        edge.setAlpha(150);
        p.setPen(QPen(edge, 1.5));
        p.setBrush(fill);
        const qreal w = width();
        const qreal h = height();
        const qreal depth = 7;     // how far the curve sticks out; the rest of the zone is only a bigger place to put a finger
        QPainterPath bulge;
        if (m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea) {     // the panel is on the left: the curve bulges to the right
            bulge.moveTo(0, h * 0.1);
            bulge.cubicTo(depth * 1.3, h * 0.28, depth * 1.3, h * 0.72, 0, h * 0.9);
        } else {                                                           // the panels are on the right: the curve bulges to the left
            bulge.moveTo(w, h * 0.1);
            bulge.cubicTo(w - depth * 1.3, h * 0.28, w - depth * 1.3, h * 0.72, w, h * 0.9);
        }
        bulge.closeSubpath();
        p.drawPath(bulge);
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        m_startX = e->globalPos().x();
        m_startWidth = m_dock->width();
        e->accept();
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton)) return;
        const int dx = e->globalPos().x() - m_startX;
        const bool leftArea = m_win->dockWidgetArea(m_dock) == Qt::LeftDockWidgetArea;
        const int width = qMax(40, leftArea ? m_startWidth + dx : m_startWidth - dx);
        m_win->resizeDocks({m_dock}, {width}, Qt::Horizontal);
        e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        e->accept();
    }

private:
    QMainWindow *m_win;
    QDockWidget *m_dock;
    int m_startX {0};
    int m_startWidth {0};
    QString m_reason {QStringLiteral("not placed yet")};
};

static void krimbleInstallDockEdgeGrabs(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        const QString grabName = QStringLiteral("krimbleEdgeGrab_") + dock->objectName();
        QWidget *grab = win->findChild<QWidget*>(grabName, Qt::FindDirectChildrenOnly);
        if (!grab) {
            grab = new KrimbleDockEdgeGrab(win, dock);
            grab->setObjectName(grabName);
        }
        if (KrimbleDockEdgeGrab *edge = static_cast<KrimbleDockEdgeGrab*>(grab)) {
            edge->reposition();
        }
    }
}
#endif

static void krimbleApplyPanelPreset(QMainWindow *win, const QStringList &show)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->objectName() == QLatin1String("ToolBox") || dock->isFloating()) continue;
        const QString title = dock->windowTitle().remove(QLatin1Char('&')).trimmed();
        bool want = false;
        Q_FOREACH (const QString &name, show) {
            if (title.compare(name, Qt::CaseInsensitive) == 0) want = true;
        }
        dock->setVisible(want);
    }
    if (QDockWidget *toolBox = win->findChild<QDockWidget*>(QStringLiteral("ToolBox"))) {
        toolBox->show();
    }
}

