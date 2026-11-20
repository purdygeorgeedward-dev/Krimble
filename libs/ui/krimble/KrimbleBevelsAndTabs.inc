// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

#ifdef Q_OS_ANDROID   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
class KrimbleBevelOverlay : public QWidget
{
public:
    explicit KrimbleBevelOverlay(QWidget *target) : QWidget(target), m_target(target)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setObjectName(QStringLiteral("krimbleBevelOverlay"));
        target->installEventFilter(this);
        reposition();
    }
    void reposition()
    {
        if (!m_target) return;
        setGeometry(m_target->rect());
        raise();
        show();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_target) {
            switch (event->type()) {
            case QEvent::Resize:
            case QEvent::Show:
            case QEvent::LayoutRequest:
                reposition();
                break;
            case QEvent::ChildAdded:
                QTimer::singleShot(0, this, [this]() { raise(); });
                break;
            default:
                break;
            }
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (qApp->property("krimbleFlatTheme").toBool()) return;   // Krimble 2026-11-15: the theme "Krimble Dark Gray" has no bevels at all
        // Krimble 2026-11-15: George (build 60, landscape screenshot): "It's the whole bottom edge of the menu bar!!!" Measured on his screenshot: the menu bar's own bottom line is dark (rows 64 to 66), and right under it,
        // rows 67 to 69 across the whole width, is a near-white line: the top line of a tool bar only about 3 real pixels high (a tool bar with nothing in it, under the menu bar). A tool bar that is
        // that thin, or has no actions, gets no bevel lines.
        // Krimble 2026-11-15 (2nd): George: "Image windows are not supposed to get bevels or highlights when maximized." A maximized image window (QMdiSubWindow) is drawn on without any bevel lines.
        if (QMdiSubWindow *krimbleSub = qobject_cast<QMdiSubWindow*>(m_target.data())) {
            if (krimbleSub->isMaximized()) return;
        }
        if (QToolBar *krimbleToolBar = qobject_cast<QToolBar*>(m_target.data())) {
            if (krimbleToolBar->height() < 24 || krimbleToolBar->actions().isEmpty()) return;
        }
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        // Krimble 2026-11-13: George: "They all need bevels." / "I mean the menu bar." The menu bar's lines are 2 device pixels thick (4 was too much); everything else stays one device pixel.
        const bool menuBarTarget = qobject_cast<QMenuBar*>(m_target.data()) != nullptr;
        // Krimble 2026-11-14: George: "Just thicker by one pixel." The theme "Krimble Bevel" has its lines one device pixel thicker (2); every other theme stays at 1.
        // Krimble 2026-11-15: George: "Try 3 just to test next time." (1 and 2 real pixels were not enough on his screen; all themes must have bevels). Every theme: 3 REAL screen pixels. Before: 2 in Krimble Bevel, 1 in the others.
        // Krimble 2026-11-15: George (build 60, floating toolbox): "it has a fat edge again." The toolbox's edge (the dock named ToolBox) is 2 real screen pixels; everything else stays at 3.
        // Krimble 2026-11-15 (674th+): George (landscape screenshot, "NO EDGES" with arrows at the toolbox): "Make the bevel edges of the toolbox better." The toolbox's edge is 3 real pixels again, on all four sides in
        // every case (docked too), with clearer tones (below).
        const bool krimbleToolBoxTarget = m_target && m_target->objectName() == QLatin1String("ToolBox");
        const qreal one = 3.0 / qMax<qreal>(1.0, devicePixelRatioF());
        // Krimble 2026-11-12 (task 35): George: "Can we reverse bevel on menus so they look recessed?" A menu (popup) gets the lines the other way round: dark on the top and left,
        // light on the bottom and right (sunken). Panels, tool bars and image windows are not changed (raised).
        const bool recessed = qobject_cast<QMenu*>(m_target.data()) != nullptr;
        // Krimble 2026-11-15: George: "Menus look like shit with bevels. Just put a black outline around them." A menu (popup, submenus too) has no bevel: a plain black outline, 2 real screen pixels thick.
        if (recessed) {
            const qreal edge = 2.0 / qMax<qreal>(1.0, devicePixelRatioF());
            // Krimble 2026-11-15 (698th): George: "Remove the outline around menus." The black outline is off (the four fills are kept as comments).
            // Krimble 2026-11-17 (735th): George (build 66): "Menus have no line around them or beveled edges so they just merge with other menus." The black outline is back (2 real
            // screen pixels), but a menu that opens from the menu bar has no line along its top, where it meets its name ("Tops of menus still have a line separating them from menu titles").
            // Submenus and other popups have all four sides.
            p.setPen(Qt::NoPen);
            if (!m_target->property("krimbleBarMenu").toBool()) p.fillRect(QRectF(0, 0, width(), edge), Qt::black);
            p.fillRect(QRectF(0, height() - edge, width(), edge), Qt::black);
            p.fillRect(QRectF(0, 0, edge, height()), Qt::black);
            p.fillRect(QRectF(width() - edge, 0, edge, height()), Qt::black);
            // p.setPen(Qt::NoPen);
            // p.fillRect(QRectF(0, 0, width(), edge), Qt::black);
            // p.fillRect(QRectF(0, height() - edge, width(), edge), Qt::black);
            return;
        }
#if 0   // Krimble 2026-11-15: the old drawing (four plain rectangles, two tones), kept; replaced by the mitered frame below
        const QColor light = recessed ? QColor(18, 19, 34) : QColor(110, 114, 156);
        const QColor dark = recessed ? QColor(110, 114, 156) : QColor(18, 19, 34);
        const qreal w = width();
        const qreal h = height();
        QDockWidget *dock = qobject_cast<QDockWidget*>(m_target.data());
        const bool allEdges = !dock || dock->isFloating();
        p.fillRect(QRectF(0, 0, w, one), light);
        p.fillRect(QRectF(0, h - one, w, one), dark);
        if (allEdges) {
            p.fillRect(QRectF(0, 0, one, h), light);
            p.fillRect(QRectF(w - one, 0, one, h), dark);
        }
#endif
        // Krimble 2026-11-15: George: "Plain rectangles is stupid." and his two close-up pictures (docs/bevel-reference/): four edges, each its own tone (top the lightest, left light,
        // right dark, bottom the darkest), meeting at 45 degree diagonal joins (a staircase of real pixels: no antialiasing). A menu is recessed: the tones are turned round.
        // A docked panel has its top and bottom edges only (as before), straight, with no joins. The width of an edge is `one` (3 real screen pixels, entry 599).
        // Krimble 2026-11-15: George (build 60): "There are white lines on the top and bottom of the menu bar for no apparent reason." The menu bar's top line was the near-white top tone, 3 real pixels thick, which
        // reads as a white stripe on the dark bar; the menu bar's top line is the softer light tone now (the same as its left edge). Panels and the rest keep the near-white top tone.
        const bool krimbleMenuBar = qobject_cast<QMenuBar*>(m_target.data()) != nullptr;
        // Krimble 2026-11-15 (2nd): George (screenshot with "WHITE LINE" and arrows at the line under the menu bar and the line between the panels): the top lines of the panels are the white ones too. Every raised
        // top line is the softer light tone now, not only the menu bar's.
        Q_UNUSED(krimbleMenuBar);
        // Krimble 2026-11-15 (4th): George: "The shading on the bevels needs to be less intense." The four tones are closer to the panel color. Before: top (110, 120, 170), left (140, 150, 196), right (46, 52, 84), bottom (18, 19, 34).
        // Krimble 2026-11-15 (692nd): George: "Yes" (derive the bevel from each theme's own colors, not fixed blue-gray tones). Every tone is the target's own window color made lighter or darker (same hue), by factors
        // that reproduce the old blue-gray tones on the old blue-gray window color (#383A53): top 92,100,145 = 170%, toolbox top 250%, left 150% / toolbox 210%, right 90% / toolbox 48%, bottom 60% / toolbox 22%;
        // recessed: top 30%, left 83%, right 170%, bottom 300%.
        const QColor krimbleBase = m_target ? m_target->palette().color(QPalette::Window) : palette().color(QPalette::Window);
        QColor topTone    = krimbleBase.lighter(recessed ? 30 : 170);
        // Krimble 2026-11-15 (3rd): George: "There is not supposed to be a hilight on the bottom edge of the menu bar!!!!!" Whatever widget lies right under the menu bar (its top within 12 real pixels of the
        // menu bar's bottom edge) gets no top line, so nothing lit can sit on the menu bar's lower edge, whichever widget it is.
        if (!qobject_cast<QMenuBar*>(m_target.data()) && !recessed && m_target) {
            if (QMainWindow *krimbleWin = qobject_cast<QMainWindow*>(m_target->window())) {
                if (QMenuBar *krimbleBar = krimbleWin->menuBar()) {
                    const qreal krimbleGap = (m_target->mapToGlobal(QPoint(0, 0)).y() - krimbleBar->mapToGlobal(QPoint(0, krimbleBar->height())).y()) * qMax<qreal>(1.0, devicePixelRatioF());
                    if (krimbleGap >= -2.0 && krimbleGap <= 12.0) topTone = QColor(0, 0, 0, 0);
                }
            }
        }
        // Krimble 2026-11-15 (3rd): George: "Selected tab still does not connect with its own tab!" Measured on his screenshot: under the selected tab (fill #373952) a 3 px light line (#5D658C) ran across the top of the panel, between the tab and the
        // panel's title bar (#383A53). A panel in a tab group gets no top line from the overlay: the tab row draws its own line under the unselected tabs, and the selected tab runs into the panel.
        // Krimble 2026-11-15: George: "Remove all the tab customizations. They are all a failure." Off: tab line suppression
        // Krimble 2026-11-15 (697th): George: "Please make tabs match their panels and don't put a line between them." Back on: a panel in a tab group gets no top line from the overlay (the tab runs into the panel).
        if (!recessed) { if (QDockWidget *krimbleTabbedDock = qobject_cast<QDockWidget*>(m_target.data())) { if (krimbleDockInTabGroup(krimbleTabbedDock)) topTone = QColor(0, 0, 0, 0); } }
        // Krimble 2026-11-15 (674th+): the toolbox's own tones: a lit top and left, a dark right and bottom, with a wider gap between light and dark than the other panels have.
        if (krimbleToolBoxTarget && !recessed && topTone.alpha() != 0) topTone = krimbleBase.lighter(250);   // was QColor(150, 160, 208)
        const QColor leftTone   = krimbleBase.lighter(recessed ? 83 : (krimbleToolBoxTarget ? 210 : 150));
        const QColor rightTone  = krimbleBase.lighter(recessed ? 170 : (krimbleToolBoxTarget ? 48 : 90));
        // const QColor bottomTone = recessed ? QColor(214, 222, 250) : (krimbleToolBoxTarget ? QColor(12, 13, 24) : QColor(32, 34, 56));
        const QColor bottomTone = krimbleBase.lighter(recessed ? 300 : (krimbleToolBoxTarget ? 22 : 60));
        const qreal w = width();
        const qreal h = height();
        const qreal t = qMin(one, qMin(w, h) / 2.0);
        QDockWidget *dock = qobject_cast<QDockWidget*>(m_target.data());
        // const bool allEdges = !dock || dock->isFloating();
        const bool allEdges = !dock || dock->isFloating() || krimbleToolBoxTarget;   // 674th+: the docked toolbox has all four edges too
        p.setPen(Qt::NoPen);
        if (!allEdges) {
            p.fillRect(QRectF(0, 0, w, t), topTone);
            p.fillRect(QRectF(0, h - t, w, t), bottomTone);
            // Krimble 2026-11-15 (707th): George (build 63): "The right dock has no hilight line on the left side." A docked panel had only its top and bottom lines. It has the light line down its
            // left side too (the same tone as a floating panel's left side), joined to the top line with a 45 degree corner.
            p.setBrush(leftTone);
            { QPolygonF poly; poly << QPointF(0, 0) << QPointF(t, t) << QPointF(t, h - t) << QPointF(0, h); p.drawPolygon(poly); }
            return;
        }
        p.setBrush(topTone);
        { QPolygonF poly; poly << QPointF(0, 0) << QPointF(w, 0) << QPointF(w - t, t) << QPointF(t, t); p.drawPolygon(poly); }
        // Krimble 2026-11-16 (728th): George (build 65): "Shadow on the toolbox is just a black line." The toolbox's right and bottom edges were one flat, nearly black band. They are a
        // shadow that deepens toward the outer edge now: from a little darker than the toolbox (85 percent) on the inside to the old dark tone at the very edge.
        if (krimbleToolBoxTarget && !recessed) {
            QLinearGradient shade(QPointF(0, h - t), QPointF(0, h));
            shade.setColorAt(0.0, krimbleBase.lighter(85));
            shade.setColorAt(1.0, bottomTone);
            p.setBrush(shade);
        } else {
            p.setBrush(bottomTone);
        }
        // Krimble 2026-11-17 (735th): George (build 66): "Tops of menus still have a line separating them from menu titles." While a menu is open, the menu bar's dark bottom edge is left
        // out under that menu's name, so the name runs straight into its menu.
        if (QMenuBar *krimbleOpenBar = qobject_cast<QMenuBar*>(m_target.data())) {
            QAction *krimbleOpen = krimbleOpenBar->activeAction();
            if (krimbleOpen && krimbleOpen->menu() && krimbleOpen->menu()->isVisible()) {
                const QRect krimbleName = krimbleOpenBar->actionGeometry(krimbleOpen);
                p.setClipRegion(QRegion(rect()) - QRegion(krimbleName.left(), 0, krimbleName.width(), height()));
            }
        }
        { QPolygonF poly; poly << QPointF(0, h) << QPointF(w, h) << QPointF(w - t, h - t) << QPointF(t, h - t); p.drawPolygon(poly); }
        p.setClipping(false);
        p.setBrush(leftTone);
        { QPolygonF poly; poly << QPointF(0, 0) << QPointF(t, t) << QPointF(t, h - t) << QPointF(0, h); p.drawPolygon(poly); }
        // p.setBrush(rightTone);
        if (krimbleToolBoxTarget && !recessed) {
            QLinearGradient shade(QPointF(w - t, 0), QPointF(w, 0));
            shade.setColorAt(0.0, krimbleBase.lighter(85));
            shade.setColorAt(1.0, rightTone);
            p.setBrush(shade);
        } else {
            p.setBrush(rightTone);
        }
        { QPolygonF poly; poly << QPointF(w, 0) << QPointF(w, h) << QPointF(w - t, h - t) << QPointF(w - t, t); p.drawPolygon(poly); }
        // Krimble 2026-11-15: George (1.0.59 test, item 4: "No orange K in corner of default layout."): the K is drawn by the menu bar's own paint filter (see KrimbleMenuBarFilter), but in every theme except
        // Krimble Painted that drawing comes BEFORE the bar paints itself, and since task 57 the flat sheets paint an opaque bar background (#49588c), which covered the K. This overlay lies on top of the bar,
        // so the K is also drawn here, after the lines, in the same place and size (2 px from the left, centered up and down, as tall as the menu names).
        if (QMenuBar *kBar = qobject_cast<QMenuBar*>(m_target.data())) {
            const QFontMetrics kMetrics(kBar->font());
            const int kSize = qMin(kBar->height() - 4, qMax(14, kMetrics.height()));
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            qApp->windowIcon().paint(&p, QRect(2, (kBar->height() - kBar->property("krimbleBarExtra").toInt() - kSize) / 2, kSize, kSize));   // Krimble 2026-11-16 (722nd): centered on the menu names
        }
    }

