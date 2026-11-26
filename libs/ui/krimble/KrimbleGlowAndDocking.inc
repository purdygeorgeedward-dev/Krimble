
#ifdef Q_OS_ANDROID
static void krimbleSetButtonGlow(QAbstractButton *button, bool on)
{
    if (on) {
        if (button->graphicsEffect()) return;
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(button);
        glow->setColor(QColor(11, 135, 221, 230));
        glow->setBlurRadius(18);
        glow->setOffset(0, 0);
        button->setGraphicsEffect(glow);
    } else if (qobject_cast<QGraphicsDropShadowEffect*>(button->graphicsEffect())) {
        button->setGraphicsEffect(nullptr);
    }
}

static bool krimbleAquaButtonEligible(QAbstractButton *button)
{
    if (!button) return false;
    if (QPushButton *push = qobject_cast<QPushButton*>(button)) return push->isEnabled() && !push->isFlat();
    if (!button->isCheckable()) return false;
    if (QToolButton *tool = qobject_cast<QToolButton*>(button)) {
        if (tool->menu() && tool->popupMode() == QToolButton::MenuButtonPopup) return false;
        return true;
    }
    return qobject_cast<QPushButton*>(button) != nullptr;
}

static void krimblePaintAquaButton(QAbstractButton *button)
{
    QPainter p(button);
    const bool mono = qApp->property("krimbleBlackWhiteTheme").toBool();
    const QColor labelColor = mono && !button->isChecked() && !button->isDown() ? QColor(Qt::black) : QColor(Qt::white);
    if (mono) krimblePaintMonoButton(p, QRectF(button->rect()), button->devicePixelRatioF(), button->isChecked() || button->isDown());
    else if (button->isChecked()) krimblePaintSelectedGlow(p, QRectF(button->rect()), button->devicePixelRatioF());
    else krimblePaintBevelButton(p, QRectF(button->rect()), button->devicePixelRatioF(), button->palette().color(QPalette::Window), button->isDown());
    if (QToolButton *tool = qobject_cast<QToolButton*>(button)) {
        QStyleOptionToolButton opt;
        opt.initFrom(tool);
        opt.rect = tool->rect();
        opt.icon = tool->icon();
        opt.iconSize = tool->iconSize();
        opt.text = tool->text();
        opt.toolButtonStyle = tool->toolButtonStyle();
        opt.font = tool->font();
        opt.arrowType = tool->arrowType();
        if (opt.arrowType != Qt::NoArrow) opt.features |= QStyleOptionToolButton::Arrow;
        if (tool->isChecked()) opt.state |= QStyle::State_On;
        opt.palette.setColor(QPalette::ButtonText, labelColor);
        tool->style()->drawControl(QStyle::CE_ToolButtonLabel, &opt, &p, tool);
    } else if (QPushButton *push = qobject_cast<QPushButton*>(button)) {
        QStyleOptionButton opt;
        opt.initFrom(push);
        opt.text = push->text();
        opt.icon = push->icon();
        opt.iconSize = push->iconSize();
        if (push->isChecked()) opt.state |= QStyle::State_On;
        opt.palette.setColor(QPalette::ButtonText, labelColor);
        push->style()->drawControl(QStyle::CE_PushButtonLabel, &opt, &p, push);
    }
}

class KrimbleAquaButtonPainter : public QObject
{
public:
    explicit KrimbleAquaButtonPainter(QObject *parent) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Paint) {
            QAbstractButton *button = static_cast<QAbstractButton*>(watched);
            if (qApp->property("krimbleCyberTheme").toBool()) return false;
            if (krimbleAquaButtonEligible(button)) {
                krimblePaintAquaButton(button);
                return true;
            }
        }
        return false;
    }
};

