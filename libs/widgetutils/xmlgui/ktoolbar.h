/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2000 Reginald Stadlbauer (reggie@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Stephan Kulow (coolo@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Sven Radej (radej@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Mark Donohoe (donohoe@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Matthias Ettrich (ettrich@kde.org)
    SPDX-FileCopyrightText: 1999, 2000 Kurt Granroth (granroth@kde.org)
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda (rodda@kde.org)

    SPDX-License-Identifier: LGPL-2.0-only
    */

#ifndef KTOOLBAR_H
#define KTOOLBAR_H

#include <kritawidgetutils_export.h>

#include <QToolBar>

class QDomElement;

class KConfigGroup;
class KConfig;
class KisKMainWindow;
class KisKXMLGUIClient;
class KisToolBarStateModel;

class KRITAWIDGETUTILS_EXPORT KisToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit KisToolBar(const QString &objectName, QWidget *parent, bool readConfig = true);

    ~KisToolBar() override;

    KisKMainWindow *mainWindow() const;

    void setIconDimensions(int size);

    int iconSizeDefault() const;

    void saveSettings(KConfigGroup &cg);

    void applySettings(const KConfigGroup &cg);

    void addXMLGUIClient(KisKXMLGUIClient *client);

    void removeXMLGUIClient(KisKXMLGUIClient *client);

    void loadState(const QDomElement &element);

    void saveState(QDomElement &element) const;

    bool eventFilter(QObject *watched, QEvent *event) override;

    static bool toolBarsEditable();

    static void setToolBarsEditable(bool editable);

    static bool toolBarsLocked();

    static void setToolBarsLocked(bool locked);

    static void emitToolbarStyleChanged();

    static KisToolBarStateModel* toolBarStateModel();

protected Q_SLOTS:
    virtual void slotMovableChanged(bool movable);

protected:
    void contextMenuEvent(QContextMenuEvent *) override;
    void actionEvent(QActionEvent *) override;

    void dragEnterEvent(QDragEnterEvent *) override;
    void dragMoveEvent(QDragMoveEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;
    void dropEvent(QDropEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void slotAppearanceChanged())
    Q_PRIVATE_SLOT(d, void slotContextAboutToShow())
    Q_PRIVATE_SLOT(d, void slotContextAboutToHide())
    Q_PRIVATE_SLOT(d, void slotContextLeft())
    Q_PRIVATE_SLOT(d, void slotContextRight())
    Q_PRIVATE_SLOT(d, void slotContextShowText())
    Q_PRIVATE_SLOT(d, void slotContextTop())
    Q_PRIVATE_SLOT(d, void slotContextBottom())
    Q_PRIVATE_SLOT(d, void slotContextIcons())
    Q_PRIVATE_SLOT(d, void slotContextText())
    Q_PRIVATE_SLOT(d, void slotContextTextRight())
    Q_PRIVATE_SLOT(d, void slotContextTextUnder())
    Q_PRIVATE_SLOT(d, void slotContextIconSize())
    Q_PRIVATE_SLOT(d, void slotLockToolBars(bool))
    Q_PRIVATE_SLOT(d, void slotToolButtonToggled(bool))
};

#endif
