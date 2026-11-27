/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISBATCHNODEUPDATE_H
#define KISBATCHNODEUPDATE_H

#include "kritaimage_export.h"

#include <QRect>
#include <QSharedPointer>
#include <boost/operators.hpp>
#include <kis_types.h>

class KRITAIMAGE_EXPORT KisBatchNodeUpdate
        : public std::vector<std::pair<KisNodeSP, QRect>>,
        boost::orable<KisBatchNodeUpdate>
{
public:
    KisBatchNodeUpdate() = default;
    KisBatchNodeUpdate(const KisBatchNodeUpdate &rhs) = default;
    KisBatchNodeUpdate(KisBatchNodeUpdate &&rhs) = default;
    KisBatchNodeUpdate& operator=(const KisBatchNodeUpdate &rhs) = default;

    KisBatchNodeUpdate(const std::vector<std::pair<KisNodeSP, QRect>> &rhs);

    void addUpdate(KisNodeSP node, const QRect &rc);

    void compress();

    KisBatchNodeUpdate compressed() const;

    KisBatchNodeUpdate& operator|=(const KisBatchNodeUpdate &rhs);

};

KRITAIMAGE_EXPORT QDebug operator<<(QDebug dbg, const KisBatchNodeUpdate &update);

using KisBatchNodeUpdateSP = QSharedPointer<KisBatchNodeUpdate>;
using KisBatchNodeUpdateWSP = QWeakPointer<KisBatchNodeUpdate>;

#endif
