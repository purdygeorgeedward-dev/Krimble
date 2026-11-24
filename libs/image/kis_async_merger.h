/* SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KIS_ASYNC_MERGER_H
#define __KIS_ASYNC_MERGER_H

#include "kritaimage_export.h"
#include "kis_types.h"
#include "KisRenderPassFlags.h"

class QRect;
class KisBaseRectsWalker;

class KRITAIMAGE_EXPORT KisAsyncMerger
{
public:
    void startMerge(KisBaseRectsWalker &walker, bool notifyClones = true);

private:
    inline void resetProjection();
    inline void setupProjection(KisProjectionLeafSP currentLeaf, const QRect& rect, bool useTempProjection);
    inline void writeProjection(KisProjectionLeafSP topmostLeaf, bool useTempProjection, const QRect &rect);
    inline bool compositeWithProjection(KisProjectionLeafSP leaf, const QRect &rect);
    inline void doNotifyClones(KisBaseRectsWalker &walker);

private:
    KisPaintDeviceSP m_currentProjection;

    KisPaintDeviceSP m_finalProjection;

    KisPaintDeviceSP m_cachedPaintDevice;
};

#endif

