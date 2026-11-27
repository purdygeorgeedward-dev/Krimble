/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2002 Joseph Wenninger <jowenn@kde.org>
    SPDX-FileCopyrightText: 2005-2007 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kactioncollection.h"
#include "config-xmlgui.h"
#include "kactioncategory.h"
#include "kxmlguiclient.h"
#include "kxmlguifactory.h"
#include "kis_action_registry.h"

#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <QDebug>
#include <QDomDocument>
#include <QSet>
#include <QGuiApplication>
#include <QMap>
#include <QList>
#include <QAction>
#include <QMetaMethod>
#include <QTextStream>
#include <QWidget>

#include <stdio.h>

#if defined(KCONFIG_BEFORE_5_24)
# define authorizeAction authorizeKAction
#endif

class KisKActionCollectionPrivate
{
public:
    KisKActionCollectionPrivate()
        : m_parentGUIClient(0L),
          configGroup(QStringLiteral("Shortcuts")),
          q(0)

    {
    }

    void setComponentForAction(QAction *action)
    {
        Q_UNUSED(action);
    }

    static QList<KisKActionCollection *> s_allCollections;

    void _k_associatedWidgetDestroyed(QObject *obj);
    void _k_actionDestroyed(QObject *obj);

    bool writeKisKXMLGUIConfigFile();

    QString m_componentName;
    QString m_componentDisplayName;

    QAction *unlistAction(QObject *);

    QMap<QString, QAction *> actionByName;
    QList<QAction *> actions;

    const KisKXMLGUIClient *m_parentGUIClient {nullptr};

    QString configGroup;

    bool connectTriggered {false};
    bool connectHovered {false};

    KisKActionCollection *q {nullptr};

    QList<QWidget *> associatedWidgets;
};

QList<KisKActionCollection *> KisKActionCollectionPrivate::s_allCollections;

KisKActionCollection::KisKActionCollection(QObject *parent, const QString &cName)
    : QObject(parent)
    , d(new KisKActionCollectionPrivate)
{
    d->q = this;
    KisKActionCollectionPrivate::s_allCollections.append(this);

    setComponentName(cName);
}

KisKActionCollection::KisKActionCollection(const KisKXMLGUIClient *parent)
    : QObject(0)
    , d(new KisKActionCollectionPrivate)
{
    d->q = this;
    KisKActionCollectionPrivate::s_allCollections.append(this);

    d->m_parentGUIClient = parent;
    d->m_componentName = parent->componentName();
}

KisKActionCollection::~KisKActionCollection()
{
    KisKActionCollectionPrivate::s_allCollections.removeAll(this);

    delete d;
}

QList<KisKActionCategory *> KisKActionCollection::categories() const
{
    return this->findChildren<KisKActionCategory *>();
}

KisKActionCategory *KisKActionCollection::getCategory(const QString &name) {
    KisKActionCategory *category = 0;
    foreach (KisKActionCategory *c, categories()) {
        if (c->text() == name) {
            category = c;
        }
    }

    if (category == 0) {
        category = new KisKActionCategory(name, this);
    }
    return category;
}

void KisKActionCollection::clear()
{
    d->actionByName.clear();
    qDeleteAll(d->actions);
    d->actions.clear();
}

QAction *KisKActionCollection::action(const QString &name) const
{
    QAction *action = 0L;

    if (!name.isEmpty()) {
        action = d->actionByName.value(name);
    }

    return action;
}

QAction *KisKActionCollection::action(int index) const
{
    return actions().value(index);
}

int KisKActionCollection::count() const
{
    return d->actions.count();
}

bool KisKActionCollection::isEmpty() const
{
    return count() == 0;
}

void KisKActionCollection::setComponentName(const QString &cName)
{
    if (count() > 0) {
        qWarning() << "this does not work on a KisKActionCollection containing actions!";
    }

    if (!cName.isEmpty()) {
        d->m_componentName = cName;
    } else {
        d->m_componentName = QCoreApplication::applicationName();
    }
}

QString KisKActionCollection::componentName() const
{
    return d->m_componentName;
}

void KisKActionCollection::setComponentDisplayName(const QString &displayName)
{
    d->m_componentDisplayName = displayName;
}

QString KisKActionCollection::componentDisplayName() const
{
    if (!d->m_componentDisplayName.isEmpty()) {
        return d->m_componentDisplayName;
    }
    if (!QGuiApplication::applicationDisplayName().isEmpty()) {
        return QGuiApplication::applicationDisplayName();
    }
    return QCoreApplication::applicationName();
}