private:
    QPointer<QWidget> m_target;
};

// Krimble 2026-10-18 (15th): the title text of a panel is hidden when the panel is in a tab group (docked, with other panels tabbed with it); it shows again when it is alone or floating
static void krimbleRefreshTabTitles(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar*>(dock->titleBarWidget());
        if (!bar) continue;
        // Krimble 2026-11-15: George (build 60, Layers screenshot): "Layers panel has no title." The Layers panel is alone in its place (no tab row over it), but tabifiedDockWidgets() also lists
        // panels that are closed, so it counted as "in a tab group" and its name was hidden. A panel is in a tab group only if a visible tab with its name is on screen.
        //     Q_FOREACH (QTabBar *tabBar, win->findChildren<QTabBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        //         if (!tabBar->isVisible()) continue;
        //         for (int i = 0; i < tabBar->count(); ++i) {
        //             if (tabBar->tabText(i) == dock->windowTitle()) { tabbed = true; break; }
        //         }
        //         if (tabbed) break;
        //     }
        // }
        // Krimble 2026-11-15 (3rd): George: "Layers menu still appears with NO TITLE." A tab bar with a single tab (a lone panel) no longer counts as a tab group: two or more tabs are needed (krimbleDockInTabGroup).
        const bool tabbed = krimbleDockInTabGroup(dock);
        // Krimble 2026-11-15: George: "The tab should only be visible when they are docked. Otherwise the tabs are pointless." (item 7, Color Lab double title: the tab, the title bar and the logo all said "Color Lab".)
        // The Color Lab panel names itself ONCE: by its tab when it is in a tab group, otherwise by its logo. The logo is hidden in a tab group; the title bar's text is always hidden for this panel (its logo or its tab
        // is the title). Other panels are as before: the title text is hidden when they are in a tab group.
        QWidget *krimbleLogo = dock->findChild<QWidget*>(QStringLiteral("KrimbleStudioLogo"));
        // Krimble 2026-11-15: run this again the moment a panel is torn off or docked (it only ran at start and on a tab choice, so a torn-off Color Lab kept its title bar text)
        if (!dock->property("krimbleTitleHooked").toBool()) {
            dock->setProperty("krimbleTitleHooked", true);
            QObject::connect(dock, &QDockWidget::topLevelChanged, win, [win](bool) { QTimer::singleShot(0, win, [win]() { krimbleRefreshTabTitles(win); }); });
        }
        bar->setTitleTextHidden(krimbleLogo ? true : tabbed);
        // Krimble 2026-11-15 (3rd): George: "Color Lab now has no logo!!!!!!" The logo is always shown (it was hidden while the panel was in a tab group, as its name was on the tab).
        if (krimbleLogo && krimbleLogo->isHidden()) krimbleLogo->setVisible(true);
        // Krimble 2026-11-17 (730th): George's Color Lab wishlist mockup: no title bar over the COLOR LAB band (the band has the float and close buttons now, krimble_color_pages.h).
        // The title bar is folded away: its buttons and label hidden, no margins, 0 high (tried in real Qt 5.15: the panel's contents then start at the top; a title bar that is only
        // hidden still takes its room). In a tab group the tab names the panel and can be dragged off as before.
        if (krimbleLogo) {
            Q_FOREACH (QWidget *part, bar->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
                if (part->isVisible()) part->hide();
            }
            if (bar->layout()) bar->layout()->setContentsMargins(0, 0, 0, 0);
            if (bar->maximumHeight() != 0) bar->setFixedHeight(0);
        }
    }
}

