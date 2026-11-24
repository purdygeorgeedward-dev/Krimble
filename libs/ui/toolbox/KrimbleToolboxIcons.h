// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the toolbox icons come in two styles, and the user switches between them (George: "I want the user to be able to switch between the full
// color themed toolbox icons and your clear icons you made for the toolbox."): **Full color**, the picture icons drawn by George (32 PNGs in the resource
// prefix :/krimble-toolbox/, one per tool), and **Silhouette**, the line icons made for Krimble (what the toolbox showed before; George calls them silhouette icons). The choice is the key
// Krimble/ToolboxColorIcons (full color is the default). A tool with no picture icon (any tool not in the table below) always shows its own icon. (Dodge, Burn, Colorize Mask Editing and Enclose and Fill
// are not in the toolbox at all, see the 357th entry.) The table maps a tool's icon name (what its factory gives to setIconName) to the picture file.
#pragma once

#include <QHash>
#include <QIcon>
#include <QIconEngine>
#include <QFile>
#include <QToolButton>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KoIcon.h>

namespace KrimbleToolboxIcons {

inline const QHash<QString, QString> &files()
{
    static const QHash<QString, QString> table = {
        { QStringLiteral("krita_tool_move"),            QStringLiteral("01_01_move") },
        { QStringLiteral("tool_rect_selection"),        QStringLiteral("01_02_rectangular_select") },
        { QStringLiteral("tool_elliptical_selection"),  QStringLiteral("01_03_elliptical_select") },
        { QStringLiteral("tool_outline_selection"),     QStringLiteral("01_04_freehand_select") },
        { QStringLiteral("tool_polygonal_selection"),   QStringLiteral("01_05_polygonal_select") },
        { QStringLiteral("tool_magnetic_selection"),    QStringLiteral("02_01_magnetic_select") },
        { QStringLiteral("tool_contiguous_selection"),  QStringLiteral("02_02_contiguous_select") },
        { QStringLiteral("tool_similar_selection"),     QStringLiteral("02_03_similar_color_select") },
        { QStringLiteral("tool_crop"),                  QStringLiteral("02_04_crop") },
        { QStringLiteral("krita_tool_color_sampler"),   QStringLiteral("02_05_color_sampler") },
        { QStringLiteral("krita_tool_smart_patch"),     QStringLiteral("03_01_healing_brush") },
        { QStringLiteral("krita_tool_freehand"),        QStringLiteral("03_02_freehand_brush") },
        { QStringLiteral("krita_tool_smudge"),          QStringLiteral("03_03_touch_gesture") },
        { QStringLiteral("krita_tool_soften"),          QStringLiteral("03_04_blur") },
        { QStringLiteral("krita_tool_eraser"),          QStringLiteral("03_05_eraser") },
        { QStringLiteral("krita_tool_gradient"),        QStringLiteral("04_01_gradient") },
        { QStringLiteral("krita_tool_color_fill"),      QStringLiteral("04_02_fill") },
        { QStringLiteral("krita_draw_path"),            QStringLiteral("04_03_bezier_curve") },
        { QStringLiteral("select"),                     QStringLiteral("04_04_shape_select") },
        // { QStringLiteral("shape_handling"),             QStringLiteral("04_04_shape_select") },   // (the 358th entry put it on Edit Shapes; George: "I think you should have left it.")
        { QStringLiteral("draw-text"),                  QStringLiteral("04_05_text") },
        { QStringLiteral("krita_tool_rectangle"),       QStringLiteral("05_01_rectangle") },
        { QStringLiteral("krita_tool_ellipse"),         QStringLiteral("05_02_ellipse") },
        { QStringLiteral("krita_tool_line"),            QStringLiteral("05_03_line") },
        { QStringLiteral("krita_tool_polygon"),         QStringLiteral("05_04_polygon") },
        { QStringLiteral("tool_pan"),                   QStringLiteral("05_05_pan") },
        { QStringLiteral("tool_zoom"),                  QStringLiteral("06_01_zoom") },
        { QStringLiteral("krita_tool_transform"),       QStringLiteral("06_02_transform") },
        { QStringLiteral("krimble-free-rotate"),        QStringLiteral("06_03_transform_rotate") },
        { QStringLiteral("krita_tool_assistant"),       QStringLiteral("06_04_assistant_tool") },
        { QStringLiteral("krita_tool_measure"),         QStringLiteral("06_05_measure") },
        { QStringLiteral("krita_tool_reference_images"), QStringLiteral("07_01_reference_images") },
        { QStringLiteral("tool_path_selection"),        QStringLiteral("07_02_edit_path_nodes") }
    };
    return table;
}

// // A third style, Black and white: the silhouette icons drawn in black. The key Krimble/ToolboxGrayIcons is on for it
// // (ToolboxColorIcons is on with it; grayMode() is checked first).
// inline bool colorMode()
// {
//     return KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxColorIcons", false);
// }

// inline bool grayMode()
// {
//     return colorMode() && KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxGrayIcons", false);
// }

// inline void setColorMode(bool on)
// {
//     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
//     group.writeEntry("ToolboxColorIcons", on);
//     group.writeEntry("ToolboxGrayIcons", false);
//     group.config()->sync();
// }

// inline void setGrayMode()
// {
//     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
//     group.writeEntry("ToolboxColorIcons", true);
//     group.writeEntry("ToolboxGrayIcons", true);
//     group.config()->sync();
// }

// Icon styles: White (the silhouette icons as they are), Black (the silhouettes drawn in black), Grayscale (the picture icons
// in shades of gray), Color (the picture icons). Key Krimble/ToolboxIconStyle; an older setting (ToolboxColorIcons / ToolboxGrayIcons) is read once.
// enum IconStyle { White = 0, Color = 1, Black = 2, Grayscale = 3, Classic = 4 };
// Photorealistic: George's photo pictures (krita/pics/krimble-toolbox-photo, full size in docs/krimble-icon-sets/photorealistic);
// a tool without one is drawn as in Classic.
enum IconStyle { White = 0, Color = 1, Black = 2, Grayscale = 3, Classic = 4, Photorealistic = 5 };

// Classic: as in the classic industry-standard toolbar (docs/krimble-refs/2026-11-21_0436_ps-toolbar-labeled-grayscale-icons.jpg),
// tools that are shapes and lines are black line art (the silhouettes in black); tools that are objects are shaded gray art
// (the picture icons in grayscale).
inline bool classicLineArt(const QString &iconName)
{
    static const QStringList lineArt = {
        QStringLiteral("krita_tool_move"), QStringLiteral("tool_rect_selection"), QStringLiteral("tool_elliptical_selection"),
        QStringLiteral("tool_outline_selection"), QStringLiteral("tool_polygonal_selection"), QStringLiteral("tool_magnetic_selection"),
        QStringLiteral("tool_contiguous_selection"), QStringLiteral("tool_similar_selection"), QStringLiteral("tool_crop"),
        QStringLiteral("krita_draw_path"), QStringLiteral("select"), QStringLiteral("draw-text"), QStringLiteral("krita_tool_line"),
        QStringLiteral("tool_pan"), QStringLiteral("krita_tool_transform"), QStringLiteral("krimble-free-rotate"),
        QStringLiteral("krita_tool_assistant"), QStringLiteral("krita_tool_measure"), QStringLiteral("tool_path_selection")
    };
    return lineArt.contains(iconName);
}

inline int &iconStyleCache()
{
    static int style = [] {
        KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
        if (group.hasKey("ToolboxIconStyle")) return qBound(0, group.readEntry("ToolboxIconStyle", 0), 5);
        if (group.readEntry("ToolboxGrayIcons", false)) return int(Black);
        return group.readEntry("ToolboxColorIcons", false) ? int(Color) : int(White);
    }();
    return style;
}

// the Color style was removed ("Remove the color option completely"): a saved Color choice shows White
// inline int iconStyle() { return iconStyleCache() == Color ? int(White) : iconStyleCache(); }
// Color and Classic were removed ("Remove the color option completely"; "Lose the \"classic\" version. It's not great."): Color shows as White, Classic as Grayscale
inline int iconStyle()
{
    if (iconStyleCache() == Color) return int(White);
    if (iconStyleCache() == Classic) return int(Grayscale);
    // Photorealistic is held back (its pictures are not in the app): anyone who picked it sees White
    static const bool photosInApp = QFile::exists(QStringLiteral(":/krimble-toolbox-photo/01_01_move.png"));
    if (iconStyleCache() == Photorealistic && !photosInApp) return int(White);
    return iconStyleCache();
}

inline void setIconStyle(int style)
{
    iconStyleCache() = qBound(0, style, 5);
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxIconStyle", iconStyleCache());
    group.config()->sync();
}

// the picture icons are shown in Color and Grayscale
// inline bool colorMode() { return iconStyle() == Color || iconStyle() == Grayscale || iconStyle() == Classic; }
inline bool colorMode() { return iconStyle() == Color || iconStyle() == Grayscale || iconStyle() == Classic || iconStyle() == Photorealistic; }
inline bool grayMode() { return iconStyle() == Black; }   // (name kept: the Black style)
inline void setColorMode(bool on) { setIconStyle(on ? Color : White); }
inline void setGrayMode() { setIconStyle(Black); }

// recolors another icon at whatever size is asked for: 0 = black silhouette, 1 = shades of gray
// (fixed-size copies can come out tiny on the phone's high-density screen)
class KrimbleRecolorIconEngine : public QIconEngine
{
public:
    KrimbleRecolorIconEngine(const QIcon &source, int kind) : m_source(source), m_kind(kind) {}
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override
    {
        QImage image = m_source.pixmap(size, mode, state).toImage().convertToFormat(QImage::Format_ARGB32);
        if (image.isNull()) return QPixmap();
        if (image.size() != size) image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        for (int y = 0; y < image.height(); ++y) {
            QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x) {
                const QRgb c = line[x];
                if (m_kind == 0) {
                    line[x] = qRgba(24, 24, 28, qAlpha(c));
                } else {
                    const int g = (qRed(c) * 2126 + qGreen(c) * 7152 + qBlue(c) * 722) / 10000;
                    line[x] = qRgba(g, g, g, qAlpha(c));
                }
            }
        }
        return QPixmap::fromImage(image);
    }
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override
    {
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
        const QPixmap pm = pixmap(rect.size() * dpr, mode, state);
        if (pm.isNull()) return;
        const QSizeF shown(pm.width() / dpr, pm.height() / dpr);
        painter->save();
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(QRectF(rect.center().x() + 1 - shown.width() / 2.0, rect.center().y() + 1 - shown.height() / 2.0, shown.width(), shown.height()), pm, QRectF(pm.rect()));
        painter->restore();
    }
    QIconEngine *clone() const override { return new KrimbleRecolorIconEngine(m_source, m_kind); }
private:
    QIcon m_source;
    int m_kind;
};

