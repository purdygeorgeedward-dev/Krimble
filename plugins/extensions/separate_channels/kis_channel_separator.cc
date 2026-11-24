/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2005 Michael Thaler <michael.thaler@physik.tu-muenchen.de>
 *
 * ported from Gimp, SPDX-FileCopyrightText: 1997 Eiichi Takamori <taka@ma1.seikyou.ne.jp>
 * original pixelize.c for GIMP 0.54 by Tracy Scott
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_channel_separator.h"

#include <limits.h>
#include <stdlib.h>
#include <vector>

#include <QStandardPaths>

#include <klocalizedstring.h>
#include <kis_debug.h>
#include <kpluginfactory.h>

#include <KisImportExportManager.h>
#include <KoUpdater.h>
#include <KoFileDialog.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoChannelInfo.h>
#include <KoColorModelStandardIds.h>

#include <KisDocument.h>
#include <kis_image.h>
#include <kis_layer.h>
#include <kis_paint_layer.h>
#include <kis_group_layer.h>
#include <kis_transaction.h>
#include <kis_undo_adapter.h>
#include <kis_global.h>
#include <kis_types.h>
#include "kis_iterator_ng.h"
#include <KisPart.h>
#include <KisViewManager.h>
#include <kis_paint_device.h>
#include <kis_node_manager.h>
#include <kis_node_commands_adapter.h>
#include <KisMimeDatabase.h>
#include "KisImageBarrierLock.h"

KisChannelSeparator::KisChannelSeparator(KisViewManager * view)
    : m_viewManager(view)
{
}

