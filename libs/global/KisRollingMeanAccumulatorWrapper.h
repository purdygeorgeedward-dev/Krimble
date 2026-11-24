/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISROLLINGMEANACCUMULATORWRAPPER_H
#define KISROLLINGMEANACCUMULATORWRAPPER_H

#include <QtGlobal>
#include <QScopedPointer>
#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KisRollingMeanAccumulatorWrapper
{
public:
    KisRollingMeanAccumulatorWrapper(int windowSize);
    ~KisRollingMeanAccumulatorWrapper();

    void operator()(qreal value);

    qreal rollingMean() const;

    qreal rollingMeanSafe() const;

    int rollingCount() const;

    void reset(int windowSize);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
