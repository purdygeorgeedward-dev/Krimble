// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

#ifdef Q_OS_ANDROID   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
// Krimble 2026-10-30: George: "All panels with selections like little buttons or whatever should have the blue glow if possible when selected." and "use that
// light up blue glow for selections like the mockups." A checked tool button or push button (the selected tool in the toolbox, a pressed-in option button in a
// panel or a dialog) gets a soft blue glow around it. The style sheet cannot blur, so this is a drop shadow effect with no offset, in light blue, put on the
// button when it is checked and taken off when it is not. Check boxes and radio buttons are left alone (they are not "little buttons"). Every button is found when
// it is first shown (the filter sees its Polish event) and the ones that exist already are found when the filter is installed.
static void krimbleSetButtonGlow(QAbstractButton *button, bool on)
{
    if (on) {
        if (button->graphicsEffect()) return;       // it has an effect already (this one, or another that is not ours)
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(button);
        glow->setColor(QColor(56, 198, 255, 230));  // the blue of the selected tab and checked button
        glow->setBlurRadius(18);
        glow->setOffset(0, 0);
        button->setGraphicsEffect(glow);
    } else if (qobject_cast<QGraphicsDropShadowEffect*>(button->graphicsEffect())) {
        button->setGraphicsEffect(nullptr);
    }
}

class KrimbleGlowFilter : public QObject
{
public:
    explicit KrimbleGlowFilter(QObject *parent) : QObject(parent) {}
    void hook(QAbstractButton *button)
    {
        if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
        if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;   // not check boxes or radio buttons
        button->setProperty("krimbleGlowHooked", true);
        QObject::connect(button, &QAbstractButton::toggled, button, [button](bool on) { krimbleSetButtonGlow(button, on); });
        krimbleSetButtonGlow(button, button->isChecked());
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QAbstractButton *button = qobject_cast<QAbstractButton*>(watched)) hook(button);
        }
        return false;
    }
};

static void krimbleInstallSelectionGlow(KisMainWindow *win)
{
    if (win->findChild<KrimbleGlowFilter*>(QStringLiteral("krimbleGlowFilter"))) return;
    KrimbleGlowFilter *filter = new KrimbleGlowFilter(win);
    filter->setObjectName(QStringLiteral("krimbleGlowFilter"));
    qApp->installEventFilter(filter);
    Q_FOREACH (QAbstractButton *button, win->findChildren<QAbstractButton*>()) {
        filter->hook(button);
    }
}

