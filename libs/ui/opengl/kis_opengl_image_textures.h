/*
 *  SPDX-FileCopyrightText: 2005-2007 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_OPENGL_IMAGE_TEXTURES_H_
#define KIS_OPENGL_IMAGE_TEXTURES_H_

#include <QVector>
#include <QMap>
#include <QOpenGLFunctions>

#include "kritaui_export.h"

#include "kis_shared.h"

#include "canvas/kis_update_info.h"
#include "opengl/kis_texture_tile.h"
#include "KisOpenGLUpdateInfoBuilder.h"
#include "KisOpenGLBufferCircularStorage.h"

class KisOpenGLImageTextures;
typedef KisSharedPtr<KisOpenGLImageTextures> KisOpenGLImageTexturesSP;

class KoColorProfile;
class KisTextureTileUpdateInfoPoolCollection;
typedef QSharedPointer<KisTextureTileInfoPool> KisTextureTileInfoPoolSP;

class KisProofingConfiguration;
typedef QSharedPointer<KisProofingConfiguration> KisProofingConfigurationSP;

class KRITAUI_EXPORT KisOpenGLImageTextures : public QObject, public KisShared
{
    Q_OBJECT
public:
    static KisOpenGLImageTexturesSP createImageTextures(KisImageWSP image,
                                                     const KoColorProfile *monitorProfile, KoColorConversionTransformation::Intent renderingIntent,
                                                     KoColorConversionTransformation::ConversionFlags conversionFlags);

    KisOpenGLImageTextures();

    virtual ~KisOpenGLImageTextures();

    KisImageSP image() const;

    void setMonitorProfile(const KoColorProfile *monitorProfile,
                           KoColorConversionTransformation::Intent renderingIntent,
                           KoColorConversionTransformation::ConversionFlags conversionFlags);

    bool setImageColorSpace(const KoColorSpace *cs);

    void initGL(QOpenGLFunctions *f);

    void setChannelFlags(const QBitArray &channelFlags);
    void setProofingConfig(KisProofingConfigurationSP);

    bool internalColorManagementActive() const;
    bool setInternalColorManagementActive(bool value, bool iniializing = false);

    static const int BACKGROUND_TEXTURE_CHECK_SIZE = 32;
    static const int BACKGROUND_TEXTURE_SIZE = BACKGROUND_TEXTURE_CHECK_SIZE * 2;

    void generateCheckerTexture(const QImage & checkImage);
    GLuint checkerTexture();

    void updateConfig(bool useBuffer, int NumMipmapLevels);

    void testingForceInitialized();

public:
    inline QRect storedImageBounds() {
        return m_storedImageBounds;
    }

    inline int xToCol(int x) {
        return x / m_texturesInfo.effectiveWidth;
    }

    inline int yToRow(int y) {
        return y / m_texturesInfo.effectiveHeight;
    }

    inline KisTextureTile* getTextureTileCR(int col, int row) {
        if (m_initialized) {
            int tile = row * m_numCols + col;
            KIS_ASSERT_RECOVER_RETURN_VALUE(m_textureTiles.size() > tile, 0);
            return m_textureTiles[tile];
        }
        return 0;
    }

    inline int getTextureBufferIndexCR(int col, int row) {
        if (m_initialized) {
            int tile = row * m_numCols + col;
            KIS_ASSERT_RECOVER_RETURN_VALUE(m_textureTiles.size() > tile, 0);
            return tile;
        }
        return -1;
    }

    QOpenGLBuffer* tileVertexBuffer() {
        return &m_tileVertexBuffer;
    }

    QOpenGLBuffer* tileTexCoordBuffer() {
        return &m_tileTexCoordBuffer;
    }

    inline qreal texelSize() const {
        Q_ASSERT(m_texturesInfo.width == m_texturesInfo.height);
        return 1.0 / m_texturesInfo.width;
    }

    KisOpenGLUpdateInfoSP updateCache(const QRect& rect, KisImageSP srcImage);
    KisOpenGLUpdateInfoSP updateCacheNoConversion(const QRect& rect);

    void recalculateCache(KisUpdateInfoSP info, bool blockMipmapRegeneration);

    void slotImageSizeChanged(qint32 w, qint32 h);

    KisOpenGLUpdateInfoBuilder& updateInfoBuilder();
    const KoColorProfile* monitorProfile();

Q_SIGNALS:
    void sigShowFloatingMessage(const QString &message, int timeout, bool priority);

protected:

    KisOpenGLImageTextures(KisImageWSP image, const KoColorProfile *monitorProfile,
                           KoColorConversionTransformation::Intent renderingIntent,
                           KoColorConversionTransformation::ConversionFlags conversionFlags);

    void recreateImageTextureTiles();

    void destroyImageTextureTiles();

    void initBufferStorage(bool useBuffer);

private:

    void getTextureSize(KisGLTexturesInfo *texturesInfo);

    void updateTextureFormat();
    KisOpenGLUpdateInfoSP updateCacheImpl(const QRect& rect, KisImageSP srcImage, bool convertColorSpace);

private:
    KisImageWSP m_image;
    QRect m_storedImageBounds;
    const KoColorProfile *m_monitorProfile {nullptr};
    KoColorConversionTransformation::Intent m_renderingIntent;
    KoColorConversionTransformation::ConversionFlags m_conversionFlags;

    bool m_internalColorManagementActive {true};

    boost::optional<GLuint> m_checkerTexture;

    KisGLTexturesInfo m_texturesInfo;
    int m_numCols {0};

    KisOpenGLBufferCircularStorage m_bufferStorage;
    QVector<KisTextureTile*> m_textureTiles;
    QOpenGLBuffer m_tileVertexBuffer;
    QOpenGLBuffer m_tileTexCoordBuffer;

    QOpenGLFunctions *m_glFuncs {nullptr};

    bool m_useOcio {false};
    bool m_initialized {false};

    KisOpenGLUpdateInfoBuilder m_updateInfoBuilder;
};

#endif

