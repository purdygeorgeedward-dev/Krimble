/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection."
 */

#include "SmartCompressionDock.h"

#include <QAction>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QtMath>
#include <vector>

#include <KConfigGroup>
#include <KSharedConfig>
#include <klocalizedstring.h>
#include <kis_icon_utils.h>
#include <kundo2magicstring.h>

#include <KoCanvasBase.h>
#include <KoToolManager.h>

#include "kis_action.h"
#include "kis_action_manager.h"
#include "kis_canvas2.h"
#include "kis_default_bounds.h"
#include "kis_image.h"
#include "kis_paint_device.h"
#include "kis_pixel_selection.h"
#include "kis_selection_decoration.h"
#include "KisViewManager.h"
#include "tool/kis_selection_tool_helper.h"

#include "KrimbleSmartDetail.h"

SmartCompressionDock::SmartCompressionDock()
    : QDockWidget(i18n("SmartCompression"))
{
    // Krimble 2026-11-13 (task 54): George: "Don't make the SmartCompression panel ugly. I know that's vague, but look it over." Reworked after looking it over in real Qt next to the theme: a black header band with the logo (as the Color Lab has), Whole picture and Regions as one joined pair, equal wide
    // buttons with pictures (the wand for Smart), the sensitivity with its number, thin dividers, the region tools as a 2 x 2 grid (shown in Regions mode only) and the Save JPEG button wide
    // and blue at the bottom. (The first version, plain buttons in rows that did not line up, is replaced.)
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 6);
    layout->setSpacing(8);

    QLabel *band = new QLabel(content);
    band->setStyleSheet(QStringLiteral("background: #000000;"));
    {
        const qreal ratio = devicePixelRatioF();
        QPixmap logo(QStringLiteral(":/dark_krimble-smartcompression-logo.png"));
        if (!logo.isNull()) {
            logo = logo.scaledToHeight(qRound(36 * ratio), Qt::SmoothTransformation);
            logo.setDevicePixelRatio(ratio);
            band->setPixmap(logo);
        }
    }
    band->setContentsMargins(10, 4, 10, 4);
    band->setMinimumHeight(46);
    layout->addWidget(band);

    QWidget *body = new QWidget(content);
    QVBoxLayout *bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(8, 0, 8, 0);
    bodyLayout->setSpacing(8);
    layout->addWidget(body);

    // the two boxes of XAT's window: Work with Whole Image, Work with Regions (one joined pair)
    QHBoxLayout *modeRow = new QHBoxLayout();
    modeRow->setSpacing(0);
    m_wholeButton = new QPushButton(i18n("Whole picture"), body);
    m_regionsButton = new QPushButton(i18n("Regions"), body);
    for (QPushButton *button : {m_wholeButton, m_regionsButton}) {
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setMinimumHeight(34);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        modeRow->addWidget(button);
    }
    m_wholeButton->setChecked(true);
    bodyLayout->addLayout(modeRow);
    connect(m_regionsButton, &QPushButton::toggled, this, &SmartCompressionDock::setRegionsMode);

    // the wand (AutoCompress) and the red cross (Remove All): Smart and Clear
    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->setSpacing(6);
    QPushButton *smartButton = new QPushButton(KisIconUtils::loadIcon("tool_contiguous_selection"), QStringLiteral("  ") + i18n("Smart"), body);
    smartButton->setToolTip(i18n("Select the flat areas to compress; the detail (edges, text, texture) is left outside and kept"));
    QPushButton *clearButton = new QPushButton(i18n("Clear"), body);
    clearButton->setToolTip(i18n("Remove the selection: the whole picture is compressed"));
    for (QPushButton *button : {smartButton, clearButton}) {
        button->setMinimumHeight(36);
        button->setIconSize(QSize(20, 20));
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        actionRow->addWidget(button);
    }
    bodyLayout->addLayout(actionRow);
    connect(smartButton, &QPushButton::clicked, this, &SmartCompressionDock::makeSmartSelection);
    connect(clearButton, &QPushButton::clicked, this, &SmartCompressionDock::clearSelection);

    // the sensitivity of the Smart Selection
    QHBoxLayout *sensRow = new QHBoxLayout();
    QLabel *sensName = new QLabel(i18n("Sensitivity"), body);
    m_sensitivity = new QSlider(Qt::Horizontal, body);
    m_sensitivity->setRange(0, 100);
    m_sensitivity->setValue(KSharedConfig::openConfig()->group("KrimbleSmartCompression").readEntry("sensitivity", 35));
    m_sensitivityLabel = new QLabel(QString::number(m_sensitivity->value()), body);
    m_sensitivityLabel->setMinimumWidth(28);
    m_sensitivityLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sensRow->addWidget(sensName);
    sensRow->addWidget(m_sensitivity, 1);
    sensRow->addWidget(m_sensitivityLabel);
    bodyLayout->addLayout(sensRow);
    connect(m_sensitivity, &QSlider::valueChanged, this, [this](int v) {
        m_sensitivityLabel->setText(QString::number(v));
        KConfigGroup group = KSharedConfig::openConfig()->group("KrimbleSmartCompression");
        group.writeEntry("sensitivity", v);
    });

    // the region tools (Regions mode only): a divider and a 2 x 2 grid: rectangle, freehand, invert, all
    m_regionTools = new QWidget(body);
    QVBoxLayout *toolsLayout = new QVBoxLayout(m_regionTools);
    toolsLayout->setContentsMargins(0, 0, 0, 0);
    toolsLayout->setSpacing(8);
    QFrame *line = new QFrame(m_regionTools);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet(QStringLiteral("color: #3a4468;"));
    toolsLayout->addWidget(line);
    QGridLayout *toolsGrid = new QGridLayout();
    toolsGrid->setSpacing(6);
    toolsLayout->addLayout(toolsGrid);
    auto addTool = [&](int index, const QString &text, const char *iconName, const char *toolId, const char *actionName) {
        QPushButton *button = new QPushButton(text, m_regionTools);
        if (iconName) button->setIcon(KisIconUtils::loadIcon(iconName));
        button->setIconSize(QSize(20, 20));
        button->setMinimumHeight(34);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        toolsGrid->addWidget(button, index / 2, index % 2);
        if (toolId) {
            const QString id = QString::fromLatin1(toolId);
            connect(button, &QPushButton::clicked, this, [this, id]() { useTool(id); });
        } else {
            const QString name = QString::fromLatin1(actionName);
            connect(button, &QPushButton::clicked, this, [this, name]() { triggerAction(name); });
        }
    };
    addTool(0, i18n("Rectangle"), "tool_rect_selection", "KisToolSelectRectangular", nullptr);
    addTool(1, i18n("Freehand"), "tool_outline_selection", "KisToolSelectOutline", nullptr);
    addTool(2, i18n("Invert"), nullptr, nullptr, "invert_selection");
    addTool(3, i18n("All"), nullptr, nullptr, "select_all");
    m_regionTools->hide();
    bodyLayout->addWidget(m_regionTools);

    QFrame *line2 = new QFrame(body);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet(QStringLiteral("color: #3a4468;"));
    bodyLayout->addWidget(line2);
    QPushButton *saveButton = new QPushButton(i18n("Save JPEG..."), body);
    saveButton->setDefault(true);
    saveButton->setMinimumHeight(40);
    bodyLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &SmartCompressionDock::exportJpeg);

    layout->addStretch(1);
    setWidget(content);
}

