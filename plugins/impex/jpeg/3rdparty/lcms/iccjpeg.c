/*
 * Little cms
 * Copyright (C) 1998-2004 Marti Maria <x@unknown.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * iccprofile.c
 *
 * This file provides code to read and write International Color Consortium
 * (ICC) device profiles embedded in JFIF JPEG image files.  The ICC has
 * defined a standard format for including such data in JPEG "APP2" markers.
 * The code given here does not know anything about the internal structure
 * of the ICC profile data; it just knows how to put the profile data into
 * a JPEG file being written, or get it back out when reading.
 *
 * This code depends on new features added to the IJG JPEG library as of
 * IJG release 6b; it will not compile or work with older IJG versions.
 *
 * NOTE: this code would need surgery to work on 16-bit-int machines
 * with ICC profiles exceeding 64K bytes in size.  If you need to do that,
 * change all the "unsigned int" variables to "INT32".  You'll also need
 * to find a malloc() replacement that can allocate more than 64K.
 */

#include "iccjpeg.h"
#include <stdlib.h>

#define ICC_MARKER  (JPEG_APP0 + 2)
#define ICC_OVERHEAD_LEN  14
#define MAX_BYTES_IN_MARKER  65533
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)

void
write_icc_profile (j_compress_ptr cinfo,
		   const JOCTET *icc_data_ptr,
		   unsigned int icc_data_len)
{
  unsigned int num_markers;
  int cur_marker = 1;
  unsigned int length;

  num_markers = icc_data_len / MAX_DATA_BYTES_IN_MARKER;
  if (num_markers * MAX_DATA_BYTES_IN_MARKER != icc_data_len)
    num_markers++;

  while (icc_data_len > 0) {
    length = icc_data_len;
    if (length > MAX_DATA_BYTES_IN_MARKER)
      length = MAX_DATA_BYTES_IN_MARKER;
    icc_data_len -= length;

    jpeg_write_m_header(cinfo, ICC_MARKER,
			(unsigned int) (length + ICC_OVERHEAD_LEN));

    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x5F);
    jpeg_write_m_byte(cinfo, 0x50);
    jpeg_write_m_byte(cinfo, 0x52);
    jpeg_write_m_byte(cinfo, 0x4F);
    jpeg_write_m_byte(cinfo, 0x46);
    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x4C);
    jpeg_write_m_byte(cinfo, 0x45);
    jpeg_write_m_byte(cinfo, 0x0);

    jpeg_write_m_byte(cinfo, cur_marker);
    jpeg_write_m_byte(cinfo, (int) num_markers);

    while (length--) {
      jpeg_write_m_byte(cinfo, *icc_data_ptr);
      icc_data_ptr++;
    }
    cur_marker++;
  }
}

void
setup_read_icc_profile (j_decompress_ptr cinfo)
{
  jpeg_save_markers(cinfo, ICC_MARKER, 0xFFFF);
}

static boolean
marker_is_icc (jpeg_saved_marker_ptr marker)
{
  return
    marker->marker == ICC_MARKER &&
    marker->data_length >= ICC_OVERHEAD_LEN &&
    GETJOCTET(marker->data[0]) == 0x49 &&
    GETJOCTET(marker->data[1]) == 0x43 &&
    GETJOCTET(marker->data[2]) == 0x43 &&
    GETJOCTET(marker->data[3]) == 0x5F &&
    GETJOCTET(marker->data[4]) == 0x50 &&
    GETJOCTET(marker->data[5]) == 0x52 &&
    GETJOCTET(marker->data[6]) == 0x4F &&
    GETJOCTET(marker->data[7]) == 0x46 &&
    GETJOCTET(marker->data[8]) == 0x49 &&
    GETJOCTET(marker->data[9]) == 0x4C &&
    GETJOCTET(marker->data[10]) == 0x45 &&
    GETJOCTET(marker->data[11]) == 0x0;
}

boolean
read_icc_profile (j_decompress_ptr cinfo,
		  JOCTET **icc_data_ptr,
		  unsigned int *icc_data_len)
{
  jpeg_saved_marker_ptr marker;
  int num_markers = 0;
  int seq_no;
  JOCTET *icc_data;
  unsigned int total_length;
#define MAX_SEQ_NO  255
  char marker_present[MAX_SEQ_NO+1];
  unsigned int data_length[MAX_SEQ_NO+1];
  unsigned int data_offset[MAX_SEQ_NO+1];

  *icc_data_ptr = 0;
  *icc_data_len = 0;

  for (seq_no = 1; seq_no <= MAX_SEQ_NO; seq_no++)
    marker_present[seq_no] = 0;

  for (marker = cinfo->marker_list; marker != 0; marker = marker->next) {
    if (marker_is_icc(marker)) {
      if (num_markers == 0)
	num_markers = GETJOCTET(marker->data[13]);
      else if (num_markers != GETJOCTET(marker->data[13]))
	return FALSE;
      seq_no = GETJOCTET(marker->data[12]);
      if (seq_no <= 0 || seq_no > num_markers)
	return FALSE;
      if (marker_present[seq_no])
	return FALSE;
      marker_present[seq_no] = 1;
      data_length[seq_no] = marker->data_length - ICC_OVERHEAD_LEN;
    }
  }

  if (num_markers == 0)
    return FALSE;

  total_length = 0;
  for (seq_no = 1; seq_no <= num_markers; seq_no++) {
    if (marker_present[seq_no] == 0)
      return FALSE;
    data_offset[seq_no] = total_length;
    total_length += data_length[seq_no];
  }

  if (total_length <= 0)
    return FALSE;

  icc_data = (JOCTET *) malloc(total_length * sizeof(JOCTET));
  if (icc_data == 0)
    return FALSE;

  for (marker = cinfo->marker_list; marker != 0; marker = marker->next) {
    if (marker_is_icc(marker)) {
      JOCTET FAR *src_ptr;
      JOCTET *dst_ptr;
      unsigned int length;
      seq_no = GETJOCTET(marker->data[12]);
      dst_ptr = icc_data + data_offset[seq_no];
      src_ptr = marker->data + ICC_OVERHEAD_LEN;
      length = data_length[seq_no];
      while (length--) {
	*dst_ptr++ = *src_ptr++;
      }
    }
  }

  *icc_data_ptr = icc_data;
  *icc_data_len = total_length;

  return TRUE;
}
