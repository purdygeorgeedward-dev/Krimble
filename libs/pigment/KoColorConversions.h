/*
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_COLORCONVERSIONS_H_
#define _KO_COLORCONVERSIONS_H_

#include <QtGlobal>
#include "kritapigment_export.h"

KRITAPIGMENT_EXPORT void rgb_to_hsv(int R, int G, int B, int *H, int *S, int *V);
KRITAPIGMENT_EXPORT void hsv_to_rgb(int H, int S, int V, int *R, int *G, int *B);

KRITAPIGMENT_EXPORT void RGBToHSV(float r, float g, float b, float *h, float *s, float *v);
KRITAPIGMENT_EXPORT void HSVToRGB(float h, float s, float v, float *r, float *g, float *b);

KRITAPIGMENT_EXPORT void RGBToHSL(float r, float g, float b, float *h, float *s, float *l);
KRITAPIGMENT_EXPORT void HSLToRGB(float h, float sl, float l, float *r, float *g, float *b);

KRITAPIGMENT_EXPORT void rgb_to_hls(quint8 r, quint8 g, quint8 b, float * h, float * l, float * s);

KRITAPIGMENT_EXPORT float hue_value(float n1, float n2, float hue);

KRITAPIGMENT_EXPORT void hls_to_rgb(float h, float l, float s, quint8 * r, quint8 * g, quint8 * b);

KRITAPIGMENT_EXPORT void rgb_to_hls(quint8 r, quint8 g, quint8 b, int * h, int * l, int * s);
KRITAPIGMENT_EXPORT void hls_to_rgb(int h, int l, int s, quint8 * r, quint8 * g, quint8 * b);

KRITAPIGMENT_EXPORT void HSIToRGB(const qreal h, const qreal s, const qreal i, qreal *red, qreal *green, qreal *blue);
KRITAPIGMENT_EXPORT void RGBToHSI(qreal r, qreal g, qreal b, qreal *h, qreal *s, qreal *i);

KRITAPIGMENT_EXPORT void RGBToHSY( qreal r, qreal g, qreal b, qreal *h, qreal *s, qreal *y, qreal R=0.299, qreal G=0.587, qreal B=0.114);
KRITAPIGMENT_EXPORT void HSYToRGB(const qreal h, const qreal s, const qreal y, qreal *red, qreal *green, qreal *blue, qreal R=0.299, qreal G=0.587, qreal B=0.114);

KRITAPIGMENT_EXPORT void HCIToRGB(const qreal h, const qreal s, const qreal i, qreal *red, qreal *green, qreal *blue);
KRITAPIGMENT_EXPORT void RGBToHCI(const qreal r, const qreal g, const qreal b, qreal *h, qreal *c, qreal *i);

KRITAPIGMENT_EXPORT void HCYToRGB(const qreal h, const qreal s, const qreal y, qreal *red, qreal *green, qreal *blue,  qreal R=0.299, qreal G=0.587, qreal B=0.114);
KRITAPIGMENT_EXPORT void RGBToHCY(const qreal r, const qreal g, const qreal b, qreal *h, qreal *c, qreal *y,  qreal R=0.299, qreal G=0.587, qreal B=0.114);

KRITAPIGMENT_EXPORT void RGBToYUV( qreal r, qreal g, qreal b, qreal *y, qreal *cb, qreal *cr, qreal R=0.299, qreal G=0.587, qreal B=0.114);
KRITAPIGMENT_EXPORT void YUVToRGB(const qreal y, const qreal cb, const qreal cr, qreal *r, qreal *g, qreal *b, qreal R=0.299, qreal G=0.587, qreal B=0.114);

KRITAPIGMENT_EXPORT void LabToLCH(const qreal l, const qreal a, const qreal b, qreal *L, qreal *C, qreal *H);
KRITAPIGMENT_EXPORT void LCHToLab(const qreal L, const qreal C, const qreal H, qreal *l, qreal *a, qreal *b);

KRITAPIGMENT_EXPORT void XYZToxyY(const qreal X, const qreal Y, const qreal Z, qreal *x, qreal *y, qreal *yY);
KRITAPIGMENT_EXPORT void xyYToXYZ(const qreal x, const qreal y, const qreal yY, qreal *X, qreal *Y, qreal *Z);

KRITAPIGMENT_EXPORT void CMYToCMYK(qreal *c, qreal *m, qreal *y, qreal *k);
KRITAPIGMENT_EXPORT void CMYKToCMY(qreal *c, qreal *m, qreal *y, qreal *k);

#endif

