/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "channelmodel.h"
#include <QImage>
#include <KoColorSpace.h>
#include <KoChannelInfo.h>
#include <KoColorModelStandardIds.h>

#include <kis_painter.h>

#include <kis_group_layer.h>
#include <kis_paint_device.h>
#include <kis_iterator_ng.h>
#include <kis_default_bounds.h>

#include <kis_canvas2.h>
#include <kis_image.h>
#include <kis_selection.h>
#include <kis_pixel_selection.h>
#include <kis_processing_applicator.h>
#include <commands/kis_set_global_selection_command.h>
#include <kundo2magicstring.h>
#include <KLocalizedString>
#include <QVector>

ChannelModel::ChannelModel(QObject* parent):
    QAbstractTableModel(parent),
    m_canvas(nullptr),
    m_channelCount(0)
{
    setThumbnailSizeLimit(QSize(64, 64));
}

ChannelModel::~ChannelModel()
{
}

QVariant ChannelModel::data(const QModelIndex& index, int role) const
{
    if (m_canvas && m_canvas->image() && index.isValid()) {
        KisGroupLayerSP rootLayer = m_canvas->image()->rootLayer();
        const KoColorSpace *cs = rootLayer->colorSpace();
        if (cs->channelCount() != m_channelCount) return QVariant();

        const QList<KoChannelInfo*> channels = cs->channels();

        int channelIndex = index.row();

        if (index.row() < cs->channelCount()) {

            switch (role) {
            case Qt::DisplayRole: {
                if (index.column() == 2) {
                    return channels.at(channelIndex)->name();
                }
                return QVariant();
            }
            case Qt::DecorationRole: {
                if (index.column() == 1 &&
                        !m_thumbnails.isEmpty() &&
                        index.row() < m_thumbnails.size()) {

                    return QVariant(m_thumbnails.at(index.row()));
                }
                return QVariant();
            }
            case Qt::CheckStateRole: {
                Q_ASSERT(index.row() < rowCount());
                Q_ASSERT(index.column() < columnCount());

                if (index.column() == 0) {
                    QBitArray flags = rootLayer->channelFlags();
                    return (flags.isEmpty() || flags.testBit(channelIndex)) ? Qt::Checked : Qt::Unchecked;
                }
                return QVariant();
            }
            }
        }
    }
    return QVariant();
}

QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section); Q_UNUSED(orientation); Q_UNUSED(role);
    return QVariant();
}

int ChannelModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (!m_canvas || !m_canvas->image()) return 0;

    return m_channelCount;
}

int ChannelModel::columnCount(const QModelIndex& /*parent*/) const
{
    if (!m_canvas) return 0;

    //columns are: checkbox, thumbnail, channel name
    return 3;
}

bool ChannelModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (m_canvas && m_canvas->image()) {
        KisGroupLayerSP rootLayer = m_canvas->image()->rootLayer();
        const KoColorSpace *cs = rootLayer->colorSpace();
        if (cs->channelCount() != m_channelCount) return false;

        const QList<KoChannelInfo*> channels = cs->channels();
        KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(index.row() <= channels.count(), false);

        int channelIndex = index.row();

        if (role == Qt::CheckStateRole) {
            QBitArray flags = rootLayer->channelFlags();
            flags = flags.isEmpty() ? cs->channelFlags(true, true) : flags;
            Q_ASSERT(!flags.isEmpty());

            flags.setBit(channelIndex, value.toInt() == Qt::Checked);
            rootLayer->setChannelFlags(flags);

            Q_EMIT channelFlagsChanged();
            emitAllDataChanged(channels.count() - 1, 0);
            return true;
        }
    }
    return false;
}

//User double clicked on a row (but on channel checkbox)
//we select this channel, and deselect all other channels (except alpha, which we don't touch)
//this makes it fast to select single color channel
void ChannelModel::rowActivated(const QModelIndex &index)
{
    if (m_canvas && m_canvas->image()) {
        KisGroupLayerWSP rootLayer = m_canvas->image()->rootLayer();
        const KoColorSpace* cs = rootLayer->colorSpace();
        if (cs->channelCount() != m_channelCount) return;

        const QList<KoChannelInfo*> channels = cs->channels();
        Q_ASSERT(index.row() <= channels.count());

        int channelIndex = index.row();

        QBitArray flags = rootLayer->channelFlags();
        flags = flags.isEmpty() ? cs->channelFlags(true, true) : flags;
        Q_ASSERT(!flags.isEmpty());

        for (int i = 0; i < channels.count(); ++i) {
            if (channels[i]->channelType() != KoChannelInfo::ALPHA) {
                flags.setBit(i, (i == channelIndex));
            }
        }

        rootLayer->setChannelFlags(flags);

        Q_EMIT channelFlagsChanged();
        emitAllDataChanged(channels.count() - 1, 0);
    }
}


