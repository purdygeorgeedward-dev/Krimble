/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISSAMPLERECTITERATOR_H
#define KISSAMPLERECTITERATOR_H

#include "kritaglobal_export.h"

#include <boost/iterator_adaptors.hpp>

#include <QSharedDataPointer>
#include <QRectF>

class KRITAGLOBAL_EXPORT KisSampleRectIterator
    : public boost::iterator_facade<KisSampleRectIterator,
                                    QPointF,
                                    boost::forward_traversal_tag,
                                    QPointF>
{
public:
    KisSampleRectIterator();
    KisSampleRectIterator(const QRectF &rect);
    KisSampleRectIterator(const KisSampleRectIterator &rhs);
    KisSampleRectIterator(KisSampleRectIterator &&rhs);
    KisSampleRectIterator& operator=(const KisSampleRectIterator &rhs);
    KisSampleRectIterator& operator=(KisSampleRectIterator &&rhs);
    ~KisSampleRectIterator();

public:
    int numSamples() const;

private:
    friend class boost::iterator_core_access;

    void increment();
    QPointF dereference() const;

private:
    struct HaltonSampler;
    QSharedDataPointer<HaltonSampler> m_sampler;

    QRectF m_rect;
    int m_index = 0;
};

#endif
