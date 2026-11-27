
#ifdef Q_OS_ANDROID
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
        if (qApp->property("krimbleFlatTheme").toBool()) return;
        if (QMdiSubWindow *krimbleSub = qobject_cast<QMdiSubWindow*>(m_target.data())) {
            if (krimbleSub->isMaximized()) return;
        }
        if (QToolBar *krimbleToolBar = qobject_cast<QToolBar*>(m_target.data())) {
            if (krimbleToolBar->height() < 24 || krimbleToolBar->actions().isEmpty()) return;
        }
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        const bool menuBarTarget = qobject_cast<QMenuBar*>(m_target.data()) != nullptr;
        const bool krimbleToolBoxTarget = m_target && m_target->objectName() == QLatin1String("ToolBox");
        const qreal one = 3.0 / qMax<qreal>(1.0, devicePixelRatioF());
        const bool recessed = qobject_cast<QMenu*>(m_target.data()) != nullptr;
        if (krimbleToolBoxTarget && !recessed) {
            if (qApp->property("krimbleBlackWhiteTheme").toBool()) {
                const qreal edge = 2.0 / qMax<qreal>(1.0, devicePixelRatioF());
                p.fillRect(QRectF(0, 0, width(), edge), Qt::black);
                p.fillRect(QRectF(0, height() - edge, width(), edge), Qt::black);
                p.fillRect(QRectF(0, 0, edge, height()), Qt::black);
                p.fillRect(QRectF(width() - edge, 0, edge, height()), Qt::black);
                return;
            }
            const qreal tb = qMin(7.0 / qMax<qreal>(1.0, devicePixelRatioF()), qMin<qreal>(width(), height()) / 2.0);
            const qreal w = width();
            const qreal h = height();
            p.setPen(Qt::NoPen);
            const qreal top = qMin(tb, 3.0 / qMax<qreal>(1.0, devicePixelRatioF()));
            p.setBrush(QColor(255, 255, 255, 85));
            { QPolygonF poly; poly << QPointF(0, 0) << QPointF(w, 0) << QPointF(w - tb, top) << QPointF(tb, top); p.drawPolygon(poly); }
            p.setBrush(QColor(255, 255, 255, 40));
            { QPolygonF poly; poly << QPointF(0, 0) << QPointF(tb, top) << QPointF(tb, h - tb) << QPointF(0, h); p.drawPolygon(poly); }
            p.setBrush(QColor(0, 0, 0, 55));
            { QPolygonF poly; poly << QPointF(w, 0) << QPointF(w, h) << QPointF(w - tb, h - tb) << QPointF(w - tb, top); p.drawPolygon(poly); }
            p.setBrush(QColor(0, 0, 0, 90));
            { QPolygonF poly; poly << QPointF(0, h) << QPointF(tb, h - tb) << QPointF(w - tb, h - tb) << QPointF(w, h); p.drawPolygon(poly); }
            return;
        }
        if (recessed) {
            const qreal edge = 2.0 / qMax<qreal>(1.0, devicePixelRatioF());
            p.setPen(Qt::NoPen);
            if (!m_target->property("krimbleBarMenu").toBool()) p.fillRect(QRectF(0, 0, width(), edge), Qt::black);
            p.fillRect(QRectF(0, height() - edge, width(), edge), Qt::black);
            p.fillRect(QRectF(0, 0, edge, height()), Qt::black);
            p.fillRect(QRectF(width() - edge, 0, edge, height()), Qt::black);
            return;
        }
