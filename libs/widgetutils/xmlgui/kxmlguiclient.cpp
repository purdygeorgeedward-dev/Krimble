/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kxmlguiclient.h"

#include "kxmlguiversionhandler_p.h"
#include "kxmlguifactory.h"
#include "kxmlguibuilder.h"
#include "kactioncollection.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QPointer>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

#include <klocalizedstring.h>

#include <assert.h>

#if defined(KCONFIG_BEFORE_5_24)
# define authorizeAction authorizeKAction
#endif

class KisKXMLGUIClientPrivate
{
public:
    KisKXMLGUIClientPrivate()
        : m_componentName(QCoreApplication::applicationName()),
          m_actionCollection(0),
          m_parent(0L),
          m_builder(0L)
    {
        m_textTagNames.append(QLatin1String("text"));
        m_textTagNames.append(QLatin1String("Text"));
        m_textTagNames.append(QLatin1String("title"));
    }
    ~KisKXMLGUIClientPrivate()
    {
    }

    bool mergeXML(QDomElement &base, QDomElement &additive,
                  KisKActionCollection *actionCollection);
    bool isEmptyContainer(const QDomElement &base,
                          KisKActionCollection *actionCollection) const;

    QDomElement findMatchingElement(const QDomElement &base,
                                    const QDomElement &additive);

    QString m_componentName;

    QDomDocument m_doc;
    KisKActionCollection *m_actionCollection;
    QDomDocument m_buildDocument;
    QPointer<KisKXMLGUIFactory> m_factory;
    KisKXMLGUIClient *m_parent;
    QList<KisKXMLGUIClient *> m_children;
    KisKXMLGUIBuilder *m_builder;
    QString m_xmlFile;
    QString m_localXMLFile;
    QStringList m_textTagNames;

    QMap<QString, KisKXMLGUIClient::StateChange> m_actionsStateMap;
};

KisKXMLGUIClient::KisKXMLGUIClient()
    : d(new KisKXMLGUIClientPrivate)
{
}

KisKXMLGUIClient::KisKXMLGUIClient(KisKXMLGUIClient *parent)
    : d(new KisKXMLGUIClientPrivate)
{
    parent->insertChildClient(this);
}

KisKXMLGUIClient::~KisKXMLGUIClient()
{
    if (d->m_parent) {
        d->m_parent->removeChildClient(this);
    }

    if (d->m_factory) {
        qWarning() << this << "deleted without having been removed from the factory first. This will leak standalone popupmenus and could lead to crashes.";
        d->m_factory->forgetClient(this);
    }

    Q_FOREACH (KisKXMLGUIClient *client, d->m_children) {
        if (d->m_factory) {
            d->m_factory->forgetClient(client);
        }
        assert(client->d->m_parent == this);
        client->d->m_parent = 0;
    }

    delete d->m_actionCollection;
    delete d;
}

QAction *KisKXMLGUIClient::action(const char *name) const
{
    QAction *act = actionCollection()->action(QLatin1String(name));
    if (!act) {
        Q_FOREACH (KisKXMLGUIClient *client, d->m_children) {
            act = client->actionCollection()->action(QLatin1String(name));
            if (act) {
                break;
            }
        }
    }
    return act;
}

KisKActionCollection *KisKXMLGUIClient::actionCollection() const
{
    if (!d->m_actionCollection) {
        d->m_actionCollection = new KisKActionCollection(this);
        d->m_actionCollection->setObjectName(QStringLiteral("KisKXMLGUIClient-KisKActionCollection"));
    }
    return d->m_actionCollection;
}

QAction *KisKXMLGUIClient::action(const QDomElement &element) const
{
    return actionCollection()->action(element.attribute(QStringLiteral("name")));
}

QString KisKXMLGUIClient::componentName() const
{
    return d->m_componentName;
}

QDomDocument KisKXMLGUIClient::domDocument() const
{
    return d->m_doc;
}

QString KisKXMLGUIClient::xmlFile() const
{
    return d->m_xmlFile;
}

