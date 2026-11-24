/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 1999, 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kxmlguifactory.h"

#include "config-xmlgui.h"

#include "kxmlguifactory_p.h"
#include "kxmlguiclient.h"
#include "kxmlguibuilder.h"
#include "KisShortcutsDialog.h"
#include "kactioncollection.h"

#include <QAction>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QWidget>
#include <QDate>
#include <QVariant>
#include <QTextCodec>
#include <QStandardPaths>
#include <QDebug>

#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <kis_icon_utils.h>
#include <WidgetUtilsDebug.h>

#include <KisPortingUtils.h>

Q_DECLARE_METATYPE(QList<QKeySequence>)

using namespace KisKXMLGUI;

class KisKXMLGUIFactoryPrivate : public BuildState
{
public:
    enum ShortcutOption { SetActiveShortcut = 1, SetDefaultShortcut = 2};

    KisKXMLGUIFactoryPrivate()
    {
        m_rootNode = new ContainerNode(0L, QString(), QString());
        m_defaultMergingName = QStringLiteral("<default>");
        tagActionList = QStringLiteral("actionlist");
        attrName = QStringLiteral("name");
    }
    ~KisKXMLGUIFactoryPrivate()
    {
        delete m_rootNode;
    }

    void pushState()
    {
        m_stateStack.push(*this);
    }

    void popState()
    {
        BuildState::operator=(m_stateStack.pop());
    }

    bool emptyState() const
    {
        return m_stateStack.isEmpty();
    }

    QWidget *findRecursive(KisKXMLGUI::ContainerNode *node, bool tag);
    QList<QWidget *> findRecursive(KisKXMLGUI::ContainerNode *node, const QString &tagName);
    void applyActionProperties(const QDomElement &element,
                               ShortcutOption shortcutOption = KisKXMLGUIFactoryPrivate::SetActiveShortcut);
    void configureAction(QAction *action, const QDomNamedNodeMap &attributes,
                         ShortcutOption shortcutOption = KisKXMLGUIFactoryPrivate::SetActiveShortcut);
    void configureAction(QAction *action, const QDomAttr &attribute,
                         ShortcutOption shortcutOption = KisKXMLGUIFactoryPrivate::SetActiveShortcut);

    void refreshActionProperties(KisKXMLGUIClient *client, const QList<QAction *> &actions, const QDomDocument &doc);
    void saveDefaultActionProperties(const QList<QAction *> &actions);

    ContainerNode *m_rootNode;

    QString m_defaultMergingName;

    QString m_containerName;

    QList<KisKXMLGUIClient *> m_clients;

    QString tagActionList;

    QString attrName;

    BuildStateStack m_stateStack;
};

QString KisKXMLGUIFactory::readConfigFile(const QString &filename, const QString &_componentName)
{
    QString componentName = _componentName.isEmpty() ? QCoreApplication::applicationName() : _componentName;
    QString xml_file;

    if (!QDir::isRelativePath(filename)) {
        xml_file = filename;
    } else {
        xml_file = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("kxmlgui5/") + componentName + QLatin1Char('/') + filename);
        if (!QFile::exists(xml_file)) {
            xml_file = QStringLiteral(":/kxmlgui5/") + componentName + QLatin1Char('/') + filename;
        }

        bool warn = false;
        if (!QFile::exists(xml_file)) {
            xml_file = QStandardPaths::locate(QStandardPaths::AppDataLocation, componentName + QLatin1Char('/') + filename);
            warn = true;
        }

        if (!QFile::exists(xml_file)) {
            xml_file = QStandardPaths::locate(QStandardPaths::AppDataLocation, filename);
            warn = true;
        }

        if (warn) {
            qWarning() << "kxmlguifactory: KisKXMLGUI file found at deprecated location" << xml_file << "-- please use ${KisKXMLGUI_INSTALL_DIR} to install these files instead.";
        }
    }

    QFile file(xml_file);
    if (xml_file.isEmpty() || !file.open(QIODevice::ReadOnly)) {
        qCritical() << "No such XML file" << filename;
        return QString();
    }

    QByteArray buffer(file.readAll());
    return QString::fromUtf8(buffer.constData(), buffer.size());
}

