/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tga_import.h"

#include <QCheckBox>
#include <QBuffer>
#include <QSlider>
#include <QApplication>

#include <kpluginfactory.h>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <kis_transaction.h>
#include <kis_paint_device.h>
#include <KisDocument.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_node.h>
#include <kis_group_layer.h>

#include <tga.h>

K_PLUGIN_FACTORY_WITH_JSON(KisTGAImportFactory, "krita_tga_import.json", registerPlugin<KisTGAImport>();)

KisTGAImport::KisTGAImport(QObject *parent, const QVariantList &)
    : KisImportExportFilter(parent)
{
}

KisTGAImport::~KisTGAImport()
{
}

static QDataStream & operator>> (QDataStream & s, TgaHeader & head)
{
    s >> head.id_length;
    s >> head.colormap_type;
    s >> head.image_type;
    s >> head.colormap_index;
    s >> head.colormap_length;
    s >> head.colormap_size;
    s >> head.x_origin;
    s >> head.y_origin;
    s >> head.width;
    s >> head.height;
    s >> head.pixel_size;
    s >> head.flags;

    return s;
}

static bool isSupported(const TgaHeader & head)
{
    if (head.image_type != TGA_TYPE_INDEXED &&
            head.image_type != TGA_TYPE_RGB &&
            head.image_type != TGA_TYPE_GREY &&
            head.image_type != TGA_TYPE_RLE_INDEXED &&
            head.image_type != TGA_TYPE_RLE_RGB &&
            head.image_type != TGA_TYPE_RLE_GREY) {
        return false;
    }

    if (head.image_type == TGA_TYPE_INDEXED ||
            head.image_type == TGA_TYPE_RLE_INDEXED) {
        if (head.colormap_length > 256 || head.colormap_size != 24 || head.colormap_type != 1) {
            return false;
        }
    }

    if (head.image_type == TGA_TYPE_RGB ||
            head.image_type == TGA_TYPE_GREY ||
            head.image_type == TGA_TYPE_RLE_RGB ||
            head.image_type == TGA_TYPE_RLE_GREY) {
        if (head.colormap_type != 0) {
            return false;
        }
    }

    if (head.width == 0 || head.height == 0) {
        return false;
    }

    if (head.pixel_size != 8 && head.pixel_size != 16 &&
            head.pixel_size != 24 && head.pixel_size != 32) {
        return false;
    }

    return true;
}

static bool loadTGA(QDataStream & s, const TgaHeader & tga, QImage &img)
{
    img = QImage(tga.width, tga.height, QImage::Format_RGB32);

    TgaHeaderInfo info(tga);

    const bool alphaFlag = tga.flags & 0xf;
    if (tga.pixel_size == 32 && !alphaFlag) {
        qWarning() << "WARNING: TGA image with 32-bit pixel size reports absence of alpha channel. It is not possible, fixing...";
    }

    if (tga.pixel_size == 32 || tga.pixel_size == 16) {
        img = QImage(tga.width, tga.height, QImage::Format_ARGB32);
    }

    uint pixel_size = (tga.pixel_size / 8);
    uint size = tga.width * tga.height * pixel_size;

    if (size < 1) {
        dbgFile << "This TGA file is broken with size " << size;
        return false;
    }

    char palette[768];
    if (info.pal) {
        s.readRawData(palette, 3 * tga.colormap_length);
    }

    uchar * const image = new uchar[size];

    if (info.rle) {
        char * dst = (char *)image;
        int num = size;

        while (num > 0) {
            uchar c;
            s >> c;

            uint count = (c & 0x7f) + 1;
            num -= count * pixel_size;

            if (num < 0) {
                dbgFile << "This TGA file is broken: the number of pixels left to read and the number of RLE pixels do not agree" << ppVar(num) << ppVar(count) << ppVar(pixel_size);
                return false;
            }

            if (c & 0x80) {
                Q_ASSERT(pixel_size <= 8);
                char pixel[8];
                s.readRawData(pixel, pixel_size);
                do {
                    memcpy(dst, pixel, pixel_size);
                    dst += pixel_size;
                } while (--count);
            } else {
                count *= pixel_size;
                s.readRawData(dst, count);
                dst += count;
            }
        }
    } else {
        s.readRawData((char *)image, size);
    }

    int y_start, y_step, y_end;
    if (tga.flags & TGA_ORIGIN_UPPER) {
        y_start = 0;
        y_step = 1;
        y_end = tga.height;
    } else {
        y_start = tga.height - 1;
        y_step = -1;
        y_end = -1;
    }

    uchar* src = image;

    bool hasAlpha = false;
    for (int y = y_start; y != y_end; y += y_step) {
        QRgb * scanline = (QRgb *) (void*) img.scanLine(y);

        if (info.pal) {
            for (int x = 0; x < tga.width; x++) {
                uchar idx = *src++;
                scanline[x] = qRgb(palette[3 * idx + 2], palette[3 * idx + 1], palette[3 * idx + 0]);
            }
        } else if (info.grey) {
            for (int x = 0; x < tga.width; x++) {
                scanline[x] = qRgb(*src, *src, *src);
                src++;
            }
        } else {
            if (tga.pixel_size == 16) {
                for (int x = 0; x < tga.width; x++) {
                    Color555 c = *reinterpret_cast<Color555 *>(src);
                    scanline[x] = qRgb((c.r << 3) | (c.r >> 2), (c.g << 3) | (c.g >> 2), (c.b << 3) | (c.b >> 2));
                    src += 2;
                }
            } else if (tga.pixel_size == 24) {
                for (int x = 0; x < tga.width; x++) {
                    scanline[x] = qRgb(src[2], src[1], src[0]);
                    src += 3;
                }
            } else if (tga.pixel_size == 32) {
                for (int x = 0; x < tga.width; x++) {
                    const uchar alpha = src[3];
                    scanline[x] = qRgba(src[2], src[1], src[0], alpha);
                    src += 4;
                    hasAlpha |= (alpha > 0);
                }
            }
        }
    }
    if (!hasAlpha && tga.pixel_size == 32) {
        img.convertTo(QImage::Format_RGB32);
        qWarning() << "WARNING: TGA image with 32-bit has all pixels transparent, removing alpha information.";
    }

    delete []image;

    return true;
}

KisImportExportErrorCode KisTGAImport::convert(KisDocument *document, QIODevice *io,  KisPropertiesConfigurationSP configuration)
{
    Q_UNUSED(configuration);
    QDataStream s(io);
    s.setByteOrder(QDataStream::LittleEndian);

    TgaHeader tga;
    s >> tga;
    s.device()->seek(TgaHeader::SIZE + tga.id_length);

    if (s.atEnd()) {
        return ImportExportCodes::FileFormatIncorrect;
    }

    if (!isSupported(tga)) {
        return ImportExportCodes::FormatFeaturesUnsupported;
    }

    QImage img;
    bool result = loadTGA(s, tga, img);

    if (result == false) {
        return ImportExportCodes::FileFormatIncorrect;
    }

    const KoColorSpace *colorSpace = KoColorSpaceRegistry::instance()->rgb8();
    KisImageSP image = new KisImage(document->createUndoStore(), img.width(), img.height(), colorSpace, "imported from tga");

    KisPaintLayerSP layer = new KisPaintLayer(image, image->nextLayerName(), 255);
    layer->paintDevice()->convertFromQImage(img, 0, 0, 0);
    image->addNode(layer.data(), image->rootLayer().data());

    document->setCurrentImage(image);
    return ImportExportCodes::OK;

}

#include "kis_tga_import.moc"

