/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEALIGNCOMMAND_H
#define KOSHAPEALIGNCOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <QList>

class KoShape;
class QRectF;

class KRITAFLAKE_EXPORT KoShapeAlignCommand : public KUndo2Command
{
public:
    enum Align {
        HorizontalLeftAlignment,
        HorizontalCenterAlignment,
        HorizontalRightAlignment,
        VerticalBottomAlignment,
        VerticalCenterAlignment,
        VerticalTopAlignment
    };
    KoShapeAlignCommand(const QList<KoShape*> &shapes, Align align, const QRectF &boundingRect, KUndo2Command *parent = 0);
    ~KoShapeAlignCommand() override;
    void redo() override;
    void undo() override;
private:
    class Private;
    Private * const d;
};

#endif
