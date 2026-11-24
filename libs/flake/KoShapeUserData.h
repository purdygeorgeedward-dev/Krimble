/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEUSERDATA_H
#define KOSHAPEUSERDATA_H

#include <QObject>

#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoShapeUserData : public QObject
{
    Q_OBJECT
public:
    explicit KoShapeUserData(QObject *parent = 0);
    ~KoShapeUserData() override;

    virtual KoShapeUserData* clone() const = 0;

protected:
    KoShapeUserData(const KoShapeUserData &rhs);
};

#endif