// Krimble 2026-11-12 (task 29): George: "I would like to get rid of the little locks at the top of the panels and use the tabs to detach them." and "Dragging the tab does
// NOTHING right now. Nothing." Qt's own drag of a dock tab out of its tab row does not happen on his phone (no code of ours blocks it that I could find), so this is our own
// gesture: press a panel's tab and drag it away from the tab row (further than the tab row is high, up or down) and that panel floats, its title bar under the finger.
// Dragging along the row (to reorder tabs) is not touched. A panel that is already floating, and a tab that does not match a docked panel, are left alone.
class KrimbleTabDetachFilter : public QObject
{
public:
    KrimbleTabDetachFilter(QTabBar *bar, QMainWindow *win) : QObject(bar), m_bar(bar), m_win(win) { bar->installEventFilter(this); }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched != m_bar) return false;
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            m_tab = (me->button() == Qt::LeftButton) ? m_bar->tabAt(me->pos()) : -1;
            m_taken = false;
            break;
        }
        case QEvent::MouseMove: {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            if (m_taken) return true;
            if (m_tab < 0 || !(me->buttons() & Qt::LeftButton)) break;
            const int limit = qMax(m_bar->height(), 24);
            if (me->pos().y() >= -limit && me->pos().y() <= m_bar->height() + limit) break;   // still on the row (or near it)
            QDockWidget *found = nullptr;
            const QString title = m_bar->tabText(m_tab);
            Q_FOREACH (QDockWidget *dock, m_win->findChildren<QDockWidget*>()) {
                if (!dock->isFloating() && dock->windowTitle() == title) { found = dock; break; }
            }
            m_tab = -1;
            if (!found) break;
            m_taken = true;
            found->setFloating(true);
            const QPoint g = m_bar->mapToGlobal(me->pos());
            const QRect w = m_win->geometry();
            const int x = qBound(w.left(), g.x() - found->width() / 2, qMax(w.left(), w.right() - found->width()));
            const int y = qBound(w.top(), g.y() - 12, qMax(w.top(), w.bottom() - 60));
            found->move(x, y);
            found->raise();
            return true;
        }
        case QEvent::MouseButtonRelease:
            if (m_taken) { m_taken = false; m_tab = -1; return true; }
            m_tab = -1;
            break;
        default:
            break;
        }
        return false;
    }
