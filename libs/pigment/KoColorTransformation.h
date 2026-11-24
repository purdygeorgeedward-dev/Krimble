/*
 *  SPDX-FileCopyrightText: 2006 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_TRANSFORMATION_H_
#define _KO_COLOR_TRANSFORMATION_H_

#include <QHash>

#include "kritapigment_export.h"

#include <QString>

class QVariant;

class KRITAPIGMENT_EXPORT KoColorTransformation
{
public:
    virtual ~KoColorTransformation();
    virtual void transform(const quint8 *src, quint8 *dst, qint32 nPixels) const = 0;

    virtual QList<QString> parameters() const;
    virtual int parameterId(const QString& name) const;

    void setParameters(const QHash<QString, QVariant> & parameters);
    virtual void setParameter(int id, const QVariant& parameter);

    virtual bool isValid() const { return true; }
};

#endif
