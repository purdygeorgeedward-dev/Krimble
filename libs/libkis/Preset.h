
/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_PRESET_H
#define LIBKIS_PRESET_H

#include "kritalibkis_export.h"
#include "libkis.h"
#include "Resource.h"
#include <kis_types.h>
#include <kis_paintop_preset.h>

class KRITALIBKIS_EXPORT Preset : public QObject
{
public:
    Preset(Resource *resource);
    ~Preset() override;

    QString toXML() const;

    void fromXML(const QString &xml);

private:
    struct Private;
    Private *const d;

    KisPaintOpPresetSP paintOpPreset();

};

#endif
