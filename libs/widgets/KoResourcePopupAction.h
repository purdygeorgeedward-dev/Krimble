/*
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEPOPUPACTION_H
#define KORESOURCEPOPUPACTION_H

#include <QAction>

#include <QSharedPointer>

#include <KoResource.h>

class KoShapeBackground;
class QModelIndex;

class KoCanvasResourcesInterface;
using KoCanvasResourcesInterfaceSP = QSharedPointer<KoCanvasResourcesInterface>;

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KoResourcePopupAction : public QAction
{
    Q_OBJECT

public:
    explicit KoResourcePopupAction(const QString &resourceType, KoCanvasResourcesInterfaceSP canvasResourcesInterface, QObject *parent = 0);

    ~KoResourcePopupAction() override;

    QSharedPointer<KoShapeBackground> currentBackground() const;
    void setCurrentBackground(QSharedPointer<KoShapeBackground> background);

    void setCurrentResource(KoResourceSP resource);
    KoResourceSP currentResource() const;

    void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);

Q_SIGNALS:
    void resourceSelected(QSharedPointer<KoShapeBackground>  background);

public Q_SLOTS:
    void updateIcon();

private Q_SLOTS:
    void indexChanged(const QModelIndex &modelIndex);

private:
    class Private;
    Private * const d;
};

#endif
