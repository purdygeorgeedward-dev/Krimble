/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_TRANSFORMMASK_H
#define LIBKIS_TRANSFORMMASK_H

#include <QObject>
#include "Node.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT TransformMask : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(TransformMask)

public:
    explicit TransformMask(KisImageSP image, QString name, QObject *parent = 0);
    explicit TransformMask(KisImageSP image, KisTransformMaskSP mask, QObject *parent = 0);
    ~TransformMask() override;
public Q_SLOTS:

    virtual QString type() const override;

    QTransform finalAffineTransform() const;

    QString toXML() const;

    bool fromXML(const QString &xml);

};

#endif

