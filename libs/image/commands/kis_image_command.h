/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMAGE_COMMAND_H_
#define KIS_IMAGE_COMMAND_H_

#include <kritaimage_export.h>

#include <kundo2command.h>

#include <QSize>
#include <QRect>
#include "kis_types.h"
#include "kis_paint_device.h"

class KRITAIMAGE_EXPORT KisImageCommand : public KUndo2Command
{

public:
    KisImageCommand(const KUndo2MagicString& name, KisImageWSP image, KUndo2Command *parent = 0);
    ~KisImageCommand() override;

public:

    class UpdateTarget
    {
    public:
        UpdateTarget(KisImageWSP image, KisNodeSP removedNode, const QRect &updateRect);
        void update();

    private:
        KisImageWSP m_image;
        QRect m_updateRect;
        int m_removedNodeIndex;
        KisNodeSP m_removedNodeParent;
    };

protected:
    KisImageWSP m_image;
};

#endif
