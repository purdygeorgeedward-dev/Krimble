/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_FILTER_MANAGER_
#define _KIS_FILTER_MANAGER_

#include <QObject>
#include <kritaui_export.h>
#include <kis_types.h>

#include <filter/kis_filter_configuration.h>
class KisViewManager;
class KisKActionCollection;
class KisActionManager;
class KisView;

class KRITAUI_EXPORT KisFilterManager : public QObject
{

    Q_OBJECT

public:

    KisFilterManager(KisViewManager * parent);
    ~KisFilterManager() override;
    void setView(QPointer<KisView>imageView);

    void setup(KisKActionCollection * ac, KisActionManager *actionManager);
    void updateGUI();

    void apply(KisFilterConfigurationSP filterConfig);
    void finish();
    void cancelRunningStroke();
    void cancelDialog();
    bool isStrokeRunning() const;

    bool isIdle() const;

    void setFilterAllSelectedFrames(bool filterAllSelectedFrames);
    bool filterAllSelectedFrames();

Q_SIGNALS:
    void sigFilterApplied(KisFilterConfigurationSP filterConfig);

private Q_SLOTS:

    void insertFilter(const QString &name);
    void showFilterDialog(const QString &filterId, KisFilterConfigurationSP overrideDefaultConfig = nullptr);
    void reapplyLastFilter();
    void reapplyLastFilterReprompt();

    void slotStrokeEndRequested();
    void slotStrokeCancelRequested();

    void filterDialogHasFinished(int);

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
