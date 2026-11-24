/*
 *  SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_NUMPARSER_H
#define KIS_NUMPARSER_H

#include <QString>

#include "kritawidgetutils_export.h"

namespace KisNumericParser {

    KRITAWIDGETUTILS_EXPORT double parseSimpleMathExpr(QString const& expr, bool* noProblem = 0);

    KRITAWIDGETUTILS_EXPORT int parseIntegerMathExpr(QString const& expr, bool* noProblem = 0);
}

#endif

