/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005-2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoToolManager.h"
#include "KoToolManager_p.h"
#include "KoToolRegistry.h"
#include "KoToolProxy.h"
#include "KoToolProxy_p.h"
#include "KoSelection.h"
#include "KoCanvasController.h"
#include "KoShape.h"
#include "KoShapeLayer.h"
#include "KoShapeRegistry.h"
#include "KoShapeManager.h"
#include "KoSelectedShapesProxy.h"
#include "KoCanvasBase.h"
#include "KoPointerEvent.h"
#include "tools/KoZoomTool.h"
#include "kis_action_registry.h"
#include "KoToolFactoryBase.h"
#include "kis_assert.h"
#include "KoCanvasResourceProvider.h"

#include <krita_container_utils.h>

#include <QWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTabletEvent>
#include <QVBoxLayout>
#include <QStringList>
#include <QApplication>
#include <kactioncollection.h>
#include <kactioncategory.h>
#include <FlakeDebug.h>

#include <QAction>
#include <klocalizedstring.h>
#include <QKeySequence>
#include <QStack>
#include <QLabel>
#include <QGlobalStatic>

Q_GLOBAL_STATIC(KoToolManager, s_instance)

class CanvasData
{
public:
    CanvasData(KoCanvasController *cc, const KoInputDevice &id)
        : activeTool(0),
          canvas(cc),
          inputDevice(id),
          dummyToolWidget(0),
          dummyToolLabel(0)
    {
    }

    ~CanvasData()
    {
        delete dummyToolWidget;
    }

    void activateToolActions()
    {
        toolActions.clear();
        disabledGlobalActions.clear();

        KisKActionCollection *windowActionCollection = canvas->actionCollection();

        if (!windowActionCollection) {
            qWarning() << "We haven't got an action collection";
            return;
        }

        QStringList globalActions;

        QMap<QKeySequence, QStringList> shortcutMap;

        Q_FOREACH(QAction *action, windowActionCollection->actions()) {

            if (action->property("tool_action").isValid()) {
                QStringList tools = action->property("tool_action").toStringList();

                if (KoToolRegistry::instance()->keys().contains(action->objectName())) {
                    action->setEnabled(true);
                    toolActions << action->objectName();
                }
                else {
                    if (tools.contains(activeToolId) || action->property("always_enabled").toBool()) {
                        action->setEnabled(true);
                        toolActions << action->objectName();
                    }
                    else {
                        action->setDisabled(true);
                    }
                }
            }
            else {
                globalActions << action->objectName();
            }

            Q_FOREACH(QKeySequence keySequence, action->shortcuts()) {
                if (keySequence.toString() != "") {
                    if (shortcutMap.contains(keySequence)) {
                        shortcutMap[keySequence].append(action->objectName());
                    }
                    else {
                        shortcutMap[keySequence] = QStringList() << action->objectName();
                    }
                }
            }
        }

        Q_FOREACH(const QKeySequence &k, shortcutMap.keys()) {
            if (shortcutMap[k].size() > 1) {
                QStringList actions = shortcutMap[k];
                bool toolActionFound = false;
                Q_FOREACH(const QString &action, actions) {
                    if (toolActions.contains(action)) {
                        toolActionFound = true;
                    }
                }
                Q_FOREACH(const QString &action, actions) {
                    if (toolActionFound && globalActions.contains(action)) {
                        windowActionCollection->action(action)->setEnabled(false);
                        disabledGlobalActions << action;
                    }
                }
            }
        }

        windowActionCollection->readSettings();
    }

    void deactivateToolActions()
    {
        if (!activeTool)
            return;

        KisKActionCollection *windowActionCollection = canvas->actionCollection();

        Q_FOREACH(const QString &action, toolActions) {
            windowActionCollection->action(action)->setDisabled(true);
        }
        Q_FOREACH(const QString &action, disabledGlobalActions) {
            windowActionCollection->action(action)->setEnabled(true);
        }
    }

    KoToolBase *activeTool;
    QString activeToolId;
    QString activationShapeId;
    QHash<QString, KoToolBase*> allTools;
    QList<KoToolBase*> mostRecentTools;
    KoCanvasController *const canvas;
    const KoInputDevice inputDevice;
    QWidget *dummyToolWidget;
    QLabel *dummyToolLabel;
    QStringList toolActions;
    QStringList disabledGlobalActions;
};

KoToolManager::KoToolManager()
    : QObject(),
      d(new Private(this))
{
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(movedFocus(QWidget*,QWidget*)));
}

