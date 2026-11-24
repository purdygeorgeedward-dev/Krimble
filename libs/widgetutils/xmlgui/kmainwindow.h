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
     (C) 2000-2008 David Faure (faure@kde.org)

    SPDX-License-Identifier: LGPL-2.0-only

*/

#ifndef KMAINWINDOW_H
#define KMAINWINDOW_H

#include <kritawidgetutils_export.h>

#include <QMainWindow>
#include <QMetaClassInfo>

class QMenu;
class KConfig;
class KConfigGroup;
class KMWSessionManager;
class KisKMainWindowPrivate;
class KisToolBar;

#define XMLGUI_DECLARE_PRIVATE(classname) \
    inline classname ## Private *k_func() { return reinterpret_cast<classname ## Private *>(k_ptr); } \
    inline const classname ## Private *k_func() const { return reinterpret_cast<classname ## Private *>(k_ptr); } \
    friend class classname ## Private;

class KRITAWIDGETUTILS_EXPORT KisKMainWindow : public QMainWindow
{
    friend class KMWSessionManager;
    friend class DockResizeListener;
    XMLGUI_DECLARE_PRIVATE(KisKMainWindow)
    Q_OBJECT
    Q_PROPERTY(bool hasMenuBar READ hasMenuBar)
    Q_PROPERTY(bool autoSaveSettings READ autoSaveSettings)
    Q_PROPERTY(QString autoSaveGroup READ autoSaveGroup)

public:
    explicit KisKMainWindow(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~KisKMainWindow() override;

    static bool canBeRestored(int number);

    static const QString classNameOfToplevel(int number);

    bool restore(int number, bool show = true);

    bool hasMenuBar();

    static QList<KisKMainWindow *> memberList();

    KisToolBar *toolBar(const QString &name = QString());

    QList<KisToolBar *> toolBars() const;

    void setAutoSaveSettings(const QString &groupName = QLatin1String("MainWindow"),
                             bool saveWindowSize = true);

    void setAutoSaveSettings(const KConfigGroup &group,
                             bool saveWindowSize = true);

    void resetAutoSaveSettings();

    bool autoSaveSettings() const;

    QString autoSaveGroup() const;

    KConfigGroup autoSaveConfigGroup() const;

    virtual void applyMainWindowSettings(const KConfigGroup &config);

    void saveMainWindowSettings(KConfigGroup &config);

    QString dbusName() const;

public Q_SLOTS:

    void appHelpActivated(void);

    void setSettingsDirty();

protected:
    bool event(QEvent *event) override;

    void closeEvent(QCloseEvent *) override;

    virtual bool queryClose();

    virtual void saveProperties(KConfigGroup &) {}

    virtual void readProperties(const KConfigGroup &) {}

    virtual void saveGlobalProperties(KConfig *sessionConfig);

    virtual void readGlobalProperties(KConfig *sessionConfig);
    void savePropertiesInternal(KConfig *, int);
    bool readPropertiesInternal(KConfig *, int);

    bool settingsDirty() const;

    virtual bool windowsLayoutSavingAllowed() const;

protected Q_SLOTS:

    void saveAutoSaveSettings();

protected:
    KisKMainWindow(KisKMainWindowPrivate &dd, QWidget *parent, Qt::WindowFlags f);

    KisKMainWindowPrivate *const k_ptr;
private:
    Q_PRIVATE_SLOT(k_func(), void _k_slotSettingsChanged(int))
    Q_PRIVATE_SLOT(k_func(), void _k_slotSaveAutoSaveSize())
};

#define KDE_RESTORE_MAIN_WINDOWS_NUM_TEMPLATE_ARGS 3

template <typename T>
inline void kRestoreMainWindows()
{
    for (int n = 1; KisKMainWindow::canBeRestored(n); ++n) {
        const QString className = KisKMainWindow::classNameOfToplevel(n);
        if (className == QLatin1String(T::staticMetaObject.className())) {
            (new T)->restore(n);
        }
    }
}

template <typename T0, typename T1>
inline void kRestoreMainWindows()
{
    const char *classNames[2];
    classNames[0] = T0::staticMetaObject.className();
    classNames[1] = T1::staticMetaObject.className();
    for (int n = 1; KisKMainWindow::canBeRestored(n); ++n) {
        const QString className = KisKMainWindow::classNameOfToplevel(n);
        if (className == QLatin1String(classNames[0])) {
            (new T0)->restore(n);
        } else if (className == QLatin1String(classNames[1])) {
            (new T1)->restore(n);
        }
    }
}

template <typename T0, typename T1, typename T2>
inline void kRestoreMainWindows()
{
    const char *classNames[3];
    classNames[0] = T0::staticMetaObject.className();
    classNames[1] = T1::staticMetaObject.className();
    classNames[2] = T2::staticMetaObject.className();
    for (int n = 1; KisKMainWindow::canBeRestored(n); ++n) {
        const QString className = KisKMainWindow::classNameOfToplevel(n);
        if (className == QLatin1String(classNames[0])) {
            (new T0)->restore(n);
        } else if (className == QLatin1String(classNames[1])) {
            (new T1)->restore(n);
        } else if (className == QLatin1String(classNames[2])) {
            (new T2)->restore(n);
        }
    }
}

#endif