const KisKXMLGUIClient *KisKActionCollection::parentGUIClient() const
{
    return d->m_parentGUIClient;
}

QList<QAction *> KisKActionCollection::actions() const
{
    return d->actions;
}

const QList< QAction * > KisKActionCollection::actionsWithoutGroup() const
{
    QList<QAction *> ret;
    Q_FOREACH (QAction *action, d->actions)
        if (!action->actionGroup()) {
            ret.append(action);
        }
    return ret;
}

const QList< QActionGroup * > KisKActionCollection::actionGroups() const
{
    QSet<QActionGroup *> set;
    Q_FOREACH (QAction *action, d->actions)
        if (action->actionGroup()) {
            set.insert(action->actionGroup());
        }
    return QList<QActionGroup*>(set.begin(), set.end());
}

QAction *KisKActionCollection::addCategorizedAction(const QString &name, QAction *action, const QString &categoryName)
{
    return getCategory(categoryName)->addAction(name, action);
}

QAction *KisKActionCollection::addAction(const QString &name, QAction *action)
{
    if (!action) {
        return action;
    }

    const QString objectName = action->objectName();
    QString indexName = name;

    if (indexName.isEmpty()) {
        indexName = objectName;

    } else {
        action->setObjectName(indexName);
    }

    if (indexName.isEmpty()) {
        QTextStream(&indexName) << (void *)action;

        action->setObjectName(indexName);
    }

    Q_ASSERT(!action->objectName().isEmpty());

    if (d->actionByName.value(indexName, 0) == action) {
        Q_ASSERT(d->actionByName.count(indexName) == 1);
        return action;
    }

    if (QAction *oldAction = d->actionByName.value(indexName)) {
        takeAction(oldAction);
    }

    const int oldIndex = d->actions.indexOf(action);
    if (oldIndex != -1) {
        d->actionByName.remove(d->actionByName.key(action));
        d->actions.removeAt(oldIndex);
    }

    d->actionByName.insert(indexName, action);
    d->actions.append(action);

    Q_FOREACH (QWidget *widget, d->associatedWidgets) {
        widget->addAction(action);
    }

    connect(action, SIGNAL(destroyed(QObject*)), SLOT(_k_actionDestroyed(QObject*)));

    d->setComponentForAction(action);

    if (d->connectHovered) {
        connect(action, SIGNAL(hovered()), SLOT(slotActionHovered()));
    }

    if (d->connectTriggered) {
        connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered()));
    }

    Q_EMIT inserted(action);
    return action;
}

void KisKActionCollection::addActions(const QList<QAction *> &actions)
{
    Q_FOREACH (QAction *action, actions) {
        addAction(action->objectName(), action);
    }
}

void KisKActionCollection::removeAction(QAction *action)
{
    delete takeAction(action);
}

QAction *KisKActionCollection::takeAction(QAction *action)
{
    if (!d->unlistAction(action)) {
        return 0;
    }

    Q_FOREACH (QWidget *widget, d->associatedWidgets) {
        widget->removeAction(action);
    }

    action->disconnect(this);

    return action;
}

QAction *KisKActionCollection::addAction(KStandardAction::StandardAction actionType, const QObject *receiver, const char *member)
{
    QAction *action = KStandardAction::create(actionType, receiver, member, this);
    return action;
}

QAction *KisKActionCollection::addAction(KStandardAction::StandardAction actionType, const QString &name,
                                      const QObject *receiver, const char *member)
{
    QAction *action = KStandardAction::create(actionType, receiver, member, 0);
    action->setParent(this);
    action->setObjectName(name);
    return addAction(name, action);
}

QAction *KisKActionCollection::addAction(const QString &name, const QObject *receiver, const char *member)
{
    QAction *a = new QAction(this);
    if (receiver && member) {
        connect(a, SIGNAL(triggered(bool)), receiver, member);
    }
    return addAction(name, a);
}

QKeySequence KisKActionCollection::defaultShortcut(QAction *action) const
{
    const QList<QKeySequence> shortcuts = defaultShortcuts(action);
    return shortcuts.isEmpty() ? QKeySequence() : shortcuts.first();
}

QList<QKeySequence> KisKActionCollection::defaultShortcuts(QAction *action) const
{
    return action->property("defaultShortcuts").value<QList<QKeySequence> >();
}

