
class KrimbleResizeGrip : public QWidget
{
public:
    explicit KrimbleResizeGrip(QWidget *dialog) : QWidget(dialog)
    {
        setFixedSize(26, 26);
        setCursor(Qt::SizeFDiagCursor);
        setAttribute(Qt::WA_NoSystemBackground);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (property("krimbleInvisible").toBool()) return;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        QColor c = palette().color(QPalette::WindowText);
        c.setAlpha(150);
        QPen pen(c);
        pen.setWidthF(1.6);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);
        const qreal w = width();
        const qreal h = height();
        for (int i = 0; i < 3; ++i) {
            // const qreal off = 8 + i * 7;
            const qreal off = 6 + i * 5;
            p.drawLine(QPointF(w - 5, h - off), QPointF(w - off, h - 5));
        }
    }
    void mousePressEvent(QMouseEvent *e) override
    {
        m_start = e->globalPos();
        m_startSize = target()->size();
        e->accept();
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton)) return;
        const QPoint d = e->globalPos() - m_start;
        QWidget *w = target();
        QSize size(m_startSize.width() + d.x(), m_startSize.height() + d.y());
        size = size.expandedTo(w->minimumSize().expandedTo(QSize(200, 120))).boundedTo(w->maximumSize());
        w->resize(size);
        e->accept();
    }
    void mouseReleaseEvent(QMouseEvent *e) override
    {
        e->accept();
    }

private:
    QWidget *target() const
    {
        if (QMdiSubWindow *sub = qobject_cast<QMdiSubWindow *>(parentWidget())) return sub;
        return window();
    }
    QPoint m_start;
    QSize m_startSize;
};

#ifdef Q_OS_ANDROID
static void krimbleRestackGrid(QGridLayout *grid)
{
    struct Entry { QLayoutItem *item; int row; int col; int rowSpan; int colSpan; };
    QList<Entry> entries;
    int maxCol = 0;
    for (int i = 0; i < grid->count(); ++i) {
        Entry e;
        e.item = grid->itemAt(i);
        grid->getItemPosition(i, &e.row, &e.col, &e.rowSpan, &e.colSpan);
        if (!e.item || e.item->layout() || e.rowSpan != 1) return;    // anything unusual: leave this grid as it is
        maxCol = qMax(maxCol, e.col + e.colSpan - 1);
        entries << e;
    }
    QMap<int, QList<Entry> > rows;
    Q_FOREACH (const Entry &e, entries) rows[e.row] << e;
    // rows that are exactly a label in the first column and a control (not another label) in the second
    auto isLabelControlRow = [](const QList<Entry> &row) {
        if (row.size() != 2) return false;
        const Entry &a = row.at(0).col == 0 ? row.at(0) : row.at(1);
        const Entry &b = row.at(0).col == 0 ? row.at(1) : row.at(0);
        if (a.col != 0 || a.colSpan != 1 || b.col != 1 || b.colSpan != 1) return false;
        QLabel *label = a.item->widget() ? qobject_cast<QLabel*>(a.item->widget()) : nullptr;
        QWidget *control = b.item->widget();
        return label && control && !qobject_cast<QLabel*>(control);
    };
    bool any = false;
    for (auto it = rows.constBegin(); it != rows.constEnd(); ++it) {
        if (isLabelControlRow(it.value())) any = true;
    }
    if (!any || maxCol < 1) return;

    Q_FOREACH (const Entry &e, entries) grid->removeItem(e.item);
    int newRow = 0;
    for (auto it = rows.constBegin(); it != rows.constEnd(); ++it) {
        const QList<Entry> &row = it.value();
        if (isLabelControlRow(row)) {
            const Entry &a = row.at(0).col == 0 ? row.at(0) : row.at(1);
            const Entry &b = row.at(0).col == 0 ? row.at(1) : row.at(0);
            if (QLabel *label = qobject_cast<QLabel*>(a.item->widget())) {
                label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }
            grid->addItem(a.item, newRow, 0, 1, maxCol + 1);
            grid->addItem(b.item, newRow + 1, 0, 1, maxCol + 1);
            newRow += 2;
        } else {
            Q_FOREACH (const Entry &e, row) {
                grid->addItem(e.item, newRow, e.col, 1, e.colSpan);
            }
            newRow += 1;
        }
    }
}

