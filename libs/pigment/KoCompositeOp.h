/*
 * SPDX-FileCopyrightText: 2005 Adrian Page <adrian@pagenet.plus.com>
 * SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/
#ifndef KOCOMPOSITEOP_H
#define KOCOMPOSITEOP_H

#include <QString>
#include <QMultiMap>
#include <QBitArray>

#include <boost/optional.hpp>

#include "kritapigment_export.h"

class KoColorSpace;

class KoColorSpace;

class KRITAPIGMENT_EXPORT KoCompositeOp
{
public:
    static QString categoryArithmetic();
    static QString categoryBinary();
    static QString categoryModulo();
    static QString categoryNegative();
    static QString categoryLight();
    static QString categoryDark();
    static QString categoryHSY();
    static QString categoryHSI();
    static QString categoryHSL();
    static QString categoryHSV();
    static QString categoryMix();
    static QString categoryMisc();
    static QString categoryQuadratic();

    struct KRITAPIGMENT_EXPORT ParameterInfo
    {
        ParameterInfo();
        ParameterInfo(const ParameterInfo &rhs);
        ParameterInfo& operator=(const ParameterInfo &rhs);

        quint8*       dstRowStart {0};
        qint32        dstRowStride {0};
        const quint8* srcRowStart {0};
        qint32        srcRowStride {0};
        const quint8* maskRowStart {0};
        qint32        maskRowStride {0};
        qint32        rows {0};
        qint32        cols {0};
        float         opacity {0.0};
        float         flow {0.0};
        float         _lastOpacityData {0.0};
        float*        lastOpacity {0};
        QBitArray     channelFlags;

        void setOpacityAndAverage(float _opacity, float _averageOpacity);

        void updateOpacityAndAverage(float value);
    private:
        inline void copy(const ParameterInfo &rhs);
    };

public:

    KoCompositeOp(const KoColorSpace * cs, const QString& id, const QString & category = KoCompositeOp::categoryMisc());
    virtual ~KoCompositeOp();

    QString id() const;
    QString description() const;
    const KoColorSpace * colorSpace() const;
    QString category() const;

    void composite(quint8 *dstRowStart, qint32 dstRowStride,
                   const quint8 *srcRowStart, qint32 srcRowStride,
                   const quint8 *maskRowStart, qint32 maskRowStride,
                   qint32 rows, qint32 numColumns,
                   float opacity, const QBitArray& channelFlags = QBitArray()) const;

    virtual void composite(const ParameterInfo& params) const;

private:
    KoCompositeOp();
    struct Private;
    Private* const d;
};

#endif
