/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KACTIONCOLLECTION_H
#define KACTIONCOLLECTION_H
#include "config-xmlgui.h"
#include <kritawidgetutils_export.h>
#include <kstandardaction.h>
#include <QObject>

class QAction;
class KisKXMLGUIClient;
class KConfigGroup;
class QActionGroup;
class QString;
class KisKActionCategory;

class KRITAWIDGETUTILS_EXPORT KisKActionCollection : public QObject
{
    friend class KisKXMLGUIClient;

    Q_OBJECT

    Q_PROPERTY(QString configGroup READ configGroup WRITE setConfigGroup)

public:
    explicit KisKActionCollection(QObject *parent, const QString &cName = QString());

    ~KisKActionCollection() override;

    static const QList<KisKActionCollection *> &allCollections();

    void clear();

    void associateWidget(QWidget *widget) const;

    void addAssociatedWidget(QWidget *widget);

    void removeAssociatedWidget(QWidget *widget);

    QList<QWidget *> associatedWidgets() const;

    void clearAssociatedWidgets();

    QString configGroup() const;

    void setConfigGroup(const QString &group);

    void readSettings();

    void updateShortcuts();

    void writeSettings(KConfigGroup *config = 0, bool writeScheme = false, QAction *oneAction = 0) const;

    int count() const;

    bool isEmpty() const;

    QAction *action(int index) const;

    QAction *action(const QString &name) const;

    QList<QAction *> actions() const;

    const QList<QAction *> actionsWithoutGroup() const;

    const QList<QActionGroup *> actionGroups() const;

    void setComponentName(const QString &componentName);

    QString componentName() const;

    void setComponentDisplayName(const QString &displayName);

    QString componentDisplayName() const;

    const KisKXMLGUIClient *parentGUIClient() const;

    QList<KisKActionCategory *> categories() const;

    KisKActionCategory *getCategory(const QString &categoryName);

Q_SIGNALS:
    void inserted(QAction *action);

    QT_MOC_COMPAT void actionHighlighted(QAction *action);

    void actionHovered(QAction *action);

    void actionTriggered(QAction *action);

protected:
    void connectNotify(const QMetaMethod &signal) override;

protected Q_SLOTS:
    virtual void slotActionTriggered();

    QT_MOC_COMPAT virtual void slotActionHighlighted();

private Q_SLOTS:
    void slotActionHovered();

public:
    Q_INVOKABLE QAction *addAction(const QString &name, QAction *action);

    Q_INVOKABLE QAction *addCategorizedAction(const QString &name, QAction *action, const QString &categoryName);

    void addActions(const QList<QAction *> &actions);

    void removeAction(QAction *action);

    QAction *takeAction(QAction *action);

    QAction *addAction(KStandardAction::StandardAction actionType, const QObject *receiver = 0, const char *member = 0);

    QAction *addAction(KStandardAction::StandardAction actionType, const QString &name,
                       const QObject *receiver = 0, const char *member = 0);

    QAction *addAction(const QString &name, const QObject *receiver = 0, const char *member = 0);

    template<class ActionType>
    ActionType *add(const QString &name, const QObject *receiver = 0, const char *member = 0)
    {
        ActionType *a = new ActionType(this);
        if (receiver && member) {
            connect(a, SIGNAL(triggered(bool)), receiver, member);
        }
        addAction(name, a);
        return a;
    }

    QKeySequence defaultShortcut(QAction *action) const;

    QList<QKeySequence> defaultShortcuts(QAction *action) const;

    void setDefaultShortcut(QAction *action, const QKeySequence &shortcut);

    Q_INVOKABLE void setDefaultShortcuts(QAction *action, const QList<QKeySequence> &shortcuts);

    bool isShortcutsConfigurable(QAction *action) const;

    void setShortcutsConfigurable(QAction *action, bool configurable);

private:
    Q_PRIVATE_SLOT(d, void _k_actionDestroyed(QObject *))
    Q_PRIVATE_SLOT(d, void _k_associatedWidgetDestroyed(QObject *))

    KisKActionCollection(const KisKXMLGUIClient *parent);

    friend class KisKActionCollectionPrivate;
    class KisKActionCollectionPrivate *const d;
};

#endif