QString KisKXMLGUIClient::localXMLFile() const
{
    if (!d->m_localXMLFile.isEmpty()) {
        return d->m_localXMLFile;
    }

    if (!QDir::isRelativePath(d->m_xmlFile)) {
        return QString();
    }

    if (d->m_xmlFile.isEmpty()) {
        return QString();
    }

    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/kxmlgui5/") +
           componentName() + QLatin1Char('/') + d->m_xmlFile;
}

void KisKXMLGUIClient::reloadXML()
{
    QString file(xmlFile());
    if (!file.isEmpty()) {
        setXMLFile(file);
    }
}

void KisKXMLGUIClient::setComponentName(const QString &componentName, const QString &componentDisplayName)
{
    d->m_componentName = componentName;
    actionCollection()->setComponentName(componentName);
    actionCollection()->setComponentDisplayName(componentDisplayName);
    if (d->m_builder) {
        d->m_builder->setBuilderClient(this);
    }
}

QString KisKXMLGUIClient::standardsXmlFileLocation()
{
    QString file = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("ui/ui_standards.xmlgui"));
    if (file.isEmpty()) {
        file = QStringLiteral(":/kxmlgui5/ui_standards.xmlgui");
        Q_ASSERT(QFile::exists(file));
    }
    return file;
}

void KisKXMLGUIClient::loadStandardsXmlFile()
{
    setXML(KisKXMLGUIFactory::readConfigFile(standardsXmlFileLocation()));
}

void KisKXMLGUIClient::setXMLFile(const QString &_file, bool merge, bool setXMLDoc)
{
    if (!_file.isNull()) {
        d->m_xmlFile = _file;
    }

    if (!setXMLDoc) {
        return;
    }

    QString file = _file;
    QStringList allFiles;
    if (!QDir::isRelativePath(file)) {
        allFiles.append(file);
    } else {
        const QString filter = componentName() + QLatin1Char('/') + _file;

        allFiles << QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("kxmlgui5/") + filter);

        const QString qrcFile(QStringLiteral(":/kxmlgui5/") + filter);
        if (QFile::exists(qrcFile)) {
            allFiles << qrcFile;
        }

        const QStringList compatFiles =
                   QStandardPaths::locateAll(QStandardPaths::AppDataLocation, filter) +
                   QStandardPaths::locateAll(QStandardPaths::AppDataLocation, _file);

        if (allFiles.isEmpty() && !compatFiles.isEmpty()) {
            qWarning() << "kxmlguiclient: KisKXMLGUI file found at deprecated location" << compatFiles << "-- please use ${KisKXMLGUI_INSTALL_DIR} to install this file instead.";
        }
        allFiles += compatFiles;
    }
    if (allFiles.isEmpty() && !_file.isEmpty()) {
        qWarning() << "cannot find .xmlgui file" << _file << "for component" << componentName();
    }

    if (!d->m_localXMLFile.isEmpty() && !file.endsWith(QStringLiteral("ui_standards.xmlgui"))) {
        const bool exists = QDir::isRelativePath(d->m_localXMLFile) || QFile::exists(d->m_localXMLFile);
        if (exists && !allFiles.contains(d->m_localXMLFile)) {
            allFiles.prepend(d->m_localXMLFile);
        }
    }

    QString doc;
    if (!allFiles.isEmpty()) {
        file = findMostRecentXMLFile(allFiles, doc);
    }

    setXML(doc, merge);
}

void KisKXMLGUIClient::setLocalXMLFile(const QString &file)
{
    d->m_localXMLFile = file;
}

void KisKXMLGUIClient::replaceXMLFile(const QString &xmlfile, const QString &localxmlfile, bool merge)
{
    if (!QDir::isAbsolutePath(xmlfile)) {
        qWarning() << "xml file" << xmlfile << "is not an absolute path";
    }

    setLocalXMLFile(localxmlfile);
    setXMLFile(xmlfile, merge);
}

