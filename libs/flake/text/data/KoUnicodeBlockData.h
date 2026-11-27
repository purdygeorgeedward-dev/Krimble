/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOUNICODEBLOCKDATA_H
#define KOUNICODEBLOCKDATA_H

#include <QString>
#include <QChar>
#include <QScopedPointer>
#include "kritaflake_export.h"

#include <boost/operators.hpp>
#include <klocalizedstring.h>

struct KRITAFLAKE_EXPORT KoUnicodeBlockData : public boost::equality_comparable<KoUnicodeBlockData> {
    KoUnicodeBlockData(QString name, uint start, uint end)
        : name(name)
        , start(start)
        , end(end) {}
    QString name;
    uint start;
    uint end;

    bool operator==(const KoUnicodeBlockData &rhs) const {
        return (start == rhs.start && end == rhs.end);
    }

    bool match (const uint &codepoint) const {
        return codepoint >= start && codepoint <= end;
    }
};

class KRITAFLAKE_EXPORT KoUnicodeBlockDataFactory {
public:
    KoUnicodeBlockDataFactory();
    ~KoUnicodeBlockDataFactory();

    KoUnicodeBlockData blockForUCS(const uint &codepoint);

    static KoUnicodeBlockData noBlock() {
        static const QString noBlockName = i18nc("@title", "No Block");
        return KoUnicodeBlockData(noBlockName, 0x10FFFF, 0x10FFFF);
    }
private:
    struct Private;

    QScopedPointer<Private> d;
};

#endif
