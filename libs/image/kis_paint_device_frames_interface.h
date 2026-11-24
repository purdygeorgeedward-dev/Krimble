/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINT_DEVICE_FRAMES_INTERFACE_H
#define __KIS_PAINT_DEVICE_FRAMES_INTERFACE_H

#include "kis_types.h"
#include "kritaimage_export.h"

class KisPaintDeviceData;
class KisPaintDeviceWriter;
class KisDataManager;
typedef KisSharedPtr<KisDataManager> KisDataManagerSP;

class KisInterstrokeData;
using KisInterstrokeDataSP = QSharedPointer<KisInterstrokeData>;

class KRITAIMAGE_EXPORT KisPaintDeviceFramesInterface
{
public:
    KisPaintDeviceFramesInterface(KisPaintDevice *parentDevice);

    QList<int> frames();

    int createFrame(bool copy, int copySrc, const QPoint &offset, KUndo2Command *parentCommand);

    void deleteFrame(int frame, KUndo2Command *parentCommand);

    void writeFrameToDevice(int frameId, KisPaintDeviceSP targetDevice);

    void uploadFrame(int srcFrameId, int dstFrameId, KisPaintDeviceSP srcDevice);

    void uploadFrame(int dstFrameId, KisPaintDeviceSP srcDevice);

    QRect frameBounds(int frameId);

    QPoint frameOffset(int frameId) const;

    void setFrameDefaultPixel(const KoColor &defPixel, int frameId);

    KoColor frameDefaultPixel(int frameId) const;

    bool writeFrame(KisPaintDeviceWriter &store, int frameId);

    bool readFrame(QIODevice *stream, int frameId);

    int currentFrameId() const;

    KisDataManagerSP frameDataManager(int frameId) const;

    void invalidateFrameCache(int frameId);

    void setFrameOffset(int frameId, const QPoint &offset);

    struct TestingDataObjects {
        typedef KisPaintDeviceData Data;
        typedef QHash<int, Data*> FramesHash;

        Data *m_data;
        Data *m_lodData;
        Data *m_externalFrameData;

        FramesHash m_frames;

        Data *m_currentData;
    };

    TestingDataObjects testingGetDataObjects() const;
    QList<KisPaintDeviceData*> testingGetDataObjectsList() const;

private:
    KisPaintDevice *q;
};

#endif
