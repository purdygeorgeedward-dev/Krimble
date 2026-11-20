// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

#ifdef Q_OS_ANDROID
// Krimble 2026-10-13 (5th): an X in the top right corner that leaves Canvas Only mode (George: "Just put a X in the top right corner."
// "Just the X by itself. Black."). Canvas Only hides the menu bar and the toolbox, and a phone has no Tab key, so nothing on the screen
// could turn it off. The X is drawn only while Canvas Only is on; tapping it turns the mode off (the same switch as View > Screen Mode >
// Canvas Only). It is not a window or picture close button: it does not close anything.
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
// Krimble 2026-10-19: George (1.0.39, test list, item 2): "Using the crop tool: It was hard to select from the canvas because the resize
// tool for the toolbox got in the way. Can the resize tools be temporarily disabled during selection?" and "I think any selection tool
// should disable resize gadgets temporarily for ease of use." True while the Crop tool or any selection tool is the active tool; the
// panel edge grab zones (KrimbleDockEdgeGrab, below) hide themselves then, and come back when another tool is chosen.
static bool krimbleSelectionToolActive()
{
    const QString id = KoToolManager::instance()->activeToolId();
    return id.startsWith(QLatin1String("KisToolSelect")) || id == QLatin1String("KisToolCrop");
}
#endif

