/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 1999 Matthias Elter <elter@kde.org>
    SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
    SPDX-FileCopyrightText: 2005 Sven Langkamp <sven.langkamp@gmail.com>
    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
    SPDX-FileCopyrightText: 2003-2019 Boudewijn Rempt <boud@valdyas.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORESOURCESERVER_H
#define KORESOURCESERVER_H

#include <QString>
#include <QList>
#include <QFileInfo>
#include <QApplication>
#include <QThread>
#include <QDir>
#include <QTemporaryFile>

#include "KoResource.h"
#include "KoResourcePaths.h"
#include "ksharedconfig.h"

#include <KisGlobalResourcesInterface.h>
#include <KisResourceLocator.h>
#include <KisResourceModel.h>
#include <KisTagModel.h>
#include <kis_assert.h>
#include <kis_debug.h>

#include <ResourceDebug.h>

class KoResource;

template <class T>
class KoResourceServerObserver
{
public:
    virtual ~KoResourceServerObserver() {}

    virtual void unsetResourceServer() = 0;

    virtual void resourceAdded(QSharedPointer<T> resource) = 0;

    virtual void removingResource(QSharedPointer<T> resource) = 0;

    virtual void resourceChanged(QSharedPointer<T> resource) = 0;

};

template <class T>
class KoResourceServer
{
public:

    typedef KoResourceServerObserver<T> ObserverType;

    KoResourceServer(const QString& type)
        : m_resourceModel(new KisResourceModel(type))
        , m_tagModel(new KisTagModel(type))
        , m_type(type)
    {
        Q_ASSERT(!type.isEmpty());
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

    }

    virtual ~KoResourceServer()
    {
        delete m_resourceModel;
        delete m_tagModel;
        Q_FOREACH (ObserverType* observer, m_observers) {
            observer->unsetResourceServer();
        }
    }

    KisResourceModel *resourceModel() const
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        return m_resourceModel;
    }

    QSharedPointer<T> firstResource() const
    {

        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        return m_resourceModel->resourceForIndex(m_resourceModel->index(0, 0)).dynamicCast<T>();
    }

    int resourceCount() const {

        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        return m_resourceModel->rowCount();
    }

    bool addResource(QSharedPointer<T> resource, bool save = true) {

        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        if (!resource || !resource->valid()) {
            warnResource << "Tried to add an invalid resource!" << resource;
            return false;
        }

        if (m_resourceModel->addResource(resource, save ? QString() : "memory")) {
            notifyResourceAdded(resource);
            return true;
        }

        return false;
    }

    bool removeResourceFromServer(QSharedPointer<T> resource){

        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        if (m_resourceModel->setResourceInactive(m_resourceModel->indexForResource(resource))) {
            notifyRemovingResource(resource);
            return true;
        }
        return false;
    }

    QString saveLocation() {
        return KisResourceLocator::instance()->resourceLocationBase() + m_type;
    }

    KoResourceSP importResourceFile(const QString &filename, const bool allowOverwrite)
    {

        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        return m_resourceModel->importResourceFile(filename, allowOverwrite);
    }

    void removeResourceFile(const QString & filename)
    {
        QFileInfo fi(filename);

        QSharedPointer<T> resource = resourceByFilename(fi.fileName());
        if (!resource) {
            warnResource << "Resource file do not exist ";
            return;
        }
        removeResourceFromServer(resource);
    }

    void addObserver(ObserverType* observer)
    {
        if (observer && !m_observers.contains(observer)) {
            m_observers.append(observer);
        }
    }

    void removeObserver(ObserverType* observer)
    {
        int index = m_observers.indexOf(observer);
        if (index < 0) {
            return;
        }
        m_observers.removeAt( index );
    }

private:

    QSharedPointer<T> resourceByFilename(const QString& filename) const
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        if (filename.isEmpty() || filename.isNull()) {
            return nullptr;
        }
        QVector<KoResourceSP> resources = m_resourceModel->resourcesForFilename(filename);

        if (resources.size() > 0) {
            return resources.first().dynamicCast<T>();
        }

        return nullptr;
    }

    QSharedPointer<T> resourceByName(const QString& name) const
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }

        if (name.isEmpty() || name.isNull()) {
            return nullptr;
        }

        QVector<KoResourceSP> resources = m_resourceModel->resourcesForName(name);

        if (resources.size() > 0) {
            return resources.first().dynamicCast<T>();
        }

        return nullptr;

    }

    QSharedPointer<T> resourceByMD5(const QString& md5) const
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }
        if (md5.isEmpty() || md5.isNull()) {
            return nullptr;
        }
        QVector<KoResourceSP> resources = m_resourceModel->resourcesForMD5(md5);

        if (resources.size() > 0) {
            return resources.first().dynamicCast<T>();
        }

        return nullptr;
    }

public:

    QSharedPointer<T> resource(const QString &md5, const QString &fileName, const QString &name)
    {
        return KisGlobalResourcesInterface::instance()->source<T>(m_type).bestMatch(md5, fileName, name);
    }

    bool updateResource(QSharedPointer<T> resource)
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }
        bool result = m_resourceModel->updateResource(resource);
        notifyResourceChanged(resource);
        return result;
    }

    bool reloadResource(QSharedPointer<T> resource)
    {
        KIS_SAFE_ASSERT_RECOVER_NOOP(QThread::currentThread() == qApp->thread());
        if (QThread::currentThread() != qApp->thread()) {
            qDebug().noquote() << kisBacktrace();
        }
        bool result = m_resourceModel->reloadResource(resource);
        notifyResourceChanged(resource);

        return result;
    }

    QVector<KisTagSP> assignedTagsList(KoResourceSP resource) const
    {
        if (resource.isNull()) {
            return QVector<KisTagSP>();
        }
        return m_resourceModel->tagsForResource(resource->resourceId());
    }

protected:

    void notifyResourceAdded(QSharedPointer<T> resource)
    {
        Q_FOREACH (ObserverType* observer, m_observers) {
            observer->resourceAdded(resource);
        }
    }

    void notifyRemovingResource(QSharedPointer<T> resource)
    {
        Q_FOREACH (ObserverType* observer, m_observers) {
            observer->removingResource(resource);
        }
    }

    void notifyResourceChanged(QSharedPointer<T> resource)
    {
        Q_FOREACH (ObserverType* observer, m_observers) {
            observer->resourceChanged(resource);
        }
    }

private:

    QList<ObserverType*> m_observers;
    KisResourceModel *m_resourceModel {0};
    KisTagModel *m_tagModel {0};
    QString m_type;
};

#endif