static int krimbleDialogMinWidth(QDialog *dialog)
{
    int w = dialog->minimumSizeHint().width();
    if (QScrollArea *scroll = dialog->findChild<QScrollArea*>(QStringLiteral("krimbleDialogScroll"))) {
        if (scroll->widget()) {
            w = qMax(w, scroll->widget()->minimumSizeHint().width() + 2 * scroll->frameWidth() + 24);   // 24: the window's margins and a scroll bar
        }
    }
    return w;
}

static bool krimbleKeepsRows(QObject *layoutOrWidget)
{
    for (QObject *o = layoutOrWidget; o; o = o->parent()) {
        if (o->property("krimbleKeepRows").toBool()) return true;
    }
    return false;
}

static void krimbleNarrowDialogLayouts(QDialog *dialog)
{
    if (!dialog || !dialog->isWindow() || dialog->property("krimbleNarrowed").toBool() || dialog->inherits("KisOpenPane")) return;
    QScreen *screen = dialog->screen() ? dialog->screen() : QGuiApplication::primaryScreen();
    if (!screen) return;
    const int available = screen->availableGeometry().width();
    if (!dialog->layout() || krimbleDialogMinWidth(dialog) <= available) return;
    dialog->setProperty("krimbleNarrowed", true);
    const bool stackLabelsAndRows = (dialog->findChild<QScrollArea*>(QStringLiteral("krimbleDialogScroll")) == nullptr);
    Q_FOREACH (QFormLayout *form, dialog->findChildren<QFormLayout*>()) {
        if (!stackLabelsAndRows || krimbleKeepsRows(form)) continue;
        form->setRowWrapPolicy(QFormLayout::WrapAllRows);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    }
    Q_FOREACH (QGridLayout *grid, dialog->findChildren<QGridLayout*>()) {
        if (!stackLabelsAndRows || krimbleKeepsRows(grid)) continue;
        krimbleRestackGrid(grid);
    }
    Q_FOREACH (QWidget *w, dialog->findChildren<QWidget*>()) {
        if (w->minimumWidth() > int(available * 0.9)) w->setMinimumWidth(0);
    }
    Q_FOREACH (QSplitter *splitter, dialog->findChildren<QSplitter*>()) {
        if (splitter->orientation() == Qt::Horizontal) splitter->setOrientation(Qt::Vertical);
    }
    Q_FOREACH (QDialogButtonBox *box, dialog->findChildren<QDialogButtonBox*>()) {
        box->setProperty("krimbleKeepRows", true);
        if (box->sizeHint().width() <= available - 24) continue;
        QList<QAbstractButton*> extra;
        Q_FOREACH (QAbstractButton *b, box->buttons()) {
            const QDialogButtonBox::ButtonRole role = box->buttonRole(b);
            if (role != QDialogButtonBox::AcceptRole && role != QDialogButtonBox::RejectRole
                    && role != QDialogButtonBox::YesRole && role != QDialogButtonBox::NoRole
                    && role != QDialogButtonBox::ApplyRole) extra << b;
        }
        if (extra.isEmpty() || !box->parentWidget() || !box->parentWidget()->layout()) continue;
        QBoxLayout *holder = nullptr;
        int index = -1;
        Q_FOREACH (QBoxLayout *l, box->parentWidget()->findChildren<QBoxLayout*>()) {
            index = l->indexOf(box);
            if (index >= 0) { holder = l; break; }
        }
        if (!holder) {
            holder = qobject_cast<QBoxLayout*>(box->parentWidget()->layout());
            index = holder ? holder->indexOf(box) : -1;
        }
        if (!holder || index < 0) continue;
        QHBoxLayout *row = new QHBoxLayout();
        row->setProperty("krimbleKeepRows", true);
        Q_FOREACH (QAbstractButton *b, extra) {
            box->removeButton(b);
            b->setParent(box->parentWidget());
            row->addWidget(b);
            b->show();
        }
        row->addStretch(1);
        holder->insertLayout(index, row);
    }
    dialog->layout()->invalidate();
    if (krimbleDialogMinWidth(dialog) > available) {
        Q_FOREACH (QHBoxLayout *row, dialog->findChildren<QHBoxLayout*>()) {
            if (!stackLabelsAndRows || krimbleKeepsRows(row)) continue;
            if (row->count() >= 3) row->setDirection(QBoxLayout::TopToBottom);
        }
    }
    Q_FOREACH (QLayout *l, dialog->findChildren<QLayout*>()) {
        for (int i = 0; i < l->count(); ++i) {
            if (QSpacerItem *spacer = l->itemAt(i)->spacerItem()) {
                if (spacer->sizeHint().width() > 40) {
                    spacer->changeSize(0, spacer->sizeHint().height(), QSizePolicy::Minimum,
                                       (spacer->expandingDirections() & Qt::Vertical) ? QSizePolicy::Expanding : QSizePolicy::Minimum);
                }
            }
        }
        const QMargins m = l->contentsMargins();
        if (m.left() > 8 || m.right() > 8 || m.top() > 8 || m.bottom() > 8) {
            l->setContentsMargins(qMin(m.left(), 6), qMin(m.top(), 6), qMin(m.right(), 6), qMin(m.bottom(), 6));
        }
        if (QBoxLayout *box = qobject_cast<QBoxLayout*>(l)) {
            if (box->spacing() > 6) box->setSpacing(4);
        } else if (QGridLayout *g = qobject_cast<QGridLayout*>(l)) {
            if (g->horizontalSpacing() > 6) g->setHorizontalSpacing(4);
            if (g->verticalSpacing() > 6) g->setVerticalSpacing(4);
        }
    }
    dialog->layout()->invalidate();
    dialog->adjustSize();
}

