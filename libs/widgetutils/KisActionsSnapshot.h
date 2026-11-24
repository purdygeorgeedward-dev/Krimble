/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISACTIONSSNAPSHOT_H
#define KISACTIONSSNAPSHOT_H

#include <kritawidgetutils_export.h>

#include <QScopedPointer>
#include <QMap>

class QAction;
class KisKActionCollection;

class KRITAWIDGETUTILS_EXPORT KisActionsSnapshot
{
public:
    KisActionsSnapshot();
    ~KisActionsSnapshot();

    void addAction(const QString &name, QAction *action);

    QMap<QString, KisKActionCollection*> actionCollections();

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