#if 0
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
        const bool krimbleMenuBar = qobject_cast<QMenuBar*>(m_target.data()) != nullptr;
        Q_UNUSED(krimbleMenuBar);
        if (QMenuBar *kBar = qobject_cast<QMenuBar*>(m_target.data())) {
            const QFontMetrics kMetrics(kBar->font());
            const int kSize = qMin(kBar->height() - 4, qMax(14, kMetrics.height()));
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            qApp->windowIcon().paint(&p, QRect(2, (kBar->height() - kBar->property("krimbleBarExtra").toInt() - kSize) / 2, kSize, kSize));
            return;
        }
        const QColor krimbleBase = m_target ? m_target->palette().color(QPalette::Window) : palette().color(QPalette::Window);
        QColor topTone    = krimbleBase.lighter(recessed ? 30 : 170);
        if (!qobject_cast<QMenuBar*>(m_target.data()) && !recessed && m_target) {
            if (QMainWindow *krimbleWin = qobject_cast<QMainWindow*>(m_target->window())) {
                if (QMenuBar *krimbleBar = krimbleWin->menuBar()) {
                    const qreal krimbleGap = (m_target->mapToGlobal(QPoint(0, 0)).y() - krimbleBar->mapToGlobal(QPoint(0, krimbleBar->height())).y()) * qMax<qreal>(1.0, devicePixelRatioF());
                    if (krimbleGap >= -2.0 && krimbleGap <= 12.0) topTone = QColor(0, 0, 0, 0);
                }
            }
        }
        if (!recessed) { if (QDockWidget *krimbleTabbedDock = qobject_cast<QDockWidget*>(m_target.data())) { if (krimbleDockInTabGroup(krimbleTabbedDock)) topTone = QColor(0, 0, 0, 0); } }
        if (krimbleToolBoxTarget && !recessed && topTone.alpha() != 0) topTone = krimbleBase.lighter(250);
        const QColor leftTone   = krimbleBase.lighter(recessed ? 83 : (krimbleToolBoxTarget ? 210 : 150));
        const QColor rightTone  = krimbleBase.lighter(recessed ? 170 : (krimbleToolBoxTarget ? 48 : 90));
        const QColor bottomTone = krimbleBase.lighter(recessed ? 300 : (krimbleToolBoxTarget ? 22 : 60));
        const qreal w = width();
        const qreal h = height();
        const qreal t = qMin(one, qMin(w, h) / 2.0);
        const qreal tt = (krimbleToolBoxTarget && !recessed) ? qMin(t, 3.0 / qMax<qreal>(1.0, qApp->devicePixelRatio())) : t;
        const qreal ts = (krimbleToolBoxTarget && !recessed) ? qMin(one + 1.0 / qMax<qreal>(1.0, devicePixelRatioF()), qMin(w, h) / 2.0) : t;
        QDockWidget *dock = qobject_cast<QDockWidget*>(m_target.data());
        const bool allEdges = !dock || dock->isFloating() || krimbleToolBoxTarget;
        p.setPen(Qt::NoPen);
        if (!allEdges) {
            p.fillRect(QRectF(0, 0, w, t), topTone);
            p.fillRect(QRectF(0, h - t, w, t), bottomTone);
            p.setBrush(leftTone);
            { QPolygonF poly; poly << QPointF(0, 0) << QPointF(t, t) << QPointF(t, h - t) << QPointF(0, h); p.drawPolygon(poly); }
            return;
        }
        p.setBrush(topTone);
        { QPolygonF poly; poly << QPointF(0, 0) << QPointF(w, 0) << QPointF(w - ts, tt) << QPointF(t, tt); p.drawPolygon(poly); }
        if (krimbleToolBoxTarget && !recessed) {
            QLinearGradient shade(QPointF(0, h - ts), QPointF(0, h));
            shade.setColorAt(0.0, krimbleBase.lighter(85));
            shade.setColorAt(1.0, bottomTone);
            p.setBrush(shade);
        } else {
            p.setBrush(bottomTone);
        }
        if (QMenuBar *krimbleOpenBar = qobject_cast<QMenuBar*>(m_target.data())) {
            QAction *krimbleOpen = krimbleOpenBar->activeAction();
            if (krimbleOpen && krimbleOpen->menu() && krimbleOpen->menu()->isVisible()) {
                const QRect krimbleName = krimbleOpenBar->actionGeometry(krimbleOpen);
                p.setClipRegion(QRegion(rect()) - QRegion(krimbleName.left(), 0, krimbleName.width(), height()));
            }
        }
        { QPolygonF poly; poly << QPointF(0, h) << QPointF(w, h) << QPointF(w - ts, h - ts) << QPointF(t, h - ts); p.drawPolygon(poly); }
        p.setClipping(false);
        p.setBrush(leftTone);
        { QPolygonF poly; poly << QPointF(0, 0) << QPointF(t, tt) << QPointF(t, h - ts) << QPointF(0, h); p.drawPolygon(poly); }
        if (krimbleToolBoxTarget && !recessed) {
            QLinearGradient shade(QPointF(w - ts, 0), QPointF(w, 0));
            shade.setColorAt(0.0, krimbleBase.lighter(85));
            shade.setColorAt(1.0, rightTone);
            p.setBrush(shade);
        } else {
            p.setBrush(rightTone);
        }
        { QPolygonF poly; poly << QPointF(w, 0) << QPointF(w, h) << QPointF(w - ts, h - ts) << QPointF(w - ts, tt); p.drawPolygon(poly); }
        if (QMenuBar *kBar = qobject_cast<QMenuBar*>(m_target.data())) {
            const QFontMetrics kMetrics(kBar->font());
            const int kSize = qMin(kBar->height() - 4, qMax(14, kMetrics.height()));
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            qApp->windowIcon().paint(&p, QRect(2, (kBar->height() - kBar->property("krimbleBarExtra").toInt() - kSize) / 2, kSize, kSize));
        }
    }