private:
    QTabBar *m_bar;
    QMainWindow *m_win;
    int m_tab = -1;
    bool m_taken = false;
};


// Krimble 2026-11-13 (task 56): George: "Tabs still do not tear off a panel. At all. They never did." The filter of 2026-11-12 (task 29) sat on each tab bar and, in a test in real Qt with real
// mouse events, it does tear a panel off; on the phone it never did, and nothing in krita.log said why. This one sits on the application (it sees every event on its way to any widget, as
// the other Krimble filters that work on the phone do), reads the mouse events AND the touch events of a tab bar of a panel group, follows the finger with the floating panel, and writes what
// it sees to krita.log (lines start with KRIMBLE-TABS) so that the next log shows where a drag stops if it still does not work.
class KrimbleTabTearOff : public QObject
{
public:
    explicit KrimbleTabTearOff(QObject *parent = nullptr) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QTabBar *bar = qobject_cast<QTabBar *>(watched);
        if (!bar) return false;
        QMainWindow *win = qobject_cast<QMainWindow *>(bar->parentWidget());
        if (!win) return false;   // the tab bars of the panel groups are direct children of the main window (the image tabs are not)
        // Krimble 2026-11-13 (task 75): George: "A tab should have curved corners on top, then inverted curves on the bottom" (his choice "Tab C"): the tab row of the panel groups is drawn here
        // (a style sheet cannot draw inverted curves).
        if (event->type() == QEvent::Paint && (bar->shape() == QTabBar::RoundedNorth || bar->shape() == QTabBar::TriangularNorth) && bar->count() > 0) {
            paintTabRow(bar);
            return true;
        }
        QPoint pos;
        bool isPress = false, isMove = false, isRelease = false, down = false;
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease: {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            pos = me->pos();
            isPress = event->type() == QEvent::MouseButtonPress && me->button() == Qt::LeftButton;
            isMove = event->type() == QEvent::MouseMove;
            isRelease = event->type() == QEvent::MouseButtonRelease;
            down = (me->buttons() & Qt::LeftButton) || isPress;
            break;
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd: {
            QTouchEvent *te = static_cast<QTouchEvent *>(event);
            if (te->touchPoints().isEmpty()) return false;
            if (!m_touchSeen) { m_touchSeen = true; log(QStringLiteral("a touch event reached a tab bar (type %1)").arg(int(event->type()))); }
            pos = te->touchPoints().first().pos().toPoint();
            isPress = event->type() == QEvent::TouchBegin;
            isMove = event->type() == QEvent::TouchUpdate;
            isRelease = event->type() == QEvent::TouchEnd;
            down = !isRelease;
            break;
        }
        default:
            return false;
        }
        if (isPress) {
            m_bar = bar;
            m_tab = bar->tabAt(pos);
            m_press = pos;   // Krimble 2026-11-13 (task 75): the tab itself can be dragged off too
            m_taken = false;
            m_dock = nullptr;
            m_maxOut = 0;
            log(QStringLiteral("press on a tab bar with %1 tabs, tab %2, at %3,%4 (bar %5 x %6)").arg(bar->count()).arg(m_tab).arg(pos.x()).arg(pos.y()).arg(bar->width()).arg(bar->height()));
            return false;
        }
        if (bar != m_bar) return false;
        if (isMove && down) {
            if (m_taken) {
                if (m_dock) {   // the panel follows the finger
                    const QPoint g = bar->mapToGlobal(pos);
                    m_dock->move(g - m_grab);
                }
                return true;
            }
            if (m_tab < 0) return false;
            const int outside = pos.y() < 0 ? -pos.y() : (pos.y() > bar->height() ? pos.y() - bar->height() : 0);
            m_maxOut = qMax(m_maxOut, outside);
            const int limit = qMax(bar->height() / 2, 16);   // half a tab row beyond the row, up or down
            // if (outside < limit) return false;
            // Krimble 2026-11-13 (task 75): George: "Tab should be grabbable. So far it has not been." Until now only a drag that left the tab row tore the panel off, so a drag that
            // started on the tab itself did nothing. Now a mostly vertical drag of more than 0.6 of the row's height from where the finger went down tears it off too (a sideways drag
            // still moves the tab within its group).
            const int vertical = qAbs(pos.y() - m_press.y());
            const int horizontal = qAbs(pos.x() - m_press.x());
            const int grabLimit = qMax(int(bar->height() * 0.6), 14);
            if (outside < limit && !(vertical >= grabLimit && vertical > horizontal)) return false;
            QDockWidget *found = nullptr;
            const QString title = bar->tabText(m_tab);
            Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget *>()) {
                if (!dock->isFloating() && dock->windowTitle() == title) { found = dock; break; }
            }
            if (!found) {
                // the title may carry a mnemonic (&) or be shortened: try the dock whose title contains the tab's text without them
                const QString plain = QString(title).remove(QLatin1Char('&'));
                Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget *>()) {
                    if (!dock->isFloating() && QString(dock->windowTitle()).remove(QLatin1Char('&')) == plain) { found = dock; break; }
                }
            }
            log(QStringLiteral("tear-off: tab '%1' went %2 px outside the row (limit %3), panel %4").arg(title).arg(outside).arg(limit).arg(found ? QStringLiteral("found") : QStringLiteral("NOT found")));
            m_tab = -1;
            if (!found) return false;
            m_taken = true;
            m_dock = found;
            found->setFloating(true);
            const QPoint g = bar->mapToGlobal(pos);
            const QRect w = win->geometry();
            const int x = qBound(w.left(), g.x() - found->width() / 2, qMax(w.left(), w.right() - found->width()));
            const int y = qBound(w.top(), g.y() - 12, qMax(w.top(), w.bottom() - 60));
            found->move(x, y);
            found->raise();
            m_grab = g - found->pos();
            return true;
        }
        if (isRelease) {
            const bool wasTaken = m_taken;
            if (!wasTaken && m_tab >= 0) log(QStringLiteral("released without tearing off: the finger went at most %1 px outside the row").arg(m_maxOut));
            m_taken = false;
            m_tab = -1;
            m_dock = nullptr;
            m_bar = nullptr;
            return wasTaken;
        }
        return false;
    }

