/*
 *  SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSPRAYRANDOMDISTRIBUTIONS_H
#define KISSPRAYRANDOMDISTRIBUTIONS_H

#include <QScopedPointer>

#include <kis_random_source.h>
#include <kis_cubic_curve.h>

class KisSprayFunctionBasedDistribution
{
public:
    KisSprayFunctionBasedDistribution();

    template <typename Function>
    KisSprayFunctionBasedDistribution(int numberOfSamples, double a, double b, Function f);

    ~KisSprayFunctionBasedDistribution();
    KisSprayFunctionBasedDistribution(const KisSprayFunctionBasedDistribution &other);
    KisSprayFunctionBasedDistribution& operator=(const KisSprayFunctionBasedDistribution &rhs);

    double operator()(KisRandomSourceSP rs) const;

    double min() const;

    double max() const;

    bool isValid() const;

protected:
    template <typename Function>
    void initialize(size_t numberOfSamples, double a, double b, Function f);

private:
    class Private;
    QScopedPointer<Private> m_d;
};

class KisSprayUniformDistribution
{
public:
    double operator()(KisRandomSourceSP rs) const;

    double min() const { return 0.0; }

    double max() const { return 1.0; }

    bool isValid() const { return true; }
};

class KisSprayUniformDistributionPolarDistance : public KisSprayUniformDistribution
{
public:
    double operator()(KisRandomSourceSP rs) const;
};

class KisSprayNormalDistribution : public KisSprayFunctionBasedDistribution
{
public:
    KisSprayNormalDistribution();

    KisSprayNormalDistribution(double mean, double standardDeviation);
};

class KisSprayNormalDistributionPolarDistance : public KisSprayNormalDistribution
{
public:
    KisSprayNormalDistributionPolarDistance();
    KisSprayNormalDistributionPolarDistance(double mean, double standardDeviation);
};

class KisSprayClusterBasedDistribution : public KisSprayFunctionBasedDistribution
{
public:
    KisSprayClusterBasedDistribution();

    explicit KisSprayClusterBasedDistribution(double clusteringAmount);
};

class KisSprayClusterBasedDistributionPolarDistance : public KisSprayFunctionBasedDistribution
{
public:
    KisSprayClusterBasedDistributionPolarDistance();
    explicit KisSprayClusterBasedDistributionPolarDistance(double clusteringAmount);
};

class KisSprayCurveBasedDistribution : public KisSprayFunctionBasedDistribution
{
public:
    KisSprayCurveBasedDistribution();

    explicit KisSprayCurveBasedDistribution(const KisCubicCurve &curve, size_t repeat = 1);
};

class KisSprayCurveBasedDistributionPolarDistance : public KisSprayFunctionBasedDistribution
{
public:
    KisSprayCurveBasedDistributionPolarDistance();
    explicit KisSprayCurveBasedDistributionPolarDistance(const KisCubicCurve &curve, size_t repeat = 1);
};

#endif