bool KisKXMLGUIFactory::saveConfigFile(const QDomDocument &doc,
                                    const QString &filename, const QString &_componentName)
{
    QString componentName = _componentName.isEmpty() ? QCoreApplication::applicationName() : _componentName;
    QString xml_file(filename);

    if (QDir::isRelativePath(xml_file))
        xml_file = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            QStringLiteral("/kxmlgui5/") + componentName + QLatin1Char('/') + filename;

    QFileInfo fileInfo(xml_file);
    QDir().mkpath(fileInfo.absolutePath());
    QFile file(xml_file);
    if (xml_file.isEmpty() || !file.open(QIODevice::WriteOnly)) {
        qCritical() << "Could not write to" << filename;
        return false;
    }

    QTextStream ts(&file);
    KisPortingUtils::setUtf8OnStream(ts);
    ts << doc;

    file.close();
    return true;
}

KisKXMLGUIFactory::KisKXMLGUIFactory(KisKXMLGUIBuilder *builder, QObject *parent)
    : QObject(parent), d(new KisKXMLGUIFactoryPrivate)
{
    d->builder = builder;
    d->guiClient = 0;
    if (d->builder) {
        d->builderContainerTags = d->builder->containerTags();
        d->builderCustomTags = d->builder->customTags();
    }
}

KisKXMLGUIFactory::~KisKXMLGUIFactory()
{
    Q_FOREACH (KisKXMLGUIClient *client, d->m_clients) {
        client->setFactory(0L);
    }
    delete d;
}

void KisKXMLGUIFactory::addClient(KisKXMLGUIClient *client)
{
    debugWidgetUtils << client;
    if (client->factory()) {
        if (client->factory() == this) {
            return;
        } else {
            client->factory()->removeClient(client);
        }
    }

    if (d->emptyState()) {
        Q_EMIT makingChanges(true);
    }
    d->pushState();

    d->guiClient = client;

    if (!d->m_clients.contains(client)) {
        d->m_clients.append(client);
    }
    else {
        debugWidgetUtils << "XMLGUI client already added " << client;
    }
    client->beginXMLPlug(d->builder->widget());

    QDomDocument doc = client->xmlguiBuildDocument();
    if (doc.documentElement().isNull()) {
        doc = client->domDocument();
    }

    QDomElement docElement = doc.documentElement();

    d->m_rootNode->index = -1;

    d->clientName = docElement.attribute(d->attrName);
    d->clientBuilder = client->clientBuilder();

    if (d->clientBuilder) {
        d->clientBuilderContainerTags = d->clientBuilder->containerTags();
        d->clientBuilderCustomTags = d->clientBuilder->customTags();
    } else {
        d->clientBuilderContainerTags.clear();
        d->clientBuilderCustomTags.clear();
    }

    d->saveDefaultActionProperties(client->actionCollection()->actions());
    if (!doc.isNull()) {
        d->refreshActionProperties(client, client->actionCollection()->actions(), doc);
    }

    BuildHelper(*d, d->m_rootNode).build(docElement);

    client->setFactory(this);

    d->builder->finalizeGUI(d->guiClient);

    d->BuildState::reset();

    client->endXMLPlug();

    d->popState();

    Q_EMIT clientAdded(client);

    Q_FOREACH (KisKXMLGUIClient *child, client->childClients()) {
        addClient(child);
    }

    if (d->emptyState()) {
        Q_EMIT makingChanges(false);
    }

}

static QDomElement findActionPropertiesElement(const QDomDocument &doc)
{
    const QLatin1String tagActionProp("ActionProperties");
    QDomElement e = doc.documentElement().firstChildElement();
    for (; !e.isNull(); e = e.nextSiblingElement()) {
        if (QString::compare(e.tagName(), tagActionProp, Qt::CaseInsensitive) == 0) {
            return e;
        }
    }
    return QDomElement();
}

void KisKXMLGUIFactoryPrivate::refreshActionProperties(KisKXMLGUIClient *client, const QList<QAction *> &actions, const QDomDocument &doc)
{
    Q_UNUSED(client);
    Q_UNUSED(actions);

    const QDomElement actionPropElement = findActionPropertiesElement(doc);
    if (!actionPropElement.isNull()) {
        applyActionProperties(actionPropElement);
    }
}