private:
    QPointer<QWidget> m_target;
};

static void krimbleRefreshTabTitles(QMainWindow *win)
{
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar*>(dock->titleBarWidget());
        if (!bar) continue;
        const bool tabbed = krimbleDockInTabGroup(dock);
        QWidget *krimbleLogo = dock->findChild<QWidget*>(QStringLiteral("KrimbleStudioLogo"));
        if (!dock->property("krimbleTitleHooked").toBool()) {
            dock->setProperty("krimbleTitleHooked", true);
            QObject::connect(dock, &QDockWidget::topLevelChanged, win, [win](bool) { QTimer::singleShot(0, win, [win]() { krimbleRefreshTabTitles(win); }); });
        }
        bar->setTitleTextHidden(krimbleLogo ? true : tabbed);
        if (krimbleLogo && krimbleLogo->isHidden()) krimbleLogo->setVisible(true);
        if (krimbleLogo) {
            Q_FOREACH (QWidget *part, bar->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
                if (part->isVisible()) part->hide();
            }
            if (bar->layout()) bar->layout()->setContentsMargins(0, 0, 0, 0);
            if (bar->maximumHeight() != 0) bar->setFixedHeight(0);
        }
    }
}

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
            if (me->pos().y() >= -limit && me->pos().y() <= m_bar->height() + limit) break;
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

