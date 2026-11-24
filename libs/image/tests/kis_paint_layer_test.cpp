/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_paint_layer_test.h"
#include <simpletest.h>
#include <QImage>
#include <QCoreApplication>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include "kundo2command.h"

#include "kis_group_layer.h"
#include "kis_types.h"
#include "kis_paint_layer.h"
#include "kis_image.h"
#include "kis_paint_device.h"
#include "kis_transparency_mask.h"
#include <testutil.h>
#include "kis_selection.h"
#include "kis_fill_painter.h"
#include "kis_pixel_selection.h"
#include <kis_iterator_ng.h>
#include "kis_layer_projection_plane.h"
#include "kis_psd_layer_style.h"
#include "filter/kis_filter_registry.h"
#include "kis_keyframe_channel.h"
#include "kis_paint_device_debug_utils.h"

void KisPaintLayerTest::testProjection()
{

    QImage qimage(QString(FILES_DATA_DIR) + '/' + "hakonepa.png");
    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->rgb8();
    KisImageSP image = new KisImage(0, qimage.width(), qimage.height(), cs, "merge test");

    KisPaintLayerSP layer = new KisPaintLayer(image, "test", OPACITY_OPAQUE_U8);
    layer->paintDevice()->convertFromQImage(qimage, 0, 0, 0);
    image->addNode(layer.data());

    QVERIFY(layer->paintDevice().data() == layer->projection().data());

    KisTransparencyMaskSP transparencyMask = new KisTransparencyMask(image, "tmask");
    transparencyMask->initSelection(layer);
    transparencyMask->selection()->pixelSelection()->invert();
    image->addNode(transparencyMask.data(), layer.data());

    Q_ASSERT(layer->hasEffectMasks());

    layer->projectionPlane()->recalculate(qimage.rect(), layer, KisRenderPassFlag::None);

    QVERIFY(layer->paintDevice().data() != layer->projection().data());

    layer->setDirty(qimage.rect());

    layer->projectionPlane()->recalculate(qimage.rect(), layer, KisRenderPassFlag::None);

    QVERIFY(layer->paintDevice().data() != layer->projection().data());

    QVERIFY(layer->projection().data() != 0);

    layer->projectionPlane()->recalculate(qimage.rect(), layer, KisRenderPassFlag::None);

    KisSequentialConstIterator it(layer->projection(), qimage.rect());
    while (it.nextPixel()) {
        QVERIFY(cs->opacityU8(it.oldRawData()) == OPACITY_TRANSPARENT_U8);
    };

    transparencyMask->select(qimage.rect());
    transparencyMask->setDirty(qimage.rect());
    image->waitForDone();

    layer->projection()->convertToQImage(0, 0, 0, qimage.width(), qimage.height()).save("aaa.png");
    QPoint errpoint;
    if (!TestUtil::compareQImages(errpoint, qimage, layer->projection()->convertToQImage(0, 0, 0, qimage.width(), qimage.height()))) {
        QFAIL(QString("Failed to create identical image, first different pixel: %1,%2 ").arg(errpoint.x()).arg(errpoint.y()).toLatin1());
    }

}

void KisPaintLayerTest::testKeyframing()
{
    const KoColorSpace *cs = KoColorSpaceRegistry::instance()->rgb8();
    KisImageSP image = new KisImage(0, 512, 512, cs, "");
    KisPaintLayerSP layer = new KisPaintLayer(image, "", OPACITY_OPAQUE_U8);
    KisPaintDeviceSP dev = layer->paintDevice();

    KisKeyframeChannel *contentChannel = layer->getKeyframeChannel(KisKeyframeChannel::Raster.id());
    QVERIFY(!contentChannel);

    layer->enableAnimation();

    contentChannel = layer->getKeyframeChannel("content", true);
    QVERIFY(contentChannel);
    QCOMPARE(contentChannel->keyframeCount(), 1);

    KUndo2Command parentCommand;

    contentChannel->addKeyframe(7, &parentCommand);
    QCOMPARE(contentChannel->keyframeCount(), 2);
    QVERIFY(contentChannel->keyframeAt(0) != contentChannel->keyframeAt(7));

    KisPaintLayerSP layerCopy = new KisPaintLayer(*layer.data());
}

void KisPaintLayerTest::testLayerStyles()
{
    KisFilterRegistry::instance();

    const QRect imageRect(0, 0, 200, 200);
    const QRect rFillRect(10, 10, 100, 100);
    const QRect tMaskRect(50, 50, 20, 20);
    const QRect partialSelectionRect(90, 50, 20, 20);

    const KoColorSpace * cs = KoColorSpaceRegistry::instance()->rgb8();
    KisImageSP image = new KisImage(0, imageRect.width(), imageRect.height(), cs, "styles test");

    KisPaintLayerSP layer = new KisPaintLayer(image, "test", OPACITY_OPAQUE_U8);
    image->addNode(layer);

    layer->paintDevice()->fill(rFillRect, KoColor(Qt::red, cs));

    layer->setDirty();
    image->waitForDone();
    KIS_DUMP_DEVICE_2(layer->projection(), imageRect, "00L_initial", "dd");

    KisPSDLayerStyleSP style(new KisPSDLayerStyle());
    style->dropShadow()->setNoise(30);
    style->dropShadow()->setEffectEnabled(true);

    layer->setLayerStyle(style);

    layer->setDirty();
    image->waitForDone();
    KIS_DUMP_DEVICE_2(image->projection(), imageRect, "02P_styled", "dd");

    KisTransparencyMaskSP transparencyMask = new KisTransparencyMask(image, "tmask");

    KisSelectionSP selection = new KisSelection();
    selection->pixelSelection()->select(tMaskRect, OPACITY_OPAQUE_U8);
    transparencyMask->setSelection(selection);
    image->addNode(transparencyMask, layer);

    layer->setDirty();
    image->waitForDone();
    KIS_DUMP_DEVICE_2(image->projection(), imageRect, "03P_styled", "dd");

    selection->pixelSelection()->select(partialSelectionRect, OPACITY_OPAQUE_U8);
    layer->setDirty(partialSelectionRect);

    layer->setDirty();
    image->waitForDone();
    KIS_DUMP_DEVICE_2(image->projection(), imageRect, "04P_partial", "dd");
}

SIMPLE_TEST_MAIN(KisPaintLayerTest)