KoToolManager::~KoToolManager()
{
    delete d;
}

QList<KoToolAction*> KoToolManager::toolActionList() const
{
    return d->toolActionList;
}

void KoToolManager::requestToolActivation(KoCanvasController * controller)
{
    if (d->canvasses.contains(controller)) {
        d->switchTool(d->canvasses.value(controller).first()->activeToolId);
    }
}

KoInputDevice KoToolManager::currentInputDevice() const
{
    return d->inputDevice;
}

void KoToolManager::initializeToolActions()
{
    d->setup();
}

void KoToolManager::addController(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    if (d->canvasses.contains(controller))
        return;
    d->setup();
    d->attachCanvas(controller);
    connect(controller->proxyObject, SIGNAL(destroyed(QObject*)), this, SLOT(attemptCanvasControllerRemoval(QObject*)));
    connect(controller->proxyObject, SIGNAL(canvasRemoved(KoCanvasController*)), this, SLOT(detachCanvas(KoCanvasController*)));
    connect(controller->proxyObject, SIGNAL(canvasSet(KoCanvasController*)), this, SLOT(attachCanvas(KoCanvasController*)));
}

void KoToolManager::removeCanvasController(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    disconnect(controller->proxyObject, SIGNAL(canvasRemoved(KoCanvasController*)), this, SLOT(detachCanvas(KoCanvasController*)));
    disconnect(controller->proxyObject, SIGNAL(canvasSet(KoCanvasController*)), this, SLOT(attachCanvas(KoCanvasController*)));
    d->detachCanvas(controller);
}

void KoToolManager::attemptCanvasControllerRemoval(QObject* controller)
{
    KoCanvasControllerProxyObject* controllerActual = qobject_cast<KoCanvasControllerProxyObject*>(controller);
    if (controllerActual) {
        removeCanvasController(controllerActual->canvasController());
    }
}

void KoToolManager::switchToolRequested(const QString & id)
{
    d->switchTool(id);
}

void KoToolManager::switchInputDeviceRequested(const KoInputDevice &id)
{
    if (!d->canvasData) return;
    d->switchInputDevice(id);
}

void KoToolManager::switchBackRequested()
{
    if (!d->canvasData) return;
    if (d->canvasData->mostRecentTools.isEmpty()) return;
    d->switchTool(d->canvasData->mostRecentTools.first()->toolId());
}

KoToolBase *KoToolManager::toolById(KoCanvasBase *canvas, const QString &id) const
{
    Q_ASSERT(canvas);
    Q_FOREACH (KoCanvasController *controller, d->canvasses.keys()) {
        if (controller->canvas() == canvas)
            return d->canvasData->allTools.value(id);
    }
    return 0;
}

KoCanvasController *KoToolManager::activeCanvasController() const
{
    if (! d->canvasData) return 0;
    return d->canvasData->canvas;
}

QString KoToolManager::preferredToolForSelection(const QList<KoShape*> &shapes)
{
    QSet<QString> shapeTypes;
    Q_FOREACH (KoShape *shape, shapes) {
        shapeTypes << shape->shapeId();
    }

    QString toolType = KoInteractionTool_ID;
    int prio = INT_MAX;
    Q_FOREACH (KoToolAction *helper, d->toolActionList) {
        if (helper->priority() >= prio)
            continue;

        bool toolWillWork = false;
        foreach (const QString &type, shapeTypes) {
            if (helper->toolFactory()->activationShapeId().split(',').contains(type)) {
                toolWillWork = true;
                break;
            }
        }

        if (toolWillWork) {
            toolType = helper->id();
            prio = helper->priority();
        }
    }
    return toolType;
}

void KoToolManager::initializeCurrentToolForCanvas()
{
    KIS_ASSERT_RECOVER_RETURN(d->canvasData);

    d->disconnectActiveTool();
    d->connectActiveTool();
    d->postSwitchTool();
}

void KoToolManager::themeChanged()
{
    Q_FOREACH (const QList<CanvasData*> &canvasDataList, d->canvasses) {
        Q_FOREACH (CanvasData *canvasData, canvasDataList) {
            Q_FOREACH (KoToolBase *tool, canvasData->allTools) {
                tool->updateOptionsWidgetIcons();
            }
        }
    }
}

KoToolManager* KoToolManager::instance()
{
    return s_instance;
}

QString KoToolManager::activeToolId() const
{
    if (!d->canvasData) return QString();
    return d->canvasData->activeToolId;
}

