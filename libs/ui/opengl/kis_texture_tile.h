/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_TEXTURE_TILE_H_
#define KIS_TEXTURE_TILE_H_

#include <QRect>
#include <QRectF>
#include <QOpenGLFunctions>

#include "kis_opengl.h"

class KisOpenGLBufferCircularStorage;
class KisTextureTileUpdateInfo;
class QOpenGLBuffer;

struct KisGLTexturesInfo {

    int width {0};
    int height {0};

    int effectiveWidth {1};
    int effectiveHeight {1};

    int border {0};

    GLint internalFormat {0};
    GLint format {0};
    GLint type {0};
};

class KisTextureTile
{
public:
    KisTextureTile(const QRect &imageRect, const KisGLTexturesInfo *texturesInfo,
                   const QByteArray &fillData, KisOpenGL::FilterMode mode,
                   KisOpenGLBufferCircularStorage *bufferStorage, int numMipmapLevels, QOpenGLFunctions *f);
    ~KisTextureTile();

    void setBufferStorage(KisOpenGLBufferCircularStorage *bufferStorage) {
        m_bufferStorage = bufferStorage;
    }

    void setNumMipmapLevels(int num) {
        m_numMipmapLevels = num;
    }

    void update(const KisTextureTileUpdateInfo &updateInfo, bool blockMipmapRegeneration);

    inline QRect tileRectInImagePixels() {
        return m_tileRectInImagePixels;
    }

    inline QRect textureRectInImagePixels() {
        return m_textureRectInImagePixels;
    }

    inline QRectF tileRectInTexturePixels() {
        return m_tileRectInTexturePixels;
    }

    QRectF imageRectInTexturePixels(const QRect &imageRect) const;

    int bindToActiveTexture(bool blockMipmapRegeneration);

private:
    inline void setTextureParameters();
    inline void restoreTextureParameters();

    void setNeedsMipmapRegeneration();
    void setPreparedLodPlane(int lod);
    void regenerateMipmap();

    GLuint m_textureId;

    QRect m_tileRectInImagePixels;
    QRectF m_tileRectInTexturePixels;
    QRect m_textureRectInImagePixels;
    KisOpenGL::FilterMode m_filter;
    const KisGLTexturesInfo *m_texturesInfo;
    bool m_needsMipmapRegeneration;
    int m_preparedLodPlane;
    int m_numMipmapLevels;
    QOpenGLFunctions *f;
    KisOpenGLBufferCircularStorage *m_bufferStorage;
    bool m_mipmapHasBeenAllocated = false;
    Q_DISABLE_COPY(KisTextureTile)
};

#endif

