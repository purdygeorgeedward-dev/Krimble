
#ifdef Q_OS_ANDROID
static void krimbleSetButtonGlow(QAbstractButton *button, bool on)
{
    if (on) {
        if (button->graphicsEffect()) return;       // it has an effect already (this one, or another that is not ours)
        QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(button);
        glow->setColor(QColor(11, 135, 221, 230));  // the selection blue (#0b87dd)
        glow->setBlurRadius(18);
        glow->setOffset(0, 0);
        button->setGraphicsEffect(glow);
    } else if (qobject_cast<QGraphicsDropShadowEffect*>(button->graphicsEffect())) {
        button->setGraphicsEffect(nullptr);
    }
}

/*
class KrimbleGlowFilter : public QObject
{
public:
    explicit KrimbleGlowFilter(QObject *parent) : QObject(parent) {}
    void hook(QAbstractButton *button)
    {
        if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
        if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;   // not check boxes or radio buttons
        if (QByteArray(button->metaObject()->className()) == "KoToolBoxButton") return;   // the toolbox selection is flat, no glow
        if (KoGroupButton *grouped = qobject_cast<KoGroupButton*>(button)) {
            if (grouped->groupPosition() != KoGroupButton::NoGroup) return;   // it paints the Aqua-style selection itself
        }
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

*/
// a checked button draws the Aqua-style selection (KrimbleAquaPaint.h) under its icon and text, in place of the halo glow
static bool krimbleAquaButtonEligible(QAbstractButton *button)
{
    // if (!button || !button->isCheckable() || !button->isChecked()) return false;
    if (!button) return false;
    // one Aqua: every plain push button (OK, Cancel, Create, Copy...) is painted like the checked tool button
    if (QPushButton *push = qobject_cast<QPushButton*>(button)) {
        if (!push->isCheckable() || push->isChecked()) return push->isEnabled() && !push->isFlat();
    }
    if (!button->isCheckable() || !button->isChecked()) return false;
    if (QToolButton *tool = qobject_cast<QToolButton*>(button)) {
        if (tool->menu() && tool->popupMode() == QToolButton::MenuButtonPopup) return false;   // keeps its own look (the menu arrow part)
        return true;
    }
    return qobject_cast<QPushButton*>(button) != nullptr;
}

static void krimblePaintAquaButton(QAbstractButton *button)
{
    QPainter p(button);
    const QRectF r = QRectF(button->rect()).adjusted(1, 1, -1, -1);
    // krimblePaintAqua(p, r, qMin(r.width(), r.height()) * 0.10);
    // krimblePaintAqua(p, r, qMin(r.width(), r.height()) * 0.10, button->isDown() ? QColor(11, 135, 221).darker(135) : QColor(11, 135, 221));
    krimblePaintAqua(p, r, qMin(r.width(), r.height()) * 0.22, button->isDown() ? QColor(11, 135, 221).darker(135) : QColor(11, 135, 221));   // a little darker while pressed
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
        opt.state |= QStyle::State_On;
        opt.palette.setColor(QPalette::ButtonText, Qt::white);
        tool->style()->drawControl(QStyle::CE_ToolButtonLabel, &opt, &p, tool);
    } else if (QPushButton *push = qobject_cast<QPushButton*>(button)) {
        QStyleOptionButton opt;
        opt.initFrom(push);
        opt.text = push->text();
        opt.icon = push->icon();
        opt.iconSize = push->iconSize();
        opt.state |= QStyle::State_On;
        opt.palette.setColor(QPalette::ButtonText, Qt::white);
        push->style()->drawControl(QStyle::CE_PushButtonLabel, &opt, &p, push);
    }
}

// Krimble 2026-11-23: paints the Aqua on the buttons it is installed on. One is installed on each hooked button, so the work is per button,
// not a look at every paint of every widget in the app (George: "Change this back to once per widget.").
class KrimbleAquaButtonPainter : public QObject
{
public:
    explicit KrimbleAquaButtonPainter(QObject *parent) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Paint) {
            QAbstractButton *button = static_cast<QAbstractButton*>(watched);
            if (krimbleAquaButtonEligible(button)) {
                krimblePaintAquaButton(button);
                return true;   // the button's own checked background is not drawn
            }
        }
        return false;
    }
};

