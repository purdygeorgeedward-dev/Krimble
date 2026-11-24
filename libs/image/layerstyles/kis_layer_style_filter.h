/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_LAYER_STYLE_FILTER_H
#define __KIS_LAYER_STYLE_FILTER_H

#include "kis_types.h"
#include "kis_shared.h"
#include "kritaimage_export.h"
#include "kis_psd_layer_style.h"
#include <QScopedPointer>

class KisLayerStyleFilterEnvironment;
class KisMultipleProjection;
class KisLayerStyleKnockoutBlower;

class KRITAIMAGE_EXPORT KisLayerStyleFilter : public KisShared
{
public:
    KisLayerStyleFilter(const KoID &id);
    virtual ~KisLayerStyleFilter();

    QString id() const;

    virtual KisLayerStyleFilter* clone() const = 0;

    virtual void processDirectly(KisPaintDeviceSP src,
                                 KisMultipleProjection *dst,
                                 KisLayerStyleKnockoutBlower *blower,
                                 const QRect &applyRect,
                                 KisPSDLayerStyleSP style,
                                 KisLayerStyleFilterEnvironment *env) const = 0;

    virtual QRect neededRect(const QRect & rect, KisPSDLayerStyleSP style, KisLayerStyleFilterEnvironment *env) const = 0;

    virtual QRect changedRect(const QRect & rect, KisPSDLayerStyleSP style, KisLayerStyleFilterEnvironment *env) const = 0;

protected:
    KisLayerStyleFilter(const KisLayerStyleFilter &rhs);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
