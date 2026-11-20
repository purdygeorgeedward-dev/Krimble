/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-02
 * Description : theme manager
 *
 * SPDX-FileCopyrightText: 2006-2011 Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "thememanager.h"
// Qt includes

#include <QStringList>
#include <QRegularExpression>
#include <QFileInfo>
#include <QFile>
#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QActionGroup>
#include <QBitmap>
#include <QPainter>
#include <QPixmap>
#include <QDate>
#include <QScreen>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QDebug>
#include <QAction>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <kcolorscheme.h>
#include <kactioncollection.h>
#include <KoResourcePaths.h>
#include <kactionmenu.h>
#include <kconfig.h>
#include <kconfiggroup.h>

// Calligra
#include <kis_icon.h>
#include <kis_config_notifier.h>
#include "KrimbleVisualStyle.h"
#include "KrimbleCommonStyle.h"
#include "KrimbleVisualStylePainted.h"

#ifdef __APPLE__
#include <QStyle>
#endif

namespace Digikam
{

// ---------------------------------------------------------------


class ThemeManager::ThemeManagerPriv
{
public:

    ThemeManagerPriv()
        : themeMenuActionGroup(0)
        , themeMenuAction(0)
    {
    }

    QString                currentThemeName;
    QMap<QString, QString> themeMap;            // map<theme name, theme config path>

    QActionGroup*          themeMenuActionGroup;
    KActionMenu*           themeMenuAction;
};

ThemeManager::ThemeManager(const QString &theme, QObject *parent)
    : QObject(parent)
    , d(new ThemeManagerPriv)
{
    //qDebug() << "Creating theme manager with theme" << theme;
    d->currentThemeName = theme;
    populateThemeMap();
}

ThemeManager::~ThemeManager()
{
    delete d;
}

QString ThemeManager::currentThemeName() const
{
    //qDebug() << "getting current themename";
    QString themeName;
    if (d->themeMenuAction && d->themeMenuActionGroup) {

        QAction* action = d->themeMenuActionGroup->checkedAction();
        if (action) {
            themeName = action->text().remove('&');
        }

        //qDebug() << "\tthemename from action" << themeName;
    }
    else if (!d->currentThemeName.isEmpty()) {

        //qDebug() << "\tcurrent themename" << d->currentThemeName;
        themeName = d->currentThemeName;
    }
    if (themeName.isEmpty()) {
        themeName = "Krimble Blue-Gray";
    }
    return themeName;
}


Q_DECL_UNUSED static QString krimbleRecolorSheet(const QString &sheet, int hue, int saturationPercent)
{
    static const QRegularExpression colorRx(QStringLiteral("#[0-9a-fA-F]{6}\\b"));
    QString out;
    out.reserve(sheet.size());
    int last = 0;
    QRegularExpressionMatchIterator it = colorRx.globalMatch(sheet);
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        out += sheet.mid(last, m.capturedStart() - last);
        QColor c(m.captured());
        int h = 0, sat = 0, v = 0;
        c.getHsv(&h, &sat, &v);
        if (h >= 221 && h <= 233 && sat >= 102 && sat <= 153) {
            c.setHsv(hue, saturationPercent * 255 / 100, v);
            out += c.name();
        } else {
            out += m.captured();
        }
        last = m.capturedEnd();
    }
    out += sheet.mid(last);
    return out;
}

Q_DECL_UNUSED static QString krimbleSameHueSheet(const QString &sheet, int hue, int saturationPercent)
{
    static const QRegularExpression colorRx(QStringLiteral("#[0-9a-fA-F]{6}\\b"));
    QString out;
    out.reserve(sheet.size());
    int last = 0;
    QRegularExpressionMatchIterator it = colorRx.globalMatch(sheet);
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        out += sheet.mid(last, m.capturedStart() - last);
        QColor c(m.captured());
        int h = 0, sat = 0, v = 0;
        c.getHsv(&h, &sat, &v);
        if (h >= 219 && h <= 237 && sat >= 64 && sat <= 166) {
            c.setHsv(hue, saturationPercent * 255 / 100, v);
            out += c.name();
        } else {
            out += m.captured();
        }
        last = m.capturedEnd();
    }
    out += sheet.mid(last);
    return out;
}

