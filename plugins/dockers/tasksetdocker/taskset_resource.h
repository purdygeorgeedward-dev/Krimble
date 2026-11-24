/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TASKSET_RESOURCE_H
#define TASKSET_RESOURCE_H

#include <KoResource.h>
#include <QStringList>
#include <QVector>

class TasksetResource : public KoResource
{

public:
    TasksetResource(const QString& filename);
    ~TasksetResource() override;

    TasksetResource(const TasksetResource &rhs);
    TasksetResource &operator=(const TasksetResource &rhs) = delete;
    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    QString defaultFileExtension() const override;

    QPair<QString, QString> resourceType() const override
    {
        return QPair<QString, QString>(ResourceType::TaskSets, "");
    }

    void setActionList(const QStringList actions);
    QStringList actionList();

    struct StepRecord {
        QString action;
        QString filterId;
        QString filterName;
        QString operationId;
        QString configXml;
    };

    void setStepList(const QVector<StepRecord> &steps);
    QVector<StepRecord> stepList() const;

private:

    QStringList m_actions;
    QVector<StepRecord> m_steps;
};

typedef QSharedPointer<TasksetResource> TasksetResourceSP;

#endif
