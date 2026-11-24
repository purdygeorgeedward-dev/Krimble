/*
 *  SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PRESETCHOOSER_H
#define PRESETCHOOSER_H

#include <QObject>
#include <QWidget>

#include <kis_preset_chooser.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class Resource;

class KRITALIBKIS_EXPORT PresetChooser : public KisPresetChooser
{
    Q_OBJECT
public:
    PresetChooser(QWidget *parent = 0);
    ~PresetChooser() override {}

public Q_SLOTS:

    void setCurrentPreset(Resource *resource);

    Resource *currentPreset() const;

Q_SIGNALS:

    void presetSelected(Resource resource);

    void presetClicked(Resource resource);

private Q_SLOTS:

    void slotResourceSelected(KoResourceSP resource);
    void slotResourceClicked(KoResourceSP resource);
};

#endif
