/*
 *  SPDX-FileCopyrightText: 2018 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOFAKEPROGRESSPROXY_H
#define KOFAKEPROGRESSPROXY_H

#include <KoProgressProxy.h>

class KRITAWIDGETUTILS_EXPORT KoFakeProgressProxy : public KoProgressProxy
{
public:
    int maximum() const override;
    void setValue(int value) override;
    void setRange(int minimum, int maximum) override;
    void setFormat(const QString &format) override;
    void setAutoNestedName(const QString &name) override;

    static KoProgressProxy* instance();
};

#endif
