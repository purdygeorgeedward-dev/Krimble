/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_COLOR_LABEL_SELECTOR_WIDGET_H
#define __KIS_COLOR_LABEL_SELECTOR_WIDGET_H

#include <QScopedPointer>
#include <QWidget>

#include "kritaui_export.h"

class KRITAUI_EXPORT KisColorLabelSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    enum SelectionIndicationType {
        FillIn,
        Outline
    };

    KisColorLabelSelectorWidget(QWidget *parent = nullptr);
    ~KisColorLabelSelectorWidget() override;

    bool isButtonChecked(int index) const;
    int currentIndex() const;
    QList<int> selection() const;

    bool exclusive() const;
    void setExclusive(bool exclusive);

    bool buttonWrapEnabled() const;
    void setButtonWrapEnabled(bool enabled);

    bool mouseDragEnabled() const;
    void setMouseDragEnabled(bool enabled);

    SelectionIndicationType selectionIndicationType() const;
    void setSelectionIndicationType(SelectionIndicationType type);

    int buttonSize() const;
    void setButtonSize(int size);

public Q_SLOTS:
    void setButtonChecked(int index, bool state);
    void setCurrentIndex(int index);
    void setSelection(const QList<int> &indices);

Q_SIGNALS:
    void buttonToggled(int index, bool state);
    void currentIndexChanged(int index);
    void selectionChanged();

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

class KRITAUI_EXPORT KisColorLabelSelectorWidgetMenuWrapper : public QWidget
{
    Q_OBJECT

public:
    KisColorLabelSelectorWidgetMenuWrapper(QWidget *parent = nullptr);
    ~KisColorLabelSelectorWidgetMenuWrapper() override;

    KisColorLabelSelectorWidget* colorLabelSelector() const;

    void showEvent(QShowEvent* e) override;

    int calculateMenuOffset() const;

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
