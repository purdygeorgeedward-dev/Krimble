/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_FILTERLAYER_H
#define LIBKIS_FILTERLAYER_H

#include <QObject>
#include "Node.h"
#include <Filter.h>
#include <Selection.h>

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"
class KRITALIBKIS_EXPORT FilterLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(FilterLayer)

public:
    explicit FilterLayer(KisImageSP image, QString name, Filter &filter, Selection &selection, QObject *parent = 0);
    explicit FilterLayer(KisAdjustmentLayerSP layer, QObject *parent = 0);
    ~FilterLayer() override;
public Q_SLOTS:

    QString type() const override;

    void setFilter(Filter &filter);

    Filter * filter();
};

#endif

