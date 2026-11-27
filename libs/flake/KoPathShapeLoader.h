/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KOPATHSHAPELOADER_H_
#define _KOPATHSHAPELOADER_H_

#include "kritaflake_export.h"

class KoPathShape;
class KoPathShapeLoaderPrivate;
class QString;

class KRITAFLAKE_EXPORT KoPathShapeLoader
{
public:
    explicit KoPathShapeLoader(KoPathShape *path);
    ~KoPathShapeLoader();

    void parseSvg(const QString &svgInputData, bool process = false);

private:
    KoPathShapeLoaderPrivate* const d;
};

#endif
