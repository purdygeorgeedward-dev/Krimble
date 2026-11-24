/* License:
 *
 * Code for making well-behaved ICC profiles
 * SPDX-FileCopyrightText: 2013, 2014, 2015 Elle Stone
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Contact information:
 * ellestone@ninedegreesbelow.com
 * https://ninedegreesbelow.com
 *
 * */

#include <lcms2.h>
int main ()
{
printf("D50X, D50Y, D50Z = %1.8f %1.8f %1.8f\n", cmsD50X, cmsD50Y, cmsD50Z);

cmsFloat64Number srgb_parameters[5] =
   { 2.4, 1.0 / 1.055,  0.055 / 1.055, 1.0 / 12.92, 0.04045 };
cmsToneCurve *srgb_parametic_curve =
   cmsBuildParametricToneCurve(NULL, 4, srgb_parameters);
cmsToneCurve *srgb_parametric[3] =
   {srgb_parametic_curve,srgb_parametic_curve,srgb_parametic_curve};

cmsFloat64Number labl_parameters[5] =
   { 3.0, 0.862076,  0.137924, 0.110703, 0.080002 };
cmsToneCurve *labl_parametic_curve =
   cmsBuildParametricToneCurve(NULL, 4, labl_parameters);
cmsToneCurve *labl_parametric[3] =
   {labl_parametic_curve,labl_parametic_curve,labl_parametic_curve};

cmsFloat64Number rec709_parameters[5] =
   { 1.0 / 0.45, 1.0 / 1.099,  0.099 / 1.099,  1.0 / 4.5, 0.018 };
cmsToneCurve *rec709_parametic_curve =
   cmsBuildParametricToneCurve(NULL, 4, rec709_parameters);
cmsToneCurve *rec709_parametric[3] =
   {rec709_parametic_curve,rec709_parametic_curve,rec709_parametic_curve};

cmsToneCurve* gamma100[3];
gamma100[0] = gamma100[1] = gamma100[2] = cmsBuildGamma (NULL, 1.00);

cmsToneCurve* gamma200[3];
gamma200[0] = gamma200[1] = gamma200[2] = cmsBuildGamma (NULL, 2.00);

cmsToneCurve* gamma180[3];
gamma180[0] = gamma180[1] = gamma180[2] = cmsBuildGamma (NULL, 1.80078125);

cmsToneCurve* gamma220[3];
gamma220[0] = gamma220[1] = gamma220[2] = cmsBuildGamma (NULL, 2.19921875);

cmsCIExyY d50_romm_spec= {0.3457, 0.3585, 1.0};

cmsCIExyY d50_illuminant_specs = {0.345702915, 0.358538597, 1.0};

cmsCIExyY  d65_srgb_adobe_specs = {0.3127, 0.3290, 1.0};

cmsCIExyY c_astm  = {0.310060511, 0.316149551, 1.0};
cmsCIExyY e_astm  = {0.333333333, 0.333333333, 1.0};

cmsCIExyY c_cie= {0.310, 0.316};
cmsCIExyY e_cie= {0.333, 0.333};

cmsCIExyY c_6774_robertson= {0.308548930, 0.324928102, 1.0};
cmsCIExyY e_5454_robertson= {0.333608970, 0.348572909, 1.0};

cmsCIExyY d60_aces= {0.32168, 0.33767, 1.0};

cmsHPROFILE profile;
cmsToneCurve* tone_curve[3];
cmsCIExyY whitepoint;
cmsCIExyYTRIPLE primaries;
const char* filename;
cmsMLU *copyright = cmsMLUalloc(NULL, 1);

cmsMLUsetASCII(copyright, "en", "US", "Copyright 2015, Elle Stone (website: http://ninedegreesbelow.com/; email: ellestone@ninedegreesbelow.com). This ICC profile is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License (https://creativecommons.org/licenses/by-sa/3.0/legalcode).");

cmsMLU *manufacturer = cmsMLUalloc(NULL, 1);

cmsMLU *description;

cmsCIExyYTRIPLE aces_cg_primaries =
{
{0.713, 0.293,  1.0},
{0.165, 0.830,  1.0},
{0.128, 0.044,  1.0}
};

whitepoint = d60_aces;
primaries = aces_cg_primaries;

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACEScg-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACEScg-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE aces_primaries =
{
{0.73470,  0.26530,  1.0},
{0.00000,  1.00000,  1.0},
{0.00010, -0.07700,  1.0}
};
cmsCIExyYTRIPLE aces_primaries_prequantized =
{
{0.734704192222, 0.265298276252,  1.0},
{-0.000004945077, 0.999992850272,  1.0},
{0.000099889199, -0.077007518685,  1.0}
};

whitepoint = d60_aces;
primaries = aces_primaries_prequantized;

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V4-g122.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ACES-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ACES-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE allcolors_primaries =
{
{0.734690265,  0.265309735,  1.0},
{0.000000000,  1.000000000,  1.0},
{0.000000000, -.0758542197,  1.0}
};
whitepoint = d50_illuminant_specs;
primaries = allcolors_primaries;

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "AllColorsRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "AllColorsRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE identity_primaries = {
{1.0, 0.0, 1.0},
{0.0, 1.0, 1.0},
{0.0, 0.0, 1.0}
};
whitepoint = d50_illuminant_specs;
primaries = identity_primaries;

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "IdentityRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "IdentityRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE romm_primaries = {
{0.7347, 0.2653, 1.0},
{0.1596, 0.8404, 1.0},
{0.0366, 0.0001, 1.0}
};
primaries = romm_primaries;
whitepoint = d50_romm_spec;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma180[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4-g18.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V4-g18.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V2-g18.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V2-g18.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "LargeRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "LargeRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE widegamut_pascale_primaries = {
{0.7347, 0.2653, 1.0},
{0.1152, 0.8264, 1.0},
{0.1566, 0.0177, 1.0}
};
primaries = widegamut_pascale_primaries;
whitepoint = d50_romm_spec;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "WideRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "WideRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE adobe_primaries = {
{0.6400, 0.3300, 1.0},
{0.2100, 0.7100, 1.0},
{0.1500, 0.0600, 1.0}
};
cmsCIExyYTRIPLE adobe_primaries_prequantized = {
{0.639996511, 0.329996864, 1.0},
{0.210005295, 0.710004866, 1.0},
{0.149997606, 0.060003644, 1.0}
};
primaries = adobe_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "ClayRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "ClayRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE rec2020_primaries = {
{0.7079, 0.2920, 1.0},
{0.1702, 0.7965, 1.0},
{0.1314, 0.0459, 1.0}
};

cmsCIExyYTRIPLE rec2020_primaries_prequantized = {
{0.708012540607, 0.291993664388, 1.0},
{0.169991652439, 0.797007778423, 1.0},
{0.130997824007, 0.045996550894, 1.0}
};

primaries = rec2020_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = rec709_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V4-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V4-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V2-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V2-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Rec2020-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Rec2020-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE srgb_primaries = {
{0.6400, 0.3300, 1.0},
{0.3000, 0.6000, 1.0},
{0.1500, 0.0600, 1.0}
};
cmsCIExyYTRIPLE srgb_primaries_pre_quantized = {
{0.639998686, 0.330010138, 1.0},
{0.300003784, 0.600003357, 1.0},
{0.150002046, 0.059997204, 1.0}
};
primaries = srgb_primaries_pre_quantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = rec709_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V4-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V4-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "sRGB-elle-V2-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "sRGB-elle-V2-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE cie_primaries_ledtuning = {
{0.7346900230, 0.2653099770, 1.0},
{0.2736747378, 0.7174284409, 1.0},
{0.1665361196, 0.0088826412, 1.0}
};
cmsCIExyYTRIPLE cie_primaries_ledtuning_prequantized = {
{0.734689082, 0.265296653, 1.0},
{0.273673341, 0.717437354, 1.0},
{0.166531028, 0.008882428, 1.0}
};
primaries = cie_primaries_ledtuning_prequantized;
whitepoint = e_astm;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma100[0];
whitepoint = e_astm;
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = srgb_parametric[0];
whitepoint = e_astm;
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

tone_curve[0] = tone_curve[1] = tone_curve[2] = labl_parametric[0];
profile = cmsCreateRGBProfile ( &whitepoint, &primaries, tone_curve );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "CIERGB-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "CIERGB-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE cie_primaries_lindbloom = {
{0.7350, 0.2650, 1.0},
{0.2740, 0.7170, 1.0},
{0.1670, 0.0090, 1.0}
};
cmsCIExyYTRIPLE cie_primaries_lindbloom_prequantized = {
{0.714504840, 0.297234644, 1.0},
{0.520085568, 0.452364535, 1.0},
{0.090957433, 0.051485032, 1.0}
};

whitepoint = d50_illuminant_specs;
const cmsToneCurve* grayTRC;

grayTRC = cmsBuildGamma (NULL, 1.00);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-g10.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-g10.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

grayTRC = cmsBuildGamma (NULL, 1.80078125);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-g18.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-g18.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-g18.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-g18.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

grayTRC = cmsBuildGamma (NULL, 2.19921875);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-g22.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-g22.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

grayTRC = cmsBuildParametricToneCurve(NULL, 4, srgb_parameters);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-srgbtrc.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-srgbtrc.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

grayTRC = cmsBuildParametricToneCurve(NULL, 4, labl_parameters);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-labl.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-labl.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

grayTRC = cmsBuildParametricToneCurve(NULL, 4, rec709_parameters);
profile = cmsCreateGrayProfile ( &whitepoint, grayTRC );
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V4-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V4-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);
cmsSetProfileVersion(profile, 2.1);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Gray-D50-elle-V2-rec709.icc");
cmsWriteTag(profile, cmsSigProfileDescriptionTag, description);
                             filename = "Gray-D50-elle-V2-rec709.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

profile  = cmsCreateLab2Profile(&d50_illuminant_specs);
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Lab-D50-Identity-elle-V2.icc");
                             filename = "Lab-D50-Identity-elle-V2.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

profile  = cmsCreateLab4Profile(&d50_illuminant_specs);
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "Lab-D50-Identity-elle-V4.icc");
                             filename = "Lab-D50-Identity-elle-V4.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

