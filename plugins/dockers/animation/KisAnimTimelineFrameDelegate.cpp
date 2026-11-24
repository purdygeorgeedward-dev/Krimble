/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisAnimTimelineFrameDelegate.h"

#include <QPen>
#include <QPainter>
#include <QApplication>
#include <QSvgRenderer>
#include <kis_painting_tweaks.h>
#include "KisAnimTimelineFramesModel.h"
#include "KisAnimTimelineColors.h"

#include "kis_node_view_color_scheme.h"

KisAnimTimelineFrameDelegate::KisAnimTimelineFrameDelegate(QObject *parent)
    : QItemDelegate(parent),
      stripes(64, 64)
{
    KisNodeViewColorScheme scm;
    labelColors = scm.allColorLabels();

    QImage stripesImage(":diagonal-stripe.svg", "svg");
    stripesImage.save("/tmp/krita_stripes.svg", "svg");
    stripes = QPixmap::fromImage(stripesImage);
}

KisAnimTimelineFrameDelegate::~KisAnimTimelineFrameDelegate()
{
}

void KisAnimTimelineFrameDelegate::paintActiveFrameSelector(QPainter *painter, const QRect &rc, bool isCurrentFrame)
{
    painter->save();

    QColor lineColor = KisAnimTimelineColors::instance()->selectorColor();
    const int lineWidth = rc.width() > 20 ? 4 : 2;

    const int x0 = rc.x();
    const int y0 = rc.y();
    const int x1 = rc.right();
    const int y1 = rc.bottom();

    QVector<QLine> linesDark;
    linesDark << QLine(x0 + lineWidth / 2, y0, x0  + lineWidth / 2, y1);
    linesDark << QLine(x1 -  lineWidth / 2 + 1, y0, x1 - lineWidth / 2 + 1, y1);

    QPen oldPen = painter->pen();
    painter->setPen(QPen(lineColor, lineWidth));
    painter->drawLines(linesDark);
    painter->setPen(oldPen);

    if (isCurrentFrame) {
        QPen oldPen = painter->pen();
        QBrush oldBrush(painter->brush());

        painter->setPen(QPen(lineColor, 0));
        painter->setBrush(lineColor);

        painter->drawEllipse(rc.center(), 2,2);

        painter->setBrush(oldBrush);
        painter->setPen(oldPen);
    }

    painter->restore();
}

void KisAnimTimelineFrameDelegate::paintSpecialKeyframeIndicator(QPainter *painter, const QModelIndex &index, const QRect &rc) const
{
    painter->save();

    bool doesFrameExist = index.data(KisAnimTimelineFramesModel::FrameExistsRole).toBool();
    bool isEditable = index.data(KisAnimTimelineFramesModel::FrameEditableRole).toBool();
    bool hasContent = index.data(KisAnimTimelineFramesModel::FrameHasContent).toBool();

    QColor color = qApp->palette().color(QPalette::Highlight);
    QColor baseColor = qApp->palette().color(QPalette::Base);
    QColor noLabelSetColor = qApp->palette().color(QPalette::Highlight);

    QVariant colorLabel = index.data(KisAnimTimelineFramesModel::FrameColorLabelIndexRole);
    if (colorLabel.isValid()) {
        color = labelColors.at(colorLabel.toInt());
    } else {
        color = noLabelSetColor;
    }

    if (!isEditable) {
        color = KisPaintingTweaks::blendColors(baseColor, color, 0.5);
    }

    if (doesFrameExist && hasContent) {
        color = baseColor;
    }

    QPen oldPen = painter->pen();
    QBrush oldBrush(painter->brush());

    painter->setPen(QPen(color, 0));
    painter->setBrush(color);

    QPointF center = rc.center();
    QPointF points[4] = {
        QPointF(center.x() + 4, center.y()   ),
        QPointF(center.x()    , center.y() - 4),
        QPointF(center.x() - 4, center.y()   ),
        QPointF(center.x()    , center.y() + 4)
    };
    painter->drawConvexPolygon(points, 4);

    painter->setBrush(oldBrush);
    painter->setPen(oldPen);

    painter->restore();
}