class KrimbleGlowFilter : public QObject
{
public:
    explicit KrimbleGlowFilter(QObject *parent) : QObject(parent), m_painter(new KrimbleAquaButtonPainter(this)) {}
    void hook(QAbstractButton *button)
    {
        // if (!button || !button->isCheckable() || button->property("krimbleGlowHooked").toBool()) return;
        if (!button || button->property("krimbleGlowHooked").toBool()) return;
        if (!button->isCheckable() && !qobject_cast<QPushButton*>(button)) return;   // plain push buttons get the Aqua too
        if (QPushButton *push = qobject_cast<QPushButton*>(button)) {
            // only Qt's own push button: subclasses (color buttons and the like) and buttons with their own style sheet keep their look
            if (QByteArray(push->metaObject()->className()) != "QPushButton" || !push->styleSheet().isEmpty()) return;
        }
        if (!qobject_cast<QToolButton*>(button) && !qobject_cast<QPushButton*>(button)) return;   // not check boxes or radio buttons
        if (QByteArray(button->metaObject()->className()) == "KoToolBoxButton") return;   // the toolbox selection is flat, no glow
        if (KoGroupButton *grouped = qobject_cast<KoGroupButton*>(button)) {
            if (grouped->groupPosition() != KoGroupButton::NoGroup) return;   // it paints the Aqua-style selection itself
        }
        button->setProperty("krimbleGlowHooked", true);
        // QObject::connect(button, &QAbstractButton::toggled, button, [button](bool on) { krimbleSetButtonGlow(button, on); });
        // krimbleSetButtonGlow(button, button->isChecked());
        krimbleSetButtonGlow(button, false);   // a halo from before is taken off
        QObject::connect(button, &QAbstractButton::toggled, button, [button](bool) { button->update(); });
        button->installEventFilter(m_painter);   // this button's paints only
        button->update();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Polish) {
            if (QAbstractButton *button = qobject_cast<QAbstractButton*>(watched)) hook(button);
            // 868th, 875th: column lines and the cast shadow on every menu (KrimbleMenuColumnDividers.h)
            if (QMenu *menu = qobject_cast<QMenu*>(watched)) {
                if (!menu->property("krimbleMenuDecor").toBool()) {
                    menu->setProperty("krimbleMenuDecor", true);
                    menu->installEventFilter(new KrimbleMenuColumnDividers(menu));
                    // Krimble Cyber (cyber-theme branch): see-through menus; the theme's border image draws their shape
                    if (qApp->property("krimbleSeeThroughMenus").toBool()) menu->setAttribute(Qt::WA_TranslucentBackground);   // Cyber and Glass
                }
            }
        }
        // once per widget: Polish comes once, when a widget is first shown; buttons made before the filter was installed are hooked by
        // krimbleInstallSelectionGlow. From the 840th to the 860th every paint of every widget in the app was looked at here.
        return false;
    }
private:
    KrimbleAquaButtonPainter *m_painter;
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
                        deco->lineWidth = g_krimbleHexLineWidth;   // task 41
                        deco->lineColor = g_krimbleHexLineColor;
                        deco->setVisible(on);
                        KisViewManager *manager = win->viewManager();
                        KisCanvas2 *hexCanvas = (manager && win->activeView()) ? win->activeView()->canvasBase() : nullptr;
                        if (on && hexCanvas && hexCanvas->canvasWidget() && !krimbleHexLocked()) {
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
                const QString title = copy.text;
                copy.text = QString();
                copy.subControls &= ~QStyle::SC_TitleBarSysMenu;   // the K is drawn below, pinned to the corner
                QProxyStyle::drawComplexControl(control, &copy, painter, widget);
                if (!titleBar->icon.isNull()) {
                    const int kSize = QProxyStyle::pixelMetric(QStyle::PM_SmallIconSize, titleBar, widget);
                    const qreal dpr = widget ? widget->devicePixelRatioF() : qApp->devicePixelRatio();
                    const QPixmap kPix = titleBar->icon.pixmap(kSize, kSize);
                    const QImage kImage = kPix.toImage().convertToFormat(QImage::Format_ARGB32);
                    int kBlank = kImage.width();   // empty columns at the left of the K picture, in real pixels
                    for (int y = 0; y < kImage.height(); ++y) {
                        for (int x = 0; x < kBlank; ++x) {
                            if (qAlpha(kImage.pixel(x, y)) > 40) { kBlank = x; break; }
                        }
                    }
                    if (kBlank >= kImage.width()) kBlank = 0;
                    const qreal kPixDpr = qMax<qreal>(1.0, kPix.devicePixelRatioF());
                    const qreal kLeft = titleBar->rect.left() + 17.0 / qMax<qreal>(1.0, dpr) - kBlank / kPixDpr;   // the visible K starts 17 real pixels from the window's left edge
                    const QRectF kRect(kLeft, titleBar->rect.top() + (titleBar->rect.height() - kSize) / 2.0, kSize, kSize);
                    painter->drawPixmap(kRect, kPix, QRectF(QPointF(0, 0), QSizeF(kPix.size())));
                }
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

// The image tabs: Qt's K is replaced by an empty picture of the same size (so the names stay where they are) and the K is
// drawn after the tab bar, its visible left edge 17 real pixels from each tab's left edge, the same as the image window title bar.
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
        int kBlank = kImage.width();   // empty columns at the left of the K picture, in real pixels
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
                if (!m_dragging) {
                    m_startPos = p;
                    m_others.clear();
                    if (m_dock->objectName() != QLatin1String("ToolBox")) {
                        Q_FOREACH (QDockWidget *other, m_win->findChildren<QDockWidget*>()) {
                            if (other != m_dock && !other->isFloating() && other->isVisible()) m_others.append(other);
                        }
                    }
                }
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
                        allowed = Qt::NoDockWidgetArea;       // not deliberate (yet): no side of the window (panels to tab with are added below)
                    }
                }
                const bool farEnough = QLineF(QPointF(p), QPointF(m_startPos)).length() >= 120.0;
                for (const QPointer<QDockWidget> &other : qAsConst(m_others)) {
                    if (!farEnough) break;
                    if (!other || other->isFloating() || !other->isVisible()) continue;
                    const QRect otherRect(other->mapToGlobal(QPoint(0, 0)), other->size());
                    if (otherRect.contains(p)) allowed |= m_win->dockWidgetArea(other);
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
            m_others.clear();
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
                // const Qt::DockWidgetArea area = (nearest == toLeft) ? Qt::LeftDockWidgetArea
                //                               : (nearest == toRight) ? Qt::RightDockWidgetArea
                //                               : (nearest == toTop) ? Qt::TopDockWidgetArea
                //                               : Qt::BottomDockWidgetArea;
                // window->addDockWidget(area, dock);
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
    QPoint m_startPos;
    QVector<QPointer<QDockWidget>> m_others;                 // the docked panels, found once when the drag starts
    Qt::DockWidgetAreas m_reachMask {Qt::NoDockWidgetArea};
    QElapsedTimer m_reachClock;                              // since when
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



