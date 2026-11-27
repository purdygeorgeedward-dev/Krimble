/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STARSHAPECONFIGWIDGET_H
#define STARSHAPECONFIGWIDGET_H

#include <ui_StarShapeConfigWidget.h>

#include <KoShapeConfigWidgetBase.h>

class StarShape;

class StarShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    StarShapeConfigWidget();
    void open(KoShape *shape) override;
    void save() override;
    void setUnit(const KoUnit &unit) override;
    bool showOnShapeCreate() override
    {
        return false;
    }
    KUndo2Command *createCommand() override;

private Q_SLOTS:
    void typeChanged();
private:
    Ui::StarShapeConfigWidget widget;
    StarShape *m_star {nullptr};
};

#endif
