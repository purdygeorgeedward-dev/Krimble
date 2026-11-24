/*
 *  SPDX-FileCopyrightText: 2020 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_SCRATCHPAD_H
#define LIBKIS_SCRATCHPAD_H

#include <QObject>
#include <QColor>
#include <QWidget>

#include <kis_types.h>

#include "libkis.h"
#include "View.h"

#include "kritalibkis_export.h"

class KoCanvasBase;
class Canvas;
class KisView;

class KRITALIBKIS_EXPORT Scratchpad: public QWidget
{
    Q_OBJECT
public:
    Scratchpad(View *view, const QColor & defaultColor, QWidget *parent = 0);
    ~Scratchpad();

public Q_SLOTS:

    void clear();

    void fillDefault();

    void fillGradient(const QPoint &gradientVectorStart = QPoint(),
                      const QPoint &gradientVectorEnd = QPoint(),
                      const QString &gradientShape = "linear",
                      const QString &gradientRepeat = "none",
                      bool reverseGradient = false,
                      bool dither = false);

    void fillBackground();

    void fillForeground();

    void fillTransparent();

    void fillDocument(bool fullContent = true);

    void fillLayer(bool fullContent = true);

    void fillPattern(QTransform transform = QTransform());

    void setFillColor(QColor color);

    void setModeManually(bool value);

    void setMode(QString modeName);

    void linkCanvasZoom(bool value);

    bool canvasZoomLink();

    void setCanvasZoomLink(bool value);

    qreal scale();

    bool setScale(qreal scale) const;

    void scaleToFit();

    void scaleReset();

    void panTo(qint32 x, qint32 y);

    void panCenter();

    void loadScratchpadImage(QImage image);

    QImage copyScratchpadImageData();

    QRect viewportBounds() const;

    QRect contentBounds() const;

Q_SIGNALS:
    void scaleChanged(qreal scale);

    void contentChanged();

    void viewportChanged(const QRect rect);

private:
    struct Private;
    const QScopedPointer<Private> d;

};

#endif

