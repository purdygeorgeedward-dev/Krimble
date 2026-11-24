/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_WINDOW_H
#define LIBKIS_WINDOW_H

#include <QObject>
#include <QAction>
#include <QMainWindow>

#include "kritalibkis_export.h"
#include "libkis.h"

class KisMainWindow;

class KRITALIBKIS_EXPORT Window : public QObject
{
    Q_OBJECT

public:
    explicit Window(KisMainWindow *window, QObject *parent = 0);
    ~Window() override;

    bool operator==(const Window &other) const;
    bool operator!=(const Window &other) const;

public Q_SLOTS:

    QMainWindow *qwindow() const;

    QList<QDockWidget *> dockers() const;

    QList<View*> views() const;

    View *addView(Document *document);

    void showView(View *v);

    View *activeView() const;

    void activate();

    void close();

    QAction *createAction(const QString &id, const QString &text = QString(), const QString &menuLocation = QString("tools/scripts"));

Q_SIGNALS:
    void windowClosed();

    void themeChanged();

    void activeViewChanged();

private:
    struct Private;
    Private *const d;

};

#endif
