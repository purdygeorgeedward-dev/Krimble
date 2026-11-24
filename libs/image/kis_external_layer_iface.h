/*
 *  SPDX-FileCopyrightText: 2006 Bart Coppens <kde@bartcoppens.be>
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_EXTERNAL_LAYER_IFACE_
#define KIS_EXTERNAL_LAYER_IFACE_

#include "kis_icon_utils.h"

#include "kis_types.h"

#include "kis_image.h"
#include "kis_layer.h"

class QString;
class QIcon;
class KUndo2Command;

class KRITAIMAGE_EXPORT KisExternalLayer : public KisLayer
{

public:
    KisExternalLayer(KisImageWSP image, const QString &name, quint8 opacity)
            : KisLayer(image, name, opacity) {}

    QIcon icon() const override {
        return KisIconUtils::loadIcon("view-refresh");
    }

    virtual void resetCache(const KoColorSpace *colorSpace = 0);

    virtual KUndo2Command* crop(const QRect & rect) {
        Q_UNUSED(rect);
        return 0;
    }

    virtual KUndo2Command* transform(const QTransform &transform) {
        Q_UNUSED(transform);
        return 0;
    }

    virtual bool supportsPerspectiveTransform() const {
        return false;
    }

    virtual KUndo2Command* setProfile(const KoColorProfile *profile) {
        Q_UNUSED(profile);
        return 0;
    }

    virtual KUndo2Command* convertTo(const KoColorSpace * dstColorSpace,
                                         KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::internalRenderingIntent(),
                                         KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::internalConversionFlags())
    {
        Q_UNUSED(dstColorSpace);
        Q_UNUSED(renderingIntent);
        Q_UNUSED(conversionFlags);
        return 0;
    }

    virtual QRect theoreticalBoundingRect() const;
};

#endif
