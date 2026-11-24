/* This file is part of the KDE libraries

   SPDX-FileCopyrightText: 2008 Michael Jansen <kde@michael-jansen.biz>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KACTIONCATEGORY_H
#define KACTIONCATEGORY_H

#include <kritawidgetutils_export.h>
#include "config-xmlgui.h"
#include <QObject>
#include <QString>
#include <QList>

#include <kstandardaction.h>
#include "kactioncollection.h"

struct KisKActionCategoryPrivate;

class QAction;

class KRITAWIDGETUTILS_EXPORT KisKActionCategory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)

public:

    explicit KisKActionCategory(const QString &text, KisKActionCollection *parent = 0);

    ~KisKActionCategory() override;

    QAction *addAction(const QString &name, QAction *action);
    QAction *addAction(
        KStandardAction::StandardAction actionType,
        const QObject *receiver = 0,
        const char *member = 0);

    QAction *addAction(
        KStandardAction::StandardAction actionType,
        const QString &name,
        const QObject *receiver = 0,
        const char *member = 0);

    QAction *addAction(
        const QString &name,
        const QObject *receiver = 0,
        const char *member = 0);

    template<class ActionType>
    ActionType *add(
        const QString &name,
        const QObject *receiver = 0,
        const char *member = 0)
    {
        ActionType *action = collection()->add<ActionType>(name, receiver, member);
        addAction(action);
        return action;
    }

    const QList<QAction *> actions() const;

    KisKActionCollection *collection() const;

    QString text() const;

    void setText(const QString &text);

private:

    void unlistAction(QAction *action);

    void addAction(QAction *action);

    friend class KisKActionCollectionPrivate;

    KisKActionCategoryPrivate *const d;
};

#endif