class KrimbleScrollBarGrip : public QObject
{
public:
    explicit KrimbleScrollBarGrip(QObject *parent) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::Paint || m_busy) return false;
        QScrollBar *bar = qobject_cast<QScrollBar*>(watched);
        if (!bar || bar->maximum() <= bar->minimum()) return false;
        m_busy = true;
        watched->event(event);
        m_busy = false;
        QStyleOptionSlider opt;
        opt.initFrom(bar);
        opt.orientation = bar->orientation();
        opt.minimum = bar->minimum();
        opt.maximum = bar->maximum();
        opt.sliderPosition = bar->sliderPosition();
        opt.sliderValue = bar->value();
        opt.pageStep = bar->pageStep();
        opt.singleStep = bar->singleStep();
        opt.upsideDown = bar->invertedAppearance();
        opt.subControls = QStyle::SC_All;
        const QRectF handle = bar->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, bar);
        if (handle.width() < 6 || handle.height() < 6) return true;
        const QColor base = bar->palette().color(QPalette::Window);
        const QColor light = base.lighter(165);
        const QColor dark = base.darker(230);
        QPainter p(bar);
        const QRectF r = handle.adjusted(0.5, 0.5, -0.5, -0.5);
        p.setPen(QPen(light, 1));
        p.drawLine(r.topLeft(), r.topRight());
        p.drawLine(r.topLeft(), r.bottomLeft());
        p.setPen(QPen(dark, 1));
        p.drawLine(r.bottomLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomRight());
        const bool vertical = bar->orientation() == Qt::Vertical;
        const qreal gap = 3.0;
        const int ridges = qMax(2, int((vertical ? handle.height() : handle.width()) * 0.40 / gap));
        const qreal across = (vertical ? handle.width() : handle.height()) * 0.55;
        const QPointF c = handle.center();
        for (int i = 0; i < ridges; ++i) {
            const qreal offset = (i - (ridges - 1) / 2.0) * gap;
            if (vertical) {
                const qreal y = qRound(c.y() + offset);
                p.setPen(QPen(light, 1));
                p.drawLine(QPointF(c.x() - across / 2, y), QPointF(c.x() + across / 2, y));
                p.setPen(QPen(dark, 1));
                p.drawLine(QPointF(c.x() - across / 2, y + 1), QPointF(c.x() + across / 2, y + 1));
            } else {
                const qreal x = qRound(c.x() + offset);
                p.setPen(QPen(light, 1));
                p.drawLine(QPointF(x, c.y() - across / 2), QPointF(x, c.y() + across / 2));
                p.setPen(QPen(dark, 1));
                p.drawLine(QPointF(x + 1, c.y() - across / 2), QPointF(x + 1, c.y() + across / 2));
            }
        }
        return true;
    }
private:
    bool m_busy {false};
};

static void krimbleMenuBottomRoom(QMenu *menu)
{
    if (!menu) return;
    menu->setProperty("krimbleMenuRoom", true);
    menu->setContentsMargins(0, 0, 0, 16);
}

class KrimbleCyberTabPainter : public QObject
{
public:
    explicit KrimbleCyberTabPainter(QObject *parent) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::Paint || !qApp->property("krimbleCyberTheme").toBool()) return false;
        QTabBar *bar = qobject_cast<QTabBar*>(watched);
        if (!bar) return false;
        QPainter p(bar);
        QFont font(QStringLiteral("Rajdhani"));
        font.setWeight(QFont::DemiBold);
        for (int i = 0; i < bar->count(); ++i) {
            QRect r = bar->tabRect(i);
            r.setWidth(r.width() - 1);
            krimbleCyberPaintTab(p, QRectF(r), i == bar->currentIndex(), bar->tabText(i), font);
        }
        return true;
    }
};

class KrimbleGlowFilter : public QObject
{
public:
    explicit KrimbleGlowFilter(QObject *parent) : QObject(parent), m_painter(new KrimbleAquaButtonPainter(this)), m_grip(new KrimbleScrollBarGrip(this)) {}
    void hookScrollBar(QScrollBar *bar)
    {
        if (!bar || bar->property("krimbleGripHooked").toBool()) return;
        bar->setProperty("krimbleGripHooked", true);
        bar->installEventFilter(m_grip);
        bar->update();
    }
    void hook(QAbstractButton *button)
    {
        if (!button || button->property("krimbleGlowHooked").toBool()) return;
        if (!button->isCheckable() && !qobject_cast<QPushButton*>(button)) return;
        if (QPushButton *push = qobject_cast<QPushButton*>(button)) {
            if (QByteArray(push->metaObject()->className()) != "QPushButton" || !push->styleSheet().isEmpty()) return;
        }
        if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;
        if (QByteArray(button->metaObject()->className()) == "KoToolBoxButton") return;
        if (KoGroupButton *grouped = qobject_cast<KoGroupButton*>(button)) {
            if (grouped->groupPosition() != KoGroupButton::NoGroup) return;
        }
        button->setProperty("krimbleGlowHooked", true);
        krimbleSetButtonGlow(button, false);
        QObject::connect(button, &QAbstractButton::toggled, button, [button](bool) { button->update(); });
        button->installEventFilter(m_painter);
        button->update();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QAbstractButton *button = qobject_cast<QAbstractButton*>(watched)) hook(button);
            if (QScrollBar *bar = qobject_cast<QScrollBar*>(watched)) hookScrollBar(bar);
            if (QTabBar *tabs = qobject_cast<QTabBar*>(watched)) {
                if (qobject_cast<QMainWindow*>(tabs->parentWidget()) && !tabs->property("krimbleCyberTabs").toBool()) {
                    tabs->setProperty("krimbleCyberTabs", true);
                    tabs->installEventFilter(new KrimbleCyberTabPainter(tabs));
                }
            }
            if (QMenu *menu = qobject_cast<QMenu*>(watched)) {
                if (qApp->property("krimbleSeeThroughMenus").toBool() && !menu->testAttribute(Qt::WA_TranslucentBackground)) menu->setAttribute(Qt::WA_TranslucentBackground);
                if (!menu->property("krimbleMenuDecor").toBool()) {
                    menu->setProperty("krimbleMenuDecor", true);
                    menu->installEventFilter(new KrimbleMenuColumnDividers(menu));
                    krimbleMenuBottomRoom(menu);
                }
            }
        }
        if (event->type() == QEvent::Show) {
            if (QMenu *menu = qobject_cast<QMenu*>(watched)) {
                if (!menu->property("krimbleMenuRoom").toBool()) {
                    krimbleMenuBottomRoom(menu);
                    menu->resize(menu->sizeHint());
                }
            }
        }
        return false;
    }