void KisAnimTimelineFrameDelegate::drawBackground(QPainter *painter, const QModelIndex &index, const QRect &rc) const
{
    painter->save();

    bool hasActiveLayerRole = index.data(KisAnimTimelineFramesModel::ActiveLayerRole).toBool();
    bool doesFrameExist = index.data(KisAnimTimelineFramesModel::FrameExistsRole).toBool();
    bool isEditable = index.data(KisAnimTimelineFramesModel::FrameEditableRole).toBool();
    bool hasContent = index.data(KisAnimTimelineFramesModel::FrameHasContent).toBool();

    QColor color;
    QColor noLabelSetColor = qApp->palette().color(QPalette::Highlight);
    QColor highlightColor = qApp->palette().color(QPalette::Highlight);
    QColor baseColor = qApp->palette().color(QPalette::Base);

    if (hasActiveLayerRole) {
        color = KisPaintingTweaks::blendColors(baseColor, highlightColor, 0.8);
        painter->fillRect(rc, color);
    } else {
        color = KisPaintingTweaks::blendColors(baseColor, highlightColor, 0.95);
        painter->fillRect(rc, color);
    }

    QVariant colorLabel = index.data(KisAnimTimelineFramesModel::FrameColorLabelIndexRole);
    if (colorLabel.isValid()) {
        color = labelColors.at(colorLabel.toInt());
    } else {
        color = noLabelSetColor;
    }

    if (!isEditable) {
        color = KisPaintingTweaks::blendColors(baseColor, color, 0.7);
    }

    if (hasContent && doesFrameExist) {
        painter->fillRect(rc, color);
    }

    if (doesFrameExist && !hasContent) {

        QPen oldPen = painter->pen();
        QBrush oldBrush(painter->brush());

        painter->setPen(QPen(color, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rc);

        painter->setBrush(oldBrush);
        painter->setPen(oldPen);
    }

    if (!doesFrameExist && hasContent) {

         if (colorLabel.toInt() == 0) {
             color = noLabelSetColor;

             if (!isEditable) {
                 color = KisPaintingTweaks::blendColors(baseColor, color, 0.7);
             }
         }

        QPoint lineStart(rc.x(), (rc.y() + rc.height()/2));
        QPoint lineEnd(rc.x() + rc.width(), (rc.y() + rc.height()/2));

        QPen holdFramePen(color);
        holdFramePen.setWidth(1);

        painter->setPen(holdFramePen);
        painter->drawLine(QLine(lineStart, lineEnd));
    }

    painter->restore();
}

void KisAnimTimelineFrameDelegate::drawFocus(QPainter *painter,
                                           const QStyleOptionViewItem &option,
                                           const QRect &rect) const
{
    if ((option.state & QStyle::State_HasFocus) == 0 || !rect.isValid())
        return;

    painter->save();

    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(option);
    o.rect = rect;
    o.state |= QStyle::State_KeyboardFocusChange;
    o.state |= QStyle::State_Item;
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
    o.backgroundColor = option.palette.color(cg, (option.state & QStyle::State_Selected)
                                             ? QPalette::Highlight : QPalette::Window);
    const QWidget *widget = qobject_cast<QWidget*>(parent());
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);

    painter->restore();
}

void KisAnimTimelineFrameDelegate::drawCloneGraphics(QPainter *painter, const QRect &rect) const
{
    painter->save();

    QBrush brush(stripes);
    brush.setStyle(Qt::TexturePattern);

    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->setOpacity(0.25f);
    painter->drawRect(rect);

    painter->restore();
}

void KisAnimTimelineFrameDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    drawBackground(painter, index, option.rect);

    if (index.data(KisAnimTimelineFramesModel::CloneOfActiveFrame).toBool() && index.data(KisAnimTimelineFramesModel::ActiveLayerRole).toBool()) {
        drawCloneGraphics(painter, option.rect);
    }

    if (option.showDecorationSelected &&
        (option.state & QStyle::State_Selected)) {
        painter->save();

        const QVariant data = index.data(KisAnimTimelineFramesModel::FrameEditableRole);
        bool isEditable = data.isValid() ? data.toBool() : true;

        QColor highlightColor = KisAnimTimelineColors::instance()->selectionColor();
        highlightColor.setAlpha(isEditable ? 128 : 64);
        QBrush brush = highlightColor;
        painter->fillRect(option.rect, brush);

        painter->restore();
    }

    drawFocus(painter, option, option.rect);

    bool specialKeys = index.data(KisAnimTimelineFramesModel::SpecialKeyframeExists).toBool();
    if (specialKeys) {
        paintSpecialKeyframeIndicator(painter, index, option.rect);
    }

    bool active = index.data(KisAnimTimelineFramesModel::ActiveFrameRole).toBool();
    bool layerIsCurrent = index.data(KisAnimTimelineFramesModel::ActiveLayerRole).toBool();
    if (active) {
        paintActiveFrameSelector(painter, option.rect, layerIsCurrent);
    }

    {
        if (index.data(KisAnimTimelineFramesModel::WithinClipRange).toBool() == false) {
            painter->save();

            painter->setOpacity(0.50f);
            painter->fillRect(option.rect, qApp->palette().color(QPalette::Base).darker(110));

            painter->restore();
        }
    }
}
