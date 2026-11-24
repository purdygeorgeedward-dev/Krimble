/*
 *  SPDX-FileCopyrightText: 2023 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTXSIMDPAINTING_H
#define TESTXSIMDPAINTING_H

#include <simpletest.h>

class TestXsimdPainting : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testKoClipMaskPainting_data();
    void testKoClipMaskPainting();
};

#endif
