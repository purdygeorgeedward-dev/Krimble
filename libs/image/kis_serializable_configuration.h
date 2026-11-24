/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_SERIALIZABLE_CONFIGURATION_H_
#define _KIS_SERIALIZABLE_CONFIGURATION_H_

class QDomElement;
class QDomDocument;
class QString;

#include "kritaimage_export.h"
#include "kis_shared.h"
#include "kis_shared_ptr.h"

class KRITAIMAGE_EXPORT KisSerializableConfiguration : public KisShared
{
public:

    KisSerializableConfiguration();

    virtual ~KisSerializableConfiguration() {}

    KisSerializableConfiguration(const KisSerializableConfiguration &rhs);

    virtual bool fromXML(const QString&, bool);

    virtual void fromXML(const QDomElement&) = 0;

    virtual void toXML(QDomDocument&, QDomElement&) const = 0;

    virtual QString toXML() const;
};

typedef KisSharedPtr<KisSerializableConfiguration> KisSerializableConfigurationSP;

class KRITAIMAGE_EXPORT KisSerializableConfigurationFactory
{
public:
    virtual ~KisSerializableConfigurationFactory();
    virtual KisSerializableConfigurationSP createDefault() = 0;
    virtual KisSerializableConfigurationSP create(const QDomElement&) = 0;
};

#endif