// Krimble 2026-11-13: only the channel at channelIndex is shown (flags of the other color channels are cleared, the alpha channel is left as it is): what a double click does (rowActivated)
void ChannelModel::isolateChannel(int channelIndex)
{
    if (!m_canvas || !m_canvas->image()) return;
    KisGroupLayerWSP rootLayer = m_canvas->image()->rootLayer();
    const KoColorSpace *cs = rootLayer->colorSpace();
    if (cs->channelCount() != m_channelCount) return;
    const QList<KoChannelInfo*> channels = cs->channels();
    if (channelIndex < 0 || channelIndex >= channels.count()) return;
    QBitArray flags = rootLayer->channelFlags();
    flags = flags.isEmpty() ? cs->channelFlags(true, true) : flags;
    for (int i = 0; i < channels.count(); ++i) {
        if (channels[i]->channelType() != KoChannelInfo::ALPHA) flags.setBit(i, (i == channelIndex));
    }
    rootLayer->setChannelFlags(flags);
    Q_EMIT channelFlagsChanged();
    emitAllDataChanged(channels.count() - 1, 0);
}

// Krimble 2026-11-13: every channel is shown again (all flags set)
void ChannelModel::showAllChannels()
{
    if (!m_canvas || !m_canvas->image()) return;
    KisGroupLayerWSP rootLayer = m_canvas->image()->rootLayer();
    const KoColorSpace *cs = rootLayer->colorSpace();
    if (cs->channelCount() != m_channelCount) return;
    rootLayer->setChannelFlags(cs->channelFlags(true, true));
    Q_EMIT channelFlagsChanged();
    emitAllDataChanged(cs->channels().count() - 1, 0);
}

// Krimble 2026-11-13: the channel's values, read from the picture as it looks (the projection of all layers), become the selection: a pixel is selected as much as the channel is light (0 to 255).
// The picture is read in one go on the main thread; a very large picture can take a moment.
void ChannelModel::loadChannelAsSelection(int channelIndex)
{
    if (!m_canvas || !m_canvas->image()) return;
    KisImageSP image = m_canvas->image();
    const KoColorSpace *cs = image->rootLayer()->colorSpace();
    if (cs->channelCount() != m_channelCount || channelIndex < 0 || channelIndex >= cs->channelCount()) return;
    const QRect bounds = image->bounds();
    image->barrierLock();
    KisPaintDeviceSP projection = image->projection();
    KisSelectionSP selection = new KisSelection();
    KisPixelSelectionSP pixels = selection->pixelSelection();
    QVector<float> values(cs->channelCount());
    for (int y = bounds.top(); y <= bounds.bottom(); ++y) {
        KisHLineConstIteratorSP source = projection->createHLineConstIteratorNG(bounds.left(), y, bounds.width());
        KisHLineIteratorSP target = pixels->createHLineIteratorNG(bounds.left(), y, bounds.width());
        do {
            cs->normalisedChannelsValue(source->oldRawData(), values);
            target->rawData()[0] = quint8(qBound(0, int(values[channelIndex] * 255.0f + 0.5f), 255));
        } while (source->nextPixel() && target->nextPixel());
    }
    image->unlock();
    pixels->invalidateOutlineCache();
    KisProcessingApplicator applicator(image, 0, KisProcessingApplicator::NONE, KisImageSignalVector(), kundo2_i18n("Load Channel as Selection"));
    applicator.applyCommand(new KisSetGlobalSelectionCommand(image, selection), KisStrokeJobData::SEQUENTIAL, KisStrokeJobData::EXCLUSIVE);
    applicator.end();
}

Qt::ItemFlags ChannelModel::flags(const QModelIndex& /*index*/) const
{
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    return flags;
}

void ChannelModel::setCanvas(KisCanvas2 *canvas)
{
    m_canvasConnections.clear();

    m_canvas = canvas;

    if (m_canvas) {
        m_canvasConnections.addConnection(m_canvas->image(),
                                          SIGNAL(sigColorSpaceChanged(const KoColorSpace*)),
                                          this,
                                          SLOT(slotColorSpaceChanged(const KoColorSpace*)));
    }
}

void ChannelModel::setChannelThumbnails(const QVector<QImage> &channels, const KoColorSpace *cs)
{
    if (m_canvas) {
        KisGroupLayerWSP rootLayer = m_canvas->image()->rootLayer();

        if (!cs || *rootLayer->colorSpace() == *cs) {
            const int newChannelCount = cs ? cs->channelCount() : 0;


            if (newChannelCount != m_channelCount) {
                beginResetModel();
                m_thumbnails = channels;
                m_channelCount = newChannelCount;
                endResetModel();
            } else {
                m_thumbnails = channels;
                emitAllDataChanged(channels.count() - 1, columnCount() - 1);
            }
        }
    }
}

void ChannelModel::slotColorSpaceChanged(const KoColorSpace *colorSpace)
{
    setChannelThumbnails({}, colorSpace);
}

void ChannelModel::setThumbnailSizeLimit(QSize size)
{
    m_thumbnailSizeLimit = size;
}

QSize ChannelModel::thumbnailSizeLimit() const
{
    return m_thumbnailSizeLimit;
}

void ChannelModel::emitAllDataChanged(int bottomRow, int rightColumn)
{
    if (bottomRow >= 0 && rightColumn >= 0) {
        int rows = rowCount();
        int cols = columnCount();
        if (rows > 0 && cols > 0) {
            Q_EMIT dataChanged(this->index(0, 0), this->index(qMin(bottomRow, rows - 1), qMin(rightColumn, cols - 1)));
        }
    }
}

#include "moc_channelmodel.cpp"
