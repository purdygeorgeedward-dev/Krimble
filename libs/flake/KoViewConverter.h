/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOVIEWCONVERTER_H
#define KOVIEWCONVERTER_H

#include "kritaflake_export.h"

#include <QtGlobal>

class KoViewTransformStillPoint;
class QPointF;
class QRectF;
class QSizeF;
class QTransform;

class KRITAFLAKE_EXPORT KoViewConverter
{
public:
    KoViewConverter();
    virtual ~KoViewConverter() {}

    virtual QPointF documentToView(const QPointF &documentPoint) const;

    virtual QPointF viewToDocument(const QPointF &viewPoint) const;

    virtual QRectF documentToView(const QRectF &documentRect) const;

    virtual QRectF viewToDocument(const QRectF &viewRect) const;

    virtual QSizeF documentToView(const QSizeF& documentSize) const;

    virtual QSizeF viewToDocument(const QSizeF& viewSize) const;

    virtual qreal documentToViewX(qreal documentX) const;

    virtual qreal documentToViewY(qreal documentY) const;

    virtual qreal viewToDocumentX(qreal viewX) const;

    virtual qreal viewToDocumentY(qreal viewY) const;

    virtual void zoom(qreal *zoomX, qreal *zoomY) const;

    virtual void setZoom(qreal zoom);

    qreal zoom() const;

    virtual KoViewTransformStillPoint makeWidgetStillPoint(const QPointF &viewPoint) const;

    virtual KoViewTransformStillPoint makeDocStillPoint(const QPointF &docPoint) const;

    QTransform documentToView() const;
    QTransform viewToDocument() const;

    virtual QTransform viewToWidget() const;
    virtual QTransform widgetToView() const;

private:
    qreal m_zoomLevel;
};

#endif
