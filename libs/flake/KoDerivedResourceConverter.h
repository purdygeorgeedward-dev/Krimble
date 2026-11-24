/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KO_DERIVED_RESOURCE_CONVERTER_H
#define __KO_DERIVED_RESOURCE_CONVERTER_H

#include <QScopedPointer>
#include <QSharedPointer>
#include "kritaflake_export.h"

class QVariant;

 class KRITAFLAKE_EXPORT KoDerivedResourceConverter
{
public:
    KoDerivedResourceConverter(int key, int sourceKey);
    virtual ~KoDerivedResourceConverter();

    int key() const;
    int sourceKey() const;

    QVariant readFromSource(const QVariant &value);
    QVariant writeToSource(const QVariant &value,
                           const QVariant &sourceValue,
                           bool *changed);

    virtual bool notifySourceChanged(const QVariant &sourceValue);

protected:
    virtual QVariant fromSource(const QVariant &value) = 0;

    virtual QVariant toSource(const QVariant &value, const QVariant &sourceValue) = 0;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

typedef QSharedPointer<KoDerivedResourceConverter> KoDerivedResourceConverterSP;

#endif
