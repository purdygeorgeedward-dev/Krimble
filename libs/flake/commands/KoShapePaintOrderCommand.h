/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2023 Wolthera van Hövell <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEPAINTORDERCOMMAND_H
#define KOSHAPEPAINTORDERCOMMAND_H

#include "kritaflake_export.h"

#include <kundo2command.h>
#include <KoFlake.h>
#include <KoShape.h>

class KRITAFLAKE_EXPORT KoShapePaintOrderCommand: public KUndo2Command
{
public:
    KoShapePaintOrderCommand(const QList<KoShape *> &shapes, KoShape::PaintOrder first, KoShape::PaintOrder second, KUndo2Command *parent = nullptr);

    ~KoShapePaintOrderCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
