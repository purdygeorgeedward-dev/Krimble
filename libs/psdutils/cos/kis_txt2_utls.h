/*
 *  SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TXT2_UTLS_H
#define KIS_TXT2_UTLS_H

#include <QVariantHash>
#include "kritapsdutils_export.h"

class KRITAPSDUTILS_EXPORT KisTxt2Utils
{
public:

    static QVariantHash uncompressKeys(QVariantHash doc);

    static QVariantHash defaultTxt2();

    static QVariantHash tyShFromTxt2(const QVariantHash Txt2, const QRectF boundsInPx, int textIndex);
};

#endif