private:
    // The tab row, "Tab C": rounded top corners; the selected tab has the color of the panel under it, no line below it, and curves flaring out to the line under the row (inverted
    // curves); the other tabs are darker and closed off by that line. The colors come from the theme's palette (the panel is the Window color).
    static void paintTabRow(QTabBar *bar)
    {
        // Krimble 2026-11-13 (task 80): George: "The top of the panel below. And they should have more space between them and have the same curves." Every tab has the same shape: rounded
        // top corners and the inverted curves at the bottom; the body of a tab is inset by the curve's width on each side, so the tabs have room between them and the curves of
        // neighbors do not overlap. The tabs that are not selected are darker and sit on the line under the row (the top of the panel below); the selected tab leaves a gap in that line.
        QPainter p(bar);
        p.setRenderHint(QPainter::Antialiasing, true);
        const QPalette pal = bar->palette();
        const QColor panel = pal.color(QPalette::Window);
        const QColor dark = panel.darker(130);
        const QColor strip = panel.darker(300);   // the spaces between the tabs: clearly darker than the tabs
        const QColor edge(0, 0, 0);
        const QColor textOn = pal.color(QPalette::WindowText).lighter(115);
        const QColor textOff = QColor(int(textOn.red() * 0.7), int(textOn.green() * 0.7), int(textOn.blue() * 0.75));
        const qreal r = 16.0, k = 10.0;   // r: top corner radius (larger = rounder)
        const qreal baseY = bar->height() - 0.5;
        p.fillRect(bar->rect(), strip);
        const int current = bar->currentIndex();
        // the line under the row first (the top of the panel below); the selected tab's body is drawn over it with a gap
        for (int pass = 0; pass < 2; ++pass) {   // the other tabs first, the selected tab last
            if (pass == 1) {   // the line under all the tabs (drawn after the darker tab bodies, which would hide it); the selected tab then covers its part
                p.setPen(QPen(edge, 1));
                p.drawLine(QPointF(0, baseY), QPointF(bar->width(), baseY));
            }
            for (int i = 0; i < bar->count(); ++i) {
                const bool selected = (i == current);
                if (selected != (pass == 1)) continue;
                const QRectF rect = QRectF(bar->tabRect(i));
                const QRectF t = rect.adjusted(k + 0.5, 0.5, -k - 0.5, 0.0);
                QPainterPath body;
                body.moveTo(t.left() - k, baseY + 1.0);
                body.quadTo(t.left(), baseY + 1.0, t.left(), baseY - k);
                body.lineTo(t.left(), t.top() + r);
                body.quadTo(t.left(), t.top(), t.left() + r, t.top());
                body.lineTo(t.right() - r, t.top());
                body.quadTo(t.right(), t.top(), t.right(), t.top() + r);
                body.lineTo(t.right(), baseY - k);
                body.quadTo(t.right(), baseY + 1.0, t.right() + k, baseY + 1.0);
                body.closeSubpath();
                p.setPen(Qt::NoPen);
                p.setBrush(selected ? panel : dark);
                p.drawPath(body);
                QPainterPath line;
                line.moveTo(t.left() - k, baseY);
                line.quadTo(t.left(), baseY, t.left(), baseY - k);
                line.lineTo(t.left(), t.top() + r);
                line.quadTo(t.left(), t.top(), t.left() + r, t.top());
                line.lineTo(t.right() - r, t.top());
                line.quadTo(t.right(), t.top(), t.right(), t.top() + r);
                line.lineTo(t.right(), baseY - k);
                line.quadTo(t.right(), baseY, t.right() + k, baseY);
                p.setPen(QPen(edge, 1));
                p.setBrush(Qt::NoBrush);
                p.drawPath(line);
                if (selected) {   // no line under the selected tab: cover it with the panel's color (the gap)
                    p.setPen(QPen(panel, 1));
                    p.drawLine(QPointF(t.left() - k + 1.0, baseY), QPointF(t.right() + k - 1.0, baseY));
                }
                p.setPen(selected ? textOn : textOff);
                p.drawText(t.adjusted(2, 5, -2, 0), Qt::AlignCenter, p.fontMetrics().elidedText(bar->tabText(i), Qt::ElideRight, int(t.width()) - 4));
            }
        }
    }

    static void log(const QString &text);
    QPointer<QTabBar> m_bar;
    QPointer<QDockWidget> m_dock;
    int m_tab = -1;
    int m_maxOut = 0;
    bool m_taken = false;
    bool m_touchSeen = false;
    QPoint m_grab;
    QPoint m_press;
};

