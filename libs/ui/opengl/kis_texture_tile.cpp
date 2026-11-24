/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#define GL_GLEXT_PROTOTYPES
#include "kis_texture_tile.h"
#include "kis_texture_tile_update_info.h"
#include "KisOpenGLBufferCircularStorage.h"

#include <kis_debug.h>
#if !defined(QT_OPENGL_ES)
#include <QOpenGLBuffer>
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x814F
#endif

#ifndef GL_RGBA16_EXT
#define GL_RGBA16_EXT 0x805B
#endif

void KisTextureTile::setTextureParameters()
{

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, m_numMipmapLevels);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_numMipmapLevels);

    if ((m_texturesInfo->internalFormat == GL_RGBA8 && m_texturesInfo->format == GL_RGBA)
#ifndef QT_OPENGL_ES_2
        || (m_texturesInfo->internalFormat == GL_RGBA16 && m_texturesInfo->format == GL_RGBA)
#endif
        || (m_texturesInfo->internalFormat == GL_RGBA16_EXT && m_texturesInfo->format == GL_RGBA)
    ) {
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    }

    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void KisTextureTile::restoreTextureParameters()
{
    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

inline QRectF relativeRect(const QRect &br ,
                           const QRect &cr ,
                           const KisGLTexturesInfo *texturesInfo)
{
    const qreal x = qreal(cr.x() - br.x()) / texturesInfo->width;
    const qreal y = qreal(cr.y() - br.y()) / texturesInfo->height;
    const qreal w = qreal(cr.width()) / texturesInfo->width;
    const qreal h = qreal(cr.height()) / texturesInfo->height;

    return QRectF(x, y, w, h);
}

#include "kis_debug.h"

KisTextureTile::KisTextureTile(const QRect &imageRect, const KisGLTexturesInfo *texturesInfo,
                               const QByteArray &fillData, KisOpenGL::FilterMode filter,
                               KisOpenGLBufferCircularStorage *bufferStorage, int numMipmapLevels, QOpenGLFunctions *fcn)

    : m_textureId(0)
    , m_tileRectInImagePixels(imageRect)
    , m_filter(filter)
    , m_texturesInfo(texturesInfo)
    , m_needsMipmapRegeneration(false)
    , m_preparedLodPlane(0)
    , m_numMipmapLevels(numMipmapLevels)
    , f(fcn)
    , m_bufferStorage(bufferStorage)
{
    const GLvoid *fd = fillData.constData();

    m_textureRectInImagePixels =
            kisGrowRect(m_tileRectInImagePixels, texturesInfo->border);

    m_tileRectInTexturePixels = relativeRect(m_textureRectInImagePixels,
                                             m_tileRectInImagePixels,
                                             m_texturesInfo);

    f->glGenTextures(1, &m_textureId);
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);

    setTextureParameters();

    KisOpenGLBufferCircularStorage::BufferBinder binder(
        m_bufferStorage, &fd, fillData.size());

    f->glTexImage2D(GL_TEXTURE_2D, 0,
                 m_texturesInfo->internalFormat,
                 m_texturesInfo->width,
                 m_texturesInfo->height, 0,
                 m_texturesInfo->format,
                 m_texturesInfo->type, fd);

    restoreTextureParameters();

    setNeedsMipmapRegeneration();
}

KisTextureTile::~KisTextureTile()
{
    f->glDeleteTextures(1, &m_textureId);
}

int KisTextureTile::bindToActiveTexture(bool blockMipmapRegeneration)
{
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);

    if (m_needsMipmapRegeneration && !blockMipmapRegeneration) {
        regenerateMipmap();
        setPreparedLodPlane(0);
    }

    return m_preparedLodPlane;
}

void KisTextureTile::setNeedsMipmapRegeneration()
{
    if (m_filter == KisOpenGL::TrilinearFilterMode ||
        m_filter == KisOpenGL::HighQualityFiltering) {

        m_needsMipmapRegeneration = true;
    }
}

void KisTextureTile::setPreparedLodPlane(int lod)
{
    m_preparedLodPlane = lod;
    m_needsMipmapRegeneration = false;
}

void KisTextureTile::regenerateMipmap()
{
    f->glGenerateMipmap(GL_TEXTURE_2D);
    m_mipmapHasBeenAllocated = true;
    m_needsMipmapRegeneration = false;
}

