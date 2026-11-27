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
#include <QCoreApplication>
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
#include "krimble_fill.h"

QRect patchImage(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, int radius, int accuracy, KisSelectionSP selection);

namespace {

#if 0
bool krimbleContentAwareFill(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, KisImageSP image)
{
    const QRect maskRect = maskDev->exactBounds();
    if (maskRect.isEmpty()) return false;
    const int holeShort = qMin(maskRect.width(), maskRect.height());
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
    maskDev->readBytes(maskBytes.data(), roi);
    std::vector<uint8_t> hole(size_t(w) * h);
    size_t holeCount = 0;
    for (size_t i = 0; i < hole.size(); ++i) { hole[i] = maskBytes[i] > 127 ? 1 : 0; holeCount += hole[i]; }
    if (holeCount == 0) return false;

    std::vector<uint8_t> filled(size_t(w) * h, 0);
    KrimbleInpaint::Options options;
    if (!KrimbleInpaint::inpaint(pixels.data(), hole.data(), w, h, 4, options, filled.data())) return false;

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
        if (!KrimbleFill::fill(m_imageDev, m_maskDev, m_image->bounds(), KrimbleFill::savedMethod())) {
            patchImage(m_imageDev, m_maskDev, m_patchRadius, m_accuracy, m_selection);
        }
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
    if (!selection) {
        viewManager()->showFloatingMessage(i18n("Content-Aware Fill: select the area to fill first"), QIcon());
        return;
    }

    KisCursorOverrideLock cursorLock(Qt::WaitCursor);

    if (!viewManager()->blockUntilOperationsFinished(image)) {
        viewManager()->showFloatingMessage(i18n("Content-Aware Fill: the picture is busy, try again"), QIcon());
        return;
    }
    viewManager()->showFloatingMessage(i18n("Content-Aware Fill: working..."), QIcon());
    QCoreApplication::processEvents();

    KisPaintDeviceSP maskDev = selection->pixelSelection();

    const int patchRadius = 4;
    const int accuracy = 50;

    KisProcessingApplicator applicator(image, node, KisProcessingApplicator::NONE,
                                        KisImageSignalVector(), kundo2_i18n("Content-Aware Fill"));

    applicator.applyCommand(new ContentAwareFillCommand(maskDev, node->paintDevice(), accuracy, patchRadius, selection, image),
                             KisStrokeJobData::BARRIER, KisStrokeJobData::EXCLUSIVE);

    applicator.end();
    image->waitForDone();
    {
        const QString status = KrimbleFill::lastStatus();
        const bool engine = status.endsWith(QLatin1String("engine"));
        if (!engine) {
            viewManager()->showFloatingMessage(i18n("Content-Aware Fill: done with Krita's old fill (%1)", status.isEmpty() ? i18n("unknown") : status), QIcon());
        }
    }
}

#include "content_aware_fill_plugin.moc"
