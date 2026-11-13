/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 55): George (Chatty's ideas, "The first one is pretty."): the SmartCompression panel follows the first mockup.
 */

#include "SmartCompressionDock.h"
#include "SmartCompressionPanel.h"

#include <QFileInfo>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QtMath>
#include <vector>

#include <KConfigGroup>
#include <KSharedConfig>
#include <klocalizedstring.h>
#include <kis_icon_utils.h>
#include <kundo2magicstring.h>

#include <KoCanvasBase.h>
#include <KoToolManager.h>

#include "KisDocument.h"
#include "kis_action.h"
#include "kis_action_manager.h"
#include "kis_canvas2.h"
#include "kis_default_bounds.h"
#include "kis_image.h"
#include "kis_paint_device.h"
#include "kis_pixel_selection.h"
#include "kis_selection.h"
#include "kis_selection_decoration.h"
#include "KisViewManager.h"
#include "tool/kis_selection_tool_helper.h"

#include "KrimbleSmartDetail.h"
#include "KrimbleSmartPreview.h"

SmartCompressionDock::SmartCompressionDock()
    : QDockWidget(i18n("SmartCompression"))
{
    SmartCompressionPanel::Icons icons;
    icons.wand = KisIconUtils::loadIcon("tool_contiguous_selection");
    icons.regions = KisIconUtils::loadIcon("tool_rect_selection");
    icons.rectangle = KisIconUtils::loadIcon("tool_rect_selection");
    icons.freehand = KisIconUtils::loadIcon("tool_outline_selection");
    icons.save = KisIconUtils::loadIcon("document-export");
    {
        const qreal ratio = devicePixelRatioF();
        QPixmap logo(QStringLiteral(":/dark_krimble-smartcompression-logo.png"));
        if (!logo.isNull()) {
            logo = logo.scaledToHeight(qRound(36 * ratio), Qt::SmoothTransformation);
            logo.setDevicePixelRatio(ratio);
        }
        icons.logo = logo;
    }
    m_panel = new SmartCompressionPanel(icons, this);
    setWidget(m_panel);

    KConfigGroup group = KSharedConfig::openConfig()->group("KrimbleSmartCompression");
    m_panel->setQuality(group.readEntry("quality", 85));
    m_panel->setLevel(group.readEntry("level", 35));
    m_panel->setSensitivity(group.readEntry("sensitivity", 35));

    m_previewTimer.setSingleShot(true);
    m_previewTimer.setInterval(600);
    connect(&m_previewTimer, &QTimer::timeout, this, &SmartCompressionDock::updatePreview);

    connect(m_panel, &SmartCompressionPanel::regionsToggled, this, &SmartCompressionDock::onRegionsToggled);
    connect(m_panel, &SmartCompressionPanel::protectionChanged, this, &SmartCompressionDock::onProtectionChanged);
    connect(m_panel, &SmartCompressionPanel::sensitivityChanged, this, &SmartCompressionDock::onSensitivityChanged);
    connect(m_panel, &SmartCompressionPanel::toolRequested, this, &SmartCompressionDock::onSelectionTool);
    connect(m_panel, &SmartCompressionPanel::actionRequested, this, &SmartCompressionDock::onSelectionAction);
    connect(m_panel, &SmartCompressionPanel::settingsChanged, this, &SmartCompressionDock::schedulePreview);
    connect(m_panel, &SmartCompressionPanel::saveRequested, this, &SmartCompressionDock::onSave);
}

void SmartCompressionDock::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);
    m_canvas = dynamic_cast<KisCanvas2 *>(canvas);
    schedulePreview();
}

void SmartCompressionDock::unsetCanvas()
{
    setEnabled(false);
    m_canvas = nullptr;
}

bool SmartCompressionDock::hasSelection() const
{
    return m_canvas && m_canvas->viewManager() && m_canvas->viewManager()->image() && m_canvas->viewManager()->image()->globalSelection();
}

