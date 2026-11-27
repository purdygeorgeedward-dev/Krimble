/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOUNIT_H
#define KOUNIT_H

#include "kritaglobal_export.h"

#include <QString>
#include <QDebug>
#include <QMetaType>
#include <QStringList>
#include <QLocale>

#include <math.h>
#include <boost/operators.hpp>

constexpr qreal POINT_TO_MM(qreal px) {return (px)*0.352777167;}
constexpr qreal MM_TO_POINT(qreal mm) {return mm*2.83465058;}
constexpr qreal POINT_TO_CM(qreal px) {return (px)*0.0352777167;}
constexpr qreal CM_TO_POINT(qreal cm) {return (cm)*28.3465058;}
constexpr qreal POINT_TO_DM(qreal px) {return (px)*0.00352777167;}
constexpr qreal DM_TO_POINT(qreal dm) {return (dm)*283.465058;}
constexpr qreal POINT_TO_INCH(qreal px) {return (px)*0.01388888888889;}
constexpr qreal INCH_TO_POINT(qreal inch) {return (inch)*72.0;}
constexpr qreal MM_TO_INCH(qreal mm) {return (mm)*0.039370147;}
constexpr qreal INCH_TO_MM(qreal inch) {return (inch)*25.399956;}
constexpr qreal POINT_TO_PI(qreal px) {return (px)*0.083333333;}
constexpr qreal POINT_TO_CC(qreal px) {return (px)*0.077880997;}
constexpr qreal PI_TO_POINT(qreal pi) {return (pi)*12;}
constexpr qreal CC_TO_POINT(qreal cc) {return (cc)*12.840103;}

static const qreal PT_ROUNDING {1000.0};

static const qreal CM_ROUNDING {10000.0};
static const qreal DM_ROUNDING {10000.0};
static const qreal MM_ROUNDING {10000.0};

static const qreal IN_ROUNDING {100000.0};

static const qreal PI_ROUNDING {100000.0};
static const qreal CC_ROUNDING {100000.0};

class KRITAGLOBAL_EXPORT KoUnit : public boost::equality_comparable<KoUnit>
{
public:
    enum Type {
        Millimeter = 0,
        Point,
        Inch,
        Centimeter,
        Decimeter,
        Pica,
        Cicero,
        Pixel,
        TypeCount
    };

    enum ListOption {
        ListAll = 0,
        HidePixel = 1,
        HideMask = HidePixel
    };
     Q_DECLARE_FLAGS(ListOptions, ListOption)

    static KoUnit fromListForUi(int index, ListOptions listOptions = ListAll, qreal factor = 1.0);

    static KoUnit fromSymbol(const QString &symbol, bool *ok = 0);

    explicit KoUnit(Type unit = Point, qreal factor = 1.0) {
        m_type = unit;
        m_pixelConversion = factor;
    }

    KoUnit& operator=(Type unit) {
        m_type = unit; m_pixelConversion = 1.0; return *this;
    }

    bool operator==(const KoUnit &other) const {
        return m_type == other.m_type &&
            (m_type != Pixel ||
             qFuzzyCompare(m_pixelConversion, other.m_pixelConversion));
    }

    KoUnit::Type type() const {
        return m_type;
    }

    void setFactor(qreal factor) {
        m_pixelConversion = factor;
    }

    static qreal convertFromUnitToUnit(const qreal value, const KoUnit &fromUnit, const KoUnit &toUnit, qreal factor = 1.0);

    qreal toUserValueRounded(const qreal value) const;
    qreal toUserValuePrecise(const qreal ptValue) const;

    qreal toUserValue(qreal ptValue, bool rounding = true) const;

    QString toUserStringValue(qreal ptValue) const;

    qreal fromUserValue(qreal value) const;

    qreal fromUserValue(const QString &value, bool *ok = 0) const;

    static QString unitDescription(KoUnit::Type type);

    QString symbol() const;

    static QStringList listOfUnitNameForUi(ListOptions listOptions = ListAll);

    int indexInListForUi(ListOptions listOptions = ListAll) const;

    static qreal parseValue(const QString &value, qreal defaultVal = 0.0);

    static qreal parseAngle(const QString &value, qreal defaultVal = 0.0);

    QString toString() const {
        return symbol();
    }

    static qreal approxTransformScale(const QTransform &t);

    void adjustByPixelTransform(const QTransform &t);

private:
    Type m_type;
    qreal m_pixelConversion;
};

#ifndef QT_NO_DEBUG_STREAM
KRITAGLOBAL_EXPORT QDebug operator<<(QDebug, const KoUnit &);
#endif

Q_DECLARE_METATYPE(KoUnit)
Q_DECLARE_OPERATORS_FOR_FLAGS(KoUnit::ListOptions)

#endif