void KoToolManager::setConverter(KoDerivedResourceConverterSP converter, KoToolBase *tool)
{
    tool->setConverter(converter);
}

void KoToolManager::setAbstractResource(KoAbstractCanvasResourceInterfaceSP abstractResource, KoToolBase *tool)
{
    tool->setAbstractResource(abstractResource);
}

KoToolManager::Private *KoToolManager::priv()
{
    return d;
}

KoToolManager::Private::Private(KoToolManager *qq)
    : q(qq),
      canvasData(0),
      layerExplicitlyDisabled(false)
{
}

KoToolManager::Private::~Private()
{
    qDeleteAll(toolActionList);
}

CanvasData *KoToolManager::Private::createCanvasData(KoCanvasController *controller, const KoInputDevice &device)
{
    QHash<QString, KoToolBase*> toolsHash;
    Q_FOREACH (KoToolAction *toolAction, toolActionList) {
        KoToolBase* tool = createTool(controller, toolAction);
        if (tool) {
            toolsHash.insert(tool->toolId(), tool);
            Q_EMIT q->createOpacityResource(tool->isOpacityPresetMode(), tool);
        }
    }

    CanvasData *cd = new CanvasData(controller, device);
    cd->allTools = toolsHash;
    return cd;
}

KoToolBase *KoToolManager::Private::createTool(KoCanvasController *controller, KoToolAction *toolAction)
{
    QHash<QString, KoToolBase*> origHash;

    if (canvasses.contains(controller)) {
        origHash = canvasses.value(controller).first()->allTools;
    }

    if (origHash.contains(toolAction->id())) {
        return origHash.value(toolAction->id());
    }

    debugFlake << "Creating tool" << toolAction->id() << ". Activated on:" << toolAction->visibilityCode() << ", prio:" << toolAction->priority();

    KoToolBase *tool = toolAction->toolFactory()->createTool(controller->canvas());
    if (tool) {
        tool->setFactory(toolAction->toolFactory());
        tool->setObjectName(toolAction->id());
    }

    KoZoomTool *zoomTool = dynamic_cast<KoZoomTool*>(tool);
    if (zoomTool) {
        zoomTool->setCanvasController(controller);
    }

    return tool;
}

void KoToolManager::Private::setup()
{
    if (toolActionList.size() > 0)
        return;

    KoShapeRegistry::instance();
    KoToolRegistry *registry = KoToolRegistry::instance();
    Q_FOREACH (const QString & id, registry->keys()) {
        toolActionList.append(new KoToolAction(registry->value(id)));
    }
}

void KoToolManager::Private::connectActiveTool()
{
    if (canvasData->activeTool) {
        connect(canvasData->activeTool, SIGNAL(cursorChanged(QCursor)),
                q, SLOT(updateCursor(QCursor)));
        connect(canvasData->activeTool, SIGNAL(activateTool(QString)),
                q, SLOT(switchToolRequested(QString)));
        connect(canvasData->activeTool, SIGNAL(statusTextChanged(QString)),
                q, SIGNAL(changedStatusText(QString)));
        connect(canvasData->activeTool, SIGNAL(textModeChanged(bool)),
                q, SIGNAL(textModeChanged(bool)));

        {
            KoCanvasResourceProvider *resourceManager = canvasData->canvas->canvas()->resourceManager();

            const QHash<int, KoAbstractCanvasResourceInterfaceSP> abstractResources =
                canvasData->activeTool->toolAbstractResources();
            const QHash<int, KoDerivedResourceConverterSP> converters = canvasData->activeTool->toolConverters();
            for (KoAbstractCanvasResourceInterfaceSP abstractResource : abstractResources) {
                resourceManager->setAbstractResource(abstractResource);
            }
            for (KoDerivedResourceConverterSP converter : converters) {
                resourceManager->addDerivedResourceConverter(converter);
            }
        }
    }

    updateCursor(Qt::BlankCursor);
}