static QString krimbleGraySheet(const QString &sheet)
{
    static const QRegularExpression colorRx(QStringLiteral("#[0-9a-fA-F]{6}\\b"));
    QString out;
    out.reserve(sheet.size());
    int last = 0;
    QRegularExpressionMatchIterator it = colorRx.globalMatch(sheet);
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        out += sheet.mid(last, m.capturedStart() - last);
        const QColor c(m.captured());
        int h = 0, sat = 0, v = 0;
        c.getHsv(&h, &sat, &v);
        if (h >= 180 && h <= 260 && sat > 140) {
            out += m.captured();
        } else if ((h >= 180 && h <= 260 && sat > 26) || (sat <= 26 && v >= 12 && v <= 243)) {
            const int y = qRound(0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue());
            out += QColor(qMin(255, y + 4), y, qMax(0, y - 4)).name();
        } else {
            out += m.captured();
        }
        last = m.capturedEnd();
    }
    out += sheet.mid(last);
    out.replace(QRegularExpression(QStringLiteral("border-radius:\\s*[0-9.]+px")), QStringLiteral("border-radius: 0px"));
    return out;
}

void ThemeManager::setCurrentTheme(const QString& name)
{
    d->currentThemeName = (name == QStringLiteral("Krimble dark") || name == QStringLiteral("Krimble Blue") || name.contains(QStringLiteral("Krita"), Qt::CaseInsensitive)) ? QStringLiteral("Krimble Blue-Gray") : name;

    if (d->themeMenuAction  && d->themeMenuActionGroup) {
        QList<QAction*> list = d->themeMenuActionGroup->actions();
        Q_FOREACH (QAction* action, list) {
            // if (action->text().remove('&') == name) {
            if (action->text().remove('&') == d->currentThemeName) {   // 695th: the mapped name, so the default's entry is checked when an old name was saved
                action->setChecked(true);
            }
        }
    }
    slotChangePalette();
}

