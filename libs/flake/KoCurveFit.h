/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2003 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCURVEFIT_H
#define KOCURVEFIT_H

#include <QList>
#include <QPointF>

#include "kritaflake_export.h"

class KoPathShape;

KRITAFLAKE_EXPORT KoPathShape * bezierFit(const QList<QPointF> &points, float error);

#endif

