/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_PAINTING_ASSISTANTS_MANAGER_H_
#define _KIS_PAINTING_ASSISTANTS_MANAGER_H_

#include <QPointF>
#include <QColor>

#include "KoPointerEvent.h"
#include "KoSnapGuide.h"
#include "kis_icon_utils.h"
#include "canvas/kis_canvas_decoration.h"
#include "kis_painting_assistant.h"
#include <kritaui_export.h>

class KisView;
class KoColorDisplayRendererInterface;

class KisPaintingAssistantsDecoration;
typedef KisSharedPtr<KisPaintingAssistantsDecoration> KisPaintingAssistantsDecorationSP;

struct AssistantEditorData {
    unsigned int buttoncount = 6;
    const int buttonSize = 24;
    bool moveButtonActivated = true;
    bool snapButtonActivated = true;
    bool lockButtonActivated = true;
    bool duplicateButtonActivated = true;
    bool deleteButtonActivated = true;
    bool widgetActivated = true;
    const int widgetOffset = 10;
    const int buttonPadding = 7;
    QPointF moveIconPosition = QPointF(0, 0);
    QPointF snapIconPosition = QPointF(0, 0);
    QPointF lockedIconPosition = QPointF(0, 0);
    QPointF duplicateIconPosition = QPointF(0, 0);
    QPointF deleteIconPosition = QPointF(0, 0);
    QSize boundingSize = QSize(0, 0);
    const int dragDecorationWidth = 15;
    const QIcon m_iconMove = KisIconUtils::loadIcon("transform-move");
    const QIcon m_iconSnapOn = KisIconUtils::loadIcon("visible");
    const QIcon m_iconSnapOff = KisIconUtils::loadIcon("novisible");
    const QIcon m_iconLockOn = KisIconUtils::loadIcon("layer-locked");
    const QIcon m_iconLockOff = KisIconUtils::loadIcon("layer-unlocked");
    const QIcon m_iconDuplicate = KisIconUtils::loadIcon("duplicateitem");
    const QIcon m_iconDelete = KisIconUtils::loadIcon("deletelayer");
    const int horizontalButtonLimit = 3;
};

class KRITAUI_EXPORT KisPaintingAssistantsDecoration : public KisCanvasDecoration
{
    Q_OBJECT
public:
    KisPaintingAssistantsDecoration(QPointer<KisView> parent);
    ~KisPaintingAssistantsDecoration() override;
    void addAssistant(KisPaintingAssistantSP assistant);
    void raiseAssistant(KisPaintingAssistantSP assistant);
    void removeAssistant(KisPaintingAssistantSP assistant);
    void removeAll();
    void setAssistants(const QList<KisPaintingAssistantSP> &assistants);
    QPointF adjustPosition(const QPointF& point, const QPointF& strokeBegin);
    void adjustLine(QPointF &point, QPointF& strokeBegin);
    void setAdjustedBrushPosition(const QPointF position);
    void endStroke();
    QList<KisPaintingAssistantHandleSP> handles();
    QList<KisPaintingAssistantSP> assistants() const;
    struct AssistantEditorData globalEditorWidgetData;
    bool hasPaintableAssistants() const;

    KisPaintingAssistantSP selectedAssistant();
    void setSelectedAssistant(KisPaintingAssistantSP assistant);
    void deselectAssistant();

    void activateAssistantsEditor();

    void deactivateAssistantsEditor();

    bool isEditingAssistants();

    void setAssistantVisible(bool set);

    void setOutlineVisible(bool set);

    void setOnlyOneAssistantSnap(bool assistant);

    void setEraserSnap(bool assistant);

    bool assistantVisibility();

    bool outlineVisibility();

    void uncache();

    int handleSize();
    void setHandleSize(int handleSize);

    QColor globalAssistantsColor();
    void setGlobalAssistantsColor(QColor color);

Q_SIGNALS:
    void assistantChanged();
    void selectedAssistantChanged();

public Q_SLOTS:

    void toggleAssistantVisible();

    void toggleOutlineVisible();
    QPointF snapToGuide(KoPointerEvent *e, const QPointF &offset, bool useModifiers);
    QPointF snapToGuide(const QPointF& pt, const QPointF &offset);

    void slotUpdateDecorationVisibility();
    void slotConfigChanged();

protected:
    void drawDecoration(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter,KisCanvas2* canvas) override;
    void drawHandles(KisPaintingAssistantSP assistant, QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *renderInterface);
    void drawEditorWidget(KisPaintingAssistantSP assistant, QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *renderInterface);

private:
    struct Private;
    Private* const d;
};

#endif
