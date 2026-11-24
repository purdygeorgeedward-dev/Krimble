/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  adopted from here http://web.archive.org/web/20090728150504/http://www.snippetcenter.org/en/a-fast-atan2-function-s1868.aspx
 */

#include "kis_fast_math.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <QtGlobal>
#include <QGlobalStatic>

const qreal MAX_SECOND_DERIV_IN_RANGE = 0.6495;

const qreal MAX_ERROR = 0.0001;

struct KisATanTable {

    KisATanTable() {
        qreal nf = ::sqrt(MAX_SECOND_DERIV_IN_RANGE / (8 * MAX_ERROR));
        NUM_ATAN_ENTRIES = int(nf) + 1;
        qreal y = 10.0;
        qreal x;
        ATanTable = new qreal[NUM_ATAN_ENTRIES + 1];
        ATanTable[0] = 0.0;
        for (quint32 i = 1; i <= NUM_ATAN_ENTRIES; i++) {
            x = (y / i) * NUM_ATAN_ENTRIES;
            ATanTable[i] = (qreal)::atan2(y, x);
        }

    }

    ~KisATanTable() {
        delete [] ATanTable;
    }

    quint32 NUM_ATAN_ENTRIES;
    qreal* ATanTable;
};

Q_GLOBAL_STATIC(KisATanTable, kisATanTable)

inline qreal interp(qreal r, qreal a, qreal b)
{
    return r*(b - a) + a;
}

inline qreal calcAngle(qreal x, qreal y)
{
    static qreal af = kisATanTable->NUM_ATAN_ENTRIES;
    static int ai = kisATanTable->NUM_ATAN_ENTRIES;
    static qreal* ATanTable = kisATanTable->ATanTable;
    qreal di = (y / x) * af;
    int i = (int)(di);
    if (i >= ai) return ::atan2(y, x);
    return interp(di - i, ATanTable[i], ATanTable[i+1]);
}

qreal KisFastMath::atan2(qreal y, qreal x)
{

    if (y == 0.0) {
        if (x >= 0.0) {
            return(0.0);
        }
        return qreal(M_PI);
    }
    if (x == 0.0) {
        if (y > 0.0) {
            return M_PI_2;
        }
        return -M_PI_2;
    }
    if (x > 0.0) {
        if (y > 0.0) {
            if (x > y) {
                return(calcAngle(x, y));
            }
            return(M_PI_2 - calcAngle(y, x));
        }
        y = -y;
        if (x > y) {
            return(-calcAngle(x, y));
        }
        return(-M_PI_2 + calcAngle(y, x));
    }
    x = -x;
    if (y > 0.0) {
        if (x > y) {
            return(M_PI - calcAngle(x, y));
        } return(M_PI_2 + calcAngle(y, x));
    }
    y = -y;
    if (x > y) {
        return(-M_PI + calcAngle(x, y));
    } return(-M_PI_2 - calcAngle(y, x));
}
