/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOZOOMTOOLFACTORY_H
#define KOZOOMTOOLFACTORY_H

#include "KoToolFactoryBase.h"

class KoZoomToolFactory : public KoToolFactoryBase
{
public:
    KoZoomToolFactory();

    KoToolBase *createTool(KoCanvasBase *canvas) override;
};
#endif
