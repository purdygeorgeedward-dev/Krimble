/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "tasksetmodel.h"

#include <QAction>
#include <klocalizedstring.h>
#include <kis_icon.h>

TasksetModel::TasksetModel(QObject* parent): QAbstractTableModel(parent)
{
}

TasksetModel::~TasksetModel()
{
}

QVariant TasksetModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid()) {

        const TasksetStep &step = m_steps.at(index.row());

        switch (role) {
            case Qt::DisplayRole:
            {
                if (step.type == TasksetStep::FilterApplication) {
                    return step.filterDisplayName;
                }
                if (step.type == TasksetStep::OperationRun) {
                    return step.operationId;
                }
                return step.action ? step.action->iconText() : QVariant();
            }
            case Qt::DecorationRole:
            {
                if (step.type == TasksetStep::FilterApplication) {
                    return KisIconUtils::loadIcon("view-filter");
                }
                if (step.type == TasksetStep::OperationRun) {
                    return KisIconUtils::loadIcon("tools-wizard");
                }
                const QIcon icon = step.action ? step.action->icon() : QIcon();
                if (icon.isNull()) {
                    return KisIconUtils::loadIcon("tools-wizard");
                }
                return icon;
            }
        }
    }
    return QVariant();
}

QVariant TasksetModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
    return i18n("Action");
}


int TasksetModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_steps.count();
}

int TasksetModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

Qt::ItemFlags TasksetModel::flags(const QModelIndex& /*index*/) const
{
    Qt::ItemFlags flags = /*Qt::ItemIsSelectable |*/ Qt::ItemIsEnabled;
    return flags;
}

void TasksetModel::addAction(QAction* action)
{
    TasksetStep step;
    step.type = TasksetStep::ActionTrigger;
    step.action = action;
    m_steps.append(step);
    beginResetModel();
    endResetModel();
}

void TasksetModel::addFilterApplication(const QString &filterId, const QString &filterConfigXml, const QString &filterDisplayName)
{
    TasksetStep step;
    step.type = TasksetStep::FilterApplication;
    step.filterId = filterId;
    step.filterConfigXml = filterConfigXml;
    step.filterDisplayName = filterDisplayName;
    m_steps.append(step);
    beginResetModel();
    endResetModel();
}

void TasksetModel::addOperationRun(const QString &operationId, const QString &operationConfigXml)
{
    TasksetStep step;
    step.type = TasksetStep::OperationRun;
    step.operationId = operationId;
    step.operationConfigXml = operationConfigXml;
    m_steps.append(step);
    beginResetModel();
    endResetModel();
}

QVector<TasksetStep> TasksetModel::steps()
{
    return m_steps;
}

TasksetStep TasksetModel::stepFromIndex(const QModelIndex& index)
{
    if (index.isValid()) {
        return m_steps.at(index.row());
    }
    return TasksetStep();
}

void TasksetModel::clear()
{
    m_steps.clear();
    beginResetModel();
    endResetModel();
}