void KisKXMLGUIFactoryPrivate::saveDefaultActionProperties(const QList<QAction *> &actions)
{
    Q_FOREACH (QAction *action, actions) {
        if (!action || action->property("_k_DefaultShortcut").isValid()) {
            continue;
        }

        QList<QKeySequence> defaultShortcut = action->property("defaultShortcuts").value<QList<QKeySequence> >();
        QList<QKeySequence> activeShortcut = action->shortcuts();

        if ((!activeShortcut.isEmpty()) && defaultShortcut.isEmpty()) {
            action->setProperty("_k_DefaultShortcut", QVariant::fromValue(activeShortcut));
        } else {
            action->setProperty("_k_DefaultShortcut", QVariant::fromValue(defaultShortcut));
        }
    }
}

void KisKXMLGUIFactory::forgetClient(KisKXMLGUIClient *client)
{
    d->m_clients.removeAll(client);
}

void KisKXMLGUIFactory::removeClient(KisKXMLGUIClient *client)
{

    if (!client || client->factory() != this) {
        return;
    }

    if (d->emptyState()) {
        Q_EMIT makingChanges(true);
    }

    d->m_clients.removeAll(client);

    const QList<KisKXMLGUIClient *> childClients(client->childClients());
    Q_FOREACH (KisKXMLGUIClient *child, childClients) {
        removeClient(child);
    }

    d->pushState();

    d->guiClient = client;
    d->clientName = client->domDocument().documentElement().attribute(d->attrName);
    d->clientBuilder = client->clientBuilder();

    client->setFactory(0L);

    QDomDocument doc = client->xmlguiBuildDocument();
    if (doc.documentElement().isNull()) {
        doc = client->domDocument().cloneNode(true).toDocument();
        client->setXMLGUIBuildDocument(doc);
    }

    d->m_rootNode->destruct(doc.documentElement(), *d);

    d->BuildState::reset();

    client->prepareXMLUnplug(d->builder->widget());

    d->popState();

    if (d->emptyState()) {
        Q_EMIT makingChanges(false);
    }

    Q_EMIT clientRemoved(client);
}

QList<KisKXMLGUIClient *> KisKXMLGUIFactory::clients() const
{
    return d->m_clients;
}

QWidget *KisKXMLGUIFactory::container(const QString &containerName, KisKXMLGUIClient *client,
                                   bool useTagName)
{
    d->pushState();
    d->m_containerName = containerName;
    d->guiClient = client;

    QWidget *result = d->findRecursive(d->m_rootNode, useTagName);

    d->guiClient = 0L;
    d->m_containerName.clear();

    d->popState();

    return result;
}

QList<QWidget *> KisKXMLGUIFactory::containers(const QString &tagName)
{
    return d->findRecursive(d->m_rootNode, tagName);
}

void KisKXMLGUIFactory::reset()
{
    d->m_rootNode->reset();

    d->m_rootNode->clearChildren();
}

void KisKXMLGUIFactory::resetContainer(const QString &containerName, bool useTagName)
{
    if (containerName.isEmpty()) {
        return;
    }

    ContainerNode *container = d->m_rootNode->findContainer(containerName, useTagName);

    if (!container) {
        return;
    }

    ContainerNode *parent = container->parent;
    if (!parent) {
        return;
    }

    parent->removeChild(container);
}

QWidget *KisKXMLGUIFactoryPrivate::findRecursive(KisKXMLGUI::ContainerNode *node, bool tag)
{
    if (((!tag && node->name == m_containerName) ||
            (tag && node->tagName == m_containerName)) &&
            (!guiClient || node->client == guiClient)) {
        return node->container;
    }

    Q_FOREACH (ContainerNode *child, node->children) {
        QWidget *cont = findRecursive(child, tag);
        if (cont) {
            return cont;
        }
    }

    return 0L;
}

static inline bool equals(const QString &str1, const char *str2)
{
    return str1.compare(QLatin1String(str2), Qt::CaseInsensitive) == 0;
}
static inline bool equals(const QString &str1, const QString &str2)
{
    return str1.compare(str2, Qt::CaseInsensitive) == 0;
}

QList<QWidget *> KisKXMLGUIFactoryPrivate::findRecursive(KisKXMLGUI::ContainerNode *node,
        const QString &tagName)
{
    QList<QWidget *> res;

    if (equals(node->tagName, tagName)) {
        res.append(node->container);
    }

    Q_FOREACH (KisKXMLGUI::ContainerNode *child, node->children) {
        res << findRecursive(child, tagName);
    }

    return res;
}