static void krimbleFitTabHeight(QTabWidget *tabs)
{
    for (int i = 0; i < tabs->count(); ++i) {
        QWidget *page = tabs->widget(i);
        if (!page->property("krimbleVPolicy").isValid()) page->setProperty("krimbleVPolicy", int(page->sizePolicy().verticalPolicy()));
        QSizePolicy sp = page->sizePolicy();
        sp.setVerticalPolicy(i == tabs->currentIndex() ? QSizePolicy::Policy(page->property("krimbleVPolicy").toInt()) : QSizePolicy::Ignored);
        page->setSizePolicy(sp);
    }
    tabs->updateGeometry();
}

static void krimbleCompactDialog(QDialog *dialog)
{
    if (!dialog || !dialog->isWindow() || !dialog->layout() || dialog->property("krimbleCompacted").toBool()) return;
    static const char *const skip[] = { "KisDlgPreferences", "KisOpenPane", "QFileDialog", "QMessageBox", "QColorDialog", "QFontDialog", "QProgressDialog", "QInputDialog", "KoFileDialog" };
    for (const char *name : skip) {
        if (dialog->inherits(name)) return;
    }
    dialog->setProperty("krimbleCompacted", true);
    Q_FOREACH (QLayout *l, dialog->findChildren<QLayout*>()) {
        for (int i = 0; i < l->count(); ++i) {
            if (QSpacerItem *spacer = l->itemAt(i)->spacerItem()) {
                const QSize hint = spacer->sizeHint();
                if (hint.width() <= 24 && (hint.height() > 10 || (spacer->expandingDirections() & Qt::Vertical))) {   // a vertical spacer
                    spacer->changeSize(hint.width(), 6, QSizePolicy::Minimum, QSizePolicy::Minimum);
                }
            }
        }
        const QMargins m = l->contentsMargins();
        if (m.left() > 8 || m.right() > 8 || m.top() > 8 || m.bottom() > 8) {
            l->setContentsMargins(qMin(m.left(), 6), qMin(m.top(), 6), qMin(m.right(), 6), qMin(m.bottom(), 6));
        }
        if (QBoxLayout *box = qobject_cast<QBoxLayout*>(l)) {
            if (box->spacing() > 6) box->setSpacing(4);
        } else if (QGridLayout *g = qobject_cast<QGridLayout*>(l)) {
            if (g->horizontalSpacing() > 6) g->setHorizontalSpacing(4);
            if (g->verticalSpacing() > 6) g->setVerticalSpacing(4);
        }
    }
    Q_FOREACH (QTabWidget *tabs, dialog->findChildren<QTabWidget*>()) {
        krimbleFitTabHeight(tabs);
        QObject::connect(tabs, &QTabWidget::currentChanged, tabs, [tabs](int) {
            krimbleFitTabHeight(tabs);
            if (QWidget *window = tabs->window()) {
                QTimer::singleShot(0, window, [window]() {
                    if (!window->layout()) return;
                    window->layout()->invalidate();
                    QScreen *screen = window->screen() ? window->screen() : QGuiApplication::primaryScreen();
                    const int maxH = screen ? int(screen->availableGeometry().height() * 0.92) : 100000;
                    window->resize(window->width(), qMin(window->sizeHint().height(), maxH));
                });
            }
        });
    }
    dialog->layout()->invalidate();
    QScreen *screen = dialog->screen() ? dialog->screen() : QGuiApplication::primaryScreen();
    const QSize limit = screen ? QSize(int(screen->availableGeometry().width() * 0.95), int(screen->availableGeometry().height() * 0.92)) : QSize(100000, 100000);
    dialog->resize(dialog->sizeHint().boundedTo(limit).expandedTo(dialog->minimumSizeHint()));
}