void ThemeManager::slotChangePalette()
{
    if (currentThemeName() == "System") {
        qApp->setPalette(QPalette());
        Q_EMIT signalThemeChanged();
        return;
    }

    //qDebug() << "slotChangePalette" << sender();

    // We must clear the icon cache before the palette is changed. That way
    // The widgets can change the icons properly when they receive the
    // PaletteChange event if needed.
    KisIconUtils::clearIconCache();

    QString theme(currentThemeName());
    if (!d->themeMap.contains(theme) && theme != QStringLiteral("System")) theme = QStringLiteral("Krimble Blue-Gray");
    QString filename        = d->themeMap.value(theme);
    KSharedConfigPtr config = KSharedConfig::openConfig(filename);

    QPalette palette               = qApp->palette();
    QPalette::ColorGroup states[3] = { QPalette::Active, QPalette::Inactive, QPalette::Disabled };
    // TT thinks tooltips shouldn't use active, so we use our active colors for all states
    KColorScheme schemeTooltip(QPalette::Active, KColorScheme::Tooltip, config);

    for ( int i = 0; i < 3 ; ++i ) {

        QPalette::ColorGroup state = states[i];
        KColorScheme schemeView(state,      KColorScheme::View,      config);
        KColorScheme schemeWindow(state,    KColorScheme::Window,    config);
        KColorScheme schemeButton(state,    KColorScheme::Button,    config);
        KColorScheme schemeSelection(state, KColorScheme::Selection, config);

        palette.setBrush(state, QPalette::WindowText,      schemeWindow.foreground());
        palette.setBrush(state, QPalette::Window,          schemeWindow.background());
        palette.setBrush(state, QPalette::Base,            schemeView.background());
        palette.setBrush(state, QPalette::Text,            schemeView.foreground());
        palette.setBrush(state, QPalette::Button,          schemeButton.background());
        palette.setBrush(state, QPalette::ButtonText,      schemeButton.foreground());
        palette.setBrush(state, QPalette::Highlight,       schemeSelection.background());
        palette.setBrush(state, QPalette::HighlightedText, schemeSelection.foreground());
        palette.setBrush(state, QPalette::ToolTipBase,     schemeTooltip.background());
        palette.setBrush(state, QPalette::ToolTipText,     schemeTooltip.foreground());
        palette.setBrush(state, QPalette::PlaceholderText, schemeView.foreground(KColorScheme::InactiveText));

        palette.setColor(state, QPalette::Light,           schemeWindow.shade(KColorScheme::LightShade));
        palette.setColor(state, QPalette::Midlight,        schemeWindow.shade(KColorScheme::MidlightShade));
        palette.setColor(state, QPalette::Mid,             schemeWindow.shade(KColorScheme::MidShade));
        palette.setColor(state, QPalette::Dark,            schemeWindow.shade(KColorScheme::DarkShade));
        palette.setColor(state, QPalette::Shadow,          schemeWindow.shade(KColorScheme::ShadowShade));
#ifdef Q_OS_ANDROID
        palette.setColor(state, QPalette::Light,           QColor(0x4A, 0x50, 0x74));
        palette.setColor(state, QPalette::Midlight,        QColor(0x41, 0x46, 0x6A));
#endif

        palette.setBrush(state, QPalette::AlternateBase,   schemeView.background(KColorScheme::AlternateBackground));
        palette.setBrush(state, QPalette::Link,            schemeView.foreground(KColorScheme::LinkText));
        palette.setBrush(state, QPalette::LinkVisited,     schemeView.foreground(KColorScheme::VisitedText));
    }
    
    //qDebug() << ">>>>>>>>>>>>>>>>>> going to set palette on app" << theme;
    // hint for the style to synchronize the color scheme with the window manager/compositor
    qApp->setProperty("KDE_COLOR_SCHEME_PATH", filename);
    qApp->setPalette(palette);
#ifdef Q_OS_ANDROID
    qApp->setProperty("krimblePaintedTheme", theme == QStringLiteral("Krimble Painted"));
    qApp->setProperty("krimbleFlatTheme", theme == QStringLiteral("Krimble Dark Gray") || !theme.startsWith(QStringLiteral("Krimble")));
    qApp->setProperty("krimbleNoRaisedButtons", theme == QStringLiteral("Krimble Dark Gray"));
    qApp->setProperty("krimbleSelectionGradient", theme.startsWith(QStringLiteral("Krimble")) && theme != QStringLiteral("Krimble Dark Gray"));
    QString krimbleSheet;
    if (theme == QStringLiteral("Krimble Blue")) {
        krimbleSheet = (krimbleVisualStyleSheet());
    } else if (theme == QStringLiteral("Krimble Blue-Gray")) {
        krimbleSheet = (krimbleRecolorSheet(krimbleVisualStyleSheet(), 236, 33));
    } else if (theme == QStringLiteral("Krimble Dark Gray")) {
        krimbleSheet = (krimbleGraySheet(krimbleVisualStyleSheet()));
    } else if (theme == QStringLiteral("Krimble Painted")) {
        krimbleSheet = (krimblePaintedStyleSheet() + QStringLiteral(
            "\nQScrollBar:vertical { background: #03050a; width: 18px; margin: 0px; border: 1px solid #000000; }\n"
            // "QScrollBar:horizontal { background: #0e1226; height: 16px; margin: 0px; border: 1px solid #05070f; }\n"
            "QScrollBar:horizontal { background: #03050a; height: 18px; margin: 0px; border: 1px solid #000000; }\n"
            "QScrollBar::handle:vertical { background: palette(button); min-height: 36px; margin: 1px; border: 1px solid palette(mid); border-radius: 3px; }\n"
            "QScrollBar::handle:horizontal { background: palette(button); min-width: 36px; margin: 1px; border: 1px solid palette(mid); border-radius: 3px; }\n"
            "QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; border: none; background: none; }\n"
            "QScrollBar::add-page, QScrollBar::sub-page { background: none; }\n"));
    } else {
        krimbleSheet = (QString());
    }
    if (!krimbleSheet.isEmpty()) {
        const QString baseColor = qApp->palette().color(QPalette::Window).darker(135).name();
        krimbleSheet = (krimbleSheet + QStringLiteral("\nQMdiArea { background: %1; }\nKisWelcomePageWidget { background: %1; }\n").arg(baseColor));
        const QString recentColor = qApp->palette().color(QPalette::Window).darker(200).name();
        krimbleSheet = (krimbleSheet + QStringLiteral("\nKisWelcomePageWidget QListView#recentDocumentsListView { background: %1; border: 1px solid %2; }\n").arg(recentColor, baseColor));
    }
    if (!krimbleSheet.isEmpty()) krimbleSheet += krimbleCommonStyleSheet();
    if (theme == QStringLiteral("Krimble Blue-Gray")) krimbleSheet = krimbleRecolorSheet(krimbleSheet, 236, 33);
    else if (theme == QStringLiteral("Krimble Dark Gray")) krimbleSheet = krimbleGraySheet(krimbleSheet);
    qApp->setStyleSheet(krimbleSheet);   // 696th: the one and only call
#endif
    KisConfigNotifier::instance()->notifyColorThemeChanged(filename);

    Q_EMIT signalThemeChanged();
}

void ThemeManager::setThemeMenuAction(KActionMenu* const action)
{
    d->themeMenuAction = action;
    populateThemeMenu();
}

void ThemeManager::registerThemeActions(KisKActionCollection *actionCollection)
{
    if (!d->themeMenuAction) return;
    actionCollection->addAction("theme_menu", d->themeMenuAction);
}

