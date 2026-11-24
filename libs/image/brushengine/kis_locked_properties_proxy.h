/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2014 Mohit Goyal <mohit.bits2011@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KIS_LOCKED_PROPERTIES_PROXY_H
#define KIS_LOCKED_PROPERTIES_PROXY_H

#include "kis_properties_configuration.h"

class KisLockedPropertiesServer;

class KRITAIMAGE_EXPORT KisLockedPropertiesProxy: public KisPropertiesConfiguration
{
public:
    KisLockedPropertiesProxy(KisPropertiesConfiguration *, KisLockedPropertiesSP);
    ~KisLockedPropertiesProxy() override;

    using KisPropertiesConfiguration::getProperty;
    QVariant getProperty(const QString &name) const override;
    using KisPropertiesConfiguration::setProperty;
    void setProperty(const QString & name, const QVariant & value) override;

    bool hasProperty(const QString& name) const override;

    QList<QString> getPropertiesKeys() const override;

    void dump() const override;

private:
    Q_DISABLE_COPY(KisLockedPropertiesProxy)
    mutable KisLockedPropertiesSP m_lockedProperties;
    KisPropertiesConfiguration* m_parent;
};

typedef KisPinnedSharedPtr<KisLockedPropertiesProxy> KisLockedPropertiesProxySP;
typedef KisWeakSharedPtr<KisLockedPropertiesProxy> KisLockedPropertiesProxyWSP;

#endif