class KrimbleNoSqueeze : public QObject
{
public:
    explicit KrimbleNoSqueeze(QObject *parent) : QObject(parent) {}
    static void fix(QWidget *w)
    {
        QLayout *layout = w ? w->layout() : nullptr;
        if (!layout || !w->isVisible()) return;
        // QLayout::totalMinimumSize() and totalHeightForWidth() already include the widget's own contents margins (measured in real Qt: a group box's 168 px), so none are added here
        int need = layout->totalMinimumSize().height();
        if (layout->hasHeightForWidth() && w->width() > 0) need = qMax(need, layout->totalHeightForWidth(w->width()));
        // only a widget that really is smaller than that is raised: a widget that has its room is left alone
        if (w->height() + 1 < need && need > w->minimumHeight() && need < 6000 && need <= w->maximumHeight()) w->setMinimumHeight(need);
    }
protected:
    bool eventFilter(QObject *o, QEvent *e) override
    {
        if (e->type() == QEvent::Show || e->type() == QEvent::Resize || e->type() == QEvent::LayoutRequest) {
            if (QWidget *w = qobject_cast<QWidget *>(o)) {
                if (!w->property("krimbleFixPending").toBool()) {
                    w->setProperty("krimbleFixPending", true);
                    QTimer::singleShot(0, w, [w]() { w->setProperty("krimbleFixPending", false); fix(w); });
                }
            }
        }
        return false;
    }
};

static void krimbleNoSqueeze(QDialog *dialog)
{
    if (!dialog || !dialog->inherits("KisDlgPreferences") || dialog->property("krimbleNoSqueeze").toBool()) return;
    dialog->setProperty("krimbleNoSqueeze", true);
    KrimbleNoSqueeze *filter = new KrimbleNoSqueeze(dialog);
    Q_FOREACH (QWidget *w, dialog->findChildren<QWidget *>()) {
        if (w->layout() && !qobject_cast<QAbstractScrollArea *>(w) && !qobject_cast<QComboBox *>(w) && !qobject_cast<QAbstractButton *>(w)) {
            w->installEventFilter(filter);
            QTimer::singleShot(0, w, [w]() { KrimbleNoSqueeze::fix(w); });
        }
    }
}

class KrimbleDialogKeeper : public QObject
{
public:
    explicit KrimbleDialogKeeper(QObject *parent) : QObject(parent) {}

    static void keepReachable(QWidget *dialog)
    {
        if (!dialog || !dialog->isWindow() || !dialog->isVisible()) {
            return;
        }
        QScreen *screen = dialog->screen();
        if (!screen) {
            screen = QGuiApplication::primaryScreen();
        }
        if (!screen) {
            return;
        }
        const QRect avail = screen->availableGeometry();
        const int reach = 160;
        QRect g = dialog->geometry();
        const int minLeft = avail.left() - (g.width() - reach);
        const int maxLeft = avail.right() - reach + 1;
        const int minTop = avail.top() - (g.height() - reach);
        const int maxTop = avail.bottom() - reach + 1;
        g.moveLeft(qBound(qMin(minLeft, maxLeft), g.left(), qMax(minLeft, maxLeft)));
        g.moveTop(qBound(qMin(minTop, maxTop), g.top(), qMax(minTop, maxTop)));
        if (g != dialog->geometry()) {
            dialog->move(g.topLeft());
        }
    }

