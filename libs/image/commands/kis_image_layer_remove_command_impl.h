/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMAGE_LAYER_REMOVE_COMMAND_IMPL_H_
#define KIS_IMAGE_LAYER_REMOVE_COMMAND_IMPL_H_

#include <kritaimage_export.h>

#include "kis_types.h"
#include "kis_image_command.h"

class KRITAIMAGE_EXPORT KisImageLayerRemoveCommandImpl : public KisImageCommand
{
public:
    KisImageLayerRemoveCommandImpl(KisImageWSP image, KisNodeSP node, KUndo2Command *parent = 0);
    ~KisImageLayerRemoveCommandImpl() override;

    void redo() override;
    void undo() override;

private:
    struct Private;
    Private * const m_d;
};
#endif
