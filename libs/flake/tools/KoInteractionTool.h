/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOINTERACTIONTOOL_H
#define KOINTERACTIONTOOL_H

#include "KoToolBase.h"
#include "kritaflake_export.h"

class KoInteractionStrategy;
class KoInteractionStrategyFactory;
class KoInteractionToolPrivate;

#define KoInteractionTool_ID "InteractionTool"

class KRITAFLAKE_EXPORT KoInteractionTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KoInteractionTool(KoCanvasBase *canvas);
    ~KoInteractionTool() override;

public:
    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

protected:
    KoInteractionTool(KoInteractionToolPrivate &dd);

    KoInteractionStrategy *currentStrategy();
    void cancelCurrentStrategy();

    KoInteractionStrategy *createStrategyBase(KoPointerEvent *event);
    virtual KoInteractionStrategy *createStrategy(KoPointerEvent *event) = 0;

    void addInteractionFactory(KoInteractionStrategyFactory *factory);
    void removeInteractionFactory(const QString &id);
    bool hasInteractionFactory(const QString &id);

    bool tryUseCustomCursor();

private:
    KoInteractionTool(const KoInteractionTool&);
    KoInteractionTool& operator=(const KoInteractionTool&);

    Q_DECLARE_PRIVATE(KoInteractionTool)
};

#endif
