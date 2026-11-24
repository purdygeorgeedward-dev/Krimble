/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOFONTCHANGETRACKER_H
#define KOFONTCHANGETRACKER_H

#include <QObject>

class KoFontChangeTracker : public QObject
{
    Q_OBJECT
public:
    explicit KoFontChangeTracker(QStringList paths, QObject *parent = nullptr);
    ~KoFontChangeTracker();

    void resetChangeTracker();
Q_SIGNALS:
    void sigUpdateConfig();
private Q_SLOTS:
    void directoriesChanged();
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
