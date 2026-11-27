/*
 *  SPDX-FileCopyrightText: 2016 Michael Abrahams <miabraha@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_CURSOR_CACHE_H
#define KIS_CURSOR_CACHE_H

#include <QString>
#include <QCursor>
#include <QObject>
#include <QPair>
#include <QPoint>
#include <QHash>

class KisCursorCache: public QObject
{
    Q_OBJECT
public:
    static KisCursorCache* instance();
    KisCursorCache();

    QCursor load(const QString & cursorName, int width, int height, int hotspotX, int hotspotY);

private:

    QHash<QString, QPair<QPoint, QCursor>> cursorHash;
};

#endif
