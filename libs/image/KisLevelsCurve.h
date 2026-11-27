/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_LEVELS_CURVE_H
#define KIS_LEVELS_CURVE_H

#include <QtGlobal>
#include <QVector>

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisLevelsCurve
{
public:
    static constexpr qreal defaultInputBlackPoint() { return 0.0; }
    static constexpr qreal defaultInputWhitePoint() { return 1.0; }
    static constexpr qreal defaultInputGamma() { return 1.0; }
    static constexpr qreal defaultOutputBlackPoint() { return 0.0; }
    static constexpr qreal defaultOutputWhitePoint() { return 1.0; }

    KisLevelsCurve();
    KisLevelsCurve(qreal inputBlackPoint, qreal inputWhitePoint, qreal inputGamma,
                   qreal outputBlackPoint, qreal outputWhitePoint);
    KisLevelsCurve(const QString &text);
    KisLevelsCurve(const KisLevelsCurve &rhs) = default;
    KisLevelsCurve& operator=(const KisLevelsCurve &rhs) = default;
    bool operator==(const KisLevelsCurve& rhs) const;

    qreal value(qreal x) const;

    qreal inputBlackPoint() const;
    qreal inputWhitePoint() const;
    qreal inputGamma() const;
    qreal outputBlackPoint() const;
    qreal outputWhitePoint() const;

    void setInputBlackPoint(qreal newInputBlackPoint);
    void setInputWhitePoint(qreal newInputWhitePoint);
    void setInputGamma(qreal newInputGamma);
    void setOutputBlackPoint(qreal newOutputBlackPoint);
    void setOutputWhitePoint(qreal newOutputWhitePoint);

    void resetAll();
    void resetInputLevels();
    void resetOutputLevels();

    bool isIdentity() const;

    const QString& name() const;
    void setName(const QString &newName);

    const QVector<quint16>& uint16Transfer(int size = 256) const;
    const QVector<qreal>& floatTransfer(int size = 256) const;

    QString toString() const;
    void fromString(const QString &text, bool *ok = nullptr);

private:
    qreal m_inputBlackPoint, m_inputWhitePoint, m_inputGamma;
    qreal m_outputBlackPoint, m_outputWhitePoint;
    qreal m_inputLevelsDelta, m_inverseInputGamma, m_outputLevelsDelta;
    QString m_name;
    mutable QVector<quint16> m_u16Transfer;
    mutable QVector<qreal> m_fTransfer;
    mutable bool m_mustRecomputeU16Transfer;
    mutable bool m_mustRecomputeFTransfer;

    void invalidate();
};

#endif
