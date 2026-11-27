/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_ANNOTATION_H_
#define _KIS_ANNOTATION_H_

#include <kis_shared.h>

#include <QByteArray>
#include <QString>

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisAnnotation : public KisShared
{

public:

    KisAnnotation(const QString & type, const QString & description, const QByteArray & data)
        : m_type(type)
        , m_description(description)
        , m_annotation(data) {}

    virtual ~KisAnnotation() {}

    virtual KisAnnotation* clone() const {
        return new KisAnnotation(*this);
    }

    const QString & type() const {
        return m_type;
    }

    const QString & description() const {
        return m_description;
    }

    const QByteArray & annotation() const {
        return m_annotation;
    }

    void setAnnotation(const QByteArray ba) {
        m_annotation = ba;
    }

    virtual QString displayText() const {
        return QString::fromUtf8(m_annotation);
    }

protected:
    KisAnnotation(const KisAnnotation &rhs)
     : KisShared(),
       m_type(rhs.m_type),
       m_description(rhs.m_description),
       m_annotation(rhs.m_annotation)
    {
    }

protected:

    QString m_type;
    QString m_description;
    QByteArray m_annotation;

};

#endif
