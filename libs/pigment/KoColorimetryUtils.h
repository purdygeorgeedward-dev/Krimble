/*
 *  SPDX-FileCopyrightText: 2023 Xaver Hugl <xaver.hugl@gmail.com>
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISCOLORIMETRYUTILS_H
#define KISCOLORIMETRYUTILS_H

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <boost/operators.hpp>
#include <QDebug>

#include <kritapigment_export.h>

namespace KoColorimetryUtils
{

QMatrix4x4 matrixFromColumns(const QVector3D &first, const QVector3D &second, const QVector3D &third);

struct XYZ;

struct KRITAPIGMENT_EXPORT xy {
    double x;
    double y;

    XYZ toXYZ() const;
    QVector2D asVector() const;
    bool operator==(const xy &other) const;
};

struct KRITAPIGMENT_EXPORT xyY {
    double x;
    double y;
    double Y;

    XYZ toXYZ() const;
    bool operator==(const xyY &other) const;
};

struct KRITAPIGMENT_EXPORT XYZ {
    double X;
    double Y;
    double Z;

    xyY toxyY() const;
    xy toxy() const;
    QVector3D asVector() const;
    XYZ operator*(double factor) const;
    XYZ operator/(double factor) const;
    XYZ operator+(const XYZ &other) const;
    bool operator==(const XYZ &other) const;

    static XYZ fromVector(const QVector3D &vector);
};

class KRITAPIGMENT_EXPORT Colorimetry
{
public:
    static const Colorimetry BT709;
    static const Colorimetry PAL_M;
    static const Colorimetry PAL;
    static const Colorimetry NTSC;
    static const Colorimetry GenericFilm;
    static const Colorimetry BT2020;
    static const Colorimetry CIEXYZ;
    static const Colorimetry DCIP3;
    static const Colorimetry DisplayP3;
    static const Colorimetry AdobeRGB;

    static QMatrix4x4 chromaticAdaptationMatrix(XYZ sourceWhitepoint, XYZ destinationWhitepoint);

    static QMatrix4x4 calculateToXYZMatrix(XYZ red, XYZ green, XYZ blue, XYZ white);

    static bool isValid(xy red, xy green, xy blue, xy white);
    static bool isReal(xy red, xy green, xy blue, xy white);

    explicit Colorimetry(XYZ red, XYZ green, XYZ blue, XYZ white);
    explicit Colorimetry(xyY red, xyY green, xyY blue, xyY white);
    explicit Colorimetry(xy red, xy green, xy blue, xy white);

    const QMatrix4x4 &toXYZ() const;
    const QMatrix4x4 &fromXYZ() const;
    QMatrix4x4 toLMS() const;
    QMatrix4x4 fromLMS() const;

    bool operator==(const Colorimetry &other) const;
    Colorimetry adaptedTo(xyY newWhitepoint) const;
    Colorimetry withWhitepoint(xyY newWhitePoint) const;
    Colorimetry interpolateGamutTo(const Colorimetry &one, double factor) const;

    QMatrix4x4 relativeColorimetricTo(const Colorimetry &other) const;
    QMatrix4x4 absoluteColorimetricTo(const Colorimetry &other) const;

    const XYZ &red() const;
    const XYZ &green() const;
    const XYZ &blue() const;
    const XYZ &white() const;

private:
    XYZ m_red;
    XYZ m_green;
    XYZ m_blue;
    XYZ m_white;
    QMatrix4x4 m_toXYZ;
    QMatrix4x4 m_fromXYZ;
};

KRITAPIGMENT_EXPORT QDebug operator<<(QDebug debug, const xy &value);
KRITAPIGMENT_EXPORT QDebug operator<<(QDebug debug, const xyY &value);
KRITAPIGMENT_EXPORT QDebug operator<<(QDebug debug, const XYZ &value);
KRITAPIGMENT_EXPORT QDebug operator<<(QDebug debug, const Colorimetry &value);

}

#endif
