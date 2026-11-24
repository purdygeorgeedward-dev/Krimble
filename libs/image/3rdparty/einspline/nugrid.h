//  Copyright (C) 2007 Kenneth P. Esler, Jr.                               //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  GNU General Public License for more details.                           //
//  You should have received a copy of the GNU General Public License      //

#ifndef NUGRID_H
#define NUGRID_H

#include "local_definitions.h"

typedef enum { LINEAR, GENERAL, CENTER, LOG } grid_type;

typedef struct
{
  grid_type code;
  double start, end;
  double* restrict points;
  int num_points;
  int (*reverse_map)(void *grid, double x);
} NUgrid;

#ifdef __cplusplus
extern "C"
#endif

typedef struct
{
  grid_type code;
  double start, end;
  double* restrict points;
  int num_points;
  int (*reverse_map)(void *grid, double x);

  double a, aInv, b, bInv, center, even_half;
  int half_points, odd_one;
  bool odd;
} center_grid;

typedef struct
{
  grid_type code;
  double start, end;
  double* restrict points;
  int num_points;
  int (*reverse_map)(void *grid, double x);

  double a, ainv, startinv;
} log_grid;

#ifdef __cplusplus
extern "C" {
#endif

NUgrid*
create_center_grid (double start, double end, double ratio,
		    int num_points);

NUgrid*
create_log_grid (double start, double end, int num_points);

NUgrid*
create_general_grid (double *points, int num_points);

void
destroy_grid (NUgrid *grid);

#ifdef __cplusplus
}
#endif
#endif
