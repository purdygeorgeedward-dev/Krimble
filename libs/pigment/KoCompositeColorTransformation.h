/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KO_COMPOSITE_COLOR_TRANSFORMATION_H
#define __KO_COMPOSITE_COLOR_TRANSFORMATION_H

#include "KoColorTransformation.h"

#include <QScopedPointer>

class KRITAPIGMENT_EXPORT KoCompositeColorTransformation : public KoColorTransformation
{
public:
    enum Mode {
        INPLACE = 0,
        BUFFERED
    };

public:
    explicit KoCompositeColorTransformation(Mode mode);
    ~KoCompositeColorTransformation() override;

    void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const override;

    void appendTransform(KoColorTransformation *transform);

    static KoColorTransformation* createOptimizedCompositeTransform(const QVector<KoColorTransformation*> transforms);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
