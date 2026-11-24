/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2014 Mohit Goyal <mohit.bits2011@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KISLOCKEDPROPERTIES_H
#define KISLOCKEDPROPERTIES_H

#include "kis_properties_configuration.h"

class KisLockedProperties : public KisShared
{
public:
    KisLockedProperties();
    ~KisLockedProperties();

    void addToLockedProperties(KisPropertiesConfigurationSP p);
    void addToLockedProperties(const KisPropertiesConfiguration *p);

    void removeFromLockedProperties(KisPropertiesConfigurationSP p);
    void removeFromLockedProperties(const KisPropertiesConfiguration *p);
    bool hasProperty(const QString &p);

    KisPropertiesConfigurationSP lockedProperties();

private:
    Q_DISABLE_COPY(KisLockedProperties)
    KisPropertiesConfigurationSP m_lockedProperties;
};

#endif