profile = cmsCreateXYZProfile();
cmsWriteTag(profile, cmsSigCopyrightTag, copyright);
description = cmsMLUalloc(NULL, 1);
cmsMLUsetASCII(description, "en", "US", "XYZ-D50-Identity-elle-V4.icc");
                             filename = "XYZ-D50-Identity-elle-V4.icc";
cmsSaveProfileToFile(profile, filename);
cmsMLUfree(description);

cmsCIExyYTRIPLE colormatch_primaries = {
{0.6300, 0.3400, 1.0},
{0.2950, 0.6050, 1.0},
{0.1500, 0.0750, 1.0}
};
cmsCIExyYTRIPLE colormatch_primaries_prequantized = {
{0.629992636, 0.339999723, 1.0},
{0.295006332, 0.604997745, 1.0},
{0.149992036, 0.075005244, 1.0}
};
primaries = colormatch_primaries_prequantized;
whitepoint = d50_romm_spec;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma180[0];

cmsCIExyYTRIPLE apple_primaries = {
{0.6250, 0.3400, 1.0},
{0.2800, 0.5950, 1.0},
{0.1550, 0.0700, 1.0}
};
cmsCIExyYTRIPLE apple_primaries_prequantized = {
{0.625012368, 0.340000081, 1.0},
{0.279996113, 0.595006943, 1.0},
{0.155001212, 0.070001183, 1.0}
};
primaries = apple_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma180[0];

