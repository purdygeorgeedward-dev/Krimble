/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGSHAPE_H
#define SVGSHAPE_H

#include "kritaflake_export.h"

class SvgSavingContext;
class SvgLoadingContext;
#include <QDomDocument>

class KRITAFLAKE_EXPORT SvgShape
{
public:
    virtual ~SvgShape();

    virtual bool saveSvg(SvgSavingContext &context);

    virtual bool loadSvg(const QDomElement &element, SvgLoadingContext &context);

    void saveMetadata(SvgSavingContext &context);
};

#endif