class KrimbleTabTearOff : public QObject
{
public:
    explicit KrimbleTabTearOff(QObject *parent = nullptr) : QObject(parent) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QTabBar *bar = qobject_cast<QTabBar *>(watched);
        if (!bar) return false;
        QMainWindow *win = qobject_cast<QMainWindow *>(bar->parentWidget());
        if (!win) return false;
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
            m_press = pos;
            m_taken = false;
            m_dock = nullptr;
            m_maxOut = 0;
            log(QStringLiteral("press on a tab bar with %1 tabs, tab %2, at %3,%4 (bar %5 x %6)").arg(bar->count()).arg(m_tab).arg(pos.x()).arg(pos.y()).arg(bar->width()).arg(bar->height()));
            return false;
        }
        if (bar != m_bar) return false;
        if (isMove && down) {
            if (m_taken) {
                if (m_dock) {
                    const QPoint g = bar->mapToGlobal(pos);
                    m_dock->move(g - m_grab);
                }
                return true;
            }
            if (m_tab < 0) return false;
            const int outside = pos.y() < 0 ? -pos.y() : (pos.y() > bar->height() ? pos.y() - bar->height() : 0);
            m_maxOut = qMax(m_maxOut, outside);
            const int limit = qMax(bar->height() / 2, 16);
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
    static void paintTabRow(QTabBar *bar)
    {
        QPainter p(bar);
        p.setRenderHint(QPainter::Antialiasing, true);
        const QPalette pal = bar->palette();
        const QColor panel = pal.color(QPalette::Window);
        const QColor dark = panel.darker(130);
        const QColor strip = panel.darker(300);
        const QColor edge(0, 0, 0);
        const QColor textOn = pal.color(QPalette::WindowText).lighter(115);
        const QColor textOff = QColor(int(textOn.red() * 0.7), int(textOn.green() * 0.7), int(textOn.blue() * 0.75));
        const qreal r = 16.0, k = 10.0;
        const qreal baseY = bar->height() - 0.5;
        p.fillRect(bar->rect(), strip);
        const int current = bar->currentIndex();
        for (int pass = 0; pass < 2; ++pass) {
            if (pass == 1) {
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
                if (selected) {
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
    Q_FOREACH (QTabBar *bar, win->findChildren<QTabBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        if (!bar->findChild<KrimbleTabDetachFilter*>(QString(), Qt::FindDirectChildrenOnly)) new KrimbleTabDetachFilter(bar, win);
    }
}

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
        static const bool krimbleOverlayOn = QSettings().value(QStringLiteral("krimble/PanelsOverlayCanvas"), false).toBool();
        if (!krimbleOverlayOn) return;
        const QRect now = m_central->geometry();
        const QRect wanted(0, now.top(), m_win->width(), now.height());
        if (now == wanted) return;
        m_busy = true;
        m_central->setGeometry(wanted);
        m_central->lower();
        m_busy = false;
    }
    QPointer<QMainWindow> m_win;
    QPointer<QWidget> m_central;
    bool m_pending = false;
    bool m_busy = false;
};

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
    plain.remove(QRegularExpression(QStringLiteral("/\\*.*?\\*/"), QRegularExpression::DotMatchesEverythingOption));
    QRegularExpressionMatchIterator it = QRegularExpression(QStringLiteral("QMainWindow\\s*,\\s*QDockWidget\\s*\\{([^}]*)\\}")).globalMatch(plain);
    QString body;
    while (it.hasNext()) body = it.next().captured(1);
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
            p.fillRect(r, dock->palette().color(QPalette::Window));
        }
        return false;
    }
};

static void krimbleInstallBevels(QMainWindow *win)
{
    Q_FOREACH (QMenuBar *menuBar, win->findChildren<QMenuBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        if (!menuBar->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(menuBar);
    }
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (!dock->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(dock);
        if (!dock->findChild<KrimbleDockBackground*>(QStringLiteral("krimbleDockBackground"), Qt::FindDirectChildrenOnly)) new KrimbleDockBackground(dock);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(bar);
    }
    Q_FOREACH (QMenu *menu, win->findChildren<QMenu*>()) {
        if (!menu->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(menu);
    }
    Q_FOREACH (QMdiSubWindow *sub, win->findChildren<QMdiSubWindow*>()) {
        if (!sub->findChild<KrimbleBevelOverlay*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly)) new KrimbleBevelOverlay(sub);
    }
    static bool s_tabTitlesHooked = false;
    if (!s_tabTitlesHooked) {
        s_tabTitlesHooked = true;
        QObject::connect(win, &QMainWindow::tabifiedDockWidgetActivated, win, [win](QDockWidget*) { krimbleRefreshTabTitles(win); });
    }
    krimbleRefreshTabTitles(win);
}
#endif
