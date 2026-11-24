/*
 *  SPDX-FileCopyrightText: 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ACTION_MANAGER_H
#define KIS_ACTION_MANAGER_H

#include <kritaui_export.h>

#include <QPointer>

#include "KisView.h"

#include "kstandardaction.h"
#include "kis_action_registry.h"
#include "operations/kis_operation_configuration.h"

class KisViewManager;
class KisAction;
class KisOperationUIFactory;
class KisOperation;

class KRITAUI_EXPORT KisActionManager : public QObject
{
    Q_OBJECT
public:
    KisActionManager(KisViewManager* viewManager, KisKActionCollection *actionCollection);
    ~KisActionManager() override;

    void setView(QPointer<KisView> imageView);

    void addAction(const QString& name, KisAction* action);

    void takeAction(KisAction* action);

    KisAction *createAction(const QString &name);

    KisAction *actionByName(const QString &name) const;

    void registerOperationUIFactory(KisOperationUIFactory* factory);
    void registerOperation(KisOperation* operation);
    void runOperation(const QString &id);
    void runOperationFromConfiguration(KisOperationConfigurationSP config);

    void updateGUI();

    KisAction *createStandardAction(KStandardAction::StandardAction,
                                    const QObject *receiver, const char *member);

    static void safePopulateMenu(QMenu *menu, const QString &actionId, KisActionManager *actionManager);

Q_SIGNALS:
    void sigOperationRun(KisOperationConfigurationSP config);

private Q_SLOTS:
    void slotActionAddedToCollection(QAction *action);

private:
    void dumpActionFlags();

    class Private;
    Private* const d;
};

#endif
