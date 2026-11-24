/*
 * SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KIS_ASSISTANT_TOOL_H_
#define _KIS_ASSISTANT_TOOL_H_

#include <QPointer>

#include <KoToolFactoryBase.h>
#include <KoIcon.h>

#include <kis_tool.h>
#include "kis_painting_assistant.h"
#include <kis_icon.h>
#include <kis_canvas2.h>

#include "ui_AssistantsToolOptions.h"

class KisAssistantTool : public KisTool
{
    Q_OBJECT
    enum PerspectiveAssistantEditionMode {
        MODE_CREATION,
        MODE_EDITING,
        MODE_DRAGGING_NODE,
        MODE_DRAGGING_TRANSLATING_TWONODES,
        MODE_DRAGGING_EDITOR_WIDGET
    };
public:
    KisAssistantTool(KoCanvasBase * canvas);
    ~KisAssistantTool() override;

    virtual quint32 priority() {
        return 3;
    }

    void beginPrimaryAction(KoPointerEvent *event) override;

    void beginAlternateAction(KoPointerEvent *event, KisTool::AlternateAction action) override;

    void continuePrimaryAction(KoPointerEvent *event) override;
    void endPrimaryAction(KoPointerEvent *event) override;
    void continueAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void endAlternateAction(KoPointerEvent *event, AlternateAction action) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QWidget *createOptionWidget() override;

private:
    void addAssistant();
    void removeAssistant(KisPaintingAssistantSP assistant);

    void assistantSelected(KisPaintingAssistantSP assistant);

    bool snap(KoPointerEvent *event);

    bool ToggleDockToolOptionsVisible = false;
    void beginActionImpl(KoPointerEvent *event);
    void continueActionImpl(KoPointerEvent *event);
    void endActionImpl(KoPointerEvent *event);
    void updateEditorWidgetData();
public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;

    void slotChangeVanishingPointAngle(double value);
    void slotChangeTwoPointDensity(double value);
    void slotChangeTwoPointUseVertical(int value);
    void slotChangeSubdivisions(int value);
    void slotChangeMinorSubdivisions(int value);
    void slotEnableFixedLength(int enabled);
    void slotChangeFixedLength(double value);

private Q_SLOTS:
    void removeAllAssistants();
    void saveAssistants();
    void loadAssistants();
    void updateToolOptionsUI();

    void slotGlobalAssistantsColorChanged(const QColor&);
    void slotGlobalAssistantOpacityChanged();

    void slotUpdateCustomColor();
    void slotCustomOpacityChanged();
    void slotLocalAssistantCheckboxChanged();

    void slotSelectedAssistantTypeChanged();
    void slotChangeFixedLengthUnit(int index);

    void slotToggleDockToolOptionsVisible();

    void slotToggleMoveButton(int index);
    void slotToggleSnapButton(int index);
    void slotToggleLockButton(int index);
    void slotToggleDuplicateButton(int index);
    void slotToggleDeleteButton(int index);

protected:
    void paint(QPainter& gc, const KoViewConverter &converter) override;

protected:
    QPointer<KisCanvas2> m_canvas;

    QList<KisPaintingAssistantHandleSP> m_handles;
    QList<KisPaintingAssistantHandleSP> m_sideHandles;
    KisPaintingAssistantHandleSP m_handleDrag;
    KisPaintingAssistantHandleSP m_handleCombine;
    KisPaintingAssistantHandleSP m_handleHover;
    KisPaintingAssistantSP m_assistantDrag;

    KisPaintingAssistantSP m_newAssistant;

    QPointF m_cursorStart;
    QPointF m_currentAdjustment;
    QPointF m_previousHandlePos;
    Ui::AssistantsToolOptions m_options;
    QWidget* m_optionsWidget {0};
    QPointF m_dragStart;
    QLineF m_radius;
    bool m_snapIsRadial {false};
    QPointF m_dragEnd;
    int m_handleSize {17};
    int m_handleMaxDist {81};
    bool assistantDuplicatingFlag {false};

private:
    void drawEditorWidget(KisPaintingAssistantSP assistant, QPainter& _gc);
    PerspectiveAssistantEditionMode m_internalMode { MODE_CREATION };
    KisPaintingAssistantHandleSP m_selectedNode1, m_selectedNode2, m_highlightedNode;
    int m_assistantHelperYOffset {10};
    QList<KisPaintingAssistantSP> m_origAssistantList;
    KisSpinBoxUnitManager* m_unitManager {0};
};

class KisAssistantToolFactory : public KoToolFactoryBase
{
public:
    KisAssistantToolFactory()
            : KoToolFactoryBase("KisAssistantTool") {
        setToolTip(i18n("Assistant Tool"));
        setSection(ToolBoxSection::View);
        setIconName(koIconNameCStr("krita_tool_assistant"));
        setPriority(0);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisAssistantToolFactory() override {}

    KoToolBase * createTool(KoCanvasBase * canvas) override {
        return new KisAssistantTool(canvas);
    }

};

#endif

