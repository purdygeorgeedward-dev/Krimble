/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2008 Alexander Dymo <adymo@kdevelop.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KSHORTCUTSCHEMESHELPER_P_H
#define KSHORTCUTSCHEMESHELPER_P_H

#include <QString>
#include <QHash>

class KisKActionCollection;
class KisKXMLGUIClient;
class KConfigBase;

class KisKShortcutSchemesHelper
{
public:

    static QString shortcutSchemeFileName(const QString &schemeName);

    static bool saveShortcutScheme();

    static QHash<QString, QString> schemeFileLocations();

};

#endif
