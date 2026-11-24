/*
 *  SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_JPEG_CONVERTER_H_
#define _KIS_JPEG_CONVERTER_H_

#include <stdio.h>

extern "C" {
#include <jpeglib.h>
}

#include <QColor>

#include "kis_types.h"
#include "kis_annotation.h"
#include <KisImportExportErrorCode.h>
class KisDocument;

namespace KisMetaData
{
class Filter;
}

struct KisJPEGOptions {
    int quality;
    bool progressive;
    bool optimize;
    int smooth;
    bool baseLineJPEG;
    int subsampling;
    bool exif;
    bool iptc;
    bool xmp;
    QList<const KisMetaData::Filter*> filters;
    QColor transparencyFillColor;
    bool forceSRGB;
    bool saveProfile;
    bool storeDocumentMetaData;
    bool storeAuthor;
    int smartLevel = 0;
    int smartProtect = 0;
    int smartSensitivity = 35;
    bool smartForceGray = false;
    bool smartGrayscale = false;
};

namespace KisMetaData
{
class Store;
}

class KisJPEGConverter : public QObject
{
    Q_OBJECT
public:
    KisJPEGConverter(KisDocument *doc, bool batchMode = false);
    ~KisJPEGConverter() override;
public:
    KisImportExportErrorCode buildImage(QIODevice *io);
    KisImportExportErrorCode buildFile(QIODevice *io, KisPaintLayerSP layer, KisJPEGOptions options, KisMetaData::Store* metaData);
    KisImageSP image();
public Q_SLOTS:
    virtual void cancel();
private:
    KisImportExportErrorCode decode(QIODevice *io);
private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
