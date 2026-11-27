/*
 *  SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GBR_BRUSH_
#define KIS_GBR_BRUSH_

#include <QImage>

#include "KisColorfulBrush.h"
#include <kis_types.h>
#include <kis_shared.h>
#include <brushengine/kis_paint_information.h>

#include "kritabrush_export.h"

class KisQImagemask;
typedef KisSharedPtr<KisQImagemask> KisQImagemaskSP;

class QString;
class QIODevice;

class BRUSH_EXPORT KisGbrBrush : public KisColorfulBrush
{

protected:

public:

    KisGbrBrush(const QString& filename);

    KisGbrBrush(const QString& filename,
                const QByteArray & data,
                qint32 & dataPos);

    KisGbrBrush(KisPaintDeviceSP image, int x, int y, int w, int h);

    KisGbrBrush(const QImage& image, const QString& name = QString());

    ~KisGbrBrush() override;

    KisGbrBrush(const KisGbrBrush& rhs);

    KoResourceSP clone() const override;

    KisGbrBrush &operator=(const KisGbrBrush &rhs);

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    QPair<QString, QString> resourceType() const override {
        return QPair<QString, QString>(ResourceType::Brushes, ResourceSubType::GbrBrushes);
    }

    virtual void makeMaskImage(bool preserveAlpha);

    QString defaultFileExtension() const override;

protected:
    friend class KisImageBrushesPipe;
    friend class KisBrushExport;

    void setBrushTipImage(const QImage& image) override;

    void toXML(QDomDocument& d, QDomElement& e) const override;

private:

    bool init();
    bool initFromPaintDev(KisPaintDeviceSP image, int x, int y, int w, int h);

    struct Private;
    Private* const d;
};

typedef QSharedPointer<KisGbrBrush> KisGbrBrushSP;

#endif

