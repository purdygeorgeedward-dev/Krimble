/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2021 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ASL_READER_UTILS_H
#define __KIS_ASL_READER_UTILS_H

#include "psd.h"
#include "psd_utils.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <QtEndian>

#include <kis_debug.h>

#define GARBAGE_VALUE_MARK 999

namespace KisAslReaderUtils
{

struct KRITAPSDUTILS_EXPORT ASLParseException : public std::runtime_error {
    ASLParseException(const QString &msg)
        : std::runtime_error(msg.toLatin1().data())
    {
    }
};

}

#define SAFE_READ_EX(byteOrder, device, varname)                                                                                                               \
    if (!psdread<byteOrder>(device, varname)) {                                                                                                                \
        QString msg = QString("Failed to read \'%1\' tag!").arg(#varname);                                                                                     \
        throw KisAslReaderUtils::ASLParseException(msg);                                                                                                       \
    }

#define SAFE_READ_SIGNATURE_EX(byteOrder, device, varname, expected)                                                                                           \
    if (!psdread<byteOrder>(device, varname) || varname != expected) {                                                                                         \
        QString msg = QString(                                                                                                                                 \
                          "Failed to check signature \'%1\' tag!\n"                                                                                            \
                          "Value: \'%2\' Expected: \'%3\'")                                                                                                    \
                          .arg(#varname)                                                                                                                       \
                          .arg(varname)                                                                                                                        \
                          .arg(expected);                                                                                                                      \
        throw KisAslReaderUtils::ASLParseException(msg);                                                                                                       \
    }

template<psd_byte_order byteOrder, typename T, size_t S>
inline bool TRY_READ_SIGNATURE_2OPS_EX(QIODevice &device, const std::array<T, S> &expected1, const std::array<T, S> &expected2)
{
    QByteArray bytes = device.peek(S);

    if (byteOrder == psd_byte_order::psdLittleEndian) {
        std::reverse(bytes.begin(), bytes.end());
    }

    if (bytes.size() != S) {
        return false;
    }

    if (std::equal(bytes.constBegin(), bytes.constEnd(), expected1.begin())
        || std::equal(bytes.constBegin(), bytes.constEnd(), expected2.begin())) {
        auto bytesRead = psdreadBytes(device, S);
        if (bytesRead.size() == S) {
            return true;
        }
    }

    dbgFile << "PS signature verification failed! Got: " << bytes.toHex() << "(" << QString(bytes) << ")";
    return false;
}

template<typename T, size_t S>
inline bool TRY_READ_SIGNATURE_2OPS_EX(psd_byte_order byteOrder, QIODevice &device, const std::array<T, S> &expected1, const std::array<T, S> &expected2)
{
    switch (byteOrder) {
    case psd_byte_order::psdLittleEndian:
        return TRY_READ_SIGNATURE_2OPS_EX<psd_byte_order::psdLittleEndian>(device, expected1, expected2);
    default:
        return TRY_READ_SIGNATURE_2OPS_EX<psd_byte_order::psdBigEndian>(device, expected1, expected2);
    }
}

namespace KisAslReaderUtils
{

template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
inline QString readStringCommon(QIODevice &device, int length)
{
    QByteArray data = psdreadBytes<byteOrder>(device, length);

    if (data.size() != length) {
        QString msg = QString(
                          "Failed to read a string! "
                          "Bytes read: %1 Expected: %2")
                          .arg(data.size())
                          .arg(length);
        throw ASLParseException(msg);
    }

    return QString(data);
}

template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
inline QString readFixedString(QIODevice &device)
{
    return readStringCommon<byteOrder>(device, 4);
}

template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
inline QString readVarString(QIODevice &device)
{
    quint32 length = 0;
    SAFE_READ_EX(byteOrder, device, length);

    if (!length) {
        length = 4;
    }

    return readStringCommon<byteOrder>(device, length);
}

template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
inline QString readPascalString(QIODevice &device)
{
    quint8 length = 0;
    SAFE_READ_EX(byteOrder, device, length);

    return readStringCommon(device, length);
}

template<psd_byte_order byteOrder = psd_byte_order::psdBigEndian>
inline QString readUnicodeString(QIODevice &device)
{
    QString string;

    if (!psdread_unicodestring<byteOrder>(device, string)) {
        QString msg = QString("Failed to read a unicode string!");
        throw ASLParseException(msg);
    }

    return string;
}
}

#endif