void ThemeManager::populateThemeMenu()
{
    if (!d->themeMenuAction) return;

    d->themeMenuAction->menu()->clear();
    delete d->themeMenuActionGroup;

    d->themeMenuActionGroup = new QActionGroup(d->themeMenuAction);
    connect(d->themeMenuActionGroup, SIGNAL(triggered(QAction*)),
            this, SLOT(slotChangePalette()));

    QAction * action;
    QStringList schemeFiles = KoResourcePaths::findAllAssets("data", "color-schemes/*.colors");
    schemeFiles += KoResourcePaths::findAllAssets("genericdata", "color-schemes/*.colors");

    QMap<QString, QAction*> actionMap;
    for (int i = 0; i < schemeFiles.size(); ++i) {
        const QString filename  = schemeFiles.at(i);
        const QFileInfo info(filename);
        KSharedConfigPtr config = KSharedConfig::openConfig(filename);
        QIcon icon = createSchemePreviewIcon(config);
        KConfigGroup group(config, "General");
        const QString name = group.readEntry("Name", info.completeBaseName());
        action = new QAction(name, d->themeMenuActionGroup);
        action->setIcon(icon);
        action->setCheckable(true);
        actionMap.insert(name, action);
    }

#ifdef Q_OS_MAC
    // Add a "System" theme, which resets the palette to system colors
    // It only seems to work as expected on macOS.
    action = new QAction("System", d->themeMenuActionGroup);
    action->setCheckable(true);
    actionMap.insert("System", action);
#endif

    // sort the list
    QStringList actionMapKeys = actionMap.keys();
    actionMapKeys.sort();

    const QString requestedThemeName = d->currentThemeName.isEmpty() ? QStringLiteral("Krimble Blue-Gray")
                                                                     : d->currentThemeName;
    Q_FOREACH (const QString& name, actionMapKeys) {
        if (name == requestedThemeName) {
            actionMap.value(name)->setChecked(true);
        }
        d->themeMenuAction->addAction(actionMap.value(name));
    }
}

QPixmap ThemeManager::createSchemePreviewIcon(const KSharedConfigPtr& config)
{
    // code taken from kdebase/workspace/kcontrol/colors/colorscm.cpp
    const uchar bits1[] = { 0xff, 0xff, 0xff, 0x2c, 0x16, 0x0b };
    const uchar bits2[] = { 0x68, 0x34, 0x1a, 0xff, 0xff, 0xff };
    const QSize bitsSize(24, 2);
    const QBitmap b1    = QBitmap::fromData(bitsSize, bits1);
    const QBitmap b2    = QBitmap::fromData(bitsSize, bits2);

    QPixmap pixmap(23, 16);
    pixmap.fill(Qt::black); // FIXME use some color other than black for borders?

    KConfigGroup group(config, "WM");
    QPainter p(&pixmap);
    KColorScheme windowScheme(QPalette::Active, KColorScheme::Window, config);
    p.fillRect(1,  1, 7, 7, windowScheme.background());
    p.fillRect(2,  2, 5, 2, QBrush(windowScheme.foreground().color(), b1));

    KColorScheme buttonScheme(QPalette::Active, KColorScheme::Button, config);
    p.fillRect(8,  1, 7, 7, buttonScheme.background());
    p.fillRect(9,  2, 5, 2, QBrush(buttonScheme.foreground().color(), b1));

    p.fillRect(15,  1, 7, 7, group.readEntry("activeBackground", QColor(96, 148, 207)));
    p.fillRect(16,  2, 5, 2, QBrush(group.readEntry("activeForeground", QColor(255, 255, 255)), b1));

    KColorScheme viewScheme(QPalette::Active, KColorScheme::View, config);
    p.fillRect(1,  8, 7, 7, viewScheme.background());
    p.fillRect(2, 12, 5, 2, QBrush(viewScheme.foreground().color(), b2));

    KColorScheme selectionScheme(QPalette::Active, KColorScheme::Selection, config);
    p.fillRect(8,  8, 7, 7, selectionScheme.background());
    p.fillRect(9, 12, 5, 2, QBrush(selectionScheme.foreground().color(), b2));

    p.fillRect(15,  8, 7, 7, group.readEntry("inactiveBackground", QColor(224, 223, 222)));
    p.fillRect(16, 12, 5, 2, QBrush(group.readEntry("inactiveForeground", QColor(20, 19, 18)), b2));

    p.end();
    return pixmap;
}

void ThemeManager::populateThemeMap()
{
    QStringList schemeFiles = KoResourcePaths::findAllAssets("data", "color-schemes/*.colors");
    schemeFiles += KoResourcePaths::findAllAssets("genericdata", "color-schemes/*.colors");
    
    for (int i = 0; i < schemeFiles.size(); ++i) {
        const QString filename  = schemeFiles.at(i);
        const QFileInfo info(filename);
        KSharedConfigPtr config = KSharedConfig::openConfig(filename);
        KConfigGroup group(config, "General");
        const QString name = group.readEntry("Name", info.completeBaseName());
        d->themeMap.insert(name, filename);
    }


}

}  // namespace Digikam
