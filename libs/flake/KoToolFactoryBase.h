/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_TOOL_FACTORY_H
#define KO_TOOL_FACTORY_H

#include "kritaflake_export.h"

#include <QString>
#include <QList>
#include <QObject>

class KoCanvasBase;
class KoToolBase;
class QKeySequence;
class KisKActionCollection;
class QAction;

namespace ToolBoxSection {
    static const QString Main {"main"};
    static const QString Shape {"0 Krita/Shape"};
    static const QString PSOrder {"1 Krimble/PSOrder"};
    static const QString Transform {"2 Krita/Transform"};
    static const QString Fill {"3 Krita/Fill"};
    static const QString View {"4 Krita/View"};
    static const QString Select {"5 Krita/Select"};
    static const QString Navigation {"navigation"};
}

class KRITAFLAKE_EXPORT KoToolFactoryBase : public QObject
{

    Q_OBJECT

public:
    explicit KoToolFactoryBase(const QString &id);
    virtual ~KoToolFactoryBase();

    QList<QAction *> createActions(KisKActionCollection *actionCollection);

    virtual KoToolBase *createTool(KoCanvasBase *canvas) = 0;

    QString id() const;
    int priority() const;
    int toolBoxPriority() const;
    QString section() const;
    QString toolTip() const;
    QString iconName() const;

    QString activationShapeId() const;

    QKeySequence shortcut() const;

protected:

    void setShortcut(const QKeySequence & shortcut);

    void setToolTip(const QString &tooltip);

    void setSection(const QString &section);

    void setIconName(const char *iconName);
    void setIconName(const QString &iconName);

    void setPriority(int newPriority);
    void setToolBoxPriority(int priority);
    void setActivationShapeId(const QString &activationShapeId);

    virtual QList<QAction *> createActionsImpl();

private Q_SLOTS:

    void activateTool();

private:
    class Private;
    Private * const d;
};

#endif
