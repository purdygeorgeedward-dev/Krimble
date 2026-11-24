/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_UPDATERPRIVATE__P_H
#define KO_UPDATERPRIVATE__P_H

#include <QObject>
#include <QPointer>
#include <QTime>

class KoUpdater;

class KoUpdaterPrivate : public QObject
{

    Q_OBJECT

public:

    KoUpdaterPrivate(int weight, const QString& name, bool isPersistent = false);

    ~KoUpdaterPrivate() override;

    bool interrupted() const { return m_interrupted; }

    int progress() const { return m_progress; }

    int weight() const { return m_weight; }

    QString autoNestedName() const;
    QString subTaskName() const;
    QString mergedSubTaskName() const;

    bool hasValidRange() const;
    bool isPersistent() const;
    bool isCompleted() const;

    QPointer<KoUpdater> connectedUpdater() const;

public Q_SLOTS:

    void cancel();

    void setInterrupted(bool value = true);

    void setProgress( int percent );

    void setAutoNestedName(const QString &name);
    void setHasValidRange(bool value);

Q_SIGNALS:

    void sigUpdated();

    void sigCancelled();

private:
    int m_progress;
    int m_weight;
    bool m_interrupted;
    QString m_autoNestedName;
    QString m_subTaskName;
    bool m_hasValidRange;
    bool m_isPersistent;
    QPointer<KoUpdater> m_connectedUpdater;
};

#endif
