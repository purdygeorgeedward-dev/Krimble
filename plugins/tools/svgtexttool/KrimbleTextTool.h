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
#include <QRectF>

class KoSvgTextShape;

class KrimbleTextTool : public KoToolBase
{
public:
    explicit KrimbleTextTool(KoCanvasBase *canvas);

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void deactivate() override;   // Krimble 2026-10-24: the frame of the chosen text goes away

private:
    // Krimble 2026-10-24: George: "it would be nice if the text frame on screen had gadgets like crop to resize it or move it around." The text that was just made or tapped is
    // chosen: a frame with handles is drawn around it. Dragging inside the frame moves the text; the handles in the middle of the left and right edges change the width of its text box;
    // the corner handles make the text bigger or smaller.
    enum Handle { NoHandle, MoveHandle, LeftHandle, RightHandle, TopLeftHandle, TopRightHandle, BottomLeftHandle, BottomRightHandle };
    KoSvgTextShape *m_selected = nullptr;
    Handle m_handle = NoHandle;
    QRectF m_frameStart;     // the frame when a handle was pressed (document coordinates)
    QRectF m_frameNow;       // the frame while it is dragged
    bool selectedStillThere() const;
    QRectF selectedFrame() const;
    Handle handleAt(const QPointF &documentPoint) const;
    QRectF frameForDrag(Handle handle, const QRectF &start, const QPointF &delta) const;
    void applyFrameChange(Handle handle, const QRectF &from, const QRectF &to);
    void repaintFrame(const QRectF &frame);

    // void askAndAddText(const QPointF &where);
    void askAndAddText(const QPointF &where, const QRectF &box = QRectF());   // Krimble 2026-10-20: box = a dragged text box (or none)

    // Krimble 2026-10-20: dragging a text box
    QPointF m_pressPoint;
    QPointF m_dragPoint;
    bool m_dragging = false;
};

class KrimbleTextToolFactory : public KoToolFactoryBase
{
public:
    KrimbleTextToolFactory();
    ~KrimbleTextToolFactory() override;
    KoToolBase *createTool(KoCanvasBase *canvas) override;
};

#endif // KRIMBLE_TEXT_TOOL_H
