/* unzip.h -- IO for uncompress .zip files using zlib
   Version 1.2.0, September 16th, 2017
   part of the MiniZip project

   Copyright (C) 2012-2017 Nathan Moinvaziri
     https://github.com/nmoinvaz/minizip
   Copyright (C) 2009-2010 Mathias Svensson
     Modifications for Zip64 support on both zip and unzip
     http://result42.com
   Copyright (C) 2007-2008 Even Rouault
     Modifications of Unzip for Zip64
   Copyright (C) 1998-2010 Gilles Vollant
     http://www.winimage.com/zLibDll/minizip.html

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/

#ifndef _UNZ_H
#define _UNZ_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ZLIB_H
#include "zlib.h"
#endif

#ifndef _ZLIBIOAPI_H
#include "ioapi.h"
#endif

#ifdef HAVE_BZIP2
#include "bzlib.h"
#endif

#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
typedef struct TagunzFile__ { int unused; } unz_file__;
typedef unz_file__ *unzFile;
#else
typedef voidp unzFile;
#endif

#define UNZ_OK                          (0)
#define UNZ_END_OF_LIST_OF_FILE         (-100)
#define UNZ_ERRNO                       (Z_ERRNO)
#define UNZ_EOF                         (0)
#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_INTERNALERROR               (-104)
#define UNZ_CRCERROR                    (-105)
#define UNZ_BADPASSWORD                 (-106)

typedef struct unz_global_info64_s
{
    uint64_t number_entry;
    uint32_t number_disk_with_CD;
    uint16_t size_comment;
} unz_global_info64;

typedef struct unz_global_info_s
{
    uint32_t number_entry;
    uint32_t number_disk_with_CD;
    uint16_t size_comment;
} unz_global_info;

typedef struct unz_file_info64_s
{
    uint16_t version;
    uint16_t version_needed;
    uint16_t flag;
    uint16_t compression_method;
    uint32_t dos_date;
    uint32_t crc;
    uint64_t compressed_size;
    uint64_t uncompressed_size;
    uint16_t size_filename;
    uint16_t size_file_extra;
    uint16_t size_file_comment;

    uint32_t disk_num_start;
    uint16_t internal_fa;
    uint32_t external_fa;

    uint64_t disk_offset;

    uint16_t size_file_extra_internal;
} unz_file_info64;

typedef struct unz_file_info_s
{
    uint16_t version;
    uint16_t version_needed;
    uint16_t flag;
    uint16_t compression_method;
    uint32_t dos_date;
    uint32_t crc;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t size_filename;
    uint16_t size_file_extra;
    uint16_t size_file_comment;

    uint16_t disk_num_start;
    uint16_t internal_fa;
    uint32_t external_fa;

    uint64_t disk_offset;
} unz_file_info;

extern unzFile ZEXPORT unzOpen(const char *path);
extern unzFile ZEXPORT unzOpen64(const void *path);

extern unzFile ZEXPORT unzOpen2(const char *path, zlib_filefunc_def *pzlib_filefunc_def);
extern unzFile ZEXPORT unzOpen2_64(const void *path, zlib_filefunc64_def *pzlib_filefunc_def);

extern int ZEXPORT unzClose(unzFile file);

extern int ZEXPORT unzGetGlobalInfo(unzFile file, unz_global_info *pglobal_info);
extern int ZEXPORT unzGetGlobalInfo64(unzFile file, unz_global_info64 *pglobal_info);

extern int ZEXPORT unzGetGlobalComment(unzFile file, char *comment, uint16_t comment_size);

extern int ZEXPORT unzOpenCurrentFile(unzFile file);

extern int ZEXPORT unzOpenCurrentFilePassword(unzFile file, const char *password);

extern int ZEXPORT unzOpenCurrentFile2(unzFile file, int *method, int *level, int raw);

extern int ZEXPORT unzOpenCurrentFile3(unzFile file, int *method, int *level, int raw, const char *password);

extern int ZEXPORT unzReadCurrentFile(unzFile file, voidp buf, uint32_t len);

extern int ZEXPORT unzGetCurrentFileInfo(unzFile file, unz_file_info *pfile_info, char *filename,
    uint16_t filename_size, void *extrafield, uint16_t extrafield_size, char *comment, uint16_t comment_size);
extern int ZEXPORT unzGetCurrentFileInfo64(unzFile file, unz_file_info64 *pfile_info, char *filename,
    uint16_t filename_size, void *extrafield, uint16_t extrafield_size, char *comment, uint16_t comment_size);

extern int ZEXPORT unzGetLocalExtrafield(unzFile file, voidp buf, uint32_t len);

extern int ZEXPORT unzCloseCurrentFile(unzFile file);

typedef int (*unzFileNameComparer)(unzFile file, const char *filename1, const char *filename2);
typedef int (*unzIteratorFunction)(unzFile file);
typedef int (*unzIteratorFunction2)(unzFile file, unz_file_info64 *pfile_info, char *filename,
    uint16_t filename_size, void *extrafield, uint16_t extrafield_size, char *comment, uint16_t comment_size);

extern int ZEXPORT unzGoToFirstFile(unzFile file);

extern int ZEXPORT unzGoToFirstFile2(unzFile file, unz_file_info64 *pfile_info, char *filename,
    uint16_t filename_size, void *extrafield, uint16_t extrafield_size, char *comment, uint16_t comment_size);

extern int ZEXPORT unzGoToNextFile(unzFile file);

extern int ZEXPORT unzGoToNextFile2(unzFile file, unz_file_info64 *pfile_info, char *filename,
    uint16_t filename_size, void *extrafield, uint16_t extrafield_size, char *comment, uint16_t comment_size);

extern int ZEXPORT unzLocateFile(unzFile file, const char *filename, unzFileNameComparer filename_compare_func);

typedef struct unz_file_pos_s
{
    uint32_t pos_in_zip_directory;
    uint32_t num_of_file;
} unz_file_pos;

extern int ZEXPORT unzGetFilePos(unzFile file, unz_file_pos *file_pos);
extern int ZEXPORT unzGoToFilePos(unzFile file, unz_file_pos *file_pos);

typedef struct unz64_file_pos_s
{
    uint64_t pos_in_zip_directory;
    uint64_t num_of_file;
} unz64_file_pos;

extern int ZEXPORT unzGetFilePos64(unzFile file, unz64_file_pos *file_pos);
extern int ZEXPORT unzGoToFilePos64(unzFile file, const unz64_file_pos *file_pos);

extern int32_t ZEXPORT unzGetOffset(unzFile file);
extern int64_t ZEXPORT unzGetOffset64(unzFile file);

extern int ZEXPORT unzSetOffset(unzFile file, uint32_t pos);
extern int ZEXPORT unzSetOffset64(unzFile file, uint64_t pos);

extern int32_t ZEXPORT unzTell(unzFile file);
extern int64_t ZEXPORT unzTell64(unzFile file);

extern int ZEXPORT unzSeek(unzFile file, uint32_t offset, int origin);
extern int ZEXPORT unzSeek64(unzFile file, uint64_t offset, int origin);

extern int ZEXPORT unzEndOfFile(unzFile file);

#ifdef __cplusplus
}
#endif

#endif
