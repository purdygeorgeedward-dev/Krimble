/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt boud @valdyas.org
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_DATAMANAGER_TEST_H
#define KIS_DATAMANAGER_TEST_H

#include <simpletest.h>

class KisDataManagerTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testCreation();
    void testDefaultPixel();

private:
    bool memoryIsFilled(quint8 c, quint8 *mem, qint32 size);
};

#endif
