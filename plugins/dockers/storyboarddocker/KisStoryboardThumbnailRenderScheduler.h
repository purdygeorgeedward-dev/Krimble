/*
 *  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISASYNCSTORYBOARDTHUMBNAILRENDERSCHEDULER_H
#define KISASYNCSTORYBOARDTHUMBNAILRENDERSCHEDULER_H

#include <QObject>
#include <QVector>

#include <kis_image.h>

#include <kis_paint_device.h>
class KisPaintDevice;
class KisAsyncStoryboardThumbnailRenderer;

class KisStoryboardThumbnailRenderScheduler : public QObject
{
    Q_OBJECT
public:
    KisStoryboardThumbnailRenderScheduler(QObject *parent);
    ~KisStoryboardThumbnailRenderScheduler();

    void setImage(KisImageSP image);
    void scheduleFrameForRegeneration(int frame, bool affected);
    void cancelAllFrameRendering();
    void cancelFrameRendering(int frame);

public Q_SLOTS:
    void slotStartFrameRendering();

private Q_SLOTS:
    void slotFrameRegenerationCompleted(int frame, KisPaintDeviceSP contents);

    void slotFrameRegenerationCancelled(int frame);

private:
    void sortAffectedFrameQueue();

    void renderNextFrame();

Q_SIGNALS:
    void sigFrameCompleted(int frame, KisPaintDeviceSP dev);
    void sigFrameCancelled(int frame);

private:
    QVector<int> m_changedFramesQueue;
    QVector<int> m_affectedFramesQueue;
    KisAsyncStoryboardThumbnailRenderer *m_renderer;
    KisImageSP m_image;
    int m_currentFrame;
};

#endif
