/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PSD_RESOURCE_BLOCK_H
#define PSD_RESOURCE_BLOCK_H

#include "kritapsd_export.h"

class QIODevice;

#include <QBuffer>
#include <QString>
#include <kis_debug.h>
#include <klocalizedstring.h>

#include <kis_annotation.h>

#include "psd.h"
#include "psd_resource_section.h"
#include "psd_utils.h"

class KRITAPSD_EXPORT PSDInterpretedResource
{
public:
    virtual ~PSDInterpretedResource()
    {
    }

    virtual bool interpretBlock(QByteArray )
    {
        return true;
    }
    virtual bool createBlock(QByteArray & )
    {
        return true;
    }
    virtual bool valid()
    {
        return true;
    }

    virtual QString displayText()
    {
        return QString();
    }

    QString error;

protected:
    void startBlock(QBuffer &buf, PSDImageResourceSection::PSDResourceID id, quint32 size)
    {
        if (!buf.isOpen()) {
            buf.open(QBuffer::WriteOnly);
        }
        buf.write("8BIM", 4);
        psdwrite(buf, (quint16)id);
        psdwrite(buf, (quint16)0);
        psdwrite(buf, (quint32)size);
    }
};

class KRITAPSD_EXPORT PSDResourceBlock : public KisAnnotation
{
public:
    PSDResourceBlock();

    ~PSDResourceBlock() override
    {
        delete resource;
    }

    KisAnnotation *clone() const Q_DECL_OVERRIDE
    {
        PSDResourceBlock *copied = new PSDResourceBlock();

        QBuffer buffer;
        buffer.open(QBuffer::WriteOnly);

        if (!write(buffer)) {
            qWarning() << "Could not copy PSDResourceBlock" << error;
            delete copied;
            return 0;
        }
        buffer.close();
        buffer.open(QBuffer::ReadOnly);

        if (!copied->read(buffer)) {
            qWarning() << "Could not copy PSDResourceBlock" << copied->error;
            delete copied;
            return 0;
        }

        return copied;
    }

    QString displayText() const override
    {
        if (resource) {
            return resource->displayText();
        }
        return i18n("Unparsed Resource Block");
    }

    bool read(QIODevice &io);
    bool write(QIODevice &io) const;
    bool valid();

    quint16 identifier;
    QString name;
    quint32 dataSize;
    QByteArray data;

    PSDInterpretedResource *resource;

    mutable QString error;
};

struct KRITAPSD_EXPORT MAC_PRINT_INFO_1001 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading MAC_PRINT_INFO_1001";
        return true;
    }
};

struct KRITAPSD_EXPORT RESN_INFO_1005 : public PSDInterpretedResource {
    enum PSDUnit {
        PSD_UNIT_INCH = 1,
        PSD_UNIT_CM = 2,
        PSD_UNIT_POINT = 3,
        PSD_UNIT_PICA = 4,
        PSD_UNIT_COLUMN = 5
    };

    RESN_INFO_1005()
        : hRes(300)
        , hResUnit(PSD_UNIT_INCH)
        , widthUnit(PSD_UNIT_INCH)
        , vRes(300)
        , vResUnit(PSD_UNIT_INCH)
        , heightUnit(PSD_UNIT_INCH)
    {
    }

    bool interpretBlock(QByteArray data) override;
    bool createBlock(QByteArray &data) override;

    Fixed hRes;
    quint16 hResUnit;
    quint16 widthUnit;
    Fixed vRes;
    quint16 vResUnit;
    quint16 heightUnit;
};

struct KRITAPSD_EXPORT ALPHA_NAMES_1006 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading ALPHA_NAMES_1006";

        return true;
    }
};

struct KRITAPSD_EXPORT DISPLAY_INFO_1007 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading DISPLAY_INFO_1007";
        return true;
    }
};

struct KRITAPSD_EXPORT CAPTION_1008 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading CAPTION_1008";
        return true;
    }
};

struct KRITAPSD_EXPORT BORDER_INFO_1009 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading BORDER_INFO_1009";
        return true;
    }
};

struct KRITAPSD_EXPORT BACKGROUND_COL_1010 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading BACKGROUND_COL_1010";
        return true;
    }
};

struct KRITAPSD_EXPORT PRINT_FLAGS_1011 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading PRINT_FLAGS_1011";
        return true;
    }
};

struct KRITAPSD_EXPORT GREY_HALFTONE_1012 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading GREY_HALFTONE_1012";
        return true;
    }
};

struct KRITAPSD_EXPORT COLOR_HALFTONE_1013 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading COLOR_HALFTONE_1013";
        return true;
    }
};

struct KRITAPSD_EXPORT DUOTONE_HALFTONE_1014 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading DUOTONE_HALFTONE_1014";
        return true;
    }
};

struct KRITAPSD_EXPORT GREY_XFER_1015 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading GREY_XFER_1015";
        return true;
    }
};

struct KRITAPSD_EXPORT COLOR_XFER_1016 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading COLOR_XFER_1016";
        return true;
    }
};

