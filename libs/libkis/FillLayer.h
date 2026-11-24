/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_FILLLAYER_H
#define LIBKIS_FILLLAYER_H

#include <QObject>
#include "Node.h"
#include <InfoObject.h>
#include <Selection.h>

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"
class KRITALIBKIS_EXPORT FillLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(FillLayer)

public:
    explicit FillLayer(KisImageSP image, QString name, KisFilterConfigurationSP filterConfig, Selection &selection, QObject *parent = 0);
    explicit FillLayer(KisGeneratorLayerSP layer, QObject *parent = 0);
    ~FillLayer() override;
public Q_SLOTS:

    virtual QString type() const override;

    bool setGenerator(const QString &generatorName, InfoObject *filterConfig);
    QString generatorName();

    InfoObject *filterConfig();
};

#endif
