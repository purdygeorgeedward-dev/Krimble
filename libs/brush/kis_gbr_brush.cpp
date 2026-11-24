/*
 *  SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
 *  SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <sys/types.h>
#include <QtEndian>

#include "kis_gbr_brush.h"

#include <QDomElement>
#include <QFile>
#include <QImage>
#include <QPoint>

#include <kis_debug.h>
#include <klocalizedstring.h>

#include <KoColor.h>
#include <KoColorSpaceRegistry.h>

#include "kis_datamanager.h"
#include "kis_paint_device.h"
#include "kis_global.h"
#include "kis_image.h"

struct GimpBrushV1Header {
    quint32 header_size;
    quint32 version;
    quint32 width;
    quint32 height;
    quint32 bytes;
};

struct GimpBrushHeader {
    quint32 header_size;
    quint32 version;
    quint32 width;
    quint32 height;
    quint32 bytes;

    quint32 magic_number;
    quint32 spacing;
};

quint32 const GimpV2BrushMagic = ('G' << 24) + ('I' << 16) + ('M' << 8) + ('P' << 0);

struct KisGbrBrush::Private {

    QByteArray data;
    quint32 header_size;
    quint32 version;
    quint32 bytes;
    quint32 magic_number;
};

#define DEFAULT_SPACING 0.25

KisGbrBrush::KisGbrBrush(const QString& filename)
    : KisColorfulBrush(filename)
    , d(new Private)
{
    setSpacing(DEFAULT_SPACING);
}

KisGbrBrush::KisGbrBrush(const QString& filename,
                         const QByteArray& data,
                         qint32 & dataPos)
    : KisColorfulBrush(filename)
    , d(new Private)
{
    setSpacing(DEFAULT_SPACING);

    d->data = QByteArray::fromRawData(data.data() + dataPos, data.size() - dataPos);
    init();
    d->data.clear();
    dataPos += d->header_size + (width() * height() * d->bytes);
}

KisGbrBrush::KisGbrBrush(KisPaintDeviceSP image, int x, int y, int w, int h)
    : KisColorfulBrush()
    , d(new Private)
{
    setSpacing(DEFAULT_SPACING);
    initFromPaintDev(image, x, y, w, h);
}

KisGbrBrush::KisGbrBrush(const QImage& image, const QString& name)
    : KisColorfulBrush()
    , d(new Private)
{
    setSpacing(DEFAULT_SPACING);

    setBrushTipImage(image);
    setName(name);
}

KisGbrBrush::KisGbrBrush(const KisGbrBrush& rhs)
    : KisColorfulBrush(rhs)
    , d(new Private(*rhs.d))
{
    d->data = QByteArray();
}

KoResourceSP KisGbrBrush::clone() const
{
    return KoResourceSP(new KisGbrBrush(*this));
}

KisGbrBrush::~KisGbrBrush()
{
    delete d;
}

bool KisGbrBrush::loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);
    d->data = dev->readAll();
    return init();
}

bool KisGbrBrush::init()
{
    GimpBrushHeader bh;

    if (sizeof(GimpBrushHeader) > (uint)d->data.size()) {
        qWarning() << filename() << "GBR could not be loaded: expected header size larger than bytearray size. Header Size:" << sizeof(GimpBrushHeader) << "Byte array size" << d->data.size();
        return false;
    }

    memcpy(&bh, d->data.constData(), sizeof(GimpBrushHeader));
    bh.header_size = qFromBigEndian(bh.header_size);
    d->header_size = bh.header_size;

    bh.version = qFromBigEndian(bh.version);
    d->version = bh.version;

    bh.width = qFromBigEndian(bh.width);
    bh.height = qFromBigEndian(bh.height);

    bh.bytes = qFromBigEndian(bh.bytes);
    d->bytes = bh.bytes;

    bh.magic_number = qFromBigEndian(bh.magic_number);
    d->magic_number = bh.magic_number;

    if (bh.version == 1) {
        bh.spacing = static_cast<int>(DEFAULT_SPACING * 100);
    }
    else {
        bh.spacing = qFromBigEndian(bh.spacing);

        if (bh.spacing > 1000) {
            qWarning() << filename()  << "GBR could not be loaded, spacing above 1000. Spacing:" << bh.spacing;
            return false;
        }
    }

    setSpacing(bh.spacing / 100.0);

    if (bh.header_size > (uint)d->data.size() || bh.header_size == 0) {
        qWarning() << "GBR could not be loaded: header size larger than bytearray size. Header Size:" << bh.header_size << "Byte array size" << d->data.size();
        return false;
    }

    QString name;

    if (bh.version == 1) {
        const char *text = d->data.constData() + sizeof(GimpBrushV1Header);
        name = QString::fromLatin1(text, bh.header_size - sizeof(GimpBrushV1Header) - 1);
    }
    else {
        name = QString::fromUtf8(d->data.constData() + sizeof(GimpBrushHeader),
                                 bh.header_size - sizeof(GimpBrushHeader) - 1);
    }

    setName(name);

    if (bh.width == 0 || bh.height == 0) {
        qWarning() << filename()  << "GBR loading failed: width or height is 0" << bh.width << bh.height;
        return false;
    }

    QImage::Format imageFormat;

    if (bh.bytes == 1) {
        imageFormat = QImage::Format_Indexed8;
    } else {
        imageFormat = QImage::Format_ARGB32;
    }

    QImage image(QImage(bh.width, bh.height, imageFormat));

    if (image.isNull()) {
        qWarning() << filename()  << "GBR loading failed; image could not be created from following dimensions" << bh.width << bh.height
                   << "QImage::Format" << imageFormat;
        return false;
    }

    qint32 k = bh.header_size;

    if (bh.bytes == 1) {
        QVector<QRgb> table;
        for (int i = 0; i < 256; ++i) table.append(qRgb(i, i, i));
        image.setColorTable(table);

        if (static_cast<qint32>(k + bh.width * bh.height) > d->data.size()) {
            qWarning() << filename()  << "GBR file dimensions bigger than bytearray size. Header:"<< k << "Width:" << bh.width << "height" << bh.height
                       << "expected byte array size:" << (k + (bh.width * bh.height)) << "actual byte array size" << d->data.size();
            return false;
        }

        setBrushApplication(ALPHAMASK);
        setBrushType(MASK);
        setHasColorAndTransparency(false);

        for (quint32 y = 0; y < bh.height; y++) {
            uchar *pixel = reinterpret_cast<uchar *>(image.scanLine(y));
            for (quint32 x = 0; x < bh.width; x++, k++) {
                qint32 val = 255 - static_cast<uchar>(d->data.at(k));
                *pixel = val;
                ++pixel;
            }
        }
    } else if (bh.bytes == 4) {

        if (static_cast<qint32>(k + (bh.width * bh.height * 4)) > d->data.size()) {
            qWarning() << filename()  << "GBR file dimensions bigger than bytearray size. Header:"<< k << "Width:" << bh.width << "height" << bh.height
                       << "expected byte array size:" << (k + (bh.width * bh.height * 4)) << "actual byte array size" << d->data.size();
            return false;
        }

        setBrushApplication(LIGHTNESSMAP);
        setBrushType(IMAGE);

        for (quint32 y = 0; y < bh.height; y++) {
            QRgb *pixel = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (quint32 x = 0; x < bh.width; x++, k += 4) {
                *pixel = qRgba(d->data.at(k), d->data.at(k + 1), d->data.at(k + 2), d->data.at(k + 3));
                ++pixel;
            }
        }

        setHasColorAndTransparency(!image.allGray());
    }
    else {
        warnKrita << filename()  << "WARNING: loading of GBR brushes with" << bh.bytes << "bytes per pixel is not supported";
        return false;
    }

    setWidth(image.width());
    setHeight(image.height());
    if (!d->data.isEmpty()) {
        d->data.clear();
    }
    setValid(image.width() != 0 && image.height() != 0);
    setBrushTipImage(image);
    return true;
}

bool KisGbrBrush::initFromPaintDev(KisPaintDeviceSP image, int x, int y, int w, int h)
{
    setBrushTipImage(image->convertToQImage(0, x, y, w, h, KoColorConversionTransformation::internalRenderingIntent(), KoColorConversionTransformation::internalConversionFlags()));
    setName(image->objectName());

    setBrushType(IMAGE);
    setBrushApplication(LIGHTNESSMAP);

    return true;
}

bool KisGbrBrush::saveToDevice(QIODevice* dev) const
{
    if (!valid() || brushTipImage().isNull()) {
        qWarning() << "this brush is not valid, set a brush tip image" << filename();
        return false;
    }
    GimpBrushHeader bh;
    QByteArray utf8Name = name().toUtf8();
    char const* name = utf8Name.data();
    int nameLength = qstrlen(name);
    int wrote;

    bh.header_size = qToBigEndian((quint32)sizeof(GimpBrushHeader) + nameLength + 1);
    bh.version = qToBigEndian((quint32)2);
    bh.width = qToBigEndian((quint32)width());
    bh.height = qToBigEndian((quint32)height());
    if (!isImageType()) {
        bh.bytes = qToBigEndian((quint32)1);
    }
    else {
        bh.bytes = qToBigEndian((quint32)4);
    }
    bh.magic_number = qToBigEndian((quint32)GimpV2BrushMagic);
    bh.spacing = qToBigEndian(static_cast<quint32>(spacing() * 100.0));

    QByteArray bytes = QByteArray::fromRawData(reinterpret_cast<char*>(&bh), sizeof(GimpBrushHeader));
    wrote = dev->write(bytes);
    bytes.clear();

    if (wrote == -1) {
        return false;
    }

    wrote = dev->write(name, nameLength + 1);

    if (wrote == -1) {
        return false;
    }

    int k = 0;

    QImage image = brushTipImage();

    if (!isImageType()) {
        bytes.resize(width() * height());
        for (qint32 y = 0; y < height(); y++) {
            for (qint32 x = 0; x < width(); x++) {
                QRgb c = image.pixel(x, y);
                bytes[k++] = static_cast<char>(255 - qRed(c));
            }
        }
    } else {
        bytes.resize(width() * height() * 4);
        for (qint32 y = 0; y < height(); y++) {
            for (qint32 x = 0; x < width(); x++) {
                QRgb pixel = image.pixel(x, y);
                bytes[k++] = static_cast<char>(qRed(pixel));
                bytes[k++] = static_cast<char>(qGreen(pixel));
                bytes[k++] = static_cast<char>(qBlue(pixel));
                bytes[k++] = static_cast<char>(qAlpha(pixel));
            }
        }
    }

    wrote = dev->write(bytes);
    if (wrote == -1) {
        return false;
    }
    return true;
}

void KisGbrBrush::setBrushTipImage(const QImage& image)
{
    KisBrush::setBrushTipImage(image);
    setValid(true);
}

void KisGbrBrush::makeMaskImage(bool preserveAlpha)
{
    if (!isImageType()) {
        return;
    }

    QImage brushTip = brushTipImage();

    if (!preserveAlpha) {
        const int imageWidth = brushTip.width();
        const int imageHeight = brushTip.height();
        QImage image(imageWidth, imageHeight, QImage::Format_Indexed8);
        QVector<QRgb> table;
        for (int i = 0; i < 256; ++i) {
            table.append(qRgb(i, i, i));
        }
        image.setColorTable(table);

        for (int y = 0; y < imageHeight; y++) {
            QRgb *pixel = reinterpret_cast<QRgb *>(brushTip.scanLine(y));
            uchar * dstPixel = image.scanLine(y);
            for (int x = 0; x < imageWidth; x++) {
                QRgb c = pixel[x];
                float alpha = qAlpha(c) / 255.0f;
                int a = 255 + int(alpha * (qGray(c) - 255));
                dstPixel[x] = (uchar)a;
            }
        }
        setBrushTipImage(image);
        setBrushType(MASK);
    }
    else {
        setBrushTipImage(brushTip);
        setBrushType(IMAGE);
    }

    setBrushApplication(ALPHAMASK);
    resetOutlineCache();
    clearBrushPyramid();
}

void KisGbrBrush::toXML(QDomDocument& d, QDomElement& e) const
{
    predefinedBrushToXML("gbr_brush", e);
    KisColorfulBrush::toXML(d, e);
}

QString KisGbrBrush::defaultFileExtension() const
{
    return QString(".gbr");
}
