/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jeremy Lugagne <lugagne.jeremy@gmail.com>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KoPathShapeMarkerCommand_H
#define KoPathShapeMarkerCommand_H

#include "kritaflake_export.h"

#include <QScopedPointer>

#include "KoFlake.h"
#include <kundo2command.h>
#include <QList>

class KoPathShape;
class KoMarker;

class KRITAFLAKE_EXPORT KoPathShapeMarkerCommand : public KUndo2Command
{
public:
    KoPathShapeMarkerCommand(const QList<KoPathShape*> &shapes, KoMarker *marker, KoFlake::MarkerPosition position, KUndo2Command *parent = 0);

    ~KoPathShapeMarkerCommand() override;

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