void SmartCompressionDock::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);
    m_canvas = dynamic_cast<KisCanvas2 *>(canvas);
}

void SmartCompressionDock::unsetCanvas()
{
    setEnabled(false);
    m_canvas = nullptr;
}

// The Smart Selection: the starting selection that the user then edits. It is the area to COMPRESS: the flat blocks of the picture (sky, a plain panel, an empty background). The blocks
// with detail (edges, text, texture) are not in it: they are what the film covers and what the compression leaves alone. (The same analysis as the Smart choice of the JPEG dialog.)
void SmartCompressionDock::makeSmartSelection()
{
    if (!m_canvas || !m_canvas->viewManager()) return;
    KisImageSP image = m_canvas->viewManager()->image();
    if (!image) return;
    image->waitForDone();
    const QRect bounds = image->bounds();
    const QImage picture = image->projection()->convertToQImage(nullptr, bounds.x(), bounds.y(), bounds.width(), bounds.height());
    if (picture.isNull() || picture.width() != bounds.width() || picture.height() != bounds.height()) return;

    std::vector<uint8_t> luminance(size_t(bounds.width()) * bounds.height());
    for (int y = 0; y < bounds.height(); ++y) {
        const QRgb *line = reinterpret_cast<const QRgb *>(picture.constScanLine(y));
        uint8_t *out = luminance.data() + size_t(y) * bounds.width();
        for (int x = 0; x < bounds.width(); ++x) {
            const QRgb p = line[x];
            out[x] = qAlpha(p) < 16 ? 0 : uint8_t((299 * qRed(p) + 587 * qGreen(p) + 114 * qBlue(p)) / 1000);
        }
    }
    std::vector<uint8_t> compress = KrimbleSmart::detailMask(luminance.data(), bounds.width(), bounds.height(), m_sensitivity->value());
    for (uint8_t &value : compress) {
        value = uint8_t(255 - value);   // detail (protected) becomes 0, flat becomes 255 (selected)
    }

    KisPixelSelectionSP selection = new KisPixelSelection(new KisDefaultBounds(image));
    selection->writeBytes(compress.data(), bounds.x(), bounds.y(), bounds.width(), bounds.height());
    KisSelectionToolHelper helper(m_canvas, kundo2_i18n("Smart Selection"));
    helper.selectPixelSelection(selection, SELECTION_REPLACE);
}

void SmartCompressionDock::clearSelection()
{
    triggerAction(QStringLiteral("deselect"));
}

// Regions mode: the film (Krita's selection overlay: it covers what is NOT selected: the protected area) and the region tools; Whole picture: no film, no tools.
// Entering Regions with no selection starts with the Smart Selection.
void SmartCompressionDock::setRegionsMode(bool regions)
{
    m_regionTools->setVisible(regions);
    showFilm(regions);
    if (regions && m_canvas && m_canvas->viewManager() && m_canvas->viewManager()->image() && !m_canvas->viewManager()->image()->globalSelection()) {
        makeSmartSelection();
    }
}

void SmartCompressionDock::useTool(const QString &toolId)
{
    KoToolManager::instance()->switchToolRequested(toolId);
}

void SmartCompressionDock::triggerAction(const QString &actionName)
{
    if (!m_canvas || !m_canvas->viewManager()) return;
    if (KisAction *action = m_canvas->viewManager()->actionManager()->actionByName(actionName)) {
        action->trigger();
    }
}

void SmartCompressionDock::showFilm(bool on)
{
    if (!m_canvas) return;
    KisSelectionDecoration *decoration = qobject_cast<KisSelectionDecoration *>(m_canvas->decoration(QStringLiteral("selection")).data());
    if (!decoration) return;
    const bool isFilm = decoration->mode() == KisSelectionDecoration::Mask;
    if (isFilm != on) {
        triggerAction(QStringLiteral("toggle-selection-overlay-mode"));
    }
}

void SmartCompressionDock::exportJpeg()
{
    triggerAction(QStringLiteral("file_export_file"));
}
