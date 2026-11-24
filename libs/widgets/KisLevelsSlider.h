/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_LEVELS_SLIDER_H
#define KIS_LEVELS_SLIDER_H

#include <QWidget>
#include <QVector>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KisLevelsSlider : public QWidget
{
    Q_OBJECT

public:
    KisLevelsSlider(QWidget *parent);
    ~KisLevelsSlider();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    qreal handlePosition(int handleIndex) const;
    QColor handleColor(int handleIndex) const;
    virtual QRect gradientRect() const;

public Q_SLOTS:
    virtual void setHandlePosition(int handleIndex, qreal newPosition);
    virtual void setHandleColor(int handleIndex, const QColor &newColor);

Q_SIGNALS:
    void handlePositionChanged(int handleIndex, qreal position);
    void handleColorChanged(int handleIndex, const QColor &color);

protected:
    struct Handle
    {
        int index;
        qreal position;
        QColor color;
    };

    static constexpr int handleWidth{11};
    static constexpr int handleHeight{11};
    static constexpr qreal minimumSpaceBetweenHandles{0.001};
    static constexpr qreal normalPositionIncrement{0.01};
    static constexpr qreal slowPositionIncrement{0.001};

    QVector<Handle> m_handles;
    int m_constrainPositions;

    int m_selectedHandle;
    int m_hoveredHandle;

    QVector<Handle> sortedHandles() const;
    int closestHandleToPosition(qreal position) const;
    qreal positionFromX(int x) const;
    int closestHandleToX(int x) const;
    int xFromPosition(qreal position) const;
    virtual void paintGradient(QPainter &painter, const QRect &rect) = 0;
    virtual void paintHandle(QPainter &painter, const QRect &rect, const Handle &handle);

    void handleIncrementInput(int direction, Qt::KeyboardModifiers modifiers);

    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
};

class KRITAWIDGETS_EXPORT KisInputLevelsSlider : public KisLevelsSlider
{
    Q_OBJECT

public:
    KisInputLevelsSlider(QWidget *parent = nullptr);
    ~KisInputLevelsSlider();

    qreal blackPoint() const;
    qreal whitePoint() const;

public Q_SLOTS:
    virtual void setBlackPoint(qreal newBlackPoint);
    virtual void setWhitePoint(qreal newWhitePoint);
    virtual void reset(qreal newBlackPoint, qreal newWhitePoint);

Q_SIGNALS:
    void blackPointChanged(qreal newBlackPoint);
    void whitePointChanged(qreal newWhitePoint);

protected:
    void paintGradient(QPainter &painter, const QRect &rect) override;
    virtual void paintBottomGradientMiddleSection(QImage &gradientImage, const QVector<Handle> &sortedHandles_);
};

class KRITAWIDGETS_EXPORT KisInputLevelsSliderWithGamma : public KisInputLevelsSlider
{
    Q_OBJECT

public:
    KisInputLevelsSliderWithGamma(QWidget *parent = nullptr);
    ~KisInputLevelsSliderWithGamma();

    qreal gamma() const;

public Q_SLOTS:
    void setHandlePosition(int handleIndex, qreal newPosition) override;
    void setGamma(qreal newGamma);
    void reset(qreal newBlackPoint, qreal newWhitePoint) override;
    void reset(qreal newBlackPoint, qreal newWhitePoint, qreal newGamma);

Q_SIGNALS:
    void gammaChanged(qreal newGamma);

protected:
    void paintBottomGradientMiddleSection(QImage &gradientImage, const QVector<Handle> &sortedHandles_) override;

private:
    qreal m_gamma;

    qreal gammaToPosition() const;
    qreal positionToGamma() const;
};

class KRITAWIDGETS_EXPORT KisOutputLevelsSlider : public KisInputLevelsSlider
{
    Q_OBJECT

public:
    KisOutputLevelsSlider(QWidget *parent = nullptr);
    ~KisOutputLevelsSlider();
};

class KRITAWIDGETS_EXPORT KisThresholdSlider : public KisInputLevelsSlider
{
    Q_OBJECT

public:
    KisThresholdSlider(QWidget *parent = nullptr);
    ~KisThresholdSlider();

    qreal threshold() const;

public Q_SLOTS:
    void setHandlePosition(int handleIndex, qreal newPosition) override;
    void setBlackPoint(qreal newBlackPoint) override;
    void setWhitePoint(qreal newWhitePoint) override;
    void reset(qreal newBlackPoint, qreal newWhitePoint) override;
    void setThreshold(qreal newGamma);

Q_SIGNALS:
    void thresholdChanged(qreal newThreshold);

protected:
    void paintBottomGradientMiddleSection(QImage &gradientImage, const QVector<Handle> &sortedHandles_) override;
    void paintHandle(QPainter &painter, const QRect &rect, const Handle &handle) override;
};

#endif
