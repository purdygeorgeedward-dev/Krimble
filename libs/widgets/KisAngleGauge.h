/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISANGLEGAUGE_H
#define KISANGLEGAUGE_H

#include <QWidget>
#include <QScopedPointer>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KisAngleGauge : public QWidget
{
    Q_OBJECT

public:
    enum IncreasingDirection
    {
        IncreasingDirection_CounterClockwise,
        IncreasingDirection_Clockwise
    };

    explicit KisAngleGauge(QWidget *parent = 0);
    ~KisAngleGauge();

    qreal angle() const;
    qreal snapAngle() const;
    qreal resetAngle() const;
    IncreasingDirection increasingDirection() const;

    void setSnapAngle(qreal newSnapAngle);
    void setResetAngle(qreal newResetAngle);
    void setIncreasingDirection(IncreasingDirection newIncreasingDirection);

public Q_SLOTS:
    void setAngle(qreal newAngle);
    void reset();

Q_SIGNALS:
    void angleChanged(qreal angle);

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEvent *e) override;
#else
    void enterEvent(QEnterEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;
private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
