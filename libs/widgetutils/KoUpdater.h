/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_UPDATER_H
#define KO_UPDATER_H

#include "KoProgressProxy.h"
#include <QObject>
#include <QPointer>
#include <QAtomicInt>

class KoProgressUpdater;
class KoUpdaterPrivate;

class KRITAWIDGETUTILS_EXPORT KoUpdater : public QObject, public KoProgressProxy {

    Q_OBJECT

public:
    virtual ~KoUpdater();

    void cancel();

public Q_SLOTS:
    void setProgress(int percent);

public:
    bool interrupted() const;

    int progress() const;

public:

    int maximum() const override;
    void setValue( int value ) override;
    void setRange( int minimum, int maximum ) override;
    void setFormat( const QString & format ) override;
    void setAutoNestedName(const QString &name) override;

Q_SIGNALS:

    void sigCancel();

    void sigProgress( int percent );

    void sigNestedNameChanged(const QString &value);
    void sigHasValidRangeChanged(bool value);

protected:

    friend class KoUpdaterPrivate;
    KoUpdater(KoUpdaterPrivate *_d);

public:

    QPointer<KoUpdaterPrivate> d;
    int range;
    int min;
    int max;

private Q_SLOTS:

    void setInterrupted(bool value);

private:

    QAtomicInt m_interrupted;
    int  m_progressPercent;
};

class KRITAWIDGETUTILS_EXPORT KoDummyUpdaterHolder
{
public:
    KoDummyUpdaterHolder();
    ~KoDummyUpdaterHolder();

    KoUpdater *updater();

private:
    Q_DISABLE_COPY_MOVE(KoDummyUpdaterHolder)
    KoUpdaterPrivate *d;
};

#endif

