/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PAINTOP_FACTORY_H_
#define KIS_PAINTOP_FACTORY_H_

#include "kis_types.h"
#include "kritaimage_export.h"
#include <QObject>
#include <QString>
#include <QIcon>
#include <QStringList>
#include <kis_threaded_text_rendering_workaround.h>
#include <brushengine/kis_paintop_settings.h>

class KisPainter;
class KisPaintOp;
class QWidget;
class KisPaintOpConfigWidget;
class KisInterstrokeDataFactory;

class KoResource;
using KoResourceSP = QSharedPointer<KoResource>;

class KisResourcesInterface;
using KisResourcesInterfaceSP = QSharedPointer<KisResourcesInterface>;

class KoResourceLoadResult;

class KRITAIMAGE_EXPORT KisPaintOpFactory : public QObject
{
    Q_OBJECT

public:

    enum PaintopVisibility {
        AUTO,
        ALWAYS,
        NEVER
    };

    KisPaintOpFactory(const QStringList & whiteListedCompositeOps = QStringList());
    ~KisPaintOpFactory() override {}

    static QString categoryStable();

#ifdef HAVE_THREADED_TEXT_RENDERING_WORKAROUND
    virtual void preinitializePaintOpIfNeeded(const KisPaintOpSettingsSP settings);
#endif

    virtual KisPaintOp * createOp(const KisPaintOpSettingsSP settings, KisPainter * painter, KisNodeSP node, KisImageSP image) = 0;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString category() const = 0;
    virtual bool lodSizeThresholdSupported() const = 0;

    virtual QList<KoResourceLoadResult> prepareLinkedResources(const KisPaintOpSettingsSP settings, KisResourcesInterfaceSP resourcesInterface) = 0;

    virtual QList<KoResourceLoadResult> prepareEmbeddedResources(const KisPaintOpSettingsSP settings, KisResourcesInterfaceSP resourcesInterface) = 0;

    virtual KisInterstrokeDataFactory* createInterstrokeDataFactory(const KisPaintOpSettingsSP settings, KisResourcesInterfaceSP resourcesInterface) const;

    QStringList whiteListedCompositeOps() const;

    virtual QIcon icon();

    virtual KisPaintOpSettingsSP createSettings(KisResourcesInterfaceSP resourcesInterface) = 0;

    virtual KisPaintOpConfigWidget* createConfigWidget(QWidget* parent, KisResourcesInterfaceSP resourcesInterface, KoCanvasResourcesInterfaceSP canvasResourcesInterface) = 0;

    void setPriority(int newPriority);

    int priority() const;

private:
    QStringList m_whiteListedCompositeOps;
    int m_priority;
    PaintopVisibility m_visibility;
};

#endif
