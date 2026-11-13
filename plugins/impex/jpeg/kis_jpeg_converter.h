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
    bool storeDocumentMetaData; //this is for getting the metadata from the document info.
    bool storeAuthor; //this is for storing author data from the document info.
    int smartLevel = 0;            // Krimble 2026-11-12 (task 24a): George: "User should be able to select the level of SmartCompression. Also should have a way to select areas to mask so they won't be compressed, or total unmask to apply compression." and "But you start with the smart selection as the mask." 0 = SmartCompression off, up to 100
    int smartProtect = 0;          // 0 = Smart Selection (automatic), 1 = the selection of the picture, 2 = nothing protected
    int smartSensitivity = 35;     // how much the Smart Selection protects, 0 to 100
    bool smartForceGray = false;   // Krimble 2026-11-13 (task 49): George (build 57): "SmartCompression logo is too big. Too much explanation in the panel. Doesn't look like XAT. Jpeg save dialog is enormous." the Grayscale check box: a color picture is saved as a gray JPEG
    bool smartGrayscale = false;   // Krimble 2026-11-12 (task 24c): George: "Do SmartCompression now" (SmartCompression: a new tab in the JPEG save dialog; part c first: a picture that is all gray is saved as a one-channel gray JPEG).
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
    /** Retrieve the constructed image
    */
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
