/*
 * SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SVGTEXTREMOVECOMMAND_H
#define SVGTEXTREMOVECOMMAND_H

#include <kundo2command.h>
#include "kritatoolsvgtext_export.h"
#include <KoSvgTextShape.h>

class KoSvgTextShape;

class KRITATOOLSVGTEXT_EXPORT SvgTextRemoveCommand : public KUndo2Command
{
public:
    SvgTextRemoveCommand(KoSvgTextShape *shape,
                         int endIndex,
                         int pos,
                         int anchor,
                         int length,
                         bool allowCleanUp = true,
                         KUndo2Command *parent = 0);
    ~SvgTextRemoveCommand() override = default;

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *other) override;

private:
    KoSvgTextShape *m_shape;
    int m_index;
    int m_originalPos;
    int m_anchor;
    int m_length;
    bool m_allowCleanUp;
    KoSvgTextShapeMementoSP m_textData;
};

#endif
