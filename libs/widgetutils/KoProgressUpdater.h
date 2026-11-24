/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPROGRESSUPDATER_H
#define KOPROGRESSUPDATER_H

#include "kritawidgetutils_export.h"

#include <QString>
#include <QObject>
#include <QPointer>

class KoUpdater;
class KoProgressProxy;
class QTextStream;
class QTime;

class KRITAWIDGETUTILS_EXPORT KoProgressUpdater : public QObject
{
    Q_OBJECT
public:

    enum Mode {
        Threaded,
        Unthreaded
    };

    explicit KoProgressUpdater(KoProgressProxy *progressProxy, Mode mode = Threaded);

    explicit KoProgressUpdater(QPointer<KoUpdater> updater);

    ~KoProgressUpdater() override;

    void start(int range = 100, const QString &text = "");

    QPointer<KoUpdater> startSubtask(int weight=1,
                                     const QString &name = QString(), bool isPersistent = false);

    void removePersistentSubtask(QPointer<KoUpdater> updater);

    bool interrupted() const;

    void setUpdateInterval(int ms);
    int updateInterval() const;

    void setAutoNestNames(bool value);
    bool autoNestNames() const;

public Q_SLOTS:
    void cancel();

private Q_SLOTS:

    void update();
    void updateUi();

Q_SIGNALS:
    void triggerUpdateAsynchronously();

private:

    class Private;
    Private *const d;

};

#endif

