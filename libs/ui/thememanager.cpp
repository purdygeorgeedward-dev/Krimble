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
#include "KrimbleVisualStyleNov10.h"   // Krimble 2026-11-12 (task 31): the Nov 10 sheet (the default theme)
#include "KrimbleVisualStylePainted.h"   // Krimble 2026-11-11: the painted sheet, for the theme "Krimble Painted"   // Krimble 2026-11-04: krimbleVisualStyleSheet(), re-applied after the theme palette

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
        //qDebug() << "\tfallback";
        // KRIMBLE 2026-10-02: fallback renamed. The theme is now called
        // "Krimble dark"; falling back to the old name "Krita dark" found no
        // theme file and left the app with the plain gray default palette.
        // themeName = "Krita dark";
        // themeName = "Krimble dark";
        themeName = "Krimble Blue-Gray";   // Krimble 2026-11-12 (task 27): the default theme
    }
    //qDebug() << "\tresult" << themeName;
    return themeName;
}


// Krimble 2026-11-12 (task 27, second part): George: "Please keep all the glow stuff. It's all really good." (after the default theme became the plain blue-gray look). The theme "Krimble Blue-Gray"
// is the flat sheet (the glow edges, the dark insides of selections, the black lines, the bars) in the old blue-gray: every color of the sheet that is one of the theme's backgrounds (hue 221 to 233,
// saturation 40 to 60 percent: the hue 227, saturation 48 family) is given hue 236 and saturation 33, as the old palette (#373952) has, and keeps its brightness; the accent blues, the cyan
// edge, the dark navy inside the glow, the text and the black are not backgrounds and stay as they are. So there is one sheet, in KrimbleVisualStyle.h, and two color families.
// Krimble 2026-11-12 (task 31): not used now (the default theme has the Nov 10 sheet); kept
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

// Krimble 2026-11-12 (task 34): George, about the default theme (the Nov 10 gradients): "And all the same hue in the theme?" The sheet's blue-gray colors drifted (hue 223 to 231, saturation 34 to 63: the
// "hue drift" of the Nov 10 brief). Every color of the sheet that is a blue-gray background, line or highlight (hue 219 to 237, saturation 25 to 65 percent) gets the SAME hue and saturation, and keeps
// its own brightness (V): George's rule of Nov 11, "the same hue and saturation but different brightness" (227 and 48, Color Lab's HSV). The accent blues (hue 199 to 216, saturation over 65), the cyan
// glow, the navy insides, the text (saturation under 25) and black and white are not in that range and stay. The gradients keep their shape: only each stop's hue and saturation change.
static QString krimbleSameHueSheet(const QString &sheet, int hue, int saturationPercent)
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

