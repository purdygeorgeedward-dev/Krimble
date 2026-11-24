/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_TRANSPARENCYMASK_H
#define LIBKIS_TRANSPARENCYMASK_H

#include <QObject>
#include "Node.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT TransparencyMask : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(TransparencyMask)

public:
    explicit TransparencyMask(KisImageSP image, QString name, QObject *parent = 0);
    explicit TransparencyMask(KisImageSP image, KisTransparencyMaskSP mask, QObject *parent = 0);
    ~TransparencyMask() override;
public Q_SLOTS:

    virtual QString type() const override;

    Selection *selection() const;

    void setSelection(Selection *selection);
};

#endif

