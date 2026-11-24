/*
 * SPDX-FileCopyrightText: 2018 Jouni Pentikäinen <joupent@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISSELECTIONPROPERTYSLIDER_H
#define KISSELECTIONPROPERTYSLIDER_H

#include <QObject>

#include <KoShape.h>
#include <kritaui_export.h>
#include <kis_signals_blocker.h>
#include "kis_slider_spin_box.h"

class KRITAUI_EXPORT KisSelectionPropertySliderBase : public KisDoubleSliderSpinBox
{
    Q_OBJECT

public:
    explicit KisSelectionPropertySliderBase(QWidget* parent = 0);
    ~KisSelectionPropertySliderBase() override;

    void setTextTemplates(const QString &normalTemplate, const QString &mixedTemplate);

    void setPrefix(const QString &) = delete;

    void setSuffix(const QString &) = delete;

protected:
    void setInternalValue(qreal value, bool blockUpdateSignal) override;

    void setSelectionValue(qreal commonValue, bool mixed);

    virtual bool hasSelection() const = 0;
    virtual qreal getCommonValue() const = 0;

private Q_SLOTS:
    void slotCompressedUpdate();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

template<class T>
class KRITAUI_EXPORT KisSelectionPropertySlider : public KisSelectionPropertySliderBase
{
public:
    explicit KisSelectionPropertySlider(QWidget *parent = 0)
        : KisSelectionPropertySliderBase(parent)
    {}

    void setValueGetter(qreal (*getter)(T))
    {
        m_valueGetter = getter;
    }

    void setSelection(QList<T> newSelection)
    {
        KisSignalsBlocker b(this);

        m_selection = newSelection;

        const qreal commonValue = getCommonValue();

        setEnabled(!m_selection.isEmpty());
        setSelectionValue(commonValue, commonValue < 0.0);
    }

    QList<T> selection() const {
        return m_selection;
    }

protected:
    bool hasSelection() const override
    {
        return !m_selection.isEmpty();
    }

    qreal getCommonValue() const override
    {
        qreal commonValue = -1.0;

        Q_FOREACH (T item, m_selection) {
            const qreal itemValue = m_valueGetter(item);

            if (commonValue < 0) {
                commonValue = itemValue;
            } else if (!qFuzzyCompare(commonValue, itemValue)) {
                commonValue = -1.0;
                break;
            }
        }

        return commonValue;
    }

private:
    qreal (*m_valueGetter)(T) {nullptr};
    QList<T> m_selection;
};

class KRITAUI_EXPORT KisShapePropertySlider : public KisSelectionPropertySlider<KoShape*>
{
public:
    KisShapePropertySlider(QWidget* parent=nullptr);
};

#endif
