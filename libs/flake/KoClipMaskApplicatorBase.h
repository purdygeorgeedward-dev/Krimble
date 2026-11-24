/*
 *  SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOCLIPMASKAPPLICATORBASE_H
#define KOCLIPMASKAPPLICATORBASE_H

#include <KoStreamedMath.h>
#include <QDebug>

struct KoClipMaskApplicatorBase {

    virtual void applyLuminanceMask(quint8 *pixels,
                                    quint8 *maskPixels,
                                    const int nPixels) const = 0;

    virtual void fallbackLuminanceMask(quint8 *pixels,
                               quint8 *maskPixels,
                               const int nPixels) const;

    virtual ~KoClipMaskApplicatorBase() = default;
};

#endif