// The picture as 8 bit RGB on white, at most about 1.5 million pixels (a larger picture is shrunk for the preview; its size is then estimated).
static QImage krimblePreviewPicture(KisImageSP image, qreal *factor)
{
    image->waitForDone();
    const QRect bounds = image->bounds();
    const QImage picture = image->projection()->convertToQImage(nullptr, bounds.x(), bounds.y(), bounds.width(), bounds.height());
    *factor = 1.0;
    if (picture.isNull() || picture.width() != bounds.width() || picture.height() != bounds.height()) return QImage();
    const qint64 pixels = qint64(bounds.width()) * bounds.height();
    QImage small = picture;
    if (pixels > 1500000) {
        *factor = qSqrt(1500000.0 / pixels);
        small = picture.scaled(qMax(8, qRound(bounds.width() * *factor)), qMax(8, qRound(bounds.height() * *factor)), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    QImage flat(small.size(), QImage::Format_RGB32);
    flat.fill(Qt::white);
    {
        QPainter painter(&flat);
        painter.drawImage(0, 0, small);
    }
    return flat.convertToFormat(QImage::Format_RGB888);
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
    std::vector<uint8_t> compress = KrimbleSmart::detailMask(luminance.data(), bounds.width(), bounds.height(), m_panel->sensitivity());
    for (uint8_t &value : compress) {
        value = uint8_t(255 - value);   // detail (protected) becomes 0, flat becomes 255 (selected)
    }

    KisPixelSelectionSP selection = new KisPixelSelection(new KisDefaultBounds(image));
    selection->writeBytes(compress.data(), bounds.x(), bounds.y(), bounds.width(), bounds.height());
    KisSelectionToolHelper helper(m_canvas, kundo2_i18n("Smart Selection"));
    helper.selectPixelSelection(selection, SELECTION_REPLACE);
}

// Regions mode: the film (it covers what is NOT selected: the protected area), the region tools, and, with no selection yet, the Smart Selection to start from. The protection is then
// the selection ("Selection"); Whole Picture: no film, the tools are off.
void SmartCompressionDock::onRegionsToggled(bool regions)
{
    m_panel->setToolsEnabled(regions);
    showFilm(regions);
    if (regions) {
        if (!hasSelection()) makeSmartSelection();
        m_panel->setProtect(SmartCompressionPanel::ProtectSelection);
    }
    schedulePreview();
}

// "Smart" chosen while in Regions mode: the selection starts again from the Smart Selection.
void SmartCompressionDock::onProtectionChanged(int protect)
{
    if (protect == SmartCompressionPanel::ProtectSmart && m_panel->isRegions()) makeSmartSelection();
    schedulePreview();
}

void SmartCompressionDock::onSensitivityChanged(int)
{
    if (m_panel->isRegions() && m_panel->protect() == SmartCompressionPanel::ProtectSmart) makeSmartSelection();
}

// A region tool or action was used: the user is editing the selection, so the selection is what protects.
void SmartCompressionDock::onSelectionTool(const QString &toolId)
{
    m_panel->setProtect(SmartCompressionPanel::ProtectSelection);
    KoToolManager::instance()->switchToolRequested(toolId);
}

void SmartCompressionDock::onSelectionAction(const QString &actionName)
{
    m_panel->setProtect(SmartCompressionPanel::ProtectSelection);
    triggerAction(actionName);
    schedulePreview();
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

void SmartCompressionDock::schedulePreview()
{
    if (isVisible()) m_previewTimer.start();
}

// The preview: the JPEG is made in memory with the panel's settings (quality, Extra Color, Grayscale, Progressive, the protection and the level) and read back; the panel shows the sizes
// and the two pictures. Original = the size of the picture's file, or, for a picture that has none, its raw size.
void SmartCompressionDock::updatePreview()
{
    if (!m_canvas || !m_canvas->viewManager() || !m_canvas->viewManager()->image()) return;
    KisImageSP image = m_canvas->viewManager()->image();
    qreal factor = 1.0;
    const QImage flat = krimblePreviewPicture(image, &factor);
    if (flat.isNull()) return;
    const int w = flat.width();
    const int h = flat.height();

    std::vector<uint8_t> rgb(size_t(w) * h * 3);
    std::vector<uint8_t> luminance(size_t(w) * h);
    for (int y = 0; y < h; ++y) {
        const uint8_t *line = flat.constScanLine(y);
        std::copy(line, line + size_t(w) * 3, rgb.begin() + size_t(y) * w * 3);
        for (int x = 0; x < w; ++x) luminance[size_t(y) * w + x] = uint8_t((299 * line[x * 3] + 587 * line[x * 3 + 1] + 114 * line[x * 3 + 2]) / 1000);
    }

    // the mask: above 127 is protected
    std::vector<uint8_t> mask(size_t(w) * h, 0);
    if (m_panel->protect() == SmartCompressionPanel::ProtectSmart) {
        mask = KrimbleSmart::detailMask(luminance.data(), w, h, m_panel->sensitivity());
    } else if (m_panel->protect() == SmartCompressionPanel::ProtectSelection && hasSelection()) {
        const QRect bounds = image->bounds();
        QImage selected(bounds.width(), bounds.height(), QImage::Format_Grayscale8);
        std::vector<uint8_t> bytes(size_t(bounds.width()) * bounds.height());
        image->globalSelection()->pixelSelection()->readBytes(bytes.data(), bounds.x(), bounds.y(), bounds.width(), bounds.height());
        for (int y = 0; y < bounds.height(); ++y) std::copy(bytes.begin() + size_t(y) * bounds.width(), bytes.begin() + size_t(y + 1) * bounds.width(), selected.scanLine(y));
        const QImage small = (w == bounds.width() && h == bounds.height()) ? selected : selected.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        for (int y = 0; y < h; ++y) {
            const uint8_t *line = small.constScanLine(y);
            for (int x = 0; x < w; ++x) mask[size_t(y) * w + x] = uint8_t(255 - line[x]);   // selected = compress; not selected = protected
        }
    }

    std::vector<uint8_t> base;
    std::vector<uint8_t> result;
    if (!KrimbleSmart::encodeJpeg(rgb.data(), w, h, m_panel->quality(), m_panel->extraColor(), m_panel->grayscale(), m_panel->progressive(), base)) return;
    if (!KrimbleSmart::requantize(base.data(), base.size(), mask.data(), w, h, m_panel->level(), m_panel->progressive(), result)) return;

    qint64 originalBytes = 0;
    if (m_canvas->viewManager()->document()) originalBytes = QFileInfo(m_canvas->viewManager()->document()->localFilePath()).size();
    if (originalBytes <= 0) originalBytes = qint64(image->width()) * image->height() * 3;
    const qint64 compressedBytes = qint64(result.size() / (factor * factor));

    std::vector<uint8_t> decoded;
    int dw = 0, dh = 0;
    QPixmap compressedPicture;
    if (KrimbleSmart::decodeJpeg(result, decoded, dw, dh)) {
        const QImage compressedImage(decoded.data(), dw, dh, dw * 3, QImage::Format_RGB888);
        compressedPicture = QPixmap::fromImage(compressedImage.copy());
    }
    const QSize box = m_panel->previewPictureSize();
    const qreal ratio = devicePixelRatioF();
    const QSize target(qMax(16, int((box.width() - 4) * ratio)), qMax(16, int((box.height() - 4) * ratio)));
    QPixmap originalThumb = QPixmap::fromImage(flat).scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap compressedThumb = compressedPicture.isNull() ? QPixmap() : compressedPicture.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    originalThumb.setDevicePixelRatio(ratio);
    compressedThumb.setDevicePixelRatio(ratio);
    m_panel->setPreview(originalThumb, compressedThumb, originalBytes, compressedBytes);
}

// Save JPEG...: the panel's settings are put where the JPEG save dialog finds them (it uses them once), and the export is opened.
void SmartCompressionDock::onSave()
{
    KConfigGroup group = KSharedConfig::openConfig()->group("KrimbleSmartCompression");
    group.writeEntry("quality", m_panel->quality());
    group.writeEntry("level", m_panel->level());
    group.writeEntry("sensitivity", m_panel->sensitivity());
    group.writeEntry("protect", m_panel->protect());
    group.writeEntry("extraColor", m_panel->extraColor());
    group.writeEntry("grayscale", m_panel->grayscale());
    group.writeEntry("progressive", m_panel->progressive());
    group.writeEntry("pending", true);
    group.sync();
    triggerAction(QStringLiteral("file_export_file"));
}
