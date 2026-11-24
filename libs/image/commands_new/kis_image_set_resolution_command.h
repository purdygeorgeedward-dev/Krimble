/*
 *  SPDX-FileCopyrightText: 2010 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMAGE_SET_RESOLUTION_COMMAND_H
#define KIS_IMAGE_SET_RESOLUTION_COMMAND_H

#include "kritaimage_export.h"
#include "kis_types.h"

#include <kundo2command.h>

class KRITAIMAGE_EXPORT KisImageSetResolutionCommand : public KUndo2Command
{
public:
    KisImageSetResolutionCommand(KisImageWSP image, qreal newXRes, qreal newYRes, KUndo2Command *parent = 0);
    void undo() override;
    void redo() override;

private:
    KisImageWSP m_image;

    qreal m_newXRes;
    qreal m_newYRes;
    qreal m_oldXRes;
    qreal m_oldYRes;
};

class KRITAIMAGE_EXPORT KisResetShapesCommand : public KUndo2Command
{
public:
    KisResetShapesCommand(KisNodeSP rootNode);

    void undo() override;
    void redo() override;

private:
    void resetNode(KisNodeSP node);

private:
    KisNodeSP m_rootNode;
};

#endif
