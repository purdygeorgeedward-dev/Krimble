// Krimble 2026-11-20 (760th): part of KisApplication.cpp, moved here unchanged and included at the same place (George: "Fix all of that before any build can take place.").
// It only compiles as part of KisApplication.cpp.

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-04: number fields on the phone (George: "Every time I tap a numeric value ... I get Cut Copy Paste
// blocking my view. The little up/down selector is too small to tap. I want to be able to long press, then drag up
// and down to change a numeric value", and "Make the up down arrows decorative but actually do the drag").
// For every number field (all QSpinBox / QDoubleSpinBox, including the bar-style ones such as "Opacity"):
//  - A short TAP opens Krimble's own number pad (big buttons). Android's text selection, its handles and its
//    Cut/Copy/Paste bar are never involved, and what is typed replaces the value.
//  - LONG PRESS (about a third of a second) and then DRAG UP or DOWN changes the value; a little tip shows the value.
//    Lift the finger to stop. Dragging up raises the value, dragging down lowers it.
//  - The tiny up/down arrows stay in place as decoration only; touching them does the same as touching the field.
// The settings are the constants in KrimbleNumberFields. Other platforms are not touched.
class KrimbleNumberPad : public QDialog
{
public:
    KrimbleNumberPad(QAbstractSpinBox *spin, std::function<void(double)> apply)
        : QDialog(spin->window()), m_spin(spin), m_apply(apply)
    {
        setWindowTitle(i18n("Enter a number"));
        setProperty("krimbleNumberPad", true);     // (no Q_OBJECT in this file, so a property marks the pad)
        setModal(true);
        QVBoxLayout *layout = new QVBoxLayout(this);
        QString range;
        double current = 0.0;
        if (QSpinBox *si = qobject_cast<QSpinBox*>(spin)) {
            current = si->value();
            m_min = si->minimum(); m_max = si->maximum(); m_decimals = 0;
            m_prefix = si->prefix(); m_suffix = si->suffix();
        } else if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(spin)) {
            current = sd->value();
            m_min = sd->minimum(); m_max = sd->maximum(); m_decimals = sd->decimals();
            m_prefix = sd->prefix(); m_suffix = sd->suffix();
        }
        QLabel *info = new QLabel(i18n("Now %1   (%2 to %3)", QString::number(current, 'f', m_decimals),
                                       QString::number(m_min, 'f', m_decimals), QString::number(m_max, 'f', m_decimals)));
        info->setAlignment(Qt::AlignCenter);
        layout->addWidget(info);
        m_display = new QLabel(QString());
        QFont f = m_display->font(); f.setPointSizeF(f.pointSizeF() * 2.2); m_display->setFont(f);
        m_display->setAlignment(Qt::AlignCenter);
        m_display->setMinimumHeight(70);
        m_display->setFrameShape(QFrame::StyledPanel);
        layout->addWidget(m_display);

        QGridLayout *grid = new QGridLayout();
        const char *keys[4][4] = {{"7","8","9","<"}, {"4","5","6","-"}, {"1","2","3","."}, {"X","0","C","OK"}};
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                const QString key = QString::fromLatin1(keys[r][c]);
                QString label = key;
                if (key == "<") label = QString::fromUtf8("\u232B");
                if (key == "X") label = i18n("Cancel");
                if (key == "C") label = i18n("Clear");
                QPushButton *b = new QPushButton(label);
                b->setMinimumSize(110, 80);
                QFont bf = b->font(); bf.setPointSizeF(bf.pointSizeF() * 1.6); b->setFont(bf);
                b->setFocusPolicy(Qt::NoFocus);
                connect(b, &QPushButton::clicked, this, [this, key]() { press(key); });
                grid->addWidget(b, r, c);
            }
        }
        layout->addLayout(grid);
    }

private:
    void press(const QString &key)
    {
        if (key == "X") { reject(); return; }
        if (key == "OK") {
            bool ok = false;
            double v = m_text.toDouble(&ok);
            if (ok) {
                v = qBound(m_min, v, m_max);
                m_apply(v);
            }
            accept();
            return;
        }
        if (key == "C") { m_text.clear(); }
        else if (key == "<") { m_text.chop(1); }
        else if (key == "-") { if (m_text.startsWith('-')) m_text.remove(0, 1); else m_text.prepend('-'); }
        else if (key == ".") { if (m_decimals > 0 && !m_text.contains('.')) m_text += (m_text.isEmpty() || m_text == "-") ? "0." : "."; }
        else { m_text += key; }
        m_display->setText(m_prefix + m_text + (m_text.isEmpty() ? QString() : m_suffix));
    }

    QAbstractSpinBox *m_spin;
    std::function<void(double)> m_apply;
    QLabel *m_display {nullptr};
    QString m_text, m_prefix, m_suffix;
    double m_min {0.0}, m_max {0.0};
    int m_decimals {0};
};