void KisKActionCollection::setDefaultShortcut(QAction *action, const QKeySequence &shortcut)
{
    setDefaultShortcuts(action, QList<QKeySequence>() << shortcut);
}

void KisKActionCollection::setDefaultShortcuts(QAction *action, const QList<QKeySequence> &shortcuts)
{
    action->setShortcuts(shortcuts);
    action->setProperty("defaultShortcuts", QVariant::fromValue(shortcuts));
}

bool KisKActionCollection::isShortcutsConfigurable(QAction *action) const
{
    const QVariant value = action->property("isShortcutConfigurable");
    return value.isValid() ? value.toBool() : true;
}

void KisKActionCollection::setShortcutsConfigurable(QAction *action, bool configurable)
{
    action->setProperty("isShortcutConfigurable", configurable);
}

QString KisKActionCollection::configGroup() const
{
    return d->configGroup;
}

void KisKActionCollection::setConfigGroup(const QString &group)
{
    d->configGroup = group;
}

void KisKActionCollection::updateShortcuts()
{
    auto actionRegistry = KisActionRegistry::instance();

    for (QMap<QString, QAction *>::ConstIterator it = d->actionByName.constBegin();
         it != d->actionByName.constEnd(); ++it) {
        actionRegistry->updateShortcut(it.key(), it.value());
    }
}

void KisKActionCollection::readSettings()
{
    auto ar = KisActionRegistry::instance();
    ar->loadCustomShortcuts();

    for (QMap<QString, QAction *>::ConstIterator it = d->actionByName.constBegin();
            it != d->actionByName.constEnd(); ++it) {
        QAction *action = it.value();
        if (!action) {
            continue;
        }

        if (isShortcutsConfigurable(action)) {
            QString actionName = it.key();
            ar->updateShortcut(actionName, action);
        }
    }
}

bool KisKActionCollectionPrivate::writeKisKXMLGUIConfigFile()
{
    const KisKXMLGUIClient *kxmlguiClient = q->parentGUIClient();
    if (!kxmlguiClient || kxmlguiClient->xmlFile().isEmpty()) {
        return false;
    }

    QString attrShortcut = QStringLiteral("shortcut");

    QString sXml(KisKXMLGUIFactory::readConfigFile(kxmlguiClient->xmlFile(), q->componentName()));
    QDomDocument doc;
    doc.setContent(sXml);

    QDomElement elem = KisKXMLGUIFactory::actionPropertiesElement(doc);

    for (QMap<QString, QAction *>::ConstIterator it = actionByName.constBegin();
            it != actionByName.constEnd(); ++it) {
        QAction *action = it.value();
        if (!action) {
            continue;
        }

        QString actionName = it.key();

        if (actionName.startsWith(QLatin1String("unnamed-"))) {
            qCritical() << "Skipped writing shortcut for action " << actionName << "(" << action->text() << ")!";
            continue;
        }

        bool bSameAsDefault = (action->shortcuts() == q->defaultShortcuts(action));

        QDomElement act_elem = KisKXMLGUIFactory::findActionByName(elem, actionName, !bSameAsDefault);
        if (act_elem.isNull()) {
            continue;
        }

        if (bSameAsDefault) {
            act_elem.removeAttribute(attrShortcut);
            if (act_elem.attributes().count() == 1) {
                elem.removeChild(act_elem);
            }
        } else {
            act_elem.setAttribute(attrShortcut, QKeySequence::listToString(action->shortcuts()));
        }
    }

    KisKXMLGUIFactory::saveConfigFile(doc, kxmlguiClient->localXMLFile(), q->componentName());
    return true;
}

void KisKActionCollection::writeSettings(KConfigGroup *config,
                                      bool writeScheme,
                                      QAction *oneAction) const
{
    if (config == 0 && d->writeKisKXMLGUIConfigFile()) {
        return;
    }

    KConfigGroup cg(KSharedConfig::openConfig(), configGroup());
    if (!config) {
        config = &cg;
    }

    QList<QAction *> writeActions;
    if (oneAction) {
        writeActions.append(oneAction);
    } else {
        writeActions = actions();
    }

    for (QMap<QString, QAction *>::ConstIterator it = d->actionByName.constBegin();
            it != d->actionByName.constEnd(); ++it) {

        QAction *action = it.value();
        if (!action) {
            continue;
        }

        QString actionName = it.key();

        if (actionName.startsWith(QLatin1String("unnamed-"))) {
            qCritical() << "Skipped saving shortcut for action without name " \
                        << action->text() << "!";
            continue;
        }

        if (isShortcutsConfigurable(action)) {
            bool bConfigHasAction = !config->readEntry(actionName, QString()).isEmpty();
            bool bSameAsDefault = (action->shortcuts() == defaultShortcuts(action));

            KConfigGroup::WriteConfigFlags flags = KConfigGroup::Persistent;

            if (writeScheme || !bSameAsDefault) {
                QString s = QKeySequence::listToString(action->shortcuts());
                if (s.isEmpty()) {
                    s = QStringLiteral("none");
                }
                config->writeEntry(actionName, s, flags);
            } else if (bConfigHasAction) {
                config->deleteEntry(actionName, flags);
            }
        }
    }

    config->sync();
}

