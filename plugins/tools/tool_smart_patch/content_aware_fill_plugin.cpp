/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "content_aware_fill_plugin.h"

#include <kpluginfactory.h>

#include <klocalizedstring.h>
#include <kundo2magicstring.h>
#include <KisViewManager.h>
#include <kis_image.h>
#include <kis_node.h>
#include <kis_selection.h>
#include <kis_pixel_selection.h>
#include <kis_action.h>
#include <kis_processing_applicator.h>
#include <QCoreApplication>   // Krimble 2026-10-20
#include <QIcon>
#include <commands_new/kis_transaction_based_command.h>
#include <kis_transaction.h>
#include <KisCursorOverrideLock.h>

#include <kis_paint_device.h>
#include <kis_image.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <kis_iterator_ng.h>
#include <QImage>
#include <QRect>
#include <vector>
#include <cstring>

#include "krimble_inpaint.h"
#include "krimble_fill.h"   // Krimble 2026-10-19

// Declared in kis_inpaint.cpp -- compiled into this plugin module
// separately from kritatoolSmartPatch's own copy (see CMakeLists.txt);
// each is an independent shared library, so this is a normal, safe
// duplication, not an ODR violation.
QRect patchImage(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, int radius, int accuracy, KisSelectionSP selection);

namespace {

// Krimble 2026-10-18: George: "Can we find a better algorithm?" then "could you write one?" The old fill (Krita's Smart Patch, patchImage() above) leaves "a blurry mess". This
// one is Krimble's own engine (krimble_inpaint.cpp: a multi-scale PatchMatch fill that solves the smooth colour of the hole from its edges at every size and copies the detail from
// patches near the hole; see the 188th entry of KRIMBLE_CHANGES.md). It works on the part of the picture around the selection (the box of the selection and a margin of twice its
// shorter side), as 8-bit sRGB, and writes back only the pixels it replaced. Returns false when it cannot (nothing selected, a hole bigger than the memory allows, the picture too
// small): the caller then uses the old fill.
#if 0   // Krimble 2026-10-19: moved to krimble_fill.cpp (KrimbleFill::fill), with a choice of method; kept here, not compiled
bool krimbleContentAwareFill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, KisImageSP image)
{
    const QRect maskRect = maskDev->exactBounds();
    if (maskRect.isEmpty()) return false;
    const int holeShort = qMin(maskRect.width(), maskRect.height());
    // const int margin = qMax(64, 2 * holeShort + 24);
    // const QRect roi = maskRect.adjusted(-margin, -margin, margin, margin) & image->bounds();
    // if (roi.width() < 24 || roi.height() < 24) return false;
    // if (qint64(roi.width()) * roi.height() > 9000000) return false;      // the memory of the engine is about 70 bytes per pixel of this area
    // Krimble 2026-10-19: George (1.0.39): Content-Aware Fill still "a blurry mess". On a phone photo (12 MP and more) the area above was often over the 9 MP limit, and then
    // Krita's old Smart Patch fill ran instead (the blur). Now the area around the hole is made smaller until it fits a budget (4 MP: the new engine needs about 50 bytes
    // per pixel of it, about 200 MB), so the new engine runs for any hole whose own box fits; only a hole bigger than that still falls back.
    const qint64 budget = 4000000;
    int margin = qMax(64, 2 * holeShort + 24);
    QRect roi = maskRect.adjusted(-margin, -margin, margin, margin) & image->bounds();
    while (margin > 16 && qint64(roi.width()) * roi.height() > budget) {
        margin = margin * 3 / 4;
        roi = maskRect.adjusted(-margin, -margin, margin, margin) & image->bounds();
    }
    if (roi.width() < 24 || roi.height() < 24) return false;
    if (qint64(roi.width()) * roi.height() > budget) return false;

    const int w = roi.width(), h = roi.height();
    const KoColorSpace *rgb8 = KoColorSpaceRegistry::instance()->rgb8();
    QImage img = imageDev->convertToQImage(rgb8->profile(), roi.x(), roi.y(), w, h).convertToFormat(QImage::Format_ARGB32);
    if (img.isNull()) return false;

    std::vector<uint8_t> pixels(size_t(w) * h * 4);
    for (int y = 0; y < h; ++y) std::memcpy(&pixels[size_t(y) * w * 4], img.constScanLine(y), size_t(w) * 4);

    std::vector<uint8_t> maskBytes(size_t(w) * h);
    maskDev->readBytes(maskBytes.data(), roi);                            // the selection is an 8-bit alpha device: one byte per pixel
    std::vector<uint8_t> hole(size_t(w) * h);
    size_t holeCount = 0;
    for (size_t i = 0; i < hole.size(); ++i) { hole[i] = maskBytes[i] > 127 ? 1 : 0; holeCount += hole[i]; }
    if (holeCount == 0) return false;

    std::vector<uint8_t> filled(size_t(w) * h, 0);
    KrimbleInpaint::Options options;
    if (!KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data())) return false;

    // the result, in the colour space of the layer, and only the replaced pixels copied
    QImage out(w, h, QImage::Format_ARGB32);
    for (int y = 0; y < h; ++y) std::memcpy(out.scanLine(y), &pixels[size_t(y) * w * 4], size_t(w) * 4);
    KisPaintDeviceSP result = new KisPaintDevice(imageDev->colorSpace());
    result->convertFromQImage(out, rgb8->profile(), roi.x(), roi.y());
    const int pixelSize = imageDev->pixelSize();
    for (int y = 0; y < h; ++y) {
        KisHLineIteratorSP dst = imageDev->createHLineIteratorNG(roi.x(), roi.y() + y, w);
        KisHLineConstIteratorSP src = result->createHLineConstIteratorNG(roi.x(), roi.y() + y, w);
        int x = 0;
        do {
            if (filled[size_t(y) * w + x]) std::memcpy(dst->rawData(), src->rawDataConst(), size_t(pixelSize));
            ++x;
        } while (dst->nextPixel() && src->nextPixel());
    }
    return true;
}
#endif

