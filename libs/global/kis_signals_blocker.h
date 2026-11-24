/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_SIGNALS_BLOCKER_H
#define __KIS_SIGNALS_BLOCKER_H

#include <QObject>
#include <QVector>

class KisSignalsBlocker
{
public:
    KisSignalsBlocker(QObject *o1,
                      QObject *o2,
                      QObject *o3 = 0,
                      QObject *o4 = 0,
                      QObject *o5 = 0,
                      QObject *o6 = 0)
    {
        if (o1) addObject(o1);
        if (o2) addObject(o2);
        if (o3) addObject(o3);
        if (o4) addObject(o4);
        if (o5) addObject(o5);
        if (o6) addObject(o6);

        blockObjects();
    }

    KisSignalsBlocker(QObject *object)
    {
        addObject(object);
        blockObjects();
    }

    ~KisSignalsBlocker()
    {
        auto it = m_objects.end();
        auto begin = m_objects.begin();

        while (it != begin) {
            --it;
            it->first->blockSignals(it->second);
        }
    }

private:
    void blockObjects() {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            it->first->blockSignals(true);
        }
    }

    inline void addObject(QObject *object) {
        m_objects.append(qMakePair(object, object->signalsBlocked()));
    }

private:
    Q_DISABLE_COPY(KisSignalsBlocker)

private:
    QVector<QPair<QObject*,bool>> m_objects;
};

#endif
