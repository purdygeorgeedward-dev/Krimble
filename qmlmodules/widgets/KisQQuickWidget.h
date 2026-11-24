/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISQQUICKWIDGET_H
#define KISQQUICKWIDGET_H

#include <QQuickWidget>
#include <kritaqmlwidgets_export.h>

class KRITAQMLWIDGETS_EXPORT KisQQuickWidget : public QQuickWidget
{
    Q_OBJECT
public:
    KisQQuickWidget(QWidget *parent = nullptr);

    ~KisQQuickWidget();

    void connectMinimumHeightToRootObject();
    void connectMinimumWidthToRootObject();
private Q_SLOTS:
    void updatePaletteFromConfig();

    void setMinimumHeightFromRoot();
    void setMinimumWidthFromRoot();
};

#endif
