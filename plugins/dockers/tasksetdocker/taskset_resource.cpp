/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "taskset_resource.h"

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QBuffer>

#include <kis_debug.h>
#include <KisPortingUtils.h>

#define TASKSET_VERSION 3

TasksetResource::TasksetResource(const QString& f)
    : KoResource(f)
{
}

TasksetResource::~TasksetResource()
{
}

TasksetResource::TasksetResource(const TasksetResource &rhs)
    : KoResource(rhs),
      m_actions(rhs.m_actions),
      m_steps(rhs.m_steps)
{
}

KoResourceSP TasksetResource::clone() const
{
    return KoResourceSP(new TasksetResource(*this));
}

namespace {

QString readConfigBlob(const QDomElement &parent)
{
    QDomElement configElement = parent.firstChildElement("config");
    if (configElement.isNull()) return QString();

    QDomNode configChild = configElement.firstChild();
    if (configChild.isCDATASection()) {
        return configChild.toCDATASection().data();
    }
    return configElement.text();
}

}

bool TasksetResource::loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);

    QDomDocument doc;
    if (!doc.setContent(dev)) {
        return false;
    }
    QDomElement element = doc.documentElement();
    setName(element.attribute("name"));

    const int version = element.attribute("version", "1").toInt();

    m_actions.clear();
    m_steps.clear();

    QDomNode node = element.firstChild();
    while (!node.isNull()) {
        QDomElement child = node.toElement();
        if (!child.isNull()) {
            if (version >= 2 && child.tagName() == "step") {
                StepRecord record;
                record.action = child.attribute("action");
                record.filterId = child.attribute("filterId");
                record.filterName = child.attribute("filterName");
                // operationId only exists in version 3+; absent attributes
                // read back as an empty string, so version-2 files load
                // correctly through this same path.
                record.operationId = child.attribute("operationId");
                record.configXml = readConfigBlob(child);
                m_steps.append(record);
                m_actions.append(record.action);
            } else if (child.tagName() == "action") {
                StepRecord record;
                record.action = child.text();
                m_steps.append(record);
                m_actions.append(record.action);
            }
        }
        node = node.nextSibling();
    }
    setValid(true);
    return true;
}

QString TasksetResource::defaultFileExtension() const
{
    return QString(".kts");
}

void TasksetResource::setActionList(const QStringList actions)
{
    m_actions = actions;
}

QStringList TasksetResource::actionList()
{
    return m_actions;
}

void TasksetResource::setStepList(const QVector<StepRecord> &steps)
{
    m_steps = steps;

    m_actions.clear();
    Q_FOREACH (const StepRecord &record, m_steps) {
        m_actions.append(record.action);
    }
}

QVector<TasksetResource::StepRecord> TasksetResource::stepList() const
{
    return m_steps;
}

bool TasksetResource::saveToDevice(QIODevice *io) const
{

    QDomDocument doc;
    QDomElement root = doc.createElement("Taskset");
    root.setAttribute("name", name() );
    root.setAttribute("version", TASKSET_VERSION);

    Q_FOREACH (const StepRecord &record, m_steps) {
        QDomElement element = doc.createElement("step");
        element.setAttribute("action", record.action);
        element.setAttribute("filterId", record.filterId);
        element.setAttribute("filterName", record.filterName);
        element.setAttribute("operationId", record.operationId);

        if (!record.configXml.isEmpty()) {
            QDomElement configElement = doc.createElement("config");
            configElement.appendChild(doc.createCDATASection(record.configXml));
            element.appendChild(configElement);
        }

        root.appendChild(element);
    }
    doc.appendChild(root);

    QTextStream textStream(io);
    KisPortingUtils::setUtf8OnStream(textStream);
    doc.save(textStream, 4);

    KoResource::saveToDevice(io);

    return true;
}