class KrimbleNumberFields : public QObject
{
public:
    static const int LongPressMs = 350;          // how long a finger must stay down before dragging changes the value
    static const int MovePixels = 9;             // logical pixels of drag for one step
    static const int TapSlop = 12;               // movement allowed during a tap or long press

    explicit KrimbleNumberFields(QObject *parent) : QObject(parent)
    {
        m_timer.setSingleShot(true);
        m_timer.setInterval(LongPressMs);
        connect(&m_timer, &QTimer::timeout, this, [this]() {
            if (m_state == Pending && m_spin) {
                m_state = Scrubbing;
                tip();
            }
        });
    }

    // Used by the number pad and the drag: changes the value the way each kind of field wants it.
    static void setSpinValue(QAbstractSpinBox *spin, double v)
    {
#ifndef KRIMBLE_HARNESS
        if (KisSliderSpinBox *ks = qobject_cast<KisSliderSpinBox*>(spin)) { ks->setValue(qRound(v)); return; }
        if (KisDoubleSliderSpinBox *kd = qobject_cast<KisDoubleSliderSpinBox*>(spin)) { kd->setValue(v); return; }
#endif
        if (QSpinBox *si = qobject_cast<QSpinBox*>(spin)) { si->setValue(qRound(v)); return; }
        if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(spin)) { sd->setValue(v); return; }
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
            break;
        default:
            return false;
        }
        if (m_replaying) {
            return false;       // KRIMBLE 2026-10-07: our own replay of a short tap
        }
        QAbstractSpinBox *spin = spinFor(watched);
        if (!spin && m_state == Idle) {
            return false;
        }
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
            if (!spin || !spin->isEnabled() || spin->isReadOnly() || mouse->button() != Qt::LeftButton
                    || spin->window()->property("krimbleNumberPad").toBool()) {
                return false;
            }
            m_spin = spin;
            m_state = Pending;
            m_pressPos = mouse->globalPos();
            m_pressWidget = qobject_cast<QWidget*>(watched);      // KRIMBLE 2026-10-07: to hand a short tap back to the field
            m_pressLocal = mouse->pos();
            m_startValue = valueOf(spin);
            m_timer.start();
            return true;
        }
        if (m_state == Idle || !m_spin) {
            return false;
        }
        if (event->type() == QEvent::MouseMove) {
            const QPoint d = mouse->globalPos() - m_pressPos;
            if (m_state == Pending && (qAbs(d.x()) > TapSlop || qAbs(d.y()) > TapSlop)) {
                // moved before the long press: not a tap and not a drag, so nothing happens
                m_timer.stop();
                // Krimble 2026-11-11: George: "Give me the finger drag." / "percentage selectors should be able to drag left and right with touch. Like opacity in layers panel." A plain number
                // field (QSpinBox, QDoubleSpinBox) that is moved mostly sideways before the long press is dragged: its value follows the finger. Anything else is cancelled, as before.
                m_state = sidewaysDrag(d) ? Dragging : Cancelled;
            } else if (m_state == Scrubbing) {
                const int steps = qRound(double(-d.y()) / MovePixels);
                const double step = stepOf(m_spin);
                setSpinValue(m_spin, qBound(minOf(m_spin), m_startValue + steps * step * multiplierOf(m_spin), maxOf(m_spin)));
                tip(mouse->globalPos());
            }
            if (m_state == Dragging) {
                dragValue(d.x());
                tip(mouse->globalPos());
            }
            return true;
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            m_timer.stop();
            const State state = m_state;
            QPointer<QAbstractSpinBox> spin2 = m_spin;
            QPointer<QWidget> pressWidget = m_pressWidget;
            const QPoint pressLocal = m_pressLocal;
            const QPoint pressGlobal = m_pressPos;
            m_state = Idle;
            m_spin = nullptr;
            QToolTip::hideText();
            if (state == Dragging) QGuiApplication::inputMethod()->hide();   // Krimble 2026-11-11: no on-screen keyboard after a drag
            if (state == Pending && spin2 && pressWidget) {
                // KRIMBLE 2026-10-07: a short tap no longer opens Krimble's number pad (George: "GIGANTIC NUMERIC
                // KEYPAD APPEARS!! DO NOT WANT!!"). The tap is handed back to the field, as if it had not been
                // caught: the press and the release are sent to it again. Old behaviour, kept as a comment:
                //     if (!spin2) { return; }
                //     KrimbleNumberPad *pad = new KrimbleNumberPad(spin2, [spin2](double v) {
                //         if (spin2) { KrimbleNumberFields::setSpinValue(spin2, v); }
                //     });
                //     pad->setAttribute(Qt::WA_DeleteOnClose);
                //     pad->open();
                QTimer::singleShot(0, this, [this, pressWidget, pressLocal, pressGlobal]() {
                    if (!pressWidget) {
                        return;
                    }
                    m_replaying = true;
                    QMouseEvent press(QEvent::MouseButtonPress, pressLocal, pressGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
                    QCoreApplication::sendEvent(pressWidget, &press);
                    QMouseEvent release(QEvent::MouseButtonRelease, pressLocal, pressGlobal, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
                    QCoreApplication::sendEvent(pressWidget, &release);
                    m_replaying = false;
                });
            }
            return true;
        }
        return false;
    }

private:
    // enum State { Idle, Pending, Scrubbing, Cancelled };
    enum State { Idle, Pending, Scrubbing, Cancelled, Dragging };   // Krimble 2026-11-11: Dragging = a finger dragged sideways on a plain number field

    static QAbstractSpinBox *spinFor(QObject *o)
    {
        QWidget *w = qobject_cast<QWidget*>(o);
        for (int depth = 0; w && depth < 3; ++depth, w = w->parentWidget()) {
            if (QAbstractSpinBox *s = qobject_cast<QAbstractSpinBox*>(w)) {
                return s;
            }
        }
        return nullptr;
    }
    static double valueOf(QAbstractSpinBox *s)
    {
        if (QSpinBox *si = qobject_cast<QSpinBox*>(s)) return si->value();
        if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(s)) return sd->value();
        return 0.0;
    }
    static double minOf(QAbstractSpinBox *s)
    {
        if (QSpinBox *si = qobject_cast<QSpinBox*>(s)) return si->minimum();
        if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(s)) return sd->minimum();
        return 0.0;
    }
    static double maxOf(QAbstractSpinBox *s)
    {
        if (QSpinBox *si = qobject_cast<QSpinBox*>(s)) return si->maximum();
        if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(s)) return sd->maximum();
        return 0.0;
    }
    static double stepOf(QAbstractSpinBox *s)
    {
        double step = 1.0;
        if (QSpinBox *si = qobject_cast<QSpinBox*>(s)) step = si->singleStep();
        else if (QDoubleSpinBox *sd = qobject_cast<QDoubleSpinBox*>(s)) step = sd->singleStep();
        return step > 0.0 ? step : 1.0;
    }
    // very wide ranges move faster so that the whole range can be reached
    static double multiplierOf(QAbstractSpinBox *s)
    {
        const double steps = (maxOf(s) - minOf(s)) / stepOf(s);
        if (steps > 2000.0) return 20.0;
        if (steps > 500.0) return 5.0;
        return 1.0;
    }
    // Krimble 2026-11-11: the drag of a plain number field: only QSpinBox and QDoubleSpinBox (the bars have their own dragging), only when the finger moved mostly sideways
    bool sidewaysDrag(const QPoint &d) const
    {
        if (!m_spin || !(qobject_cast<QSpinBox*>(m_spin) || qobject_cast<QDoubleSpinBox*>(m_spin))) return false;
        return qAbs(d.x()) > TapSlop && qAbs(d.x()) > 2 * qAbs(d.y());
    }
    // dragging the width of the field changes the value by up to 100 (a percentage box moves over its whole 0 to 100; a bigger range moves 100 units; a small range such as 0 to 1 moves its whole range)
    void dragValue(int dx)
    {
        if (!m_spin) return;
        const double range = maxOf(m_spin) - minOf(m_spin);
        const double unit = qMin(range, 100.0);
        double v = m_startValue + double(dx) / qMax(1, m_spin->width()) * unit;
        v = qBound(minOf(m_spin), v, maxOf(m_spin));
        setSpinValue(m_spin, v);
    }
    void tip(const QPoint &where = QPoint())
    {
        if (m_spin) {
            QToolTip::showText(where.isNull() ? m_pressPos : where, m_spin->text(), m_spin);
        }
    }

    QPointer<QAbstractSpinBox> m_spin;
    QPointer<QWidget> m_pressWidget;     // KRIMBLE 2026-10-07
    QPoint m_pressLocal;                 // KRIMBLE 2026-10-07
    bool m_replaying {false};            // KRIMBLE 2026-10-07
    State m_state {Idle};
    QPoint m_pressPos;
    double m_startValue {0.0};
    QTimer m_timer;
};
#endif

