/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 55): George (Chatty's ideas, the first one: "The first one is pretty."): the SmartCompression panel follows that mockup.
 *
 *  The look of the SmartCompression panel, with nothing of the program in it (only Qt), so that it can be run and looked at on its own: the header, Whole Picture and Regions, JPEG Quality
 *  and Smart Compression (a slider and a number box each), Protection (Smart, Selection, None) with its sensitivity, the region tools, Extra Color / Grayscale / Progressive, the preview
 *  (Original, Compressed, how much smaller) and Save JPEG. The dock connects its signals to Krita.
 */

#ifndef _SMARTCOMPRESSION_PANEL_H_
#define _SMARTCOMPRESSION_PANEL_H_

#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

class SmartCompressionPanel : public QWidget
{
    Q_OBJECT
public:
    enum Protect { ProtectSmart = 0, ProtectSelection = 1, ProtectNone = 2 };

    // the pictures for the header and the buttons are handed in (the dock loads them from the program's icon set, a test loads files)
    struct Icons {
        QIcon wand, picture, regions, rectangle, freehand, invert, all, save;
        QPixmap logo;   // the SmartCompression logo
    };

    explicit SmartCompressionPanel(const Icons &icons, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        // the chosen Protection row is marked with a soft blue field (as in the mockup)
        setStyleSheet(QStringLiteral("QRadioButton:checked { background: rgba(29, 124, 178, 70); border: 1px solid #2a8fd0; border-radius: 6px; }"));
        QVBoxLayout *root = new QVBoxLayout(this);
        root->setContentsMargins(0, 0, 0, 6);
        root->setSpacing(8);

        // the header: the wand and the logo on a black band
        QWidget *band = new QWidget(this);
        band->setStyleSheet(QStringLiteral("background: #000000;"));
        QHBoxLayout *bandLayout = new QHBoxLayout(band);
        bandLayout->setContentsMargins(10, 4, 10, 4);
        QLabel *wand = new QLabel(band);
        wand->setPixmap(icons.wand.pixmap(30, 30));
        QLabel *logo = new QLabel(band);
        logo->setPixmap(icons.logo);
        bandLayout->addWidget(wand);
        bandLayout->addWidget(logo);
        bandLayout->addStretch(1);
        band->setMinimumHeight(48);
        root->addWidget(band);

        QWidget *body = new QWidget(this);
        QVBoxLayout *bodyLayout = new QVBoxLayout(body);
        bodyLayout->setContentsMargins(8, 0, 8, 0);
        bodyLayout->setSpacing(8);
        root->addWidget(body);

        // Whole Picture / Regions
        QHBoxLayout *modeRow = new QHBoxLayout();
        modeRow->setSpacing(6);
        m_whole = new QPushButton(icons.picture, QStringLiteral(" ") + tr("Whole Picture"), body);
        m_regions = new QPushButton(icons.regions, QStringLiteral(" ") + tr("Regions"), body);
        for (QPushButton *button : {m_whole, m_regions}) {
            button->setCheckable(true);
            button->setAutoExclusive(true);
            button->setMinimumHeight(40);
            button->setIconSize(QSize(22, 22));
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            modeRow->addWidget(button);
        }
        m_whole->setChecked(true);
        bodyLayout->addLayout(modeRow);
        connect(m_regions, &QPushButton::toggled, this, &SmartCompressionPanel::regionsToggled);

        // JPEG Quality and Smart Compression
        addSliderRow(bodyLayout, body, tr("JPEG Quality"), 85, &m_quality, &m_qualitySpin);
        addSliderRow(bodyLayout, body, tr("Smart Compression"), 35, &m_level, &m_levelSpin);
        m_levelSpin->setSuffix(QStringLiteral("%"));
        bodyLayout->addWidget(divider(body));

        // Protection
        QLabel *protection = new QLabel(tr("Protection"), body);
        QFont f = protection->font();
        f.setPointSizeF(f.pointSizeF() * 1.15);
        protection->setFont(f);
        bodyLayout->addWidget(protection);
        m_protectGroup = new QButtonGroup(this);
        const QStringList protectTexts = {tr("Smart \u2013 automatically protect image details"), tr("Selection \u2013 protect manually selected regions"), tr("None \u2013 uniform compression")};
        for (int i = 0; i < 3; ++i) {
            QRadioButton *radio = new QRadioButton(protectTexts.at(i), body);
            radio->setMinimumHeight(30);
            m_protectGroup->addButton(radio, i);
            bodyLayout->addWidget(radio);
        }
        m_protectGroup->button(ProtectSmart)->setChecked(true);
        connect(m_protectGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this](int id) { Q_EMIT protectionChanged(id); Q_EMIT settingsChanged(); });
        addSliderRow(bodyLayout, body, tr("Protection Sensitivity"), 35, &m_sensitivity, &m_sensitivitySpin);
        bodyLayout->addWidget(divider(body));

        // the region tools: Rectangle, Freehand, Invert, All (enabled in Regions mode)
        m_tools = new QWidget(body);
        QHBoxLayout *toolsRow = new QHBoxLayout(m_tools);
        toolsRow->setContentsMargins(0, 0, 0, 0);
        toolsRow->setSpacing(6);
        struct Tool { QString text; QIcon icon; QString toolId; QString actionName; };
        const QList<Tool> tools = {{tr("Rectangle"), icons.rectangle, QStringLiteral("KisToolSelectRectangular"), QString()},
                                   {tr("Freehand"), icons.freehand, QStringLiteral("KisToolSelectOutline"), QString()},
                                   {tr("Invert"), icons.invert, QString(), QStringLiteral("invert_selection")},
                                   {tr("All"), icons.all, QString(), QStringLiteral("select_all")}};
        for (const Tool &tool : tools) {
            QPushButton *button = new QPushButton(tool.icon, tool.text, m_tools);
            button->setIconSize(QSize(18, 18));
            button->setMinimumHeight(34);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            toolsRow->addWidget(button);
            connect(button, &QPushButton::clicked, this, [this, tool]() {
                if (!tool.toolId.isEmpty()) Q_EMIT toolRequested(tool.toolId); else Q_EMIT actionRequested(tool.actionName);
            });
        }
        m_tools->setEnabled(false);
        bodyLayout->addWidget(m_tools);
        bodyLayout->addWidget(divider(body));

        // Extra Color, Grayscale, Progressive, each with a short line under it
        m_extraColor = addCheck(bodyLayout, body, tr("Extra Color"), tr("preserve subtle color"), false);
        m_grayscale = addCheck(bodyLayout, body, tr("Grayscale"), tr("convert to monochrome"), false);
        m_progressive = addCheck(bodyLayout, body, tr("Progressive"), tr("faster web loading"), true);

        // the preview: Original, how much smaller, Compressed
        m_previewFrame = new QFrame(body);
        m_previewFrame->setObjectName(QStringLiteral("smartPreview"));
        m_previewFrame->setStyleSheet(QStringLiteral("QFrame#smartPreview { background: #0f1428; border: 1px solid #2d3866; border-radius: 10px; }"
                                                     "QLabel { background: transparent; border: none; }"));
        QGridLayout *previewGrid = new QGridLayout(m_previewFrame);
        previewGrid->setContentsMargins(8, 8, 8, 8);
        previewGrid->setHorizontalSpacing(8);
        m_originalPicture = new QLabel(m_previewFrame);
        m_compressedPicture = new QLabel(m_previewFrame);
        for (QLabel *label : {m_originalPicture, m_compressedPicture}) {
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumSize(96, 96);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            label->setStyleSheet(QStringLiteral("background: #060914; border: 1px solid #36457a;"));
        }
        m_originalCaption = new QLabel(tr("Original"), m_previewFrame);
        m_compressedCaption = new QLabel(tr("Compressed"), m_previewFrame);
        m_percent = new QLabel(QString(), m_previewFrame);
        m_percent->setAlignment(Qt::AlignCenter);
        m_percent->setStyleSheet(QStringLiteral("color: #6be28a; font-weight: bold;"));
        previewGrid->addWidget(m_originalPicture, 0, 0);
        previewGrid->addWidget(m_percent, 0, 1);
        previewGrid->addWidget(m_compressedPicture, 0, 2);
        previewGrid->addWidget(m_originalCaption, 1, 0, Qt::AlignHCenter);
        previewGrid->addWidget(m_compressedCaption, 1, 2, Qt::AlignHCenter);
        previewGrid->setColumnStretch(0, 3);
        previewGrid->setColumnStretch(1, 2);
        previewGrid->setColumnStretch(2, 3);
        bodyLayout->addWidget(m_previewFrame);

        // Save JPEG...
        QPushButton *save = new QPushButton(icons.save, QStringLiteral("  ") + tr("Save JPEG..."), body);
        save->setDefault(true);
        save->setMinimumHeight(44);
        save->setIconSize(QSize(22, 22));
        bodyLayout->addWidget(save);
        connect(save, &QPushButton::clicked, this, &SmartCompressionPanel::saveRequested);
        root->addStretch(1);

        for (QSlider *slider : {m_quality, m_level, m_sensitivity}) connect(slider, &QSlider::valueChanged, this, &SmartCompressionPanel::settingsChanged);
        connect(m_sensitivity, &QSlider::valueChanged, this, &SmartCompressionPanel::sensitivityChanged);
        for (QCheckBox *box : {m_extraColor, m_grayscale, m_progressive}) connect(box, &QCheckBox::toggled, this, &SmartCompressionPanel::settingsChanged);
    }

    int quality() const { return m_quality->value(); }
    int level() const { return m_level->value(); }
    int protect() const { return m_protectGroup->checkedId(); }
    int sensitivity() const { return m_sensitivity->value(); }
    bool extraColor() const { return m_extraColor->isChecked(); }
    bool grayscale() const { return m_grayscale->isChecked(); }
    bool progressive() const { return m_progressive->isChecked(); }
    void setQuality(int v) { m_quality->setValue(v); }
    void setLevel(int v) { m_level->setValue(v); }
    void setSensitivity(int v) { m_sensitivity->setValue(v); }
    void setProtect(int p) { if (QAbstractButton *b = m_protectGroup->button(p)) b->setChecked(true); }
    void setRegions(bool on) { (on ? m_regions : m_whole)->setChecked(true); }
    void setToolsEnabled(bool on) { m_tools->setEnabled(on); }
    bool isRegions() const { return m_regions->isChecked(); }

    // the preview: the pictures (already scaled by the caller) and the sizes in bytes
    void setPreview(const QPixmap &original, const QPixmap &compressed, qint64 originalBytes, qint64 compressedBytes)
    {
        m_originalPicture->setPixmap(original);
        m_compressedPicture->setPixmap(compressed);
        m_originalCaption->setText(tr("Original") + QStringLiteral("  ") + sizeText(originalBytes));
        m_compressedCaption->setText(tr("Compressed") + QStringLiteral("  ") + sizeText(compressedBytes));
        if (originalBytes > 0 && compressedBytes >= 0) {
            const int percent = qRound(100.0 * (1.0 - double(compressedBytes) / double(originalBytes)));
            m_percent->setText(QStringLiteral("\u2013") + QString::number(qAbs(percent)) + QStringLiteral("%\n") + (percent >= 0 ? tr("Smaller") : tr("Larger")));
        } else {
            m_percent->clear();
        }
    }
    QSize previewPictureSize() const { return m_originalPicture->size(); }

    static QString sizeText(qint64 bytes)
    {
        if (bytes >= 1024 * 1024) return QString::number(bytes / 1048576.0, 'f', 2) + QStringLiteral(" MB");
        return QString::number(qRound(bytes / 1024.0)) + QStringLiteral(" KB");
    }

Q_SIGNALS:
    void regionsToggled(bool regions);
    void protectionChanged(int protect);
    void sensitivityChanged(int value);
    void settingsChanged();
    void toolRequested(const QString &toolId);
    void actionRequested(const QString &actionName);
    void saveRequested();

private:
    static QFrame *divider(QWidget *parent)
    {
        QFrame *line = new QFrame(parent);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet(QStringLiteral("color: #3a4468;"));
        return line;
    }
    void addSliderRow(QVBoxLayout *layout, QWidget *parent, const QString &text, int value, QSlider **sliderOut, QSpinBox **spinOut)
    {
        QHBoxLayout *row = new QHBoxLayout();
        row->setSpacing(8);
        QLabel *label = new QLabel(text, parent);
        QSlider *slider = new QSlider(Qt::Horizontal, parent);
        slider->setRange(0, 100);
        slider->setValue(value);
        QSpinBox *spin = new QSpinBox(parent);
        spin->setRange(0, 100);
        spin->setValue(value);
        spin->setMinimumWidth(70);
        row->addWidget(label);
        row->addWidget(slider, 1);
        row->addWidget(spin);
        layout->addLayout(row);
        connect(slider, &QSlider::valueChanged, spin, &QSpinBox::setValue);
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), slider, &QSlider::setValue);
        *sliderOut = slider;
        *spinOut = spin;
    }
    QCheckBox *addCheck(QVBoxLayout *layout, QWidget *parent, const QString &title, const QString &sub, bool checked)
    {
        QCheckBox *box = new QCheckBox(title, parent);
        box->setChecked(checked);
        box->setMinimumHeight(28);
        QLabel *subtitle = new QLabel(sub, parent);
        subtitle->setStyleSheet(QStringLiteral("color: #9aa6d0; font-size: 11px; margin-left: 30px;"));
        QVBoxLayout *column = new QVBoxLayout();
        column->setSpacing(0);
        column->addWidget(box);
        column->addWidget(subtitle);
        layout->addLayout(column);
        return box;
    }

    QPushButton *m_whole {nullptr};
    QPushButton *m_regions {nullptr};
    QSlider *m_quality {nullptr};
    QSpinBox *m_qualitySpin {nullptr};
    QSlider *m_level {nullptr};
    QSpinBox *m_levelSpin {nullptr};
    QButtonGroup *m_protectGroup {nullptr};
    QSlider *m_sensitivity {nullptr};
    QSpinBox *m_sensitivitySpin {nullptr};
    QWidget *m_tools {nullptr};
    QCheckBox *m_extraColor {nullptr};
    QCheckBox *m_grayscale {nullptr};
    QCheckBox *m_progressive {nullptr};
    QFrame *m_previewFrame {nullptr};
    QLabel *m_originalPicture {nullptr};
    QLabel *m_compressedPicture {nullptr};
    QLabel *m_originalCaption {nullptr};
    QLabel *m_compressedCaption {nullptr};
    QLabel *m_percent {nullptr};
};

#endif
