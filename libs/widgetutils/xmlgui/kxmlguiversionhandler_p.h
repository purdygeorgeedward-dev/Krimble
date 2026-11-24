/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KisKXMLGUIVERSIONHANDLER_P_H
#define KisKXMLGUIVERSIONHANDLER_P_H

#include <QStringList>

class KXmlGuiVersionHandler
{
public:
    KXmlGuiVersionHandler(const QStringList &files);

    QString finalFile() const
    {
        return m_file;
    }
    QString finalDocument() const
    {
        return m_doc;
    }

    static QString findVersionNumber(const QString &xml);

private:
    QString m_file;
    QString m_doc;
};

#endif
