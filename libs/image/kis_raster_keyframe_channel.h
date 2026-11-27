/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_RASTER_KEYFRAME_CHANNEL_H
#define _KIS_RASTER_KEYFRAME_CHANNEL_H

#include "kis_keyframe_channel.h"

class KRITAIMAGE_EXPORT KisRasterKeyframe : public KisKeyframe
{
    Q_OBJECT
public:
    KisRasterKeyframe(KisPaintDeviceWSP paintDevice);
    KisRasterKeyframe(KisPaintDeviceWSP paintDevice, const int &premadeFrameID, const int &colorLabelId = 0);
    ~KisRasterKeyframe() override;

    int frameID() const;
    bool hasContent();
    QRect contentBounds();

    void writeFrameToDevice(KisPaintDeviceSP writeTarget);

    KisKeyframeSP duplicate(KisKeyframeChannel *newChannel = 0) override;

private:
    KisRasterKeyframe(const KisRasterKeyframe &rhs);

    int m_frameID;
    KisPaintDeviceWSP m_paintDevice;
};

class KRITAIMAGE_EXPORT KisRasterKeyframeChannel : public KisKeyframeChannel
{
    Q_OBJECT
public:
    KisRasterKeyframeChannel(const KoID& id, const KisPaintDeviceWSP paintDevice, const KisDefaultBoundsBaseSP bounds);
    KisRasterKeyframeChannel(const KisRasterKeyframeChannel &rhs, const KisPaintDeviceWSP newPaintDevice);
    ~KisRasterKeyframeChannel() override;

    void writeToDevice(int time, KisPaintDeviceSP targetDevice);

    void importFrame(int time, KisPaintDeviceSP sourceDevice, KUndo2Command *parentCommand);

    QRect frameExtents(KisKeyframeSP keyframe);

    QString frameFilename(int frameId) const;
    void setFilenameSuffix(const QString &suffix);

    QDomElement toXML(QDomDocument doc, const QString &layerFilename) override;
    void loadXML(const QDomElement &channelNode) override;

    void setOnionSkinsEnabled(bool value);
    bool onionSkinsEnabled() const;

    KisPaintDeviceWSP paintDevice();

    virtual void insertKeyframe(int time, KisKeyframeSP keyframe, KUndo2Command *parentUndoCmd = nullptr) override;
    virtual void removeKeyframe(int time, KUndo2Command *parentUndoCmd = nullptr) override;

    void cloneKeyframe(int source, int destination, KUndo2Command *parentUndoCmd = nullptr);
    bool areClones(int timeA, int timeB);
    QSet<int> clonesOf(int time);
    QSet<int> timesForFrameID(int frameID) const;
    static QSet<int> clonesOf(const KisNode *node, int time);

    void makeUnique(int time, KUndo2Command *parentUndoCmd = nullptr);

private:
    QRect affectedRect(int time) const override;

    void saveKeyframe(KisKeyframeSP keyframe, QDomElement keyframeElement, const QString &layerFilename) override;
    QPair<int, KisKeyframeSP> loadKeyframe(const QDomElement &keyframeNode) override;

    KisKeyframeSP createKeyframe() override;

    void setFrameFilename(int frameId, const QString &filename);
    QString chooseFrameFilename(int frameId, const QString &layerFilename);

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
