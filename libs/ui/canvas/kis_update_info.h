/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_UPDATE_INFO_H_
#define KIS_UPDATE_INFO_H_

#include <QPainter>

#include "kis_image_patch.h"
#include "kis_shared.h"
#include "kritaui_export.h"
#include "opengl/kis_texture_tile_update_info.h"

#include "kis_ui_types.h"

class KRITAUI_EXPORT KisUpdateInfo : public KisShared
{
public:
    KisUpdateInfo();
    virtual ~KisUpdateInfo();

    virtual QRect dirtyViewportRect();
    virtual QRect dirtyImageRect() const = 0;
    virtual int levelOfDetail() const = 0;
    virtual bool canBeCompressed() const;
};

Q_DECLARE_METATYPE(KisUpdateInfoSP)

struct ConversionOptions {
    ConversionOptions() : m_needsConversion(false) {}
    ConversionOptions(const KoColorSpace *destinationColorSpace,
                      KoColorConversionTransformation::Intent renderingIntent,
                      KoColorConversionTransformation::ConversionFlags conversionFlags)
        : m_needsConversion(true),
          m_destinationColorSpace(destinationColorSpace),
          m_renderingIntent(renderingIntent),
          m_conversionFlags(conversionFlags)
    {
    }

    bool m_needsConversion {false};
    const KoColorSpace *m_destinationColorSpace {0};
    KoColorConversionTransformation::Intent m_renderingIntent {KoColorConversionTransformation::IntentPerceptual};
    KoColorConversionTransformation::ConversionFlags m_conversionFlags {KoColorConversionTransformation::Empty};
};

class KisOpenGLUpdateInfo;
typedef KisSharedPtr<KisOpenGLUpdateInfo> KisOpenGLUpdateInfoSP;

class KRITAUI_EXPORT KisOpenGLUpdateInfo : public KisUpdateInfo
{
public:
    KisOpenGLUpdateInfo();

    KisTextureTileUpdateInfoSPList tileList;

    QRect dirtyViewportRect() override;
    QRect dirtyImageRect() const override;

    void assignDirtyImageRect(const QRect &rect);
    void assignLevelOfDetail(int lod);

    int levelOfDetail() const override;

    bool tryMergeWith(const KisOpenGLUpdateInfo& rhs);

private:
    QRect m_dirtyImageRect;
    int m_levelOfDetail;
};

class KisPPUpdateInfo : public KisUpdateInfo
{
public:
    enum TransferType {
        DIRECT,
        PATCH
    };

    QRect dirtyViewportRect() override;
    QRect dirtyImageRect() const override;
    int levelOfDetail() const override;

    QRect dirtyImageRectVar;

    QRect imageRect;

    QRectF viewportRect;

    qreal scaleX;
    qreal scaleY;

    TransferType transfer;

    QPainter::RenderHints renderHints;

    qint32 borderWidth;

    KisImagePatch patch;
};

class KisMarkerUpdateInfo : public KisUpdateInfo
{
public:
    enum Type {
        StartBatch = 0,
        EndBatch,
        BlockLodUpdates,
        UnblockLodUpdates,
    };

public:
    KisMarkerUpdateInfo(Type type, const QRect &dirtyImageRect);

    Type type() const;

    QRect dirtyImageRect() const override;
    int levelOfDetail() const override;
    bool canBeCompressed() const override;

private:
    Type m_type;
    QRect m_dirtyImageRect;
};

#endif