void KoToolManager::Private::disconnectActiveTool()
{
    if (canvasData->activeTool) {
        {
            KoCanvasResourceProvider *resourceManager = canvasData->canvas->canvas()->resourceManager();

            const QList<int> abstractKeys = canvasData->activeTool->toolAbstractResources().keys();
            const QList<int> derivedKeys = canvasData->activeTool->toolConverters().keys();
            for (int key : abstractKeys) {
                if (resourceManager->hasAbstractResource(key))
                    resourceManager->removeAbstractResource(key);
            }
            for (int key : derivedKeys) {
                if (resourceManager->hasDerivedResourceConverter(key))
                    resourceManager->removeDerivedResourceConverter(key);
            }
        }

        canvasData->deactivateToolActions();
        Q_EMIT q->aboutToChangeTool(canvasData->canvas);
        canvasData->activeTool->deactivate();
        disconnect(canvasData->activeTool, SIGNAL(cursorChanged(QCursor)),
                   q, SLOT(updateCursor(QCursor)));
        disconnect(canvasData->activeTool, SIGNAL(activateTool(QString)),
                   q, SLOT(switchToolRequested(QString)));
        disconnect(canvasData->activeTool, SIGNAL(statusTextChanged(QString)),
                   q, SIGNAL(changedStatusText(QString)));
        disconnect(canvasData->activeTool, SIGNAL(textModeChanged(bool)),
                   q, SIGNAL(textModeChanged(bool)));
    }

    Q_EMIT q->changedStatusText(QString());
}

void KoToolManager::Private::switchTool(const QString &id)
{
    if (!canvasData) return;

    canvasData->activeToolId = id;
    KoToolBase *tool = canvasData->allTools.value(id);
    if (! tool) {
        return;
    }

    canvasData->activationShapeId = tool->factory()->activationShapeId();

    if (canvasData->activeTool == tool && tool->toolId() != KoInteractionTool_ID)
        return;

    disconnectActiveTool();

    if (canvasData->activeTool) {
        canvasData->mostRecentTools.prepend(canvasData->activeTool);
    }
    canvasData->activeTool = tool;
    canvasData->mostRecentTools.removeOne(tool);

    connectActiveTool();
    postSwitchTool();
}

void KoToolManager::Private::postSwitchTool()
{
#ifndef NDEBUG
    int canvasCount = 1;
    Q_FOREACH (QList<CanvasData*> list, canvasses) {
        bool first = true;
        Q_FOREACH (CanvasData *data, list) {
            if (first) {
                debugFlake << "Canvas" << canvasCount++;
            }
            debugFlake << "  +- Tool:" << data->activeToolId  << (data == canvasData ? " *" : "");
            first = false;
        }
    }
#endif
    Q_ASSERT(canvasData);
    if (!canvasData) return;

    QSet<KoShape*> shapesToOperateOn;
    if (canvasData->activeTool
            && canvasData->activeTool->canvas()
            && canvasData->activeTool->canvas()->shapeManager()) {
        KoSelection *selection = canvasData->activeTool->canvas()->shapeManager()->selection();
        Q_ASSERT(selection);
        QList<KoShape *> shapesDelegatesList = selection->selectedEditableShapesAndDelegates();
        if (!shapesDelegatesList.isEmpty()) {
            shapesToOperateOn = QSet<KoShape*>(shapesDelegatesList.begin(),
                                               shapesDelegatesList.end());
        }
    }

    if (canvasData->canvas->canvas()) {
        updateToolForProxy();

        canvasData->activateToolActions();

        canvasData->activeTool->activate(shapesToOperateOn);
    } else {

        canvasData->activateToolActions();

        canvasData->activeTool->activate(shapesToOperateOn);
    }

    QList<QPointer<QWidget> > optionWidgetList = canvasData->activeTool->optionWidgets();
    if (optionWidgetList.empty()) {
        QWidget *toolWidget;
        QString title = canvasData->activeTool->factory()->toolTip();
        toolWidget = canvasData->dummyToolWidget;
        if (toolWidget == 0) {
            toolWidget = new QWidget();
            toolWidget->setObjectName("DummyToolWidget");
            QVBoxLayout *layout = new QVBoxLayout(toolWidget);
            layout->setContentsMargins(3, 3, 3, 3);
            canvasData->dummyToolLabel = new QLabel(toolWidget);
            layout->addWidget(canvasData->dummyToolLabel);
            layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
            canvasData->dummyToolWidget = toolWidget;
        }
        Q_UNUSED(title);
        canvasData->dummyToolLabel->setText(i18n("No options"));
        optionWidgetList.append(toolWidget);
    }

    Q_EMIT q->changedTool(canvasData->canvas);

    Q_EMIT q->toolOptionWidgetsChanged(canvasData->canvas, optionWidgetList);
}