void KisKActionCollection::slotActionTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Q_EMIT actionTriggered(action);
    }
}

void KisKActionCollection::slotActionHighlighted()
{
    slotActionHovered();
}

void KisKActionCollection::slotActionHovered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        Q_EMIT actionHighlighted(action);
        Q_EMIT actionHovered(action);
    }
}

void KisKActionCollectionPrivate::_k_actionDestroyed(QObject *obj)
{
    unlistAction(obj);
}

void KisKActionCollection::connectNotify(const QMetaMethod &signal)
{
    if (d->connectHovered && d->connectTriggered) {
        return;
    }

    if (signal.methodSignature() == "actionHighlighted(QAction*)" ||
            signal.methodSignature() == "actionHovered(QAction*)") {
        if (!d->connectHovered) {
            d->connectHovered = true;
            Q_FOREACH (QAction *action, actions()) {
                connect(action, SIGNAL(hovered()), SLOT(slotActionHovered()));
            }
        }

    } else if (signal.methodSignature() == "actionTriggered(QAction*)") {
        if (!d->connectTriggered) {
            d->connectTriggered = true;
            Q_FOREACH (QAction *action, actions()) {
                connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered()));
            }
        }
    }

    QObject::connectNotify(signal);
}

const QList< KisKActionCollection * > &KisKActionCollection::allCollections()
{
    return KisKActionCollectionPrivate::s_allCollections;
}

void KisKActionCollection::associateWidget(QWidget *widget) const
{
    Q_FOREACH (QAction *action, actions()) {
        if (!widget->actions().contains(action)) {
            widget->addAction(action);
        }
    }
}

void KisKActionCollection::addAssociatedWidget(QWidget *widget)
{
    if (!d->associatedWidgets.contains(widget)) {
        widget->addActions(actions());

        d->associatedWidgets.append(widget);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_k_associatedWidgetDestroyed(QObject*)));
    }
}

void KisKActionCollection::removeAssociatedWidget(QWidget *widget)
{
    Q_FOREACH (QAction *action, actions()) {
        widget->removeAction(action);
    }

    d->associatedWidgets.removeAll(widget);
    disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_k_associatedWidgetDestroyed(QObject*)));
}

QAction *KisKActionCollectionPrivate::unlistAction(QObject *action)
{

    const auto indexOf = [&](const QObject *action, int from = 0) -> int {
        for (int i = from; i < actions.size(); i++) {
            if (actions[i] == action) {
                return i;
            }
        }
        return -1;
    };

    int index = indexOf(action);

    if (index == -1) {
        return 0;
    }

    Q_ASSERT(indexOf(action, index + 1) == -1);

    const QString name = action->objectName();

    actionByName.remove(name);
    QAction *tmp = actions.at(index);
    actions.removeAt(index);

    QList<KisKActionCategory *> categories = q->findChildren<KisKActionCategory *>();
    Q_FOREACH (KisKActionCategory *category, categories) {
        category->unlistAction(tmp);
    }

    return tmp;
}

QList< QWidget * > KisKActionCollection::associatedWidgets() const
{
    return d->associatedWidgets;
}

void KisKActionCollection::clearAssociatedWidgets()
{
    Q_FOREACH (QWidget *widget, d->associatedWidgets)
        Q_FOREACH (QAction *action, actions()) {
            widget->removeAction(action);
        }

    d->associatedWidgets.clear();
}

void KisKActionCollectionPrivate::_k_associatedWidgetDestroyed(QObject *obj)
{
    associatedWidgets.removeAll(static_cast<QWidget *>(obj));
}

#include "moc_kactioncollection.cpp"
