/*
 *  SPDX-FileCopyrightText: 2026 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef LCMSPREDFINEDPIPELINEFUNCTIONS_H
#define LCMSPREDFINEDPIPELINEFUNCTIONS_H

#include <lcms2.h>
#include <KoColorProfileConstants.h>

class LcmsPredefinedPipelineFunctions
{
public:

    static bool setPerceptualQuantizerAToBDummyPipeline(cmsHPROFILE iccProfile, cmsTagSignature tag, ColorPrimaries primaries, double diffuseWhiteNits = 80.0);
    static bool setPerceptualQuantizerBToADummyPipeline(cmsHPROFILE iccProfile, cmsTagSignature tag, ColorPrimaries primaries, double diffuseWhiteNits = 80.0);
    static bool setDiffuseWhitePerceptualQuantizer(cmsHPROFILE iccProfile, double diffuseWhiteNits = 80.0);

};

#endif