static void propagateTranslationDomain(QDomDocument &doc, const QStringList tagNames)
{
    const QLatin1String attrDomain("translationDomain");
    QDomElement base = doc.documentElement();
    QString domain = base.attribute(attrDomain);
    if (domain.isEmpty()) {
        domain = QString::fromUtf8(KLocalizedString::applicationDomain());
        if (domain.isEmpty()) {
            return;
        }
    }
    foreach (const QString &tagName, tagNames) {
        QDomNodeList textNodes = base.elementsByTagName(tagName);
        for (int i = 0; i < textNodes.length(); ++i) {
            QDomElement e = textNodes.item(i).toElement();
            QString localDomain = e.attribute(attrDomain);
            if (localDomain.isEmpty()) {
                 e.setAttribute(attrDomain, domain);
            }
        }
    }
}

void KisKXMLGUIClient::setXML(const QString &document, bool merge)
{
    QDomDocument doc;
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    QString errorMsg;
    int errorLine, errorColumn;
    bool result = document.isEmpty() || doc.setContent(document, &errorMsg, &errorLine, &errorColumn);
    if (result) {
#else
    QDomDocument::ParseResult result = doc.setContent(document);
    if (document.isEmpty() || result) {
#endif
        propagateTranslationDomain(doc, d->m_textTagNames);
        setDOMDocument(doc, merge);
    } else {
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
        qCritical() << "Error parsing XML document:" << errorMsg << "at line" << errorLine << "column" << errorColumn;
#else
        qCritical() << "Error parsing XML document:" << result.errorMessage << "at line" << result.errorLine << "column" << result.errorColumn;
#endif
#ifdef NDEBUG
        setDOMDocument(QDomDocument(), merge);
#else
        abort();
#endif
    }
}

void KisKXMLGUIClient::setDOMDocument(const QDomDocument &document, bool merge)
{
    if (merge && !d->m_doc.isNull()) {
        QDomElement base = d->m_doc.documentElement();

        QDomElement e = document.documentElement();

        d->mergeXML(base, e, actionCollection());

        base = d->m_doc.documentElement();

        if (base.isNull()) {
            d->m_doc = document;
        }
    } else {
        d->m_doc = document;
    }

    setXMLGUIBuildDocument(QDomDocument());
}

static inline bool equalstr(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) == 0;
}
static inline bool equalstr(const QString &a, const QLatin1String &b)
{
    return a.compare(b, Qt::CaseInsensitive) == 0;
}

