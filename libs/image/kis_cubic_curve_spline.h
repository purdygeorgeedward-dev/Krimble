/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2024 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_CUBIC_CURVE_SPLINE_H_
#define _KIS_CUBIC_CURVE_SPLINE_H_

#include <QVector>
#include <QList>

#include <Eigen/Sparse>

#include <kis_assert.h>

template <typename T>
class KisTridiagonalSystem
{

public:

    static
    QVector<T> calculate(QList<T> &a,
                         QList<T> &b,
                         QList<T> &c,
                         QList<T> &f) {
        QVector<T> x;
        QVector<T> alpha;
        QVector<T> beta;

        int i;
        int size = b.size();

        Q_ASSERT(a.size() == size - 1 &&
                 c.size() == size - 1 &&
                 f.size() == size);

        x.resize(size);

        if (size == 1) {
            x[0] = f[0] / b[0];
            return x;
        }

        alpha.resize(size);
        beta.resize(size);

        alpha[1] = -c[0] / b[0];
        beta[1] =  f[0] / b[0];

        for (i = 1; i < size - 1; i++) {
            alpha[i+1] = -c[i] /
                         (a[i-1] * alpha[i] + b[i]);

            beta[i+1] = (f[i] - a[i-1] * beta[i])
                        /
                        (a[i-1] * alpha[i] + b[i]);
        }

        x.last() = (f.last() - a.last() * beta.last())
                   /
                   (b.last() + a.last() * alpha.last());

        for (i = size - 2; i >= 0; i--)
            x[i] = alpha[i+1] * x[i+1] + beta[i+1];

        return x;
    }
};

template <typename T_point, typename T>
class KisLegacyCubicSpline
{

protected:
    QList<T> m_a;
    QVector<T> m_b;
    QVector<T> m_c;
    QVector<T> m_d;

    QVector<T> m_h;
    T m_begin;
    T m_end;
    int m_intervals {0};

public:
    KisLegacyCubicSpline() {}
    KisLegacyCubicSpline(const QList<T_point> &a) {
        createSpline(a);
    }

    void createSpline(const QList<T_point> &a) {
        int intervals = m_intervals = a.size() - 1;
        int i;
        m_begin = a.first().x();
        m_end = a.last().x();

        m_a.clear();
        m_b.resize(intervals);
        m_c.clear();
        m_d.resize(intervals);
        m_h.resize(intervals);

        for (i = 0; i < intervals; i++) {
            m_h[i] = a[i+1].x() - a[i].x();
            m_a.append(a[i].y());
        }
        m_a.append(a.last().y());

        QList<T> tri_b;
        QList<T> tri_f;
        QList<T> tri_a;

        for (i = 0; i < intervals - 1; i++) {
            tri_b.append(2.*(m_h[i] + m_h[i+1]));

            tri_f.append(6.*((m_a[i+2] - m_a[i+1]) / m_h[i+1] - (m_a[i+1] - m_a[i]) / m_h[i]));
        }
        for (i = 1; i < intervals - 1; i++)
            tri_a.append(m_h[i]);

        if (intervals > 1) {
            m_c = KisTridiagonalSystem<T>::calculate(tri_a, tri_b, tri_a, tri_f);
        }
        m_c.prepend(0);
        m_c.append(0);

        for (i = 0; i < intervals; i++)
            m_d[i] = (m_c[i+1] - m_c[i]) / m_h[i];

        for (i = 0; i < intervals; i++)
            m_b[i] = -0.5 * (m_c[i] * m_h[i])  - (1 / 6.0) * (m_d[i] * m_h[i] * m_h[i]) + (m_a[i+1] - m_a[i]) / m_h[i];
    }

    T getValue(T x) const {
        T x0;
        int i = findRegion(x, x0);
        return m_a[i] +
               m_b[i] *(x - x0) +
               0.5 * m_c[i] *(x - x0) *(x - x0) +
               (1 / 6.0)* m_d[i] *(x - x0) *(x - x0) *(x - x0);
    }

    T begin() const {
        return m_begin;
    }

    T end() const {
        return m_end;
    }

protected:

    int findRegion(T x, T &x0) const {
        int i;
        x0 = m_begin;
        for (i = 0; i < m_intervals; i++) {
            if (x >= x0 && x < x0 + m_h[i])
                return i;
            x0 += m_h[i];
        }
        if (x >= x0) {
            x0 -= m_h[m_intervals-1];
            return m_intervals - 1;
        }

        qDebug("X value: %f\n", x);
        qDebug("m_begin: %f\n", m_begin);
        qDebug("m_end  : %f\n", m_end);
        Q_ASSERT_X(0, "findRegion", "X value is outside regions");
        return -1;
    }
};

template <typename T_point, typename T>
class KisCubicSpline
{
public:
    KisCubicSpline() {}
    KisCubicSpline(const QList<T_point> &a) {
        createSpline(a);
    }

