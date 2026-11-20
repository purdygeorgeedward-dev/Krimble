// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

// ============================================================================================================================
// Krimble 2026-10-12: Quick Adjust (Claude's idea; George: "Build quick adjust. It's such a useful feature. Be careful of the width.")
// A small panel with six sliders that adjust the picture WITHOUT changing its pixels: each group of sliders is an adjustment layer
// (created the first time a slider in that group moves away from 0, and edited as the slider moves), so the user can delete or
// hide the layer, or change it later in the Layers panel. Brightness/Contrast, Saturation/Hue/Lightness and Vibrance.
// Narrow on purpose: one slider per row, no side-by-side controls.
// ============================================================================================================================
// Krimble 2026-10-30: George: "new QuickAdjust dialog was supposed to already be built by ChatGPT. Either complete it or replace it." The drawing
// (ChatGPT's mockup) has a small picture at the start of each slider row, a rainbow Hue slider and a Preview switch that says ON or OFF. These are the
// missing parts, drawn in code (no picture files). The mockup's "Krimble" logo in the heading is left out: George, "QuickAdjust does not need to say
// Krimble in it."
static QPixmap krimbleQuickAdjustIcon(int which, int px)
{
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF r(3, 3, px - 6, px - 6);
    const QPointF c = r.center();
    const QColor ink(0xf4, 0xf6, 0xff);
    p.setPen(QPen(ink, 2));
    p.setBrush(Qt::NoBrush);
    switch (which) {
    case 0: {   // Brightness: a sun
        const qreal core = r.width() * 0.22;
        p.drawEllipse(c, core, core);
        for (int k = 0; k < 8; ++k) {
            const qreal a = k * M_PI / 4.0;
            const QPointF dir(std::cos(a), std::sin(a));
            p.drawLine(c + dir * (core + 3), c + dir * (r.width() * 0.46));
        }
        break; }
    case 1:     // Contrast: a circle, the left half filled
        p.drawEllipse(r);
        p.setBrush(ink);
        p.drawPie(r, 90 * 16, 180 * 16);
        break;
    case 2: {   // Saturation: three overlapping drops of color
        const qreal d = r.width() * 0.58;
        p.setPen(Qt::NoPen);
        const QColor cols[3] = { QColor(40, 190, 255, 215), QColor(255, 90, 170, 215), QColor(255, 200, 40, 215) };
        const QPointF pts[3] = { QPointF(r.left() + d / 2, r.top() + d / 2), QPointF(r.right() - d / 2, r.top() + d / 2), QPointF(c.x(), r.bottom() - d / 2) };
        for (int k = 0; k < 3; ++k) { p.setBrush(cols[k]); p.drawEllipse(pts[k], d / 2, d / 2); }
        break; }
    case 3: {   // Hue: a ring of all the colors
        QConicalGradient g(c, 0);
        for (int k = 0; k <= 6; ++k) g.setColorAt(k / 6.0, QColor::fromHsv((k * 60) % 360, 255, 255));
        const qreal w = r.width() * 0.22;
        p.setPen(QPen(QBrush(g), w));
        p.drawEllipse(r.adjusted(w / 2, w / 2, -w / 2, -w / 2));
        break; }
    case 4: {   // Lightness: a square, the left half light
        const QRectF sq = r.adjusted(2, 2, -2, -2);
        p.drawRoundedRect(sq, 3, 3);
        p.setBrush(ink);
        p.drawRect(QRectF(sq.left(), sq.top(), sq.width() / 2, sq.height()));
        break; }
    case 5: {   // Vibrance: a four-point sparkle
        const qreal R = r.width() * 0.5, q = r.width() * 0.14;
        QPainterPath path;
        path.moveTo(c.x(), c.y() - R); path.lineTo(c.x() + q, c.y() - q); path.lineTo(c.x() + R, c.y());
        path.lineTo(c.x() + q, c.y() + q); path.lineTo(c.x(), c.y() + R); path.lineTo(c.x() - q, c.y() + q);
        path.lineTo(c.x() - R, c.y()); path.lineTo(c.x() - q, c.y() - q); path.closeSubpath();
        p.setPen(QPen(ink, 1.5));
        p.setBrush(QColor(0x9d, 0x8c, 0xff));
        p.drawPath(path);
        break; }
    }
    return pm;
}

// the Preview switch of the drawing: a pill that says ON or OFF, with a round knob, and a blue glow when it is on
class KrimbleToggleSwitch : public QAbstractButton
{
public:
    explicit KrimbleToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent)
    {
        setCheckable(true);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    // Krimble 2026-11-11: George: "The rounded preview toggle is too big in QuickAdjust." 64 x 30 (was 84 x 40), with a thinner margin around the switch and a smaller knob.
    // Krimble 2026-11-15: George: "the preview toggle button is too big to fit in between the buttons now. Shrink it." 44 x 20 (was 64 x 30).
    QSize sizeHint() const override { return QSize(44, 20); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(rect()).adjusted(3, 3, -3, -3);   // Krimble 2026-11-11: smaller switch
        const qreal radius = r.height() / 2.0;
        if (isChecked()) {
            p.setBrush(Qt::NoBrush);
            for (int i = 4; i >= 1; --i) {
                p.setPen(QPen(QColor(56, 198, 255, 20 * (5 - i)), i * 2.0));
                p.drawRoundedRect(r, radius, radius);
            }
        }
        QLinearGradient bg(r.topLeft(), r.bottomLeft());
        // Krimble 2026-11-06 (3rd): the mockup's ON switch (sampled): #0050B4 at the top, #0069D8, #026AE5 at the bottom, a #54E5FF outline, a #EAEDF6 knob
        if (isChecked()) { bg.setColorAt(0, QColor(0x00, 0x50, 0xb4)); bg.setColorAt(0.35, QColor(0x00, 0x69, 0xd8)); bg.setColorAt(1, QColor(0x02, 0x6a, 0xe5)); }
        else             { bg.setColorAt(0, QColor(0x4a, 0x52, 0x76)); bg.setColorAt(1, QColor(0x2a, 0x30, 0x48)); }
        p.setPen(QPen(isChecked() ? QColor(0x54, 0xe5, 0xff) : QColor(0x6b, 0x74, 0x98), 1.5));
        p.setBrush(bg);
        p.drawRoundedRect(r, radius, radius);
        const qreal knob = r.height() - 6;   // Krimble 2026-11-11: smaller switch
        const qreal x = isChecked() ? r.right() - knob - 3 : r.left() + 3;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0xea, 0xed, 0xf6));
        p.drawEllipse(QRectF(x, r.top() + 3, knob, knob));   // Krimble 2026-11-11
        p.setPen(QColor(0xf4, 0xf6, 0xff));
        QFont f = font(); f.setBold(true); f.setPointSizeF(qMax<qreal>(7.0, f.pointSizeF() * 0.8)); p.setFont(f);   // Krimble 2026-11-11: a smaller ON/OFF
        const QRectF textRect = isChecked() ? QRectF(r.left() + 6, r.top(), r.width() - knob - 14, r.height())
                                            : QRectF(r.left() + knob + 10, r.top(), r.width() - knob - 14, r.height());
        p.drawText(textRect, Qt::AlignCenter, isChecked() ? QStringLiteral("ON") : QStringLiteral("OFF"));
    }
};

