/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef _KIS_META_DATA_VALUE_H_
#define _KIS_META_DATA_VALUE_H_

#include <QList>
#include <QMap>

#include <kritametadata_export.h>
#include <boost/operators.hpp>

class QVariant;

namespace KisMetaData
{

struct Rational : public boost::equality_comparable<Rational>
{
    explicit Rational(qint32 n = 0, qint32 d = 1) : numerator(n), denominator(d) {}
    qint32 numerator;
    qint32 denominator;
    bool operator==(const Rational& ur) const {
        return numerator == ur.numerator && denominator == ur.denominator;
    }
};

class KRITAMETADATA_EXPORT Value
{
    struct Private;
public:
    enum ValueType {
        Invalid,
        Variant,
        OrderedArray,
        UnorderedArray,
        AlternativeArray,
        LangArray,
        Structure,
        Rational
    };
public:
    Value();
    Value(const QVariant& value);
    Value(const QList<Value>& array, ValueType type = OrderedArray);
    Value(const QMap<QString, Value>& structure);
    Value(const KisMetaData::Rational& rational);
    Value(const Value& v);
    Value& operator=(const Value& v);
    ~Value();
public:
    void addPropertyQualifier(const QString& _name, const Value&);
    const QMap<QString, Value>& propertyQualifiers() const;
public:
    ValueType type() const;
    double asDouble() const;
    int asInteger() const;
    QVariant asVariant() const;
    bool setVariant(const QVariant& variant);
    bool setStructureVariant(const QString& fieldNAme, const QVariant& variant);
    bool setArrayVariant(int index, const QVariant& variant);
    KisMetaData::Rational asRational() const;
    QList<KisMetaData::Value> asArray() const;
    bool isArray() const;
    QMap<QString, KisMetaData::Value> asStructure() const;
    QMap<QString, KisMetaData::Value> asLangArray() const;
    QString toString() const;
public:
    bool operator==(const Value&) const;
    Value& operator+=(const Value&);
private:
    Private* const d;
};
}

KRITAMETADATA_EXPORT QDebug operator<<(QDebug debug, const KisMetaData::Value &v);

#endif
