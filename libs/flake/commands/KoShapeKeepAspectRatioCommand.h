/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEKEEPASPECTRATIOCOMMAND_H
#define KOSHAPEKEEPASPECTRATIOCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QList>

class KoShape;

class KRITAFLAKE_EXPORT KoShapeKeepAspectRatioCommand : public KUndo2Command
{
public:
    KoShapeKeepAspectRatioCommand(const QList<KoShape*> &shapes, bool newKeepAspectRatio, KUndo2Command* parent = 0);
    ~KoShapeKeepAspectRatioCommand() override;

    void redo() override;
    void undo() override;

private:
    QList<KoShape*> m_shapes;
    QList<bool> m_oldKeepAspectRatio;
    QList<bool> m_newKeepAspectRatio;
};

#endif
