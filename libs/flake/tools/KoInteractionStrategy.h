/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007, 2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOINTERACTIONSTRATEGY_H
#define KOINTERACTIONSTRATEGY_H

#include "kritaflake_export.h"

#include <Qt>

class KoPointerEvent;
class KoViewConverter;
class KoInteractionStrategyPrivate;
class KoToolBase;
class KUndo2Command;
class QPointF;
class QPainter;
class KoColorDisplayRendererInterface;

class KRITAFLAKE_EXPORT KoInteractionStrategy
{
public:
    explicit KoInteractionStrategy(KoToolBase *parent);
    virtual ~KoInteractionStrategy();

    virtual void paint(QPainter &painter, const KoViewConverter &converter, const KoColorDisplayRendererInterface *displayRendererInterface);
    virtual void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) = 0;

    virtual KUndo2Command *createCommand() = 0;
    virtual void cancelInteraction();
    virtual void finishInteraction(Qt::KeyboardModifiers modifiers) = 0;

    KoToolBase *tool() const;

protected:
    KoInteractionStrategy(KoInteractionStrategyPrivate &);

    KoInteractionStrategyPrivate *d_ptr;

    uint handleRadius() const;

    uint decorationThickness() const;

    uint grabSensitivity() const;

private:
    Q_DECLARE_PRIVATE(KoInteractionStrategy)
};

#endif
