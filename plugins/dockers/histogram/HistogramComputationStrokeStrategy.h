/*
 *  SPDX-FileCopyrightText: 2020 Agata Cacko <tamtamy.tymona@gmail.com>
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef HISTOGRAMCOMPUTATIONSTROKESTRATEGY_H
#define HISTOGRAMCOMPUTATIONSTROKESTRATEGY_H

#include <KisIdleTaskStrokeStrategy.h>
#include <vector>

class KoColorSpace;

using HistVector = std::vector<std::vector<quint32> >;

struct HistogramData
{
    HistVector bins;
    HistVector binsLog;
    const KoColorSpace* colorSpace {0};
    float maximumValue {1.0};
};
Q_DECLARE_METATYPE(HistogramData)

class HistogramComputationStrokeStrategy : public KisIdleTaskStrokeStrategy
{
    Q_OBJECT
public:
    HistogramComputationStrokeStrategy(KisImageSP image);
    ~HistogramComputationStrokeStrategy() override;

private:
    void initStrokeCallback() override;
    void doStrokeCallback(KisStrokeJobData *data) override;
    void finishStrokeCallback() override;

    void initiateVector(HistVector &vec, const KoColorSpace* colorSpace);

Q_SIGNALS:
    void computationResultReady(HistogramData data);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
