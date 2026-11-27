/*
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PROJECTION_BACKEND
#define KIS_PROJECTION_BACKEND

#include "kis_update_info.h"

class KoColorProfile;
class KisImagePatch;
class KisDisplayFilter;

#include <KoColorConversionTransformation.h>

class KisProjectionBackend
{
public:
    virtual ~KisProjectionBackend();

    virtual void setImage(KisImageWSP image) = 0;
    virtual void setImageSize(qint32 w, qint32 h) = 0;
    virtual void setMonitorProfile(const KoColorProfile* monitorProfile, KoColorConversionTransformation::Intent renderingIntent, KoColorConversionTransformation::ConversionFlags conversionFlags) = 0;
    virtual void setChannelFlags(const QBitArray &channelFlags) = 0;
    virtual void setDisplayFilter(QSharedPointer<KisDisplayFilter> displayFilter) = 0;

    virtual void updateCache(const QRect &dirtyImageRect) = 0;

    virtual void recalculateCache(KisPPUpdateInfoSP info) = 0;

    virtual void alignSourceRect(QRect& rect, qreal scale);

    virtual KisImagePatch getNearestPatch(KisPPUpdateInfoSP info) = 0;

    virtual void drawFromOriginalImage(QPainter& gc,
                                       KisPPUpdateInfoSP info) = 0;
};

#endif
