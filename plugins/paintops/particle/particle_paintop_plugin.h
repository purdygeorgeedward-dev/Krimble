/*
 *  SPDX-FileCopyrightText: 2010 Lukáš Tvrdý (lukast.dev@gmail.com)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef PARTICLE_PAINTOP_PLUGIN_H_
#define PARTICLE_PAINTOP_PLUGIN_H_

#include <QObject>
#include <QVariantList>

class ParticlePaintOpPlugin : public QObject
{
    Q_OBJECT
public:
    ParticlePaintOpPlugin(QObject *parent, const QVariantList &);
    ~ParticlePaintOpPlugin() override;
};

#endif
