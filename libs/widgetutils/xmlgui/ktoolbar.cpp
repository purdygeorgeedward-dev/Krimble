/* This file is part of the KDE libraries

    SPDX-FileCopyrightText: 2000 Reginald Stadlbauer (reggie@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Stephan Kulow (coolo@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Mark Donohoe (donohoe@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Sven Radej (radej@kde.org)
    SPDX-FileCopyrightText: 1997, 1998 Matthias Ettrich (ettrich@kde.org)
    SPDX-FileCopyrightText: 1999 Chris Schlaeger (cs@kde.org)
    SPDX-FileCopyrightText: 1999 Kurt Granroth (granroth@kde.org)
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda (rodda@kde.org)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "ktoolbar.h"
#include "config-xmlgui.h"
#include <QAction>
#include <QApplication>
#include <QScreen>
#include <QFrame>
#include <QLayout>
#include <QMenu>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QToolButton>
#include <QDomElement>
#ifdef HAVE_DBUS
#include <QDBusConnection>
#include <QDBusMessage>
#endif
#include <QDebug>
#include <QActionGroup>

#include <kconfig.h>
#include <ksharedconfig.h>
#ifdef HAVE_ICONTHEMES
#include <kicontheme.h>
#endif
#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <kconfiggroup.h>

#include "kactioncollection.h"
#include "kedittoolbar.h"
#include "kxmlguifactory.h"
#include "kxmlguiwindow.h"

#include <kis_icon_utils.h>

#include "KisToolBarStateModel.h"

Q_GLOBAL_STATIC(KisToolBarStateModel, s_toolBarsModel)

enum SettingLevel { Level_KDEDefault, Level_AppXML, Level_UserSettings,
                    NSettingLevels
                  };
enum { Unset = -1 };

class KisToolBar::Private
{
public:
    Private(KisToolBar *qq)
        : q(qq),
          isMainToolBar(false),
          unlockedMovable(true),
          contextOrient(0),
          contextMode(0),
          contextSize(0),
          contextButtonTitle(0),
          contextShowText(0),
          contextButtonAction(0),
          contextTop(0),
          contextLeft(0),
          contextRight(0),
          contextBottom(0),
          contextIcons(0),
          contextTextRight(0),
          contextText(0),
          contextTextUnder(0),
          contextLockAction(0),
          dropIndicatorAction(0),
          context(0),
          dragAction(0)
    {
    }

    void slotAppearanceChanged();
    void slotContextAboutToShow();
    void slotContextAboutToHide();
    void slotContextLeft();
    void slotContextRight();
    void slotContextShowText();
    void slotContextTop();
    void slotContextBottom();
    void slotContextIcons();
    void slotContextText();
    void slotContextTextRight();
    void slotContextTextUnder();
    void slotContextIconSize();
    void slotLockToolBars(bool lock);
    void slotToolButtonToggled(bool checked);

    void init(bool readConfig = true, bool isMainToolBar = false);
    QString getPositionAsString() const;
    QMenu *contextMenu(const QPoint &globalPos);
    void setLocked(bool locked);
    void adjustSeparatorVisibility();
    void loadKDESettings();
    void applyCurrentSettings();
    void customizeButtonPalette(QToolButton *button, bool checked);

    QAction *findAction(const QString &actionName, KisKXMLGUIClient **client = 0) const;

    static Qt::ToolButtonStyle toolButtonStyleFromString(const QString &style);
    static QString toolButtonStyleToString(Qt::ToolButtonStyle);
    static Qt::ToolBarArea positionFromString(const QString &position);
    static Qt::ToolButtonStyle toolButtonStyleSetting();

    KisToolBar *q;
    bool isMainToolBar : 1;
    bool unlockedMovable : 1;
    static bool s_editable;

    QSet<KisKXMLGUIClient *> xmlguiClients;

    QMenu *contextOrient;
    QMenu *contextMode;
    QMenu *contextSize;

    QAction *contextButtonTitle;
    QAction *contextShowText;
    QAction *contextButtonAction;
    QAction *contextTop;
    QAction *contextLeft;
    QAction *contextRight;
    QAction *contextBottom;
    QAction *contextIcons;
    QAction *contextTextRight;
    QAction *contextText;
    QAction *contextTextUnder;
    KToggleAction *contextLockAction;
    QMap<QAction *, int> contextIconSizes;

    class IntSetting
    {
    public:
        IntSetting()
        {
            for (int level = 0; level < NSettingLevels; ++level) {
                values[level] = Unset;
            }
        }
        int currentValue() const
        {
            int val = Unset;
            for (int level = 0; level < NSettingLevels; ++level) {
                if (values[level] != Unset) {
                    val = values[level];
                }
            }
            return val;
        }
        int defaultValue() const
        {
            int val = Unset;
            for (int level = 0; level < Level_UserSettings; ++level) {
                if (values[level] != Unset) {
                    val = values[level];
                }
            }
            return val;
        }
        QString toString() const
        {
            QString str;
            for (int level = 0; level < NSettingLevels; ++level) {
                str += QString::number(values[level]) + QLatin1Char(' ');
            }
            return str;
        }
        int &operator[](int index)
        {
            return values[index];
        }
    private:
        int values[NSettingLevels];
    };
    IntSetting iconSizeSettings;
    IntSetting toolButtonStyleSettings;

    QList<QAction *> actionsBeingDragged;
    QAction *dropIndicatorAction;

    QMenu *context;
    QAction *dragAction;
    QPoint dragStartPosition;

    struct ToolBarsStateUpdater;
    static ToolBarsStateUpdater s_toolBarsStateUpdater;
};

bool KisToolBar::Private::s_editable = false;

struct KisToolBar::Private::ToolBarsStateUpdater
{
    ToolBarsStateUpdater()
    {
        s_toolBarsModel->LAGER_QT(toolBarsLocked).bind(std::bind(&ToolBarsStateUpdater::updateToolbars, this, std::placeholders::_1));
    }

    void updateToolbars(bool locked) {
        Q_FOREACH (KisKMainWindow *mw, KisKMainWindow::memberList()) {
            Q_FOREACH (KisToolBar *toolbar, mw->findChildren<KisToolBar *>()) {
                toolbar->d->setLocked(locked);
            }
        }
    }
};

KisToolBar::Private::ToolBarsStateUpdater KisToolBar::Private::s_toolBarsStateUpdater = {};

void KisToolBar::Private::init(bool readConfig, bool _isMainToolBar)
{
    isMainToolBar = _isMainToolBar;
    loadKDESettings();

    if (readConfig) {
        KConfigGroup cg(KSharedConfig::openConfig(), QString());
        q->applySettings(cg);
    }

    if (q->mainWindow()) {
        connect(q, SIGNAL(allowedAreasChanged(Qt::ToolBarAreas)),
                q->mainWindow(), SLOT(setSettingsDirty()));
        connect(q, SIGNAL(iconSizeChanged(QSize)),
                q->mainWindow(), SLOT(setSettingsDirty()));
        connect(q, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
                q->mainWindow(), SLOT(setSettingsDirty()));
        connect(q, SIGNAL(movableChanged(bool)),
                q->mainWindow(), SLOT(setSettingsDirty()));
        connect(q, SIGNAL(orientationChanged(Qt::Orientation)),
                q->mainWindow(), SLOT(setSettingsDirty()));
    }

    q->setMovable(!KisToolBar::toolBarsLocked());

    connect(q, SIGNAL(movableChanged(bool)),
            q, SLOT(slotMovableChanged(bool)));

    q->setAcceptDrops(true);

#ifdef HAVE_DBUS
    QDBusConnection::sessionBus().connect(QString(), QStringLiteral("/KisToolBar"), QStringLiteral("org.kde.KisToolBar"),
                                          QStringLiteral("styleChanged"), q, SLOT(slotAppearanceChanged()));
#endif
}

QString KisToolBar::Private::getPositionAsString() const
{
    switch (q->mainWindow()->toolBarArea(const_cast<KisToolBar *>(q))) {
    case Qt::BottomToolBarArea:
        return QStringLiteral("Bottom");
    case Qt::LeftToolBarArea:
        return QStringLiteral("Left");
    case Qt::RightToolBarArea:
        return QStringLiteral("Right");
    case Qt::TopToolBarArea:
    default:
        return QStringLiteral("Top");
    }
}

QMenu *KisToolBar::Private::contextMenu(const QPoint &globalPos)
{
    if (!context) {
        context = new QMenu(q);

        contextButtonTitle = context->addSection(i18nc("@title:menu", "Show Text"));
        contextShowText = context->addAction(QString(), q, SLOT(slotContextShowText()));

        context->addSection(i18nc("@title:menu", "Toolbar Settings"));

        contextOrient = new QMenu(i18nc("Toolbar orientation", "Orientation"), context);

        contextTop = contextOrient->addAction(i18nc("toolbar position string", "Top"), q, SLOT(slotContextTop()));
        contextTop->setChecked(true);
        contextLeft = contextOrient->addAction(i18nc("toolbar position string", "Left"), q, SLOT(slotContextLeft()));
        contextRight = contextOrient->addAction(i18nc("toolbar position string", "Right"), q, SLOT(slotContextRight()));
        contextBottom = contextOrient->addAction(i18nc("toolbar position string", "Bottom"), q, SLOT(slotContextBottom()));

        QActionGroup *positionGroup = new QActionGroup(contextOrient);
        Q_FOREACH (QAction *action, contextOrient->actions()) {
            action->setActionGroup(positionGroup);
            action->setCheckable(true);
        }

        contextMode = new QMenu(i18n("Text Position"), context);

        contextIcons = contextMode->addAction(i18n("Icons Only"), q, SLOT(slotContextIcons()));
        contextText = contextMode->addAction(i18n("Text Only"), q, SLOT(slotContextText()));
        contextTextRight = contextMode->addAction(i18n("Text Alongside Icons"), q, SLOT(slotContextTextRight()));
        contextTextUnder = contextMode->addAction(i18n("Text Under Icons"), q, SLOT(slotContextTextUnder()));

        QActionGroup *textGroup = new QActionGroup(contextMode);
        Q_FOREACH (QAction *action, contextMode->actions()) {
            action->setActionGroup(textGroup);
            action->setCheckable(true);
        }

        contextSize = new QMenu(i18n("Icon Size"), context);

        contextIconSizes.insert(contextSize->addAction(i18nc("@item:inmenu Icon size", "Default"), q, SLOT(slotContextIconSize())),
                                iconSizeSettings.defaultValue());

        QList<int> avSizes;
        avSizes << 16 << 22 << 24 << 32 << 48 << 64 << 128 << 256;

        std::sort(avSizes.begin(), avSizes.end());

        if (avSizes.count() < 10) {
            Q_FOREACH (int it, avSizes) {
                QString text;
                if (it < 19) {
                    text = i18n("Small (%1x%2)", it, it);
                } else if (it < 25) {
                    text = i18n("Medium (%1x%2)", it, it);
                } else if (it < 35) {
                    text = i18n("Large (%1x%2)", it, it);
                } else {
                    text = i18n("Huge (%1x%2)", it, it);
                }

                contextIconSizes.insert(contextSize->addAction(text, q, SLOT(slotContextIconSize())), it);
            }
        } else {
            const int progression[] = { 16, 22, 32, 48, 64, 96, 128, 192, 256 };

            for (uint i = 0; i < 9; i++) {
                Q_FOREACH (int it, avSizes) {
                    if (it >= progression[ i ]) {
                        QString text;
                        if (it < 19) {
                            text = i18n("Small (%1x%2)", it, it);
                        } else if (it < 25) {
                            text = i18n("Medium (%1x%2)", it, it);
                        } else if (it < 35) {
                            text = i18n("Large (%1x%2)", it, it);
                        } else {
                            text = i18n("Huge (%1x%2)", it, it);
                        }

                        contextIconSizes.insert(contextSize->addAction(text, q, SLOT(slotContextIconSize())), it);
                        break;
                    }
                }
            }
        }

        QActionGroup *sizeGroup = new QActionGroup(contextSize);
        Q_FOREACH (QAction *action, contextSize->actions()) {
            action->setActionGroup(sizeGroup);
            action->setCheckable(true);
        }

        if (!q->toolBarsLocked() && !q->isMovable()) {
            unlockedMovable = false;
        }

        delete contextLockAction;
        contextLockAction = new KToggleAction(KisIconUtils::loadIcon(QStringLiteral("system-lock-screen")), i18n("Lock Toolbar Positions"), q);
        contextLockAction->setChecked(q->toolBarsLocked());
        connect(contextLockAction, SIGNAL(toggled(bool)), q, SLOT(slotLockToolBars(bool)));

        context->addMenu(contextMode);
        context->addMenu(contextSize);
        context->addMenu(contextOrient);
        context->addSeparator();

        connect(context, SIGNAL(aboutToShow()), q, SLOT(slotContextAboutToShow()));
    }

    contextButtonAction = q->actionAt(q->mapFromGlobal(globalPos));
    if (contextButtonAction) {
        contextShowText->setText(contextButtonAction->text());
        contextShowText->setIcon(contextButtonAction->icon());
        contextShowText->setCheckable(true);
    }

    contextOrient->menuAction()->setVisible(!q->toolBarsLocked());

    return context;
}

void KisToolBar::Private::setLocked(bool locked)
{
    if (unlockedMovable) {
        q->setMovable(!locked);
    }
}

void KisToolBar::Private::adjustSeparatorVisibility()
{
    bool visibleNonSeparator = false;
    int separatorToShow = -1;

    for (int index = 0; index < q->actions().count(); ++index) {
        QAction *action = q->actions()[ index ];
        if (action->isSeparator()) {
            if (visibleNonSeparator) {
                separatorToShow = index;
                visibleNonSeparator = false;
            } else {
                action->setVisible(false);
            }
        } else if (!visibleNonSeparator) {
            if (action->isVisible()) {
                visibleNonSeparator = true;
                if (separatorToShow != -1) {
                    q->actions()[ separatorToShow ]->setVisible(true);
                    separatorToShow = -1;
                }
            }
        }
    }

    if (separatorToShow != -1) {
        q->actions()[ separatorToShow ]->setVisible(false);
    }
}

Qt::ToolButtonStyle KisToolBar::Private::toolButtonStyleFromString(const QString &_style)
{
    QString style = _style.toLower();
    if (style == QStringLiteral("textbesideicon") || style == QLatin1String("icontextright")) {
        return Qt::ToolButtonTextBesideIcon;
    } else if (style == QStringLiteral("textundericon") || style == QLatin1String("icontextbottom")) {
        return Qt::ToolButtonTextUnderIcon;
    } else if (style == QStringLiteral("textonly")) {
        return Qt::ToolButtonTextOnly;
    } else {
        return Qt::ToolButtonIconOnly;
    }
}

QString KisToolBar::Private::toolButtonStyleToString(Qt::ToolButtonStyle style)
{
    switch (style) {
    case Qt::ToolButtonIconOnly:
    default:
        return QStringLiteral("IconOnly");
    case Qt::ToolButtonTextBesideIcon:
        return QStringLiteral("TextBesideIcon");
    case Qt::ToolButtonTextOnly:
        return QStringLiteral("TextOnly");
    case Qt::ToolButtonTextUnderIcon:
        return QStringLiteral("TextUnderIcon");
    }
}

Qt::ToolBarArea KisToolBar::Private::positionFromString(const QString &position)
{
    Qt::ToolBarArea newposition = Qt::TopToolBarArea;
    if (position == QStringLiteral("left")) {
        newposition = Qt::LeftToolBarArea;
    } else if (position == QStringLiteral("bottom")) {
        newposition = Qt::BottomToolBarArea;
    } else if (position == QStringLiteral("right")) {
        newposition = Qt::RightToolBarArea;
    }
    return newposition;
}

void KisToolBar::Private::slotAppearanceChanged()
{
    loadKDESettings();
    applyCurrentSettings();
}

Qt::ToolButtonStyle KisToolBar::Private::toolButtonStyleSetting()
{
    KConfigGroup group(KSharedConfig::openConfig(), "Toolbar style");
    const QString fallback = KisToolBar::Private::toolButtonStyleToString(Qt::ToolButtonTextBesideIcon);
    return KisToolBar::Private::toolButtonStyleFromString(group.readEntry("ToolButtonStyle", fallback));
}

void KisToolBar::Private::loadKDESettings()
{
    iconSizeSettings[Level_KDEDefault] = q->iconSizeDefault();

    if (isMainToolBar) {
        toolButtonStyleSettings[Level_KDEDefault] = toolButtonStyleSetting();
    } else {
        const QString fallBack = toolButtonStyleToString(Qt::ToolButtonTextBesideIcon);
        KConfigGroup group(KSharedConfig::openConfig(), "Toolbar style");
        const QString value = group.readEntry("ToolButtonStyleOtherToolbars", fallBack);
        toolButtonStyleSettings[Level_KDEDefault] = KisToolBar::Private::toolButtonStyleFromString(value);
    }
}

void KisToolBar::Private::applyCurrentSettings()
{
    const int currentIconSize = iconSizeSettings.currentValue();
    q->setIconSize(QSize(currentIconSize, currentIconSize));
    q->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(toolButtonStyleSettings.currentValue()));

    KisKMainWindow *kmw = q->mainWindow();
    if (kmw) {
        kmw->setSettingsDirty();
    }
}

void KisToolBar::Private::customizeButtonPalette(QToolButton *button, bool checked)
{
    QPalette p = button->palette();
    QColor color = q->palette().color(checked ? QPalette::Highlight : QPalette::Button);
    p.setColor(QPalette::Button, color);
    button->setPalette(p);
}

QAction *KisToolBar::Private::findAction(const QString &actionName, KisKXMLGUIClient **clientOut) const
{
    Q_FOREACH (KisKXMLGUIClient *client, xmlguiClients) {
        QAction *action = client->actionCollection()->action(actionName);
        if (action) {
            if (clientOut) {
                *clientOut = client;
            }
            return action;
        }
    }
    return 0;
}

void KisToolBar::Private::slotContextAboutToShow()
{

    KXmlGuiWindow *kmw = qobject_cast<KXmlGuiWindow *>(q->mainWindow());

    QAction *configureAction = 0;
    const char *actionName;
    actionName = KStandardAction::name(KStandardAction::ConfigureToolbars);
    configureAction = findAction(QLatin1String(actionName));

    if (!configureAction && kmw) {
        configureAction = kmw->actionCollection()->action(QLatin1String(actionName));
    }

    if (configureAction) {
        context->addAction(configureAction);
    }

    context->addAction(contextLockAction);

    if (kmw) {
        kmw->setupToolbarMenuActions();
        QAction *tbAction = kmw->toolBarMenuAction();
        if (!q->toolBarsLocked() && tbAction && tbAction->associatedWidgets().count() > 0) {
            context->addAction(tbAction);
        }
    }

    KisKEditToolBar::setGlobalDefaultToolBar(q->QObject::objectName().toLatin1().constData());

    switch (q->toolButtonStyle()) {
    case Qt::ToolButtonIconOnly:
    default:
        contextIcons->setChecked(true);
        break;
    case Qt::ToolButtonTextBesideIcon:
        contextTextRight->setChecked(true);
        break;
    case Qt::ToolButtonTextOnly:
        contextText->setChecked(true);
        break;
    case Qt::ToolButtonTextUnderIcon:
        contextTextUnder->setChecked(true);
        break;
    }

    QMapIterator< QAction *, int > it = contextIconSizes;
    while (it.hasNext()) {
        it.next();
        if (it.value() == q->iconSize().width()) {
            it.key()->setChecked(true);
            break;
        }
    }

    switch (q->mainWindow()->toolBarArea(q)) {
    case Qt::BottomToolBarArea:
        contextBottom->setChecked(true);
        break;
    case Qt::LeftToolBarArea:
        contextLeft->setChecked(true);
        break;
    case Qt::RightToolBarArea:
        contextRight->setChecked(true);
        break;
    default:
    case Qt::TopToolBarArea:
        contextTop->setChecked(true);
        break;
    }

    const bool showButtonSettings = contextButtonAction
                                    && !contextShowText->text().isEmpty()
                                    && contextTextRight->isChecked();
    contextButtonTitle->setVisible(showButtonSettings);
    contextShowText->setVisible(showButtonSettings);
    if (showButtonSettings) {
        contextShowText->setChecked(contextButtonAction->priority() >= QAction::NormalPriority);
    }
}

void KisToolBar::Private::slotContextAboutToHide()
{
    KXmlGuiWindow *kmw = qobject_cast<KXmlGuiWindow *>(q->mainWindow());
    if (kmw && kmw->toolBarMenuAction()) {
        if (kmw->toolBarMenuAction()->associatedWidgets().count() > 1) {
            context->removeAction(kmw->toolBarMenuAction());
        }
    }

    QAction *configureAction = 0;
    const char *actionName;
    actionName = KStandardAction::name(KStandardAction::ConfigureToolbars);
    configureAction = findAction(QLatin1String(actionName));

    if (!configureAction && kmw) {
        configureAction = kmw->actionCollection()->action(QLatin1String(actionName));
    }

    if (configureAction) {
        context->removeAction(configureAction);
    }

    context->removeAction(contextLockAction);
}

void KisToolBar::Private::slotContextLeft()
{
    q->mainWindow()->addToolBar(Qt::LeftToolBarArea, q);
}

void KisToolBar::Private::slotContextRight()
{
    q->mainWindow()->addToolBar(Qt::RightToolBarArea, q);
}

void KisToolBar::Private::slotContextShowText()
{
    Q_ASSERT(contextButtonAction);
    const QAction::Priority priority = contextShowText->isChecked()
                                       ? QAction::NormalPriority : QAction::LowPriority;
    contextButtonAction->setPriority(priority);

    QString componentName;
    QString filename;
    KisKXMLGUIClient *client;
    if (findAction(contextButtonAction->objectName(), &client)) {
        componentName = client->componentName();
        filename = client->xmlFile();
    }
    if (filename.isEmpty()) {
        componentName = QCoreApplication::applicationName();
        filename = componentName + QStringLiteral("ui.xmlgui");
    }

    const QString configFile = KisKXMLGUIFactory::readConfigFile(filename, componentName);

    QDomDocument document;
    document.setContent(configFile);
    QDomElement elem = KisKXMLGUIFactory::actionPropertiesElement(document);
    QDomElement actionElem = KisKXMLGUIFactory::findActionByName(elem, contextButtonAction->objectName(), true);
    actionElem.setAttribute(QStringLiteral("priority"), priority);
    KisKXMLGUIFactory::saveConfigFile(document, filename, componentName);
}

void KisToolBar::Private::slotContextTop()
{
    q->mainWindow()->addToolBar(Qt::TopToolBarArea, q);
}

void KisToolBar::Private::slotContextBottom()
{
    q->mainWindow()->addToolBar(Qt::BottomToolBarArea, q);
}

void KisToolBar::Private::slotContextIcons()
{
    q->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolButtonStyleSettings[Level_UserSettings] = q->toolButtonStyle();
}

void KisToolBar::Private::slotContextText()
{
    q->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolButtonStyleSettings[Level_UserSettings] = q->toolButtonStyle();
}

void KisToolBar::Private::slotContextTextUnder()
{
    q->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButtonStyleSettings[Level_UserSettings] = q->toolButtonStyle();
}

void KisToolBar::Private::slotContextTextRight()
{
    q->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButtonStyleSettings[Level_UserSettings] = q->toolButtonStyle();
}

void KisToolBar::Private::slotContextIconSize()
{
    QAction *action = qobject_cast<QAction *>(q->sender());
    if (action && contextIconSizes.contains(action)) {
        const int iconSize = contextIconSizes.value(action);
        q->setIconDimensions(iconSize);
    }
}

void KisToolBar::Private::slotLockToolBars(bool lock)
{
    q->setToolBarsLocked(lock);
}

void KisToolBar::Private::slotToolButtonToggled(bool checked)
{
    QToolButton *tb = qobject_cast<QToolButton *>(q->sender());
    if (tb) {
        customizeButtonPalette(tb, checked);
    }
}

KisToolBar::KisToolBar(const QString &objectName, QWidget *parent, bool readConfig)
    : QToolBar(parent),
      d(new Private(this))
{
    setObjectName(objectName);
    d->init(readConfig, objectName == QStringLiteral("mainToolBar"));

    if (QMainWindow *mw = qobject_cast<QMainWindow *>(parent)) {
        mw->addToolBar(this);
    }
}

KisToolBar::~KisToolBar()
{
    delete d->contextLockAction;
    delete d;
}

void KisToolBar::saveSettings(KConfigGroup &cg)
{
    Q_ASSERT(!cg.name().isEmpty());

    const int currentIconSize = iconSize().width();
    if (!cg.hasDefault("IconSize") && currentIconSize == d->iconSizeSettings.defaultValue()) {
        cg.revertToDefault("IconSize");
        d->iconSizeSettings[Level_UserSettings] = Unset;
    } else {
        cg.writeEntry("IconSize", currentIconSize);
        d->iconSizeSettings[Level_UserSettings] = currentIconSize;
    }

    const Qt::ToolButtonStyle currentToolButtonStyle = toolButtonStyle();
    if (!cg.hasDefault("ToolButtonStyle") && currentToolButtonStyle == d->toolButtonStyleSettings.defaultValue()) {
        cg.revertToDefault("ToolButtonStyle");
        d->toolButtonStyleSettings[Level_UserSettings] = Unset;
    } else {
        cg.writeEntry("ToolButtonStyle", d->toolButtonStyleToString(currentToolButtonStyle));
        d->toolButtonStyleSettings[Level_UserSettings] = currentToolButtonStyle;
    }
}

void KisToolBar::addXMLGUIClient(KisKXMLGUIClient *client)
{
    d->xmlguiClients << client;
}

void KisToolBar::removeXMLGUIClient(KisKXMLGUIClient *client)
{
    d->xmlguiClients.remove(client);
}

void KisToolBar::contextMenuEvent(QContextMenuEvent *event)
{
    QToolBar::contextMenuEvent(event);
}

void KisToolBar::loadState(const QDomElement &element)
{
    QMainWindow *mw = mainWindow();
    if (!mw) {
        return;
    }

    {
        QDomNode textNode = element.namedItem(QStringLiteral("text"));
        QByteArray domain;
        QByteArray text;
        QByteArray context;
        if (textNode.isElement()) {
            QDomElement textElement = textNode.toElement();
            domain = textElement.attribute(QStringLiteral("translationDomain")).toUtf8();
            text = textElement.text().toUtf8();
            context = textElement.attribute(QStringLiteral("context")).toUtf8();
        } else {
            textNode = element.namedItem(QStringLiteral("Text"));
            if (textNode.isElement()) {
                QDomElement textElement = textNode.toElement();
                domain = textElement.attribute(QStringLiteral("translationDomain")).toUtf8();
                text = textElement.text().toUtf8();
                context = textElement.attribute(QStringLiteral("context")).toUtf8();
            }
        }

        if (domain.isEmpty()) {
            domain = element.ownerDocument().documentElement().attribute(QStringLiteral("translationDomain")).toUtf8();
            if (domain.isEmpty()) {
                domain = KLocalizedString::applicationDomain();
            }
        }
        QString i18nText;
        if (!text.isEmpty() && !context.isEmpty()) {
            i18nText = i18ndc(domain.constData(), context.constData(), text.constData());
        } else if (!text.isEmpty()) {
            i18nText = i18nd(domain.constData(), text.constData());
        }

        if (!i18nText.isEmpty()) {
            setWindowTitle(i18nText);
        }
    }

    bool loadingAppDefaults = true;
    if (element.hasAttribute(QStringLiteral("tempXml"))) {
        loadingAppDefaults = false;
        const QString iconSizeDefault = element.attribute(QStringLiteral("iconSizeDefault"));
        if (!iconSizeDefault.isEmpty()) {
            d->iconSizeSettings[Level_AppXML] = iconSizeDefault.toInt();
        }
        const QString toolButtonStyleDefault = element.attribute(QStringLiteral("toolButtonStyleDefault"));
        if (!toolButtonStyleDefault.isEmpty()) {
            d->toolButtonStyleSettings[Level_AppXML] = d->toolButtonStyleFromString(toolButtonStyleDefault);
        }
    } else {
        bool newLine = false;
        QString attrNewLine = element.attribute(QStringLiteral("newline")).toLower();
        if (!attrNewLine.isEmpty()) {
            newLine = attrNewLine == QStringLiteral("true");
        }
        if (newLine && mw) {
            mw->insertToolBarBreak(this);
        }

    }

    int newIconSize = -1;
    if (element.hasAttribute(QStringLiteral("iconSize"))) {
        bool ok;
        newIconSize = element.attribute(QStringLiteral("iconSize")).trimmed().toInt(&ok);
        if (!ok) {
            newIconSize = -1;
        }
    }
    if (newIconSize != -1) {
        d->iconSizeSettings[loadingAppDefaults ? Level_AppXML : Level_UserSettings] = newIconSize;
    }

    const QString newToolButtonStyle = element.attribute(QStringLiteral("iconText"));
    if (!newToolButtonStyle.isEmpty()) {
        d->toolButtonStyleSettings[loadingAppDefaults ? Level_AppXML : Level_UserSettings] = d->toolButtonStyleFromString(newToolButtonStyle);
    }

    bool hidden = false;
    {
        QString attrHidden = element.attribute(QStringLiteral("hidden")).toLower();
        if (!attrHidden.isEmpty()) {
            hidden = attrHidden  == QStringLiteral("true");
        }
    }

    Qt::ToolBarArea pos = Qt::NoToolBarArea;
    {
        QString attrPosition = element.attribute(QStringLiteral("position")).toLower();
        if (!attrPosition.isEmpty()) {
            pos = KisToolBar::Private::positionFromString(attrPosition);
        }
    }
    if (pos != Qt::NoToolBarArea) {
        mw->addToolBar(pos, this);
    }

    setVisible(!hidden);

    d->applyCurrentSettings();
}

void KisToolBar::saveState(QDomElement &current) const
{
    Q_ASSERT(!current.isNull());

    current.setAttribute(QStringLiteral("tempXml"), QLatin1String("true"));

    current.setAttribute(QStringLiteral("noMerge"), QLatin1String("1"));
    current.setAttribute(QStringLiteral("position"), d->getPositionAsString().toLower());
    current.setAttribute(QStringLiteral("hidden"), isHidden() ? QLatin1String("true") : QLatin1String("false"));

    const int currentIconSize = iconSize().width();
    if (currentIconSize == d->iconSizeSettings.defaultValue()) {
        current.removeAttribute(QStringLiteral("iconSize"));
    } else {
        current.setAttribute(QStringLiteral("iconSize"), iconSize().width());
    }

    if (toolButtonStyle() == d->toolButtonStyleSettings.defaultValue()) {
        current.removeAttribute(QStringLiteral("iconText"));
    } else {
        current.setAttribute(QStringLiteral("iconText"), d->toolButtonStyleToString(toolButtonStyle()));
    }

    if (d->iconSizeSettings[Level_AppXML] != Unset) {
        current.setAttribute(QStringLiteral("iconSizeDefault"), d->iconSizeSettings[Level_AppXML]);
    }
    if (d->toolButtonStyleSettings[Level_AppXML] != Unset) {
        const Qt::ToolButtonStyle bs = static_cast<Qt::ToolButtonStyle>(d->toolButtonStyleSettings[Level_AppXML]);
        current.setAttribute(QStringLiteral("toolButtonStyleDefault"), d->toolButtonStyleToString(bs));
    }
}

void KisToolBar::applySettings(const KConfigGroup &cg)
{
    Q_ASSERT(!cg.name().isEmpty());

    if (cg.hasKey("IconSize")) {
        d->iconSizeSettings[Level_UserSettings] = cg.readEntry("IconSize", 0);
    }
    if (cg.hasKey("ToolButtonStyle")) {
        d->toolButtonStyleSettings[Level_UserSettings] = d->toolButtonStyleFromString(cg.readEntry("ToolButtonStyle", QString()));
    }

    d->applyCurrentSettings();
}

KisKMainWindow *KisToolBar::mainWindow() const
{
    return qobject_cast<KisKMainWindow *>(const_cast<QObject *>(parent()));
}

void KisToolBar::setIconDimensions(int size)
{
    QToolBar::setIconSize(QSize(size, size));
    d->iconSizeSettings[Level_UserSettings] = size;
}

int KisToolBar::iconSizeDefault() const
{
    return 22;
}

void KisToolBar::slotMovableChanged(bool movable)
{
    setMovable(movable);
}

void KisToolBar::dragEnterEvent(QDragEnterEvent *event)
{
    if (toolBarsEditable() && event->proposedAction() & (Qt::CopyAction | Qt::MoveAction) &&
            event->mimeData()->hasFormat(QStringLiteral("application/x-kde-action-list"))) {
        QByteArray data = event->mimeData()->data(QStringLiteral("application/x-kde-action-list"));

        QDataStream stream(data);

        QStringList actionNames;

        stream >> actionNames;

        Q_FOREACH (const QString &actionName, actionNames) {
            Q_FOREACH (KisKActionCollection *ac, KisKActionCollection::allCollections()) {
                QAction *newAction = ac->action(actionName);
                if (newAction) {
                    d->actionsBeingDragged.append(newAction);
                    break;
                }
            }
        }

        if (d->actionsBeingDragged.count()) {
            QAction *overAction = actionAt(event->pos());

            QFrame *dropIndicatorWidget = new QFrame(this);
            dropIndicatorWidget->resize(8, height() - 4);
            dropIndicatorWidget->setFrameShape(QFrame::VLine);
            dropIndicatorWidget->setLineWidth(3);

            d->dropIndicatorAction = insertWidget(overAction, dropIndicatorWidget);

            insertAction(overAction, d->dropIndicatorAction);

            event->acceptProposedAction();
            return;
        }
    }

    QToolBar::dragEnterEvent(event);
}

void KisToolBar::dragMoveEvent(QDragMoveEvent *event)
{
    if (toolBarsEditable())
        Q_FOREVER {
        if (d->dropIndicatorAction)
            {
                QAction *overAction = 0L;
                Q_FOREACH (QAction *action, actions()) {
                    QWidget *widget = widgetForAction(action);
                    if (event->pos().x() < widget->pos().x() + (widget->width() / 2)) {
                        overAction = action;
                        break;
                    }
                }

                if (overAction != d->dropIndicatorAction) {
                    int dropIndicatorIndex = actions().indexOf(d->dropIndicatorAction);
                    if (dropIndicatorIndex + 1 < actions().count()) {
                        if (actions()[ dropIndicatorIndex + 1 ] == overAction) {
                            break;
                        }
                    } else if (!overAction) {
                        break;
                    }

                    insertAction(overAction, d->dropIndicatorAction);
                }

                event->accept();
                return;
            }
            break;
        }

    QToolBar::dragMoveEvent(event);
}

void KisToolBar::dragLeaveEvent(QDragLeaveEvent *event)
{
    delete d->dropIndicatorAction;
    d->dropIndicatorAction = 0L;
    d->actionsBeingDragged.clear();

    if (toolBarsEditable()) {
        event->accept();
        return;
    }

    QToolBar::dragLeaveEvent(event);
}

void KisToolBar::dropEvent(QDropEvent *event)
{
    if (toolBarsEditable()) {
        Q_FOREACH (QAction *action, d->actionsBeingDragged) {
            if (actions().contains(action)) {
                removeAction(action);
            }
            insertAction(d->dropIndicatorAction, action);
        }
    }

    delete d->dropIndicatorAction;
    d->dropIndicatorAction = 0L;
    d->actionsBeingDragged.clear();

    if (toolBarsEditable()) {
        event->accept();
        return;
    }

    QToolBar::dropEvent(event);
}

void KisToolBar::mousePressEvent(QMouseEvent *event)
{
    if (toolBarsEditable() && event->button() == Qt::LeftButton) {
        if (QAction *action = actionAt(event->pos())) {
            d->dragAction = action;
            d->dragStartPosition = event->pos();
            event->accept();
            return;
        }
    }

    QToolBar::mousePressEvent(event);
}

void KisToolBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!toolBarsEditable() || !d->dragAction) {
        return QToolBar::mouseMoveEvent(event);
    }

    if ((event->pos() - d->dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        event->accept();
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);

        QStringList actionNames;
        actionNames << d->dragAction->objectName();

        stream << actionNames;
    }

    mimeData->setData(QStringLiteral("application/x-kde-action-list"), data);

    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    if (dropAction == Qt::MoveAction)
        if (drag->target() != this) {
            removeAction(d->dragAction);
        }

    d->dragAction = 0L;
    event->accept();
}

void KisToolBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (d->dragAction) {
        d->dragAction = 0L;
        event->accept();
        return;
    }

    QToolBar::mouseReleaseEvent(event);
}

bool KisToolBar::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (me->buttons() & Qt::RightButton)
            if (QWidget *ww = qobject_cast<QWidget *>(watched))
                if (ww->parent() == this && !ww->isEnabled()) {
                    QCoreApplication::postEvent(this, new QContextMenuEvent(QContextMenuEvent::Mouse, me->pos(), me->globalPos()));
                }

    } else if (event->type() == QEvent::ParentChange) {
        if (QWidget *ww = qobject_cast<QWidget *>(watched)) {
            if (!this->isAncestorOf(ww)) {
                ww->removeEventFilter(this);
                Q_FOREACH (QWidget *child, ww->findChildren<QWidget *>()) {
                    child->removeEventFilter(this);
                }
            }
        }
    }

    QToolButton *tb;
    if ((tb = qobject_cast<QToolButton *>(watched))) {
        const QList<QAction *> tbActions = tb->actions();
        if (!tbActions.isEmpty()) {
            if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *me = static_cast<QMouseEvent *>(event);
                if (me->button() == Qt::MiddleButton ) {
                    QAction *act = tbActions.first();
                    if (me->type() == QEvent::MouseButtonPress) {
                        tb->setDown(act->isEnabled());
                    } else {
                        tb->setDown(false);
                        if (act->isEnabled()) {
                            QMetaObject::invokeMethod(act, "triggered", Qt::DirectConnection,
                                                      Q_ARG(Qt::MouseButtons, me->button()),
                                                      Q_ARG(Qt::KeyboardModifiers, QApplication::keyboardModifiers()));
                        }
                    }
                }
            }

            if (event->type() == QEvent::Show || event->type() == QEvent::Paint || event->type() == QEvent::EnabledChange) {
                QAction *act = tb->defaultAction();
                if (act) {
                    const QString text = KLocalizedString::removeAcceleratorMarker(act->iconText().isEmpty() ? act->text() : act->iconText());
                    const QString toolTip = KLocalizedString::removeAcceleratorMarker(act->toolTip());
                    tb->setText(i18nc("@action:intoolbar Text label of toolbar button", "%1", text));
                    tb->setToolTip(i18nc("@info:tooltip Tooltip of toolbar button", "%1", toolTip));
                }
            }
        }
    }

    if (toolBarsEditable()) {
        if (QWidget *ww = qobject_cast<QWidget *>(watched)) {
            switch (event->type()) {
            case QEvent::MouseButtonPress: {
                QMouseEvent *me = static_cast<QMouseEvent *>(event);
                QMouseEvent newEvent(me->type(), mapFromGlobal(ww->mapToGlobal(me->pos())), me->globalPos(),
                                     me->button(), me->buttons(), me->modifiers());
                mousePressEvent(&newEvent);
                return true;
            }
            case QEvent::MouseMove: {
                QMouseEvent *me = static_cast<QMouseEvent *>(event);
                QMouseEvent newEvent(me->type(), mapFromGlobal(ww->mapToGlobal(me->pos())), me->globalPos(),
                                     me->button(), me->buttons(), me->modifiers());
                mouseMoveEvent(&newEvent);
                return true;
            }
            case QEvent::MouseButtonRelease: {
                QMouseEvent *me = static_cast<QMouseEvent *>(event);
                QMouseEvent newEvent(me->type(), mapFromGlobal(ww->mapToGlobal(me->pos())), me->globalPos(),
                                     me->button(), me->buttons(), me->modifiers());
                mouseReleaseEvent(&newEvent);
                return true;
            }
            default:
                break;
            }
        }
    }

    return QToolBar::eventFilter(watched, event);
}

void KisToolBar::actionEvent(QActionEvent *event)
{
    if (event->type() == QEvent::ActionRemoved) {
        QWidget *widget = widgetForAction(event->action());
        if (widget) {
            widget->removeEventFilter(this);

            Q_FOREACH (QWidget *child, widget->findChildren<QWidget *>()) {
                child->removeEventFilter(this);
            }
            QToolButton *tb = qobject_cast<QToolButton *>(widget);
            if (tb) {
                tb->disconnect(this, SLOT(slotToolButtonToggled(bool)));
            }
        }
    }

    QToolBar::actionEvent(event);

    if (event->type() == QEvent::ActionAdded) {
        QWidget *widget = widgetForAction(event->action());
        if (widget) {
            widget->installEventFilter(this);

            Q_FOREACH (QWidget *child, widget->findChildren<QWidget *>()) {
                child->installEventFilter(this);
            }
            if (!(widget->sizePolicy().horizontalPolicy() & QSizePolicy::GrowFlag)
                    && !(orientation() == Qt::Vertical && toolButtonStyle() == Qt::ToolButtonTextBesideIcon)) {
                const int index = layout()->indexOf(widget);
                if (index != -1) {
                    layout()->itemAt(index)->setAlignment(Qt::AlignJustify);
                }
            }
            QToolButton *tb = qobject_cast<QToolButton *>(widget);
            if (tb && event->action()->icon().isNull() == false) {
                d->customizeButtonPalette(tb, tb->isChecked());
                connect(tb, SIGNAL(toggled(bool)), this, SLOT(slotToolButtonToggled(bool)));
                widget->setFixedSize(32, 32);
            }
        }
    }

    d->adjustSeparatorVisibility();
}

bool KisToolBar::toolBarsEditable()
{
    return KisToolBar::Private::s_editable;
}

void KisToolBar::setToolBarsEditable(bool editable)
{
    if (KisToolBar::Private::s_editable != editable) {
        KisToolBar::Private::s_editable = editable;
    }
}

void KisToolBar::setToolBarsLocked(bool locked)
{
    s_toolBarsModel->settoolBarsLocked(locked);
}

bool KisToolBar::toolBarsLocked()
{
    return s_toolBarsModel->toolBarsLocked();
}

void KisToolBar::emitToolbarStyleChanged()
{
#ifdef HAVE_DBUS
    QDBusMessage message = QDBusMessage::createSignal(QStringLiteral("/KisToolBar"), QStringLiteral("org.kde.KisToolBar"), QStringLiteral("styleChanged"));
    QDBusConnection::sessionBus().send(message);
#endif
}

KisToolBarStateModel* KisToolBar::toolBarStateModel()
{
    return s_toolBarsModel;
}

#include "moc_ktoolbar.cpp"
