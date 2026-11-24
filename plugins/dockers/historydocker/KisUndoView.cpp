/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Matus Talcik <matus.talcik@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <kundo2qstack.h>
#include "KisUndoView.h"
#include "KisUndoModel.h"

#ifndef QT_NO_UNDOVIEW

#include <kundo2group.h>
#include <QAbstractItemModel>
#include <QPointer>
#include <QIcon>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QWidgetAction>
#include <QGridLayout>
#include <kis_config.h>
#include <KisKineticScroller.h>

#include "kis_double_parse_spin_box.h"
#include "kis_int_parse_spin_box.h"

class KisUndoViewPrivate
{
public:
    KisUndoViewPrivate() {}

#ifndef QT_NO_UNDOGROUP
    QPointer<KUndo2Group> group {0};
#endif
    KisUndoModel *model {0};
    KisUndoView *q {0};

    void init(KisUndoView* view);
};

void KisUndoViewPrivate::init(KisUndoView* view)
{
    q = view;
    model = new KisUndoModel(q);
    model->setDevicePixelRatio(view->devicePixelRatioF());
    q->setModel(model);
    q->setSelectionModel(model->selectionModel());
}

KisUndoView::KisUndoView(QWidget *parent)
    : QListView(parent)
    , d(new KisUndoViewPrivate)
{
    d->init(this);

    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(this);
    if (scroller) {
        connect(scroller, SIGNAL(stateChanged(QScroller::State)),
                this, SLOT(slotScrollerStateChanged(QScroller::State)));
    }
}

KisUndoView::KisUndoView(KUndo2QStack *stack, QWidget *parent)
    : KisUndoView(parent)
{
    setStack(stack);
}

#ifndef QT_NO_UNDOGROUP

KisUndoView::KisUndoView(KUndo2Group *group, QWidget *parent)
    : KisUndoView(parent)
{
    setGroup(group);
}

#endif

KisUndoView::~KisUndoView()
{
    delete d;

}

KUndo2QStack *KisUndoView::stack() const
{

    return d->model->stack();
}

void KisUndoView::setStack(KUndo2QStack *stack)
{

#ifndef QT_NO_UNDOGROUP
    setGroup(0);
#endif
    d->model->setStack(stack);
}

#ifndef QT_NO_UNDOGROUP

void KisUndoView::setGroup(KUndo2Group *group)
{

    if (d->group == group)
        return;

    if (d->group != 0) {
        disconnect(d->group, SIGNAL(activeStackChanged(KUndo2QStack*)),
                d->model, SLOT(setStack(KUndo2QStack*)));
    }

    d->group = group;

    if (d->group != 0) {
        connect(d->group, SIGNAL(activeStackChanged(KUndo2QStack*)),
                d->model, SLOT(setStack(KUndo2QStack*)));
        d->model->setStack(d->group->activeStack());
    } else {
        d->model->setStack(0);
    }
}

KUndo2Group *KisUndoView::group() const
{

    return d->group;
}

#endif

void KisUndoView::setEmptyLabel(const QString &label)
{

    d->model->setEmptyLabel(label);
}

QString KisUndoView::emptyLabel() const
{

    return d->model->emptyLabel();
}

void KisUndoView::setCleanIcon(const QIcon &icon)
{

    d->model->setCleanIcon(icon);

}

QIcon KisUndoView::cleanIcon() const
{

    return d->model->cleanIcon();
}

void KisUndoView::setCanvas(KisCanvas2 *canvas) {
    d->model->setCanvas(canvas);
}

#endif
