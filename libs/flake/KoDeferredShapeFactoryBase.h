/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt (boud@valdyas.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODEFERREDSHAPEFACTORYBASE_H
#define KODEFERREDSHAPEFACTORYBASE_H

#include "kritaflake_export.h"

#include <QObject>

class KoShape;
class KoDocumentResourceManager;
class KoProperties;

class QString;

class KRITAFLAKE_EXPORT KoDeferredShapeFactoryBase : public QObject
{
    Q_OBJECT
public:

    explicit KoDeferredShapeFactoryBase(QObject *parent);

    ~KoDeferredShapeFactoryBase() override;

    virtual QString deferredPluginName() = 0;

    virtual KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const = 0;

    virtual KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = 0) const;

};

#endif