// View > Show > Hex (a check mark) and Hex Size...; Layer > New > Hex Pattern...
static void krimbleInstallHexMenus(KisMainWindow *win)
{
    // Krimble 2026-10-18 (8th): once: the saved state is read, and the overlay follows the open picture (see g_krimbleHexOn)
    static bool s_hexFollowHooked = false;
    if (!s_hexFollowHooked) {
        s_hexFollowHooked = true;
        krimbleHexLoadState();
        QObject::connect(win, &KisMainWindow::activeViewChanged, win, [win]() {
            QTimer::singleShot(300, win, [win]() { krimbleHexFollowView(win); });
        });
    }
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QMenu *menu = top->menu();
        if (!menu || menu->property("krimbleHexMenus").toBool()) continue;
        QString topTitle = top->text();
        topTitle.remove(QLatin1Char('&'));
        const bool isView = (topTitle == QLatin1String("View"));
        const bool isLayer = (topTitle == QLatin1String("Layer"));
        if (!isView && !isLayer) continue;
        menu->setProperty("krimbleHexMenus", true);
        QObject::connect(menu, &QMenu::aboutToShow, menu, [win, menu, isView, isLayer]() {
            Q_FOREACH (QAction *action, menu->actions()) {
                QString text = action->text();
                text.remove(QLatin1Char('&'));
                QMenu *sub = action->menu();
                if (!sub) continue;
                if (isView && text == QLatin1String("Show")) {
                    if (sub->findChild<QAction*>(QStringLiteral("krimbleHex"))) {
                        QAction *hexAction = sub->findChild<QAction*>(QStringLiteral("krimbleHex"));
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                        QSignalBlocker blocker(hexAction);
                        hexAction->setChecked(deco && deco->visible());
                        if (QAction *lockAct = sub->findChild<QAction*>(QStringLiteral("krimbleHexLock"))) {   // Krimble 2026-11-12 (task 42)
                            QSignalBlocker lockBlocker(lockAct);
                            lockAct->setChecked(krimbleHexLocked());
                        }
                        continue;
                    }
                    QAction *hexAction = new QAction(i18n("Hex"), sub);
                    hexAction->setObjectName(QStringLiteral("krimbleHex"));
                    hexAction->setCheckable(true);
                    QObject::connect(hexAction, &QAction::toggled, win, [win](bool on) {
                        g_krimbleHexOn = on;          // Krimble 2026-10-18 (8th): on for every picture, and saved
                        krimbleHexSaveState();
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, on);
                        if (!deco) return;
                        deco->size = g_krimbleHexSize;
                        deco->flatTop = g_krimbleHexFlat;
                        deco->lineWidth = g_krimbleHexLineWidth;   // task 41
                        deco->lineColor = g_krimbleHexLineColor;
                        deco->setVisible(on);
                        KisViewManager *manager = win->viewManager();
                        KisCanvas2 *hexCanvas = (manager && win->activeView()) ? win->activeView()->canvasBase() : nullptr;
                        if (on && hexCanvas && hexCanvas->canvasWidget() && !krimbleHexLocked()) {   // Krimble 2026-11-12 (task 42): no pinching while locked
                            // George: "Hex overlay should be pinch zoom then drop with a tap."
                            deco->adjusting = true;
                            KrimbleHexAdjustFilter *adjustFilter = new KrimbleHexAdjustFilter(hexCanvas->canvasWidget(), deco, hexCanvas);
                            adjustFilter->setOnDrop([win]() {
                                if (QMessageBox::question(win, i18n("Hex Pattern"),
                                        i18n("Make this a layer? It is a transparent vector layer with the hexes at the size you set."),
                                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                                    krimbleMakeHexLayer(win);
                                }
                            });
                        } else {
                            deco->adjusting = false;
                        }
                        if (hexCanvas) hexCanvas->updateCanvas();
                    });
                    QAction *sizeAction = new QAction(i18n("Hex Size..."), sub);
                    sizeAction->setObjectName(QStringLiteral("krimbleHexSize"));
                    QObject::connect(sizeAction, &QAction::triggered, win, [win]() {
                        if (krimbleHexRefuseWhenLocked(win, i18n("Hex Size"))) return;   // Krimble 2026-11-12 (task 42)
                        qreal thickness = g_krimbleHexThickness;
                        QColor color = g_krimbleHexColor;
                        if (!krimbleAskHex(win, false, &g_krimbleHexSize, &g_krimbleHexFlat, &thickness, &color)) return;
                        krimbleHexSaveState();        // Krimble 2026-10-18 (8th)
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                        if (deco) {
                            deco->size = g_krimbleHexSize;
                            deco->flatTop = g_krimbleHexFlat;
                            KisViewManager *manager = win->viewManager();
                            if (manager && win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
                        }
                    });
                    // Krimble 2026-11-12 (task 41): George: "I turned on the hex overlay in view and the lines are too thin to see. There is no control for line thickness or color." View > Show > Hex Lines...: thickness, color and opacity of the lines.
                    QAction *linesAction = new QAction(i18n("Hex Lines..."), sub);
                    linesAction->setObjectName(QStringLiteral("krimbleHexLines"));
                    QObject::connect(linesAction, &QAction::triggered, win, [win]() {
                        if (krimbleHexRefuseWhenLocked(win, i18n("Hex Lines"))) return;   // Krimble 2026-11-12 (task 42)
                        auto apply = [win]() {
                            KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                            if (deco) {
                                deco->lineWidth = g_krimbleHexLineWidth;
                                deco->lineColor = g_krimbleHexLineColor;
                                if (win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
                            }
                        };
                        if (krimbleAskHexLines(win, &g_krimbleHexLineWidth, &g_krimbleHexLineColor, apply)) {
                            krimbleHexSaveState();
                        }
                    });
                    // Krimble 2026-11-12 (task 42): View > Show > Lock Hex Settings (a check mark).
                    QAction *lockAction = new QAction(i18n("Lock Hex Settings"), sub);
                    lockAction->setObjectName(QStringLiteral("krimbleHexLock"));
                    lockAction->setCheckable(true);
                    lockAction->setChecked(krimbleHexLocked());
                    QObject::connect(lockAction, &QAction::toggled, win, [](bool on) { krimbleHexSetLocked(on); });
                    sub->addAction(hexAction);
                    sub->addAction(sizeAction);
                    sub->addAction(linesAction);
                    sub->addAction(lockAction);
                } else if (isLayer && text == QLatin1String("New")) {
                    // Krimble 2026-10-30: Layer > New > Star... (see krimbleMakeStarLayer), above Hex Pattern...
                    if (!sub->findChild<QAction*>(QStringLiteral("krimbleStarLayer"))) {
                        QAction *starAction = new QAction(i18n("Star..."), sub);
                        starAction->setObjectName(QStringLiteral("krimbleStarLayer"));
                        QObject::connect(starAction, &QAction::triggered, win, [win]() { krimbleMakeStarLayer(win); });
                        sub->addAction(starAction);
                    }
                    if (sub->findChild<QAction*>(QStringLiteral("krimbleHexLayer"))) continue;
                    QAction *layerAction = new QAction(i18n("Hex Pattern..."), sub);
                    layerAction->setObjectName(QStringLiteral("krimbleHexLayer"));
                    QObject::connect(layerAction, &QAction::triggered, win, [win]() { krimbleMakeHexLayer(win); });
                    sub->addAction(layerAction);
                }
            }
        });
    }
}

class KrimbleMdiTitleStyle : public QProxyStyle
{
public:
    KrimbleMdiTitleStyle() : QProxyStyle(QApplication::style()->objectName()) {}
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
    {
        const int base = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_TitleBarHeight) return int(base * 1.25);
        return base;
    }
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if (control == QStyle::CC_TitleBar) {
            if (const QStyleOptionTitleBar *titleBar = qstyleoption_cast<const QStyleOptionTitleBar*>(option)) {
                QStyleOptionTitleBar copy(*titleBar);
                copy.palette.setColor(QPalette::Highlight, QColor(55, 57, 82));
                copy.palette.setColor(QPalette::HighlightedText, QColor(230, 230, 230));
                // Krimble 2026-10-20: George (1.0.41, screenshot of an image window's title): "Image window title text is weird double image." The base style
                // (Fusion) draws the title twice, a light copy one logical pixel down and right as a shadow and the text on top; at this phone's density
                // (3.19) the shadow is 3 device pixels away and shows as a double image. The bar is drawn without its text, and the text is drawn once.
                const QString title = copy.text;
                copy.text = QString();
                QProxyStyle::drawComplexControl(control, &copy, painter, widget);
                const QRect textRect = subControlRect(control, titleBar, QStyle::SC_TitleBarLabel, widget);
                painter->save();
                QFont titleFont = painter->font();
                titleFont.setBold(true);
                painter->setFont(titleFont);
                painter->setPen(QColor(230, 230, 230));
                painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter,
                                  QFontMetrics(titleFont).elidedText(title, Qt::ElideRight, textRect.width()));
                painter->restore();
                return;
            }
        }
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
};