void KisTextureTile::update(const KisTextureTileUpdateInfo &updateInfo, bool blockMipmapRegeneration)
{
    f->glBindTexture(GL_TEXTURE_2D, m_textureId);

    setTextureParameters();

    const int patchLevelOfDetail = updateInfo.patchLevelOfDetail();
    const QSize patchSize = updateInfo.realPatchSize();
    const QPoint patchOffset = updateInfo.realPatchOffset();

    if (!blockMipmapRegeneration &&
        patchLevelOfDetail > 0 &&
        (m_needsMipmapRegeneration &&
         !updateInfo.isEntireTileUpdated())
            || !m_mipmapHasBeenAllocated) {

        regenerateMipmap();
    }

    QSize tileSize = updateInfo.realTileSize();
    int pixelSize = updateInfo.pixelSize();
    int centerWidth = patchSize.width();
    int centerHeight = patchSize.height();

    int topHeight;
    if (updateInfo.isTopmost()) {
        topHeight = patchOffset.y();
    } else {
        topHeight = 0;
    }

    int leftWidth;
    if (updateInfo.isLeftmost()) {
        leftWidth = patchOffset.x();
    } else {
        leftWidth = 0;
    }

    int rightWidth;
    if (updateInfo.isRightmost()) {
        rightWidth = tileSize.width() - patchOffset.x() - centerWidth;
    } else {
        rightWidth = 0;
    }

    int bottomHeight;
    if (updateInfo.isBottommost()) {
        bottomHeight = tileSize.height() - patchOffset.y() - centerHeight;
    } else {
        bottomHeight = 0;
    }

    if (topHeight > 0 || leftWidth > 0 || rightWidth > 0 || bottomHeight > 0) {
        int bufWidth = leftWidth + centerWidth + rightWidth;
        int bufHeight = topHeight + centerHeight + bottomHeight;

        int centerStride = centerWidth * pixelSize;
        int leftStride = leftWidth * pixelSize;
        int rightStride = rightWidth * pixelSize;
        int bufStride = bufWidth * pixelSize;

        QByteArray buf(bufStride * bufHeight, 0);
        quint8 *bufData = reinterpret_cast<quint8 *>(buf.data());
        const quint8 *patchData = updateInfo.data();

        if (topHeight > 0) {
            const quint8 *topSrcPtr = patchData;
            quint8 *topDstPtr = bufData + leftStride;
            for (int y = 0; y < topHeight; ++y) {
                memcpy(topDstPtr, topSrcPtr, centerStride);
                topDstPtr += bufStride;
            }
        }

        if (leftWidth > 0) {
            int leftDstSkip = centerStride + rightStride;
            const quint8 *leftSrcPtr = patchData;
            quint8 *leftDstPtr = bufData + (topHeight * bufStride);
            for (int y = 0; y < centerHeight; ++y) {
                for (int x = 0; x < leftWidth; ++x) {
                    memcpy(leftDstPtr, leftSrcPtr, pixelSize);
                    leftDstPtr += pixelSize;
                }
                leftSrcPtr += centerStride;
                leftDstPtr += leftDstSkip;
            }
        }

        {
            const quint8 *centerSrcPtr = patchData;
            quint8 *centerDstPtr = bufData + (topHeight * bufStride) + leftStride;
            for (int y = 0; y < centerHeight; ++y) {
                memcpy(centerDstPtr, centerSrcPtr, centerStride);
                centerSrcPtr += centerStride;
                centerDstPtr += bufStride;
            }
        }

        if (rightWidth > 0) {
            int rightDstSkip = leftStride + centerStride;
            const quint8 *rightSrcPtr = patchData + (centerStride - pixelSize);
            quint8 *rightDstPtr = bufData + (topHeight * bufStride) + rightDstSkip;
            for (int y = 0; y < centerHeight; ++y) {
                for (int x = 0; x < rightWidth; ++x) {
                    memcpy(rightDstPtr, rightSrcPtr, pixelSize);
                    rightDstPtr += pixelSize;
                }
                rightSrcPtr += centerStride;
                rightDstPtr += rightDstSkip;
            }
        }

        if (bottomHeight > 0) {
            const quint8 *bottomSrcPtr = patchData + ((centerHeight - 1) * centerStride);
            quint8 *bottomDstPtr = bufData + ((topHeight + centerHeight) * bufStride) + leftStride;
            for (int y = 0; y < bottomHeight; ++y) {
                memcpy(bottomDstPtr, bottomSrcPtr, centerStride);
                bottomDstPtr += bufStride;
            }
        }

        const GLvoid *fd = bufData;
        KisOpenGLBufferCircularStorage::BufferBinder b(m_bufferStorage, &fd, buf.size());
        f->glTexSubImage2D(GL_TEXTURE_2D,
                           patchLevelOfDetail,
                           patchOffset.x() - leftWidth,
                           patchOffset.y() - topHeight,
                           bufWidth,
                           bufHeight,
                           m_texturesInfo->format,
                           m_texturesInfo->type,
                           fd);

    } else if (updateInfo.isEntireTileUpdated()) {
        const GLvoid *fd = updateInfo.data();
        KisOpenGLBufferCircularStorage::BufferBinder b(
            m_bufferStorage, &fd, updateInfo.patchPixelsLength());

        f->glTexImage2D(GL_TEXTURE_2D, patchLevelOfDetail,
                     m_texturesInfo->internalFormat,
                     patchSize.width(),
                     patchSize.height(), 0,
                     m_texturesInfo->format,
                     m_texturesInfo->type,
                     fd);

    } else {
        const GLvoid *fd = updateInfo.data();
        const int size = centerWidth * centerHeight * pixelSize;
        KisOpenGLBufferCircularStorage::BufferBinder b(
            m_bufferStorage, &fd, size);

        f->glTexSubImage2D(GL_TEXTURE_2D, patchLevelOfDetail,
                        patchOffset.x(), patchOffset.y(),
                        patchSize.width(), patchSize.height(),
                        m_texturesInfo->format,
                        m_texturesInfo->type,
                        fd);
    }

    restoreTextureParameters();

    if (!patchLevelOfDetail) {
        if (m_mipmapHasBeenAllocated &&
                (m_filter == KisOpenGL::BilinearFilterMode ||
                 m_filter == KisOpenGL::NearestFilterMode)) {

            setPreparedLodPlane(0);
        } else {
            setNeedsMipmapRegeneration();
        }
    } else {
        setPreparedLodPlane(patchLevelOfDetail);
    }
}

QRectF KisTextureTile::imageRectInTexturePixels(const QRect &imageRect) const
{
    return relativeRect(m_textureRectInImagePixels,
                        imageRect,
                        m_texturesInfo);

}