cmsCIExyYTRIPLE pal_primaries = {
{0.6400, 0.3300, 1.0},
{0.2900, 0.6000, 1.0},
{0.1500, 0.0600, 1.0}
};
cmsCIExyYTRIPLE pal_primaries_prequantized = {
{0.640007798, 0.330006592, 1.0},
{0.290000327, 0.600000840, 1.0},
{0.149998025, 0.059996098, 1.0}
};
primaries = pal_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];

cmsCIExyYTRIPLE smpte_c_primaries = {
{0.6300, 0.3400, 1.0},
{0.3100, 0.5950, 1.0},
{0.1550, 0.0700, 1.0}
};
cmsCIExyYTRIPLE smpte_c_primaries_prequantized = {
{0.629996495, 0.339990597, 1.0},
{0.309997880, 0.594995808, 1.0},
{0.149999952, 0.069999431, 1.0}
};
primaries = smpte_c_primaries_prequantized;
whitepoint = d65_srgb_adobe_specs;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];

cmsCIExyYTRIPLE ntcs_primaries = {
{0.6700, 0.3300, 1.0},
{0.2100, 0.7100, 1.0},
{0.1400, 0.0800, 1.0}
};
cmsCIExyYTRIPLE ntcs_primaries_prequantized = {
{0.670010373, 0.330001186, 1.0},
{0.209999261, 0.710001124, 1.0},
{0.139996061, 0.080002934, 1.0}
};
primaries = ntcs_primaries_prequantized;
whitepoint = c_astm;
tone_curve[0] = tone_curve[1] = tone_curve[2] = gamma220[0];

cmsMLUfree(copyright);

return 0;
}