// Krimble: same tiny transaction-wrapping command shape as
// KisToolSmartPatch::InpaintCommand in kis_tool_smart_patch.cpp -- kept as
// its own local copy here rather than reused across the tool/action
// boundary, since the tool's version is a private nested class.
class ContentAwareFillCommand : public KisTransactionBasedCommand
{
public:
    ContentAwareFillCommand(KisPaintDeviceSP maskDev, KisPaintDeviceSP imageDev, int accuracy, int patchRadius, KisSelectionSP selection, KisImageSP image)
        : m_maskDev(maskDev), m_imageDev(imageDev), m_accuracy(accuracy), m_patchRadius(patchRadius), m_selection(selection), m_image(image)
    {
    }

    KUndo2Command *paint() override
    {
        KisTransaction transaction(m_imageDev);
        // Krimble 2026-10-18: the new engine first; Krita's own Smart Patch fill is the fallback (when the engine cannot take the job: see krimbleContentAwareFill)
        // if (!krimbleContentAwareFill(m_imageDev, m_maskDev, m_image)) {
        // Krimble 2026-10-19: the method chosen in the healing brush's Tool Options (Sharp, Smooth or Classic); Classic, or a job Krimble's engines cannot take, uses Krita's fill
        if (!KrimbleFill::fill(m_imageDev, m_maskDev, m_image->bounds(), KrimbleFill::savedMethod())) {
            patchImage(m_imageDev, m_maskDev, m_patchRadius, m_accuracy, m_selection);
        }
        // patchImage(m_imageDev, m_maskDev, m_patchRadius, m_accuracy, m_selection);
        return transaction.endAndTake();
    }

private:
    KisPaintDeviceSP m_maskDev, m_imageDev;
    int m_accuracy, m_patchRadius;
    KisSelectionSP m_selection;
    KisImageSP m_image;
};

}

K_PLUGIN_FACTORY_WITH_JSON(ContentAwareFillPluginFactory, "contentawarefill.json", registerPlugin<ContentAwareFillPlugin>();)

ContentAwareFillPlugin::ContentAwareFillPlugin(QObject *parent, const QVariantList &)
    : KisActionPlugin(parent)
{
    KisAction *action = createAction("content_aware_fill");
    connect(action, SIGNAL(triggered()), this, SLOT(slotContentAwareFill()));
}

ContentAwareFillPlugin::~ContentAwareFillPlugin()
{
}

void ContentAwareFillPlugin::slotContentAwareFill()
{
    KisImageSP image = viewManager()->image();
    if (!image) return;

    KisNodeSP node = viewManager()->activeNode();
    if (!node || !node->paintDevice()) return;

    KisSelectionSP selection = viewManager()->selection();
    // if (!selection) return; // nothing to fill without an active selection
    // Krimble 2026-10-20: George (1.0.40): "Content-Aware fill does nothing now." Every way it could quietly do nothing now says so, and the fill says
    // it is working (a big hole can take several seconds on the phone) and when it is done.
    if (!selection) {
        viewManager()->showFloatingMessage(i18n("Content-Aware Fill: select the area to fill first"), QIcon());
        return;
    }

    KisCursorOverrideLock cursorLock(Qt::WaitCursor);

    // if (!viewManager()->blockUntilOperationsFinished(image)) return;
    if (!viewManager()->blockUntilOperationsFinished(image)) {
        viewManager()->showFloatingMessage(i18n("Content-Aware Fill: the picture is busy, try again"), QIcon());
        return;
    }
    viewManager()->showFloatingMessage(i18n("Content-Aware Fill: working..."), QIcon());
    QCoreApplication::processEvents();

    // Krimble: unlike the sloppy KisFilter-based first pass, this passes
    // the selection's own real pixel mask straight through -- correct for
    // irregular/feathered selections, since patchImage() gets to see the
    // true selected shape while deciding what to fill vs. what's valid
    // source material, not just a rectangular bounding-box approximation.
    KisPaintDeviceSP maskDev = selection->pixelSelection();

    const int patchRadius = 4; // matches the Healing Brush tool's own default
    const int accuracy = 50;   // matches the Healing Brush tool's own default

    KisProcessingApplicator applicator(image, node, KisProcessingApplicator::NONE,
                                        KisImageSignalVector(), kundo2_i18n("Content-Aware Fill"));

    applicator.applyCommand(new ContentAwareFillCommand(maskDev, node->paintDevice(), accuracy, patchRadius, selection, image),
                             KisStrokeJobData::BARRIER, KisStrokeJobData::EXCLUSIVE);

    applicator.end();
    image->waitForDone();
    viewManager()->showFloatingMessage(i18n("Content-Aware Fill: done"), QIcon());   // Krimble 2026-10-20
}

#include "content_aware_fill_plugin.moc"