    static void keepOnScreenSidesAndTop(QWidget *window)
    {
        if (!window || !window->isWindow() || !window->isVisible()) return;
        QScreen *screen = window->screen();
        if (!screen) screen = QGuiApplication::primaryScreen();
        if (!screen) return;
        QRect avail = screen->availableGeometry();
        const int titleBar = qMax(0, window->geometry().top() - window->frameGeometry().top());
        avail.setTop(avail.top() + qMax(48, titleBar));
        QRect g = window->geometry();
        const QRect original = g;
        if (g.width() > avail.width()) g.setWidth(avail.width());
        g.moveLeft(qBound(avail.left(), g.left(), avail.right() - g.width() + 1));
        if (g.top() < avail.top()) g.moveTop(avail.top());
        if (g.top() > avail.bottom() - 36) g.moveTop(avail.bottom() - 36);
        if (g != original) window->setGeometry(g);
    }

    static void keepOnScreen(QWidget *dialog)
    {
        if (!dialog || !dialog->isWindow() || !dialog->isVisible()) {
            return;
        }
        QScreen *screen = dialog->screen();
        if (!screen) {
            screen = QGuiApplication::primaryScreen();
        }
        if (!screen) {
            return;
        }
        QRect avail = screen->availableGeometry();
        // Keep a strip free at the top for a title bar that may be drawn outside the dialog's area.
        const int titleBar = qMax(0, dialog->geometry().top() - dialog->frameGeometry().top());
        const int topStrip = qMax(48, titleBar);
        avail.setTop(avail.top() + topStrip);

        QRect g = dialog->geometry();
        const QRect original = g;

        if (g.width() > avail.width()) {
            g.setWidth(avail.width());
        }
        if (g.height() > avail.height()) {
            g.setHeight(avail.height());
        }
        g.moveLeft(qBound(avail.left(), g.left(), avail.right() - g.width() + 1));
        g.moveTop(qBound(avail.top(), g.top(), avail.bottom() - g.height() + 1));

        if (g != original) {
            dialog->setGeometry(g);
        }
    }

    static bool isKeptWindow(QWidget *w)
    {
        if (!w || !w->isWindow()) return false;
        if (qobject_cast<QDialog*>(w)) return true;
        if (QDockWidget *dock = qobject_cast<QDockWidget*>(w)) return dock->isFloating();
        return false;
    }

