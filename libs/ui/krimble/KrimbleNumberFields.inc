
#ifdef Q_OS_ANDROID
class KrimbleNumberPad : public QDialog
{
public:
    KrimbleNumberPad(QAbstractSpinBox *spin, std::function<void(double)> apply)
        : QDialog(spin->window()), m_spin(spin), m_apply(apply)
    {
        setWindowTitle(i18n("Enter a number"));
        setProperty("krimbleNumberPad", true);
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
    static const int LongPressMs = 350;
    static const int MovePixels = 9;
    static const int TapSlop = 12;

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
            return false;
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
            m_pressWidget = qobject_cast<QWidget*>(watched);
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
                m_timer.stop();
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
            if (state == Dragging) QGuiApplication::inputMethod()->hide();
            if (state == Pending && spin2 && pressWidget) {
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
    enum State { Idle, Pending, Scrubbing, Cancelled, Dragging };

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
    static double multiplierOf(QAbstractSpinBox *s)
    {
        const double steps = (maxOf(s) - minOf(s)) / stepOf(s);
        if (steps > 2000.0) return 20.0;
        if (steps > 500.0) return 5.0;
        return 1.0;
    }
    bool sidewaysDrag(const QPoint &d) const
    {
        if (!m_spin || !(qobject_cast<QSpinBox*>(m_spin) || qobject_cast<QDoubleSpinBox*>(m_spin))) return false;
        return qAbs(d.x()) > TapSlop && qAbs(d.x()) > 2 * qAbs(d.y());
    }
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
    QPointer<QWidget> m_pressWidget;
    QPoint m_pressLocal;
    bool m_replaying {false};
    State m_state {Idle};
    QPoint m_pressPos;
    double m_startValue {0.0};
    QTimer m_timer;
};
#endif