void KrimbleTabTearOff::log(const QString &text)
{
    KisUsageLogger::log(QStringLiteral("KRIMBLE-TABS ") + text);
}

static void krimbleInstallTabDetach(QMainWindow *win)
{
    // the tab bars of the panel groups are direct children of the main window (the image tabs live inside the MDI area)
    Q_FOREACH (QTabBar *bar, win->findChildren<QTabBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        if (!bar->findChild<KrimbleTabDetachFilter*>(QString(), Qt::FindDirectChildrenOnly)) new KrimbleTabDetachFilter(bar, win);
    }
}

// Krimble 2026-11-13: George: "6. BUILD NOW." (panels lay over the canvas instead of shrinking it: "Instead of laying on top of the image, the right side panel forces the image in the main canvas
// area to zoom waaaay out."). The 546th entry only froze the zoom; the canvas area was still smaller by the panel's width. Here the central widget (the canvas area) keeps the whole width of the
// window: after the main window's layout has put the panels at the sides and shrunk the central widget, it is stretched back sideways to the window's full width and put behind the panels, which
// then lie on top of the canvas. The height is left as the layout made it (toolbars and the menu bar above, the status bar below). A panel that floats is not touched. In the settings,
// krimble/PanelsOverlayCanvas = false turns it off.
class KrimbleCanvasOverlay : public QObject
{
public:
    explicit KrimbleCanvasOverlay(QMainWindow *win) : QObject(win), m_win(win)
    {
        win->installEventFilter(this);
        track();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win.data()) {
            if (event->type() == QEvent::Resize || event->type() == QEvent::LayoutRequest || event->type() == QEvent::Show || event->type() == QEvent::ChildAdded) schedule();
        } else if (watched == m_central.data()) {
            // Krimble 2026-11-16 (722nd): George (build 64): "There is a weird visual jitter (periodic jolts) when trying to paint on screen which disrupts the line. Very bad bug." Every time
            // the main window lays itself out again (any widget in it asking for a new size: the status bar's memory figure does, again and again while painting), Qt puts the canvas area
            // back between the panels (narrower, and moved right by the toolbox's width), and it was stretched back to the full width only a moment LATER (a timer), so for that moment the
            // picture sat in another place under the finger and was drawn there: the jolt, and the break in the line. The canvas area is now put back AT ONCE, inside the very move or
            // resize that Qt made, before anything is drawn and before the next touch arrives.
            if ((event->type() == QEvent::Resize || event->type() == QEvent::Move) && !m_busy) apply();
        }
        return false;
    }