struct KRITAPSD_EXPORT DUOTONE_XFER_1017 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading DUOTONE_XFER_1017";
        return true;
    }
};

struct KRITAPSD_EXPORT DUOTONE_INFO_1018 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading DUOTONE_INFO_1018";
        return true;
    }
};

struct KRITAPSD_EXPORT EFFECTIVE_BW_1019 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading EFFECTIVE_BW_1019";
        return true;
    }
};

struct KRITAPSD_EXPORT EPS_OPT_1021 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading EPS_OPT_1021";
        return true;
    }
};

struct KRITAPSD_EXPORT QUICK_MASK_1022 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading QUICK_MASK_1022";
        return true;
    }
};

struct KRITAPSD_EXPORT LAYER_STATE_1024 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading LAYER_STATE_1024";
        return true;
    }
};

struct KRITAPSD_EXPORT WORKING_PATH_1025 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading WORKING_PATH_1025";
        return true;
    }
};

struct KRITAPSD_EXPORT LAYER_GROUP_1026 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading LAYER_GROUP_1026";
        return true;
    }
};

struct KRITAPSD_EXPORT IPTC_NAA_DATA_1028 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading IPTC_NAA_DATA_1028";
        return true;
    }
};

struct KRITAPSD_EXPORT IMAGE_MODE_RAW_1029 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading IMAGE_MODE_RAW_1029";
        return true;
    }
};

struct KRITAPSD_EXPORT JPEG_QUAL_1030 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading JPEG_QUAL_1030";
        return true;
    }
};

struct KRITAPSD_EXPORT GRID_GUIDE_1032 : public PSDInterpretedResource {
    GRID_GUIDE_1032()
        : version(1)
        , gridHorizontal(0)
        , gridVertical(0)
    {
    }

    bool interpretBlock(QByteArray data) override
    {
        dbgFile << "Reading GRID_GUIDE_1032";
        QDataStream ds(data);
        ds.setByteOrder(QDataStream::BigEndian);
        qint32 guidesLength;

        ds >> version >> gridHorizontal >> gridVertical >> guidesLength;

        for (qint32 i=0; i < guidesLength; i++) {
            quint32 guide;
            bool horizontal;
            ds >> guide >> horizontal;
            if (horizontal) {
                horizontalGuides.append(guide / documentMultiplier);
            } else {
                verticalGuides.append(guide / documentMultiplier);
            }
        }

        return ds.atEnd();
    }

    bool createBlock(QByteArray &data) override
    {
        QBuffer buf(&data);
        quint32 size = 16;
        size += (horizontalGuides.size() * 5);
        size += (verticalGuides.size() * 5);
        startBlock(buf, PSDImageResourceSection::GRID_GUIDE, size);
        psdwrite(buf, version);
        psdwrite(buf, (quint32)gridHorizontal);
        psdwrite(buf, (quint32)gridVertical);
        psdwrite(buf, quint32(horizontalGuides.size()+verticalGuides.size()));
        Q_FOREACH(quint32 guide, verticalGuides) {
            psdwrite(buf, quint32(guide * documentMultiplier));
            psdwrite(buf, false);
        }
        Q_FOREACH(quint32 guide, horizontalGuides) {
            psdwrite(buf, quint32(guide * documentMultiplier));
            psdwrite(buf, true);
        }
        buf.close();
        return true;
    }

    bool valid() override
    {
        return true;
    }

    QString displayText() override
    {
        QStringList guidesText;
        guidesText.append(QString("Grids and Guides version: %1").arg(version));
        guidesText.append(QString("Grids vertical: %1, horizontal: %2")
                          .arg(gridVertical).arg(gridHorizontal));
        QStringList vertical;
        QStringList horizontal;
        Q_FOREACH(quint32 guide, verticalGuides) {
            vertical.append(QString::number(guide));
        }
        Q_FOREACH(quint32 guide, horizontalGuides) {
            horizontal.append(QString::number(guide));
        }
        guidesText.append(QString("Vertical guides: %1").arg(vertical.join(", ")));
        guidesText.append(QString("Horizontal guides: %1").arg(horizontal.join(", ")));
        return guidesText.join("\n");
    }

    const int documentMultiplier{32};
    quint32 version;
    qint32 gridHorizontal;
    qint32 gridVertical;
    QList<quint32> horizontalGuides;
    QList<quint32> verticalGuides;
};

struct KRITAPSD_EXPORT THUMB_RES_1033 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading THUMB_RES_1033";
        return true;
    }
};

struct KRITAPSD_EXPORT COPYRIGHT_FLG_1034 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading COPYRIGHT_FLG_1034";
        return true;
    }
};

struct KRITAPSD_EXPORT URL_1035 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading URL_1035";
        return true;
    }
};

struct KRITAPSD_EXPORT THUMB_RES2_1036 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading THUMB_RES2_1036";
        return true;
    }
};

