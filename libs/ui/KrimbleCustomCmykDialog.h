// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-11: George: "I really want [industry standard] style interfaces for RGB to CMYK." and, of the pictures of the Custom CMYK, Dot Gain Curves, Ink Colors and Color Settings dialogs: "yes"
// (start with the Custom CMYK dialog). The Custom CMYK dialog of the working CMYK profile, built on the conversion engine (KrimbleCmykEngine.h, changelog 450): name, ink colors (the presets),
// dot gain (Standard with its percent, or Curves... for the 13 point table of each ink), separation type (GCR or UCR), black generation, black ink limit, total ink limit, UCA amount, black point
// compensation, and a gray ramp graph that shows the plates the engine makes for a gray from white to black. Load... and Save... use the saved profiles of KrimbleCmykProfiles. The dialog is a
// scroll area, so it fits a phone. Header only and without Q_OBJECT (everything is wired with lambdas), so no moc or CMake change is needed. NOT in this first version: the Ink Colors dialog (xyY or Lab
// values of each ink; only the presets for now), the display control points with midpoints, and the camera made profile.
#pragma once

#include <QButtonGroup>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QSettings>
#include "KrimbleCmykIcc.h"
#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGuiApplication>
#include <QScreen>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>

#include "KrimbleCmykEngine.h"

