/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection."
 */

#include "SmartCompressionDock.h"

#include <QAction>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QtMath>
#include <vector>

#include <KConfigGroup>
#include <KSharedConfig>
#include <klocalizedstring.h>
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
    QWidget *content = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    // the two boxes of XAT's window: Work with Whole Image, Work with Regions
    QHBoxLayout *modeRow = new QHBoxLayout();
    m_wholeButton = new QToolButton(content);
    m_wholeButton->setText(i18n("Whole picture"));
    m_wholeButton->setCheckable(true);
    m_wholeButton->setChecked(true);
    m_wholeButton->setAutoExclusive(true);
    m_regionsButton = new QToolButton(content);
    m_regionsButton->setText(i18n("Regions"));
    m_regionsButton->setCheckable(true);
    m_regionsButton->setAutoExclusive(true);
    modeRow->addWidget(m_wholeButton);
    modeRow->addWidget(m_regionsButton);
    layout->addLayout(modeRow);
    connect(m_regionsButton, &QToolButton::toggled, this, &SmartCompressionDock::setRegionsMode);

    // the wand (AutoCompress) and the red cross (Remove All): Smart and Clear
    QHBoxLayout *actionRow = new QHBoxLayout();
    QPushButton *smartButton = new QPushButton(i18n("Smart"), content);
    smartButton->setToolTip(i18n("Select the flat areas to compress; the detail (edges, text, texture) is left outside and kept"));
    QPushButton *clearButton = new QPushButton(i18n("Clear"), content);
    clearButton->setToolTip(i18n("Remove the selection: the whole picture is compressed"));
    actionRow->addWidget(smartButton);
    actionRow->addWidget(clearButton);
    layout->addLayout(actionRow);
    connect(smartButton, &QPushButton::clicked, this, &SmartCompressionDock::makeSmartSelection);
    connect(clearButton, &QPushButton::clicked, this, &SmartCompressionDock::clearSelection);

    // the sensitivity of the Smart Selection
    QHBoxLayout *sensRow = new QHBoxLayout();
    m_sensitivityLabel = new QLabel(content);
    m_sensitivity = new QSlider(Qt::Horizontal, content);
    m_sensitivity->setRange(0, 100);
    m_sensitivity->setValue(KSharedConfig::openConfig()->group("KrimbleSmartCompression").readEntry("sensitivity", 35));
    m_sensitivityLabel->setText(i18n("Sensitivity %1", m_sensitivity->value()));
    sensRow->addWidget(m_sensitivityLabel);
    sensRow->addWidget(m_sensitivity, 1);
    layout->addLayout(sensRow);
    connect(m_sensitivity, &QSlider::valueChanged, this, [this](int v) {
        m_sensitivityLabel->setText(i18n("Sensitivity %1", v));
        KConfigGroup group = KSharedConfig::openConfig()->group("KrimbleSmartCompression");
        group.writeEntry("sensitivity", v);
    });

    // the region tools (only in Regions mode): rectangle, freehand, invert, all
    m_regionTools = new QWidget(content);
    QHBoxLayout *toolsRow = new QHBoxLayout(m_regionTools);
    toolsRow->setContentsMargins(0, 0, 0, 0);
    auto addTool = [&](const QString &text, const char *toolId, const char *actionName) {
        QToolButton *button = new QToolButton(m_regionTools);
        button->setText(text);
        toolsRow->addWidget(button);
        if (toolId) {
            const QString id = QString::fromLatin1(toolId);
            connect(button, &QToolButton::clicked, this, [this, id]() { useTool(id); });
        } else {
            const QString name = QString::fromLatin1(actionName);
            connect(button, &QToolButton::clicked, this, [this, name]() { triggerAction(name); });
        }
    };
    addTool(i18n("Rectangle"), "KisToolSelectRectangular", nullptr);
    addTool(i18n("Freehand"), "KisToolSelectOutline", nullptr);
    addTool(i18n("Invert"), nullptr, "invert_selection");
    addTool(i18n("All"), nullptr, "select_all");
    m_regionTools->hide();
    layout->addWidget(m_regionTools);

    QPushButton *saveButton = new QPushButton(i18n("Save JPEG..."), content);
    layout->addWidget(saveButton);
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
