/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_DOCKWIDGET_H
#define LIBKIS_DOCKWIDGET_H

#include <QDockWidget>

#include "kritalibkis_export.h"
#include "libkis.h"

#include <KoCanvasObserverBase.h>

class KoCanvasBase;

class KRITALIBKIS_EXPORT DockWidget : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
    Q_DISABLE_COPY(DockWidget)

public:
    explicit DockWidget();
    ~DockWidget() override;

protected Q_SLOTS:

    void setCanvas(KoCanvasBase* canvas) override;
    void unsetCanvas() override;

protected Q_SLOTS:

    Canvas* canvas() const;

    virtual void canvasChanged(Canvas *canvas) = 0;

private:
    struct Private;
    Private *const d;

};

#endif