// Krimble 2026-11-16 (728th): George (build 65): "I tried to leave the toolbox at the bottom, out of the way, but it snapped to the side. Not sure how to set that. Can docking be turned on
// and off? That seems like a good idea." One switch for every panel and the toolbox: "Dock by Dragging" (in the toolbox handle's menu; setting krimble/DragDocking, on by default). Off: a
// panel or the toolbox that is dragged stays where it is let go, anywhere, and never docks by itself; the Dock entries of the menus still dock it. The same function is in KoToolBoxDocker.cpp.
static bool krimbleDragDockingOn()
{
    QVariant v = qApp->property("krimbleDragDocking");
    if (!v.isValid()) {
        v = QSettings().value(QStringLiteral("krimble/DragDocking"), true).toBool();
        qApp->setProperty("krimbleDragDocking", v);
    }
    return v.toBool();
}

class KrimbleDockTolerance : public QObject
{
public:
    // Krimble 2026-10-12: George: "Panels should not try to snap into place until they are right on top of the edge they are going to
    // snap to." 8 logical px (about 27 on the screen), was 36.
    // Krimble 2026-10-18 (2nd): George (1.0.37, fc126d8): "4. When a panel is detached by dragging it is still way too eager to reattach. Distance is an issue."
    // 8 -> 4 logical px; and a panel can join another panel (tabs) only after the finger has moved 120 logical px from where the drag began.
    static const int Distance = 4;
    static const int DwellMs = 350;   // Krimble 2026-10-30: how long the finger must stay within reach of a side before the panel may dock there

