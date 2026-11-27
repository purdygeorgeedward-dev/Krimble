/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESTROKECOMMAND_H
#define KOSHAPESTROKECOMMAND_H

#include "kritaflake_export.h"

#include <KoFlakeTypes.h>
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeStrokeModel;

class KRITAFLAKE_EXPORT KoShapeStrokeCommand : public KUndo2Command
{
public:
    KoShapeStrokeCommand(const QList<KoShape*> &shapes, KoShapeStrokeModelSP stroke, KUndo2Command *parent = 0);

    KoShapeStrokeCommand(const QList<KoShape*> &shapes, const QList<KoShapeStrokeModelSP> &strokes, KUndo2Command *parent = 0);

    KoShapeStrokeCommand(KoShape* shape, KoShapeStrokeModelSP stroke, KUndo2Command *parent = 0);

    ~KoShapeStrokeCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
