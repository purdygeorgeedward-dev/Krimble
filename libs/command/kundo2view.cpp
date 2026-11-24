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
#include "kundo2stack.h"
#include "kundo2view.h"
#include "kundo2model.h"
#include "kundo2group.h"

#ifndef QT_NO_UNDOVIEW

#include <QAbstractItemModel>
#include <QPointer>
#include <QIcon>

class KUndo2ViewPrivate
{
public:
    KUndo2ViewPrivate() :
#ifndef QT_NO_UNDOGROUP
        group(0),
#endif
        model(0) {}

#ifndef QT_NO_UNDOGROUP
    QPointer<KUndo2Group> group;
#endif
    KUndo2Model *model {0};
    KUndo2View* q {0};

    void init(KUndo2View* view);
};

void KUndo2ViewPrivate::init(KUndo2View* view)
{
    q = view;
    model = new KUndo2Model(q);
    q->setModel(model);
    q->setSelectionModel(model->selectionModel());
}

KUndo2View::KUndo2View(QWidget *parent) : QListView(parent), d(new KUndo2ViewPrivate)
{
    d->init(this);
}

KUndo2View::KUndo2View(KUndo2QStack *stack, QWidget *parent) : QListView(parent), d(new KUndo2ViewPrivate)
{
    d->init(this);
    setStack(stack);
}

#ifndef QT_NO_UNDOGROUP

KUndo2View::KUndo2View(KUndo2Group *group, QWidget *parent) : QListView(parent), d(new KUndo2ViewPrivate)
{
    d->init(this);
    setGroup(group);
}

#endif

KUndo2View::~KUndo2View()
{
    delete d;
}

KUndo2QStack *KUndo2View::stack() const
{

    return d->model->stack();
}

void KUndo2View::setStack(KUndo2QStack *stack)
{

#ifndef QT_NO_UNDOGROUP
    setGroup(0);
#endif
    d->model->setStack(stack);
}

#ifndef QT_NO_UNDOGROUP

void KUndo2View::setGroup(KUndo2Group *group)
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
        d->model->setStack((KUndo2QStack *)d->group->activeStack());
    } else {
        d->model->setStack(0);
    }
}

KUndo2Group *KUndo2View::group() const
{

    return d->group;
}

#endif

void KUndo2View::setEmptyLabel(const QString &label)
{

    d->model->setEmptyLabel(label);
}

QString KUndo2View::emptyLabel() const
{

    return d->model->emptyLabel();
}

void KUndo2View::setCleanIcon(const QIcon &icon)
{

    d->model->setCleanIcon(icon);

}

QIcon KUndo2View::cleanIcon() const
{

    return d->model->cleanIcon();
}

#endif