#if 0   // Krimble 2026-10-30: switched off: George: "not drawn in code. I will provide a graphic." The code is kept, not run.
// Krimble 2026-10-30: George: "QuickAdjust should have its own logo, a big bold stylize QuickAdjust." (not the Krimble K and paw: "no.") A logo made of the
// word itself, drawn in code: heavy italic letters, "Quick" light with a blue glow and "Adjust" in Krimble orange (#FD6001) with a dark edge, and a thin
// slider line under them with a round knob. It shrinks to fit the width of the window.
class KrimbleQuickAdjustLogo : public QWidget
{
public:
    explicit KrimbleQuickAdjustLogo(QWidget *parent = nullptr) : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumHeight(66);
        setAccessibleName(QStringLiteral("QuickAdjust"));
    }
    QSize sizeHint() const override { return QSize(300, 70); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QString quick = QStringLiteral("Quick"), adjust = QStringLiteral("Adjust");
        QFont f = font();
        f.setBold(true);
        f.setItalic(true);
        f.setWeight(QFont::Black);
        f.setLetterSpacing(QFont::PercentageSpacing, 96);
        f.setPixelSize(44);
        QFontMetricsF fm(f);
        const qreal total = fm.horizontalAdvance(quick) + fm.horizontalAdvance(adjust);
        const qreal avail = width() - 28.0;
        if (total > avail) {
            f.setPixelSize(qMax(18, int(44.0 * avail / total)));
            fm = QFontMetricsF(f);
        }
        const qreal quickWidth = fm.horizontalAdvance(quick);
        const qreal textWidth = quickWidth + fm.horizontalAdvance(adjust);
        const qreal x = (width() - textWidth) / 2.0;
        const qreal y = 8.0 + fm.ascent();
        QPainterPath quickPath, adjustPath;
        quickPath.addText(x, y, f, quick);
        adjustPath.addText(x + quickWidth, y, f, adjust);
        // glow behind the letters
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(56, 198, 255, 80), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(quickPath);
        p.setPen(QPen(QColor(253, 96, 1, 90), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(adjustPath);
        // the letters
        QLinearGradient quickFill(0, y - fm.ascent(), 0, y + fm.descent());
        quickFill.setColorAt(0.0, QColor(0xff, 0xff, 0xff));
        quickFill.setColorAt(1.0, QColor(0xb4, 0xc2, 0xff));
        p.setPen(QPen(QColor(8, 12, 36), 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(quickFill);
        p.drawPath(quickPath);
        QLinearGradient adjustFill(0, y - fm.ascent(), 0, y + fm.descent());
        adjustFill.setColorAt(0.0, QColor(0xff, 0x8d, 0x42));
        adjustFill.setColorAt(0.5, QColor(0xfd, 0x60, 0x01));
        adjustFill.setColorAt(1.0, QColor(0xc4, 0x4a, 0x00));
        p.setBrush(adjustFill);
        p.drawPath(adjustPath);
        // the slider line under the word, with its knob
        const qreal lineY = y + fm.descent() + 7.0;
        p.setPen(QPen(QColor(0x53, 0x5c, 0x80), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(x, lineY), QPointF(x + textWidth, lineY));
        p.setPen(QPen(QColor(0xfd, 0x60, 0x01), 3, Qt::SolidLine, Qt::RoundCap));
        const qreal knobX = x + textWidth * 0.62;
        p.drawLine(QPointF(x, lineY), QPointF(knobX, lineY));
        p.setPen(QPen(QColor(0xf4, 0xf6, 0xff), 1.5));
        p.setBrush(QColor(0xf4, 0xf6, 0xff));
        p.drawEllipse(QPointF(knobX, lineY), 5.0, 5.0);
    }
};
#endif   // Krimble 2026-10-30: end of the logo drawn in code (switched off)

// Krimble 2026-11-05: a picture that fills the width it is given and keeps its shape (George's QuickAdjust and Color Studio logos, white on a
// transparent background). It is drawn smoothly at the screen's own resolution, so it is much bigger than the text and stays sharp.
class KrimbleLogoLabel : public QWidget
{
public:
    explicit KrimbleLogoLabel(const QString &resource, const QString &name, QWidget *parent = nullptr, int maxLogicalWidth = 340) : QWidget(parent), m_pix(resource)
    {
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
        setAccessibleName(name);
        setMaximumWidth(maxLogicalWidth);   // Krimble 2026-11-06 (2nd): 340 unless the caller says otherwise (the QuickAdjust banner is as wide as the window)
    }
    bool hasHeightForWidth() const override { return !m_pix.isNull(); }
    int heightForWidth(int w) const override { return m_pix.isNull() ? 0 : int(double(w) * m_pix.height() / m_pix.width()); }
    QSize sizeHint() const override { return QSize(200, heightForWidth(200)); }
protected:
    void resizeEvent(QResizeEvent *) override
    {
        // the height follows the width, so the logo keeps its shape whatever layout it sits in
        const int h = heightForWidth(width());
        if (h > 0 && (height() != h || minimumHeight() != h)) setFixedHeight(h);
    }
    void paintEvent(QPaintEvent *) override
    {
        if (m_pix.isNull()) return;
        const qreal ratio = devicePixelRatioF();
        const QSize target = (size() * ratio).boundedTo(QSize(100000, 100000));
        if (m_cache.isNull() || m_cacheSize != target) {
            m_cache = m_pix.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_cache.setDevicePixelRatio(ratio);
            m_cacheSize = target;
        }
        QPainter p(this);
        p.drawPixmap(QPointF(0, 0), m_cache);
    }
private:
    QPixmap m_pix;
    QPixmap m_cache;
    QSize m_cacheSize;
};

// Krimble 2026-11-05: the image window's menu bar icon button (Qt's QMdi::ControlLabel; it is invisible because the window icon is blank) is made click-through
// whenever Qt puts it in the menu bar. On a style change Qt sizes it without dividing by the screen density, so on this phone it is about 3.4 times too
// big and covers "File"; a tap on "File" then opened the image window's system menu (George, 1.0.45-beta). Taps go through it to the menu bar now.
// Tested with real Qt: the label gets the attribute while the window keeps its system menu hint (so the close button and Close stay).
// Krimble 2026-11-16 (722nd): George (build 64): "The orange K logo is too close to File until you open an image, and then it fits. I'd like for it to fit all the time." With an image
// open, the image window's icon button sits in the bar's left corner and Qt starts the menus 16 units later; with no image there was no button and the menus started at the edge, under the
// K the bar draws. (The gap item in front of the menus never helped: Qt gives an item without text no width at all. Tried in real Qt 5.15.) With no image open the bar now has this
// empty holder in its left corner, the same size as the image window's button, so the menus are in the same place with and without an image. Qt swaps it for the image window's button
// when an image is opened and puts it back when the last one is closed (tried in real Qt 5.15: "File" starts at the same place in every state).
class KrimbleKHolder : public QWidget
{
public:
    explicit KrimbleKHolder(QWidget *parent) : QWidget(parent)
    {
        setObjectName(QStringLiteral("krimbleKHolder"));
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setFixedSize(22, 22);
    }
    QSize sizeHint() const override { return QSize(qMax(16, minimumWidth()), 16); }   // Krimble 2026-11-16 (728th): as wide as the K needs (set by placeK)
};

class KrimbleMenuBarFilter : public QObject
{
public:
    explicit KrimbleMenuBarFilter(QObject *parent) : QObject(parent) {}

    // Krimble 2026-11-06: George (screenshot with an arrow): "THE ORANGE K AT TOP LEFT IS RUNNING AWAY AT THE TOP." and "MAKE TOP BAR TALLER AND ORANGE K BIGGER." The "orange K" is the
    // image window's icon button in the menu bar (Qt's QMdi::ControlLabel) showing the program icon (the window icon was set to it on 2026-11-05). Qt draws that icon at 16 x 16
    // whatever the size of the button, and the button is sized without dividing by the screen density, so on this phone the K was a small piece at the top edge of the bar.
    // Tested in real Qt: the button is the menu bar's top left corner widget, a fixed size is respected, and the bar grows to fit if it is given a minimum height. Now the button is
    // 28 px square (logical), the bar is at least 32 px tall (it was 22), and the K is drawn by this filter scaled to the whole button.
    static void adopt(QWidget *label, QMenuBar *bar)
    {
        if (!label || label->property("krimbleKAdopted").toBool()) return;
        label->setProperty("krimbleKAdopted", true);
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        // Krimble 2026-11-07: George: "make the K and the menubar smaller." 22 px (was 28; Qt's own size was 16) and a 26 px bar (was 32; Qt's own was 22).
        label->setFixedSize(22, 22);   // Krimble 2026-11-11: back to 22 x 22 together with the menu bar's own height (see below)
        if (bar) {
            // Krimble 2026-11-11: George: "the menu bar is too tall in the build I'm using. Please put it back." The 26 px minimum height (2026-11-07; 32 px on 2026-11-06) is gone: the bar is as tall
            // as Qt makes it from the menu names and the 22 px button, as it was before the K was enlarged.
            bar->updateGeometry();
            // Krimble 2026-11-07: not needed at 22 px: Qt reserves 16 px + 6 px for the button, so a 22 px K ends where the first menu starts (tested in real Qt; a spacer item is
            // 13 px wide at least and pushed the menus too far). The spacer code below stays, for a bigger K.
        }
    }

    // Krimble 2026-11-07: George (screenshot of a Settings menu with Full Screen, Configure Toolbars and Preferences): "Why do we have a Settings menu???" He asked for it to be gone on
    // 2026-10-06 (everything in it moved: Preferences to Edit, Full Screen to View > Screen Mode, Customize Toolbar to Window). KDE's standard menu file supplies a Settings
    // menu on its own when the program's menu file has none, so it comes back with those three. Any menu bar entry called Settings is hidden, now and whenever the menu bar
    // is built again (the actions are in other menus too, so nothing is lost).
    static void hideSettingsMenu(QMenuBar *bar)
    {
        if (!bar) return;
        Q_FOREACH (QAction *entry, bar->actions()) {
            if (!entry->menu()) continue;
            QString title = entry->text();
            title.remove(QLatin1Char('&'));
            if (title.trimmed().compare(QLatin1String("Settings"), Qt::CaseInsensitive) == 0 && entry->isVisible())
                entry->setVisible(false);
        }
    }

    // Qt lays the menus out as if the button were its own 16 px (it ignores a fixed size), so with a 28 px button "File" would sit under the K. Tested in real Qt: a small
    // empty item in front of the first menu pushes all the menus to the right (File starts 13 px later). If the menu bar is rebuilt and the item is lost, it is put back.
    static void ensureSpacer(QMenuBar *bar)
    {
        if (!bar) return;
        QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
        if (!spacer) {
            spacer = new QWidgetAction(bar);
            spacer->setObjectName(QStringLiteral("krimbleKSpacer"));
            QWidget *gap = new QWidget(bar);
            // gap->setFixedSize(10, 28);
            gap->setFixedSize(4, 22);   // 4 px: the 22 px button leaves room for Qt's own 16 px reservation plus this small gap
            gap->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            spacer->setDefaultWidget(gap);
        }
        const QList<QAction *> all = bar->actions();
        if (!all.isEmpty() && all.first() == spacer) return;
        if (all.contains(spacer)) bar->removeAction(spacer);
        const QList<QAction *> rest = bar->actions();
        if (rest.isEmpty()) bar->addAction(spacer); else bar->insertAction(rest.first(), spacer);
    }

    // Krimble 2026-11-11: George (a screenshot of the File menu open): "K overlap onto file. Why not K always in the corner a little left?" Qt puts the button of the orange K a few pixels
    // in from the left edge of the bar (about 11 px on the phone), and it starts the first menu name only about 21 px in from the edge, so the K (the button is 22 px wide) reached
    // over the start of "File" and the box that marks the open menu. Now the button is always put in the corner (1 px from the left edge, centered up and down), and if the first menu
    // name starts closer than 4 px to the right of it, the spacer item in front of the menus (ensureSpacer) is made wider by the missing amount (it only grows, and never past 60 px).
    // It is done after every layout of the bar (and whenever Qt moves the button), and does nothing when everything is already in place.
    static void placeK(QMenuBar *bar)
    {
        if (!bar) return;
        // Krimble 2026-11-16 (722nd): George (build 64): "menubar should be taller by a few pixels by adding more shadow area at bottom." The bar is 4 units taller than Qt makes it from the
        // menu names; the names, the K and the window buttons stay where they were (centered in the old height), so the 4 units are all under them.
        // {
        //     const int wantMin = bar->sizeHint().height() + krimbleBarExtra;
        //     if (bar->minimumHeight() != wantMin) bar->setMinimumHeight(wantMin);
        //     if (bar->property("krimbleBarExtra").toInt() != krimbleBarExtra) bar->setProperty("krimbleBarExtra", krimbleBarExtra);
        // }
        // Krimble 2026-11-16 (728th): George (build 65): "Extra shadow lines on menubar disappeared. Menu appeared detached from title. No good. That was a decision before. The menu connects to
        // the title without overlap and not separated." and "Would like a couple more lines under the menu titles before getting down to the shadow." The 722nd made the BAR taller than its
        // names, so the open name's box stopped 4 units above the bar's lower edge and the menu hung under a gap. The bar is no longer forced taller. Instead every name has 3 more units of
        // room under its text (the style sheet, thememanager.cpp: padding-bottom), so the name's box is the full height of the bar again and the menu meets it. The K and the window buttons
        // are centered on the text (3 units above the middle of the bar).
        const int krimbleBarExtra = 3;
        if (bar->minimumHeight() != 0) bar->setMinimumHeight(0);
        if (bar->property("krimbleBarExtra").toInt() != krimbleBarExtra) bar->setProperty("krimbleBarExtra", krimbleBarExtra);
        // Krimble 2026-11-16 (728th): George (build 65, tabs, no corner button): "File menu title is still too close to the orange K." The holder of the 722nd was created and never SHOWN: a
        // widget added to a bar that is already on screen stays hidden until show() is called, and Qt reserves no room for a hidden corner widget. Tried in real Qt 5.15 this way round (bar
        // shown first, holder added after): hidden, "File" starts at 6; shown, at the holder's width plus 6. The holder is shown, and is as wide as the K plus 8 units.
        {
            const QFontMetrics holderMetrics(bar->font());
            const int holderWidth = 2 + qMin(bar->height() - 4, qMax(14, holderMetrics.height())) + 8;
            QWidget *corner = bar->cornerWidget(Qt::TopLeftCorner);
            if (!corner) {
                corner = new KrimbleKHolder(bar);
                bar->setCornerWidget(corner, Qt::TopLeftCorner);
            }
            if (corner->objectName() == QLatin1String("krimbleKHolder")) {
                if (corner->width() != holderWidth || corner->minimumWidth() != holderWidth) corner->setFixedWidth(holderWidth);
                // shown unless Qt has put it aside for an image window's own button (then it is not the bar's corner widget and this code does not see it)
                if (!corner->isVisible() && bar->isVisible()) corner->show();
            }
        }
        // Krimble 2026-11-17 (738th): George (build 67 screenshots, no image open): "Same orange K too close to the File menu." The holder and the spacer above still left "File" (and its box when
        // open) right against the K. Now the menu bar measures where the first menu name actually starts and, if that is short of the K plus 8 units, adds the difference as left padding
        // (style sheet, the menu bar's own), whatever Qt reserves or does not reserve in front of it. At most the K plus 8 units, so it cannot run away.
        {
            const QFontMetrics krimbleKMetrics(bar->font());
            const int krimbleKRoom = 2 + qMin(bar->height() - 4, qMax(14, krimbleKMetrics.height())) + 8;
            QAction *krimbleFirst = nullptr;
            Q_FOREACH (QAction *a, bar->actions()) {
                if (a->isVisible() && a->objectName() != QLatin1String("krimbleKSpacer") && !qobject_cast<QWidgetAction*>(a)) { krimbleFirst = a; break; }
            }
            if (krimbleFirst && bar->isVisible()) {
                const int have = bar->actionGeometry(krimbleFirst).left();
                const int cur = bar->property("krimblePadLeft").toInt();
                const int want = qBound(0, cur + krimbleKRoom - have, krimbleKRoom);
                if (have > 0 && want != cur) {
                    bar->setProperty("krimblePadLeft", want);
                    bar->setStyleSheet(QStringLiteral("QMenuBar { padding-left: %1px; }").arg(want));
                }
            }
        }
        // Krimble 2026-11-16 (722nd): George (build 64): "The minimize, maximize, and close gadgets render too high in the menu bar." Qt puts them at the top edge of the bar; they are
        // centered up and down on the menu names (as the K is), now and whenever Qt moves them back.
        if (QWidget *buttons = bar->cornerWidget(Qt::TopRightCorner)) {
            if (buttons->isVisible() && QByteArray(buttons->metaObject()->className()).endsWith("ControllerWidget")) {
                const int y = qMax(0, (bar->height() - krimbleBarExtra - buttons->height()) / 2);
                if (buttons->y() != y) buttons->move(buttons->x(), y);
            }
        }
        QWidget *label = bar->cornerWidget(Qt::TopLeftCorner);
        // Krimble 2026-11-15: George (screenshot of the welcome page, no image open): "The k too close." With no image open the bar has no corner button (the K is drawn by the overlay), so this function returned at once and the
        // gap in front of the menus was never widened. The button is only moved when it exists; the gap is made wide enough in every case.
        const bool krimbleHaveLabel = label && QByteArray(label->metaObject()->className()).endsWith("ControlLabel") && label->isVisible();
        const QPoint target(1, krimbleHaveLabel ? qMax(0, (bar->height() - krimbleBarExtra - label->height()) / 2) : 0);   // Krimble 2026-11-16 (722nd): in the bar's old height
        if (krimbleHaveLabel && label->pos() != target) label->move(target);
        QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
        QAction *first = nullptr;
        Q_FOREACH (QAction *entry, bar->actions()) {
            if (entry == spacer || !entry->isVisible() || !entry->menu()) continue;
            first = entry;
            break;
        }
        if (!first) return;
        // Krimble 2026-11-15: George: "File menu title is still too close to the orange K. PLEASE MOVE THE MENU TO THE RIGHT!!!!!" The gap was measured from the invisible button (22 units wide), but the K the bar draws is as tall as the menu font
        // (drawK, 2 units from the left edge), so the open menu's box started at the K's right edge and the name "File" about 11 units later (the sheet's padding), only about 3 units clear on the phone. The first menu's box now starts at least
        // 8 units right of the drawn K.
        const int krimbleKSize = qMin(bar->height() - 4, qMax(14, QFontMetrics(bar->font()).height()));
        const int deficit = (2 + krimbleKSize + 8) - bar->actionGeometry(first).x();
        if (deficit <= 0) return;
        if (!spacer) {
            ensureSpacer(bar);
            spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
        }
        QWidget *gap = spacer ? spacer->defaultWidget() : nullptr;
        if (!gap || gap->width() >= 90) return;   // Krimble 2026-11-15: was 60
        gap->setFixedSize(qMin(90, gap->width() + deficit), gap->height());
        // Krimble 2026-11-15 (706th): George (build 63): "K still starts out too close then corrects later." The menu bar keeps the places of its entries and works them out again only when an entry
        // changes (or the bar is resized); a new size of the gap's widget alone does not count, so the wider gap showed only after something else made the bar lay out again (opening an image).
        // Changing the entry's data makes Qt send the bar an "entry changed" event, and the bar lays its entries out again at once.
        spacer->setData(gap->width());
        bar->update();
    }

    static void schedulePlaceK(QMenuBar *bar)
    {
        if (!bar || bar->property("krimbleKPlacePending").toBool()) return;
        bar->setProperty("krimbleKPlacePending", true);
        QTimer::singleShot(0, bar, [bar]() {
            bar->setProperty("krimbleKPlacePending", false);
            KrimbleMenuBarFilter::placeK(bar);
        });
    }
protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::ChildAdded) {
            if (QWidget *child = qobject_cast<QWidget *>(static_cast<QChildEvent *>(event)->child())) {
                if (QByteArray(child->metaObject()->className()).endsWith("ControlLabel")) {
                    adopt(child, qobject_cast<QMenuBar *>(watched));
                    child->installEventFilter(this);
                }
                // Krimble 2026-11-16 (722nd): the window buttons (minimize, restore, close) of an image, at the right end of the bar: watched, so they are centered again whenever Qt moves them
                else if (QByteArray(child->metaObject()->className()).endsWith("ControllerWidget")) {
                    child->installEventFilter(this);
                    schedulePlaceK(qobject_cast<QMenuBar *>(watched));
                }
            }
        } else if ((event->type() == QEvent::LayoutRequest || event->type() == QEvent::Resize || event->type() == QEvent::Show) && qobject_cast<QMenuBar *>(watched)) {
            schedulePlaceK(qobject_cast<QMenuBar *>(watched));   // 2026-11-11: the K in the corner, the menus clear of it
        } else if ((event->type() == QEvent::Move || event->type() == QEvent::Show) && qobject_cast<QWidget *>(watched)
                   && (QByteArray(watched->metaObject()->className()).endsWith("ControlLabel") || QByteArray(watched->metaObject()->className()).endsWith("ControllerWidget"))) {   // Krimble 2026-11-16 (722nd): the window buttons too
            schedulePlaceK(qobject_cast<QMenuBar *>(static_cast<QWidget *>(watched)->parentWidget()));   // Qt moved the button: put it back in the corner
        } else if ((event->type() == QEvent::ActionAdded || event->type() == QEvent::ActionRemoved) && qobject_cast<QMenuBar *>(watched)) {
            QMenuBar *bar = qobject_cast<QMenuBar *>(watched);
            QTimer::singleShot(0, bar, [bar]() { KrimbleMenuBarFilter::hideSettingsMenu(bar); });   // 2026-11-07: no Settings menu, also after the menu bar is rebuilt
            schedulePlaceK(bar);   // Krimble 2026-11-15 (706th): the gap is checked again when the menus arrive (at the start the bar is shown before it has its menus, and then nothing was measured)
            QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
            const QList<QAction *> all = bar->actions();
            if (spacer && (all.isEmpty() || all.first() != spacer))
                QTimer::singleShot(0, bar, [bar]() { KrimbleMenuBarFilter::ensureSpacer(bar); });   // the menu bar was rebuilt: put the spacer back in front
        } else if (event->type() == QEvent::Paint && qobject_cast<QMenuBar *>(watched)) {
            // Krimble 2026-11-08: George: "The menu bar looks like shit currently. FIX IT." (and, of the mockups: "the top bar that darkens near each end. Those are much better."). The
            // bar's background is painted here, under the menu names (the style sheet makes the bar's own background transparent): a horizontal gradient that is dark at both ends and lighter
            // in the middle (#1C2346, #394585, #1C2346: from the mockups' title bars), a little lighter at the top and darker toward the bottom, a light line along the top, and a dark edge
            // along the bottom with a lighter line above it. Tried in real Qt next to the mockups (menubar_compare.png). Before: a plain top-to-bottom gradient #374676 to #242D4C.
            // Krimble 2026-11-11: this painting belongs to the "Krimble Painted" theme only (thememanager.cpp sets the property). For the flat theme "Krimble dark" and for every other theme
            // the bar is left to the style sheet and the palette (before, it was painted over for every theme, so the flat top bar color never showed).
            // Krimble 2026-11-13 (task 46): George: "The orange K is still too far to the right." Qt puts the K's button about 11 px in from the left edge of the bar and may move it again whenever the bar is laid out, so
            // the K drawn inside that button could be anywhere from 1 to 13 px in (about 42 screen pixels on the phone). The K is drawn HERE, by the bar itself, 2 px from the bar's left edge and
            // centered up and down, whatever the button does (the button paints nothing now; the first menu name still starts after the button, so they cannot overlap).
            auto drawK = [watched]() {   // after the painting of the bar when the bar is painted (the Painted theme), or at once
                QWidget *kBar = static_cast<QWidget *>(watched);
                const QFontMetrics kMetrics(kBar->font());
                const int kSize = qMin(kBar->height() - 4, qMax(14, kMetrics.height()));
                QPainter kPainter(kBar);
                kPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                qApp->windowIcon().paint(&kPainter, QRect(2, (kBar->height() - kBar->property("krimbleBarExtra").toInt() - kSize) / 2, kSize, kSize));   // Krimble 2026-11-16 (722nd): centered on the menu names (the bar's extra height is under them)
            };
            if (!qApp->property("krimblePaintedTheme").toBool()) {
                drawK();
                return false;
            }
            QWidget *bar = static_cast<QWidget *>(watched);
            QPainter painter(bar);
            const QRectF r(bar->rect());
            QLinearGradient across(r.left(), 0, r.right(), 0);
            across.setColorAt(0.0, QColor("#1c2346"));
            across.setColorAt(0.5, QColor("#394585"));
            across.setColorAt(1.0, QColor("#1c2346"));
            painter.fillRect(r, across);
            // QLinearGradient down(0, r.top(), 0, r.bottom());
            // Krimble 2026-11-11: George (Nov 9): "The menu bar, ANY bar, is NOT SUPPOSED TO BE SHADED ON THE BOTTOM!!!!!!! IT LOOKS PUFFY. IT WAS SUPPOSED TO BE DARKENED TO THE CORNERS." The top-to-bottom
            // shading (light at the top, dark at the bottom) is gone: the bar shades across only, dark at both ends (this fix was made in an earlier chat and lost; see the changelog).
            painter.setPen(QColor(150, 165, 225, 120));
            painter.drawLine(0, 0, bar->width(), 0);
            painter.setPen(QColor(62, 74, 128, 200));
            painter.drawLine(0, bar->height() - 2, bar->width(), bar->height() - 2);
            painter.setPen(QColor(8, 11, 28, 255));
            painter.drawLine(0, bar->height() - 1, bar->width(), bar->height() - 1);
            painter.end();
            drawK();   // Krimble 2026-11-13 (task 46)
            return false;   // the bar then draws its menu names on top as usual
        } else if (event->type() == QEvent::Paint) {
            QWidget *label = qobject_cast<QWidget *>(watched);
            if (label && QByteArray(label->metaObject()->className()).endsWith("ControlLabel")) {
                // Krimble 2026-11-08: George: "Also do something about the stupid orange K in the corner. Get rid of it." Nothing is drawn in the button now (returning true swallows the
                // paint, so Qt's own icon is not drawn either). The button stays, invisible and click-through as before 2026-11-05, so the image window keeps its system menu hint (the
                // close and restore buttons on the right are separate). The code that drew the K (18 x 18 px, 2026-11-07; earlier the whole button) is kept in this comment:
                // Krimble 2026-11-11: George: "The problem was that it wasn't rendering correctly. It was either too big or out of position and needed to be moved." The orange K is drawn
                // again at the left end of the menu bar (the top left of the image window), and the two faults are fixed: SIZE: as tall as the menu names (the menu bar's font height,
                // never more than the button), so it follows the font and the UI scale and cannot be too big; POSITION: centered on the menu bar's middle line (not on the button, which Qt
                // can place at the top edge) and centered across the button's width. Qt's own icon is still swallowed (return true).
                return true;
            }
        }
        return false;
    }
};

// Krimble 2026-10-30: "Keep as layers" in the Quick Adjust window (off at first; kept while the program runs)
static bool g_krimbleQuickKeepLayers = false;

class KrimbleQuickAdjust : public QObject
{
public:
    KrimbleQuickAdjust(KisMainWindow *win, QWidget *content) : QObject(content), m_win(win)
    {
        QVBoxLayout *layout = new QVBoxLayout(content);
        // Krimble 2026-11-05: George: QuickAdjust is a "complete disaster" next to ChatGPT's mockup (screenshots 2026-11-05). The mockup has ONE line per
        // adjustment, in its own rounded frame: picture, name, number box, then the slider filling the rest of the line, a thin groove and a small round
        // handle, with Reset at the left and Cancel and OK at the right. Ours had the slider on a second line, with a groove and handle about three times
        // too big. Rows are one framed line each, the columns line up, the pictures are smaller, and the groove and handle sizes are set for the
        // QuickAdjust sliders only (the app-wide style sheet is unchanged). The text stays as large as in the rest of the app.
        // Krimble 2026-11-06: George: "way too big still. Shorten it. Eliminate space between the adjusters. Eliminate negative space." Tighter: the rows are 2 px
        // apart (were 6), margins 4 (were 8), a row has 1 px above and below (was 3), the number boxes are 20 px tall and the sliders 16 (were about 24 and 22), and
        // there is no stretch before the buttons.
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(5);   // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." (the picture: rows 4 px apart)
        QHBoxLayout *options = new QHBoxLayout;
        // Krimble 2026-11-06 (2nd): the mockup's purple sliders picture (cut out of the mockup, 104 x 60 px, about 30 px wide on the screen) instead of the word "Adjustments"
        options->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-sliders.png"), i18n("Adjustments"), content, 34));
        options->addStretch();
        // Krimble 2026-10-30: the Preview switch of the drawing (a pill that says ON or OFF), with its label
        m_previewLabel = new QLabel(i18n("Preview"), content);   // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The Preview label and switch move to the button row at the bottom (see krimbleShowQuickAdjustDialog).
        options->addWidget(m_previewLabel);
        m_preview = new KrimbleToggleSwitch(content);
        m_preview->setChecked(true);
        options->addWidget(m_preview);
        // Krimble 2026-11-06 (3rd): George: "match colors in areas at top." The row with the sliders picture and the Preview switch is a panel, as in the mockup: a fill from #313251
        // (top) to #2D314E (bottom) and a #343458 outline (sampled from the mockup picture).
        {
            QFrame *topPanel = new QFrame(content);
            topPanel->setObjectName(QStringLiteral("krimbleQuickTop"));
            topPanel->setStyleSheet(QStringLiteral("QFrame#krimbleQuickTop { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #313251, stop:1 #2d314e); border: 1px solid #343458; border-radius: 6px; }"
                                                   "QFrame#krimbleQuickTop QLabel { background: transparent; border: none; }"));
            options->setContentsMargins(4, 1, 4, 1);
            topPanel->setLayout(options);
            topPanel->hide();   // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The top panel (sliders picture, Preview) is not in the picture; Preview is in the bottom row.
        }
        // Krimble 2026-10-30: George: "I didn't expect operations as layers. Offer it as a toggle." The adjustments are applied to the layer when OK is
        // pressed (merged down into the paint layer below), unless this is checked: then they stay as adjustment layers (the old behaviour).
        m_keepLayers = new QCheckBox(i18n("Keep as layers"), content);
        m_keepLayers->setChecked(g_krimbleQuickKeepLayers);
        connect(m_keepLayers, &QCheckBox::toggled, this, [](bool on) { g_krimbleQuickKeepLayers = on; });
        // Krimble 2026-11-05: every name has the width of the longest one, so the number boxes and sliders line up in columns
        int nameWidth = 0;
        for (int i = 0; i < SliderCount; ++i) nameWidth = qMax(nameWidth, content->fontMetrics().horizontalAdvance(i18n(def(i).name)));
        for (int i = 0; i < SliderCount; ++i) {
            QFrame *frame = new QFrame(content);
            frame->setObjectName(QStringLiteral("krimbleQuickRow"));
            // Krimble 2026-11-06 (2nd): the mockup's row: a fill from #282A41 (top) to #1F2235 (bottom) and a #404565 outline (sampled from the mockup picture)
            frame->setStyleSheet(QStringLiteral("QFrame#krimbleQuickRow { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #282a41, stop:1 #1f2235); border: 1px solid #404565; border-radius: 6px; }"));
            // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The picture's row: a fill from #202141 to #13142b, a purple #6058aa outline, rounded 10 px, a big icon at the left, the name over the number box, the slider on the right.
            frame->setStyleSheet(QStringLiteral("QFrame#krimbleQuickRow { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #202141, stop:1 #13142b); border: 1px solid #6058aa; border-radius: 10px; }"));
            QGridLayout *row = new QGridLayout(frame);
            // row->setSpacing(6);
            // row->setContentsMargins(6, 1, 6, 1);
            // Krimble 2026-11-15 (item 12): George (build 60 screenshot, QuickAdjust still 75% of the screen tall): "QuickAdjust is still HUGE." The rows are about a third shorter: picture 30 (was 46), number box 64 x 22
            // (was 80 x 28), margins 3 above and below (was 5), no gap between the name and the number box (was 2).
            // Krimble 2026-11-15 (2nd): George: "Make it tiny." One line per row now: picture, name, slider, number box.
            row->setContentsMargins(4, 2, 6, 2);
            row->setHorizontalSpacing(6);
            row->setVerticalSpacing(0);
            // Krimble 2026-10-30: the small picture at the start of the row (the drawing's sun, half circle, drops, color ring, half square, sparkle)
            QLabel *rowIcon = new QLabel(content);
            // Krimble 2026-11-06: George: "Can we get things to render in native resolution so they're sharp?" The picture was 16 x 16 pixels, stretched 3.4 times to the
            // screen; it is drawn at the screen's own resolution now (devicePixelRatio set), still 16 logical px wide.
            {
                // the six pictures cut out of George's picture (krita/pics/svg/dark_krimble-quickadjust-icon-N.png), 38 logical px, sharp at the screen's resolution
                const qreal ratio = content->devicePixelRatioF();
                QPixmap big(QStringLiteral(":/dark_krimble-quickadjust-icon-%1.png").arg(i));
                QPixmap sharp = big.scaled(qRound(20 * ratio), qRound(20 * ratio), Qt::KeepAspectRatio, Qt::SmoothTransformation);   // Krimble 2026-11-15 (item 12): was 44; (2nd) 28 before "Make it tiny."
                sharp.setDevicePixelRatio(ratio);
                rowIcon->setPixmap(sharp);
                rowIcon->setStyleSheet(QStringLiteral("background: transparent; border: none;"));
            }
            rowIcon->setFixedSize(22, 22);   // Krimble 2026-11-15 (2nd): "Make it tiny."
            row->addWidget(rowIcon, 0, 0, Qt::AlignCenter);
            m_labels[i] = new QLabel(i18n(def(i).name), content);
            // Krimble 2026-11-15 (item 12), George: text 4x the rest of the screen is too big. Old line: (sizes 15/15/16/13/16 px before)
            m_labels[i]->setStyleSheet(QStringLiteral("font-weight: bold; color: #ffffff; background: transparent; border: none;"));   // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust."
            row->addWidget(m_labels[i], 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
            m_numbers[i] = new QSpinBox(content);
            m_numbers[i]->setRange(def(i).min, def(i).max);
            // Krimble 2026-11-15: George: "I can't see the numbers any more. They have no space." 74 wide (the width of his picture of QuickAdjust), same height; the arrow buttons are 16 wide (were 22), so the number has about 55 units, was about 34.
            m_numbers[i]->setFixedSize(74, 20);
            // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." (the picture: 74 x 25 logical px)
            m_numbers[i]->setAlignment(Qt::AlignCenter);
            // Krimble 2026-11-12 (task 5): the picture's number box has a small up and down arrow at the right, with a thin line before them (arrow pictures: dark_krimble-quickadjust-arrow-up.png and -down.png)
            m_numbers[i]->setStyleSheet(QStringLiteral(
                // Krimble 2026-11-15 (item 12), George: text 4x the rest of the screen is too big. Old line: (sizes 15/15/16/13/16 px before)
                // "QSpinBox { font-weight: bold; color: #ffffff; padding-right: 22px; }"
                "QSpinBox { font-weight: bold; color: #ffffff; padding-right: 17px; padding-left: 0px; }"
                // "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 22px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                // "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 22px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                // 
                "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 16px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 16px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                "QSpinBox::up-arrow { image: url(:/dark_krimble-quickadjust-arrow-up.png); width: 9px; height: 7px; }"
                "QSpinBox::down-arrow { image: url(:/dark_krimble-quickadjust-arrow-down.png); width: 9px; height: 7px; }"));
            // row->addWidget(m_numbers[i], 1, 1, Qt::AlignLeft | Qt::AlignTop);
            row->addWidget(m_numbers[i], 0, 3, Qt::AlignRight | Qt::AlignVCenter);
            // layout->addLayout(row);
            layout->addWidget(frame);
            m_sliders[i] = new QSlider(Qt::Horizontal, content);
            m_sliders[i]->setRange(def(i).min, def(i).max);
            m_sliders[i]->setMinimumWidth(1);
            m_sliders[i]->setMinimumHeight(18);   // Krimble 2026-11-15 (2nd): "Make it tiny." (the ball is 16 units; the row's other widgets are 20 to 22 high)
            m_sliders[i]->setObjectName(QStringLiteral("krimbleQuickAdjustSlider"));
            // Krimble 2026-11-05: thin groove (4 px) and a small round handle (14 px; a border radius of exactly half the handle size draws a square in Qt,
            // so it is 8 px, not 9) for the QuickAdjust sliders; the filled part is the selection blue.
            // const QString thinHandle = QStringLiteral(
            //     "QSlider::handle:horizontal { width: 14px; height: 14px; margin: -6px 0; border: 2px solid #c9dfff; border-radius: 8px;"
            //     " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f1f5ff, stop:1 #728eb8); }");
            // Krimble 2026-11-05 (2nd): George: "Quick Adjust lines are too thick. Gadgets are goofy looking. Make them tiny metal balls." A 12 px metal ball
            // (light radial gradient, bright spot at the top left, 1 px dark rim; 5.5 px radius, because exactly half draws a square in Qt) on a 2 px line.
            // const QString thinHandle = QStringLiteral(
            //     "QSlider::handle:horizontal { width: 12px; height: 12px; margin: -5px 0; border: 1px solid #4d5573; border-radius: 5.5px;"
            //     " background: qradialgradient(cx:0.4,cy:0.35,radius:0.75,fx:0.35,fy:0.3, stop:0 #ffffff, stop:0.35 #d9dde6, stop:0.75 #9aa1b3, stop:1 #6b7390); }");
            // Krimble 2026-11-06 (3rd): George: "match the sliders perfectly in color and scale." Measured on the mockup picture (999 px wide; the window here is about 300 px, so 0.3 px
            // per mockup pixel): the ball is 44 px across = 13 px, light at the top (#F9FDFF, #CFD1E6, #AEB2CD, #8F93B0) down to #8A8EAB, with a #9DA0C9 rim; the line is about
            // 11 px = 3 px thick; its filled part is cyan at the top (#3FD1F6) through #0DA0FF to #008CEC at the bottom, and the rest is #72749D, #5C5F88, #7378A0.
            // const QString thinHandle = QStringLiteral(
            //     "QSlider::handle:horizontal { width: 13px; height: 13px; margin: -5px 0; border: 1px solid #9da0c9; border-radius: 6px;"
            //     " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f9fdff, stop:0.2 #cfd1e6, stop:0.4 #aeb2cd, stop:0.7 #8f93b0, stop:1 #8a8eab); }");
            // Krimble 2026-11-11: George: "make the handles round balls ... A metal ball bearing would look better." (then, of a first picture: "Too big. Too flat. Do you know what a ball
            // bearing looks like?", and "better" for the second). A steel ball 18 px across: the top of the ball reflects the bright sky (#F4F7FC to #C9D0DE), then a dark horizon band across the
            // middle (#4A5164, #262B38), then the lighter bounce from below (#7F889C to #C3CAD8), with a dark rim (#141822). A border radius of 8 on a handle 18 px wide with a 1 px border is a circle.
            // const QString thinHandle = QStringLiteral(
            //     "QSlider::handle:horizontal { width: 16px; height: 16px; margin: -7px 0; border: 1px solid #141822; border-radius: 8px;"
            //     " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f4f7fc, stop:0.30 #c9d0de, stop:0.46 #4a5164, stop:0.56 #262b38, stop:0.78 #7f889c, stop:1 #c3cad8); }");
            // Krimble 2026-11-15 (2nd): George: "Use my graphic." The handle is his ball bearing picture (krimble-ball-bearing.png), stretched to a 16 unit box.
            // Krimble 2026-11-15: George: "QuickAdjust size is better, but the ball bearings are too big." The ball is 10 units across (the same as every other slider), was 16.
            // const QString thinHandle = QStringLiteral(
            //     "QSlider::handle:horizontal { width: 16px; height: 16px; margin: -7px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing.png) 0 0 0 0 stretch stretch; }");
            const QString thinHandle = QStringLiteral(
                // "QSlider::handle:horizontal { width: 10px; height: 10px; margin: -4px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing.png) 0 0 0 0 stretch stretch; }"
                // Krimble 2026-11-16 (722nd): the smaller, darker ball with a cast shadow (see thememanager.cpp)
                "QSlider::handle:horizontal { width: 12px; height: 12px; margin: -5px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing-small.png) 0 0 0 0 stretch stretch; }");
            if (i != Hue) {
                // m_sliders[i]->setStyleSheet(QStringLiteral(
                //     "QSlider::groove:horizontal { height: 4px; border: 1px solid #535c80; border-radius: 3px; background: #20263f; }"
                //     "QSlider::sub-page:horizontal { height: 4px; border: 1px solid #146a9f; border-radius: 3px; background: #1d7cb2; }")
                //     + thinHandle);
                // m_sliders[i]->setStyleSheet(QStringLiteral(
                //     "QSlider::groove:horizontal { height: 2px; border: none; border-radius: 1px; background: #6f789c; }"
                //     "QSlider::sub-page:horizontal { height: 2px; border: none; border-radius: 1px; background: #2aa6e8; }")
                //     + thinHandle);
                m_sliders[i]->setStyleSheet(QStringLiteral(
                    "QSlider::groove:horizontal { height: 3px; border: none; border-radius: 1px;"
                    // " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #72749d, stop:0.4 #5c5f88, stop:1 #7378a0); }"
                    // "QSlider::sub-page:horizontal { height: 3px; border: none; border-radius: 1px;"
                    // " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #3fd1f6, stop:0.4 #0da0ff, stop:1 #008cec); }")
                    // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The picture's line: cyan up to the handle, white after it.
                    " background: #f4f5fb; }"
                    "QSlider::sub-page:horizontal { height: 3px; border: none; border-radius: 1px;"
                    " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #00f4ff, stop:1 #00c6f3); }")
                    + thinHandle);
            }
            if (i == Hue) {
                // Krimble 2026-11-11: George: "Hue should actually land on the hue it's selecting. It shouldn't be arbitrary." The rainbow is now centered on the foreground color's hue (red if it is
                // gray): at the middle (0) the line is the color's own hue, and at every other position the line's color IS the hue that the slider's value turns it into (value + hue). It follows the
                // foreground color while the window is open, like the Lightness and Saturation lines did.
                const QString hueHandle = thinHandle;
                auto updateHueLine = [this, hueHandle]() {
                    QColor foreground(Qt::red);
                    if (KisViewManager *view = m_win->viewManager()) {
                        if (view->canvasResourceProvider()) foreground = view->canvasResourceProvider()->fgColor().toQColor();
                    }
                    int hue = 0, saturation = 0, value = 0;
                    foreground.getHsv(&hue, &saturation, &value);
                    const int base = (hue < 0 || saturation == 0) ? 0 : hue;
                    QString stops;
                    for (int step = 0; step <= 6; ++step) {
                        const int h = (((base - 180 + step * 60) % 360) + 360) % 360;
                        stops += QStringLiteral(" stop:%1 %2,").arg(step / 6.0, 0, 'f', 4).arg(QColor::fromHsv(h, 255, 255).name());
                    }
                    stops.chop(1);
                    m_sliders[Hue]->setStyleSheet(QStringLiteral(
                        "QSlider::groove:horizontal { height: 3px; border: none; border-radius: 1px;"
                        " background: qlineargradient(x1:0,y1:0,x2:1,y2:0,%1); }"
                        "QSlider::sub-page:horizontal { background: transparent; border: none; }").arg(stops) + hueHandle);
                };
                updateHueLine();
                if (KisViewManager *view = m_win->viewManager()) {
                    if (view->canvasResourceProvider())
                        connect(view->canvasResourceProvider(), &KisCanvasResourceProvider::sigFGColorChanged, this, [updateHueLine](const KoColor &) { updateHueLine(); });
                }
                // Krimble 2026-10-30: the drawing's rainbow Hue slider: the groove shows the colors the slider goes through (and has no orange fill)
                // (the fixed rainbow below is the one before 2026-11-11, from magenta to red whatever the color; it is switched off by the "if (false)" and kept)
                if (false) m_sliders[i]->setStyleSheet(QStringLiteral(
                    // "QSlider::groove:horizontal { height: 9px; border: 1px solid #535c80; border-radius: 4px;"
                    // "QSlider::groove:horizontal { height: 5px; border: 1px solid #535c80; border-radius: 4px;"
                    "QSlider::groove:horizontal { height: 3px; border: none; border-radius: 1px;"
                    " background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #ff00c8, stop:0.2 #4a3cff, stop:0.4 #00c8ff,"
                    " stop:0.6 #1fdc3f, stop:0.8 #ffe600, stop:1 #ff2a00); }"
                    "QSlider::sub-page:horizontal { background: transparent; border: none; }") + thinHandle);
            }
            row->addWidget(m_sliders[i], 0, 2);   // Krimble 2026-11-15 (2nd): one line. Before: spanned two rows. Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." the slider on the right, as high as name and number together
            row->setColumnStretch(2, 1);
            connect(m_sliders[i], &QSlider::valueChanged, this, [this, i](int value) {
                m_values[i] = value;
                QSignalBlocker blocker(m_numbers[i]);
                m_numbers[i]->setValue(value);
                m_timer.start(90);
            });
            connect(m_numbers[i], qOverload<int>(&QSpinBox::valueChanged), this, [this, i](int value) {
                m_sliders[i]->setValue(value);
            });
        }
        layout->addWidget(m_keepLayers);   // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust."
        // Krimble 2026-11-05: Reset is in the button row now, at the left of Cancel and OK, like the mockup (see resetAll() and krimbleShowQuickAdjustDialog)
        m_timer.setSingleShot(true);
        connect(&m_timer, &QTimer::timeout, this, [this]() { apply(); });
        for (int i = 0; i < SliderCount; ++i) m_values[i] = 0;
        connect(m_preview, &QAbstractButton::toggled, this, [this](bool enabled) {
            m_previewEnabled = enabled;
            flush();
        });
    }

    // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." the Preview label and switch, put in the dialog's button row
    QWidget *previewLabel() const { return m_previewLabel; }
    QAbstractButton *previewSwitch() const { return m_preview; }

    // Krimble 2026-11-05: Reset (every slider back to 0), called from the Reset button in the dialog's button row
    void resetAll()
    {
        for (int i = 0; i < SliderCount; ++i) m_sliders[i]->setValue(0);
    }

    // Krimble 2026-10-13 (11th): for the dialog version (George: "Build Quick Adjust as a dialog window launched from Image Adjust."):
    // OK applies any slider move that is still waiting; Cancel takes back the adjustment layers made while the dialog was open.
    void flush()
    {
        m_timer.stop();
        apply();
    }
    void commitVisible()
    {
        m_previewEnabled = true;
        if (m_preview) m_preview->setChecked(true);
        flush();
    }
    // Krimble 2026-10-30: called when OK is pressed, after commitVisible(). Not checked "Keep as layers": the adjustment layers are merged down, bottom one first,
    // each into the paint layer directly below it (a layer that is not on a paint layer stays as a layer). Either way the layer that was active before
    // Quick Adjust is the active layer again (George: Edit > Fill > Foreground Color pasted a weird image while the adjustment layers were active: the fill
    // went into an adjustment layer).
    void finish()
    {
        KisViewManager *view = m_win->viewManager();
        if (!view || !view->nodeManager()) return;
        KisImageSP image = view->image();
        if (image && !(m_keepLayers && m_keepLayers->isChecked())) {
            QList<KisNodeSP> made;
            KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
            for (KisNodeWSP *ref : refs) {
                KisNodeSP node = *ref;
                if (node && node->parent()) made.append(node);
            }
            std::sort(made.begin(), made.end(), [](const KisNodeSP &a, const KisNodeSP &b) {
                return a->parent() == b->parent() && a->parent()->index(a) < b->parent()->index(b);
            });
            QAction *mergeDown = view->actionCollection()->action(QStringLiteral("merge_layer"));
            Q_FOREACH (KisNodeSP node, made) {
                KisNodeSP below = node->prevSibling();
                if (!below || !dynamic_cast<KisPaintLayer*>(below.data()) || !mergeDown) continue;
                view->nodeManager()->slotNonUiActivatedNode(node);
                mergeDown->trigger();
                image->waitForDone();
            }
        }
        if (m_activeBefore && m_activeBefore->parent()) view->nodeManager()->slotNonUiActivatedNode(m_activeBefore);
    }
    void removeCreatedLayers()
    {
        m_timer.stop();
        KisViewManager *view = m_win->viewManager();
        if (!view) return;
        KisNodeCommandsAdapter adapter(view);
        KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
        for (int i = 0; i < 3; ++i) {
            KisNodeSP node = *refs[i];
            if (node && node->parent()) adapter.removeNode(node);
            *refs[i] = KisNodeWSP();
        }
    }

private:
    enum { Brightness, Contrast, Saturation, Hue, Lightness, Vibrance, SliderCount };
    struct Def { const char *name; int min; int max; };
    static Def def(int i)
    {
        static const Def defs[SliderCount] = {
            { "Brightness", -100, 100 }, { "Contrast", -100, 100 }, { "Saturation", -100, 100 },
            { "Hue", -180, 180 }, { "Lightness", -100, 100 }, { "Vibrance", -100, 100 }
        };
        return defs[i];
    }
    void apply()
    {
        updateLayer(m_bcLayer, QStringLiteral("brightnesscontrast"), i18n("Quick Adjust: Brightness/Contrast"),
                    { { QStringLiteral("brightness"), m_values[Brightness] }, { QStringLiteral("contrast"), m_values[Contrast] } },
                    m_values[Brightness] != 0 || m_values[Contrast] != 0);
        updateLayer(m_hsvLayer, QStringLiteral("hsvadjustment"), i18n("Quick Adjust: Color"),
                    { { QStringLiteral("h"), m_values[Hue] }, { QStringLiteral("s"), m_values[Saturation] }, { QStringLiteral("v"), m_values[Lightness] } },
                    m_values[Hue] != 0 || m_values[Saturation] != 0 || m_values[Lightness] != 0);
        updateLayer(m_vibLayer, QStringLiteral("vibrance"), i18n("Quick Adjust: Vibrance"),
                    { { QStringLiteral("vibrance"), m_values[Vibrance] } }, m_values[Vibrance] != 0);
        KisNodeWSP *refs[3] = { &m_bcLayer, &m_hsvLayer, &m_vibLayer };
        for (KisNodeWSP *ref : refs) {
            KisNodeSP node = *ref;
            if (node && node->parent()) node->setVisible(m_previewEnabled);
        }
    }
    void updateLayer(KisNodeWSP &ref, const QString &filterId, const QString &layerName, const QMap<QString, QVariant> &props, bool anyNonZero)
    {
        KisViewManager *view = m_win->viewManager();
        if (!view) return;
        KisImageSP image = view->image();
        if (!image) return;
        if (m_image.data() != image.data()) {      // another picture: forget the layers of the old one
            m_bcLayer = KisNodeWSP();
            m_hsvLayer = KisNodeWSP();
            m_vibLayer = KisNodeWSP();
            m_image = image;
        }
        KisNodeSP existing = ref;
        KisAdjustmentLayer *layer = (existing && existing->parent()) ? dynamic_cast<KisAdjustmentLayer*>(existing.data()) : nullptr;
        if (!layer) {
            if (!anyNonZero) return;
            KisFilterSP filter = KisFilterRegistry::instance()->value(filterId);
            if (!filter) return;
            KisFilterConfigurationSP config = filter->defaultConfiguration(KisGlobalResourcesInterface::instance());
            for (auto it = props.constBegin(); it != props.constEnd(); ++it) {
                config->setProperty(it.key(), it.value());
            }
            KisAdjustmentLayerSP newLayer = new KisAdjustmentLayer(image, layerName, config, KisSelectionSP());
            KisNodeSP active = view->activeNode();
            if (!m_activeBefore) m_activeBefore = active;   // Krimble 2026-10-30: finish() makes it the active layer again
            KisNodeSP parent = active ? active->parent() : KisNodeSP();
            if (!parent) parent = image->root();
            KisNodeCommandsAdapter adapter(view);
            adapter.addNode(newLayer, parent, active);
            ref = KisNodeSP(newLayer);
        } else {
            KisFilterConfigurationSP config = layer->filter()->clone();
            for (auto it = props.constBegin(); it != props.constEnd(); ++it) {
                config->setProperty(it.key(), it.value());
            }
            layer->setFilter(config);
            layer->setDirty();
        }
    }

    KisMainWindow *m_win;
    QLabel *m_labels[SliderCount];
    QSlider *m_sliders[SliderCount];
    QSpinBox *m_numbers[SliderCount];
    QAbstractButton *m_preview = nullptr;   // Krimble 2026-10-30: the Preview switch
    QLabel *m_previewLabel = nullptr;       // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust."
    QCheckBox *m_keepLayers = nullptr;     // Krimble 2026-10-30
    KisNodeWSP m_activeBefore;             // Krimble 2026-10-30: the layer that was active when the first adjustment layer was made
    bool m_previewEnabled = true;
    int m_values[SliderCount];
    QTimer m_timer;
    KisImageWSP m_image;
    KisNodeWSP m_bcLayer;
    KisNodeWSP m_hsvLayer;
    KisNodeWSP m_vibLayer;
};

// Krimble 2026-10-13 (11th): Quick Adjust as a dialog window (George: "Quick Adjust should be a dialog window, not a growth of some kind." and
// "Build Quick Adjust as a dialog window launched from Image Adjust."). One window, not tied to any other: six sliders, Reset, OK and Cancel.
// While it is open, the picture shows the change live (adjustment layers are made as the sliders move); Cancel takes those layers away, OK keeps
// them. The window sits at the bottom of the screen so the picture stays visible above it.
static void krimbleShowQuickAdjustDialog(KisMainWindow *win)
{
    KisViewManager *view = win->viewManager();
    if (!view || !view->image()) {
        QMessageBox::information(win, i18n("Quick Adjust"), i18n("Open a picture first."));
        return;
    }
    QDialog *dialog = new QDialog(win);
    // Krimble 2026-11-16 (728th): George (build 65): "OK does not close QuickAdjust. It's probably blocked by the sizing gadget." It was: the corner grip every plain dialog gets
    // (KrimbleResizeGrip, KisApplication.cpp) lies in the bottom right corner, on top of OK (the room that is normally added under the buttons for it is lost when this window sets
    // its own margins). QuickAdjust opens at its natural size and has no corner grip.
    // Krimble 2026-11-17 (731st): George: "You could just make the corner grip on QuickAdjust smaller and a little outside of the corner and invisible and scoot the button a little to
    // the left. Then both functions would work." QuickAdjust keeps a corner grip, small (16 units), drawn as nothing, in the very corner (a part of a window cannot lie outside it); OK
    // is moved 18 units to the left (the bottom row has that much room at its right end), so the grip and OK no longer overlap.
    dialog->setProperty("krimbleSmallGrip", true);
    // Krimble 2026-11-06 (2nd): George: "CAPTURE DARKER COLOR SCHEME FROM MOCK-UPS." The QuickAdjust window has the mockup's colors, sampled from the mockup picture: window
    // #181B2E, number boxes #121420 with a #5A5D80 outline, grey buttons #4A4B6A (top) to #26283D (bottom) with a #6B6C8C outline.
    // Krimble 2026-11-06 (3rd): George: "make QuickAdjust text smaller (not talking about the logo)". The text of the window (rows, Preview, Keep as layers, buttons) is 25% smaller,
    // white like the mockup's.
    {
        QFont smaller = dialog->font();
        // Krimble 2026-11-15 (item 12): George: "The QuickAdjust window is MAGNIFIED!!!! THAT SHOULD NEVER HAPPEN." The window's text is the program's own text size now (no 75%, and no sizes written in the style sheets).
        dialog->setFont(smaller);
    }
    dialog->setStyleSheet(QStringLiteral(
        "QLabel, QCheckBox { color: #ffffff; }"
        // "QDialog { background-color: #181b2e; }"
        // "QSpinBox { background-color: #121420; border: 1px solid #5a5d80; border-radius: 4px; }"
        // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The picture: window #0e0e21, number boxes #0c0c1e with a #51577b outline.
        "QDialog { background-color: #0e0e21; }"
        "QSpinBox { background-color: #0c0c1e; border: 1px solid #51577b; border-radius: 6px; }"
        // "QPushButton { color: #f1f4fc; border: 1px solid #6b6c8c; border-radius: 6px;"
        // Krimble 2026-11-06 (3rd): George: "long smooth subtle gradients, not shaping every item with shading": the buttons have only a hair of shading
        // "QPushButton { color: #f1f4fc; border: 1px solid #4d4f72; border-radius: 6px;"
        // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The picture's buttons: a #3c3971 to #323062 fill, a #635ab0 outline, bold, rounded 10 px.
        // Krimble 2026-11-15 (item 12), George: text 4x the rest of the screen is too big. Old line: (sizes 15/15/16/13/16 px before)
        "QPushButton { color: #f1f4fc; font-weight: bold; border: 1px solid #635ab0; border-radius: 10px; padding: 4px 10px;"
        " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #3c3971, stop:1 #323062); }"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Quick Adjust"));
    dialog->setModal(false);
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(4, 4, 4, 4);
    QWidget *content = new QWidget(dialog);
    KrimbleQuickAdjust *adjust = new KrimbleQuickAdjust(win, content);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    // Krimble 2026-10-14: George (1.0.36): "Quick Adjust is perfect size. Has no title bar?" The window has none on the phone, so its name is
    // written at the top of it.
    // Krimble 2026-10-30: George: "QuickAdjust does not need to say Krimble in it." The heading shows the tool name only.
    // Krimble 2026-10-30: George: "not drawn in code. I will provide a graphic." The logo drawn in code (KrimbleQuickAdjustLogo, switched off above with #if 0)
    // is not used; the plain heading is back until his graphic of QuickAdjust arrives.
    // Krimble 2026-11-05: George's QuickAdjust logo graphic (white on transparent), 80% of the window width, much larger than the text.
    {
        QHBoxLayout *logoRow = new QHBoxLayout;
        // Krimble 2026-11-05 (2nd): George: "Title logos ... are twice the size they should be and they are centered when they should be left aligned."
        // Left aligned, 40% of the window width (was centered, 80%).
        // Krimble 2026-11-06: George: "make QuickAdjust logo slightly larger." 40% of the window width became 50%.
        // Krimble 2026-11-06 (2nd): George: "I am thinking the Quick Adjust can use the graphics from the mock up as the title and purple graphics at the top". The header of the
        // mockup (the Krimble wordmark with the orange K and paw, "QuickAdjust", the purple light streak) is cut out of the mockup picture (969 x 106 px, about one to one on the
        // phone) and fills the width of the window.
        // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The header of the new picture (the wordmark, a divider, QuickAdjust, the purple streak).
        // Krimble 2026-11-15 (item 12): George: "It should be able to resize when the user wants it bigger, but not via magnification (stretching) of the contents." The banner stretched to the
        // width of the window (1057 real pixels wide at full width). It is 520 real pixels wide at most, at the left, whatever the size of the window.
        {
            const qreal bannerDpr = qMax<qreal>(1.0, dialog->devicePixelRatioF());
            logoRow->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-banner2.png"), QStringLiteral("Krimble QuickAdjust"), dialog, qRound(520 / bannerDpr)), 1);
            logoRow->addStretch(0);
        }
        layout->addLayout(logoRow);
    }
    layout->addWidget(content);
    // Krimble 2026-11-05: Reset at the left of Cancel and OK; OK is blue (the mockup's default button)
    QPushButton *resetButton = buttons->addButton(i18n("Reset"), QDialogButtonBox::ResetRole);
    QObject::connect(resetButton, &QPushButton::clicked, adjust, [adjust]() { adjust->resetAll(); });
    if (QPushButton *okButton = buttons->button(QDialogButtonBox::Ok)) {
        okButton->setStyleSheet(QStringLiteral(
            // Krimble 2026-11-06 (2nd): the mockup's OK button: #2986F0 at the top to #05429D at the bottom, a #3DA7FF outline (sampled from the mockup picture)
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2986f0, stop:1 #05429d); border: 1px solid #3da7ff; border-radius: 6px; }"));
    }
    // Krimble 2026-11-12 (task 5): George (his picture of QuickAdjust): "match this version of QuickAdjust." The bottom row of the picture: Reset at the left, then Preview with its ON switch, a thin divider, Cancel and OK (blue). The button box stays (hidden): its
    // accepted and rejected signals below are driven by the Cancel and OK buttons of this row.
    buttons->hide();
    {
        // Krimble 2026-11-15 (item 12): George (1.0.59, screenshots): "QuickAdjust should always be a small panel." Cause of the tall window with every button on its own line: the one row (Reset,
        // Preview, switch, divider, Cancel, OK) is about 450 px of layout wide at this text size and the phone is about 340 wide in portrait, so the window narrowing in KisApplication.cpp
        // (krimbleNarrowDialogLayouts: "every row of three or more controls becomes a column") turned it into a column. Now two short rows that fit (Preview and its switch; Reset, Cancel, OK), and both
        // rows are marked krimbleKeepRows so the narrowing leaves them alone.
        QVBoxLayout *bottomBox = new QVBoxLayout;
        bottomBox->setContentsMargins(0, 6, 0, 2);
        bottomBox->setSpacing(6);
        // Krimble 2026-11-15 (702nd): the Preview row is no longer used (Preview is in the bottom row since the 689th); this layout had no parent and was never freed.
        // Krimble 2026-11-15 (item 12), George: text 4x the rest of the screen is too big. Old line: (sizes 15/15/16/13/16 px before)
        if (QWidget *label = adjust->previewLabel()) { label->setParent(dialog); label->setStyleSheet(QStringLiteral("color: #ffffff; background: transparent; border: none;")); label->show();  }
        if (QAbstractButton *sw = adjust->previewSwitch()) { sw->setParent(dialog); sw->show(); }
        // Krimble 2026-11-15: George: "But put it in the space on the bottom." The Preview label and its switch are in the bottom row, in the space between Reset and Cancel (they were a row of their own above it).
        QHBoxLayout *bottom = new QHBoxLayout;
        bottom->setProperty("krimbleKeepRows", true);
        bottom->setSpacing(8);
        QPushButton *resetRow = new QPushButton(i18n("Reset"), dialog);
        QObject::connect(resetRow, &QPushButton::clicked, adjust, [adjust]() { adjust->resetAll(); });
        bottom->addWidget(resetRow);
        bottom->addStretch(1);
        if (QWidget *pl = adjust->previewLabel()) bottom->addWidget(pl);
        if (QAbstractButton *ps = adjust->previewSwitch()) bottom->addWidget(ps);
        bottom->addStretch(1);
        // The divider between Preview and Cancel is not used now that Preview has its own line.
        // QFrame *divider = new QFrame(dialog);
        // divider->setFixedSize(1, 36);
        // divider->setStyleSheet(QStringLiteral("background: #f0f1fa; border: none;"));
        // bottom->addWidget(divider);
        QPushButton *cancelRow = new QPushButton(i18n("Cancel"), dialog);
        QObject::connect(cancelRow, &QPushButton::clicked, buttons, &QDialogButtonBox::rejected);
        bottom->addWidget(cancelRow);
        QPushButton *okRow = new QPushButton(i18n("OK"), dialog);
        okRow->setDefault(true);
        // Krimble 2026-11-15 (item 12), George: text 4x the rest of the screen is too big. Old line: (sizes 15/15/16/13/16 px before)
        okRow->setStyleSheet(QStringLiteral("QPushButton { color: #ffffff; font-weight: bold; padding: 4px 10px; border: 1px solid #29c4ff; border-radius: 10px;"
                                            " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #006bfd, stop:1 #0033c5); }"));
        QObject::connect(okRow, &QPushButton::clicked, buttons, &QDialogButtonBox::accepted);
        bottom->addWidget(okRow);
        bottom->addSpacing(18);   // Krimble 2026-11-17 (731st): room for the small corner grip at the right of OK
        bottomBox->addLayout(bottom);
        layout->addLayout(bottomBox);
    }
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [adjust, dialog]() {
        adjust->commitVisible();
        adjust->finish();     // Krimble 2026-10-30: merge down unless "Keep as layers"; the earlier layer is active again
        dialog->accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, [adjust, dialog]() {
        adjust->removeCreatedLayers();
        dialog->reject();
    });
    // closing the window another way (Android's back button) counts as Cancel
    QObject::connect(dialog, &QDialog::rejected, dialog, [adjust]() { adjust->removeCreatedLayers(); });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        // Krimble 2026-11-15 (item 12): George: "It should start out small because otherwise it gets in the way." The window opens at the size its contents ask for (at most 94% of the screen
        // width), and the user can make it bigger.
        // Krimble 2026-11-15 (item 12): George (build 60 screenshot): "QuickAdjust is still HUGE." It opens as narrow as its contents allow, at most 72% of the screen width (the contents' wish was 80%); it is only the
        // START size: the user can drag it wider. (No limit on resizing.)
        const int width = qMax(dialog->minimumSizeHint().width(), qMin(dialog->sizeHint().width(), int(avail.width() * 0.72)));
        // Krimble 2026-11-06: the exact height for this width (the logo's height depends on its width, which sizeHint() does not know), no 20 px extra
        const int contentHeight = dialog->layout()->hasHeightForWidth() ? dialog->layout()->heightForWidth(width) : dialog->sizeHint().height();
        const int height = qMin(contentHeight, int(avail.height() * 0.90));
        dialog->resize(width, height);
        dialog->move(avail.left() + (avail.width() - width)/2,
                     avail.bottom() - height - 8);
    }
    dialog->show();
}

// Opens (or shows and hides) the Quick Adjust panel. It starts floating, 62% of the screen wide, so it never has to fit next to the
// toolbox, canvas and panels.
static void krimbleToggleQuickAdjust(KisMainWindow *win)
{
    QDockWidget *dock = win->findChild<QDockWidget*>(QStringLiteral("KrimbleQuickAdjust"));
    if (dock) {
        dock->setVisible(!dock->isVisible());
        if (dock->isVisible()) dock->raise();
        return;
    }
    dock = new QDockWidget(i18n("Quick Adjust"), win);
    dock->setObjectName(QStringLiteral("KrimbleQuickAdjust"));
    QWidget *content = new QWidget(dock);
    new KrimbleQuickAdjust(win, content);
    dock->setWidget(content);
    win->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::NoDockWidgetArea);   // Krimble 2026-10-12: Quick Adjust is a floating tool; dragging it never docks it
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dock->resize(int(avail.width() * 0.62), content->sizeHint().height() + 40);
        dock->move(avail.left() + int(avail.width() * 0.19), avail.top() + int(avail.height() * 0.30));
    }
    dock->show();
    dock->raise();
}