bool KisKXMLGUIClientPrivate::mergeXML(QDomElement &base, QDomElement &additive, KisKActionCollection *actionCollection)
{
    const QLatin1String tagAction("Action");
    const QLatin1String tagMerge("Merge");
    const QLatin1String tagSeparator("Separator");
    const QLatin1String tagMergeLocal("MergeLocal");
    const QLatin1String tagText("text");
    const QLatin1String attrAppend("append");
    const QString       attrName(QStringLiteral("name"));
    const QString       attrWeakSeparator(QStringLiteral("weakSeparator"));
    const QString       attrAlreadyVisited(QStringLiteral("alreadyVisited"));
    const QString       attrNoMerge(QStringLiteral("noMerge"));
    const QLatin1String attrOne("1");

    if (additive.attribute(attrNoMerge) == attrOne) {
        base.parentNode().replaceChild(additive, base);
        return true;
    } else {
        {
            const QDomNamedNodeMap attribs = additive.attributes();
            const uint attribcount = attribs.count();

            for (uint i = 0; i < attribcount; ++i) {
                const QDomNode node = attribs.item(i);
                base.setAttribute(node.nodeName(), node.nodeValue());
            }
        }

        QDomNode n = base.firstChild();
        while (!n.isNull()) {
            QDomElement e = n.toElement();
            n = n.nextSibling();
            if (e.isNull()) {
                continue;
            }

            const QString tag = e.tagName();

            if (equalstr(tag, tagAction)) {
                const QString name =  e.attribute(attrName);
                if (!actionCollection->action(name)) {
                    base.removeChild(e);
                    continue;
                }
            }

            else if (equalstr(tag, tagSeparator)) {
                e.setAttribute(attrWeakSeparator, (uint)1);

                QDomElement prev = e.previousSibling().toElement();
                if (prev.isNull() ||
                        (equalstr(prev.tagName(), tagSeparator) && !prev.attribute(attrWeakSeparator).isNull()) ||
                        (equalstr(prev.tagName(), tagText))) {
                    base.removeChild(e);
                    continue;
                }
            }

            else if (equalstr(tag, tagMergeLocal)) {
                QDomNode it = additive.firstChild();
                while (!it.isNull()) {
                    QDomElement newChild = it.toElement();
                    it = it.nextSibling();
                    if (newChild.isNull()) {
                        continue;
                    }

                    if (equalstr(newChild.tagName(), tagText)) {
                        continue;
                    }

                    if (newChild.attribute(attrAlreadyVisited) == attrOne) {
                        continue;
                    }

                    QString itAppend(newChild.attribute(attrAppend));
                    QString elemName(e.attribute(attrName));

                    if ((itAppend.isNull() && elemName.isEmpty()) ||
                            (itAppend == elemName)) {
                        QDomElement matchingElement = findMatchingElement(newChild, base);
                        if (matchingElement.isNull() || equalstr(newChild.tagName(), tagSeparator)) {
                            base.insertBefore(newChild, e);
                        }
                    }
                }

                base.removeChild(e);
                continue;
            }

            else if (equalstr(tag, tagText)) {
                continue;
            } else if (equalstr(tag, tagMerge)) {
                continue;
            }

            else {
                QDomElement matchingElement = findMatchingElement(e, additive);
                if (!matchingElement.isNull()) {
                    matchingElement.setAttribute(attrAlreadyVisited, (uint)1);

                    if (mergeXML(e, matchingElement, actionCollection)) {
                        base.removeChild(e);
                        additive.removeChild(matchingElement);
                        continue;
                    }

                    continue;
                } else {
                    QDomElement dummy;
                    if (mergeXML(e, dummy, actionCollection)) {
                        base.removeChild(e);
                    }
                    continue;
                }
            }
        }

        n = additive.firstChild();
        while (!n.isNull()) {
            QDomElement e = n.toElement();
            n = n.nextSibling();
            if (e.isNull()) {
                continue;
            }

            QDomElement matchingElement = findMatchingElement(e, base);

            if (matchingElement.isNull()) {
                base.appendChild(e);
            }
        }

        QDomElement last = base.lastChild().toElement();
        if (equalstr(last.tagName(), tagSeparator) &&
                (!last.attribute(attrWeakSeparator).isNull())) {
            base.removeChild(last);
        }
    }

    return isEmptyContainer(base, actionCollection);
}

bool KisKXMLGUIClientPrivate::isEmptyContainer(const QDomElement &base, KisKActionCollection *actionCollection) const
{
    QDomNode n = base.firstChild();
    while (!n.isNull()) {
        const QDomElement e = n.toElement();
        n = n.nextSibling();
        if (e.isNull()) {
            continue;
        }

        const QString tag = e.tagName();

        if (equalstr(tag, QLatin1String("Action"))) {
            if (actionCollection->action(e.attribute(QStringLiteral("name")))) {
                return false;
            }
        } else if (equalstr(tag, QLatin1String("Separator"))) {
            const QString weakAttr = e.attribute(QStringLiteral("weakSeparator"));
            if (weakAttr.isEmpty() || weakAttr.toInt() != 1) {
                return false;
            }
        }

        else if (equalstr(tag, QLatin1String("merge"))) {
            continue;
        }

        else if (equalstr(tag, QLatin1String("text"))) {
            continue;
        }

        else {
            return false;
        }
    }

    return true;
}

QDomElement KisKXMLGUIClientPrivate::findMatchingElement(const QDomElement &base, const QDomElement &additive)
{
    QDomNode n = additive.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        n = n.nextSibling();
        if (e.isNull()) {
            continue;
        }

        const QString tag = e.tagName();
        if (equalstr(tag, QLatin1String("Action"))
                || equalstr(tag, QLatin1String("MergeLocal"))) {
            continue;
        }

        if (equalstr(tag, base.tagName()) &&
                e.attribute(QStringLiteral("name")) == base.attribute(QStringLiteral("name"))) {
            return e;
        }
    }

    return QDomElement();
}

void KisKXMLGUIClient::setXMLGUIBuildDocument(const QDomDocument &doc)
{
    d->m_buildDocument = doc;
}

