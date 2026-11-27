//  Copyright (C) 2007 Kenneth P. Esler, Jr.                               //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  GNU General Public License for more details.                           //
//  You should have received a copy of the GNU General Public License      //

#ifndef MULTI_BSPLINE_EVAL_Z_H
#define MULTI_BSPLINE_EVAL_Z_H

void
eval_multi_UBspline_1d_z (multi_UBspline_1d_z *spline,
			  double x,
			  complex_double* restrict vals);

void
eval_multi_UBspline_1d_z_vg (multi_UBspline_1d_z *spline,
			     double x,
			     complex_double* restrict vals,
			     complex_double* restrict grads);

void
eval_multi_UBspline_1d_z_vgl (multi_UBspline_1d_z *spline,
			      double x,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict lapl);

void
eval_multi_UBspline_1d_z_vgh (multi_UBspline_1d_z *spline,
			      double x,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict hess);

void
eval_multi_UBspline_2d_z (multi_UBspline_2d_z *spline,
			  double x, double y,
			  complex_double* restrict vals);

void
eval_multi_UBspline_2d_z_vg (multi_UBspline_2d_z *spline,
			     double x, double y,
			     complex_double* restrict vals,
			     complex_double* restrict grads);

void
eval_multi_UBspline_2d_z_vgl (multi_UBspline_2d_z *spline,
			      double x, double y,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict lapl);

void
eval_multi_UBspline_2d_z_vgh (multi_UBspline_2d_z *spline,
			      double x, double y,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict hess);

void
eval_multi_UBspline_3d_z (multi_UBspline_3d_z *spline,
			  double x, double y, double z,
			  complex_double* restrict vals);

void
eval_multi_UBspline_3d_z_vg (multi_UBspline_3d_z *spline,
			     double x, double y, double z,
			     complex_double* restrict vals,
			     complex_double* restrict grads);

void
eval_multi_UBspline_3d_z_vgl (multi_UBspline_3d_z *spline,
			      double x, double y, double z,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict lapl);

void
eval_multi_UBspline_3d_z_vgh (multi_UBspline_3d_z *spline,
			      double x, double y, double z,
			      complex_double* restrict vals,
			      complex_double* restrict grads,
			      complex_double* restrict hess);

#endif