private:
    KrimbleAquaButtonPainter *m_painter;
    KrimbleScrollBarGrip *m_grip;
};

static void krimbleInstallSelectionGlow(KisMainWindow *win)
{
    Q_FOREACH (QWidget *krimbleWidget, QApplication::allWidgets()) {
        if (QMenu *menu = qobject_cast<QMenu*>(krimbleWidget)) {
            if (!menu->property("krimbleMenuRoom").toBool()) krimbleMenuBottomRoom(menu);
            if (!menu->property("krimbleMenuDecor").toBool()) {
                menu->setProperty("krimbleMenuDecor", true);
                menu->installEventFilter(new KrimbleMenuColumnDividers(menu));
            }
        }
    }
    if (win->findChild<KrimbleGlowFilter*>(QStringLiteral("krimbleGlowFilter"))) return;
    KrimbleGlowFilter *filter = new KrimbleGlowFilter(win);
    filter->setObjectName(QStringLiteral("krimbleGlowFilter"));
    qApp->installEventFilter(filter);
    Q_FOREACH (QScrollBar *bar, win->findChildren<QScrollBar*>()) filter->hookScrollBar(bar);
    Q_FOREACH (QAbstractButton *button, win->findChildren<QAbstractButton*>()) {
        filter->hook(button);
    }
}

