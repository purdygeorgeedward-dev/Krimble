/*
 *  SPDX-FileCopyrightText: 2008-2009 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_MASK_GENERATOR_H_
#define _KIS_MASK_GENERATOR_H_

#include <QScopedPointer>

#include <KoID.h>
#include <klocalizedstring.h>

#include "kritaimage_export.h"

class QDomElement;
class QDomDocument;
class KisBrushMaskApplicatorBase;

const KoID DefaultId("default", ki18n("Default"));
const KoID SoftId("soft", ki18n("Soft"));
const KoID GaussId("gauss", ki18n("Gaussian"));

static const int OVERSAMPLING = 4;

class KRITAIMAGE_EXPORT KisMaskGenerator
{
public:
    enum Type {
        CIRCLE, RECTANGLE
    };
public:

    KisMaskGenerator(qreal radius, qreal ratio, qreal fh, qreal fv, int spikes, bool antialiasEdges, Type type, const KoID& id = DefaultId);
    KisMaskGenerator(const KisMaskGenerator &rhs);

    virtual ~KisMaskGenerator();

    virtual KisMaskGenerator* clone() const = 0;

private:

    void init();

public:
    virtual quint8 valueAt(qreal x, qreal y) const = 0;

    virtual bool shouldSupersample() const;

    virtual bool shouldSupersample6x6() const;

    virtual bool shouldVectorize() const;

    virtual KisBrushMaskApplicatorBase *applicator() const = 0;

    virtual void toXML(QDomDocument& , QDomElement&) const;

    static KisMaskGenerator* fromXML(const QDomElement&);

    qreal width() const;

    qreal height() const;

    qreal diameter() const;
    void setDiameter(qreal value);

    qreal ratio() const;
    qreal horizontalFade() const;
    qreal verticalFade() const;
    int spikes() const;
    Type type() const;
    bool isEmpty() const;
    void fixRotation(qreal &xr, qreal &yr) const;

    inline QString id() const { return m_id.id(); }
    inline QString name() const { return m_id.name(); }

    static QList<KoID> maskGeneratorIds();

    qreal softness() const;
    virtual void setSoftness(qreal softness);

    QString curveString() const;
    void setCurveString(const QString& curveString);

    bool antialiasEdges() const;
    virtual void setScale(qreal scaleX, qreal scaleY);

protected:
    qreal effectiveSrcWidth() const;
    qreal effectiveSrcHeight() const;

private:
    struct Private;
    const QScopedPointer<Private> d;
    const KoID& m_id;
};

#endif
