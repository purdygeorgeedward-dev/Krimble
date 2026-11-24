/*
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_IMAGE_PATCH_H_
#define KIS_IMAGE_PATCH_H_

#include <QPainter>
#include <QImage>
#include <kis_types.h>

#define BORDER_SIZE(scale) (ceil(0.5/scale))

class KisImagePatch
{
public:
    KisImagePatch();

    KisImagePatch(QRect imageRect, qint32 borderWidth,
                  qreal scaleX, qreal scaleY);

    void setImage(QImage image);

    void preScale(const QRectF &dstRect);

    QRect patchRect();

    void drawMe(QPainter &gc,
                const QRectF &dstRect,
                QPainter::RenderHints renderHints);

    bool isValid();

private:
    qreal m_scaleX {0.0};
    qreal m_scaleY {0.0};

    QRect m_patchRect;

    QRectF m_interestRect;

    QImage m_image;
    bool m_isScaled {false};
};

#endif
