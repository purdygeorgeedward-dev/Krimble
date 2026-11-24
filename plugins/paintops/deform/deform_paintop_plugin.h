/*
 *  SPDX-FileCopyrightText: 2008 Lukáš Tvrdý (lukast.dev@gmail.com)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DEFORM_PAINTOP_PLUGIN_H_
#define DEFORM_PAINTOP_PLUGIN_H_

#include <QObject>
#include <QVariant>

class DeformPaintOpPlugin : public QObject
{
    Q_OBJECT
public:
    DeformPaintOpPlugin(QObject *parent, const QVariantList &);
    ~DeformPaintOpPlugin() override;
};

#endif
