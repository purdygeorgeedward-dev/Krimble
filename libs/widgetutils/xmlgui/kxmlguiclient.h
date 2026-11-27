/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KisKXMLGUICLIENT_H
#define KisKXMLGUICLIENT_H

#include <kritawidgetutils_export.h>

#include <QMap>
#include <QStringList>

class QDomDocument;
class QDomElement;
class QWidget;

class QAction;
class KisKActionCollection;
class KisKXMLGUIClientPrivate;
class KisKXMLGUIFactory;
class KisKXMLGUIBuilder;

namespace KDEPrivate
{
class KisKEditToolBarWidget;
}

class KRITAWIDGETUTILS_EXPORT KisKXMLGUIClient
{
    friend class KDEPrivate::KisKEditToolBarWidget;
public:
    KisKXMLGUIClient();

    explicit KisKXMLGUIClient(KisKXMLGUIClient *parent);

    virtual ~KisKXMLGUIClient();

    QAction *action(const char *name) const;

    virtual QAction *action(const QDomElement &element) const;

    virtual KisKActionCollection *actionCollection() const;

    virtual QString componentName() const;

    virtual QDomDocument domDocument() const;

    virtual QString xmlFile() const;

    virtual QString localXMLFile() const;

    void setXMLGUIBuildDocument(const QDomDocument &doc);
    QDomDocument xmlguiBuildDocument() const;

    void setFactory(KisKXMLGUIFactory *factory);
    KisKXMLGUIFactory *factory() const;

    KisKXMLGUIClient *parentClient() const;

    void insertChildClient(KisKXMLGUIClient *child);

    void removeChildClient(KisKXMLGUIClient *child);

    QList<KisKXMLGUIClient *> childClients();

    void setClientBuilder(KisKXMLGUIBuilder *builder);

    KisKXMLGUIBuilder *clientBuilder() const;

    void reloadXML();

    void plugActionList(const QString &name, const QList<QAction *> &actionList);

    void unplugActionList(const QString &name);

    static QString findMostRecentXMLFile(const QStringList &files, QString &doc);

    void addStateActionEnabled(const QString &state, const QString &action);

    void addStateActionDisabled(const QString &state, const QString &action);

    enum ReverseStateChange { StateNoReverse, StateReverse };
    struct StateChange {
        QStringList actionsToEnable;
        QStringList actionsToDisable;
    };

    StateChange getActionsToChangeForState(const QString &state);

    void beginXMLPlug(QWidget *);
    void endXMLPlug();
    void prepareXMLUnplug(QWidget *);

    void replaceXMLFile(const QString &xmlfile, const QString &localxmlfile, bool merge = false);

protected:
    virtual void setComponentName(const QString &componentName, const QString &componentDisplayName);

    void setXMLFile(const QString &file, bool merge = false, bool setXMLDoc = true);

    static QString standardsXmlFileLocation();

    void loadStandardsXmlFile();

    void setLocalXMLFile(const QString &file);

    void setXML(const QString &document, bool merge = false);

    void setDOMDocument(const QDomDocument &document, bool merge = false);

    virtual void stateChanged(const QString &newstate, ReverseStateChange reverse = StateNoReverse);

protected:
    virtual void virtual_hook(int id, void *data);

private:
    KisKXMLGUIClientPrivate *const d;
};

#endif
