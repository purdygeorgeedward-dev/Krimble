/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KIS_MULTIPLIERS_DOUBLE_SLIDER_SPINBOX_H_
#define _KIS_MULTIPLIERS_DOUBLE_SLIDER_SPINBOX_H_

#include <QWidget>

#include <kritawidgetutils_export.h>

class KRITAWIDGETUTILS_EXPORT KisMultipliersDoubleSliderSpinBox : public QWidget {
    Q_OBJECT
public:
    KisMultipliersDoubleSliderSpinBox(QWidget* _parent = 0);
    ~KisMultipliersDoubleSliderSpinBox() override;

    void addMultiplier(double v);
    void setRange(qreal minimum, qreal maximum, int decimals = 0);

    qreal value();

public Q_SLOTS:
    void setValue(qreal value);

public:
    void setExponentRatio(qreal dbl);

    void setPrefix(const QString& prefix);
    void setSuffix(const QString& suffix);

    void setBlockUpdateSignalOnDrag(bool block);

    void setSingleStep(qreal value);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    virtual QSize minimumSize() const;

Q_SIGNALS:
    void valueChanged(qreal value);

private:
    Q_PRIVATE_SLOT(d, void updateRange())
    struct Private;
    Private* const d;
};

#endif