    KrimbleDockTolerance(QMainWindow *win, QDockWidget *dock)
        : QObject(dock), m_win(win), m_dock(dock) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched != m_dock || !m_dock->isFloating()) {
            return false;
        }
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
            if (mouse->buttons() & Qt::LeftButton) {
                const QPoint p = mouse->globalPos();
                if (!m_dragging) m_startPos = p;
                const QRect r(m_win->mapToGlobal(QPoint(0, 0)), m_win->size());
                Qt::DockWidgetAreas allowed = Qt::NoDockWidgetArea;
                if (p.x() - r.left() < Distance) allowed |= Qt::LeftDockWidgetArea;
                if (r.right() - p.x() < Distance) allowed |= Qt::RightDockWidgetArea;
                if (p.y() - r.top() < Distance) allowed |= Qt::TopDockWidgetArea;
                if (r.bottom() - p.y() < Distance) allowed |= Qt::BottomDockWidgetArea;
                // Krimble 2026-10-30: George: "I think there should be a proximity sensor that has a feel for when it's deliberate." (the Brushes panel attached to an
                // edge too eagerly). A side of the window is allowed only after the finger has stayed within reach of it for DwellMs (350 ms), so a finger
                // passing the edge on its way somewhere else does not dock the panel. "Within reach" is the zone used on release below (14 px, 28 px on the right).
                {
                    Qt::DockWidgetAreas reachAreas = Qt::NoDockWidgetArea;
                    if (p.x() - r.left() < 14) reachAreas |= Qt::LeftDockWidgetArea;
                    if (r.right() - p.x() < 28) reachAreas |= Qt::RightDockWidgetArea;
                    if (p.y() - r.top() < 14) reachAreas |= Qt::TopDockWidgetArea;
                    if (r.bottom() - p.y() < 14) reachAreas |= Qt::BottomDockWidgetArea;
                    if (reachAreas != m_reachMask) {
                        m_reachMask = reachAreas;
                        m_reachClock.restart();
                    }
                    if (m_reachMask == Qt::NoDockWidgetArea || m_reachClock.elapsed() < DwellMs) {
                        allowed = Qt::NoDockWidgetArea;       // not deliberate (yet): no side of the window (panels to tab with are added below)
                    }
                }
                // Krimble 2026-10-15: George (1.0.36): "Panels don't go into tabs at all now when you drag them on top of each other." and
                // "Love the tabs. Bring them back." and "Expand so they tab." Dropping a panel onto another panel needs the area of that panel to be
                // allowed, and only the window's edges were. While the finger is over a docked panel, that panel's area is allowed too, so the
                // panel can join it (as a tab, or beside it, as Qt decides from where on that panel the finger is).
                Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
                    if (other == m_dock || other->isFloating() || !other->isVisible()) continue;
                    // Krimble 2026-10-19: George (1.0.39): "The toolbox isn't close to an edge and it snaps to one aggressively." The toolbox
                    // does not join other panels (tabs); with the panels covering much of the screen, a finger passing over one docked it there.
                    if (m_dock->objectName() == QLatin1String("ToolBox")) continue;
                    if (QLineF(QPointF(p), QPointF(m_startPos)).length() < 120.0) continue;     // Krimble 2026-10-18 (2nd): not at once
                    const QRect otherRect(other->mapToGlobal(QPoint(0, 0)), other->size());
                    if (otherRect.contains(p)) allowed |= m_win->dockWidgetArea(other);
                }
                // Krimble 2026-11-15 (708th): George (build 63): "I used the menu and dragged the toolbox and got unpredictable results." Three things could dock a dragged toolbox: Qt's own drop
                // (which picks a place inside the dock area from where the finger is: above, below or beside the panels there), the code on release below, and the toolbox's own edge timer.
                // Qt's own drop is off for the toolbox; it is docked only by the release code and the timer, and both put it in one place per side (its own column or row, see the release code below).
                if (m_dock->objectName() == QLatin1String("ToolBox")) allowed = Qt::NoDockWidgetArea;
                if (!krimbleDragDockingOn()) allowed = Qt::NoDockWidgetArea;   // Krimble 2026-11-16 (728th): "Dock by Dragging" is off: nothing docks from a drag
                if (m_dock->allowedAreas() != allowed) {
                    m_dock->setAllowedAreas(allowed);
                }
                m_dragging = true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_dragging) {
            m_dragging = false;
            const QPoint releasePos = static_cast<QMouseEvent*>(event)->globalPos();
            // Krimble 2026-10-30: deliberate = the finger stayed within reach of a side for DwellMs (see the move handler above)
            const bool dwelled = m_reachMask != Qt::NoDockWidgetArea && m_reachClock.isValid() && m_reachClock.elapsed() >= DwellMs;
            m_reachMask = Qt::NoDockWidgetArea;
            QPointer<QDockWidget> dock(m_dock);
            QPointer<QMainWindow> window(m_win);
            QTimer::singleShot(0, m_dock, [dock, window, releasePos, dwelled]() {
                if (dock) {
                    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
                }
                // Krimble 2026-10-18 (16th): George (1.0.37, c666301): "4 and 13 in super close range they should attach, not bounce." (13: "I can't get panels to attach at bottom or top
                // any more unless I just tell them in menu where to dock. Sometimes Tool options panel bounces.") Qt's own drop test for the top and bottom edges needs a very small
                // spot, and with a finger (and the 4 px of Distance above) the panel was refused and stayed where it was. When the panel is let go within 14 logical px of a
                // side of the window and Qt has not docked it, it is docked there, on the nearest side.
                if (!dock || !window || !dock->isFloating()) return;
                if (!dwelled) return;     // Krimble 2026-10-30: not deliberate: it stays where it was let go
                if (!krimbleDragDockingOn()) return;   // Krimble 2026-11-16 (728th): "Dock by Dragging" is off
                const QRect r(window->mapToGlobal(QPoint(0, 0)), window->size());
                const int toLeft = releasePos.x() - r.left();
                const int toRight = r.right() - releasePos.x();
                const int toTop = releasePos.y() - r.top();
                const int toBottom = r.bottom() - releasePos.y();
                const int nearest = qMin(qMin(toLeft, toRight), qMin(toTop, toBottom));
                // Krimble 2026-10-23 (2nd): George (1.0.42): "Right panels could stand to be a touch more eager to attach." The right side reaches twice as far (28 logical px).
                const int reach = (nearest == toRight) ? 28 : 14;
                if (nearest > reach) return;
                // const Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
                //                               : (nearest == toRight) ? Qt::RightDockWidgetArea
                //                               : (nearest == toTop) ? Qt::TopDockWidgetArea
                //                               : Qt::BottomDockWidgetArea;
                // window->addDockWidget(area, dock);
                Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
                                        : (nearest == toRight) ? Qt::RightDockWidgetArea
                                        : (nearest == toTop) ? Qt::TopDockWidgetArea
                                        : Qt::BottomDockWidgetArea;
                // Krimble 2026-11-15 (708th): George: "Dock right causes dock top on toolbox." The finger holds the toolbox by its handle, which is at its top: near a corner of the screen the top
                // edge could be the nearest one by a pixel or two, and the toolbox went to the top when it was let go at the right. For the toolbox a side within reach wins over top and bottom.
                // It is also given its own full-height column at the outer end of that side (Qt::Horizontal), or its own row (top, bottom), instead of a place among the panels there.
                if (dock->objectName() == QLatin1String("ToolBox")) {
                    // Krimble 2026-11-16 (728th): George (build 65): "I tried to leave the toolbox at the bottom ... but it snapped to the side." A side within reach won over the bottom
                    // too. It wins only over the TOP now (the case of the 708th: the handle is at the toolbox's top); let go at the bottom edge, the toolbox docks at the bottom.
                    if (area == Qt::TopDockWidgetArea) {
                        if (toRight <= 28) area = Qt::RightDockWidgetArea;
                        else if (toLeft <= 14) area = Qt::LeftDockWidgetArea;
                    }
                    const bool side = (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea);
                    window->addDockWidget(area, dock, side ? Qt::Horizontal : Qt::Vertical);
                } else {
                    window->addDockWidget(area, dock);
                }
                dock->setFloating(false);
                dock->show();
            });
        }
        return false;
    }

