/*
 *  SPDX-FileCopyrightText: 2022 Eoin O'Neill <eoinoneill1991@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Emmet O'Neill <emmetoneill.pdx@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISFRAMEDISPLAYPROXY_H
#define KISFRAMEDISPLAYPROXY_H

#include <QObject>

#include "kritaui_export.h"

#include "kis_animation_frame_cache.h"

class KRITAUI_EXPORT KisFrameDisplayProxy : public QObject
{
    Q_OBJECT
public:
    KisFrameDisplayProxy(class KisCanvas2 *canvas, QObject *parent = nullptr);
    ~KisFrameDisplayProxy();

    bool displayFrame(int frame, bool forceReproject);

    int activeFrame() const;

Q_SIGNALS:
    void sigFrameChange();
    void sigFrameDisplayRefreshed();

    void sigFrameRefreshSkipped();

private:
    int activeKeyframe() const;

    bool shouldUploadFrame(KisAnimationFrameCacheSP cache, int from, int to);
    bool needsReprojection(KisAnimationFrameCacheSP cache, int from, int to);

    QScopedPointer<struct Private> m_d;
};

#endif
