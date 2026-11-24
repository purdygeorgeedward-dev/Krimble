/* This file is part of the KDE libraries
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

#include "kmainwindow.h"
#include "config-xmlgui.h"
#include "kmainwindow_p.h"
#ifdef HAVE_DBUS
#include "kmainwindowiface_p.h"
#endif
#include "ktoolbarhandler_p.h"
#include "khelpmenu.h"
#include "ktoolbar.h"

#include <QApplication>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QCloseEvent>
#include <QScreen>
#include <QDockWidget>
#include <QLayout>
#include <QMenuBar>
#include <QSessionManager>
#include <QStatusBar>
#include <QStyle>
#include <QWidget>
#include <QWindow>
#ifdef HAVE_DBUS
#include <QDBusConnection>
#endif
#include <ktoggleaction.h>
#include <kaboutdata.h>
#include <kconfig.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <kwindowconfig.h>
#include <kconfiggui.h>

static const char WINDOW_PROPERTIES[]="WindowProperties";

static QMenuBar *internalMenuBar(KisKMainWindow *mw)
{
    return mw->findChild<QMenuBar *>(QString(), Qt::FindDirectChildrenOnly);
}

static QStatusBar *internalStatusBar(KisKMainWindow *mw)
{
    return mw->findChild<QStatusBar *>(QString(), Qt::FindDirectChildrenOnly);
}

class DockResizeListener : public QObject
{
public:
    DockResizeListener(KisKMainWindow *win);
    ~DockResizeListener() override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    KisKMainWindow *m_win;
};

DockResizeListener::DockResizeListener(KisKMainWindow *win) :
    QObject(win),
    m_win(win)
{
}

DockResizeListener::~DockResizeListener()
{
}

bool DockResizeListener::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
    case QEvent::Hide:
        m_win->k_ptr->setSettingsDirty(KisKMainWindowPrivate::CompressCalls);
        break;

    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}

KMWSessionManager::KMWSessionManager()
{
    connect(qApp, SIGNAL(saveStateRequest(QSessionManager&)),
            this, SLOT(saveState(QSessionManager&)));
}

KMWSessionManager::~KMWSessionManager()
{
}

bool KMWSessionManager::saveState(QSessionManager &)
{
#if 0
    KConfigGui::setSessionConfig(sm.sessionId(), sm.sessionKey());

    KConfig *config = KConfigGui::sessionConfig();
    if (KisKMainWindow::memberList().count()) {
        KisKMainWindow::memberList().first()->saveGlobalProperties(config);
    }

    int n = 0;
    foreach (KisKMainWindow *mw, KisKMainWindow::memberList()) {
        n++;
        mw->savePropertiesInternal(config, n);
    }

    KConfigGroup group(config, "Number");
    group.writeEntry("NumberOfWindows", n);

    config->sync();

    QString localFilePath =  QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QLatin1Char('/') + config->name();
    if (QFile::exists(localFilePath)) {
        QStringList discard;
        discard << QLatin1String("rm");
        discard << localFilePath;
        sm.setDiscardCommand(discard);
    }
#endif
    return true;
}

Q_GLOBAL_STATIC(KMWSessionManager, ksm)
Q_GLOBAL_STATIC(QList<KisKMainWindow *>, sMemberList)

KisKMainWindow::KisKMainWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), k_ptr(new KisKMainWindowPrivate)
{
    k_ptr->init(this);
}

KisKMainWindow::KisKMainWindow(KisKMainWindowPrivate &dd, QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f), k_ptr(&dd)
{
    k_ptr->init(this);
}

void KisKMainWindowPrivate::init(KisKMainWindow *_q)
{
    q = _q;

    q->setAnimated(q->style()->styleHint(QStyle::SH_Widget_Animate, 0, q));

    q->setAttribute(Qt::WA_DeleteOnClose);

    helpMenu = 0;

#ifdef HAVE_GLOBALACCEL
    QObject::connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)),
                     q, SLOT(_k_slotSettingsChanged(int)));
#endif

    ksm();

    sMemberList()->append(q);

    settingsDirty = false;
    autoSaveSettings = false;
    autoSaveWindowSize = true;
    settingsTimer = 0;
    sizeTimer = 0;

    dockResizeListener = new DockResizeListener(_q);
    letDirtySettings = true;

    sizeApplied = false;
}

static bool endsWithHashNumber(const QString &s)
{
    for (int i = s.length() - 1;
            i > 0;
            --i) {
        if (s[ i ] == QLatin1Char('#') && i != s.length() - 1) {
            return true;
        }
        if (!s[ i ].isDigit()) {
            break;
        }
    }
    return false;
}
#ifdef HAVE_DBUS
static inline bool isValidDBusObjectPathCharacter(const QChar &c)
{
    ushort u = c.unicode();
    return (u >= QLatin1Char('a') && u <= QLatin1Char('z'))
           || (u >= QLatin1Char('A') && u <= QLatin1Char('Z'))
           || (u >= QLatin1Char('0') && u <= QLatin1Char('9'))
           || (u == QLatin1Char('_')) || (u == QLatin1Char('/'));
}
#endif
void KisKMainWindowPrivate::polish(KisKMainWindow *q)
{
    QString objname;
    QString s;
    int unusedNumber = 1;
    const QString name = q->objectName();
    bool startNumberingImmediately = true;
    bool tryReuse = false;
    if (name.isEmpty()) {
        objname = QStringLiteral("MainWindow#");
    } else if (name.endsWith(QLatin1Char('#'))) {
        objname = name;
    } else if (endsWithHashNumber(name)) {
        objname = name;
        tryReuse = true;
        startNumberingImmediately = false;
    } else {
        objname = name;
        startNumberingImmediately = false;
    }

    s = objname;
    if (startNumberingImmediately) {
        s += QLatin1Char('1');
    }

    for (;;) {
        const QList<QWidget *> list = qApp->topLevelWidgets();
        bool found = false;
        foreach (QWidget *w, list) {
            if (w != q && w->objectName() == s) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
        if (tryReuse) {
            objname = name.left(name.length() - 1);
            unusedNumber = 0;
            tryReuse = false;
        }
        s.setNum(++unusedNumber);
        s = objname + s;
    }
    q->setObjectName(s);
    q->winId();
    q->setWindowRole(s);

#ifdef HAVE_DBUS
    dbusName = QLatin1Char('/') + QCoreApplication::applicationName() + QLatin1Char('/');
    dbusName += q->objectName().replace(QLatin1Char('/'), QLatin1Char('_'));
    const int len = dbusName.length();
    for (int i = 0; i < len; ++i) {
        if (!isValidDBusObjectPathCharacter(dbusName[i])) {
            dbusName[i] = QLatin1Char('_');
        }
    }

    QDBusConnection::sessionBus().registerObject(dbusName, q, QDBusConnection::ExportScriptableSlots |
            QDBusConnection::ExportScriptableProperties |
            QDBusConnection::ExportNonScriptableSlots |
            QDBusConnection::ExportNonScriptableProperties |
            QDBusConnection::ExportAdaptors);
#endif
}

void KisKMainWindowPrivate::setSettingsDirty(CallCompression callCompression)
{
    if (!letDirtySettings) {
        return;
    }

    settingsDirty = true;
    if (autoSaveSettings) {
        if (callCompression == CompressCalls) {
            if (!settingsTimer) {
                settingsTimer = new QTimer(q);
                settingsTimer->setInterval(500);
                settingsTimer->setSingleShot(true);
                QObject::connect(settingsTimer, SIGNAL(timeout()), q, SLOT(saveAutoSaveSettings()));
            }
            settingsTimer->start();
        } else {
            q->saveAutoSaveSettings();
        }
    }
}

void KisKMainWindowPrivate::setSizeDirty()
{
    if (autoSaveWindowSize) {
        if (!sizeTimer) {
            sizeTimer = new QTimer(q);
            sizeTimer->setInterval(500);
            sizeTimer->setSingleShot(true);
            QObject::connect(sizeTimer, SIGNAL(timeout()), q, SLOT(_k_slotSaveAutoSaveSize()));
        }
        sizeTimer->start();
    }
}

KisKMainWindow::~KisKMainWindow()
{
    sMemberList()->removeAll(this);
    delete static_cast<QObject *>(k_ptr->dockResizeListener);
    delete k_ptr;
}

bool KisKMainWindow::canBeRestored(int number)
{
    if (!qApp->isSessionRestored()) {
        return false;
    }
    KConfig *config = KConfigGui::sessionConfig();
    if (!config) {
        return false;
    }

    KConfigGroup group(config, "Number");
    const int n = group.readEntry("NumberOfWindows", 1);
    return number >= 1 && number <= n;
}

const QString KisKMainWindow::classNameOfToplevel(int )
{
    return QString();
#if 0
    if (!qApp->isSessionRestored()) {
        return QString();
    }
    KConfig *config = KConfigGui::sessionConfig();
    if (!config) {
        return QString();
    }

    KConfigGroup group(config, QByteArray(WINDOW_PROPERTIES).append(QByteArray::number(number)).constData());
    if (!group.hasKey("ClassName")) {
        return QString();
    } else {
        return group.readEntry("ClassName");
    }
#endif
}

bool KisKMainWindow::restore(int , bool )
{
#if 0
    if (!canBeRestored(number)) {
        return false;
    }
    KConfig *config = KConfigGui::sessionConfig();
    if (readPropertiesInternal(config, number)) {
        if (show) {
            KisKMainWindow::show();
        }
        return false;
    }
#endif
    return false;
}

void KisKMainWindow::appHelpActivated(void)
{
    K_D(KisKMainWindow);
    if (!d->helpMenu) {
        d->helpMenu = new KisKHelpMenu(this);
        if (!d->helpMenu) {
            return;
        }
    }
    d->helpMenu->appHelpActivated();
}

void KisKMainWindow::closeEvent(QCloseEvent *e)
{
    K_D(KisKMainWindow);

    if (d->settingsTimer && d->settingsTimer->isActive()) {
        d->settingsTimer->stop();
        saveAutoSaveSettings();
    }
    if (d->sizeTimer && d->sizeTimer->isActive()) {
        d->sizeTimer->stop();
        d->_k_slotSaveAutoSaveSize();
    }

    if (queryClose()) {
        d->autoSaveSettings = false;
        d->letDirtySettings = false;
        e->accept();
    } else {
        e->ignore();
    }
}

bool KisKMainWindow::queryClose()
{
    return true;
}

void KisKMainWindow::saveGlobalProperties(KConfig *)
{
}

void KisKMainWindow::readGlobalProperties(KConfig *)
{
}

void KisKMainWindow::savePropertiesInternal(KConfig *config, int number)
{
    K_D(KisKMainWindow);
    const bool oldASWS = d->autoSaveWindowSize;
    d->autoSaveWindowSize = true;

    KConfigGroup cg(config, QByteArray(WINDOW_PROPERTIES).append(QByteArray::number(number)).constData());

    cg.writeEntry("ObjectName", objectName());
    cg.writeEntry("ClassName", metaObject()->className());

    saveMainWindowSettings(cg);

    cg = KConfigGroup(config, QByteArray::number(number).constData());
    saveProperties(cg);

    d->autoSaveWindowSize = oldASWS;
}

void KisKMainWindow::saveMainWindowSettings(KConfigGroup &cg)
{
    if (!windowsLayoutSavingAllowed()) return;

    K_D(KisKMainWindow);

    if (d->autoSaveWindowSize) {
        KWindowConfig::saveWindowSize(windowHandle(), cg);
    }

    const QByteArray state = saveState();
    cg.writeEntry("State", state.toBase64());

    QStatusBar *sb = internalStatusBar(this);
    if (sb) {
        if (!cg.hasDefault("StatusBar") && !sb->isHidden()) {
            cg.revertToDefault("StatusBar");
        } else {
            cg.writeEntry("StatusBar", sb->isHidden() ? "Disabled" : "Enabled");
        }
    }

    QMenuBar *mb = internalMenuBar(this);
    if (mb && !mb->isNativeMenuBar()) {
        if (!cg.hasDefault("MenuBar") && !mb->isHidden()) {
            cg.revertToDefault("MenuBar");
        } else {
            if (!QCoreApplication::testAttribute(Qt::AA_DontUseNativeMenuBar)) {
                cg.writeEntry("MenuBar", "Enabled");
            }
            cg.writeEntry("MenuBar", mb->isHidden() ? "Disabled" : "Enabled");
        }
    }

    {
        if (!cg.hasDefault("ToolBarsMovable") && KisToolBar::toolBarsLocked()) {
            cg.revertToDefault("ToolBarsMovable");
        } else {
            cg.writeEntry("ToolBarsMovable", KisToolBar::toolBarsLocked() ? "Disabled" : "Enabled");
        }
    }

    int n = 1;
    foreach (KisToolBar *toolbar, toolBars()) {
        QByteArray groupName("Toolbar");
        groupName += (toolbar->objectName().isEmpty() ? QByteArray::number(n) : QByteArray(" ").append(toolbar->objectName().toUtf8()));

        KConfigGroup toolbarGroup(&cg, groupName.constData());
        toolbar->saveSettings(toolbarGroup);
        n++;
    }
}

bool KisKMainWindow::readPropertiesInternal(KConfig *config, int number)
{
    K_D(KisKMainWindow);

    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    if (number == 1) {
        readGlobalProperties(config);
    }

    KConfigGroup cg(config, QByteArray(WINDOW_PROPERTIES).append(QByteArray::number(number)).constData());

    if (cg.hasKey("ObjectName")) {
        setObjectName(cg.readEntry("ObjectName"));
    }

    d->sizeApplied = false;
    applyMainWindowSettings(cg);

    KConfigGroup grp(config, QByteArray::number(number).constData());
    readProperties(grp);

    d->letDirtySettings = oldLetDirtySettings;

    return true;
}

void KisKMainWindow::applyMainWindowSettings(const KConfigGroup &cg)
{
    K_D(KisKMainWindow);

    QWidget *focusedWidget = QApplication::focusWidget();

    const bool oldLetDirtySettings = d->letDirtySettings;
    d->letDirtySettings = false;

    if (!d->sizeApplied) {
        winId();
        KWindowConfig::restoreWindowSize(windowHandle(), cg);
        resize(windowHandle()->size());
        d->sizeApplied = true;
    }

    QStatusBar *sb = internalStatusBar(this);
    if (sb) {
        QString entry = cg.readEntry("StatusBar", "Enabled");
        sb->setVisible( entry != QLatin1String("Disabled") );
    }

    QMenuBar *mb = internalMenuBar(this);
    if (mb && !mb->isNativeMenuBar()) {
        QString entry = cg.readEntry("MenuBar", "Enabled");
#ifdef Q_OS_ANDROID
        entry = QLatin1String("Enabled");
#endif
        mb->setVisible( entry != QLatin1String("Disabled") );
    }

    {
        QString entry = cg.readEntry("ToolBarsMovable", "Disabled");
        KisToolBar::setToolBarsLocked(entry == QLatin1String("Disabled"));
    }

    int n = 1;
    foreach (KisToolBar *toolbar, toolBars()) {
        QByteArray groupName("Toolbar");
        groupName += (toolbar->objectName().isEmpty() ? QByteArray::number(n) : QByteArray(" ").append(toolbar->objectName().toUtf8()));

        KConfigGroup toolbarGroup(&cg, groupName.constData());
        toolbar->applySettings(toolbarGroup);
        n++;
    }

    QByteArray state;
    if (cg.hasKey("State")) {
        state = cg.readEntry("State", state);
        state = QByteArray::fromBase64(state);
        restoreState(state);
    }

    if (focusedWidget) {
        focusedWidget->setFocus();
    }

    d->settingsDirty = false;
    d->letDirtySettings = oldLetDirtySettings;
}

void KisKMainWindow::setSettingsDirty()
{
    K_D(KisKMainWindow);
    d->setSettingsDirty();
}

bool KisKMainWindow::settingsDirty() const
{
    K_D(const KisKMainWindow);
    return d->settingsDirty;
}

bool KisKMainWindow::windowsLayoutSavingAllowed() const
{
    return true;
}

void KisKMainWindow::setAutoSaveSettings(const QString &groupName, bool saveWindowSize)
{
    setAutoSaveSettings(KConfigGroup(KSharedConfig::openConfig(), groupName), saveWindowSize);
}

void KisKMainWindow::setAutoSaveSettings(const KConfigGroup &group,
                                      bool saveWindowSize)
{
    K_D(KisKMainWindow);
    d->autoSaveSettings = true;
    d->autoSaveGroup = group;
    d->autoSaveWindowSize = saveWindowSize;

    if (!saveWindowSize && d->sizeTimer) {
        d->sizeTimer->stop();
    }

    applyMainWindowSettings(d->autoSaveGroup);
}

void KisKMainWindow::resetAutoSaveSettings()
{
    K_D(KisKMainWindow);
    d->autoSaveSettings = false;
    if (d->settingsTimer) {
        d->settingsTimer->stop();
    }
}

bool KisKMainWindow::autoSaveSettings() const
{
    K_D(const KisKMainWindow);
    return d->autoSaveSettings;
}

QString KisKMainWindow::autoSaveGroup() const
{
    K_D(const KisKMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup.name() : QString();
}

KConfigGroup KisKMainWindow::autoSaveConfigGroup() const
{
    K_D(const KisKMainWindow);
    return d->autoSaveSettings ? d->autoSaveGroup : KConfigGroup();
}

void KisKMainWindow::saveAutoSaveSettings()
{
    K_D(KisKMainWindow);
    Q_ASSERT(d->autoSaveSettings);
    saveMainWindowSettings(d->autoSaveGroup);
    d->autoSaveGroup.sync();
    d->settingsDirty = false;
}

bool KisKMainWindow::event(QEvent *ev)
{
    K_D(KisKMainWindow);
    switch (ev->type()) {
#ifdef Q_OS_WIN
    case QEvent::Move:
#endif
    case QEvent::Resize:
        d->setSizeDirty();
        break;
    case QEvent::Polish:
        d->polish(this);
        break;
    case QEvent::ChildPolished: {
        QChildEvent *event = static_cast<QChildEvent *>(ev);
        QDockWidget *dock = qobject_cast<QDockWidget *>(event->child());
        KisToolBar *toolbar = qobject_cast<KisToolBar *>(event->child());
        QMenuBar *menubar = qobject_cast<QMenuBar *>(event->child());
        if (dock) {
            connect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                    this, SLOT(setSettingsDirty()));
            connect(dock, SIGNAL(visibilityChanged(bool)),
                    this, SLOT(setSettingsDirty()), Qt::QueuedConnection);
            connect(dock, SIGNAL(topLevelChanged(bool)),
                    this, SLOT(setSettingsDirty()));

            dock->installEventFilter(k_ptr->dockResizeListener);
        } else if (toolbar) {
            toolbar->installEventFilter(k_ptr->dockResizeListener);
        } else if (menubar) {
            menubar->installEventFilter(k_ptr->dockResizeListener);
        }
    }
    break;
    case QEvent::ChildRemoved: {
        QChildEvent *event = static_cast<QChildEvent *>(ev);
        QDockWidget *dock = qobject_cast<QDockWidget *>(event->child());
        KisToolBar *toolbar = qobject_cast<KisToolBar *>(event->child());
        QMenuBar *menubar = qobject_cast<QMenuBar *>(event->child());
        if (dock) {
            disconnect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                       this, SLOT(setSettingsDirty()));
            disconnect(dock, SIGNAL(visibilityChanged(bool)),
                       this, SLOT(setSettingsDirty()));
            disconnect(dock, SIGNAL(topLevelChanged(bool)),
                       this, SLOT(setSettingsDirty()));
            dock->removeEventFilter(k_ptr->dockResizeListener);
        } else if (toolbar) {
            toolbar->removeEventFilter(k_ptr->dockResizeListener);
        } else if (menubar) {
            menubar->removeEventFilter(k_ptr->dockResizeListener);
        }
    }
    break;
    default:
        break;
    }
    return QMainWindow::event(ev);
}

bool KisKMainWindow::hasMenuBar()
{
    return internalMenuBar(this);
}

void KisKMainWindowPrivate::_k_slotSettingsChanged(int category)
{
    Q_UNUSED(category);

    q->setAnimated(q->style()->styleHint(QStyle::SH_Widget_Animate, 0, q));
}

void KisKMainWindowPrivate::_k_slotSaveAutoSaveSize()
{
    if (autoSaveGroup.isValid()) {
        KWindowConfig::saveWindowSize(q->windowHandle(), autoSaveGroup);
    }
}

KisToolBar *KisKMainWindow::toolBar(const QString &name)
{
    QString childName = name;
    if (childName.isEmpty()) {
        childName = QStringLiteral("mainToolBar");
    }

    KisToolBar *tb = findChild<KisToolBar *>(childName);
    if (tb) {
        return tb;
    }

    KisToolBar *toolbar = new KisToolBar(childName, this);
    return toolbar;
}

QList<KisToolBar *> KisKMainWindow::toolBars() const
{
    QList<KisToolBar *> ret;

    foreach (QObject *child, children())
        if (KisToolBar *toolBar = qobject_cast<KisToolBar *>(child)) {
            ret.append(toolBar);
        }

    return ret;
}

QList<KisKMainWindow *> KisKMainWindow::memberList()
{
    return *sMemberList();
}

QString KisKMainWindow::dbusName() const
{
    return k_func()->dbusName;
}

#include "moc_kmainwindow.cpp"

