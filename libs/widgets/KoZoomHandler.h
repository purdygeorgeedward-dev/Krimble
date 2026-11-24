/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2006, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOZOOMHANDLER_H
#define KOZOOMHANDLER_H

#include "kritawidgets_export.h"
#include <KoZoomMode.h>
#include <KoViewConverter.h>

class KRITAWIDGETS_EXPORT KoZoomHandler : public KoViewConverter
{
public:

    KoZoomHandler();
    ~KoZoomHandler() override;

    inline qreal zoomedResolutionX() const { return m_zoomedResolutionX; }

    inline qreal zoomedResolutionY() const { return m_zoomedResolutionY; }

    inline qreal resolutionX() const { return m_resolutionX; }
    inline qreal resolutionY() const { return m_resolutionY; }

    inline qreal zoomFactorX() const { return m_zoomedResolutionX / m_resolutionX; }

    inline qreal zoomFactorY() const { return m_zoomedResolutionY / m_resolutionY; }

    void setDpi(int dpiX, int dpiY);

    void setResolution(qreal resolutionX, qreal resolutionY);

    virtual void setZoomedResolution(qreal zoomedResolutionX, qreal zoomedResolutionY);

    void setZoom(qreal zoom) override;

    inline void setZoomMode(KoZoomMode::Mode zoomMode) { m_zoomMode = zoomMode; }
    inline int zoomInPercent() const { return qRound(KoViewConverter::zoom() * 100); }
    KoZoomMode::Mode zoomMode() const { return m_zoomMode; }

    inline qreal zoomItX(qreal z) const
        {
            return m_zoomedResolutionX * z;
        }

    inline qreal zoomItY(qreal z) const
        {
            return m_zoomedResolutionY * z ;
        }

    inline qreal unzoomItX(qreal x) const
        {
            return  x / m_zoomedResolutionX;
        }

    inline qreal unzoomItY(qreal y) const
        {
            return  y / m_zoomedResolutionY;
        }

    void setZoomMarginSize( int size );

    int zoomMarginSize() const;

    QPointF documentToView(const QPointF &documentPoint) const override;

    QPointF viewToDocument(const QPointF &viewPoint) const override;

    QRectF documentToView(const QRectF &documentRect) const override;

    QRectF viewToDocument(const QRectF &viewRect) const override;

    QSizeF documentToView(const QSizeF &documentSize) const override;

    QSizeF viewToDocument(const QSizeF &viewSize) const override;

    qreal documentToViewX(qreal documentX) const override;

    qreal documentToViewY(qreal documentY) const override;

    qreal viewToDocumentX(qreal viewX) const override;

    qreal viewToDocumentY(qreal viewY) const override;

    void zoom(qreal *zoomX, qreal *zoomY) const override;

    using KoViewConverter::zoom;

protected:

    KoZoomMode::Mode m_zoomMode;

    qreal m_resolutionX;
    qreal m_resolutionY;
    qreal m_zoomedResolutionX;
    qreal m_zoomedResolutionY;

    int m_zoomMarginSize;
};

#endif
