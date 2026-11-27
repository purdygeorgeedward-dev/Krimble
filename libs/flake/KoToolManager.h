/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006, 2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_TOOL_MANAGER
#define KO_TOOL_MANAGER

#include "KoInputDevice.h"
#include "kritaflake_export.h"
#include "KoDerivedResourceConverter.h"
#include "KoAbstractCanvasResourceInterface.h"

#include <QObject>
#include <QList>

class KoCanvasController;
class KoShapeControllerBase;
class KoToolFactoryBase;
class KoCanvasBase;
class KoToolBase;
class KisKActionCollection;
class KoShape;
class KoShapeLayer;
class QKeySequence;

class QCursor;

class KRITAFLAKE_EXPORT KoToolAction : public QObject
{
    Q_OBJECT
public:
    explicit KoToolAction(KoToolFactoryBase *toolFactory);
    ~KoToolAction() override;

public:
    QString id() const;
    QString iconText() const;
    QString toolTip() const;
    QString iconName() const;
    QKeySequence shortcut() const;

    QString section() const;
    int priority() const;
    int toolBoxPriority() const;
    int buttonGroupId() const;
    QString visibilityCode() const;

    KoToolFactoryBase *toolFactory() const;

public Q_SLOTS:
    void trigger();

Q_SIGNALS:
    void changed();

private:
    class Private;
    Private *const d;
};

class KRITAFLAKE_EXPORT KoToolManager : public QObject
{
    Q_OBJECT

public:
    KoToolManager();
    static KoToolManager* instance();
    ~KoToolManager() override;

    void initializeToolActions();

    void addController(KoCanvasController *controller);

    void removeCanvasController(KoCanvasController *controller);
    Q_SLOT void attemptCanvasControllerRemoval(QObject *controller);

    KoCanvasController *activeCanvasController() const;

    KoToolBase *toolById(KoCanvasBase *canvas, const QString &id) const;

    KoInputDevice currentInputDevice() const;

    QString preferredToolForSelection(const QList<KoShape*> &shapes);

    QList<KoToolAction*> toolActionList() const;

    void requestToolActivation(KoCanvasController *controller);

    QString activeToolId() const;

    void initializeCurrentToolForCanvas();

    void setConverter(KoDerivedResourceConverterSP converter, KoToolBase *tool);
    void setAbstractResource(KoAbstractCanvasResourceInterfaceSP abstractResource, KoToolBase *tool);

    class Private;
    KoToolManager::Private *priv();

public Q_SLOTS:
    void switchToolRequested(const QString &id);

    void switchInputDeviceRequested(const KoInputDevice &id);

    void switchBackRequested();

    void themeChanged();

Q_SIGNALS:
    void aboutToChangeTool(KoCanvasController *canvas);

    void changedTool(KoCanvasController *canvas);

    void toolCodesSelected(const QList<QString> &types);

    void currentLayerChanged(const KoCanvasController *canvas, const KoShapeLayer *layer);

    void inputDeviceChanged(const KoInputDevice &device);

    void changedCanvas(const KoCanvasBase *canvas);

    void changedStatusText(const QString &statusText);

    void addedTool(KoToolAction *toolAction, KoCanvasController *canvas);

    void toolOptionWidgetsChanged(KoCanvasController *controller, const QList<QPointer<QWidget> > &widgets);

    void textModeChanged(bool text);

    void createOpacityResource(bool isOpacityPresetMode, KoToolBase *tool);

private:
    KoToolManager(const KoToolManager&);
    KoToolManager operator=(const KoToolManager&);

    Q_PRIVATE_SLOT(d, void detachCanvas(KoCanvasController *controller))
    Q_PRIVATE_SLOT(d, void attachCanvas(KoCanvasController *controller))
    Q_PRIVATE_SLOT(d, void movedFocus(QWidget *from, QWidget *to))
    Q_PRIVATE_SLOT(d, void updateCursor(const QCursor &cursor))
    Q_PRIVATE_SLOT(d, void selectionChanged(const QList<KoShape*> &shapes))
    Q_PRIVATE_SLOT(d, void currentLayerChanged(const KoShapeLayer *layer))

    Private *const d;
};

#endif
