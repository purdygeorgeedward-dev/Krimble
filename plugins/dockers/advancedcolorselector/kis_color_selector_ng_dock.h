/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_COLOR_SELECTOR_NG_DOCKER_H
#define KIS_COLOR_SELECTOR_NG_DOCKER_H

#include <QDockWidget>
#include <KoCanvasObserverBase.h>

class KisColorSelectorNgDockerWidget;

class KisColorSelectorNgDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KisColorSelectorNgDock(bool classic = false);
    QString observerName() override { return "KisColorSelectorNgDock"; }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    KisColorSelectorNgDockerWidget *m_colorSelectorNgWidget;
    void krimbleFitFloating(bool snap);
    bool m_krimbleFitting = false;
    QSize m_krimbleDockedMin;
    QSize m_krimbleDockedMax;
};

#endif
