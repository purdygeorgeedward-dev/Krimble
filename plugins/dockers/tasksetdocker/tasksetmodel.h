/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TASKSETMODEL_H
#define TASKSETMODEL_H

#include <QModelIndex>
#include <QVector>
#include <QString>

#include <kis_types.h>

class QAction;

/**
 * Krimble: a step is either a plain action trigger (the original,
 * Krita-native behaviour -- fine for parameterless commands like Flatten
 * Image or a tool switch), or a real filter application captured with its
 * exact parameters via KisFilterManager::sigFilterApplied -- the
 * industry standard actions equivalent case, correctly replayable without
 * reopening an empty dialog the way a plain trigger of a filter's menu
 * action would.
 */
struct TasksetStep
{
    enum Type { ActionTrigger, FilterApplication, OperationRun };

    Type type = ActionTrigger;
    QAction *action = nullptr;        // valid when type == ActionTrigger
    QString filterId;                 // valid when type == FilterApplication
    QString filterConfigXml;          // valid when type == FilterApplication, full KisFilterConfiguration::toXML()
    QString filterDisplayName;        // valid when type == FilterApplication, for display without needing the registry
    QString operationId;              // valid when type == OperationRun
    QString operationConfigXml;       // valid when type == OperationRun, full KisOperationConfiguration::toXML()
};

class TasksetModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TasksetModel(QObject* parent = 0);
    ~TasksetModel() override;
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addAction(QAction* action);
    void addFilterApplication(const QString &filterId, const QString &filterConfigXml, const QString &filterDisplayName);
    void addOperationRun(const QString &operationId, const QString &operationConfigXml);
    TasksetStep stepFromIndex(const QModelIndex& index);
    QVector<TasksetStep> steps();

public Q_SLOTS:
    void clear();
private:
    QVector<TasksetStep> m_steps;
};

#endif // TASKSETMODEL_H
