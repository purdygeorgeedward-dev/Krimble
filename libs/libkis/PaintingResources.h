/*
 *  SPDX-FileCopyrightText: 2020 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef LIBKIS_PAINTINGRESOURCES_H
#define LIBKIS_PAINTINGRESOURCES_H

#include "kis_figure_painting_tool_helper.h"

namespace PaintingResources
{
    const QString defaultStrokeStyle = "ForegroundColor";
    const QString defaultFillStyle = "None";

    KisFigurePaintingToolHelper createHelper(KisImageWSP image,
                                             KisNodeSP node,
                                             const QString strokeStyle = defaultStrokeStyle,
                                             const QString fillStyle = defaultFillStyle);

};

#endif