struct KRITAPSD_EXPORT GLOBAL_ANGLE_1037 : public PSDInterpretedResource {
    GLOBAL_ANGLE_1037()
        : angle(30)
    {
    }

    bool interpretBlock(QByteArray data) override
    {
        dbgFile << "Reading GLOBAL_ANGLE_1037";

        QDataStream ds(data);
        ds.setByteOrder(QDataStream::BigEndian);

        ds >> angle;

        return true;
    }

    bool createBlock(QByteArray &data) override
    {
        QBuffer buf(&data);
        startBlock(buf, PSDImageResourceSection::GLOBAL_ANGLE, 4);
        psdwrite(buf, (quint32)angle);
        return true;
    }

    bool valid() override
    {
        return true;
    }

    QString displayText() override
    {
        return QString("Global Angle: %1").arg(angle);
    }

    qint32 angle;
};

struct KRITAPSD_EXPORT COLOR_SAMPLER_1038 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading COLOR_SAMPLER_1038";
        return true;
    }
};

struct KRITAPSD_EXPORT ICC_PROFILE_1039 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray data) override;
    bool createBlock(QByteArray &data) override;

    QByteArray icc;
};

struct KRITAPSD_EXPORT WATERMARK_1040 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading WATERMARK_1040";
        return true;
    }
};

struct KRITAPSD_EXPORT ICC_UNTAGGED_1041 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading ICC_UNTAGGED_1041";
        return true;
    }
};

struct KRITAPSD_EXPORT EFFECTS_VISIBLE_1042 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading EFFECTS_VISIBLE_1042";
        return true;
    }
};

struct KRITAPSD_EXPORT SPOT_HALFTONE_1043 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading SPOT_HALFTONE_1043";
        return true;
    }
};

struct KRITAPSD_EXPORT DOC_IDS_1044 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading DOC_IDS_1044";
        return true;
    }
};

struct KRITAPSD_EXPORT ALPHA_NAMES_UNI_1045 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading ALPHA_NAMES_UNI_1045";
        return true;
    }
};

struct KRITAPSD_EXPORT IDX_COL_TAB_CNT_1046 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading IDX_COL_TAB_CNT_1046";
        return true;
    }
};

struct KRITAPSD_EXPORT IDX_TRANSPARENT_1047 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading IDX_TRANSPARENT_1047";
        return true;
    }
};

struct KRITAPSD_EXPORT GLOBAL_ALT_1049 : public PSDInterpretedResource {
    GLOBAL_ALT_1049()
        : altitude(30)
    {
    }
    bool interpretBlock(QByteArray data) override
    {
        dbgFile << "Reading GLOBAL_ALT_1049";
        QDataStream ds(data);
        ds.setByteOrder(QDataStream::BigEndian);
        ds >> altitude;
        return true;
    }

    bool createBlock(QByteArray &data) override
    {
        QBuffer buf(&data);
        startBlock(buf, PSDImageResourceSection::GLOBAL_ALT, 4);
        psdwrite(buf, (quint32)altitude);
        return true;
    }

    bool valid() override
    {
        return true;
    }

    QString displayText() override
    {
        return QString("Global Altitude: %1").arg(altitude);
    }

    qint32 altitude;
};

struct KRITAPSD_EXPORT SLICES_1050 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading SLICES_1050";
        return true;
    }
};

struct KRITAPSD_EXPORT WORKFLOW_URL_UNI_1051 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading WORKFLOW_URL_UNI_1051";
        return true;
    }
};

struct KRITAPSD_EXPORT JUMP_TO_XPEP_1052 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "JUMP_TO_XPEP_1052";
        return true;
    }
};

struct KRITAPSD_EXPORT ALPHA_ID_1053 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "ALPHA_ID_1053";
        return true;
    }
};

struct KRITAPSD_EXPORT URL_LIST_UNI_1054 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "URL_LIST_UNI_1054";
        return true;
    }
};

struct KRITAPSD_EXPORT VERSION_INFO_1057 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "VERSION_INFO_1057";
        return true;
    }
};

struct KRITAPSD_EXPORT EXIF_DATA_1058 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading EXIF_DATA_1058";
        return true;
    }
};

struct KRITAPSD_EXPORT XMP_DATA_1060 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading XMP_DATA_1060";
        return true;
    }
};

struct KRITAPSD_EXPORT PATH_INFO_FIRST_2000 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "PATH_INFO_FIRST_2000";
        return true;
    }
};

struct KRITAPSD_EXPORT PATH_INFO_LAST_2998 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "PATH_INFO_LAST_2998";
        return true;
    }
};

struct KRITAPSD_EXPORT CLIPPING_PATH_2999 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading CLIPPING_PATH_2999";
        return true;
    }
};

struct KRITAPSD_EXPORT PRINT_FLAGS_2_10000 : public PSDInterpretedResource {
    bool interpretBlock(QByteArray ) override
    {
        dbgFile << "Reading PRINT_FLAGS_2_10000";
        return true;
    }
};

#endif
