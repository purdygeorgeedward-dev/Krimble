/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TASKSETDOCKER_DOCK_H
#define TASKSETDOCKER_DOCK_H

#include <QDockWidget>
#include <QModelIndex>
#include <QPointer>

#include <KoCanvasObserverBase.h>
#include <KoResourceServer.h>

#include <kis_canvas2.h>
#include <kis_types.h>
#include <filter/kis_filter_configuration.h>
#include <operations/kis_operation_configuration.h>

#include "taskset_resource.h"
#include "ui_wdgtasksetdocker.h"

class TasksetModel;

class TasksetDockerDock : public QDockWidget, public KoCanvasObserverBase, public Ui_WdgTasksetDocker {
    Q_OBJECT
public:
    TasksetDockerDock();
    ~TasksetDockerDock() override;
    QString observerName() override { return "TasksetDockerDock"; }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void actionTriggered(QAction* action);
    void filterApplied(KisFilterConfigurationSP filterConfig);
    void operationRun(KisOperationConfigurationSP operationConfig);
    void activated (const QModelIndex& index);
    void recordClicked();
    void saveClicked();
    void clearClicked();
    void resourceSelected( KoResourceSP resource );

private:
    QPointer<KisCanvas2> m_canvas;
    TasksetModel *m_model;
    bool m_blocked;
    KoResourceServer<TasksetResource> *m_rserver {0};
};

#endif