void KoToolManager::Private::switchCanvasData(CanvasData *cd)
{
    Q_ASSERT(cd);

    KoCanvasBase *oldCanvas = 0;
    KoInputDevice oldInputDevice;

    if (canvasData) {
        oldCanvas = canvasData->canvas->canvas();
        oldInputDevice = canvasData->inputDevice;

        if (canvasData->activeTool) {
            disconnectActiveTool();
        }

        KoToolProxy *proxy = proxies.value(oldCanvas);
        Q_ASSERT(proxy);
        proxy->setActiveTool(0);
    }

    canvasData = cd;
    inputDevice = canvasData->inputDevice;

    if (canvasData->activeTool) {
        const QString krimbleWanted = qApp->property("krimbleLastToolId").toString();
        if (!krimbleWanted.isEmpty() && canvasData->activeToolId != krimbleWanted) {
            if (KoToolBase *wantedTool = canvasData->allTools.value(krimbleWanted)) {
                canvasData->mostRecentTools.prepend(canvasData->activeTool);
                canvasData->mostRecentTools.removeOne(wantedTool);
                canvasData->activeTool = wantedTool;
                canvasData->activeToolId = krimbleWanted;
                canvasData->activationShapeId = wantedTool->factory()->activationShapeId();
            }
        }
    }

    if (canvasData->activeTool) {
        connectActiveTool();
        postSwitchTool();
    }

    if (oldInputDevice != canvasData->inputDevice) {
        Q_EMIT q->inputDeviceChanged(canvasData->inputDevice);
    }

    if (oldCanvas != canvasData->canvas->canvas()) {
        Q_EMIT q->changedCanvas(canvasData->canvas->canvas());
    }
}

void KoToolManager::Private::detachCanvas(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    if (canvasData && canvasData->canvas == controller) {
        KoCanvasController *newCanvas = 0;
        Q_FOREACH (KoCanvasController* canvas, canvasses.keys()) {
            if (canvas != controller) {
                newCanvas = canvas;
                break;
            }
        }
        if (newCanvas) {
            switchCanvasData(canvasses.value(newCanvas).first());
        } else {
            disconnectActiveTool();
            Q_EMIT q->toolOptionWidgetsChanged(controller, QList<QPointer<QWidget> >());
            canvasData = 0;
        }
    }

    KoToolProxy *proxy = proxies.value(controller->canvas());
    if (proxy)
        proxy->setActiveTool(0);

    QList<KoToolBase *> tools;
    Q_FOREACH (CanvasData *canvasData, canvasses.value(controller)) {
        Q_FOREACH (KoToolBase *tool, canvasData->allTools) {
            if (! tools.contains(tool)) {
                tools.append(tool);
            }
        }
        delete canvasData;
    }
    Q_FOREACH (KoToolBase *tool, tools) {
        delete tool;
    }
    canvasses.remove(controller);
    Q_EMIT q->changedCanvas(canvasData ? canvasData->canvas->canvas() : 0);
}

void KoToolManager::Private::attachCanvas(KoCanvasController *controller)
{
    Q_ASSERT(controller);
    CanvasData *cd = createCanvasData(controller, KoInputDevice::mouse());

    switchCanvasData(cd);

    inputDevice = cd->inputDevice;
    QList<CanvasData*> canvasses_;
    canvasses_.append(cd);
    canvasses[controller] = canvasses_;

    KoToolProxy *tp = proxies[controller->canvas()];
    if (tp)
        tp->priv()->setCanvasController(controller);

    if (cd->activeTool == 0) {
        int highestPriority = INT_MAX;
        KoToolAction * helper = 0;
        const QString krimbleLastTool = qApp->property("krimbleLastToolId").toString();
        if (!krimbleLastTool.isEmpty()) {
            Q_FOREACH (KoToolAction * th, toolActionList) {
                if (th->id() == krimbleLastTool) { helper = th; break; }
            }
        }
        if (!helper)
        Q_FOREACH (KoToolAction * th, toolActionList) {
            if (th->id() == QLatin1String("PanTool")) { helper = th; break; }
        }
        if (!helper)
        Q_FOREACH (KoToolAction * th, toolActionList) {
            if (th->section() == ToolBoxSection::Main) {
                if (th->priority() < highestPriority) {
                    highestPriority = qMin(highestPriority, th->priority());
                    helper = th;
                }
            }
        }
        if (helper)
            switchTool(helper->id());
    }

    Connector *connector = new Connector(controller->canvas()->shapeManager());
    connect(connector, SIGNAL(selectionChanged(QList<KoShape*>)), q,
            SLOT(selectionChanged(QList<KoShape*>)));
    connect(controller->canvas()->selectedShapesProxy(),
            SIGNAL(currentLayerChanged(const KoShapeLayer*)),
            q, SLOT(currentLayerChanged(const KoShapeLayer*)));

    Q_EMIT q->changedCanvas(canvasData ? canvasData->canvas->canvas() : 0);
}

