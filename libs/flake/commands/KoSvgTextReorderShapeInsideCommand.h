/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSVGTEXTREORDERSHAPEINSIDECOMMAND_H
#define KOSVGTEXTREORDERSHAPEINSIDECOMMAND_H

#include <kundo2command.h>
#include <kritaflake_export.h>

class KoSvgTextShape;
class KoShape;

class KRITAFLAKE_EXPORT KoSvgTextReorderShapeInsideCommand: public KUndo2Command
{
public:
    enum MoveShapeType  {
        MoveEarlier,
        MoveLater,
        BringToFront,
        SendToBack
    };

    KoSvgTextReorderShapeInsideCommand(KoSvgTextShape* textShape, QList<KoShape*> shape, MoveShapeType type, KUndo2Command *parent = nullptr);
    ~KoSvgTextReorderShapeInsideCommand();

    void redo() override;
    void undo() override;
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
