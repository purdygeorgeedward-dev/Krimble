/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISQUICKPOPUPWIDGET_H
#define KISQUICKPOPUPWIDGET_H

#include <QFrame>
#include <QQuickItem>

#include <kritaqmlwidgets_export.h>

class KRITAQMLWIDGETS_EXPORT KisQQuickPopupWidget: public QFrame
{
    Q_OBJECT
public:
    KisQQuickPopupWidget(QWidget *parent = nullptr);
    ~KisQQuickPopupWidget();

    QQuickItem *rootObject() const;

    QMargins layoutContentMargins() const;
    void setLayoutMargins(const QMargins margins);
Q_SIGNALS:
    void signalRootObjectReady();
protected:
    void keyPressEvent(QKeyEvent *event) override;

    bool event(QEvent *e) override;
private Q_SLOTS:
    void emitRootObjectReady();
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