void KisKXMLGUIFactory::plugActionList(KisKXMLGUIClient *client, const QString &name,
                                    const QList<QAction *> &actionList)
{
    d->pushState();
    d->guiClient = client;
    d->actionListName = name;
    d->actionList = actionList;
    d->clientName = client->domDocument().documentElement().attribute(d->attrName);

    d->m_rootNode->plugActionList(*d);

    d->saveDefaultActionProperties(actionList);
    d->refreshActionProperties(client, actionList, client->domDocument());

    d->BuildState::reset();
    d->popState();
}

void KisKXMLGUIFactory::unplugActionList(KisKXMLGUIClient *client, const QString &name)
{
    d->pushState();
    d->guiClient = client;
    d->actionListName = name;
    d->clientName = client->domDocument().documentElement().attribute(d->attrName);

    d->m_rootNode->unplugActionList(*d);

    d->BuildState::reset();
    d->popState();
}

void KisKXMLGUIFactoryPrivate::applyActionProperties(const QDomElement &actionPropElement,
        ShortcutOption shortcutOption)
{
    for (QDomElement e = actionPropElement.firstChildElement();
            !e.isNull(); e = e.nextSiblingElement()) {
        if (!equals(e.tagName(), "action")) {
            continue;
        }

        QAction *action = guiClient->action(e);
        if (!action) {
            continue;
        }

        configureAction(action, e.attributes(), shortcutOption);
    }
}

void KisKXMLGUIFactoryPrivate::configureAction(QAction *action, const QDomNamedNodeMap &attributes,
        ShortcutOption shortcutOption)
{
    for (int i = 0; i < attributes.length(); i++) {
        QDomAttr attr = attributes.item(i).toAttr();
        if (attr.isNull()) {
            continue;
        }

        configureAction(action, attr, shortcutOption);
    }
}

void KisKXMLGUIFactoryPrivate::configureAction(QAction *action, const QDomAttr &attribute,
        ShortcutOption shortcutOption)
{
    QString attrName = attribute.name();
    if (equals(attrName, "accel")) {
        attrName = QStringLiteral("shortcut");
    }

    if (equals(attrName, "name")) {
        return;
    }

    if (equals(attrName, "icon")) {
        action->setIcon(KisIconUtils::loadIcon(attribute.value()));
        return;
    }

    QVariant propertyValue;

    QVariant::Type propertyType = action->property(attrName.toLatin1().constData()).type();
    bool isShortcut = (propertyType == QMetaType::QKeySequence);

    if (propertyType == QMetaType::Int) {
        propertyValue = QVariant(attribute.value().toInt());
    } else if (propertyType == QMetaType::UInt) {
        propertyValue = QVariant(attribute.value().toUInt());
    } else if (isShortcut) {
        action->setShortcuts(QKeySequence::listFromString(attribute.value()));
        if (shortcutOption & KisKXMLGUIFactoryPrivate::SetDefaultShortcut) {
            action->setProperty("defaultShortcuts",
                                QVariant::fromValue(QKeySequence::listFromString(attribute.value())));
        }
    } else {
        propertyValue = QVariant(attribute.value());
    }
    if (!isShortcut && !action->setProperty(attrName.toLatin1().constData(), propertyValue)) {
        qWarning() << "Error: Unknown action property " << attrName << " will be ignored!";
    }
}

QDomElement KisKXMLGUIFactory::actionPropertiesElement(QDomDocument &doc)
{
    QDomElement elem = findActionPropertiesElement(doc);

    if (elem.isNull()) {
        elem = doc.createElement(QStringLiteral("ActionProperties"));
        doc.documentElement().appendChild(elem);
    }
    return elem;
}

QDomElement KisKXMLGUIFactory::findActionByName(QDomElement &elem, const QString &sName, bool create)
{
    const QLatin1String attrName("name");
    for (QDomNode it = elem.firstChild(); !it.isNull(); it = it.nextSibling()) {
        QDomElement e = it.toElement();
        if (e.attribute(attrName) == sName) {
            return e;
        }
    }

    if (create) {
        QDomElement act_elem = elem.ownerDocument().createElement(QStringLiteral("Action"));
        act_elem.setAttribute(attrName, sName);
        elem.appendChild(act_elem);
        return act_elem;
    }
    return QDomElement();
}