QDomDocument KisKXMLGUIClient::xmlguiBuildDocument() const
{
    return d->m_buildDocument;
}

void KisKXMLGUIClient::setFactory(KisKXMLGUIFactory *factory)
{
    d->m_factory = factory;
}

KisKXMLGUIFactory *KisKXMLGUIClient::factory() const
{
    return d->m_factory;
}

KisKXMLGUIClient *KisKXMLGUIClient::parentClient() const
{
    return d->m_parent;
}

void KisKXMLGUIClient::insertChildClient(KisKXMLGUIClient *child)
{
    if (child->d->m_parent) {
        child->d->m_parent->removeChildClient(child);
    }
    d->m_children.append(child);
    child->d->m_parent = this;
}

void KisKXMLGUIClient::removeChildClient(KisKXMLGUIClient *child)
{
    assert(d->m_children.contains(child));
    d->m_children.removeAll(child);
    child->d->m_parent = 0;
}

QList<KisKXMLGUIClient *> KisKXMLGUIClient::childClients()
{
    return d->m_children;
}

void KisKXMLGUIClient::setClientBuilder(KisKXMLGUIBuilder *builder)
{
    d->m_builder = builder;
}

KisKXMLGUIBuilder *KisKXMLGUIClient::clientBuilder() const
{
    return d->m_builder;
}

void KisKXMLGUIClient::plugActionList(const QString &name, const QList<QAction *> &actionList)
{
    if (!d->m_factory) {
        return;
    }

    d->m_factory->plugActionList(this, name, actionList);
}

void KisKXMLGUIClient::unplugActionList(const QString &name)
{
    if (!d->m_factory) {
        return;
    }

    d->m_factory->unplugActionList(this, name);
}

QString KisKXMLGUIClient::findMostRecentXMLFile(const QStringList &files, QString &doc)
{
    KXmlGuiVersionHandler versionHandler(files);
    doc = versionHandler.finalDocument();
    return versionHandler.finalFile();
}

void KisKXMLGUIClient::addStateActionEnabled(const QString &state,
        const QString &action)
{
    StateChange stateChange = getActionsToChangeForState(state);

    stateChange.actionsToEnable.append(action);

    d->m_actionsStateMap.insert(state, stateChange);
}

void KisKXMLGUIClient::addStateActionDisabled(const QString &state,
        const QString &action)
{
    StateChange stateChange = getActionsToChangeForState(state);

    stateChange.actionsToDisable.append(action);

    d->m_actionsStateMap.insert(state, stateChange);
}

KisKXMLGUIClient::StateChange KisKXMLGUIClient::getActionsToChangeForState(const QString &state)
{
    return d->m_actionsStateMap[state];
}

void KisKXMLGUIClient::stateChanged(const QString &newstate, KisKXMLGUIClient::ReverseStateChange reverse)
{
    StateChange stateChange = getActionsToChangeForState(newstate);

    bool setTrue = (reverse == StateNoReverse);
    bool setFalse = !setTrue;

    for (QStringList::const_iterator it = stateChange.actionsToEnable.constBegin();
            it != stateChange.actionsToEnable.constEnd(); ++it) {

        QAction *action = actionCollection()->action(*it);
        if (action) {
            action->setEnabled(setTrue);
        }
    }

    for (QStringList::const_iterator it = stateChange.actionsToDisable.constBegin();
            it != stateChange.actionsToDisable.constEnd(); ++it) {

        QAction *action = actionCollection()->action(*it);
        if (action) {
            action->setEnabled(setFalse);
        }
    }

}

void KisKXMLGUIClient::beginXMLPlug(QWidget *w)
{
    actionCollection()->addAssociatedWidget(w);
    foreach (KisKXMLGUIClient *client, d->m_children) {
        client->beginXMLPlug(w);
    }
}

void KisKXMLGUIClient::endXMLPlug()
{
}

void KisKXMLGUIClient::prepareXMLUnplug(QWidget *w)
{
    actionCollection()->removeAssociatedWidget(w);
    foreach (KisKXMLGUIClient *client, d->m_children) {
        client->prepareXMLUnplug(w);
    }
}

void KisKXMLGUIClient::virtual_hook(int, void *)
{
}
