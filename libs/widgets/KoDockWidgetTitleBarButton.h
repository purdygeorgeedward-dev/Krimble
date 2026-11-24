/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KODOCKWIDGETTITLEBARBUTTON_H_
#define KODOCKWIDGETTITLEBARBUTTON_H_

#include "kritawidgets_export.h"
#include <QAbstractButton>

class QEvent;
class QPaintEvent;

class KRITAWIDGETS_EXPORT KoDockWidgetTitleBarButton : public QAbstractButton
{
    Q_OBJECT

public:
    explicit KoDockWidgetTitleBarButton(QWidget *parent = 0);
    ~KoDockWidgetTitleBarButton() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEvent *e) override;
#else
    void enterEvent(QEnterEvent *e) override;
#endif
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    class Private;
    Private * const d;
};

#endif