    void createSpline(const QList<T_point> &a) {
        KIS_SAFE_ASSERT_RECOVER_RETURN(a.size() > 0);

        const int intervals = a.size() - 1;
        m_points = a;

        m_coefficients.clear();

        if (a.size() == 1) {
            m_coefficients.append({ 0.0, 0.0, 0.0, a.first().y() });
            return;
        }

        if (a.size() == 2) {
            const T c = (a.last().y() - a.first().y()) / (a.last().x() - a.first().x());
            const T d = a.first().y() - c * a.first().x();
            m_coefficients.append({ 0.0, 0.0, c, d });
            return;
        }

        using Triplet = Eigen::Triplet<qreal>;
        using Matrix = Eigen::SparseMatrix<qreal>;
        using Vector = Eigen::VectorXd;

        const int numberOfRows = intervals * 4;
        const int numberOfColumns = numberOfRows;
        std::vector<Triplet> triplets;
        Matrix A(numberOfRows, numberOfColumns);
        Vector b(numberOfRows);

        triplets.reserve(numberOfRows * 4);
        qint32 row = 0;
        T pointX = a.first().x();
        T pointY = a.first().y();
        T pointXSquared = pointX * pointX;
        T pointXCubed = pointXSquared * pointX;
        for (qint32 i = 0; i < intervals; ++i) {
            const int baseColumn = i * 4;
            triplets.push_back(Triplet(row, baseColumn + 0, pointXCubed));
            triplets.push_back(Triplet(row, baseColumn + 1, pointXSquared));
            triplets.push_back(Triplet(row, baseColumn + 2, pointX));
            triplets.push_back(Triplet(row, baseColumn + 3, 1.0));
            b(row) = pointY;
            ++row;
            pointX = a[i + 1].x();
            pointY = a[i + 1].y();
            pointXSquared = pointX * pointX;
            pointXCubed = pointXSquared * pointX;
            triplets.push_back(Triplet(row, baseColumn + 0, pointXCubed));
            triplets.push_back(Triplet(row, baseColumn + 1, pointXSquared));
            triplets.push_back(Triplet(row, baseColumn + 2, pointX));
            triplets.push_back(Triplet(row, baseColumn + 3, 1.0));
            b(row) = pointY;
            ++row;
        }
        pointX = a.first().x();
        triplets.push_back(Triplet(row, 0, 6.0 * pointX));
        triplets.push_back(Triplet(row, 1, 2.0));
        b(row) = 0.0;
        ++row;
        pointX = a.last().x();
        triplets.push_back(Triplet(row, numberOfColumns - 4, 6.0 * pointX));
        triplets.push_back(Triplet(row, numberOfColumns - 3, 2.0));
        b(row) = 0.0;
        ++row;
        for (qint32 i = 1; i < a.size() - 1; ++i) {
            pointX = a[i].x();
            const qint32 baseColumn = i * 4;
            if (a[i].isSetAsCorner()) {
                triplets.push_back(Triplet(row, baseColumn - 4, 6.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn - 3, 2.0));
                b(row) = 0.0;
                ++row;
                triplets.push_back(Triplet(row, baseColumn + 0, 6.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn + 1, 2.0));
                b(row) = 0.0;
                ++row;
            } else {
                pointXSquared = pointX * pointX;
                triplets.push_back(Triplet(row, baseColumn - 4, 3.0 * pointXSquared));
                triplets.push_back(Triplet(row, baseColumn - 3, 2.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn - 2, 1.0));
                triplets.push_back(Triplet(row, baseColumn + 0, -3.0 * pointXSquared));
                triplets.push_back(Triplet(row, baseColumn + 1, -2.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn + 2, -1.0));
                b(row) = 0.0;
                ++row;
                triplets.push_back(Triplet(row, baseColumn - 4, 6.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn - 3, 2.0));
                triplets.push_back(Triplet(row, baseColumn + 0, -6.0 * pointX));
                triplets.push_back(Triplet(row, baseColumn + 1, -2.0));
                b(row) = 0.0;
                ++row;
            }
        }
        A.setFromTriplets(triplets.begin(), triplets.end());
        Eigen::SparseLU<Matrix> solver(A);
        Vector x = solver.solve(b);
        for (qint32 i = 0; i < intervals; ++i) {
            row = i * 4;
            m_coefficients.append({x(row), x(row + 1), x(row + 2), x(row + 3)});
        }
    }

    T getValue(T x) const {
        KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(m_coefficients.size() > 0, 0.0);
        int interval;
        for (interval = 0; interval < m_coefficients.size() - 1; ++interval) {
            if (x < m_points[interval + 1].x()) {
                break;
            }
        }
        const T xSquared = x * x;
        const T xCubed = xSquared * x;
        const Coefficients& coefficients = m_coefficients[interval];
        return coefficients.a * xCubed + coefficients.b * xSquared +
               coefficients.c * x + coefficients.d;
    }

private:
    struct Coefficients
    {
        T a, b, c, d;
    };

    QList<T_point> m_points;
    QList<Coefficients> m_coefficients;
};

#endif
