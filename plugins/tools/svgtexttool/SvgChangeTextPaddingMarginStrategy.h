/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGCHANGETEXTPADDINGMARGINSTRATEGY_H
#define SVGCHANGETEXTPADDINGMARGINSTRATEGY_H

#include <KoInteractionStrategy.h>

#include <KoSvgTextShape.h>
#include <optional>

class SvgTextTool;
class KoPathShape;

class SvgChangeTextPaddingMarginStrategy : public KoInteractionStrategy
{
public:
    SvgChangeTextPaddingMarginStrategy(SvgTextTool *tool, KoSvgTextShape *shape, const QPointF &clicked);
    ~SvgChangeTextPaddingMarginStrategy();

    static std::optional<QPointF> hitTest(KoSvgTextShape *shape, const QPointF &mousePos, const qreal grabSensitivityInPts);
private:
    KoSvgTextShape *m_shape;
    KoPathShape *m_referenceShape;
    bool m_isPadding;
    QPointF m_lastMousePos;

public:
    void paint(QPainter &painter, const KoViewConverter &converter, const KoColorDisplayRendererInterface *displayRendererInterface) override;
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
};

#endif
