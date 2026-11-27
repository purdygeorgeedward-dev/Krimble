/* SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kactioncategory.h"
#include "config-xmlgui.h"
#include <QAction>
#include "kstandardaction.h"

struct KisKActionCategoryPrivate {

    KisKActionCategoryPrivate(KisKActionCategory *host);

    KisKActionCategory *q;

    QString text;

    QList<QAction *> actions;

};

KisKActionCategory::KisKActionCategory(const QString &text, KisKActionCollection *parent)
    :   QObject(parent)
    , d(new KisKActionCategoryPrivate(this))
{
    d->text = text;
}

KisKActionCategory::~KisKActionCategory()
{
    delete d;
}

const QList<QAction *> KisKActionCategory::actions() const
{
    return d->actions;
}

QAction *KisKActionCategory::addAction(const QString &name, QAction *action)
{
    collection()->addAction(name, action);
    addAction(action);
    return action;
}

QAction *KisKActionCategory::addAction(
    KStandardAction::StandardAction actionType,
    const QObject *receiver,
    const char *member)
{
    QAction *action = collection()->addAction(actionType, receiver, member);
    addAction(action);
    return action;
}

QAction *KisKActionCategory::addAction(
    KStandardAction::StandardAction actionType,
    const QString &name,
    const QObject *receiver,
    const char *member)
{
    QAction *action = collection()->addAction(actionType, name, receiver, member);
    addAction(action);
    return action;
}

QAction *KisKActionCategory::addAction(
    const QString &name,
    const QObject *receiver,
    const char *member)
{
    QAction *action = collection()->addAction(name, receiver, member);
    addAction(action);
    return action;
}

void KisKActionCategory::addAction(QAction *action)
{
    if (!d->actions.contains(action)) {
        d->actions.append(action);
    }
}

KisKActionCollection *KisKActionCategory::collection() const
{
    return qobject_cast<KisKActionCollection *>(parent());
}

QString KisKActionCategory::text() const
{
    return d->text;
}

void KisKActionCategory::setText(const QString &text)
{
    d->text = text;
}

void KisKActionCategory::unlistAction(QAction *action)
{

    int index = d->actions.indexOf(action);

    if (index == -1) {
        return;
    }

    d->actions.takeAt(index);
}

KisKActionCategoryPrivate::KisKActionCategoryPrivate(KisKActionCategory *host)
    : q(host)
{}

