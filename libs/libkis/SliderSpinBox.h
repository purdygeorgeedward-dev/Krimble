/*
 * SPDX-FileCopyrightText: 2010 Justin Noel <justin@ics.com>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_SLIDERSPINBOX_H
#define LIBKIS_SLIDERSPINBOX_H

#include "kis_slider_spin_box.h"
#include "IntParseSpinBox.h"
#include "DoubleParseSpinBox.h"

#include "kritalibkis_export.h"
#include "libkis.h"

template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
class KisSliderSpinBoxPrivate;

class KRITALIBKIS_EXPORT SliderSpinBox : public IntParseSpinBox
{
    Q_OBJECT
    Q_DISABLE_COPY(SliderSpinBox)

public:
    explicit SliderSpinBox();
    ~SliderSpinBox() override;

public Q_SLOTS:

    QWidget* widget() const;

    int fastSliderStep() const;
    int softMinimum() const;
    int softMaximum() const;
    bool isDragging() const;
    void setValue(int newValue);
    void setRange(int newMinimum, int newMaximum, bool computeNewFastSliderStep = true);
    void setMinimum(int newMinimum, bool computeNewFastSliderStep = true);
    void setMaximum(int newMaximum, bool computeNewFastSliderStep = true);
    void setExponentRatio(qreal newExponentRatio);
    void setBlockUpdateSignalOnDrag(bool newBlockUpdateSignalOnDrag);
    void setFastSliderStep(int newFastSliderStep);
    void setSoftRange(int newSoftMinimum, int newSoftMaximum);
    void setSoftMinimum(int newSoftMinimum);
    void setSoftMaximum(int newSoftMaximum);

Q_SIGNALS:
    void draggingFinished();

private:
    struct Private;
    Private *const d;
};

class KRITALIBKIS_EXPORT DoubleSliderSpinBox : public DoubleParseSpinBox
{
    Q_OBJECT
    Q_DISABLE_COPY(DoubleSliderSpinBox)

public:
    explicit DoubleSliderSpinBox();
    ~DoubleSliderSpinBox() override;

public Q_SLOTS:

    QWidget* widget() const;

    qreal fastSliderStep() const;
    qreal softMinimum() const;
    qreal softMaximum() const;
    bool isDragging() const;
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

Q_SIGNALS:
    void draggingFinished();

private:
    struct Private;
    Private *const d;
};

#endif