// the gray ramp: the C, M, Y and K plates (percent, up) the engine makes for a gray from white (left) to black (right)
class KrimbleGrayRampGraph : public QWidget
{
public:
    explicit KrimbleGrayRampGraph(QWidget *parent = nullptr) : QWidget(parent) { setMinimumSize(100, 150); }   // Krimble 2026-11-11: 100 wide (was 170), for a phone
    QSize sizeHint() const override { return QSize(230, 190); }
    void setProfile(const KrimbleCmykProfile &profile)
    {
        const KrimbleCmykEngine engine(profile);
        const int n = 24;
        m_plates.clear();
        for (int i = 0; i <= n; ++i) {
            const int v = qRound(255.0 * (1.0 - double(i) / n));
            m_plates.append(engine.fromRgb(QColor(v, v, v)));
        }
        update();
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        const QRectF r = QRectF(rect()).adjusted(4, 4, -4, -4);
        p.fillRect(r, QColor("#111521"));
        p.setPen(QPen(QColor("#3a4570"), 1));
        for (int i = 1; i < 4; ++i) {
            p.drawLine(QPointF(r.left() + r.width() * i / 4.0, r.top()), QPointF(r.left() + r.width() * i / 4.0, r.bottom()));
            p.drawLine(QPointF(r.left(), r.top() + r.height() * i / 4.0), QPointF(r.right(), r.top() + r.height() * i / 4.0));
        }
        const int n = m_plates.size() - 1;
        if (n >= 1) {
            static const char *colors[4] = {"#1ea0ff", "#ff3d7f", "#ffd400", "#f2f4ff"};   // C, M, Y, K (K is light so that it shows on the dark graph)
            for (int ch = 0; ch < 4; ++ch) {
                QPainterPath path;
                for (int i = 0; i <= n; ++i) {
                    const double v = qBound(0.0, m_plates[i][ch], 1.0);
                    const QPointF pt(r.left() + r.width() * i / double(n), r.bottom() - r.height() * v);
                    if (i == 0) path.moveTo(pt); else path.lineTo(pt);
                }
                p.setPen(QPen(QColor(colors[ch]), 2));
                p.setBrush(Qt::NoBrush);
                p.drawPath(path);
            }
        }
        p.setPen(QPen(QColor("#000000"), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRect(r);
    }
private:
    QVector<KrimblePlates> m_plates;
};

// the dot gain curve of one ink: the printed dot for a plate dot (a straight line is no gain)
class KrimbleDotGainGraph : public QWidget
{
public:
    explicit KrimbleDotGainGraph(QWidget *parent = nullptr) : QWidget(parent) { setMinimumSize(170, 170); }
    QSize sizeHint() const override { return QSize(220, 220); }
    void setGain(const KrimbleDotGain &gain, int channel) { m_gain = gain; m_channel = channel; update(); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        const QRectF r = QRectF(rect()).adjusted(4, 4, -4, -4);
        p.fillRect(r, QColor("#111521"));
        p.setPen(QPen(QColor("#3a4570"), 1));
        for (int i = 1; i < 10; ++i) {
            p.drawLine(QPointF(r.left() + r.width() * i / 10.0, r.top()), QPointF(r.left() + r.width() * i / 10.0, r.bottom()));
            p.drawLine(QPointF(r.left(), r.top() + r.height() * i / 10.0), QPointF(r.right(), r.top() + r.height() * i / 10.0));
        }
        p.setPen(QPen(QColor("#6a74a0"), 1, Qt::DashLine));
        p.drawLine(r.bottomLeft(), r.topRight());
        QPainterPath path;
        const int steps = 50;
        for (int i = 0; i <= steps; ++i) {
            const double a = double(i) / steps;
            const QPointF pt(r.left() + r.width() * a, r.bottom() - r.height() * m_gain.apply(m_channel, a));
            if (i == 0) path.moveTo(pt); else path.lineTo(pt);
        }
        p.setPen(QPen(QColor("#f2f4ff"), 2));
        p.setBrush(Qt::NoBrush);
        p.drawPath(path);
        p.setPen(QPen(QColor("#000000"), 1));
        p.drawRect(r);
    }
private:
    KrimbleDotGain m_gain;
    int m_channel = 0;
};

// Dot Gain Curves: the printed dot for a plate dot of 2, 4, 6, 8, 10, 20 ... 90 percent for each ink (All Same sets the four inks together). Returns true on OK.
inline bool krimbleShowDotGainCurves(QWidget *parent, KrimbleDotGain &gain)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(QStringLiteral("Dot Gain Curves"));
    KrimbleDotGain work = gain;
    int channel = 0;
    QVBoxLayout *root = new QVBoxLayout(&dialog);
    QHBoxLayout *top = new QHBoxLayout;
    root->addLayout(top);
    KrimbleDotGainGraph *graph = new KrimbleDotGainGraph(&dialog);
    top->addWidget(graph, 1);
    QGridLayout *grid = new QGridLayout;
    top->addLayout(grid);
    QDoubleSpinBox *boxes[KrimbleDotGain::N];
    QCheckBox *allSame = new QCheckBox(QStringLiteral("All Same"), &dialog);
    allSame->setChecked(work.allSame);
    for (int i = 0; i < KrimbleDotGain::N; ++i) {
        boxes[i] = new QDoubleSpinBox(&dialog);
        boxes[i]->setRange(0.0, 100.0);
        boxes[i]->setDecimals(1);
        boxes[i]->setSuffix(QStringLiteral(" %"));
        const int row = i < 7 ? i : i - 7;
        const int col = i < 7 ? 0 : 2;
        grid->addWidget(new QLabel(QString::number(KrimbleDotGain::kPoints[i]) + QStringLiteral(":"), &dialog), row, col);
        grid->addWidget(boxes[i], row, col + 1);
    }
    auto refresh = [&]() {
        for (int i = 0; i < KrimbleDotGain::N; ++i) {
            QSignalBlocker blocker(boxes[i]);
            boxes[i]->setValue(work.table[channel][i]);
        }
        graph->setGain(work, channel);
    };
    for (int i = 0; i < KrimbleDotGain::N; ++i) {
        QObject::connect(boxes[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [&, i](double v) {
            work.curves = true;
            if (allSame->isChecked()) { for (int ch = 0; ch < 4; ++ch) work.table[ch][i] = v; }
            else work.table[channel][i] = v;
            graph->setGain(work, channel);
        });
    }
    QHBoxLayout *inks = new QHBoxLayout;
    root->addLayout(inks);
    static const char *names[4] = {"Cyan", "Magenta", "Yellow", "Black"};
    QButtonGroup *group = new QButtonGroup(&dialog);
    for (int ch = 0; ch < 4; ++ch) {
        QRadioButton *radio = new QRadioButton(QString::fromLatin1(names[ch]), &dialog);
        radio->setChecked(ch == 0);
        group->addButton(radio, ch);
        inks->addWidget(radio);
        QObject::connect(radio, &QRadioButton::toggled, &dialog, [&, ch](bool on) { if (on) { channel = ch; refresh(); } });
    }
    inks->addWidget(allSame);
    QObject::connect(allSame, &QCheckBox::toggled, &dialog, [&](bool on) {
        work.allSame = on;
        if (on) { for (int ch = 0; ch < 4; ++ch) for (int i = 0; i < KrimbleDotGain::N; ++i) work.table[ch][i] = work.table[channel][i]; refresh(); }
    });
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    root->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    refresh();
    if (dialog.exec() != QDialog::Accepted) return false;
    work.curves = true;
    work.allSame = allSame->isChecked();
    gain = work;
    return true;
}

// Custom CMYK: the working CMYK profile (KrimbleCmykProfiles::current). OK makes the edited profile the working profile.
inline void krimbleShowCustomCmykDialog(QWidget *parent)
{
    KrimbleCmykProfile profile = KrimbleCmykProfiles::current();
    QDialog dialog(parent);
    dialog.setWindowTitle(QStringLiteral("Custom CMYK"));
    // dialog.resize(520, 700);
    // Krimble 2026-11-11: George's screenshot (build 56) showed the dialog wider than his phone: the ink colors box, the dot gain percent and the Gray Ramp were cut off at the right edge, with a horizontal
    // scroll bar. The dialog is now no wider than the screen (and no taller), and has no horizontal scroll bar.
    {
        QSize room(520, 700);
        if (QScreen *screen = QGuiApplication::primaryScreen()) room = QSize(qMin(520, screen->availableGeometry().width() - 8), qMin(700, screen->availableGeometry().height() - 60));
        dialog.resize(room);
    }
    QVBoxLayout *outer = new QVBoxLayout(&dialog);
    outer->setContentsMargins(4, 4, 4, 4);   // Krimble 2026-11-11: thinner margins, for a phone
    QScrollArea *scroll = new QScrollArea(&dialog);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Krimble 2026-11-11: the content is made to fit the width
    QWidget *content = new QWidget;
    scroll->setWidget(content);
    outer->addWidget(scroll, 1);
    QVBoxLayout *lay = new QVBoxLayout(content);
    lay->setContentsMargins(2, 2, 2, 2);

    QHBoxLayout *nameRow = new QHBoxLayout;
    nameRow->addWidget(new QLabel(QStringLiteral("Name:"), content));
    QLineEdit *nameEdit = new QLineEdit(content);
    nameEdit->setMinimumWidth(40);   // Krimble 2026-11-11: no minimum that is wider than a phone
    nameRow->addWidget(nameEdit, 1);
    lay->addLayout(nameRow);

    // ink options
    QGroupBox *inkBox = new QGroupBox(QStringLiteral("Ink Options"), content);
    QGridLayout *inkGrid = new QGridLayout(inkBox);
    inkGrid->setContentsMargins(6, 6, 6, 6);
    inkGrid->addWidget(new QLabel(QStringLiteral("Ink Colors:"), inkBox), 0, 0);
    QComboBox *inkCombo = new QComboBox(inkBox);
    inkCombo->addItems(KrimbleInkSet::presetNames());
    inkCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);   // Krimble 2026-11-11: as wide as the room allows, not as wide as the longest name
    inkCombo->setMinimumContentsLength(8);
    if (profile.inkPreset == QLatin1String("Custom")) inkCombo->addItem(QStringLiteral("Custom"));
    inkGrid->addWidget(inkCombo, 0, 1, 1, 2);
    inkGrid->addWidget(new QLabel(QStringLiteral("Dot Gain:"), inkBox), 1, 0);
    QComboBox *gainCombo = new QComboBox(inkBox);
    gainCombo->addItem(QStringLiteral("Standard"));
    gainCombo->addItem(QStringLiteral("Curves..."));
    gainCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    gainCombo->setMinimumContentsLength(6);
    inkGrid->addWidget(gainCombo, 1, 1);
    QDoubleSpinBox *gainPercent = new QDoubleSpinBox(inkBox);
    gainPercent->setRange(0.0, 50.0);
    gainPercent->setDecimals(0);
    gainPercent->setSuffix(QStringLiteral(" %"));
    inkGrid->addWidget(gainPercent, 1, 2);
    lay->addWidget(inkBox);

    // separation options
    QGroupBox *sepBox = new QGroupBox(QStringLiteral("Separation Options"), content);
    QGridLayout *sepGrid = new QGridLayout(sepBox);
    sepGrid->setContentsMargins(6, 6, 6, 6);
    sepGrid->addWidget(new QLabel(QStringLiteral("Separation Type:"), sepBox), 0, 0);
    QRadioButton *gcrRadio = new QRadioButton(QStringLiteral("GCR"), sepBox);
    QRadioButton *ucrRadio = new QRadioButton(QStringLiteral("UCR"), sepBox);
    QHBoxLayout *typeRow = new QHBoxLayout;
    typeRow->addWidget(gcrRadio);
    typeRow->addWidget(ucrRadio);
    sepGrid->addLayout(typeRow, 0, 1);
    sepGrid->addWidget(new QLabel(QStringLiteral("Black Generation:"), sepBox), 1, 0);
    QComboBox *blackGen = new QComboBox(sepBox);
    blackGen->addItems(QStringList() << QStringLiteral("None") << QStringLiteral("Light") << QStringLiteral("Medium") << QStringLiteral("Heavy") << QStringLiteral("Maximum"));
    blackGen->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    blackGen->setMinimumContentsLength(6);
    sepGrid->addWidget(blackGen, 1, 1);
    sepGrid->addWidget(new QLabel(QStringLiteral("Black Ink Limit:"), sepBox), 2, 0);
    QSpinBox *blackLimit = new QSpinBox(sepBox);
    blackLimit->setRange(0, 100);
    blackLimit->setSuffix(QStringLiteral(" %"));
    sepGrid->addWidget(blackLimit, 2, 1);
    sepGrid->addWidget(new QLabel(QStringLiteral("Total Ink Limit:"), sepBox), 3, 0);
    QSpinBox *totalLimit = new QSpinBox(sepBox);
    totalLimit->setRange(100, 400);
    totalLimit->setSuffix(QStringLiteral(" %"));
    sepGrid->addWidget(totalLimit, 3, 1);
    sepGrid->addWidget(new QLabel(QStringLiteral("UCA Amount:"), sepBox), 4, 0);
    QSpinBox *ucaAmount = new QSpinBox(sepBox);
    ucaAmount->setRange(0, 100);
    ucaAmount->setSuffix(QStringLiteral(" %"));
    sepGrid->addWidget(ucaAmount, 4, 1);
    // sepGrid->addWidget(new QLabel(QStringLiteral("Gray Ramp:"), sepBox), 0, 2, Qt::AlignHCenter);
    // KrimbleGrayRampGraph *ramp = new KrimbleGrayRampGraph(sepBox);
    // sepGrid->addWidget(ramp, 1, 2, 4, 1);
    // Krimble 2026-11-11: the Gray Ramp is under the settings, across the width (it was a third column at the right, which was too wide for a phone).
    sepGrid->addWidget(new QLabel(QStringLiteral("Gray Ramp:"), sepBox), 5, 0, 1, 2);
    KrimbleGrayRampGraph *ramp = new KrimbleGrayRampGraph(sepBox);
    ramp->setMinimumHeight(170);
    sepGrid->addWidget(ramp, 6, 0, 1, 2);
    lay->addWidget(sepBox);

    // QCheckBox *bpc = new QCheckBox(QStringLiteral("Use Black Point Compensation"), content);
    QCheckBox *bpc = new QCheckBox(QStringLiteral("Black Point Compensation"), content);   // Krimble 2026-11-11: shorter, for a phone
    // the labels may wrap onto two lines, so that no label forces the dialog wider than the screen
    Q_FOREACH (QLabel *label, content->findChildren<QLabel *>()) label->setWordWrap(true);
    lay->addWidget(bpc);
    lay->addStretch(1);

    // the buttons: Load..., Save..., OK, Cancel
    QHBoxLayout *buttonRow = new QHBoxLayout;
    QPushButton *loadButton = new QPushButton(QStringLiteral("Load..."), &dialog);
    QPushButton *saveButton = new QPushButton(QStringLiteral("Save..."), &dialog);
    buttonRow->addWidget(loadButton);
    buttonRow->addWidget(saveButton);
    buttonRow->addStretch(1);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    // buttonRow->addWidget(buttons);
    // outer->addLayout(buttonRow);
    // Krimble 2026-11-11: four buttons in one row were wider than a phone. Load and Save are one row, Cancel and OK the one under it.
    QHBoxLayout *okRow = new QHBoxLayout;
    okRow->addStretch(1);
    okRow->addWidget(buttons);
    outer->addLayout(buttonRow);
    outer->addLayout(okRow);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // the gray ramp is made a moment after the last change (the engine solves 25 grays)
    QTimer *rampTimer = new QTimer(&dialog);
    rampTimer->setSingleShot(true);
    rampTimer->setInterval(250);
    QObject::connect(rampTimer, &QTimer::timeout, &dialog, [&]() { ramp->setProfile(profile); });

    QString lastAuto;
    auto enableByType = [&]() {
        blackGen->setEnabled(gcrRadio->isChecked());
        ucaAmount->setEnabled(gcrRadio->isChecked());
    };
    // the profile to the widgets
    auto toUi = [&]() {
        const QSignalBlocker b1(inkCombo), b2(gainCombo), b3(gainPercent), b4(gcrRadio), b5(ucrRadio), b6(blackGen), b7(blackLimit), b8(totalLimit), b9(ucaAmount), b10(bpc);
        int inkIndex = inkCombo->findText(profile.inkPreset);
        if (inkIndex < 0) { inkCombo->addItem(profile.inkPreset); inkIndex = inkCombo->count() - 1; }
        inkCombo->setCurrentIndex(inkIndex);
        gainCombo->setCurrentIndex(profile.dotGain.curves ? 1 : 0);
        gainPercent->setValue(profile.dotGain.percent);
        gainPercent->setEnabled(!profile.dotGain.curves);
        gcrRadio->setChecked(profile.gcr);
        ucrRadio->setChecked(!profile.gcr);
        blackGen->setCurrentText(profile.blackGeneration);
        blackLimit->setValue(qRound(profile.blackLimit));
        totalLimit->setValue(qRound(profile.totalLimit));
        ucaAmount->setValue(qRound(profile.uca));
        bpc->setChecked(profile.blackPointCompensation);
        enableByType();
        lastAuto = profile.autoName();
        nameEdit->setText(profile.displayName());
    };
    // the widgets to the profile (except the name)
    auto fromUi = [&]() {
        profile.gcr = gcrRadio->isChecked();
        profile.blackGeneration = blackGen->currentText();
        profile.blackLimit = blackLimit->value();
        profile.totalLimit = totalLimit->value();
        profile.uca = ucaAmount->value();
        profile.blackPointCompensation = bpc->isChecked();
    };
    // something changed: the profile follows the widgets, the name follows the settings (unless it was typed), the gray ramp is made again
    auto changed = [&]() {
        fromUi();
        enableByType();
        const QString autoNow = profile.autoName();
        if (nameEdit->text() == lastAuto) nameEdit->setText(autoNow);
        lastAuto = autoNow;
        rampTimer->start();
    };
    toUi();
    ramp->setProfile(profile);

    QObject::connect(inkCombo, QOverload<int>::of(&QComboBox::activated), &dialog, [&](int) {
        const QString name = inkCombo->currentText();
        if (KrimbleInkSet::preset(name, profile.inks)) profile.inkPreset = name;
        changed();
    });
    QObject::connect(gainCombo, QOverload<int>::of(&QComboBox::activated), &dialog, [&](int index) {
        if (index == 1) {
            if (krimbleShowDotGainCurves(&dialog, profile.dotGain)) {
                profile.dotGain.curves = true;
            }
        } else {
            profile.dotGain.curves = false;
            profile.dotGain.fillFromStandard();
        }
        { const QSignalBlocker blocker(gainCombo); gainCombo->setCurrentIndex(profile.dotGain.curves ? 1 : 0); }
        gainPercent->setEnabled(!profile.dotGain.curves);
        changed();
    });
    QObject::connect(gainPercent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [&](double v) {
        profile.dotGain.percent = v;
        profile.dotGain.fillFromStandard();
        changed();
    });
    QObject::connect(gcrRadio, &QRadioButton::toggled, &dialog, [&](bool) { changed(); });
    QObject::connect(blackGen, QOverload<int>::of(&QComboBox::currentIndexChanged), &dialog, [&](int) { changed(); });
    QObject::connect(blackLimit, QOverload<int>::of(&QSpinBox::valueChanged), &dialog, [&](int) { changed(); });
    QObject::connect(totalLimit, QOverload<int>::of(&QSpinBox::valueChanged), &dialog, [&](int) { changed(); });
    QObject::connect(ucaAmount, QOverload<int>::of(&QSpinBox::valueChanged), &dialog, [&](int) { changed(); });
    QObject::connect(bpc, &QCheckBox::toggled, &dialog, [&](bool) { changed(); });

    QObject::connect(loadButton, &QPushButton::clicked, &dialog, [&]() {
        QMenu menu(&dialog);
        const QStringList saved = KrimbleCmykProfiles::list();
        for (const QString &name : saved) menu.addAction(name);
        if (saved.isEmpty()) menu.addAction(QStringLiteral("(no saved profiles yet)"))->setEnabled(false);
        QAction *chosen = menu.exec(loadButton->mapToGlobal(QPoint(0, loadButton->height())));
        if (!chosen || !chosen->isEnabled()) return;
        KrimbleCmykProfile loaded;
        if (KrimbleCmykProfiles::load(chosen->text(), loaded)) {
            profile = loaded;
            toUi();
            rampTimer->start();
        }
    });
    QObject::connect(saveButton, &QPushButton::clicked, &dialog, [&]() {
        fromUi();
        const QString typed = nameEdit->text().trimmed();
        profile.name = (typed.isEmpty() || typed == profile.autoName()) ? QString() : typed;
        if (KrimbleCmykProfiles::save(profile))
            QMessageBox::information(&dialog, QStringLiteral("Custom CMYK"), QStringLiteral("Saved: %1").arg(profile.displayName()));
        else
            QMessageBox::warning(&dialog, QStringLiteral("Custom CMYK"), QStringLiteral("The profile could not be saved."));
    });

    if (dialog.exec() != QDialog::Accepted) return;
    fromUi();
    const QString typed = nameEdit->text().trimmed();
    profile.name = (typed.isEmpty() || typed == profile.autoName()) ? QString() : typed;
    KrimbleCmykProfiles::setCurrent(profile);
    // Krimble 2026-11-13: George: "Finish the CMYK system." OK also makes the ICC profile of these settings (KrimbleCmykIcc.h), so Image > Convert Image Color Space (CMYK) and soft proofing can use it.
    // It is made again only when the settings are not the ones the last profile was made from.
    const QByteArray stamp = QCryptographicHash::hash(QJsonDocument(profile.toJson()).toJson(QJsonDocument::Compact), QCryptographicHash::Sha1).toHex();
    if (QSettings().value(QStringLiteral("Krimble/CmykIccStamp")).toByteArray() == stamp) return;
    const QString made = KrimbleCmykIcc::install(profile, parent);
    if (made.isEmpty()) return;
    QSettings().setValue(QStringLiteral("Krimble/CmykIccStamp"), stamp);
    QMessageBox::information(parent, QStringLiteral("Custom CMYK"),
                             QStringLiteral("The profile \"%1\" is ready. Use it in Image > Convert Image Color Space (color model CMYK), or as the proofing profile.").arg(made));
}
