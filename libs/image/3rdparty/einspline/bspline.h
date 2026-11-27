//  Copyright (C) 2007 Kenneth P. Esler, Jr.                               //
//  it under the terms of the GNU General Public License as published by   //
//  the Free Software Foundation; either version 2 of the License, or      //
//  GNU General Public License for more details.                           //
//  You should have received a copy of the GNU General Public License      //

#ifndef BSPLINE_H
#define BSPLINE_H

#include "bspline_base.h"
#include "bspline_structs.h"
#include "multi_bspline_structs.h"

#ifdef HAVE_SSE2
#include "bspline_eval_sse_s.h"
#include "bspline_eval_sse_c.h"
#include "bspline_eval_sse_d.h"
#include "bspline_eval_sse_z.h"
#elif defined HAVE_SSE
#include "bspline_eval_sse_s.h"
#include "bspline_eval_sse_c.h"
#include "bspline_eval_std_d.h"
#include "bspline_eval_std_z.h"
#elif defined USE_ALTIVEC
#include "bspline_eval_altivec_s.h"
#include "bspline_eval_std_c.h"
#include "bspline_eval_std_d.h"
#include "bspline_eval_std_z.h"
#else
#include "bspline_eval_std_s.h"
#include "bspline_eval_std_c.h"
#include "bspline_eval_std_d.h"
#include "bspline_eval_std_z.h"
#endif

#include "bspline_create.h"
#include "multi_bspline_create.h"
#endif