private:
    void track()
    {
        QWidget *central = m_win ? m_win->centralWidget() : nullptr;
        if (central == m_central.data()) return;
        if (m_central) m_central->removeEventFilter(this);
        m_central = central;
        if (m_central) m_central->installEventFilter(this);
    }
    void schedule()
    {
        if (m_pending || m_busy) return;
        m_pending = true;
        QTimer::singleShot(0, this, [this]() { m_pending = false; apply(); });
    }
    void apply()
    {
        if (!m_win) return;
        track();
        if (!m_central || !m_win->isVisible()) return;
        // Krimble 2026-11-17 (732nd): off by default. George: the slowdown is the canvas lying under the panels (full-width canvas draws pixels nobody sees, and every
        // panel repaint forces the canvas under it to be redrawn). The canvas now ends at the panel's edge, as in the industry standard; the zoom freeze (546th) stays.
        // krimble/PanelsOverlayCanvas = true turns the overlay back on.
        static const bool krimbleOverlayOn = QSettings().value(QStringLiteral("krimble/PanelsOverlayCanvas"), false).toBool();
        if (!krimbleOverlayOn) return;
        const QRect now = m_central->geometry();
        const QRect wanted(0, now.top(), m_win->width(), now.height());
        if (now == wanted) return;
        m_busy = true;
        m_central->setGeometry(wanted);
        m_central->lower();   // the panels and toolbars are drawn above it
        m_busy = false;
    }
    QPointer<QMainWindow> m_win;
    QPointer<QWidget> m_central;
    bool m_pending = false;
    bool m_busy = false;
};

// Krimble 2026-11-15: George: "panels should use the theme background." (1.0.59 test items 1, 5, 8, 9, 10: the panels and the toolbox were see-through). The style sheets paint the window and the docks with
// the rule `QMainWindow, QDockWidget { background: ...; }` (a flat color in the flat themes, a vertical gradient in Krimble Bevel and Krimble Painted), but a docked panel does not draw it itself
// and its contents are transparent, so with the canvas under the panels (KrimbleCanvasOverlay) the picture showed through. This filter paints that same background (read from the application's
// style sheet, so every theme and a theme change are followed) into each dock before the dock draws its content. A gradient is spread over the dock's own height and width.
struct KrimbleDockBackgroundSpec
{
    bool valid = false;
    bool gradient = false;
    QColor flat;
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 1;
    QVector<QPair<qreal, QColor> > stops;
};

static KrimbleDockBackgroundSpec krimbleDockBackgroundSpec()
{
    static const QChar *s_lastData = nullptr;
    static int s_lastSize = -1;
    static KrimbleDockBackgroundSpec s_spec;
    const QString sheet = qApp->styleSheet();
    if (sheet.constData() == s_lastData && sheet.size() == s_lastSize) return s_spec;
    s_lastData = sheet.constData();
    s_lastSize = sheet.size();
    s_spec = KrimbleDockBackgroundSpec();
    QString plain = sheet;
    plain.remove(QRegularExpression(QStringLiteral("/\\*.*?\\*/"), QRegularExpression::DotMatchesEverythingOption));   // the comments of the sheet hold old copies of the rule
    QRegularExpressionMatchIterator it = QRegularExpression(QStringLiteral("QMainWindow\\s*,\\s*QDockWidget\\s*\\{([^}]*)\\}")).globalMatch(plain);
    QString body;
    while (it.hasNext()) body = it.next().captured(1);   // the last rule wins
    const QRegularExpressionMatch bg = QRegularExpression(QStringLiteral("background\\s*:\\s*([^;]+);")).match(body);
    if (!bg.hasMatch()) return s_spec;
    const QString value = bg.captured(1).trimmed();
    if (value.startsWith(QLatin1Char('#'))) {
        s_spec.flat = QColor(value);
        s_spec.valid = s_spec.flat.isValid();
    } else if (value.startsWith(QStringLiteral("qlineargradient"))) {
        const char *names[4] = {"x1", "y1", "x2", "y2"};
        qreal v[4] = {0, 0, 0, 1};
        for (int i = 0; i < 4; ++i) {
            const QRegularExpressionMatch m = QRegularExpression(QStringLiteral("%1\\s*:\\s*(-?[0-9.]+)").arg(QLatin1String(names[i]))).match(value);
            if (m.hasMatch()) v[i] = m.captured(1).toDouble();
        }
        s_spec.x1 = v[0]; s_spec.y1 = v[1]; s_spec.x2 = v[2]; s_spec.y2 = v[3];
        QRegularExpressionMatchIterator st = QRegularExpression(QStringLiteral("stop\\s*:\\s*([0-9.]+)\\s+(#[0-9a-fA-F]{6})")).globalMatch(value);
        while (st.hasNext()) {
            const QRegularExpressionMatch m = st.next();
            s_spec.stops.append(qMakePair(m.captured(1).toDouble(), QColor(m.captured(2))));
        }
        s_spec.gradient = true;
        s_spec.valid = s_spec.stops.size() >= 2;
    }
    return s_spec;
}

