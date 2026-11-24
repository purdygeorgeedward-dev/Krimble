/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef RECTANGLESHAPECONFIGWIDGET_H
#define RECTANGLESHAPECONFIGWIDGET_H

#include <ui_RectangleShapeConfigWidget.h>

#include <KoShapeConfigWidgetBase.h>
#include <KoShape.h>

class RectangleShape;

class RectangleShapeConfigWidget : public KoShapeConfigWidgetBase, public KoShape::ShapeChangeListener
{
    Q_OBJECT
public:
    RectangleShapeConfigWidget();
    void open(KoShape *shape) override;
    void save() override;
    void setUnit(const KoUnit &unit) override;
    bool showOnShapeCreate() override
    {
        return false;
    }
    KUndo2Command *createCommand() override;

    void notifyShapeChanged(KoShape::ChangeType type, KoShape *shape) override;

private:
    void loadPropertiesFromShape(RectangleShape *shape);

private:
    Ui::RectangleShapeConfigWidget widget;
    RectangleShape *m_rectangle;
};

#endif