private:
    QMainWindow *m_win;
    QDockWidget *m_dock;
    bool m_dragging {false};
    QPoint m_startPos;   // Krimble 2026-10-18 (2nd): where the finger was when this drag began
    Qt::DockWidgetAreas m_reachMask {Qt::NoDockWidgetArea};   // Krimble 2026-10-30: the sides the finger is within reach of
    QElapsedTimer m_reachClock;                              // since when
};
#endif

// KRIMBLE 2026-10-04: the <text> that krita5.xmlgui gives to an <Action> was never used: an action's
// label comes from its own definition (the .action files or the code), so edits of the labels in
// krita5.xmlgui (for example Quit -> Exit) did not show. This applies them: every <Action> in the
// loaded menu file that carries a <text> sets the label of the action with that name.
// The "Hide/Show Right Panels" item and the hidden Attach/Detach items keep their own text.
static void krimbleApplyXmlGuiLabels(KisMainWindow *win)
{
    static QHash<QString, QString> labels;
    if (labels.isEmpty()) {
        const QDomDocument doc = win->domDocument();
        const QDomNodeList actions = doc.elementsByTagName(QStringLiteral("Action"));
        for (int i = 0; i < actions.count(); ++i) {
            const QDomElement element = actions.at(i).toElement();
            const QString name = element.attribute(QStringLiteral("name"));
            const QDomElement text = element.firstChildElement(QStringLiteral("text"));
            if (name.isEmpty() || text.isNull() || text.text().trimmed().isEmpty()) {
                continue;
            }
            if (name == QLatin1String("settings_toggle_right_panels")
                    || name == QLatin1String("settings_attach_panel_menu")
                    || name == QLatin1String("settings_detach_panel_menu")) {
                continue;
            }
            labels.insert(name, text.text().trimmed());
        }
    }
    if (labels.isEmpty()) {
        return;
    }
    // KRIMBLE 2026-10-04 (2nd): the first version only looked at actions that are children of the window. Quit stayed
    // "Quit" on the phone (build b12): actions belong to the action collection, which is not necessarily below the
    // window. Now the actions that are actually IN the menus are used as well (menu->actions()): those are the very
    // objects that are drawn.
    QList<QAction*> candidates;
    QList<QObject*> roots;
    roots << win << KoToolManager::instance();
    Q_FOREACH (QObject *root, roots) {
        candidates << root->findChildren<QAction*>();
    }
    Q_FOREACH (QMenu *menu, win->findChildren<QMenu*>()) {
        candidates << menu->actions();
    }
    if (win->menuBar()) {
        Q_FOREACH (QMenu *menu, win->menuBar()->findChildren<QMenu*>()) {
            candidates << menu->actions();
        }
    }
    Q_FOREACH (QAction *action, candidates) {
        const auto it = labels.constFind(action->objectName());
        if (it != labels.constEnd() && action->text() != it.value()) {
            action->setText(it.value());
        }
    }
}