class KrimbleDockBackground : public QObject
{
public:
    explicit KrimbleDockBackground(QDockWidget *dock) : QObject(dock) { dock->installEventFilter(this); setObjectName(QStringLiteral("krimbleDockBackground")); }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::Paint) return false;
        QDockWidget *dock = qobject_cast<QDockWidget*>(watched);
        if (!dock) return false;
        const KrimbleDockBackgroundSpec spec = krimbleDockBackgroundSpec();
        QPainter p(dock);
        const QRect r = dock->rect();
        if (spec.valid && spec.gradient) {
            QLinearGradient g(spec.x1 * r.width(), spec.y1 * r.height(), spec.x2 * r.width(), spec.y2 * r.height());
            for (int i = 0; i < spec.stops.size(); ++i) g.setColorAt(spec.stops.at(i).first, spec.stops.at(i).second);
            p.fillRect(r, QBrush(g));
        } else if (spec.valid) {
            p.fillRect(r, spec.flat);
        } else {
            p.fillRect(r, dock->palette().color(QPalette::Window));   // no rule found in the sheet: the palette's window color
        }
        return false;   // the dock draws its own content over this
    }
};

static void krimbleInstallBevels(QMainWindow *win)
{
    // Krimble 2026-11-12 (task 43): George (the new build, a screenshot of the start screen): "on start the menu bar has nothing to separate it from the rest of the screen. No border. No hilight or shadow. Then it becomes the painted version." The menu bar itself never had the one-pixel lines (only panels, tool bars, menus and image windows had): it gets them now (raised: light top and left, dark bottom and right).
    Q_FOREACH (QMenuBar *menuBar, win->findChildren<QMenuBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        if (!menuBar->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(menuBar);
    }
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (!dock->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(dock);
        // Krimble 2026-11-15: George (1.0.59 test, items 1, 5, 8, 9, 10: "UI components are transparent", the toolbox "sits on top of the image", Recent Images "writes over the toolbox"): the style sheets make the
        // children of a dock transparent, and since build 59 the canvas lies under the docks (KrimbleCanvasOverlay), so the image and the Recent Images page showed through every panel and the toolbox.
        // Every dock now paints its own background (the palette's Window color) before its content.
        if (!dock->findChild<KrimbleDockBackground*>(QStringLiteral("krimbleDockBackground"), Qt::FindDirectChildrenOnly)) new KrimbleDockBackground(dock);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(bar);
    }
    // Krimble 2026-10-18 (4th): George (1.0.37, fc126d8): "I don't see any edges." (the one-pixel light and dark lines of menus, finding 11). The first way, the
    // PE_FrameMenu of the app's style, was never reached: the theme's style sheet draws menus itself. The same overlay as for panels and toolbars is put on every
    // menu and submenu (light on the top and left edges, dark on the bottom and right, one device pixel).
    Q_FOREACH (QMenu *menu, win->findChildren<QMenu*>()) {
        if (!menu->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(menu);
    }
    // Krimble 2026-11-12 (task 28): George: "all windows need bevels!" The floating image windows (QMdiSubWindow) get the same overlay (light top and left, dark bottom and right, one device pixel); they are drawn by the style, not by the style sheet, so the sheet's bevel lines do not reach them.
    Q_FOREACH (QMdiSubWindow *sub, win->findChildren<QMdiSubWindow*>()) {
        if (!sub->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(sub);
    }
    // Krimble 2026-10-18 (15th): the double titles of panels in tabs (see krimbleRefreshTabTitles); at once when a tab is chosen, and every 2 s with the lines above
    // Krimble 2026-11-15: George: "Remove all the tab customizations. They are all a failure." Off: tab titles
    // if (!s_tabTitlesHooked) {
    // }
    // Krimble 2026-11-15 (680th): George: "ColorLab has THREE TITLES." (the tab, the title bar text and the logo). This is back on: it only hides the title bar text of a panel in a tab group (and Color Lab's, whose logo or tab is its title). It paints and moves nothing.
    static bool s_tabTitlesHooked = false;
    if (!s_tabTitlesHooked) {
        s_tabTitlesHooked = true;
        QObject::connect(win, &QMainWindow::tabifiedDockWidgetActivated, win, [win](QDockWidget*) { krimbleRefreshTabTitles(win); });
    }
    krimbleRefreshTabTitles(win);
}
#endif   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
