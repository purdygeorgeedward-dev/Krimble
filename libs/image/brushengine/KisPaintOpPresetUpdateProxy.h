/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINTOP_PRESET_UPDATE_PROXY_H
#define __KIS_PAINTOP_PRESET_UPDATE_PROXY_H

#include <QScopedPointer>
#include <QObject>

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisPaintOpPresetUpdateProxy : public QObject
{
    Q_OBJECT

public:
    KisPaintOpPresetUpdateProxy();
    ~KisPaintOpPresetUpdateProxy() override;

    void notifySettingsChanged();
    void notifyUniformPropertiesChanged();

    void postponeSettingsChanges();

    void unpostponeSettingsChanges();

Q_SIGNALS:
    void sigSettingsChanged();

    void sigSettingsChangedUncompressedEarlyWarning();
    void sigSettingsChangedUncompressed();
    void sigUniformPropertiesChanged();

private Q_SLOTS:
    void slotDeliverSettingsChanged();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