// a picture in shades of gray (the same light and dark, the transparency kept)
inline QIcon grayIcon(const QIcon &source) { return QIcon(new KrimbleRecolorIconEngine(source, 1)); }

// the earlier version, fixed-size copies
inline QIcon grayIconFixedSizes(const QIcon &source)
{
    QIcon gray;
    QList<QSize> sizes = source.availableSizes();
    if (sizes.isEmpty()) sizes << QSize(128, 128);
    for (const QSize &size : sizes) {
        QImage image = source.pixmap(size).toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < image.height(); ++y) {
            QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x) {
                const QRgb c = line[x];
                const int g = (qRed(c) * 2126 + qGreen(c) * 7152 + qBlue(c) * 722) / 10000;
                line[x] = qRgba(g, g, g, qAlpha(c));
            }
        }
        gray.addPixmap(QPixmap::fromImage(image));
    }
    return gray;
}

inline int &buttonStyleCache()
{
    static int style = KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxButtonStyle", 0);
    return style;
}

inline int buttonStyle()
{
    return buttonStyleCache();
}

inline void setButtonStyle(int style)
{
    buttonStyleCache() = style;
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxButtonStyle", style);
    group.config()->sync();
}

// The Light gray switch: the toolbox buttons get a light gray face (flat or raised), like the classic light gray toolbox. Key Krimble/ToolboxLightGray.
/*
inline bool &lightGrayCache()
{
    static bool on = KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxLightGray", false);
    return on;
}

inline bool lightGray()
{
    return lightGrayCache();
}

inline void setLightGray(bool on)
{
    lightGrayCache() = on;
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxLightGray", on);
    group.config()->sync();
}

inline QColor lightGrayFace()
{
    return QColor(198, 198, 202);
}
*/
// Button background: 0 = the panel's own (default), 1 = light gray, 2 = white, 3 = dark gray, 4 = glow (a dark tile with a teal glow,
// docs/krimble-refs/2026-11-21_1245_glow-background-sample.png: "Let's add that as a standard option, the pretty background."). Key Krimble/ToolboxButtonFace (the older ToolboxLightGray is read once).
inline int &buttonFaceCache()
{
    static int face = [] {
        KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
        if (group.hasKey("ToolboxButtonFace")) return qBound(0, group.readEntry("ToolboxButtonFace", 0), 4);
        return group.readEntry("ToolboxLightGray", false) ? 1 : 0;
    }();
    return face;
}

