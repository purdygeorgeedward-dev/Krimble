/*
 *  SPDX-FileCopyrightText: 2018 Jouni Pentikäinen <joupent@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISCANVASWINDOW_H
#define KISCANVASWINDOW_H

#include <QWidget>

class KisMainWindow;

class KisCanvasWindow : public QWidget
{
public:
    explicit KisCanvasWindow(KisMainWindow *mainWindow);
    ~KisCanvasWindow() override;

    QWidget * swapMainWidget(QWidget *widget);

    void closeEvent(QCloseEvent *event) override;
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
