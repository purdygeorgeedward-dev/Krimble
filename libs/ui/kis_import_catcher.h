/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMPORT_CATCHER_H_
#define KIS_IMPORT_CATCHER_H_

#include <QObject>

#include "kritaui_export.h"
#include <kis_types.h>

class KisViewManager;

class KRITAUI_EXPORT KisImportCatcher : QObject
{

    Q_OBJECT

public:

    KisImportCatcher(const QString &url, KisViewManager* view, const QString &layerType);
    ~KisImportCatcher() override;

    int numLayersImported() const;

    static void adaptClipToImageColorSpace(KisPaintDeviceSP dev, KisImageSP image);

private Q_SLOTS:
    void slotLoadingFinished();

private:
    void deleteMyself();

private:

    struct Private;
    Private* const m_d;
};

#endif