    static QString krimbleDialogKey(QDialog *dialog)
    {
        QString name = QString::fromLatin1(dialog->metaObject()->className()) + QLatin1Char('|');
        name += dialog->objectName().isEmpty() ? dialog->windowTitle() : dialog->objectName();
        if (QInputDialog *input = qobject_cast<QInputDialog*>(dialog)) { name += QLatin1Char('|'); name += input->labelText(); }
        QScreen *screen = QGuiApplication::primaryScreen();
        const bool portrait = screen && screen->availableGeometry().height() > screen->availableGeometry().width();
        return QStringLiteral("krimble/DialogGeometry/%1_%2").arg(qHash(name)).arg(portrait ? QStringLiteral("P") : QStringLiteral("L"));
    }
    static void krimbleRememberDialog(QDialog *dialog, QEvent *event)
    {
        if (!dialog || !dialog->isWindow()) return;
        if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
            if (dialog->isVisible() && QApplication::mouseButtons() != Qt::NoButton) dialog->setProperty("krimbleUserSized", true);
        } else if (event->type() == QEvent::Hide) {
            if (dialog->property("krimbleUserSized").toBool()) {
                QSettings().setValue(krimbleDialogKey(dialog), dialog->geometry());
                dialog->setProperty("krimbleUserSized", false);
            }
        } else if (event->type() == QEvent::Show) {
            dialog->setProperty("krimbleUserSized", false);
            QPointer<QDialog> guard(dialog);
            QTimer::singleShot(250, dialog, [guard]() {
                if (!guard || !guard->isVisible()) return;
                QRect saved = QSettings().value(krimbleDialogKey(guard)).toRect();
                QScreen *screen = QGuiApplication::primaryScreen();
                if (!saved.isValid() || !screen) return;
                const QRect avail = screen->availableGeometry();
                saved.setSize(saved.size().boundedTo(avail.size()).expandedTo(guard->minimumSize().boundedTo(avail.size())));
                saved.moveLeft(qBound(avail.left(), saved.left(), avail.right() - saved.width() + 1));
                saved.moveTop(qBound(avail.top(), saved.top(), avail.bottom() - saved.height() + 1));
                if (guard->geometry() != saved) guard->setGeometry(saved);
            });
        }
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Show || event->type() == QEvent::Hide || event->type() == QEvent::Resize || event->type() == QEvent::Move) {
            if (QDialog *rememberDialog = qobject_cast<QDialog*>(watched)) krimbleRememberDialog(rememberDialog, event);
        }
        switch (event->type()) {
        case QEvent::Show:
        case QEvent::Resize: {
            if (QWidget *dialog = qobject_cast<QWidget*>(watched)) {
                if (QDialog *wrapDialog = qobject_cast<QDialog*>(dialog)) {
                    if (wrapDialog->isWindow()) {
                        bool changed = false;
                        Q_FOREACH (QLabel *label, wrapDialog->findChildren<QLabel*>()) {
                            if (!label->wordWrap() && label->text().length() > 40) {
                                label->setWordWrap(true);
                                changed = true;
                            }
                        }
                        if (changed && wrapDialog->layout()) {
                            wrapDialog->layout()->invalidate();
                            wrapDialog->adjustSize();
                        }
                    }
                    krimbleNarrowDialogLayouts(wrapDialog);
                    if (event->type() == QEvent::Show) krimbleCompactDialog(wrapDialog);
                    if (event->type() == QEvent::Show) krimbleNoSqueeze(wrapDialog);
                }
                if (QDialog *pageDialog = qobject_cast<QDialog*>(dialog)) {
                    if (pageDialog->isWindow()) {
                        Q_FOREACH (QAbstractItemView *pageList, pageDialog->findChildren<QAbstractItemView*>()) {
                            const QString krimblePageListClass = QString::fromLatin1(pageList->metaObject()->className());
                            if (event->type() == QEvent::Show) qWarning() << "KRIMBLE-PREFS item view in dialog" << pageDialog->objectName() << "class" << krimblePageListClass;
                            if (krimblePageListClass.endsWith(QLatin1String("KPageListView")) && !pageList->property("krimblePageListDelegate").toBool()) {
                                pageList->setProperty("krimblePageListDelegate", true);
                                pageList->setItemDelegate(new KrimblePageListDelegate(pageList->itemDelegate(), pageList));
                                pageList->viewport()->update();
                                qWarning() << "KRIMBLE-PREFS highlight-under-icon delegate installed on" << krimblePageListClass;
                            }
                            if (QLatin1String(pageList->metaObject()->className()) == QLatin1String("KPageListView")) {
                                if (QScreen *pageScreen = QGuiApplication::primaryScreen()) {
                                    const qreal keyboardDpr = qMax<qreal>(1.0, pageScreen->devicePixelRatio());
                                    const int keyboardColumn = qRound(486 / keyboardDpr);   // in the style sheets' and layouts' own units
                                    const int cap = qMax(int(pageScreen->availableGeometry().width() * 0.24), keyboardColumn);
                                    if (pageList->maximumWidth() > cap) pageList->setMaximumWidth(cap);
                                    pageList->setMinimumWidth(qMax(pageList->minimumWidth(), keyboardColumn));
                                    pageList->setIconSize(QSize(40, 40));
                                    {
                                        KrimblePageListStyle *pageListStyle = new KrimblePageListStyle();
                                        pageListStyle->setParent(pageList);
                                        pageList->setStyle(pageListStyle);
                                        pageList->setItemDelegate(new KrimblePageListDelegate(pageList->itemDelegate(), pageList));
                                    }
                                }
                            }
                        }
                    }
                }
                if (QDialog *gripDialog = qobject_cast<QDialog*>(dialog)) {
                    if (gripDialog->isWindow() && !qobject_cast<KoDialog*>(gripDialog)
                            && !gripDialog->property("krimbleNoGrip").toBool()
                            && !qobject_cast<QMessageBox*>(gripDialog) && !qobject_cast<QProgressDialog*>(gripDialog)) {
                        QWidget *grip = gripDialog->findChild<QWidget*>(QStringLiteral("krimbleResizeGrip"), Qt::FindDirectChildrenOnly);
                        if (!grip) {
                            grip = new KrimbleResizeGrip(gripDialog);
                            grip->setObjectName(QStringLiteral("krimbleResizeGrip"));
                        }
                        const bool krimbleSmallGrip = gripDialog->property("krimbleSmallGrip").toBool();
                        if (krimbleSmallGrip && grip->width() != 16) {
                            grip->setFixedSize(16, 16);
                            grip->setProperty("krimbleInvisible", true);
                        }
                        if (gripDialog->layout() && !krimbleSmallGrip && !gripDialog->property("krimbleGripStrip").toBool()) {
                            QMargins m = gripDialog->layout()->contentsMargins();
                            m.setBottom(m.bottom() + grip->height());
                            gripDialog->layout()->setContentsMargins(m);
                            gripDialog->setProperty("krimbleGripStrip", true);
                        }
                        grip->move(gripDialog->width() - grip->width(), gripDialog->height() - grip->height());
                        grip->show();
                        grip->raise();
                    }
                }
                if (QMdiSubWindow *gripSub = qobject_cast<QMdiSubWindow*>(dialog)) {
                    QWidget *grip = gripSub->findChild<QWidget*>(QStringLiteral("krimbleResizeGrip"), Qt::FindDirectChildrenOnly);
                    if (!grip) {
                        grip = new KrimbleResizeGrip(gripSub);
                        grip->setObjectName(QStringLiteral("krimbleResizeGrip"));
                    }
                    grip->move(gripSub->width() - grip->width(), gripSub->height() - grip->height());
                    grip->setVisible(!gripSub->isMaximized() && !gripSub->isMinimized());
                    grip->raise();
                }
                if (isKeptWindow(dialog)) {
                    // Watch the dialog's window for touches: a window sees ALL fingers, while a
                    // widget only sees them if it accepts the first touch (which would stop one-finger
                    // taps from reaching buttons).
                    if (QWindow *handle = dialog->windowHandle()) {
                        if (!handle->property("krimbleKeeper").toBool()) {
                            handle->installEventFilter(this);
                            handle->setProperty("krimbleKeeper", true);
                            handle->setProperty("krimbleDialog", QVariant::fromValue(static_cast<QObject*>(dialog)));
                        }
                    }
                    if (event->type() == QEvent::Show) {
                        QPointer<QWidget> guard(dialog);
                        QTimer::singleShot(0, dialog, [guard]() {
                            keepOnScreen(guard);
                        });
                    }
                }
            }
            break;
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        case QEvent::TouchCancel:
            if (QWindow *window = qobject_cast<QWindow*>(watched)) {
                return handleTouch(window, static_cast<QTouchEvent*>(event));
            }
            break;
        default:
            break;
        }
        return false;
    }

