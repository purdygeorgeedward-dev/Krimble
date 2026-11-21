// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the toolbox icons come in two styles, and the user switches between them (George: "I want the user to be able to switch between the full
// color themed toolbox icons and your clear icons you made for the toolbox."): **Full color**, the picture icons drawn by George (32 PNGs in the resource
// prefix :/krimble-toolbox/, one per tool), and **Silhouette**, the line icons made for Krimble (what the toolbox showed before; George calls them silhouette icons). The choice is the key
// Krimble/ToolboxColorIcons (full color is the default). A tool with no picture icon (any tool not in the table below) always shows its own icon. (Dodge, Burn, Colorize Mask Editing and Enclose and Fill
// are not in the toolbox at all, see the 357th entry.) The table maps a tool's icon name (what its factory gives to setIconName) to the picture file.
#pragma once

#include <QHash>
#include <QIcon>
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

// A third style, Black and white: the picture icons in shades of gray. The key Krimble/ToolboxGrayIcons is on for it
// (with ToolboxColorIcons on, since it uses the pictures).
inline bool colorMode()
{
    return KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxColorIcons", false);
}

inline bool grayMode()
{
    return colorMode() && KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxGrayIcons", false);
}

inline void setColorMode(bool on)
{
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxColorIcons", on);
    group.writeEntry("ToolboxGrayIcons", false);
    group.config()->sync();
}

inline void setGrayMode()
{
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxColorIcons", true);
    group.writeEntry("ToolboxGrayIcons", true);
    group.config()->sync();
}

// a picture in shades of gray (the same light and dark, the transparency kept)
inline QIcon grayIcon(const QIcon &source)
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

inline bool hasPicture(const QString &iconName)
{
    const auto it = files().constFind(iconName);
    if (it == files().constEnd()) return false;
    return !QIcon(QStringLiteral(":/krimble-toolbox/") + it.value() + QStringLiteral(".png")).isNull();
}

// the icon for a tool: the picture icon in the Full color style (when the tool has one), the program's own icon otherwise
inline QIcon icon(const QString &iconName)
{
    if (colorMode()) {
        const auto it = files().constFind(iconName);
        if (it != files().constEnd()) {
            const QIcon picture(QStringLiteral(":/krimble-toolbox/") + it.value() + QStringLiteral(".png"));
            // if (!picture.isNull()) return picture;
            if (!picture.isNull()) return grayMode() ? grayIcon(picture) : picture;
        }
    }
    return kisIcon(iconName);
}

} // namespace KrimbleToolboxIcons
