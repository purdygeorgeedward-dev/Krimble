
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
    case 0: {
        const qreal core = r.width() * 0.22;
        p.drawEllipse(c, core, core);
        for (int k = 0; k < 8; ++k) {
            const qreal a = k * M_PI / 4.0;
            const QPointF dir(std::cos(a), std::sin(a));
            p.drawLine(c + dir * (core + 3), c + dir * (r.width() * 0.46));
        }
        break; }
    case 1:
        p.drawEllipse(r);
        p.setBrush(ink);
        p.drawPie(r, 90 * 16, 180 * 16);
        break;
    case 2: {
        const qreal d = r.width() * 0.58;
        p.setPen(Qt::NoPen);
        const QColor cols[3] = { QColor(40, 190, 255, 215), QColor(255, 90, 170, 215), QColor(255, 200, 40, 215) };
        const QPointF pts[3] = { QPointF(r.left() + d / 2, r.top() + d / 2), QPointF(r.right() - d / 2, r.top() + d / 2), QPointF(c.x(), r.bottom() - d / 2) };
        for (int k = 0; k < 3; ++k) { p.setBrush(cols[k]); p.drawEllipse(pts[k], d / 2, d / 2); }
        break; }
    case 3: {
        QConicalGradient g(c, 0);
        for (int k = 0; k <= 6; ++k) g.setColorAt(k / 6.0, QColor::fromHsv((k * 60) % 360, 255, 255));
        const qreal w = r.width() * 0.22;
        p.setPen(QPen(QBrush(g), w));
        p.drawEllipse(r.adjusted(w / 2, w / 2, -w / 2, -w / 2));
        break; }
    case 4: {
        const QRectF sq = r.adjusted(2, 2, -2, -2);
        p.drawRoundedRect(sq, 3, 3);
        p.setBrush(ink);
        p.drawRect(QRectF(sq.left(), sq.top(), sq.width() / 2, sq.height()));
        break; }
    case 5: {
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

class KrimbleToggleSwitch : public QAbstractButton
{
public:
    explicit KrimbleToggleSwitch(QWidget *parent = nullptr) : QAbstractButton(parent)
    {
        setCheckable(true);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    QSize sizeHint() const override { return QSize(44, 20); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(rect()).adjusted(3, 3, -3, -3);
        const qreal radius = r.height() / 2.0;
        if (isChecked()) {
            p.setBrush(Qt::NoBrush);
            for (int i = 4; i >= 1; --i) {
                p.setPen(QPen(QColor(56, 198, 255, 20 * (5 - i)), i * 2.0));
                p.drawRoundedRect(r, radius, radius);
            }
        }
        QLinearGradient bg(r.topLeft(), r.bottomLeft());
        if (isChecked()) { bg.setColorAt(0, QColor(0x00, 0x50, 0xb4)); bg.setColorAt(0.35, QColor(0x00, 0x69, 0xd8)); bg.setColorAt(1, QColor(0x02, 0x6a, 0xe5)); }
        else             { bg.setColorAt(0, QColor(0x4a, 0x52, 0x76)); bg.setColorAt(1, QColor(0x2a, 0x30, 0x48)); }
        p.setPen(QPen(isChecked() ? QColor(0x54, 0xe5, 0xff) : QColor(0x6b, 0x74, 0x98), 1.5));
        p.setBrush(bg);
        p.drawRoundedRect(r, radius, radius);
        const qreal knob = r.height() - 6;
        const qreal x = isChecked() ? r.right() - knob - 3 : r.left() + 3;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0xea, 0xed, 0xf6));
        p.drawEllipse(QRectF(x, r.top() + 3, knob, knob));
        p.setPen(QColor(0xf4, 0xf6, 0xff));
        QFont f = font(); f.setBold(true); f.setPointSizeF(qMax<qreal>(7.0, f.pointSizeF() * 0.8)); p.setFont(f);
        const QRectF textRect = isChecked() ? QRectF(r.left() + 6, r.top(), r.width() - knob - 14, r.height())
                                            : QRectF(r.left() + knob + 10, r.top(), r.width() - knob - 14, r.height());
        p.drawText(textRect, Qt::AlignCenter, isChecked() ? QStringLiteral("ON") : QStringLiteral("OFF"));
    }
};

