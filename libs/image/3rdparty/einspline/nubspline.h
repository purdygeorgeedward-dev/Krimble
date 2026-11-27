//  Copyright (C) 2007 Kenneth P. Esler, Jr.                               //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  GNU General Public License for more details.                           //
//  You should have received a copy of the GNU General Public License      //

#ifndef NUBSPLINE_H
#define NUBSPLINE_H

#include "nubspline_create.h"

#ifdef HAVE_SSE2
  #include "nubspline_structs.h"
  #include "nubspline_eval_sse_s.h"
  #include "nubspline_eval_sse_c.h"
  #include "nubspline_eval_sse_d.h"
  #include "nubspline_eval_sse_z.h"
#else
  #include "nubspline_structs.h"
  #include "nubspline_eval_std_s.h"
  #include "nubspline_eval_std_c.h"
  #include "nubspline_eval_std_d.h"
  #include "nubspline_eval_std_z.h"
#endif

#endif