// inline int buttonFace() { return buttonFaceCache(); }
// Glow (4) is dropped ("Glow background is dropped."): anyone who had picked it sees the panel background
inline int buttonFace() { return buttonFaceCache() == 4 ? 0 : buttonFaceCache(); }

inline void setButtonFace(int face)
{
    buttonFaceCache() = qBound(0, face, 4);
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxButtonFace", buttonFaceCache());
    group.config()->sync();
}

// inline bool lightGray() { return buttonFace() != 0; }   // a light background (light gray or white)
inline bool customFace() { return buttonFace() != 0; }   // a background of its own (light gray, white or dark gray)
inline bool lightGray() { return buttonFace() == 1 || buttonFace() == 2; }   // a light background (light gray or white)
inline void setLightGray(bool on) { setButtonFace(on ? 1 : 0); }

inline QColor lightGrayFace()
{
    // return buttonFace() == 2 ? QColor(250, 250, 252) : QColor(198, 198, 202);
    if (buttonFace() == 3) return QColor(78, 78, 84);
    if (buttonFace() == 4) return QColor(4, 30, 44);   // the glow tile's average, for anything that needs one color
    return buttonFace() == 2 ? QColor(250, 250, 252) : QColor(198, 198, 202);
}

inline bool glowFace() { return buttonFace() == 4; }

