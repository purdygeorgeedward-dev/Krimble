/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSLIDERCOMBO_H_
#define KOSLIDERCOMBO_H_

#include <QComboBox>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KoSliderCombo : public QComboBox
{

    Q_OBJECT

public:

    explicit KoSliderCombo(QWidget *parent=0);

    ~KoSliderCombo() override;

    qreal decimals() const;

    qreal minimum() const;

    qreal maximum() const;

    void setDecimals(int number);

    void setMinimum(qreal min);

    void setMaximum(qreal max);

    qreal value() const;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public Q_SLOTS:

    void setValue(qreal value);

Q_SIGNALS:

    void valueChanged(qreal value, bool final);

protected:
    void paintEvent(QPaintEvent *) override;
    void hideEvent(QHideEvent *) override;
    void changeEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private:
    Q_PRIVATE_SLOT(d, void sliderValueChanged(int value))
    Q_PRIVATE_SLOT(d, void sliderReleased())
    Q_PRIVATE_SLOT(d, void lineEditFinished())

    class KoSliderComboPrivate;
    KoSliderComboPrivate * const d;
};

#endif
