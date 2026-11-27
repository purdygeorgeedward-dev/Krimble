/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_TIFF_PSD_RESOURCE_RECORD_H
#define _KIS_TIFF_PSD_RESOURCE_RECORD_H

#include <QMap>
#include <QString>
#include <psd.h>

#include "kritatiffpsd_export.h"

class QIODevice;

class PSDResourceBlock;

class KRITATIFFPSD_EXPORT KisTiffPsdResourceRecord
{
public:
    enum PSDResourceID {
        UNKNOWN = 0,

        PS2_IMAGE_INFO = 1000,
        MAC_PRINT_INFO = 1001,
        PS2_COLOR_TAB = 1003,
        RESN_INFO = 1005,
        ALPHA_NAMES = 1006,
        DISPLAY_INFO = 1007,
        CAPTION = 1008,
        BORDER_INFO = 1009,

        BACKGROUND_COL = 1010,
        PRINT_FLAGS = 1011,
        GREY_HALFTONE = 1012,
        COLOR_HALFTONE = 1013,
        DUOTONE_HALFTONE = 1014,
        GREY_XFER = 1015,
        COLOR_XFER = 1016,
        DUOTONE_XFER = 1017,
        DUOTONE_INFO = 1018,
        EFFECTIVE_BW = 1019,

        OBSOLETE_01 = 1020,
        EPS_OPT = 1021,
        QUICK_MASK = 1022,
        OBSOLETE_02 = 1023,
        LAYER_STATE = 1024,
        WORKING_PATH = 1025,
        LAYER_GROUP = 1026,
        OBSOLETE_03 = 1027,
        IPTC_NAA_DATA = 1028,
        IMAGE_MODE_RAW = 1029,

        JPEG_QUAL = 1030,
        GRID_GUIDE = 1032,
        THUMB_RES = 1033,
        COPYRIGHT_FLG = 1034,
        URL = 1035,
        THUMB_RES2 = 1036,
        GLOBAL_ANGLE = 1037,
        COLOR_SAMPLER = 1038,
        ICC_PROFILE = 1039,

        WATERMARK = 1040,
        ICC_UNTAGGED = 1041,
        EFFECTS_VISIBLE = 1042,
        SPOT_HALFTONE = 1043,
        DOC_IDS = 1044,
        ALPHA_NAMES_UNI = 1045,
        IDX_COL_TAB_CNT = 1046,
        IDX_TRANSPARENT = 1047,
        GLOBAL_ALT = 1049,

        SLICES = 1050,
        WORKFLOW_URL_UNI = 1051,
        JUMP_TO_XPEP = 1052,
        ALPHA_ID = 1053,
        URL_LIST_UNI = 1054,
        VERSION_INFO = 1057,
        EXIF_DATA = 1058,
        EXIF_DATA_3 = 1059,

        XMP_DATA = 1060,
        CAPTION_DIGEST = 1061,
        PRINT_SCALE = 1062,
        PIXEL_ASPECT_RATION = 1064,
        LAYER_COMPS = 1065,
        ALTERNATE_DUOTONE = 1066,
        ALTERNATE_SPOT =
            1067,
        LAYER_SELECTION_ID = 1069,

        HDR_TONING = 1070,
        CS2_PRINT_INFO = 1071,
        LAYER_GROUP_ENABLED_ID = 1072,
        COLOR_SAMPLERS = 1073,
        MEASUREMENT_SCALE = 1074,
        TIMELINE_INFO = 1075,
        SHEET_DISCLOSURE = 1076,
        CS3_DISPLAY_INFO = 1077,
        ONION_SKINS = 1078,

        COUNT_INFO = 1080,
        CS5_PRINT_INFO = 1082,
        CS5_PRINT_STYLE = 1083,
        CS5_NSPrintInfo = 1084,
        CS5_WIN_DEVMODE = 1085,
        CS6_AUTOSAVE_FILE_PATH =
            1086,
        CS6_AUTOSAVE_FORMAT =
            1087,
        CC_PATH_SELECTION_SATE = 1088,

        PATH_INFO_FIRST = 2000,
        PATH_INFO_LAST = 2998,
        CLIPPING_PATH = 2999,

        CC_ORIGIN_PATH_INFO = 3000,

        PLUGIN_RESOURCE_START = 4000,
        PLUGIN_RESOURCE_END = 4999,

        IMAGE_READY_VARS = 7000,
        IMAGE_READY_DATA_SETS = 7001,

        LIGHTROOM_WORKFLOW = 8000,

        PRINT_FLAGS_2 = 10000
    };

    KisTiffPsdResourceRecord();
    ~KisTiffPsdResourceRecord();

    bool read(QIODevice &io);
    bool write(QIODevice &io);
    bool valid();

    static QString idToString(PSDResourceID id);

    QMap<PSDResourceID, PSDResourceBlock *> resources;

    QString error;
};

#endif
