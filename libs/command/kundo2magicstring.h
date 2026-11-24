/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2014 Alexander Potashev <aspotashev@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KUNDO2MAGICSTRING_H
#define KUNDO2MAGICSTRING_H

#include <QString>
#include <QDebug>

#include <klocalizedstring.h>
#include <boost/operators.hpp>

#include "kritacommand_export.h"

class KRITACOMMAND_EXPORT KUndo2MagicString : public boost::equality_comparable<KUndo2MagicString>
{
public:
    KUndo2MagicString();

    QString toString() const;

    QString toSecondaryString() const;

    bool isEmpty() const;

    bool operator==(const KUndo2MagicString &rhs) const;

private:
    explicit KUndo2MagicString(const QString &text);

    friend KUndo2MagicString kundo2_noi18n(const QString &text);
    template <typename A1>
    friend KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1);
    template <typename A1, typename A2>
    friend KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2);
    template <typename A1, typename A2, typename A3>
    friend KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3);
    template <typename A1, typename A2, typename A3, typename A4>
    friend KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4);

    friend KUndo2MagicString kundo2_i18n(const char *text);
    template <typename A1>
    friend KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1);
    template <typename A1, typename A2>
    friend KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2);
    template <typename A1, typename A2, typename A3>
    friend KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3);
    template <typename A1, typename A2, typename A3, typename A4>
    friend KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4);

    friend KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text);
    template <typename A1>
    friend KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1);
    template <typename A1, typename A2>
    friend KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2);
    template <typename A1, typename A2, typename A3>
    friend KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3);

    template <typename A1>
    friend KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1);
    template <typename A1, typename A2>
    friend KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2);
    template <typename A1, typename A2, typename A3>
    friend KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3);

    template <typename A1>
    friend KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1);
    template <typename A1, typename A2>
    friend KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2);
    template <typename A1, typename A2, typename A3>
    friend KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3);

private:
    QString m_text;
};

inline QDebug operator<<(QDebug dbg, const KUndo2MagicString &v)
{
    if (v.toString() != v.toSecondaryString()) {
        dbg.nospace() << v.toString() << "(" << v.toSecondaryString() << ")";
    } else {
        dbg.nospace() << v.toString();
    }

    return dbg.space();
}

inline KUndo2MagicString kundo2_noi18n(const QString &text)
{
    return KUndo2MagicString(text);
}

template <typename A1>
inline KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1)
{
    return KUndo2MagicString(QString(text).arg(a1));
}

template <typename A1, typename A2>
inline KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2)
{
    return KUndo2MagicString(QString(text).arg(a1).arg(a2));
}

template <typename A1, typename A2, typename A3>
inline KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{
    return KUndo2MagicString(QString(text).arg(a1).arg(a2).arg(a3));
}

template <typename A1, typename A2, typename A3, typename A4>
inline KUndo2MagicString kundo2_noi18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    return KUndo2MagicString(QString(text).arg(a1).arg(a2).arg(a3).arg(a4));
}

inline KUndo2MagicString kundo2_i18n(const char *text)
{
    return KUndo2MagicString(i18nc("(qtundo-format)", text));
}

template <typename A1>
inline KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1)
{
    return KUndo2MagicString(i18nc("(qtundo-format)", text, a1));
}

template <typename A1, typename A2>
inline KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2)
{
    return KUndo2MagicString(i18nc("(qtundo-format)", text, a1, a2));
}

template <typename A1, typename A2, typename A3>
inline KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{
    return KUndo2MagicString(i18nc("(qtundo-format)", text, a1, a2, a3));
}

template <typename A1, typename A2, typename A3, typename A4>
inline KUndo2MagicString kundo2_i18n(const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    return KUndo2MagicString(i18nc("(qtundo-format)", text, a1, a2, a3, a4));
}

inline QString prependContext(const char *ctxt)
{
    return QString("(qtundo-format) %1").arg(ctxt);
}

inline KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text)
{
    return KUndo2MagicString(i18nc(prependContext(ctxt).toLatin1().data(), text));
}

template <typename A1>
inline KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1)
{
    return KUndo2MagicString(i18nc(prependContext(ctxt).toLatin1().data(), text, a1));
}

template <typename A1, typename A2>
inline KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2)
{
    return KUndo2MagicString(i18nc(prependContext(ctxt).toLatin1().data(), text, a1, a2));
}

template <typename A1, typename A2, typename A3>
inline KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{
    return KUndo2MagicString(i18nc(prependContext(ctxt).toLatin1().data(), text, a1, a2, a3));
}

template <typename A1, typename A2, typename A3, typename A4>
inline KUndo2MagicString kundo2_i18nc(const char *ctxt, const char *text, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    return KUndo2MagicString(i18nc(prependContext(ctxt).toLatin1().data(), text, a1, a2, a3, a4));
}

template <typename A1>
inline KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1)
{
    return KUndo2MagicString(i18ncp("(qtundo-format)", sing, plur, a1));
}

template <typename A1, typename A2>
inline KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2)
{
    return i18ncp("(qtundo-format)", sing, plur, a1, a2);
}

template <typename A1, typename A2, typename A3>
inline KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3)
{
    return i18ncp("(qtundo-format)", sing, plur, a1, a2, a3);
}

template <typename A1, typename A2, typename A3, typename A4>
inline KUndo2MagicString kundo2_i18np(const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    return i18ncp("(qtundo-format)", sing, plur, a1, a2, a3, a4);
}

template <typename A1>
inline KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1)
{
    return KUndo2MagicString(i18ncp(prependContext(ctxt).toLatin1().data(), sing, plur, a1));
}

template <typename A1, typename A2>
inline KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2)
{
    return i18ncp(prependContext(ctxt).toLatin1().data(), sing, plur, a1, a2);
}

template <typename A1, typename A2, typename A3>
inline KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3)
{
    return i18ncp(prependContext(ctxt).toLatin1().data(), sing, plur, a1, a2, a3);
}

template <typename A1, typename A2, typename A3, typename A4>
inline KUndo2MagicString kundo2_i18ncp(const char *ctxt, const char *sing, const char *plur, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
    return i18ncp(prependContext(ctxt).toLatin1().data(), sing, plur, a1, a2, a3, a4);
}

#endif
