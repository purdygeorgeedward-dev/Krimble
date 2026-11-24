/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_CLONELAYER_H
#define LIBKIS_CLONELAYER_H

#include <QObject>
#include "Node.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT CloneLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(CloneLayer)

public:
    explicit CloneLayer(KisImageSP image, QString name, KisLayerSP source, QObject *parent = 0);

    explicit CloneLayer(KisCloneLayerSP layer, QObject *parent = 0);
    ~CloneLayer() override;
public Q_SLOTS:

    virtual QString type() const override;

    Node* sourceNode() const;

    void setSourceNode(Node* node);
};

#endif