private:
    bool handleTouch(QWindow *window, QTouchEvent *touch)
    {
        // QDialog *dialog = qobject_cast<QDialog*>(qvariant_cast<QObject*>(window->property("krimbleDialog")));
        QWidget *dialog = qobject_cast<QWidget*>(qvariant_cast<QObject*>(window->property("krimbleDialog")));
        if (!dialog || !dialog->isWindow()) {
            return false;
        }
        const QList<QTouchEvent::TouchPoint> points = touch->touchPoints();
        const bool twoFingers = points.count() >= 2 && touch->type() != QEvent::TouchEnd
                                && touch->type() != QEvent::TouchCancel;
        if (twoFingers) {
            const QPointF centre = (points.at(0).screenPos() + points.at(1).screenPos()) * 0.5;
            if (m_target != dialog) {
                m_target = dialog;
                m_last = centre;
            } else {
                const QPointF delta = centre - m_last;
                m_last = centre;
                dialog->move(dialog->pos() + delta.toPoint());
            }
            touch->accept();
            return true;     // the dialog's contents do not react to the two-finger move
        }
        if (m_target) {
            QPointer<QWidget> moved(m_target);
            m_target.clear();
            // QTimer::singleShot(0, dialog, [moved]() { keepOnScreen(moved); });   (bounced a dialog back on screen: George, no)
            QTimer::singleShot(0, dialog, [moved]() {
                QDockWidget *movedDock = qobject_cast<QDockWidget*>(moved.data());
                if (movedDock && movedDock->objectName() == QLatin1String("ToolBox")) {
                    QMetaObject::invokeMethod(movedDock, "krimbleDockAtEdge");
                }
            });
            if (points.count() >= 2) {
                touch->accept();
                return true;
            }
        }
        return false;
    }

    QPointer<QWidget> m_target;
    QPointF m_last;
};
#endif

