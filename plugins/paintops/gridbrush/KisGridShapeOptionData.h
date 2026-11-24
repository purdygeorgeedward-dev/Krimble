/*
 *  SPDX-FileCopyrightText: 2022 Agata Cacko <cacko.azh@gmail.com>
 *  SPDX-FileCopyrightText: 2009, 2010 Lukáš Tvrdý (lukast.dev@gmail.com)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GRID_SHAPE_OPTION_DATA_H
#define KIS_GRID_SHAPE_OPTION_DATA_H

#include "kis_types.h"
#include <boost/operators.hpp>
#include <kritapaintop_export.h>

class KisPropertiesConfiguration;

const QString GRIDSHAPE_SHAPE = "GridShape/shape";

struct KisGridShapeOptionData : boost::equality_comparable<KisGridShapeOptionData>
{

    inline friend bool operator==(const KisGridShapeOptionData &lhs, const KisGridShapeOptionData &rhs) {
        return lhs.shape == rhs.shape
			;
    }

    int shape {0};

    bool read(const KisPropertiesConfiguration *setting);
    void write(KisPropertiesConfiguration *setting) const;

};

#endif
