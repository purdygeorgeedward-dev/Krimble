/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGCONVERTTEXTTYPECOMMAND_H
#define SVGCONVERTTEXTTYPECOMMAND_H

#include <kundo2command.h>
#include "kritaflake_export.h"
#include <KoSvgTextShape.h>

class KRITAFLAKE_EXPORT KoSvgConvertTextTypeCommand : public KUndo2Command
{
public:

    KoSvgConvertTextTypeCommand(KoSvgTextShape *shape, KoSvgTextShape::TextType type, int pos, KUndo2Command *parent = 0);
    ~KoSvgConvertTextTypeCommand() override = default;

    void redo() override;

    void undo() override;

private:
    KoSvgTextShape *m_shape;
    KoSvgTextShapeMementoSP m_textData;

    KoSvgTextShape::TextType m_conversionType;
    int m_pos;
};

#endif
