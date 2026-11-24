/* Palette-manipulation functions functions for xcftools
 *
 * This file was written by Henning Makholm <henning@makholm.net>
 * It is hereby in the public domain.
 *
 * In jurisdictions that do not recognise grants of copyright to the
 * public domain: I, the author and (presumably, in those jurisdictions)
 * copyright holder, hereby permit anyone to distribute and use this code,
 * in source code or binary form, with or without modifications. This
 * permission is world-wide and irrevocable.
 *
 * Of course, I will not be liable for any errors or shortcomings in the
 * code, since I give it away without asking any compenstations.
 *
 * If you use or distribute this code, I would appreciate receiving
 * credit for writing it, in whichever way you find proper and customary.
 */

#ifndef PALETTE_H
#define PALETTE_H

#include "xcftools.h"
#include "pixels.h"

#define MAX_PALETTE 256
extern rgba palette[MAX_PALETTE] ;
extern unsigned paletteSize ;

typedef uint8_t index_t ;

void init_palette_hash(void);

int lookup_or_intern(rgba color);

int palettify_row(rgba *row,unsigned ncols);

int palettify_rows(rgba *rows[],unsigned ncols,unsigned nlines);

#endif