// the Glow background: a dark navy tile, lit from the middle with teal, rounded a little
inline void paintGlowTile(QPainter &p, const QRectF &r)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    const qreal radius = qMin(r.width(), r.height()) * 0.10;
    QPainterPath tile;
    tile.addRoundedRect(r, radius, radius);
    QRadialGradient glow(QPointF(r.center().x(), r.top() + r.height() * 0.62), qMax(r.width(), r.height()) * 0.72);
    glow.setColorAt(0.00, QColor(28, 118, 142));
    glow.setColorAt(0.35, QColor(10, 72, 96));
    glow.setColorAt(0.75, QColor(2, 36, 54));
    glow.setColorAt(1.00, QColor(1, 12, 24));
    p.fillPath(tile, glow);
    p.setPen(QPen(QColor(90, 190, 215, 70), qMax<qreal>(1.0, r.width() * 0.012)));
    p.drawPath(tile);
    p.restore();
}

// the photorealistic picture for a tool (or a toolbox button such as "08_02_quick_mask"), or a null icon
inline QIcon photoIcon(const QString &fileName)
{
    if (fileName.isEmpty()) return QIcon();
    const QString path = QStringLiteral(":/krimble-toolbox-photo/") + fileName + QStringLiteral(".png");
    return QFile::exists(path) ? QIcon(path) : QIcon();
}

inline bool hasPhoto(const QString &iconName)
{
    return !photoIcon(files().value(iconName)).isNull();
}

inline bool hasPicture(const QString &iconName)
{
    const auto it = files().constFind(iconName);
    if (it == files().constEnd()) return false;
    return !QIcon(QStringLiteral(":/krimble-toolbox/") + it.value() + QStringLiteral(".png")).isNull();
}

// the icon for a tool: the picture icon in the Full color style (when the tool has one), the program's own icon otherwise
// the silhouette icon drawn in black (its shape and transparency kept): the Black and white style
inline QIcon blackIcon(const QIcon &source) { return QIcon(new KrimbleRecolorIconEngine(source, 0)); }

// the earlier version, fixed-size copies
inline QIcon blackIconFixedSizes(const QIcon &source)
{
    QIcon black;
    QList<QSize> sizes = source.availableSizes();
    if (sizes.isEmpty()) sizes << QSize(32, 32) << QSize(64, 64) << QSize(128, 128);
    for (const QSize &size : sizes) {
        QImage image = source.pixmap(size).toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < image.height(); ++y) {
            QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
            for (int x = 0; x < image.width(); ++x) line[x] = qRgba(24, 24, 28, qAlpha(line[x]));
        }
        black.addPixmap(QPixmap::fromImage(image));
    }
    return black;
}

