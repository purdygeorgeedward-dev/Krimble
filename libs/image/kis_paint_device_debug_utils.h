/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PAINT_DEVICE_DEBUG_UTILS_H
#define __KIS_PAINT_DEVICE_DEBUG_UTILS_H

class QRect;
class QString;

#include <kis_types.h>
#include <kritaimage_export.h>

void KRITAIMAGE_EXPORT kis_debug_save_device_incremental(KisPaintDeviceSP device,
                                                         int i,
                                                         const QRect &rc,
                                                         const QString &suffix, const QString &prefix);

#define KIS_DUMP_DEVICE_1(device, rc, suffix)                           \
    do {                                                                \
        static int i = -1; i++;                                         \
        kis_debug_save_device_incremental((device), i, (rc), (suffix), QString()); \
    } while(0)

#define KIS_DUMP_DEVICE_2(device, rc, suffix, prefix)                   \
    do {                                                                \
        static int i = -1; i++;                                         \
        kis_debug_save_device_incremental((device), i, (rc), (suffix), (prefix)); \
    } while(0)

#endif
