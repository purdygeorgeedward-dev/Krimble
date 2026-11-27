/*
 *  This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_RANDOM_GENERATOR_2D_H_
#define _KIS_RANDOM_GENERATOR_2D_H_

#include <kritaimage_export.h>

#include <QtGlobal>

class KRITAIMAGE_EXPORT KisRandomGenerator2D
{
public:
    KisRandomGenerator2D(quint64 seed);
    ~KisRandomGenerator2D();
    quint64 randomAt(qint64 x, qint64 y);
    double doubleRandomAt(qint64 x, qint64 y);
private:
    struct Private;
    Private* const d;
};

#endif
