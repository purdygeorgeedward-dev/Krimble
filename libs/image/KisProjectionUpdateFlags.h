/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISPROJECTIONUPDATEFLAGS_H
#define KISPROJECTIONUPDATEFLAGS_H

#include <QFlags>

enum class KisProjectionUpdateFlag
{
    None = 0x0,
    NoFilthy = 0x1,

    DontInvalidateFrames = 0x2
};
Q_DECLARE_FLAGS(KisProjectionUpdateFlags, KisProjectionUpdateFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(KisProjectionUpdateFlags)

#endif
