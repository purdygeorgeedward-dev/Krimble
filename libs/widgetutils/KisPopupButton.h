/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_POPUP_BUTTON_H_
#define _KIS_POPUP_BUTTON_H_

#include <QToolButton>

#include <kritawidgetutils_export.h>

class KRITAWIDGETUTILS_EXPORT KisPopupButton : public QToolButton
{

    Q_OBJECT

public:

    KisPopupButton(QWidget* parent);
    ~KisPopupButton() override;

    void setPopupWidget(QWidget* widget);

    void setPopupWidgetWidth(int w);

    void adjustPosition();

    void setArrowVisible(bool v);

    bool isPopupWidgetVisible();

Q_SIGNALS:
    void sigPopupWidgetShown();

public Q_SLOTS:

    void showPopupWidget();

    void hidePopupWidget();

    void setPopupWidgetVisible(bool visible);

    void setPopupWidgetDetached(bool detach);

protected:
    void paintEvent(QPaintEvent* event) override;

    void paintPopupArrow();

private Q_SLOTS:
    void slotMenuAboutToHide();

private:
    struct Private;
    Private* const m_d;
};

#endif
