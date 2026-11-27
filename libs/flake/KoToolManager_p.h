/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_TOOL_MANAGER_P
#define KO_TOOL_MANAGER_P

#include <QList>
#include <QObject>
#include <QString>
#include <QHash>

#include <QKeySequence>
#include <QAction>

#include "KoInputDevice.h"
#include "KoToolManager.h"

class KoToolFactoryBase;
class KoShapeManager;
class KoCanvasBase;
class KoToolBase;
class KoShape;
class KoToolManager;
class KoCanvasController;
class KoShapeLayer;
class CanvasData;
class KoToolProxy;

class Q_DECL_HIDDEN KoToolManager::Private
{
public:
    Private(KoToolManager *qq);
    ~Private();

    void setup();

    void connectActiveTool();
    void disconnectActiveTool();
    void switchTool(const QString &id);
    void postSwitchTool();
    void switchCanvasData(CanvasData *cd);

    bool eventFilter(QObject *object, QEvent *event);

    void detachCanvas(KoCanvasController *controller);
    void attachCanvas(KoCanvasController *controller);
    void movedFocus(QWidget *from, QWidget *to);
    void updateCursor(const QCursor &cursor);
    void switchBackRequested();
    void selectionChanged(const QList<KoShape*> &shapes);
    void currentLayerChanged(const KoShapeLayer *layer);
    void updateToolForProxy();
    void switchToolTemporaryRequested(const QString &id);
    CanvasData *createCanvasData(KoCanvasController *controller, const KoInputDevice &device);
    KoToolBase* createTool(KoCanvasController *controller, KoToolAction *toolAction);

    void switchInputDevice(const KoInputDevice &device);

    void registerToolProxy(KoToolProxy *proxy, KoCanvasBase *canvas);

    KoToolManager *q;

    QList<KoToolAction*> toolActionList;

    QHash<KoCanvasController*, QList<CanvasData*> > canvasses;
    QHash<KoCanvasBase*, KoToolProxy*> proxies;

    CanvasData *canvasData;

    KoInputDevice inputDevice;

    bool layerExplicitlyDisabled;
};

class Connector : public QObject
{
    Q_OBJECT
public:
    explicit Connector(KoShapeManager *parent);

public Q_SLOTS:
    void selectionChanged();

Q_SIGNALS:
    void selectionChanged(const QList<KoShape*> &shape);

private:
    KoShapeManager *m_shapeManager;
};

#endif
