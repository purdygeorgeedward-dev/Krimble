// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the toolbox icons come in two styles, and the user switches between them (George: "I want the user to be able to switch between the full
// color themed toolbox icons and your clear icons you made for the toolbox."): **Full color**, the picture icons drawn by George (32 PNGs in the resource
// prefix :/krimble-toolbox/, one per tool), and **Clear**, the program's own line icons (what the toolbox showed before). The choice is the key
// Krimble/ToolboxColorIcons (full color is the default). A tool with no picture icon (Dodge, Burn, Colorize Mask Editing, Enclose and Fill, and any other tool
// not in the table below) always shows its own icon. The table maps a tool's icon name (what its factory gives to setIconName) to the picture file.
#pragma once

#include <QHash>
#include <QIcon>
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

inline bool colorMode()
{
    return KSharedConfig::openConfig()->group("Krimble").readEntry("ToolboxColorIcons", true);
}

inline void setColorMode(bool on)
{
    KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
    group.writeEntry("ToolboxColorIcons", on);
    group.config()->sync();
}

// the icon for a tool: the picture icon in the Full color style (when the tool has one), the program's own icon otherwise
inline QIcon icon(const QString &iconName)
{
    if (colorMode()) {
        const auto it = files().constFind(iconName);
        if (it != files().constEnd()) {
            const QIcon picture(QStringLiteral(":/krimble-toolbox/") + it.value() + QStringLiteral(".png"));
            if (!picture.isNull()) return picture;
        }
    }
    return kisIcon(iconName);
}

} // namespace KrimbleToolboxIcons
