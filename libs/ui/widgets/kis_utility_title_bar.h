/*
 *  SPDX-FileCopyrightText: 2020 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISUTILITYTITLEBAR_H
#define KISUTILITYTITLEBAR_H

#include <QWidget>

#ifdef Q_OS_MACOS
#include <sys/types.h>
#endif

#include "kritaui_export.h"

class QLabel;
class QHBoxLayout;
class QPushButton;

class KRITAUI_EXPORT KisUtilityTitleBar : public QWidget
{
    Q_OBJECT

public:
    KisUtilityTitleBar(QWidget *parent = nullptr);
    KisUtilityTitleBar(QLabel *title, QWidget *parent = nullptr);
    ~KisUtilityTitleBar();
    QSize sizeHint() const override
    {
        return QSize(32, 32);
    }

    QWidget* widgetArea();
    void setWidgetArea(QWidget* widgetArea);

public Q_SLOTS:
    void setLocked(bool locked);
protected:
    QHBoxLayout *widgetAreaLayout;

    const int SPACING_UNIT = 16;

    class Private;
    Private * const d;
};

#endif
