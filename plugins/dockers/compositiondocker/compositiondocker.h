/*
 *  SPDX-FileCopyrightText: 2012 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COMPOSITIONDOCKER_H
#define COMPOSITIONDOCKER_H

#include <QObject>
#include <QVariant>

class CompositionDockerPlugin : public QObject
{
    Q_OBJECT
public:
    CompositionDockerPlugin(QObject *parent, const QVariantList &);
    ~CompositionDockerPlugin() override;
};

#endif
