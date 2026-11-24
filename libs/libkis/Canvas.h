/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_CANVAS_H
#define LIBKIS_CANVAS_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"

class KoCanvasBase;
class KisDisplayColorConverter;

class KRITALIBKIS_EXPORT Canvas : public QObject
{
    Q_OBJECT

public:
    explicit Canvas(KoCanvasBase *canvas, QObject *parent = 0);
    ~Canvas() override;

    bool operator==(const Canvas &other) const;
    bool operator!=(const Canvas &other) const;

public Q_SLOTS:

    qreal zoomLevel() const;

    void setZoomLevel(qreal value);

    void setPreferredCenter(const QPointF& imagePos);

    QPointF preferredCenter() const;

    void pan(const QPoint& offset);

    void resetZoom();

    qreal rotation() const;

    void setRotation(qreal angle);

    void resetRotation();

    bool mirror() const;

    void setMirror(bool value);

    bool wrapAroundMode() const;

    void setWrapAroundMode(bool enable);

    bool levelOfDetailMode() const;

    void setLevelOfDetailMode(bool enable);

    View *view() const;

private:

    friend class ManagedColor;

    KisDisplayColorConverter *displayColorConverter() const;

    struct Private;
    Private *const d;

};

#endif
