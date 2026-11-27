/*
 *  SPDX-FileCopyrightText: 2005, 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_PNG_CONVERTER_H_
#define _KIS_PNG_CONVERTER_H_

#include <png.h>

#include <QColor>

#include "kis_types.h"
#include "kis_global.h"
#include "kis_annotation.h"
#include <kritaui_export.h>
#include <KisImportExportErrorCode.h>

class KoStore;
class KisDocument;
class KoColorSpace;

namespace KisMetaData
{
class Filter;
class Store;
}

struct KisPNGOptions {
    KisPNGOptions()
        : compression(0)
        , interlace(false)
        , alpha(true)
        , exif(true)
        , iptc(true)
        , xmp(true)
        , tryToSaveAsIndexed(true)
        , saveSRGBProfile(false)
        , forceSRGB(false)
        , storeMetaData(false)
        , storeAuthor(false)
        , saveAsHDR(false)
        , transparencyFillColor(Qt::white)
        , downsample(false)
    {}

    int compression;
    bool interlace;
    bool alpha;
    bool exif;
    bool iptc;
    bool xmp;
    bool tryToSaveAsIndexed;
    bool saveSRGBProfile;
    bool forceSRGB;
    bool storeMetaData;
    bool storeAuthor;
    bool saveAsHDR;
    QList<const KisMetaData::Filter*> filters;
    QColor transparencyFillColor;
    bool downsample;
};

class KRITAUI_EXPORT KisPNGConverter : public QObject
{
    Q_OBJECT
public:
    KisPNGConverter(KisDocument *doc, bool batchMode = false);
    ~KisPNGConverter() override;
public:
    KisImportExportErrorCode buildImage(const QString &filename);
    KisImportExportErrorCode buildImage(QIODevice* iod);
    KisImportExportErrorCode buildFile(const QString &filename, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP device, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, KisPNGOptions options, KisMetaData::Store* metaData);
    KisImportExportErrorCode buildFile(QIODevice*, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP device, vKisAnnotationSP_it annotationsStart, vKisAnnotationSP_it annotationsEnd, KisPNGOptions options, KisMetaData::Store* metaData);
    KisImageSP image();

    static bool saveDeviceToStore(const QString &filename, const QRect &imageRect, const qreal xRes, const qreal yRes, KisPaintDeviceSP dev, KoStore *store, KisMetaData::Store* metaData = 0);

    static bool isColorSpaceSupported(const KoColorSpace *cs);

public Q_SLOTS:
    virtual void cancel();
private:
    void progress(png_structp png_ptr, png_uint_32 row_number, int pass);
private:
    png_uint_32 m_max_row;
    KisImageSP m_image;
    KisDocument *m_doc;
    bool m_stop;
    bool m_batchMode;
    QString m_path;
};

#endif