#ifdef Q_OS_ANDROID
// Krimble 2026-10-12: an invisible touch zone, 14 px wide, along the inner edge of every docked panel (the right edge of a panel
// on the left, the left edge of a panel on the right). Dragging it resizes the panel's side of the window. It draws nothing and
// takes no room of its own, so the edge between the toolbox, the canvas and the panels can be as thin as the base style makes it
// (George: "a usable invisible grab zone there, not more UI fat").
class KrimbleDockEdgeGrab : public QWidget
{
public:
    // Krimble 2026-10-12 (2nd): George: not the inner edge, it overlaps the tools. The zone is a child of the main window now and lies
    // on the OUTER side of the panel's edge, over the first 14 px of the canvas. (The first version was a child of the panel.)
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
            m_reason = QStringLiteral("hidden: panel floating or not visible");   // Krimble 2026-10-23 (2nd): the reasons are shown by the layout-info overlay
            hide();
            return;
        }
        // Krimble 2026-10-19: no grab zone while the Crop tool or a selection tool is in use (see krimbleSelectionToolActive above):
        // the zone lies over the first 30 px of the canvas, where it took the finger that was drawing the crop or selection box.
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
        // Krimble 2026-10-12 (3rd): George: "The touch zone doesn't have to be the whole edge of a panel or toolbox. It could be a small
        // slightly protruding handle in the middle." So one small handle (18 x 80 px) in the middle of each side's edge, on the
        // canvas side, drawn as a little rounded tab with three grip lines. Only the topmost panel of a side has it, and it is
        // centered on the whole column of panels on that side.
        // Krimble 2026-10-13 (2nd): George (1.0.35): "Handle is now too small. Hard to grab in portrait." (and it could not be seen: its color was
        // the canvas's gray). The bulge drawn is still 7 px deep, but its zone is 22 px wide and 64 px tall, and it is the panel's own
        // color with a thin light edge, so it looks like the panel bulging out.
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
        // setGeometry(area == Qt::LeftDockWidgetArea ? g.right() + 1 : g.left() - w, g.top(), w, g.height());   (the whole edge, the second version)
        // Krimble 2026-10-13 (6th): George: "Lose the visible handle. Make an invisible grab area extend out from the side of the edge." (and,
        // about the first list, "4 replaced"). The zone is now 24 logical px wide (about 5 mm), as tall as the whole column of panels on
        // that side, and lies OUTSIDE the panels' edge, over the canvas (to the right of the toolbox, to the left of the right panels).
        // Nothing is drawn.
        const int stripWidth = 30;   // Krimble 2026-10-17 (2nd): George: "Try just one more pixel? Or 2?" 28 -> 30. Before: 28 // Krimble 2026-10-17: George (1.0.37): "It's also sometimes still hard to grab edges for resize." "Try a small change first." 24 -> 28
        Q_UNUSED(stripWidth);
        // Krimble 2026-10-23: George (1.0.41 test list item 27): "Handles on toolbox and side panel still too small to grab easy, but close. I drew an
        // idea." His drawing: a tab sticking out of the middle of the toolbox's edge onto the canvas, the toolbox's own color, angled top and bottom
        // ends, a dotted line down its middle. The zone is now only that tab (not the whole edge, which took fingers that were drawing), centred on
        // the column of panels on that side. kTabDepth is what is drawn; the zone is kTabZone wide so a finger can catch it.
        // Krimble 2026-11-16 (728th): George (build 65): "It's hard to move the edge of the right hand pane again. So sick of the same issues coming back." Why it came back: since the
        // canvas lies under the panels (build 59, KrimbleCanvasOverlay) the canvas area covers Qt's own separator, the strip along the whole edge that could be dragged anywhere in
        // 1.0.58; a finger on it went to the canvas. Only this zone still worked, and it is 140 units tall in the middle of the edge with nothing drawn to show where. The zone is the
        // whole edge again: as tall as the column of panels, and as wide as Qt's separator plus 4 units on the canvas side and 4 on the panel side (the first full-edge zones, 24 to 30
        // units wide over the canvas, took fingers that were drawing; this one is the strip the separator had in 1.0.58 and a little more). On the right side only.
        // The toolbox's side (left) keeps the zone it had: George said of a full-edge zone there (1.0.39) that any tool that draws would grab the toolbox's edge.
        // if (area == Qt::LeftDockWidgetArea) {
        //     const int zoneH = qMin(kTabHeight, column.height());
        //     const int y = column.top() + (column.height() - zoneH) / 2;
        //     setGeometry(column.right() + 1, y, kTabZone, zoneH);
        //     const int sepWidth = qMax(4, m_win->style()->pixelMetric(QStyle::PM_DockWidgetSeparatorExtent, nullptr, m_win));
        //     setGeometry(column.left() - sepWidth - 4, column.top(), sepWidth + 8, column.height());
        // }
        // Krimble 2026-11-17 (740th): George (build 67): "Can't grab the edge of the side panel either." and, of the edge between the canvas and the toolbox / side panel: "If it's a grab area,
        // it's not big enough and should also be invisible as previously discussed." Both edges (toolbox and side panels) now have an invisible zone along the WHOLE edge: Qt's separator, plus
        // 16 units on the panel's own side and 4 on the canvas side (the canvas side stays narrow so fingers that are drawing are not taken; the canvas no longer lies under the panels, 732nd).
        {
            const int sepWidth = qMax(4, m_win->style()->pixelMetric(QStyle::PM_DockWidgetSeparatorExtent, nullptr, m_win));
            if (area == Qt::LeftDockWidgetArea) setGeometry(column.right() + 1 - 16, column.top(), 16 + sepWidth + 4, column.height());
            else setGeometry(column.left() - sepWidth - 4, column.top(), sepWidth + 4 + 16, column.height());
        }
        show();
        raise();
        // Krimble 2026-11-17: build 66 compile error: the local y now lives only inside the left-side branch above, so "y" here was the member function. y() is the place it was shown at.
        m_reason = QStringLiteral("shown at x%1 y%2, %3x%4").arg(x()).arg(y()).arg(width()).arg(height());
    }
    // Krimble 2026-10-23 (2nd): one line for the layout-info overlay (Window > ... QA overlay): is this panel's edge tab shown, and if not, why
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
    // Krimble 2026-10-23: the tab from George's drawing (see reposition). Tuning values.
    static constexpr int kTabDepth = 10;    // logical px the tab sticks out from the panel's edge
    static constexpr int kTabZone = 20;     // logical px wide touch zone (the tab plus room for a finger)
    // Krimble 2026-10-26: George (1.0.44, with a screenshot): "Way too hard to grab the side panel sizing gadget. Try to grab sizing gadget and I get this from color panel" (the black
    // square: the color selector's pop-up, opened by a touch that fell on the Color panel just below the zone: the zone covered y 425 to 615 and the touch was at about y 624). With the tab
    // no longer drawn (kTabVisible = false) nothing shows where the zone is, so it is 140 again, the height of which he said in 1.0.42: "it works great" (the tab itself was too big then).
    static constexpr int kTabHeight = 140;  // 70 -> 140 (2026-10-26)

    // Krimble 2026-10-25: George (1.0.43): "Now we have the little dots and the handle and more dots. As long as we have the little dots there I think we can make the handle
    // invisible (as long as it works)." The tab (its shape and its dotted line) is not drawn any more; its touch zone (kTabZone wide, kTabHeight tall, in the middle of the
    // column of panels) is exactly as before, so a finger on it still resizes that side. Krita's own dotted handle in the separator (the "little dots", 212th entry) stays.
    // Set this to true to see the tab again.
    static constexpr bool kTabVisible = false;

    void paintEvent(QPaintEvent *) override
    {
        if (!kTabVisible) return;
        // Krimble 2026-10-13 (6th): nothing is drawn any more (the grab zone is invisible). The old drawing is kept below, not run.
        // return;
        // Krimble 2026-10-23: the tab from George's drawing: panel color, angled ends, dotted line down the middle.
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
            // Krimble 2026-10-23 (2nd): on the right the tab lies over the canvas's scroll bar, which has the panel's own color, so the tab could not be seen there; a thin light edge
            // makes it show. The toolbox side (George: "it works great") is drawn as before.
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
        // (the rounded tab with three grip lines, kept:)
        // Krimble 2026-10-12 (5th): a slight curve outward from the panel edge, no grip lines.
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
    QString m_reason {QStringLiteral("not placed yet")};   // Krimble 2026-10-23 (2nd)
};

static void krimbleInstallDockEdgeGrabs(QMainWindow *win)
{
    // Krimble 2026-10-19: George (1.0.39): "it's not just selection tools. Any tool, including text, when you draw will grab the edge of the
    // toolbox. We should go back to the original handle concept from Krita." The invisible grab zones over the canvas (KrimbleDockEdgeGrab)
    // are switched off: any that exist are hidden and no new ones are made. Panels are resized with Krita's own handle again, the separator
    // between a panel and the canvas (widened for fingers by KisWideDockSeparatorStyle, not changed). The code below is kept, not run.
    // Krimble 2026-10-23: no longer hidden here: this runs on a timer, and each tab decides for itself whether it shows (reposition()).
    //     existing->hide();
    // }
    // return;
    // Krimble 2026-10-23: back on, as the small visible tab from George's drawing (1.0.41 test list item 27), not the full-edge strip.
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

// Krimble 2026-10-12: Layout Presets (Claude's idea: "workspace presets: Retouch, Illustration, Layout and Quick Edit, one tap each";
// George: "Better workspaces! Yes. The Krita ones suck."). Each preset shows the panels that job needs and hides the rest. The
// toolbox is always kept. Panels are matched by their title, so a translated Krimble matches on the English title only.
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

