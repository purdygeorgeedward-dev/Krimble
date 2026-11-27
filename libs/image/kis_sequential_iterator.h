/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SEQUENTIAL_ITERATOR_H
#define __KIS_SEQUENTIAL_ITERATOR_H

#include <KoAlwaysInline.h>

#include "kis_types.h"
#include "kis_paint_device.h"
#include "kis_iterator_ng.h"

struct DevicePolicy {
    DevicePolicy(KisPaintDeviceSP dev) : m_dev(dev) {}

    template <typename Convertible>
    DevicePolicy(Convertible sel) : m_dev(sel) {}

    KisHLineConstIteratorSP createConstIterator(const QRect &rect) {
        return m_dev->createHLineConstIteratorNG(rect.x(), rect.y(), rect.width());
    }

    KisHLineIteratorSP createIterator(const QRect &rect) {
        return m_dev->createHLineIteratorNG(rect.x(), rect.y(), rect.width());
    }

    int pixelSize() const {
        return m_dev->pixelSize();
    }

    KisPaintDeviceSP m_dev;
};

template <class SourcePolicy = DevicePolicy>
struct ReadOnlyIteratorPolicy {
    typedef KisHLineConstIteratorSP IteratorTypeSP;

    ReadOnlyIteratorPolicy(SourcePolicy source, const QRect &rect) {
        m_iter = !rect.isEmpty() ? source.createConstIterator(rect) : 0;
    }

    ALWAYS_INLINE void updatePointersCache() {
        m_rawDataConst = m_iter ? m_iter->rawDataConst() : 0;
        m_oldRawData = m_iter ? m_iter->oldRawData() : 0;
    }

    ALWAYS_INLINE const quint8* rawDataConst() const {
        return m_rawDataConst;
    }

    ALWAYS_INLINE const quint8* oldRawData() const {
        return m_oldRawData;
    }

    IteratorTypeSP m_iter;

private:
    const quint8 *m_rawDataConst {nullptr};
    const quint8 *m_oldRawData {nullptr};
};

template <class SourcePolicy = DevicePolicy>
struct WritableIteratorPolicy {
    typedef KisHLineIteratorSP IteratorTypeSP;

    WritableIteratorPolicy(SourcePolicy source, const QRect &rect) {
        m_iter = !rect.isEmpty() ? source.createIterator(rect) : 0;
    }

    ALWAYS_INLINE void updatePointersCache() {
        m_rawData = m_iter ? m_iter->rawData() : 0;
        m_oldRawData = m_iter ? m_iter->oldRawData() : 0;
    }

    ALWAYS_INLINE quint8* rawData() {
        return m_rawData;
    }

    ALWAYS_INLINE const quint8* rawDataConst() const {
        return m_rawData;
    }

    ALWAYS_INLINE const quint8* oldRawData() const {
        return m_oldRawData;
    }

    IteratorTypeSP m_iter;

private:
    quint8 *m_rawData {nullptr};
    const quint8 *m_oldRawData {nullptr};
};

struct NoProgressPolicy
{
    ALWAYS_INLINE void setRange(int , int )
    {
    }

    ALWAYS_INLINE void setValue(int )
    {
    }

    ALWAYS_INLINE void setFinished()
    {
    }
};

template <class IteratorPolicy, class SourcePolicy = DevicePolicy, class ProgressPolicy = NoProgressPolicy>
class KisSequentialIteratorBase
{
public:
    KisSequentialIteratorBase(SourcePolicy source, const QRect &rect, ProgressPolicy progressPolicy = ProgressPolicy())
        : m_policy(source, rect),
          m_progressPolicy(progressPolicy),
          m_pixelSize(source.pixelSize()),
          m_rowsLeft(rect.height() - 1),
          m_columnOffset(0),
          m_iteratorX(0),
          m_iteratorY(0),
          m_isStarted(false)
    {
        m_columnsLeft = m_numConseqPixels =
            m_policy.m_iter ? m_policy.m_iter->nConseqPixels() : 0;

        m_policy.updatePointersCache();
        m_iteratorX = m_policy.m_iter ? m_policy.m_iter->x() : 0;
        m_iteratorY = m_policy.m_iter ? m_policy.m_iter->y() : 0;

        m_progressPolicy.setRange(rect.top(), rect.top() + rect.height());
        m_progressPolicy.setValue(rect.top());
    }

    ~KisSequentialIteratorBase() {
        m_progressPolicy.setFinished();
    }

    inline int nConseqPixels() const {
        return m_isStarted ? m_columnsLeft : 1;
    }

    inline bool nextPixels(int numPixels) {
        numPixels--;

        m_columnsLeft -= numPixels;
        m_columnOffset += numPixels * m_pixelSize;

        return nextPixel();
    }

    inline bool nextPixel() {
        if (!m_isStarted) {
            m_isStarted = true;
            return m_policy.m_iter;
        }

        m_columnsLeft--;

        if (m_columnsLeft > 0) {
            m_columnOffset += m_pixelSize;
            return true;
        } else {
            bool result = m_policy.m_iter->nextPixels(m_numConseqPixels);
            if (result) {
                m_columnOffset = 0;
                m_columnsLeft = m_numConseqPixels = m_policy.m_iter->nConseqPixels();
                m_policy.updatePointersCache();
            } else if (m_rowsLeft > 0) {
                m_rowsLeft--;
                m_policy.m_iter->nextRow();
                m_columnOffset = 0;
                m_columnsLeft = m_numConseqPixels = m_policy.m_iter->nConseqPixels();
                m_policy.updatePointersCache();
                m_progressPolicy.setValue(m_policy.m_iter->y());
            } else if (m_rowsLeft == 0) {
                m_progressPolicy.setValue(m_policy.m_iter->y() + 1);
            }

            m_iteratorX = m_policy.m_iter->x();
            m_iteratorY = m_policy.m_iter->y();
        }
        return m_columnsLeft > 0;
    }

    ALWAYS_INLINE int x() const {
        return m_iteratorX + m_numConseqPixels - m_columnsLeft;
    }

    ALWAYS_INLINE int y() const {
        return m_iteratorY;
    }

    ALWAYS_INLINE quint8* rawData() {
        return m_policy.rawData() + m_columnOffset;
    }

    ALWAYS_INLINE const quint8* rawDataConst() const {
        return m_policy.rawDataConst() + m_columnOffset;
    }

    ALWAYS_INLINE const quint8* oldRawData() const {
        return m_policy.oldRawData() + m_columnOffset;
    }

private:
    Q_DISABLE_COPY(KisSequentialIteratorBase)
    IteratorPolicy m_policy;
    ProgressPolicy m_progressPolicy;
    const int m_pixelSize;
    int m_rowsLeft;

    int m_numConseqPixels;
    int m_columnsLeft;

    int m_columnOffset;
    int m_iteratorX;
    int m_iteratorY;

    bool m_isStarted;
};

typedef KisSequentialIteratorBase<ReadOnlyIteratorPolicy<> > KisSequentialConstIterator;
typedef KisSequentialIteratorBase<WritableIteratorPolicy<> > KisSequentialIterator;

#endif
