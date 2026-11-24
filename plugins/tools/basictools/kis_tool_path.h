/*
 *  SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_PATH_H_
#define KIS_TOOL_PATH_H_

#include <KoCreatePathTool.h>
#include <KoToolFactoryBase.h>

#include "flake/kis_node_shape.h"
#include "kis_tool_shape.h"
#include "kis_delegated_tool.h"
#include <kis_icon.h>

class KoCanvasBase;
class KisToolPath;

class __KisToolPathLocalTool : public KoCreatePathTool {
public:
    __KisToolPathLocalTool(KoCanvasBase * canvas, KisToolPath* parentTool);

    void paintPath(KoPathShape &path, QPainter &painter, const KoViewConverter &converter) override;
    void addPathShape(KoPathShape* pathShape) override;

    using KoCreatePathTool::createOptionWidgets;
    using KoCreatePathTool::endPathWithoutLastPoint;
    using KoCreatePathTool::endPath;
    using KoCreatePathTool::cancelPath;
    using KoCreatePathTool::removeLastPoint;

private:
    KisToolPath* const m_parentTool;
};

typedef KisDelegatedTool<KisToolShape,
                         __KisToolPathLocalTool,
                         DeselectShapesActivationPolicy> DelegatedPathTool;

class KisToolPath : public DelegatedPathTool
{
    Q_OBJECT

public:
    KisToolPath(KoCanvasBase * canvas);
    void mousePressEvent(KoPointerEvent *event) override;

    QList< QPointer<QWidget> > createOptionWidgets() override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    void beginPrimaryAction(KoPointerEvent* event) override;
    void continuePrimaryAction(KoPointerEvent *event) override;
    void endPrimaryAction(KoPointerEvent *event) override;

    void beginAlternateAction(KoPointerEvent *event, AlternateAction action) override;

    void beginPrimaryDoubleClickAction(KoPointerEvent* event) override;

    KisPopupWidgetInterface* popupWidget() override;

protected:
    void requestStrokeCancellation() override;
    void requestStrokeEnd() override;

protected Q_SLOTS:
    void resetCursorStyle() override;

private:
    friend class __KisToolPathLocalTool;
};

class KisToolPathFactory : public KisToolPaintFactoryBase
{

public:
    KisToolPathFactory()
            : KisToolPaintFactoryBase("KisToolPath") {
        setToolTip(i18n("Pen Tool: Shift-mouseclick ends the curve."));
        setSection(ToolBoxSection::PSOrder);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
        setIconName(koIconNameCStr("krita_draw_path"));
        setPriority(7);
        setToolBoxPriority(150);
    }

    ~KisToolPathFactory() override {}

    KoToolBase * createTool(KoCanvasBase *canvas) override {
        return new KisToolPath(canvas);
    }
};

#endif
