/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONFIGWIDGETBASE_H
#define KOSHAPECONFIGWIDGETBASE_H

#include <QWidget>

#include "kritaflake_export.h"

class KoShape;
class KUndo2Command;
class KoUnit;
class KoCanvasResourceProvider;

class KRITAFLAKE_EXPORT KoShapeConfigWidgetBase : public QWidget
{
    Q_OBJECT
public:
    KoShapeConfigWidgetBase();
    ~KoShapeConfigWidgetBase() override;

    virtual void open(KoShape *shape) = 0;
    virtual void save() = 0;

    virtual void setUnit(const KoUnit &unit);

    void setResourceManager(KoCanvasResourceProvider *rm);

    virtual bool showOnShapeCreate();

    virtual bool showOnShapeSelect();

    virtual KUndo2Command * createCommand();

Q_SIGNALS:
    void propertyChanged();

    void accept();

protected:
    KoCanvasResourceProvider *m_resourceManager;
};

#endif
