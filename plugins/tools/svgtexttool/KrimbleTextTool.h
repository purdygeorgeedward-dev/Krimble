/*
 * Krimble 2026-10-18: the new Type tool, designed from scratch (George: "The existing type tool is garbage. I'm letting you design the new one from scratch. Just
 * get rid of this crap version."). Tap the canvas with it and an ordinary window opens: a text box to type in, the font, the size, bold, italic, the alignment and
 * the color. OK puts the text on the picture as vector text (a KoSvgTextShape, in a vector layer). There is no editing on the canvas: no cursor, no handles, and
 * nothing for Android's own text selection controls to fight with. The old tool (SvgTextTool) is no longer registered; its code is left in the plugin, unused.
 */
#ifndef KRIMBLE_TEXT_TOOL_H
#define KRIMBLE_TEXT_TOOL_H

#include <KoToolBase.h>
#include <KoToolFactoryBase.h>
#include <QPointF>

class KrimbleTextTool : public KoToolBase
{
public:
    explicit KrimbleTextTool(KoCanvasBase *canvas);

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

private:
    void askAndAddText(const QPointF &where);
};

class KrimbleTextToolFactory : public KoToolFactoryBase
{
public:
    KrimbleTextToolFactory();
    ~KrimbleTextToolFactory() override;
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // KRIMBLE_TEXT_TOOL_H
