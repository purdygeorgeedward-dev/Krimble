/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISMENUSTYLEDONTCLOSEONALT_H
#define KISMENUSTYLEDONTCLOSEONALT_H

#include "kritawidgetutils_export.h"
#include <QProxyStyle>

class KRITAWIDGETUTILS_EXPORT KisMenuStyleDontCloseOnAlt : public QProxyStyle
{
public:
    KisMenuStyleDontCloseOnAlt(QStyle *baseStyle);
    int styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;
};

#endif
