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

struct TasksetStep
{
    enum Type { ActionTrigger, FilterApplication, OperationRun };

    Type type = ActionTrigger;
    QAction *action = nullptr;
    QString filterId;
    QString filterConfigXml;
    QString filterDisplayName;
    QString operationId;
    QString operationConfigXml;
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

#endif
