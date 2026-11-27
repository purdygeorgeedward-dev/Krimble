/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPERUBBERSELECTSTRATEGY_H
#define KOSHAPERUBBERSELECTSTRATEGY_H

#include "KoInteractionStrategy.h"

#include <QRectF>

#include "kritaflake_export.h"

class KoToolBase;
class KoShapeRubberSelectStrategyPrivate;

class KRITAFLAKE_EXPORT KoShapeRubberSelectStrategy : public KoInteractionStrategy
{
public:
    KoShapeRubberSelectStrategy(KoToolBase *tool, const QPointF &clicked, bool useSnapToGrid = false);

    void paint(QPainter &painter, const KoViewConverter &converter, const KoColorDisplayRendererInterface *displayRendererInterface) override;
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;

protected:
    KoShapeRubberSelectStrategy(KoShapeRubberSelectStrategyPrivate &);

    QRectF selectedRectangle() const;

    enum SelectionMode {
        CrossingSelection,
        CoveringSelection
    };

    virtual SelectionMode currentMode() const;

private:
    Q_DECLARE_PRIVATE(KoShapeRubberSelectStrategy)
};

#endif
