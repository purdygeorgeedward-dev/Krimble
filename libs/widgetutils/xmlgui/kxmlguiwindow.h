/*
    This file is part of the KDE libraries
     Copyright
     (C) 2000 Reginald Stadlbauer (reggie@kde.org)
     (C) 1997 Stephan Kulow (coolo@kde.org)
     (C) 1997-2000 Sven Radej (radej@kde.org)
     (C) 1997-2000 Matthias Ettrich (ettrich@kde.org)
     (C) 1999 Chris Schlaeger (cs@kde.org)
     (C) 2002 Joseph Wenninger (jowenn@kde.org)
     (C) 2005-2006 Hamish Rodda (rodda@kde.org)

    SPDX-License-Identifier: LGPL-2.0-only

*/

#ifndef KisKXMLGUIWINDOW_H
#define KisKXMLGUIWINDOW_H

#include "kxmlguiclient.h"
#include "kxmlguibuilder.h"
#include "kmainwindow.h"
#include <QMetaClassInfo>

class KMenu;
class KisKXMLGUIFactory;
class KConfig;
class KConfigGroup;
class KisToolBar;
class KXmlGuiWindowPrivate;

class KRITAWIDGETUTILS_EXPORT KXmlGuiWindow : public KisKMainWindow, public KisKXMLGUIBuilder, virtual public KisKXMLGUIClient
{
    XMLGUI_DECLARE_PRIVATE(KXmlGuiWindow)
    Q_OBJECT
    Q_PROPERTY(bool hasMenuBar READ hasMenuBar)
    Q_PROPERTY(bool autoSaveSettings READ autoSaveSettings)
    Q_PROPERTY(QString autoSaveGroup READ autoSaveGroup)
    Q_PROPERTY(bool standardToolBarMenuEnabled READ isStandardToolBarMenuEnabled WRITE setStandardToolBarMenuEnabled)
    Q_FLAGS(StandardWindowOption)

public:
    explicit KXmlGuiWindow(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~KXmlGuiWindow() override;

    void setHelpMenuEnabled(bool showHelpMenu = true);

    bool isHelpMenuEnabled() const;

    virtual KisKXMLGUIFactory *guiFactory();

    void createGUI(const QString &xmlfile = QString());

    void setStandardToolBarMenuEnabled(bool enable);
    bool isStandardToolBarMenuEnabled() const;

    void createStandardStatusBarAction();

    enum StandardWindowOption {
        ToolBar = 1,

        Keys = 2,

        StatusBar = 4,

        Save = 8,

        Create = 16,

        Default = ToolBar | Keys | StatusBar | Save | Create
    };
    Q_DECLARE_FLAGS(StandardWindowOptions, StandardWindowOption)

    void setupGUI(StandardWindowOptions options = Default, const QString &xmlfile = QString());

    void setupGUI(const QSize &defaultSize, StandardWindowOptions options = Default, const QString &xmlfile = QString());

    QAction *toolBarMenuAction();

    void setupToolbarMenuActions();

    void finalizeGUI(KisKXMLGUIClient *client) override;

    void finalizeGUI(bool force);

    void applyMainWindowSettings(const KConfigGroup &config) override;

public Q_SLOTS:
    virtual void configureToolbars();

    virtual void slotStateChanged(const QString &newstate);

    void slotStateChanged(const QString &newstate,
                          bool reverse);

protected:
    bool event(QEvent *event) override;

protected Q_SLOTS:
    virtual void saveNewToolbarConfig();

private:
    Q_PRIVATE_SLOT(k_func(), void _k_slotFactoryMakingChanges(bool))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KXmlGuiWindow::StandardWindowOptions)

#endif
