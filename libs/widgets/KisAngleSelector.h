/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISANGLESELECTOR_H
#define KISANGLESELECTOR_H

#include <QWidget>
#include <QScopedPointer>

#include <KisAngleGauge.h>
#include <kis_double_parse_spin_box.h>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KisAngleSelectorSpinBox : public KisDoubleParseSpinBox
{
    Q_OBJECT
public:
    KisAngleSelectorSpinBox(QWidget *parent = 0);
    ~KisAngleSelectorSpinBox() override;
    void setRange(double min, double max);
    double valueFromText(const QString & text) const override;
    bool isFlat() const;
    void setFlat(bool newFlat);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void refreshStyle();
protected:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEvent *e) override;
#else
    void enterEvent(QEnterEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

class KRITAWIDGETS_EXPORT KisAngleSelector : public QWidget
{
    Q_OBJECT

public:
    enum FlipOptionsMode
    {
        FlipOptionsMode_NoFlipOptions,
        FlipOptionsMode_MenuButton,
        FlipOptionsMode_Buttons,
        FlipOptionsMode_ContextMenu
    };

    explicit KisAngleSelector(QWidget *parent = 0);
    ~KisAngleSelector();

    qreal angle() const;
    qreal snapAngle() const;
    qreal resetAngle() const;
    int	decimals() const;
    qreal maximum() const;
    qreal minimum() const;
    QString prefix() const;
    bool wrapping() const;
    FlipOptionsMode flipOptionsMode() const;
    int widgetsHeight() const;
    KisAngleGauge::IncreasingDirection increasingDirection() const;
    bool isUsingFlatSpinBox() const;

    void setSnapAngle(qreal newSnapAngle);
    void setResetAngle(qreal newResetAngle);
    void setDecimals(int newNumberOfDecimals);
    void setMaximum(qreal newMaximum);
    void setMinimum(qreal newMinimum);
    void setRange(qreal newMinimum, qreal newMaximum);
    void setPrefix(const QString &newPrefix);
    void setWrapping(bool newWrapping);
    void setFlipOptionsMode(FlipOptionsMode newMode);
    void setWidgetsHeight(int newHeight);
    void setIncreasingDirection(KisAngleGauge::IncreasingDirection newIncreasingDirection);
    void useFlatSpinBox(bool newUseFlatSpinBox);

    static qreal closestCoterminalAngleInRange(qreal angle, qreal minimum, qreal maximum, bool *ok = nullptr);
    qreal closestCoterminalAngleInRange(qreal angle, bool *ok = nullptr) const;
    static qreal flipAngle(qreal angle, Qt::Orientations orientations);
    static qreal flipAngle(qreal angle, qreal minimum, qreal maximum, Qt::Orientations orientations, bool *ok = nullptr);
    void flip(Qt::Orientations orientations);

public Q_SLOTS:
    void setAngle(qreal newAngle);
    void reset();

Q_SIGNALS:
    void angleChanged(qreal angle);

private:
    struct Private;
    const QScopedPointer<Private> m_d;

    bool event(QEvent *e) override;
    bool eventFilter(QObject *o, QEvent *e) override;
};

#endif
