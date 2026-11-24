/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPETRANSFORMCOMMAND_H
#define KOSHAPETRANSFORMCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>

class KoShape;
class QTransform;

class KRITAFLAKE_EXPORT KoShapeTransformCommand : public KUndo2Command
{
public:

    KoShapeTransformCommand(const QList<KoShape*> &shapes, const QList<QTransform> &oldState, const QList<QTransform> &newState, KUndo2Command * parent = 0);
    ~KoShapeTransformCommand() override;
    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    class Private;
    Private * const d;
};

#endif
