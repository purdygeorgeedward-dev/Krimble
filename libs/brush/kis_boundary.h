/*
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_BOUNDARY_H_
#define _KIS_BOUNDARY_H_

#include <QPainter>

#include <kritabrush_export.h>

#include "kis_types.h"

class BRUSH_EXPORT KisBoundary
{
public:
    KisBoundary(KisFixedPaintDeviceSP dev);
    ~KisBoundary();
    void generateBoundary();

    void paint(QPainter& painter) const;

    QPainterPath path() const;

private:
    struct Private;
    Private* const d;
};

#endif
