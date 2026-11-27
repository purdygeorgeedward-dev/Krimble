/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEFACTORYBASE_H
#define KOSHAPEFACTORYBASE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QDomDocument>

#include "kritaflake_export.h"
#include <KisQStringListFwd.h>

class KoShape;
class KoProperties;
class KoShapeConfigWidgetBase;
class KoShapeLoadingContext;
class KoDocumentResourceManager;

#define SHAPETEMPLATE_MIMETYPE "application/x-flake-shapetemplate"
#define SHAPEID_MIMETYPE "application/x-flake-shapeId"

struct KRITAFLAKE_EXPORT KoShapeTemplate {
    KoShapeTemplate() {
        properties = 0;
    }
    QString id;
    QString templateId;
    QString name;
    QString family;
    QString toolTip;
    QString iconName;
    const KoProperties *properties;
};

class KRITAFLAKE_EXPORT KoShapeFactoryBase : public QObject
{
    Q_OBJECT
public:

    KoShapeFactoryBase(const QString &id, const QString &name, const QString &deferredPluginName = QString());
    ~KoShapeFactoryBase() override;

    virtual QList<KoShapeConfigWidgetBase*> createShapeOptionPanels() {
        return QList<KoShapeConfigWidgetBase*>();
    }

    QString id() const;
    QList<KoShapeTemplate> templates() const;
    QString toolTip() const;
    QString iconName() const;
    QString name() const;
    QString family() const;
    int loadingPriority() const;

    QList<QPair<QString, QStringList> > odfElements() const;

    virtual bool supports(const QDomElement &element, KoShapeLoadingContext &context) const = 0;

    bool hidden() const;

    virtual void newDocumentResourceManager(KoDocumentResourceManager *manager) const;

    virtual KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const;

    virtual KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = 0) const;

protected:

    void addTemplate(const KoShapeTemplate &params);

    void setToolTip(const QString &tooltip);

    void setIconName(const char *iconName);

    void setFamily(const QString &family);

    void setLoadingPriority(int priority);

    void setXmlElementNames(const QString &nameSpace, const QStringList &elementNames);

    void setXmlElements(const QList<QPair<QString, QStringList> > &elementNamesList);

    void setHidden(bool hidden);

private:

    void getDeferredPlugin();

private Q_SLOTS:

    void pruneDocumentResourceManager(QObject *);

private:

    class Private;
    Private * const d;
};

#endif
