/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_GROUPLAYER_H
#define LIBKIS_GROUPLAYER_H

#include <QObject>
#include "Node.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT GroupLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(GroupLayer)

public:
    explicit GroupLayer(KisImageSP image, QString name, QObject *parent = 0);
    explicit GroupLayer(KisGroupLayerSP layer, QObject *parent = 0);
    ~GroupLayer() override;
public Q_SLOTS:

    virtual QString type() const override;

    void setPassThroughMode(bool passthrough);

    bool passThroughMode() const;
};

#endif

