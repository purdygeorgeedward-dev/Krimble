/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef kxmlguibuilder_h
#define kxmlguibuilder_h

#include <kritawidgetutils_export.h>

#include <KisQStringListFwd.h>

class KisKXMLGUIBuilderPrivate;
class KisKXMLGUIClient;

class QAction;
class QDomElement;
class QWidget;

class KRITAWIDGETUTILS_EXPORT KisKXMLGUIBuilder
{
public:

    explicit KisKXMLGUIBuilder(QWidget *widget);
    virtual ~KisKXMLGUIBuilder();

    KisKXMLGUIClient *builderClient() const;
    void setBuilderClient(KisKXMLGUIClient *client);
    QWidget *widget();

    virtual QStringList containerTags() const;

    virtual QWidget *createContainer(QWidget *parent, int index,
                                     const QDomElement &element, QAction *&containerAction);

    virtual void removeContainer(QWidget *container, QWidget *parent,
                                 QDomElement &element, QAction *containerAction);

    virtual QStringList customTags() const;

    virtual QAction *createCustomElement(QWidget *parent, int index, const QDomElement &element);

    virtual void removeCustomElement(QWidget *parent, QAction *action);

    virtual void finalizeGUI(KisKXMLGUIClient *client);

protected:
    virtual void virtual_hook(int id, void *data);
private:
    KisKXMLGUIBuilderPrivate *const d;
};

#endif

