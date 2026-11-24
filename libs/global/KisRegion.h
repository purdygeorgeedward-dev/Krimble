/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISREGION_H
#define KISREGION_H

#include "kritaglobal_export.h"
#include <QVector>
#include <QRect>
#include <boost/operators.hpp>

class QRegion;

class KRITAGLOBAL_EXPORT KisRegion :
        public boost::equality_comparable<KisRegion>,
        public boost::andable<KisRegion, QRect>
{
public:
    static QVector<QRect>::iterator mergeSparseRects(QVector<QRect>::iterator beginIt, QVector<QRect>::iterator endIt);

    static void approximateOverlappingRects(QVector<QRect> &rects, int gridSize);

    static void makeGridLikeRectsUnique(QVector<QRect> &rects);

public:
    KisRegion() = default;
    KisRegion(const KisRegion &rhs) = default;
    KisRegion(const QRect &rect);
    KisRegion(std::initializer_list<QRect> rects);

    KisRegion(const QVector<QRect> &rects);
    KisRegion(QVector<QRect> &&rects);

    KisRegion& operator=(const KisRegion &rhs);
    friend KRITAGLOBAL_EXPORT bool operator==(const KisRegion &lhs, const KisRegion &rhs);

    KisRegion& operator&=(const QRect &rect);

    QRect boundingRect() const;
    QVector<QRect> rects() const;
    int rectCount() const;
    bool isEmpty() const;

    QRegion toQRegion() const;

    void translate(int dx, int dy);
    KisRegion translated(int x, int y) const;

    static KisRegion fromQRegion(const QRegion &region);

    static KisRegion fromOverlappingRects(const QVector<QRect> &rects, int gridSize);

private:
    void mergeAllRects();

private:
    QVector<QRect> m_rects;
};

KRITAGLOBAL_EXPORT bool operator==(const KisRegion &lhs, const KisRegion &rhs);

#endif
