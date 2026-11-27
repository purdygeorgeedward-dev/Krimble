/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __SLIDER_AND_SPIN_BOX_SYNC_H
#define __SLIDER_AND_SPIN_BOX_SYNC_H

#include <QObject>
#include <functional>

class QSpinBox;
class KisDoubleSliderSpinBox;

class SliderAndSpinBoxSync : public QObject
{
    Q_OBJECT
    using IntFunction = std::function<int()>;

public:
    SliderAndSpinBoxSync(KisDoubleSliderSpinBox *slider,
                         QSpinBox *spinBox,
                         IntFunction parentValueOp);

    ~SliderAndSpinBoxSync() override;

public Q_SLOTS:
    void slotParentValueChanged();

private Q_SLOTS:
    void sliderChanged(qreal value);
    void spinBoxChanged(int value);

private:
    KisDoubleSliderSpinBox *m_slider;
    QSpinBox *m_spinBox;
    IntFunction m_parentValueOp;

    bool m_blockUpdates;
};

#endif
