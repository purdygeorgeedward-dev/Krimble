/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef UnzipTask_h
#define UnzipTask_h

#import <Foundation/Foundation.h>

FOUNDATION_EXPORT NSData *_Nullable UnzipTask(NSString *_Nonnull path,
                                              NSString *_Nonnull target);

#endif