#if 0
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
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(56, 198, 255, 80), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(quickPath);
        p.setPen(QPen(QColor(253, 96, 1, 90), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(adjustPath);
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
#endif

class KrimbleLogoLabel : public QWidget
{
public:
    explicit KrimbleLogoLabel(const QString &resource, const QString &name, QWidget *parent = nullptr, int maxLogicalWidth = 340) : QWidget(parent), m_pix(resource)
    {
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
        setAccessibleName(name);
        setMaximumWidth(maxLogicalWidth);
    }
    bool hasHeightForWidth() const override { return !m_pix.isNull(); }
    int heightForWidth(int w) const override { return m_pix.isNull() ? 0 : int(double(w) * m_pix.height() / m_pix.width()); }
    QSize sizeHint() const override { return QSize(200, heightForWidth(200)); }
protected:
    void resizeEvent(QResizeEvent *) override
    {
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

class KrimbleKHolder : public QWidget
{
public:
    explicit KrimbleKHolder(QWidget *parent) : QWidget(parent)
    {
        setObjectName(QStringLiteral("krimbleKHolder"));
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setFixedSize(22, 22);
    }
    QSize sizeHint() const override { return QSize(qMax(16, minimumWidth()), 16); }
};

class KrimbleMenuBarFilter : public QObject
{
public:
    explicit KrimbleMenuBarFilter(QObject *parent) : QObject(parent) {}

    static void adopt(QWidget *label, QMenuBar *bar)
    {
        if (!label || label->property("krimbleKAdopted").toBool()) return;
        label->setProperty("krimbleKAdopted", true);
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        label->setFixedSize(22, 22);
        if (bar) {
            bar->updateGeometry();
        }
    }

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

    static void ensureSpacer(QMenuBar *bar)
    {
        if (!bar) return;
        QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
        if (!spacer) {
            spacer = new QWidgetAction(bar);
            spacer->setObjectName(QStringLiteral("krimbleKSpacer"));
            QWidget *gap = new QWidget(bar);
            gap->setFixedSize(4, 22);
            gap->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            spacer->setDefaultWidget(gap);
        }
        const QList<QAction *> all = bar->actions();
        if (!all.isEmpty() && all.first() == spacer) return;
        if (all.contains(spacer)) bar->removeAction(spacer);
        const QList<QAction *> rest = bar->actions();
        if (rest.isEmpty()) bar->addAction(spacer); else bar->insertAction(rest.first(), spacer);
    }

    static void placeK(QMenuBar *bar)
    {
        if (!bar) return;
        const int krimbleBarExtra = 3;
        if (bar->minimumHeight() != 0) bar->setMinimumHeight(0);
        if (bar->property("krimbleBarExtra").toInt() != krimbleBarExtra) bar->setProperty("krimbleBarExtra", krimbleBarExtra);
        if (QWidget *corner = bar->cornerWidget(Qt::TopLeftCorner)) {
            if (corner->isVisible()) corner->hide();
        }
        if (QWidget *buttons = bar->cornerWidget(Qt::TopRightCorner)) {
            if (buttons->isVisible() && QByteArray(buttons->metaObject()->className()).endsWith("ControllerWidget")) {
                const int y = qMax(0, (bar->height() - krimbleBarExtra - buttons->height()) / 2);
                if (buttons->y() != y) buttons->move(buttons->x(), y);
            }
        }
        if (QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"))) {
            spacer->setObjectName(QString());
            bar->removeAction(spacer);
            spacer->deleteLater();
        }
        const int krimbleKSize = qMin(bar->height() - 4, qMax(14, QFontMetrics(bar->font()).height()));
        QAction *first = nullptr;
        Q_FOREACH (QAction *entry, bar->actions()) {
            if (!entry->isVisible() || !entry->menu()) continue;
            first = entry;
            break;
        }
        if (!first || !bar->isVisible()) return;
        const QRect item = bar->actionGeometry(first);
        if (item.width() <= 0) return;
        QString title = first->text();
        title.remove(QLatin1Char('&'));
        const int textInset = qMax(0, (item.width() - QFontMetrics(bar->font()).horizontalAdvance(title)) / 2);
        const int textStart = 2 + krimbleKSize + qRound(70.0 / qMax<qreal>(1.0, bar->devicePixelRatioF()));
        const int cur = bar->property("krimblePadLeft").toInt();
        const int want = qBound(0, cur + (textStart - textInset) - item.left(), 200);
        if (want != cur) {
            bar->setProperty("krimblePadLeft", want);
            bar->setStyleSheet(QStringLiteral("QMenuBar { padding-left: %1px; }").arg(want));
        }
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
                else if (QByteArray(child->metaObject()->className()).endsWith("ControllerWidget")) {
                    child->installEventFilter(this);
                    schedulePlaceK(qobject_cast<QMenuBar *>(watched));
                }
            }
        } else if ((event->type() == QEvent::LayoutRequest || event->type() == QEvent::Resize || event->type() == QEvent::Show) && qobject_cast<QMenuBar *>(watched)) {
            schedulePlaceK(qobject_cast<QMenuBar *>(watched));
        } else if ((event->type() == QEvent::Move || event->type() == QEvent::Show) && qobject_cast<QWidget *>(watched)
                   && (QByteArray(watched->metaObject()->className()).endsWith("ControlLabel") || QByteArray(watched->metaObject()->className()).endsWith("ControllerWidget"))) {
            schedulePlaceK(qobject_cast<QMenuBar *>(static_cast<QWidget *>(watched)->parentWidget()));
        } else if ((event->type() == QEvent::ActionAdded || event->type() == QEvent::ActionRemoved) && qobject_cast<QMenuBar *>(watched)) {
            QMenuBar *bar = qobject_cast<QMenuBar *>(watched);
            QTimer::singleShot(0, bar, [bar]() { KrimbleMenuBarFilter::hideSettingsMenu(bar); });
            schedulePlaceK(bar);
            QWidgetAction *spacer = bar->findChild<QWidgetAction *>(QStringLiteral("krimbleKSpacer"));
            const QList<QAction *> all = bar->actions();
            if (spacer && (all.isEmpty() || all.first() != spacer))
                QTimer::singleShot(0, bar, [bar]() { KrimbleMenuBarFilter::ensureSpacer(bar); });
        } else if (event->type() == QEvent::Paint && qobject_cast<QMenuBar *>(watched)) {
            auto drawK = [watched]() {
                QWidget *kBar = static_cast<QWidget *>(watched);
                const QFontMetrics kMetrics(kBar->font());
                const int kSize = qMin(kBar->height() - 4, qMax(14, kMetrics.height()));
                QPainter kPainter(kBar);
                kPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                qApp->windowIcon().paint(&kPainter, QRect(2, (kBar->height() - kBar->property("krimbleBarExtra").toInt() - kSize) / 2, kSize, kSize));
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
            painter.end();
            drawK();
            return false;
        } else if (event->type() == QEvent::Paint) {
            QWidget *label = qobject_cast<QWidget *>(watched);
            if (label && QByteArray(label->metaObject()->className()).endsWith("ControlLabel")) {
                return true;
            }
        }
        return false;
    }
};

static bool g_krimbleQuickKeepLayers = false;

class KrimbleQuickAdjust : public QObject
{
public:
    KrimbleQuickAdjust(KisMainWindow *win, QWidget *content) : QObject(content), m_win(win)
    {
        QVBoxLayout *layout = new QVBoxLayout(content);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(5);
        QHBoxLayout *options = new QHBoxLayout;
        options->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-sliders.png"), i18n("Adjustments"), content, 34));
        options->addStretch();
        m_previewLabel = new QLabel(i18n("Preview"), content);
        options->addWidget(m_previewLabel);
        m_preview = new KrimbleToggleSwitch(content);
        m_preview->setChecked(true);
        options->addWidget(m_preview);
        {
            QFrame *topPanel = new QFrame(content);
            topPanel->setObjectName(QStringLiteral("krimbleQuickTop"));
            topPanel->setStyleSheet(QStringLiteral("QFrame#krimbleQuickTop { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #313251, stop:1 #2d314e); border: 1px solid #343458; border-radius: 6px; }"
                                                   "QFrame#krimbleQuickTop QLabel { background: transparent; border: none; }"));
            options->setContentsMargins(4, 1, 4, 1);
            topPanel->setLayout(options);
            topPanel->hide();
        }
        m_keepLayers = new QCheckBox(i18n("Keep as layers"), content);
        m_keepLayers->setChecked(g_krimbleQuickKeepLayers);
        connect(m_keepLayers, &QCheckBox::toggled, this, [](bool on) { g_krimbleQuickKeepLayers = on; });
        int nameWidth = 0;
        for (int i = 0; i < SliderCount; ++i) nameWidth = qMax(nameWidth, content->fontMetrics().horizontalAdvance(i18n(def(i).name)));
        for (int i = 0; i < SliderCount; ++i) {
            QFrame *frame = new QFrame(content);
            frame->setObjectName(QStringLiteral("krimbleQuickRow"));
            frame->setStyleSheet(QStringLiteral("QFrame#krimbleQuickRow { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #282a41, stop:1 #1f2235); border: 1px solid #404565; border-radius: 6px; }"));
            frame->setStyleSheet(QStringLiteral("QFrame#krimbleQuickRow { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #202141, stop:1 #13142b); border: 1px solid #6058aa; border-radius: 10px; }"));
            QGridLayout *row = new QGridLayout(frame);
            row->setContentsMargins(4, 2, 6, 2);
            row->setHorizontalSpacing(6);
            row->setVerticalSpacing(0);
            QLabel *rowIcon = new QLabel(content);
            {
                const qreal ratio = content->devicePixelRatioF();
                QPixmap big(QStringLiteral(":/dark_krimble-quickadjust-icon-%1.png").arg(i));
                QPixmap sharp = big.scaled(qRound(20 * ratio), qRound(20 * ratio), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                sharp.setDevicePixelRatio(ratio);
                rowIcon->setPixmap(sharp);
                rowIcon->setStyleSheet(QStringLiteral("background: transparent; border: none;"));
            }
            rowIcon->setFixedSize(22, 22);
            row->addWidget(rowIcon, 0, 0, Qt::AlignCenter);
            m_labels[i] = new QLabel(i18n(def(i).name), content);
            m_labels[i]->setStyleSheet(QStringLiteral("font-weight: bold; color: #ffffff; background: transparent; border: none;"));
            row->addWidget(m_labels[i], 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
            m_numbers[i] = new QSpinBox(content);
            m_numbers[i]->setRange(def(i).min, def(i).max);
            m_numbers[i]->setFixedSize(74, 20);
            m_numbers[i]->setAlignment(Qt::AlignCenter);
            m_numbers[i]->setStyleSheet(QStringLiteral(
                "QSpinBox { font-weight: bold; color: #ffffff; padding-right: 17px; padding-left: 0px; }"
                "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 16px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 16px; height: 10px; border: none; border-left: 1px solid #323252; background: transparent; }"
                "QSpinBox::up-arrow { image: url(:/dark_krimble-quickadjust-arrow-up.png); width: 9px; height: 7px; }"
                "QSpinBox::down-arrow { image: url(:/dark_krimble-quickadjust-arrow-down.png); width: 9px; height: 7px; }"));
            row->addWidget(m_numbers[i], 0, 3, Qt::AlignRight | Qt::AlignVCenter);
            layout->addWidget(frame);
            m_sliders[i] = new QSlider(Qt::Horizontal, content);
            m_sliders[i]->setRange(def(i).min, def(i).max);
            m_sliders[i]->setMinimumWidth(1);
            m_sliders[i]->setMinimumHeight(18);
            m_sliders[i]->setObjectName(QStringLiteral("krimbleQuickAdjustSlider"));
            const QString thinHandle = QStringLiteral(
                "QSlider::handle:horizontal { width: 12px; height: 12px; margin: -5px 0; border: none; background: transparent; border-image: url(:/krimble-ball-bearing-small.png) 0 0 0 0 stretch stretch; }");
            if (i != Hue) {
                m_sliders[i]->setStyleSheet(QStringLiteral(
                    "QSlider::groove:horizontal { height: 3px; border: none; border-radius: 1px;"
                    " background: #f4f5fb; }"
                    "QSlider::sub-page:horizontal { height: 3px; border: none; border-radius: 1px;"
                    " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #00f4ff, stop:1 #00c6f3); }")
                    + thinHandle);
            }
            if (i == Hue) {
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
                if (false) m_sliders[i]->setStyleSheet(QStringLiteral(
                    "QSlider::groove:horizontal { height: 3px; border: none; border-radius: 1px;"
                    " background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #ff00c8, stop:0.2 #4a3cff, stop:0.4 #00c8ff,"
                    " stop:0.6 #1fdc3f, stop:0.8 #ffe600, stop:1 #ff2a00); }"
                    "QSlider::sub-page:horizontal { background: transparent; border: none; }") + thinHandle);
            }
            row->addWidget(m_sliders[i], 0, 2);
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
        layout->addWidget(m_keepLayers);
        m_timer.setSingleShot(true);
        connect(&m_timer, &QTimer::timeout, this, [this]() { apply(); });
        for (int i = 0; i < SliderCount; ++i) m_values[i] = 0;
        connect(m_preview, &QAbstractButton::toggled, this, [this](bool enabled) {
            m_previewEnabled = enabled;
            flush();
        });
    }

    QWidget *previewLabel() const { return m_previewLabel; }
    QAbstractButton *previewSwitch() const { return m_preview; }

    void resetAll()
    {
        for (int i = 0; i < SliderCount; ++i) m_sliders[i]->setValue(0);
    }

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
        if (m_image.data() != image.data()) {
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
            if (!m_activeBefore) m_activeBefore = active;
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
    QAbstractButton *m_preview = nullptr;
    QLabel *m_previewLabel = nullptr;
    QCheckBox *m_keepLayers = nullptr;
    KisNodeWSP m_activeBefore;
    bool m_previewEnabled = true;
    int m_values[SliderCount];
    QTimer m_timer;
    KisImageWSP m_image;
    KisNodeWSP m_bcLayer;
    KisNodeWSP m_hsvLayer;
    KisNodeWSP m_vibLayer;
};

static void krimbleShowQuickAdjustDialog(KisMainWindow *win)
{
    KisViewManager *view = win->viewManager();
    if (!view || !view->image()) {
        QMessageBox::information(win, i18n("Quick Adjust"), i18n("Open a picture first."));
        return;
    }
    QDialog *dialog = new QDialog(win);
    dialog->setProperty("krimbleSmallGrip", true);
    {
        QFont smaller = dialog->font();
        dialog->setFont(smaller);
    }
    dialog->setStyleSheet(QStringLiteral(
        "QLabel, QCheckBox { color: #ffffff; }"
        "QDialog { background-color: #0e0e21; }"
        "QSpinBox { background-color: #0c0c1e; border: 1px solid #51577b; border-radius: 6px; }"
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
    {
        QHBoxLayout *logoRow = new QHBoxLayout;
        {
            const qreal bannerDpr = qMax<qreal>(1.0, dialog->devicePixelRatioF());
            logoRow->addWidget(new KrimbleLogoLabel(QStringLiteral(":/dark_krimble-quickadjust-logo.png"), QStringLiteral("QuickAdjust"), dialog, qRound(360 / bannerDpr)), 1);
            logoRow->addStretch(0);
        }
        layout->addLayout(logoRow);
    }
    layout->addWidget(content);
    QPushButton *resetButton = buttons->addButton(i18n("Reset"), QDialogButtonBox::ResetRole);
    QObject::connect(resetButton, &QPushButton::clicked, adjust, [adjust]() { adjust->resetAll(); });
    if (QPushButton *okButton = buttons->button(QDialogButtonBox::Ok)) {
        okButton->setStyleSheet(QStringLiteral(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2986f0, stop:1 #05429d); border: 1px solid #3da7ff; border-radius: 6px; }"));
    }
    buttons->hide();
    {
        QVBoxLayout *bottomBox = new QVBoxLayout;
        bottomBox->setContentsMargins(0, 6, 0, 2);
        bottomBox->setSpacing(6);
        if (QWidget *label = adjust->previewLabel()) { label->setParent(dialog); label->setStyleSheet(QStringLiteral("color: #ffffff; background: transparent; border: none;")); label->show();  }
        if (QAbstractButton *sw = adjust->previewSwitch()) { sw->setParent(dialog); sw->show(); }
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
        QPushButton *cancelRow = new QPushButton(i18n("Cancel"), dialog);
        QObject::connect(cancelRow, &QPushButton::clicked, buttons, &QDialogButtonBox::rejected);
        bottom->addWidget(cancelRow);
        QPushButton *okRow = new QPushButton(i18n("OK"), dialog);
        okRow->setDefault(true);
        okRow->setStyleSheet(QStringLiteral("QPushButton { color: #ffffff; font-weight: bold; padding: 4px 10px; border: 1px solid #29c4ff; border-radius: 10px;"
                                            " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #006bfd, stop:1 #0033c5); }"));
        QObject::connect(okRow, &QPushButton::clicked, buttons, &QDialogButtonBox::accepted);
        bottom->addWidget(okRow);
        bottom->addSpacing(18);
        bottomBox->addLayout(bottom);
        layout->addLayout(bottomBox);
    }
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [adjust, dialog]() {
        adjust->commitVisible();
        adjust->finish();
        dialog->accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, [adjust, dialog]() {
        adjust->removeCreatedLayers();
        dialog->reject();
    });
    QObject::connect(dialog, &QDialog::rejected, dialog, [adjust]() { adjust->removeCreatedLayers(); });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        const int width = qMax(dialog->minimumSizeHint().width(), qMin(dialog->sizeHint().width(), int(avail.width() * 0.72)));
        const int contentHeight = dialog->layout()->hasHeightForWidth() ? dialog->layout()->heightForWidth(width) : dialog->sizeHint().height();
        const int height = qMin(contentHeight, int(avail.height() * 0.90));
        dialog->resize(width, height);
        dialog->move(avail.left() + (avail.width() - width)/2,
                     avail.bottom() - height - 8);
    }
    dialog->show();
}

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
    dock->setAllowedAreas(Qt::NoDockWidgetArea);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dock->resize(int(avail.width() * 0.62), content->sizeHint().height() + 40);
        dock->move(avail.left() + int(avail.width() * 0.19), avail.top() + int(avail.height() * 0.30));
    }
    dock->show();
    dock->raise();
}

