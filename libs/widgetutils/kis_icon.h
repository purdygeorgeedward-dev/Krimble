/*
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOICON_H
#define KOICON_H

#include <kis_icon_utils.h>
#include <QString>

#define koIcon(name) (KisIconUtils::loadIcon(QLatin1String(name)))
#define kisIcon(name) (KisIconUtils::loadIcon(name))
#define koIconName(name) (QLatin1String(name))
#define koIconNameCStr(name) (name)

#define koIconNeeded(comment, neededName) (KisIconUtils::loadIcon(QLatin1String(neededName)))
#define koIconNeededWithSubs(comment, neededName, substituteName) (KisIconUtils::loadIcon(QLatin1String(substituteName)))
#define koIconNameNeeded(comment, neededName) (QLatin1String(neededName))
#define koIconNameNeededWithSubs(comment, neededName, substituteName) (QLatin1String(substituteName))
#define koIconNameCStrNeeded(comment, neededName) (neededName)
#define koIconNameCStrNeededWithSubs(comment, neededName, substituteName) (substituteName)

#endif
