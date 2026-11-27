/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEUNGROUPCOMMAND_H
#define KOSHAPEUNGROUPCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QScopedPointer>

class KoShape;
class KoShapeGroup;
class KoShapeContainer;

class KRITAFLAKE_EXPORT KoShapeUngroupCommand : public KUndo2Command
{
public:
    KoShapeUngroupCommand(KoShapeContainer *container, const QList<KoShape *> &shapes,
                          const QList<KoShape *> &topLevelShapes = QList<KoShape*>(), KUndo2Command *parent = 0);
    ~KoShapeUngroupCommand();

    void redo() override;
    void undo() override;

private:
    struct Private;
    const QScopedPointer<Private> m_d;

};

#endif
