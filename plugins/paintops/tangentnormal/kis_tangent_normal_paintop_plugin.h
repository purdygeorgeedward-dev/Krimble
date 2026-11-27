/*
 *  SPDX-FileCopyrightText: 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _TANGENTNORMAL_PAINTOP_PLUGIN_H_
#define _TANGENTNORMAL_PAINTOP_PLUGIN_H_

#include <QObject>
#include <QVariant>

class TangentNormalPaintOpPlugin: public QObject
{
    Q_OBJECT
public:
    TangentNormalPaintOpPlugin(QObject *parent, const QVariantList &);
    ~TangentNormalPaintOpPlugin() override;
};

#endif
