/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODRAG_H
#define KODRAG_H

#include "kritaflake_export.h"

#include <QList>

class QMimeData;
class QString;
class QByteArray;
class KoDragPrivate;
class KoShape;

class KRITAFLAKE_EXPORT KoDrag
{
public:
    KoDrag();
    ~KoDrag();

    bool setSvg(const QList<KoShape*> shapes);

    void setData(const QString &mimeType, const QByteArray &data);

    void addToClipboard();

    QMimeData *mimeData();

private:
    KoDragPrivate * const d;
};

#endif
