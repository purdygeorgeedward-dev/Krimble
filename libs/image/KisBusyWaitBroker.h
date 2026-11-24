/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISBUSYWAITBROKER_H
#define KISBUSYWAITBROKER_H

#include <kis_types.h>
#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisBusyWaitBroker
{
public:
    KisBusyWaitBroker();
    ~KisBusyWaitBroker();

    static KisBusyWaitBroker* instance();

    void notifyWaitOnImageStarted(KisImage *image);
    void notifyWaitOnImageEnded(KisImage *image);

    void notifyGeneralWaitStarted();
    void notifyGeneralWaitEnded();

    void setFeedbackCallback(std::function<void(KisImageSP)> callback);

    bool guiThreadIsWaitingForBetterWeather() const;

private:

private:
    Q_DISABLE_COPY(KisBusyWaitBroker);

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