void KisChannelSeparator::separate(KoUpdater * progressUpdater, enumSepAlphaOptions alphaOps, enumSepSource sourceOps, bool downscale, bool toColor, bool activateCurrentChannel)
{
    KisImageSP image = m_viewManager->image();
    if (!image) return;

    KisPaintDeviceSP src;

    switch (sourceOps) {
    case ALL_LAYERS:
        src = image->projection();
        break;
    case CURRENT_LAYER:
        src = m_viewManager->activeDevice();
        break;
    default:
        break;
    }

    if (!src) return;

    progressUpdater->setProgress(1);

    const KoColorSpace * dstCs = 0;

    quint32 numberOfChannels = src->channelCount();
    const KoColorSpace * srcCs  = src->colorSpace();
    const QList<KoChannelInfo *> channels = srcCs->channels();
    vKisPaintDeviceSP paintDevices;

    const QRect rect = image->bounds();

    KisImageBarrierLock lock(image);
    int i = 0;
    for (QList<KoChannelInfo *>::const_iterator it =  channels.constBegin(); it != channels.constEnd(); ++it) {

        KoChannelInfo *ch = (*it);

        bool channelToColor = (toColor && ch->channelType() != KoChannelInfo::ALPHA);

        if (ch->channelType() == KoChannelInfo::ALPHA && alphaOps != CREATE_ALPHA_SEPARATION) {
            continue;
        }

        qint32 channelSize = ch->size();
        qint32 channelPos = ch->pos();
        qint32 destSize = 1;

        KisPaintDeviceSP dev;
        if (channelToColor) {
            dev = new KisPaintDevice(srcCs);
        } else {
            if (channelSize == 1 || downscale) {
                dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->colorSpace(GrayAColorModelID.id(), Integer8BitsColorDepthID.id(), 0));
            } else {
                dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->colorSpace(GrayAColorModelID.id(), Integer16BitsColorDepthID.id(), 0));
                destSize = 2;
            }
        }

        dstCs = dev->colorSpace();

        paintDevices.push_back(dev);

        KisHLineConstIteratorSP srcIt = src->createHLineConstIteratorNG(rect.x(), rect.y(), rect.width());
        KisHLineIteratorSP dstIt = dev->createHLineIteratorNG(rect.x(), rect.y(), rect.width());

        for (qint32 row = 0; row < rect.height(); ++row) {
            do {
                if (channelToColor) {
                    dstCs->singleChannelPixel(dstIt->rawData(), srcIt->oldRawData(), channelPos);

                    if (alphaOps == COPY_ALPHA_TO_SEPARATIONS) {
                        dstCs->setOpacity(dstIt->rawData(), srcCs->opacityU8(srcIt->oldRawData()), 1);
                    } else {
                        dstCs->setOpacity(dstIt->rawData(), OPACITY_OPAQUE_U8, 1);
                    }
                } else {

                    if (channelSize == 1 && destSize == 1) {

                        dstIt->rawData()[0] = srcIt->oldRawData()[channelPos];

                        if (alphaOps == COPY_ALPHA_TO_SEPARATIONS) {
                            dstCs->setOpacity(dstIt->rawData(), srcCs->opacityU8(srcIt->oldRawData()), 1);
                        } else {
                            dstCs->setOpacity(dstIt->rawData(), OPACITY_OPAQUE_U8, 1);
                        }
                    } else if (channelSize == 2 && destSize == 2) {

                        dstIt->rawData()[0] = srcIt->oldRawData()[channelPos];
                        dstIt->rawData()[1] = srcIt->oldRawData()[channelPos + 1];

                        if (alphaOps == COPY_ALPHA_TO_SEPARATIONS) {
                            dstCs->setOpacity(dstIt->rawData(), srcCs->opacityU8(srcIt->oldRawData()), 1);
                        } else {
                            dstCs->setOpacity(dstIt->rawData(), OPACITY_OPAQUE_U8, 1);
                        }
                    } else if (channelSize != 1 && destSize == 1) {
                        memset(dstIt->rawData(), srcCs->scaleToU8(srcIt->oldRawData(), channelPos), 1);

                        dstCs->setOpacity(dstIt->rawData(), OPACITY_OPAQUE_U8, 1);
                    } else if (channelSize != 2 && destSize == 2) {
                        dstIt->rawData()[0] = srcCs->scaleToU8(srcIt->oldRawData(), channelPos);

                        dstCs->setOpacity(dstIt->rawData(), OPACITY_OPAQUE_U8, 1);

                    }
                }

            } while (dstIt->nextPixel() && srcIt->nextPixel());
            dstIt->nextRow();
            srcIt->nextRow();
        }
        ++i;

        progressUpdater->setProgress((i * 100) / numberOfChannels);
        if (progressUpdater->interrupted()) {
            break;
        }
    }

    vKisPaintDeviceSP::const_iterator paintDeviceIterator = paintDevices.cbegin();

    if (!progressUpdater->interrupted()) {
        KisNodeCommandsAdapter adapter(m_viewManager);
        adapter.beginMacro(kundo2_i18n("Separate Image"));

        for (QList<KoChannelInfo *>::const_iterator it =  channels.constBegin(); it != channels.constEnd(); ++it) {

            KoChannelInfo *ch = (*it);
            if (ch->channelType() == KoChannelInfo::ALPHA && alphaOps != CREATE_ALPHA_SEPARATION) {
                continue;
            }

            KisPaintLayerSP l = new KisPaintLayer(image, ch->name(), OPACITY_OPAQUE_U8, *paintDeviceIterator);

            if (toColor && ch->channelType() != KoChannelInfo::ALPHA && activateCurrentChannel) {
                QBitArray channelFlags(channels.count());
                int i = 0;
                for (QList<KoChannelInfo *>::const_iterator it2 =  channels.constBegin(); it2 != channels.constEnd(); ++it2) {
                    channelFlags.setBit(i, (it == it2));
                    i++;
                }
                l->setChannelFlags(channelFlags);
            }

            adapter.addNode(l.data(), image->rootLayer(), 0);
            ++paintDeviceIterator;
        }

        adapter.endMacro();
    }

    progressUpdater->setProgress(100);

}
