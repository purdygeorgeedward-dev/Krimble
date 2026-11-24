/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_RANDOM_SOURCE_H
#define __KIS_RANDOM_SOURCE_H

#include <QScopedPointer>
#include "kis_shared.h"
#include "kis_shared_ptr.h"

#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KisRandomSource : public KisShared
{
public:
    KisRandomSource();
    KisRandomSource(int seed);
    KisRandomSource(const KisRandomSource &rhs);
    KisRandomSource& operator=(const KisRandomSource &rhs);

    ~KisRandomSource();

    qint64 generate() const;

    int generate(int min, int max) const;

    qreal generateNormalized() const;

    qreal generateGaussian(qreal mean, qreal sigma) const;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

class KisRandomSource;
typedef KisSharedPtr<KisRandomSource> KisRandomSourceSP;
typedef KisWeakSharedPtr<KisRandomSource> KisRandomSourceWSP;

#endif
