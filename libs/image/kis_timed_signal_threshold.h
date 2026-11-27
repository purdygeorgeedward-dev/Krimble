/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_TIMED_SIGNAL_THRESHOLD_H
#define __KIS_TIMED_SIGNAL_THRESHOLD_H

#include "kritaimage_export.h"
#include <QScopedPointer>
#include <QObject>

class KRITAIMAGE_EXPORT KisTimedSignalThreshold : public QObject
{
    Q_OBJECT
public:
    KisTimedSignalThreshold(int delay, int cancelDelay = -1, QObject *parent = 0);
    ~KisTimedSignalThreshold() override;

public Q_SLOTS:
    void forceDone();

    void start();

    void stop();

    void setEnabled(bool value);

    void setDelayThreshold(int delay, int cancelDelay = -1);

Q_SIGNALS:
    void timeout();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
