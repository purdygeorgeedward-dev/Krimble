/*
 * SPDX-FileCopyrightText: 2009 Lukáš Tvrdý (lukast.dev@gmail.com)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GRID_PAINTOP_PLUGIN_H_
#define GRID_PAINTOP_PLUGIN_H_

#include <QObject>
#include <QVariant>

class GridPaintOpPlugin : public QObject
{
    Q_OBJECT
public:
    GridPaintOpPlugin(QObject *parent, const QVariantList &);
    ~GridPaintOpPlugin() override;
};

#endif
