//  Copyright (C) 2007 Kenneth P. Esler, Jr.                               //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  GNU General Public License for more details.                           //
//  You should have received a copy of the GNU General Public License      //

#ifndef BSPLINE_STRUCTS_STD_H
#define BSPLINE_STRUCTS_STD_H

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  float* restrict coefs;
  Ugrid x_grid;
  BCtype_s xBC;
} UBspline_1d_s;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  float* restrict coefs;
  int x_stride;
  Ugrid x_grid, y_grid;
  BCtype_s xBC, yBC;
} UBspline_2d_s;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  float* restrict coefs;
  int x_stride, y_stride;
  Ugrid x_grid, y_grid, z_grid;
  BCtype_s xBC, yBC, zBC;
} UBspline_3d_s;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  double* restrict coefs;
  Ugrid x_grid;
  BCtype_d xBC;
} UBspline_1d_d;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  double* restrict coefs;
  int x_stride;
  Ugrid x_grid, y_grid;
  BCtype_d xBC, yBC;
} UBspline_2d_d;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  double* restrict coefs;
  int x_stride, y_stride;
  Ugrid x_grid, y_grid, z_grid;
  BCtype_d xBC, yBC, zBC;
} UBspline_3d_d;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_float* restrict coefs;
  Ugrid x_grid;
  BCtype_c xBC;
} UBspline_1d_c;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_float* restrict coefs;
  int x_stride;
  Ugrid x_grid, y_grid;
  BCtype_c xBC, yBC;
} UBspline_2d_c;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_float* restrict coefs;
  int x_stride, y_stride;
  Ugrid x_grid, y_grid, z_grid;
  BCtype_c xBC, yBC, zBC;

} UBspline_3d_c;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_double* restrict coefs;
  Ugrid x_grid;
  BCtype_z xBC;
} UBspline_1d_z;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_double* restrict coefs;
  int x_stride;
  Ugrid x_grid, y_grid;
  BCtype_z xBC, yBC;
} UBspline_2d_z;

typedef struct
{
  spline_code spcode;
  type_code    tcode;
  complex_double* restrict coefs;
  int x_stride, y_stride;
  Ugrid x_grid, y_grid, z_grid;
  BCtype_z xBC, yBC, zBC;
} UBspline_3d_z;

#endif