static void krimbleInstallHexMenus(KisMainWindow *win)
{
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
                        if (QAction *lockAct = sub->findChild<QAction*>(QStringLiteral("krimbleHexLock"))) {
                            QSignalBlocker lockBlocker(lockAct);
                            lockAct->setChecked(krimbleHexLocked());
                        }
                        continue;
                    }
                    QAction *hexAction = new QAction(i18n("Hex"), sub);
                    hexAction->setObjectName(QStringLiteral("krimbleHex"));
                    hexAction->setCheckable(true);
                    QObject::connect(hexAction, &QAction::toggled, win, [win](bool on) {
                        g_krimbleHexOn = on;
                        krimbleHexSaveState();
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, on);
                        if (!deco) return;
                        deco->size = g_krimbleHexSize;
                        deco->flatTop = g_krimbleHexFlat;
                        deco->lineWidth = g_krimbleHexLineWidth;
                        deco->lineColor = g_krimbleHexLineColor;
                        deco->setVisible(on);
                        KisViewManager *manager = win->viewManager();
                        KisCanvas2 *hexCanvas = (manager && win->activeView()) ? win->activeView()->canvasBase() : nullptr;
                        if (on && hexCanvas && hexCanvas->canvasWidget() && !krimbleHexLocked()) {
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
                        if (krimbleHexRefuseWhenLocked(win, i18n("Hex Size"))) return;
                        qreal thickness = g_krimbleHexThickness;
                        QColor color = g_krimbleHexColor;
                        if (!krimbleAskHex(win, false, &g_krimbleHexSize, &g_krimbleHexFlat, &thickness, &color)) return;
                        krimbleHexSaveState();
                        KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, false);
                        if (deco) {
                            deco->size = g_krimbleHexSize;
                            deco->flatTop = g_krimbleHexFlat;
                            KisViewManager *manager = win->viewManager();
                            if (manager && win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
                        }
                    });
                    QAction *linesAction = new QAction(i18n("Hex Lines..."), sub);
                    linesAction->setObjectName(QStringLiteral("krimbleHexLines"));
                    QObject::connect(linesAction, &QAction::triggered, win, [win]() {
                        if (krimbleHexRefuseWhenLocked(win, i18n("Hex Lines"))) return;
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

class KrimbleTabKFilter : public QObject
{
public:
    explicit KrimbleTabKFilter(QTabBar *bar) : QObject(bar) { bar->installEventFilter(this); }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QTabBar *bar = qobject_cast<QTabBar *>(watched);
        if (!bar || event->type() != QEvent::Paint || m_painting) return false;
        const QIcon k = qApp->windowIcon();
        if (k.isNull()) return false;
        const qreal dpr = qMax<qreal>(1.0, bar->devicePixelRatioF());
        const QSize iconSize = bar->iconSize();
        if (m_blank.isNull() || m_blankSize != iconSize) {
            QPixmap blank(iconSize * dpr);
            blank.setDevicePixelRatio(dpr);
            blank.fill(Qt::transparent);
            m_blank = QIcon(blank);
            m_blankSize = iconSize;
        }
        for (int i = 0; i < bar->count(); ++i) {
            if (!bar->tabIcon(i).isNull() && bar->tabIcon(i).cacheKey() != m_blank.cacheKey()) bar->setTabIcon(i, m_blank);
        }
        m_painting = true;
        static_cast<QObject *>(bar)->event(event);
        m_painting = false;
        const int kSize = iconSize.height();
        const QPixmap kPix = k.pixmap(kSize, kSize);
        const QImage kImage = kPix.toImage().convertToFormat(QImage::Format_ARGB32);
        int kBlank = kImage.width();
        for (int y = 0; y < kImage.height(); ++y) {
            for (int x = 0; x < kBlank; ++x) {
                if (qAlpha(kImage.pixel(x, y)) > 40) { kBlank = x; break; }
            }
        }
        if (kBlank >= kImage.width()) kBlank = 0;
        const qreal kPixDpr = qMax<qreal>(1.0, kPix.devicePixelRatioF());
        QPainter painter(bar);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        for (int i = 0; i < bar->count(); ++i) {
            const QRect tab = bar->tabRect(i);
            const qreal kLeft = tab.left() + 17.0 / dpr - kBlank / kPixDpr;
            painter.drawPixmap(QRectF(kLeft, tab.top() + (tab.height() - kSize) / 2.0, kSize, kSize), kPix, QRectF(QPointF(0, 0), QSizeF(kPix.size())));
        }
        return true;
    }
private:
    QIcon m_blank;
    QSize m_blankSize;
    bool m_painting {false};
};

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
    static const int Distance = 4;
    static const int DwellMs = 350;

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
                const QRect r(m_win->mapToGlobal(QPoint(0, 0)), m_win->size());
                Qt::DockWidgetAreas allowed = Qt::NoDockWidgetArea;
                if (p.x() - r.left() < Distance) allowed |= Qt::LeftDockWidgetArea;
                if (r.right() - p.x() < Distance) allowed |= Qt::RightDockWidgetArea;
                if (p.y() - r.top() < Distance) allowed |= Qt::TopDockWidgetArea;
                if (r.bottom() - p.y() < Distance) allowed |= Qt::BottomDockWidgetArea;
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
                        allowed = Qt::NoDockWidgetArea;
                    }
                }
                if (m_dock->objectName() == QLatin1String("ToolBox")) allowed = Qt::NoDockWidgetArea;
                if (!krimbleDragDockingOn()) allowed = Qt::NoDockWidgetArea;
                if (m_dock->allowedAreas() != allowed) {
                    m_dock->setAllowedAreas(allowed);
                }
                m_dragging = true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_dragging) {
            m_dragging = false;
            const QPoint releasePos = static_cast<QMouseEvent*>(event)->globalPos();
            const bool dwelled = m_reachMask != Qt::NoDockWidgetArea && m_reachClock.isValid() && m_reachClock.elapsed() >= DwellMs;
            m_reachMask = Qt::NoDockWidgetArea;
            QPointer<QDockWidget> dock(m_dock);
            QPointer<QMainWindow> window(m_win);
            QTimer::singleShot(0, m_dock, [dock, window, releasePos, dwelled]() {
                if (dock) {
                    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
                }
                if (!dock || !window || !dock->isFloating()) return;
                if (!dwelled) return;
                if (!krimbleDragDockingOn()) return;
                const QRect r(window->mapToGlobal(QPoint(0, 0)), window->size());
                const int toLeft = releasePos.x() - r.left();
                const int toRight = r.right() - releasePos.x();
                const int toTop = releasePos.y() - r.top();
                const int toBottom = r.bottom() - releasePos.y();
                const int nearest = qMin(qMin(toLeft, toRight), qMin(toTop, toBottom));
                const int reach = (nearest == toRight) ? 28 : 14;
                if (nearest > reach) return;
                Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
                                        : (nearest == toRight) ? Qt::RightDockWidgetArea
                                        : (nearest == toTop) ? Qt::TopDockWidgetArea
                                        : Qt::BottomDockWidgetArea;
                if (dock->objectName() == QLatin1String("ToolBox")) {
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
    Qt::DockWidgetAreas m_reachMask {Qt::NoDockWidgetArea};
    QElapsedTimer m_reachClock;
};
#endif

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

