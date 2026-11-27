/*
 * SPDX-FileCopyrightText: 2016 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_VISUAL_COLOR_SELECTOR_SHAPE_H
#define KIS_VISUAL_COLOR_SELECTOR_SHAPE_H

#include <QWidget>
#include <QScopedPointer>
#include <QPixmap>
#include <QRegion>
#include <QMouseEvent>

#include <KoColor.h>
#include "KoColorDisplayRendererInterface.h"

#include "KisVisualColorSelector.h"
#include "KisColorSelectorConfiguration.h"

class KisVisualColorSelectorShape : public QWidget
{
    Q_OBJECT
public:
    enum Dimensions{onedimensional, twodimensional};
    explicit KisVisualColorSelectorShape(KisVisualColorSelector *parent,
                                         KisVisualColorSelectorShape::Dimensions dimension,
                                         int channel1, int channel2);
    ~KisVisualColorSelectorShape() override;

    QPointF getCursorPosition() const;
    Dimensions getDimensions() const;
    const QImage& getImageMap();
    KoColor getCurrentColor();
    QColor getColorFromConverter(KoColor c);

    virtual QRect getSpaceForSquare(QRect geom) = 0;
    virtual QRect getSpaceForCircle(QRect geom) = 0;
    virtual QRect getSpaceForTriangle(QRect geom) = 0;

    bool isHueControl() const;
    virtual bool supportsGamutMask() const;

    void forceImageUpdate();

    virtual void updateGamutMask();

    virtual void setBorderWidth(int width) = 0;

    int channel(int dimension) const;

    quint32 channelMask() const;

    void setCursorPosition(QPointF position, bool signal = false);

    void setChannelValues(QVector4D channelValues, quint32 channelFlags);

    void setAcceptTabletEvents(bool on);

Q_SIGNALS:
    void sigCursorMoved(QPointF pos);

protected:
    KisVisualColorSelector* colorSelector() const;
    KisVisualColorModel* selectorModel() const;
    QImage convertImageMap(const quint8 *rawColor, quint32 bufferSize, QSize imgSize) const;
    virtual QImage renderBackground(const QVector4D &channelValues, const QImage &alpha) const;
    virtual QImage compositeBackground() const;
    virtual QImage renderAlphaMask() const;
    virtual QImage renderStaticAlphaMask() const;
    virtual QPointF mousePositionToShapeCoordinate(const QPointF &pos, const QPointF &dragStart) const;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void tabletEvent(QTabletEvent* event) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent *) override;

private:
    struct Private;
    const QScopedPointer<Private> m_d;

    virtual QPointF convertShapeCoordinateToWidgetCoordinate(QPointF) const = 0;

    virtual QPointF convertWidgetCoordinateToShapeCoordinate(QPointF coordinate) const = 0;

    virtual QRegion getMaskMap() = 0;
    virtual void drawCursor(QPainter &painter) = 0;
    virtual void drawGamutMask(QPainter &painter);
};

#endif
