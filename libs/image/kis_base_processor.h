/*
 *  SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_BASE_PROCESSOR_H_
#define _KIS_BASE_PROCESSOR_H_

#include <list>

#include <QString>

#include <klocalizedstring.h>
#include <QKeySequence>

#include "KoID.h"
#include "KoColorSpace.h"

#include "kis_types.h"
#include "kis_shared.h"
#include "kis_image.h"
#include "kis_filter_configuration.h"
#include "kritaimage_export.h"

class QWidget;

class KisBookmarkedConfigurationManager;
class KisConfigWidget;

class KoResource;
typedef QSharedPointer<KoResource> KoResourceSP;

class KisResourcesInterface;
typedef QSharedPointer<KisResourcesInterface> KisResourcesInterfaceSP;

class KRITAIMAGE_EXPORT KisBaseProcessor : public KisShared
{
    friend class KisBaseProcessorConfigurationFactory;

public:

    KisBaseProcessor(const KoID& id, const KoID & category, const QString & entry);

    virtual ~KisBaseProcessor();

    virtual KisFilterConfigurationSP  defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const;

    KisBookmarkedConfigurationManager* bookmarkManager();

    const KisBookmarkedConfigurationManager* bookmarkManager() const;

    QString id() const;

    QString name() const;

    KoID menuCategory() const;

    QString menuEntry() const;

    QKeySequence shortcut() const;

    virtual KisConfigWidget * createConfigurationWidget(QWidget * parent, const KisPaintDeviceSP dev, bool useForMasks) const;
public:
    bool supportsPainting() const;

    bool supportsAdjustmentLayers() const;

    bool supportsThreading() const;

    bool showConfigurationWidget();

    ColorSpaceIndependence colorSpaceIndependence() const;

    virtual KisFilterConfigurationSP factoryConfiguration(KisResourcesInterfaceSP resourcesInterface) const;

protected:

    void setSupportsPainting(bool v);
    void setSupportsAdjustmentLayers(bool v);
    void setSupportsThreading(bool v);
    void setColorSpaceIndependence(ColorSpaceIndependence v);
    void setShowConfigurationWidget(bool v);

    void setShortcut(const QKeySequence & shortcut);

protected:

    void init(const QString& configEntryGroup);

private:
    struct Private;
    Private* const d;
};

#endif
