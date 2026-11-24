/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISLOCALSTROKERESOURCES_H
#define KISLOCALSTROKERESOURCES_H

#include <KisResourcesInterface.h>

class KisLocalStrokeResourcesPrivate;

class KRITARESOURCES_EXPORT KisLocalStrokeResources : public KisResourcesInterface
{
public:
    KisLocalStrokeResources();
    KisLocalStrokeResources(const QList<KoResourceSP> &localResources);

    void addResource(KoResourceSP resource);

    void removeResource(KoResourceSP resource);

    KisLocalStrokeResources* clone() const;

    QList<KoResourceSP> resources() const;

protected:
    ResourceSourceAdapter* createSourceImpl(const QString &type) const override;

private:
    Q_DECLARE_PRIVATE(KisLocalStrokeResources);
};

#endif
