/*
 *  SPDX-FileCopyrightText: 2018 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISFRAMECACHESTORE_H
#define KISFRAMECACHESTORE_H

#include "kritaui_export.h"
#include <QScopedPointer>
#include "kis_types.h"

#include "opengl/kis_texture_tile_info_pool.h"

class KisOpenGLUpdateInfoBuilder;

class KisOpenGLUpdateInfo;
typedef KisSharedPtr<KisOpenGLUpdateInfo> KisOpenGLUpdateInfoSP;

class KRITAUI_EXPORT KisFrameCacheStore
{
public:
    KisFrameCacheStore();
    KisFrameCacheStore(const QString &frameCachePath);

    ~KisFrameCacheStore();

    void saveFrame(int frameId, KisOpenGLUpdateInfoSP info, const QRect &imageBounds);
    KisOpenGLUpdateInfoSP loadFrame(int frameId, const KisOpenGLUpdateInfoBuilder &builder);

    void moveFrame(int srcFrameId, int dstFrameId);

    void forgetFrame(int frameId);
    bool hasFrame(int frameId) const;

    int frameLevelOfDetail(int frameId) const;
    QRect frameDirtyRect(int frameId) const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
