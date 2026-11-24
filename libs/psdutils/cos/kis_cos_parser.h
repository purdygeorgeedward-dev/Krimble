/*
 *  SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISCOSPARSER_H
#define KISCOSPARSER_H

#include <QVariant>
#include <QIODevice>
#include "kritapsdutils_export.h"

class KRITAPSDUTILS_EXPORT KisCosParser
{
public:
    QVariantHash parseCosToJson(QByteArray *ba);
private:

    bool parseValue(QIODevice &dev, QVariant &val);
    bool parseObject(QIODevice &dev, QVariantHash &object, bool checkEnd = true);
    bool parseArray(QIODevice &dev, QVariantList &array);
};

#endif
