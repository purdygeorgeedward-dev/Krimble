// Krimble 2026-11-20 (760th): part of KisApplication.cpp, moved here unchanged and included at the same place (George: "Fix all of that before any build can take place.").
// It only compiles as part of KisApplication.cpp.

// Krimble 2026-10-11: a visible corner grip (three diagonal lines, 44 px square) at the bottom right of every dialog that has no
// touch resize of its own: drag it to resize the dialog. KoDialog dialogs already have their own edges and are left alone.
class KrimbleResizeGrip : public QWidget
{
public:
    explicit KrimbleResizeGrip(QWidget *dialog) : QWidget(dialog)
    {
        setFixedSize(26, 26);   // Krimble 2026-10-12: smaller, and the dialog gets a strip for it below the buttons (it covered the Create button)
        setCursor(Qt::SizeFDiagCursor);
        setAttribute(Qt::WA_NoSystemBackground);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (property("krimbleInvisible").toBool()) return;   // Krimble 2026-11-17 (731st): the small grip of QuickAdjust is not drawn
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
        m_startSize = window()->size();
        e->accept();
    }
    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton)) return;
        const QPoint d = e->globalPos() - m_start;
        QWidget *w = window();
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
    QPoint m_start;
    QSize m_startSize;
};

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-03/04: keeps dialogs where they can be reached, and lets any dialog be moved with
// two fingers.
//  - Dialogs sometimes opened partly above the top of the screen, so their title bar and edges could
//    not be grabbed. Every top-level dialog is brought back inside the usable screen area right after
//    it is shown and again after it is resized: no larger than the screen, and its top-left corner on
//    screen. A strip at the top (at least 48 px, or the height of the window's own title bar if that
//    is larger) is kept free, so a title bar drawn above the dialog's area is not cut off either.
//  - Two-finger drag: put two fingers anywhere on a dialog and drag, and the whole dialog moves with
//    the middle point between the fingers (the idea of the unmerged branch move/two-finger-window-mover,
//    rebuilt here). One finger behaves as before. Lifting the fingers brings the dialog back on screen
//    if it was dragged too far.
// Krimble 2026-10-13: George: "All of them. Check them all for things like that. Don't let them get big." (the export option windows;
// the PNG one was about two screens wide with a column of labels beside nothing). A window that is wider than the screen even at
// its smallest is narrowed here, whichever window it is:
//  - a form layout puts each label above its field (WrapAllRows);
//  - a grid whose rows are "label | control" puts the label on a line of its own above the control;
//  - long labels wrap (done by the caller, before this).
// A window that already fits is not touched.
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

// Krimble 2026-10-23: how wide a window's contents want to be at least. For a KoDialog window the main widget sits in a scroll area (KoDialog.cpp), so the window's own
// minimum width is small whatever the contents need; the contents' minimum width is what decides whether the narrowing below is wanted.
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

// Krimble 2026-10-25: George (the New window): "The new dialog is too tall." The narrowing below puts every label above its field, and stacks every row of three controls (the Cancel and Create buttons too:
// the layout inside a QDialogButtonBox is a row with a spacer and two buttons). A part of a window can be marked, by the property "krimbleKeepRows" on a widget (or on a layout): the narrowing leaves the
// form rows, the grid rows and the button rows of that part, and of everything inside it, as they were designed (label beside its field, buttons side by side).
static bool krimbleKeepsRows(QObject *layoutOrWidget)
{
    for (QObject *o = layoutOrWidget; o; o = o->parent()) {
        if (o->property("krimbleKeepRows").toBool()) return true;
    }
    return false;
}

