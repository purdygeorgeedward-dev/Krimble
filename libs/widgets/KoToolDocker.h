/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_TOOL_DOCKER_H
#define KO_TOOL_DOCKER_H

#include <QDockWidget>
#include <QPointer>
#include <KisKineticScroller.h>

class QWidget;

#include <kritawidgets_export.h>
#include <KoCanvasObserverBase.h>
class KRITAWIDGETS_EXPORT KoToolDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KoToolDocker(QWidget *parent = 0);
    ~KoToolDocker() override;

    void resetWidgets();

    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

public Q_SLOTS:
    void setOptionWidgets(const QList<QPointer<QWidget> > &optionWidgetList);
    void slotScrollerStateChange(QScroller::State state);

    bool hasOptionWidget();

private:
    Q_PRIVATE_SLOT(d, void locationChanged(Qt::DockWidgetArea area))

    class Private;
    Private * const d;
};

#endif
