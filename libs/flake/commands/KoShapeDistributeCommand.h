/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEDISTRIBUTECOMMAND_H
#define KOSHAPEDISTRIBUTECOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoShape;

class KRITAFLAKE_EXPORT KoShapeDistributeCommand : public KUndo2Command
{
public:
    enum Distribute {
        HorizontalCenterDistribution,
        HorizontalGapsDistribution,
        HorizontalLeftDistribution,
        HorizontalRightDistribution,
        VerticalCenterDistribution,
        VerticalGapsDistribution,
        VerticalBottomDistribution,
        VerticalTopDistribution
    };
    KoShapeDistributeCommand(const QList<KoShape*> &shapes, Distribute distribute,
            const QRectF &boundingRect, KUndo2Command *parent = 0);
    ~KoShapeDistributeCommand() override;
    void redo() override;
    void undo() override;
private:

    class Private;
    Private * const d;
};

#endif
