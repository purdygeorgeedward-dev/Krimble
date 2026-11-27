/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTOOLSELECTION_H
#define KOTOOLSELECTION_H

#include "kritaflake_export.h"
#include <QObject>

class KRITAFLAKE_EXPORT KoToolSelection : public QObject
{
    Q_OBJECT
public:
    explicit KoToolSelection(QObject *parent = 0);
    ~KoToolSelection() override;

    virtual bool hasSelection() {
        return false;
    }
};

#endif