static void krimbleNarrowDialogLayouts(QDialog *dialog)
{
    if (!dialog || !dialog->isWindow() || dialog->property("krimbleNarrowed").toBool()) return;
    QScreen *screen = dialog->screen() ? dialog->screen() : QGuiApplication::primaryScreen();
    if (!screen) return;
    const int available = screen->availableGeometry().width();
    if (!dialog->layout() || krimbleDialogMinWidth(dialog) <= available) return;   // Krimble 2026-10-23: the contents' width for windows with a scroll area
    dialog->setProperty("krimbleNarrowed", true);
    // Krimble 2026-10-25: George: "How do we change global rules? That's a dumb rule." (labels above their fields, and the Cancel and Create buttons one above the other). Since 2026-10-23 the main
    // part of a KoDialog window is in a scroll area ("krimbleDialogScroll"), so a window that is wider than the screen can be scrolled and nothing is out of reach: the two rules that move things
    // around (labels above their fields, and every row of three or more controls turned into a column) are now used only for a window that has NO scroll area, because its right side would be off the screen.
    // Everything else below (the fixed widths, the splitters, the margins and spacing) is as before. To switch the two rules off for every window, make stackLabelsAndRows false.
    const bool stackLabelsAndRows = (dialog->findChild<QScrollArea*>(QStringLiteral("krimbleDialogScroll")) == nullptr);
    Q_FOREACH (QFormLayout *form, dialog->findChildren<QFormLayout*>()) {
        if (!stackLabelsAndRows || krimbleKeepsRows(form)) continue;   // Krimble 2026-10-25: a marked part, or a window that scrolls, keeps its label beside its field
        form->setRowWrapPolicy(QFormLayout::WrapAllRows);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    }
    Q_FOREACH (QGridLayout *grid, dialog->findChildren<QGridLayout*>()) {
        if (!stackLabelsAndRows || krimbleKeepsRows(grid)) continue;
        krimbleRestackGrid(grid);
    }
    // Krimble 2026-10-18: George (1.0.37, with the JPG Options window): "All those super wide dialogs for no reason have to go. Analyze the rest of the dialog windows for
    // negative space and width." A scan of the 283 form files found 46 likely offenders: fixed minimum widths written into the form (425, 480, 720, 730 px ...), side-by-side
    // panes (horizontal splitters: the resource and bundle managers), rows of four or more controls (Image Size, Canvas Size with 6 grid columns, Layer Style options,
    // Transform options), and tabs whose widest page sets the window's width. The function above handled only simple label and field rows; it now also does these:
    // (1) a fixed minimum width wider than 90% of the screen is dropped; (2) a horizontal splitter becomes vertical; (3) if the window is still too wide, every row of
    // three or more controls is stacked; (4) in a window that was too wide the margins and spacing of every layout are tightened (negative space).
    Q_FOREACH (QWidget *w, dialog->findChildren<QWidget*>()) {
        if (w->minimumWidth() > int(available * 0.9)) w->setMinimumWidth(0);
    }
    Q_FOREACH (QSplitter *splitter, dialog->findChildren<QSplitter*>()) {
        if (splitter->orientation() == Qt::Horizontal) splitter->setOrientation(Qt::Vertical);
    }
    dialog->layout()->invalidate();
    if (krimbleDialogMinWidth(dialog) > available) {   // Krimble 2026-10-23
        Q_FOREACH (QHBoxLayout *row, dialog->findChildren<QHBoxLayout*>()) {
            if (!stackLabelsAndRows || krimbleKeepsRows(row)) continue;   // Krimble 2026-10-25: the Cancel and Create buttons (and every other row) stay side by side in a window that scrolls
            if (row->count() >= 3) row->setDirection(QBoxLayout::TopToBottom);
        }
    }
    Q_FOREACH (QLayout *l, dialog->findChildren<QLayout*>()) {
        // Krimble 2026-10-18 (9th): a spacer in a form file has a design size, and one with a width (a vertical spacer of 414 x 16 in the PNG options form) makes the whole window at least
        // that wide. In a window that is too wide the width of every spacer is dropped.
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

// Krimble 2026-10-23: George (1.0.41 screenshots of Image Size, the JPEG options, File Info ...): "There's just a lot of empty negative space in these windows." Three causes,
// each handled here for the plain windows of the program (not Preferences, the New window or the system dialogs, which have their own handling):
// (1) a vertical spacer in a form is a gap of its design size (or an expanding one that takes whatever height the window has): it becomes a small gap;
// (2) margins and spacing of the default size: capped (6 px margins, 4 px spacing), as the narrowing above does for windows that are too wide;
// (3) a tab widget is as tall as its tallest page, so a short page (JPEG Basic) floats in blank: the pages that are not showing do not count for the height.
// The window then opens at the size its contents ask for (but no larger than 95% x 92% of the screen), not at a taller size left over from before.
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

// Krimble 2026-11-08: George (screenshots of Preferences: File Handling, Window and Cursor pages): "Prefs dialog is a mess." The rows inside the group boxes were pressed together
// (the text of one row cut by the next: Show outline / Use effective outline size, the compression options, Multiple Document Mode / Subwindows): a group box was given less height than its
// own layout needs. This could not be reproduced in real Qt on a computer (tabbed pages with wrapped form rows in a narrow scroll area, with this style sheet, with and without a short
// window), so the rule does not depend on the cause: in Preferences, every widget that has a layout (the group boxes, the tab pages and what is inside them) is never smaller than its layout
// needs (its minimum height, or the height for its width if the layout wraps), re-checked when the widget is shown, resized or laid out again. The page then scrolls instead of pressing
// its rows together. Only Preferences (the other windows have their own handling).
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
            // if (QWidget *w = qobject_cast<QWidget *>(o)) QTimer::singleShot(0, w, [w]() { fix(w); });
            // Krimble 2026-11-20 (760th): George: "Fix all of that before any build can take place." (the app-wide filters) A widget being resized or laid out many times in a row
            // (a panel drag, the canvas) queued one fix per event; now at most one is waiting per widget at a time. Same fix, same widgets.
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

    // Krimble 2026-10-11: dialogs are only kept REACHABLE after a two-finger drag: a dialog taller than the screen can be
    // dragged up to reach its bottom buttons and stays there (no bounce back). At least 160 px of it stay on screen.
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

    // Krimble 2026-10-12: George: "I think the toolbox shouldn't bounce off the bottom. Only the top or sides." Like keepOnScreen, but the
    // bottom edge is free: only the left, right and top edges push the window back in, and at least 120 px of it must stay on screen.
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
        if (g.top() > avail.bottom() - 36) g.moveTop(avail.bottom() - 36);   // Krimble 2026-10-14: the title area (36 px) stays on screen
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

    // Krimble 2026-10-11: floating panels (the toolbox too) get the same two-finger move and bounce back inside the screen.
    static bool isKeptWindow(QWidget *w)
    {
        if (!w || !w->isWindow()) return false;
        if (qobject_cast<QDialog*>(w)) return true;
        if (QDockWidget *dock = qobject_cast<QDockWidget*>(w)) return dock->isFloating();
        return false;
    }

    // Krimble 2026-11-16 (728th): George (build 65): "Resizing Save As... window does not set the size for next time it is invoked. When the dimensions of a window are resized by user,
    // they should be that size and position when invoked again." Every dialog window: when the user has resized or moved it (its size or place changed while a finger was down), its
    // size and place are saved when it closes, and put back a moment after it opens the next time (after the fitting code above has run). Saved per window (its class, its name or title,
    // for an input window also its label) and per orientation of the screen, under krimble/DialogGeometry in the settings. A saved place that no longer fits the screen is moved inside.
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
            if (QDialog *rememberDialog = qobject_cast<QDialog*>(watched)) krimbleRememberDialog(rememberDialog, event);   // Krimble 2026-11-16 (728th)
        }
        switch (event->type()) {
        case QEvent::Show:
        case QEvent::Resize: {
            if (QWidget *dialog = qobject_cast<QWidget*>(watched)) {
                // Krimble 2026-10-12: a long label that does not wrap makes the whole dialog as wide as the label (the JPEG export
                // dialog was about three screens wide because of two long sentences). Long labels in dialogs now wrap.
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
                    krimbleNarrowDialogLayouts(wrapDialog);   // Krimble 2026-10-13: a window wider than the screen is narrowed
                    if (event->type() == QEvent::Show) krimbleCompactDialog(wrapDialog);   // Krimble 2026-10-23: no empty negative space (see krimbleCompactDialog)
                    if (event->type() == QEvent::Show) krimbleNoSqueeze(wrapDialog);       // Krimble 2026-11-08: Preferences rows are never pressed together (see krimbleNoSqueeze)
                }
                // Krimble 2026-10-12: George: "That column of colored icons in the dialog is still too wide." The page list on the
                // left of a page dialog (Preferences) is at most 24% of the screen's width.
                if (QDialog *pageDialog = qobject_cast<QDialog*>(dialog)) {
                    if (pageDialog->isWindow()) {
                        Q_FOREACH (QAbstractItemView *pageList, pageDialog->findChildren<QAbstractItemView*>()) {
                            // Krimble 2026-11-15 (706th): George (build 63, Preferences screenshots): "Preferences selection still tints icons." Measured on his screenshots: the selected keycap is the plain
                            // one blended with about 30% of the highlight color (Qt's QIcon::Selected picture, drawn by the list's own delegate), and the pictures are about 85 units, not the 44 the block
                            // below sets. So the block below has never run: its test asks for the class name "KPageListView" exactly, and the list's class is in a namespace (KDEPrivate::KPageListView in
                            // KWidgetsAddons). The delegate that draws the highlight UNDER the picture is installed here, once, for any class whose name ends in KPageListView. Only the delegate: the sizes
                            // (44 unit pictures, the column cap) of the block below would change a list George has been looking at for a month, so they stay off.
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
                                    // Krimble 2026-11-15 (item 11): George: "Just render it full size." The keyboard picture is 480 real pixels wide; the column is wide enough for it (486 real pixels with
                                    // its 3 px at each side), or 24% of the screen, whichever is wider.
                                    const qreal keyboardDpr = qMax<qreal>(1.0, pageScreen->devicePixelRatio());
                                    const int keyboardColumn = qRound(486 / keyboardDpr);   // in the style sheets' and layouts' own units
                                    const int cap = qMax(int(pageScreen->availableGeometry().width() * 0.24), keyboardColumn);
                                    if (pageList->maximumWidth() > cap) pageList->setMaximumWidth(cap);
                                    pageList->setMinimumWidth(qMax(pageList->minimumWidth(), keyboardColumn));
                                    // Krimble 2026-10-13 (3rd): George (1.0.35): "Preferences icons are way too small." The pictures were drawn at
                                    // the program's small default (32 logical px); they are now 64 logical px (about 216 on the screen), which
                                    // is the most that fits in the narrowed list.
                                    pageList->setIconSize(QSize(44, 44));
                                    {
                                        KrimblePageListStyle *pageListStyle = new KrimblePageListStyle();
                                        pageListStyle->setParent(pageList);
                                        pageList->setStyle(pageListStyle);   // Krimble 2026-10-14: the real way to make the pictures big (see above)
                                        pageList->setItemDelegate(new KrimblePageListDelegate(pageList->itemDelegate(), pageList));   // Krimble 2026-11-15: the highlight under the icon, not over it
                                    }
                                }
                            }
                        }
                    }
                }
                // Krimble 2026-10-11: the corner grip on dialogs that have no touch resize of their own
                if (QDialog *gripDialog = qobject_cast<QDialog*>(dialog)) {
                    if (gripDialog->isWindow() && !qobject_cast<KoDialog*>(gripDialog)
                            && !gripDialog->property("krimbleNoGrip").toBool()   // Krimble 2026-11-16 (728th): a window that asks for no corner grip (QuickAdjust: it lay on top of OK)
                            && !qobject_cast<QMessageBox*>(gripDialog) && !qobject_cast<QProgressDialog*>(gripDialog)) {
                        QWidget *grip = gripDialog->findChild<QWidget*>(QStringLiteral("krimbleResizeGrip"), Qt::FindDirectChildrenOnly);
                        if (!grip) {
                            grip = new KrimbleResizeGrip(gripDialog);
                            grip->setObjectName(QStringLiteral("krimbleResizeGrip"));
                        }
                        // Krimble 2026-11-17 (731st): a window that asks for a small grip (QuickAdjust) gets one of 16 units, drawn as nothing, and no extra strip under its buttons
                        const bool krimbleSmallGrip = gripDialog->property("krimbleSmallGrip").toBool();
                        if (krimbleSmallGrip && grip->width() != 16) {
                            grip->setFixedSize(16, 16);
                            grip->setProperty("krimbleInvisible", true);
                        }
                        // Krimble 2026-10-12: George: "Sizing gadget covers the button." A strip as tall as the grip is added under the
                        // dialog's contents (once), so the grip sits below the buttons instead of on top of them.
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
                    // After the current event, once the dialog has its final size and place.
                    // Krimble 2026-10-20: George (1.0.40, test list, item 16: the corner resize of Preferences "bounces. No resize."): "The bounce
                    // behavior in the windows is annoying. It's only useful if a window is about to become stranded offscreen, and that doesn't
                    // happen any more thanks to our two finger move." The pull back inside the screen (keepOnScreen: no bigger than the screen,
                    // pushed back from the edges) now runs only when a window is SHOWN, so it opens fully on screen; a resize (the corner grip,
                    // the window's own edges) is no longer pushed back. The two-finger move still keeps 160 px on screen (keepReachable).
                    // QPointer<QWidget> guard(dialog);
                    // QTimer::singleShot(0, dialog, [guard]() {
                    //     keepOnScreen(guard);
                    // });
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
                // Krimble 2026-10-12: George: "if a window has two finger it doesn't need bounce back. We can dump bounce back except on
                // the toolbox." Only the toolbox bounces back inside the screen; every other window stays where it was dragged
                // (as long as part of it is reachable).
                // Krimble 2026-10-13 (7th): George: "Get rid of bounces. They're annoying." After a two-finger drag nothing is moved any
                // more: every window and panel, the toolbox included, stays exactly where the fingers let go. The old rules are kept:
                //     keepOnScreenSidesAndTop(moved);   // the toolbox was pushed back off the top and the sides
                //     keepReachable(moved);             // other windows: at least 160 px had to stay on screen
                // }
                // Krimble 2026-10-14: George (1.0.36): "Bounce toolbox off sides and top. Not bottom." Only the toolbox, after a two-finger drag;
                // every other window and panel still stays where it is let go.
                QDockWidget *movedDock = qobject_cast<QDockWidget*>(moved.data());
                if (movedDock && movedDock->objectName() == QLatin1String("ToolBox")) {
                    // Krimble 2026-10-17: George (1.0.37): "I would rather the toolbar dock at the edge than bounce." At an edge the toolbox docks.
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

