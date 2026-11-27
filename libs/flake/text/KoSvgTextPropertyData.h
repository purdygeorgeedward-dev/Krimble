/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSVGTEXTPROPERTYDATA_H
#define KOSVGTEXTPROPERTYDATA_H

#include "KoSvgTextProperties.h"
#include "kritaflake_export.h"

#include <boost/operators.hpp>
#include <QDebug>

struct KRITAFLAKE_EXPORT KoSvgTextPropertyData : public boost::equality_comparable<KoSvgTextPropertyData>
{
    KoSvgTextProperties commonProperties;

    KoSvgTextProperties inheritedProperties {KoSvgTextProperties::defaultProperties()};

    QSet<KoSvgTextProperties::PropertyId> tristate;

    bool spanSelection {false};

    bool enabled{false};

    bool operator==(const KoSvgTextPropertyData &rhs) const {
        return commonProperties == rhs.commonProperties
                && inheritedProperties == rhs.inheritedProperties
                && tristate == rhs.tristate
                && spanSelection == rhs.spanSelection
                && enabled == rhs.enabled;
    }
};

QDebug KRITAFLAKE_EXPORT operator<<(QDebug dbg, const KoSvgTextPropertyData &prop);

Q_DECLARE_METATYPE(KoSvgTextPropertyData)

#endif
