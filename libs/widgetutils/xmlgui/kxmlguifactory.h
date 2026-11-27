/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef kxmlguifactory_h
#define kxmlguifactory_h

#include <kritawidgetutils_export.h>

#include <QObject>

class QAction;
class KisKXMLGUIFactoryPrivate;
class KisKXMLGUIClient;
class KisKXMLGUIBuilder;

class QDomAttr;
class QDomDocument;
class QDomElement;
class QDomNode;
class QDomNamedNodeMap;

namespace KisKXMLGUI
{
struct MergingIndex;
struct ContainerNode;
struct ContainerClient;
class BuildHelper;
}

class KRITAWIDGETUTILS_EXPORT KisKXMLGUIFactory : public QObject
{
    friend class KisKXMLGUI::BuildHelper;
    Q_OBJECT
public:
    explicit KisKXMLGUIFactory(KisKXMLGUIBuilder *builder, QObject *parent = 0);

    ~KisKXMLGUIFactory() override;

    static QString readConfigFile(const QString &filename,
                                  const QString &componentName = QString());
    static bool saveConfigFile(const QDomDocument &doc, const QString &filename,
                               const QString &componentName = QString());

    static QDomElement actionPropertiesElement(QDomDocument &doc);

    static QDomElement findActionByName(QDomElement &elem, const QString &sName, bool create);

    void addClient(KisKXMLGUIClient *client);

    void removeClient(KisKXMLGUIClient *client);

    void plugActionList(KisKXMLGUIClient *client, const QString &name, const QList<QAction *> &actionList);
    void unplugActionList(KisKXMLGUIClient *client, const QString &name);

    QList<KisKXMLGUIClient *> clients() const;

    QWidget *container(const QString &containerName, KisKXMLGUIClient *client, bool useTagName = false);

    QList<QWidget *> containers(const QString &tagName);

    void reset();

    void resetContainer(const QString &containerName, bool useTagName = false);

Q_SIGNALS:
    void clientAdded(KisKXMLGUIClient *client);
    void clientRemoved(KisKXMLGUIClient *client);

    void makingChanges(bool);

private:
    friend class KisKXMLGUIClient;
    void forgetClient(KisKXMLGUIClient *client);

    KisKXMLGUIFactoryPrivate *const d;
};

#endif

