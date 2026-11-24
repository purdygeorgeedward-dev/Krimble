/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_MAINWINDOW_OBSERVER_H
#define KIS_MAINWINDOW_OBSERVER_H

#include <KoCanvasObserverBase.h>
#include <kritaui_export.h>

class KisViewManager;

class KRITAUI_EXPORT KisMainwindowObserver : public KoCanvasObserverBase
{
public:
    KisMainwindowObserver();
    ~KisMainwindowObserver() override;

    virtual void setViewManager(KisViewManager* kisview) = 0;

};

#endif