void KoToolManager::Private::movedFocus(QWidget *from, QWidget *to)
{
    Q_UNUSED(from);
    if (!canvasData || to == 0) {
        return;
    }

    if (to == canvasData->canvas->canvas()->canvasWidget()) {
        return;
    }

    KoCanvasController *newCanvas = 0;
    Q_FOREACH (KoCanvasController* canvas, canvasses.keys()) {
        if (canvas->canvas()->canvasWidget() == to) {
            newCanvas = canvas;
            break;
        }
    }

    if (newCanvas == 0) {
        return;
    }

    Q_FOREACH (CanvasData *data, canvasses.value(newCanvas)) {
        if (data->inputDevice == inputDevice) {
            switchCanvasData(data);
            return;
        }
    }
    switchCanvasData(canvasses.value(newCanvas).first());
}

void KoToolManager::Private::updateCursor(const QCursor &cursor)
{
    Q_ASSERT(canvasData);
    Q_ASSERT(canvasData->canvas);
    Q_ASSERT(canvasData->canvas->canvas());
    canvasData->canvas->canvas()->setCursor(cursor);
}

void KoToolManager::Private::selectionChanged(const QList<KoShape*> &shapes)
{
    QList<QString> types;
    Q_FOREACH (KoShape *shape, shapes) {
        QSet<KoShape*> delegates = shape->toolDelegates();
        if (delegates.isEmpty()) {
            delegates << shape;
        }

        foreach (KoShape *shape2, delegates) {
            Q_ASSERT(shape2);
            if (! types.contains(shape2->shapeId())) {
                types.append(shape2->shapeId());
            }
        }
    }

    const QStringList activationShapeIds = canvasData->activationShapeId.split(',');

    if (!(canvasData->activationShapeId.isNull() && shapes.size() > 0)
            && !activationShapeIds.contains("flake/always")
            && !activationShapeIds.contains("flake/edit")) {

        bool currentToolWorks = false;
        foreach (const QString &type, types) {
            if (activationShapeIds.contains(type)) {
                currentToolWorks = true;
                break;
            }
        }
        if (!currentToolWorks) {
            switchTool(KoInteractionTool_ID);
        }
    }

    Q_EMIT q->toolCodesSelected(types);
}

void KoToolManager::Private::currentLayerChanged(const KoShapeLayer *layer)
{
    Q_EMIT q->currentLayerChanged(canvasData->canvas, layer);
    layerExplicitlyDisabled = layer && !layer->isShapeEditable();
    updateToolForProxy();

    debugFlake << "Layer changed to" << layer << "explicitly disabled:" << layerExplicitlyDisabled;
}

void KoToolManager::Private::updateToolForProxy()
{
    KoToolProxy *proxy = proxies.value(canvasData->canvas->canvas());
    if(!proxy) return;

    bool canUseTool = !layerExplicitlyDisabled || canvasData->activationShapeId.endsWith(QLatin1String("/always"));
    proxy->setActiveTool(canUseTool ? canvasData->activeTool : 0);
}

void KoToolManager::Private::switchInputDevice(const KoInputDevice &device)
{
    Q_ASSERT(canvasData);
    if (!canvasData) return;
    if (inputDevice == device) return;
    if (inputDevice.isMouse() && device.isMouse()) return;
    if (device.isMouse() && !inputDevice.isMouse()) {
        return;
    }

    QList<CanvasData*> items = canvasses[canvasData->canvas];

    Q_FOREACH (CanvasData *cd, items) {
        if (cd->inputDevice == device) {
            switchCanvasData(cd);

            if (!canvasData->activeTool) {
                switchTool(KoInteractionTool_ID);
            }

            return;
        }
    }

    CanvasData *cd = createCanvasData(canvasData->canvas, device);
    QString oldTool = canvasData->activeToolId;

    items.append(cd);
    canvasses[cd->canvas] = items;

    switchCanvasData(cd);

    switchTool(oldTool);
}

void KoToolManager::Private::registerToolProxy(KoToolProxy *proxy, KoCanvasBase *canvas)
{
    proxies.insert(canvas, proxy);
    Q_FOREACH (KoCanvasController *controller, canvasses.keys()) {
        if (controller->canvas() == canvas) {
            proxy->priv()->setCanvasController(controller);
            break;
        }
    }
}

#include "moc_KoToolManager.cpp"
