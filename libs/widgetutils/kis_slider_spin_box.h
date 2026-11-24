/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Justin Noel <justin@ics.com>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISSLIDERSPINBOX_H
#define KISSLIDERSPINBOX_H

#include <QScopedPointer>

#include <kis_int_parse_spin_box.h>
#include <kis_double_parse_spin_box.h>

#include <kritawidgetutils_export.h>

template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
class KisSliderSpinBoxPrivate;

class KRITAWIDGETUTILS_EXPORT KisSliderSpinBox : public KisIntParseSpinBox
{
    Q_OBJECT
public:
    KisSliderSpinBox(QWidget * parent = nullptr);
    ~KisSliderSpinBox() override;

    int fastSliderStep() const;
    int softMinimum() const;
    int softMaximum() const;
    bool isDragging() const;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void setValue(int newValue);
    void setRange(int newMinimum, int newMaximum, bool computeNewFastSliderStep = true);
    void setMinimum(int newMinimum, bool computeNewFastSliderStep = true);
    void setMaximum(int newMaximum, bool computeNewFastSliderStep = true);
    void setExponentRatio(qreal newExponentRatio);
    void setBlockUpdateSignalOnDrag(bool newBlockUpdateSignalOnDrag);
    void setFastSliderStep(int newFastSliderStep);
    void setPageStep(int newPageStep);
    void setSoftRange(int newSoftMinimum, int newSoftMaximum);
    void setSoftMinimum(int newSoftMinimum);
    void setSoftMaximum(int newSoftMaximum);

Q_SIGNALS:
    void draggingFinished();

protected:
    virtual void setInternalValue(int value, bool blockUpdateSignal);
    void setPrivateValue(int value);

private:
    template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
    friend class KisSliderSpinBoxPrivate;
    QScopedPointer<KisSliderSpinBoxPrivate<KisSliderSpinBox, KisIntParseSpinBox>> d;
};

class KRITAWIDGETUTILS_EXPORT KisDoubleSliderSpinBox : public KisDoubleParseSpinBox
{
    Q_OBJECT
public:
    KisDoubleSliderSpinBox(QWidget * parent = nullptr);
    ~KisDoubleSliderSpinBox() override;

    qreal fastSliderStep() const;
    qreal softMinimum() const;
    qreal softMaximum() const;
    bool isDragging() const;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void setValue(qreal newValue);
    void setRange(qreal newMinimum, qreal newMaximum, int newNumberOfDecimals = 0, bool computeNewFastSliderStep = true);
    void setMinimum(qreal newMinimum, bool computeNewFastSliderStep = true);
    void setMaximum(qreal newMaximum, bool computeNewFastSliderStep = true);
    void setExponentRatio(qreal newExponentRatio);
    void setBlockUpdateSignalOnDrag(bool newBlockUpdateSignalOnDrag);
    void setFastSliderStep(qreal newFastSliderStep);
    void setSoftRange(qreal newSoftMinimum, qreal newSoftMaximum);
    void setSoftMinimum(qreal newSoftMinimum);
    void setSoftMaximum(qreal newSoftMaximum);

protected:
    virtual void setInternalValue(qreal newValue, bool newBlockUpdateSignal);
    void setPrivateValue(qreal newValue);

Q_SIGNALS:
    void draggingFinished();

private:
    template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
    friend class KisSliderSpinBoxPrivate;
    QScopedPointer<KisSliderSpinBoxPrivate<KisDoubleSliderSpinBox, KisDoubleParseSpinBox>> d;
};

#endif