// draws another icon larger in its square, at whatever size is asked for (fixed-size copies came out tiny on the
// phone's high-density screen: the eyedropper "disappeared" in 1.0.73)
class KrimbleEnlargedIconEngine : public QIconEngine
{
public:
    KrimbleEnlargedIconEngine(const QIcon &source, qreal factor) : m_source(source), m_factor(factor) {}
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override
    {
        const QSize big(qRound(rect.width() * m_factor), qRound(rect.height() * m_factor));
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : 1.0;
        QPixmap large = m_source.pixmap(big * dpr, mode, state);
        if (large.isNull()) return;
        large = large.scaled(big * dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const QSizeF shown(large.width() / dpr, large.height() / dpr);
        const QRectF target(rect.center().x() + 1 - shown.width() / 2.0, rect.center().y() + 1 - shown.height() / 2.0, shown.width(), shown.height());
        painter->save();
        painter->setClipRect(rect);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(target, large, QRectF(large.rect()));
        painter->restore();
    }
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override
    {
        QPixmap pm(size);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        paint(&p, QRect(QPoint(0, 0), size), mode, state);
        p.end();
        return pm;
    }
    QIconEngine *clone() const override { return new KrimbleEnlargedIconEngine(m_source, m_factor); }
private:
    QIcon m_source;
    qreal m_factor;
};

inline QIcon enlargedIcon(const QIcon &source, qreal factor)
{
    return QIcon(new KrimbleEnlargedIconEngine(source, factor));
}

// the earlier version: fixed-size copies (too small on the phone)
inline QIcon enlargedIconFixedSizes(const QIcon &source, qreal factor)
{
    QIcon bigger;
    QList<QSize> sizes = source.availableSizes();
    if (sizes.isEmpty()) sizes << QSize(32, 32) << QSize(64, 64) << QSize(128, 128);
    for (const QSize &size : sizes) {
        const QSize big(qRound(size.width() * factor), qRound(size.height() * factor));
        const QPixmap large = source.pixmap(big).scaled(big, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap pm(size);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.drawPixmap((size.width() - large.width()) / 2, (size.height() - large.height()) / 2, large);
        p.end();
        bigger.addPixmap(pm);
    }
    return bigger;
}

inline QIcon plainIcon(const QString &iconName);

inline QIcon icon(const QString &iconName)
{
    // the eyedropper is drawn small in its square: it is shown 20% larger ("Our eyedropper could be a little bigger.")
    // Krimble 2026-11-23: the eyedropper's own SVG files are drawn 20% larger instead; the wrapper had no icon name, so a theme change
    // skipped it and it stayed light gray in Krimble Light (George: "I want to know why that one tool is borked." / "Yes.")
    return plainIcon(iconName);
}

inline QIcon plainIcon(const QString &iconName)
{
    if (iconStyle() == Photorealistic) {
        const QIcon photo = photoIcon(files().value(iconName));
        if (!photo.isNull()) return photo;
    }
    // if (iconStyle() == Classic) {
    if (iconStyle() == Classic || iconStyle() == Photorealistic) {   // a tool with no photo is drawn as in Classic
        if (classicLineArt(iconName) || !hasPicture(iconName)) return blackIcon(kisIcon(iconName));
        return grayIcon(QIcon(QStringLiteral(":/krimble-toolbox/") + files().value(iconName) + QStringLiteral(".png")));
    }

    // White icons are only shown on a dark background: on light gray or white they are drawn in black ("White should only appear on dark backgrounds.")
    if (iconStyle() == White && lightGray()) return blackIcon(kisIcon(iconName));

    // Black and white: the silhouettes in black ("the problem with those black and white icons is they're just the color ones desaturated. I'd like to see the silhouettes in black.")
    if (grayMode()) return blackIcon(kisIcon(iconName));
    if (colorMode()) {
        const auto it = files().constFind(iconName);
        if (it != files().constEnd()) {
            const QIcon picture(QStringLiteral(":/krimble-toolbox/") + it.value() + QStringLiteral(".png"));
            if (!picture.isNull()) return iconStyle() == Grayscale ? grayIcon(picture) : picture;
        }
    }
    return kisIcon(iconName);
}

// Quick Mask and Screen Mode: the photo in the Photorealistic style, their own drawn icon otherwise
// (kept in the button's "krimbleDrawnIcon" property when the button is made)
inline void refreshGadget(QToolButton *button, const QString &photoName)
{
    if (!button) return;
    const QIcon photo = iconStyle() == Photorealistic ? photoIcon(photoName) : QIcon();
    // button->setIcon(!photo.isNull() ? photo : button->property("krimbleDrawnIcon").value<QIcon>());
    // in the Black style (and on light gray or white buttons) they are drawn black like the tools ("Make UNDO black for black toolbox style.")
    const QIcon drawn = button->property("krimbleDrawnIcon").value<QIcon>();
    const bool black = iconStyle() == Black || lightGray();
    button->setIcon(!photo.isNull() ? photo : (black ? blackIcon(drawn) : drawn));
}

inline void refreshGadgets(QWidget *top)
{
    if (!top) return;
    refreshGadget(top->findChild<QToolButton *>(QStringLiteral("krimbleUndoButton")), QStringLiteral("08_01_undo"));
    refreshGadget(top->findChild<QToolButton *>(QStringLiteral("krimbleQuickMaskButton")), QStringLiteral("08_02_quick_mask"));
    refreshGadget(top->findChild<QToolButton *>(QStringLiteral("krimbleScreenModeButton")), QStringLiteral("08_03_screen_mode"));
}

} // namespace KrimbleToolboxIcons
