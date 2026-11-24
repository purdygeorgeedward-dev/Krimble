/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOPARAMETERCHANGESTRATEGY_H
#define KOPARAMETERCHANGESTRATEGY_H

#include "kritaflake_export.h"
#include "KoInteractionStrategy.h"
#include <QPointF>

class KoParameterShape;
class KoParameterChangeStrategyPrivate;

class KRITAFLAKE_EXPORT KoParameterChangeStrategy : public KoInteractionStrategy
{
public:
    KoParameterChangeStrategy(KoToolBase *tool, KoParameterShape *parameterShape, int handleId);
    ~KoParameterChangeStrategy() override;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    KUndo2Command* createCommand() override;

protected:
    KoParameterChangeStrategy(KoParameterChangeStrategyPrivate &);

private:
    Q_DECLARE_PRIVATE(KoParameterChangeStrategy)
};

#endif
