/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOZOOMSTRATEGY_H
#define KOZOOMSTRATEGY_H

#include "KoShapeRubberSelectStrategy.h"

class KoCanvasController;
class KoZoomTool;

class KoZoomStrategy : public KoShapeRubberSelectStrategy
{
public:
    KoZoomStrategy(KoZoomTool *tool, KoCanvasController *controller, const QPointF &clicked);

    void forceZoomOut();
    void forceZoomIn();

    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    void cancelInteraction() override;

protected:
    SelectionMode currentMode() const override;
private:
    KoCanvasController *m_controller;

    bool m_forceZoomOut;
    Q_DECLARE_PRIVATE(KoShapeRubberSelectStrategy)
};

#endif