void ThemeManager::setCurrentTheme(const QString& name)
{
    //qDebug() << "setCurrentTheme();" << d->currentThemeName << "to" << name;
    // d->currentThemeName = name;
    // Krimble 2026-11-12 (task 27): the theme "Krimble dark" is gone: its colors and the flat style sheet are now the theme "Krimble Blue", and the default theme is "Krimble Blue-Gray" (the
    // old plain palette). A settings file that still says "Krimble dark" is read as the default, so it does not find no theme file and leave the app with the bare default palette.
    // d->currentThemeName = (name == QStringLiteral("Krimble dark")) ? QStringLiteral("Krimble Blue-Gray") : name;
    // Krimble 2026-11-12 (task 36): George: "Delete Krimble Blue. It looks bad." The theme is not installed any more; a settings file that says "Krimble Blue" (or "Krimble dark") is read as the default, Krimble Blue-Gray.
    d->currentThemeName = (name == QStringLiteral("Krimble dark") || name == QStringLiteral("Krimble Blue")) ? QStringLiteral("Krimble Blue-Gray") : name;

    if (d->themeMenuAction  && d->themeMenuActionGroup) {
        QList<QAction*> list = d->themeMenuActionGroup->actions();
        Q_FOREACH (QAction* action, list) {
            if (action->text().remove('&') == name) {
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
        // Krimble 2026-11-05: George's screenshots show thin white lines (beside the canvas, along the edges of panels) and a white frame around dialogs. Qt's Fusion
        // style, which Krimble uses, draws the highlight edge of frames and window frames (QStyle::PE_FrameWindow, QFusionStyle) with palette.light() and
        // palette.midlight(), and the lines above make those a bright gray (the "light shade" of the window color). They are the theme's blue-gray family
        // now: Light #4A5074, Midlight #41466A. The five lines above are kept; this only replaces two of their results on Android. This is the
        // white-line part only: it does not by itself prove that the large white box around dialogs comes from the same drawing.
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
    // Krimble 2026-11-04: George: the app starts with a weird gray (window and behind the toolbox) that goes away
    // after using it a while. The application style sheet is set early in KisApplication.cpp, before this palette is
    // applied; widgets styled before the palette changed can keep the old (gray) colors until something makes Qt
    // repolish them. Setting the same style sheet again here, right after the palette, makes Qt repolish every widget
    // with the theme's colors. This also runs when the theme is changed from the menu.
    // Krimble 2026-11-11: George: "The painted in theme has to go. It conflicts with all other themes." and "The other themes stopped working with the new gradient theme."
    // The sheet used to be applied for every theme, so it painted over the other themes' colors. It is now applied only while the "Krimble dark" theme is chosen;
    // for any other theme the application style sheet is cleared, so that theme's own palette shows.
    // qApp->setStyleSheet(krimbleVisualStyleSheet());
    // qApp->setStyleSheet(theme == QStringLiteral("Krimble dark") ? krimbleVisualStyleSheet() : QString());
    // Krimble 2026-11-11: George: "Can we put in the Krimble gradient theme as an option?" Three cases, by the theme's name: "Krimble dark" gets the flat sheet, "Krimble Painted" gets the
    // painted (gradient) sheet (KrimbleVisualStylePainted.h), and every other theme gets no sheet at all.
    // qApp->setProperty("krimblePaintedTheme", theme == QStringLiteral("Krimble Painted"));
    // Krimble 2026-11-12 (task 31): the Nov 10 look painted the menu bar across (dark at both ends) too: the default theme "Krimble Blue-Gray" has it again.
    // qApp->setProperty("krimblePaintedTheme", theme == QStringLiteral("Krimble Painted") || theme == QStringLiteral("Krimble Blue-Gray"));
    // Krimble 2026-11-13 (task 46): George: "The main menu bar is ugly. Remove the paint job on it." The menu bar painting (the gradient dark at both ends, with its lines) is back to the Krimble Painted theme only; the default theme leaves the bar to the style sheet (transparent over the window) and the one-pixel lines.
    qApp->setProperty("krimblePaintedTheme", theme == QStringLiteral("Krimble Painted"));   // Krimble 2026-11-11: the menu bar filter (KisMainWindow.cpp) paints its gradient only for this theme
    // if (theme == QStringLiteral("Krimble dark")) {
    if (theme == QStringLiteral("Krimble Blue")) {   // Krimble 2026-11-12 (task 27): the flat sheet belongs to the theme "Krimble Blue" now; "Krimble Blue-Gray" (the default) has none
        qApp->setStyleSheet(krimbleVisualStyleSheet());
    } else if (theme == QStringLiteral("Krimble Blue-Gray")) {   // Krimble 2026-11-12 (task 27): the default theme: the same sheet in the old blue-gray colors
        // Krimble 2026-11-13 (task 63): George: "I TOLD YOU TO SPLIT THEM OFF!!!!!! TO USE SEPARATE THEMES!!!!!!!" The default theme is the flat one again, as in 1.0.56-beta: the flat sheet recolored to the old blue-gray (hue 236, saturation 33) with the original palette. The Nov 10 look
        // is its own theme below. The lines that were here (the Nov 10 sheet on the default theme) are in the branch of "Krimble Bevel".
        qApp->setStyleSheet(krimbleRecolorSheet(krimbleVisualStyleSheet(), 236, 33));
    } else if (theme == QStringLiteral("Krimble Bevel")) {   // Krimble 2026-11-13 (task 63): George: "I TOLD YOU TO SPLIT THEM OFF!!!!!! TO USE SEPARATE THEMES!!!!!!!" the Nov 10 look as its own theme
        // qApp->setStyleSheet(krimbleRecolorSheet(krimbleVisualStyleSheet(), 236, 33));   // task 27 (second part): the flat sheet recolored
        // qApp->setStyleSheet(krimbleNov10StyleSheet());   // task 31 (before task 34)
        qApp->setStyleSheet(krimbleSameHueSheet(krimbleNov10StyleSheet(), 227, 48));   // Krimble 2026-11-12 (task 34): all the same hue and saturation. Krimble 2026-11-12 (task 31): George: "The Nov 10 version is so much better. Revert to that and make it the default."
    } else if (theme == QStringLiteral("Krimble Painted")) {
        qApp->setStyleSheet(krimblePaintedStyleSheet());
    } else {
        qApp->setStyleSheet(QString());
    }
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

    // KRIMBLE 2026-10-02: currentThemeName() cannot be used here. The action
    // group was just created and has nothing checked yet, so it returns the
    // fallback name and no menu entry got checked. Compare against the theme
    // name that was requested instead.
    // const QString requestedThemeName = d->currentThemeName.isEmpty() ? QStringLiteral("Krimble dark")
    //                                                                  : d->currentThemeName;
    const QString requestedThemeName = d->currentThemeName.isEmpty() ? QStringLiteral("Krimble Blue-Gray")   // Krimble 2026-11-12 (task 27): the default theme
                                                                     : d->currentThemeName;
    Q_FOREACH (const QString& name, actionMapKeys) {
        // if ( name ==  currentThemeName()) {
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
