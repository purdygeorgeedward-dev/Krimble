/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ICON_UTILS_H
#define __KIS_ICON_UTILS_H

#include "kritawidgetutils_export.h"

class QAbstractButton;
class QComboBox;
class QAction;
class QObject;
class QTabBar;
class QTabWidget;

#include <QIcon>
#include <QString>
#include <QStringList>

namespace KisIconUtils
{

    enum StdSizes {
        SizeSmall = 16, SizeSmallMedium = 22, SizeMedium = 32, SizeLarge = 48,
        SizeHuge = 64, SizeEnormous = 128
    };

    enum class Group {
        NoGroup = -1,
        Desktop = 0,
        FirstGroup = 0,
        Toolbar,
        MainToolbar,
        Small,
        Panel,
        Dialog,
        LastGroup,
        User
    };

    KRITAWIDGETUTILS_EXPORT QIcon loadIcon(const QString &name);

    KRITAWIDGETUTILS_EXPORT bool useDarkIcons();

    KRITAWIDGETUTILS_EXPORT void updateIconCommon(QObject *object);

    KRITAWIDGETUTILS_EXPORT void updateIcon(QAbstractButton *button);

    KRITAWIDGETUTILS_EXPORT void clearIconCache();

    KRITAWIDGETUTILS_EXPORT void updateIcon(QComboBox *comboBox);

    KRITAWIDGETUTILS_EXPORT void updateIcon(QAction *action);

    KRITAWIDGETUTILS_EXPORT void updateIcon(QTabBar *tabBar);

    KRITAWIDGETUTILS_EXPORT QStringList allUniqueLoadedIconNames();
}

#endif
