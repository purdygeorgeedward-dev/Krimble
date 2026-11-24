/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_GROUPSHAPE_H
#define LIBKIS_GROUPSHAPE_H

#include "kritalibkis_export.h"
#include "libkis.h"
#include "Shape.h"
#include <kis_types.h>
#include <KoShapeGroup.h>

class KRITALIBKIS_EXPORT GroupShape : public Shape
{
    Q_OBJECT

public:
    explicit GroupShape(QObject *parent = 0);
    GroupShape(KoShapeGroup *shape, QObject *parent = 0);
    ~GroupShape();
public Q_SLOTS:

    QString type() const override;

    QList<Shape*> children();
};

#endif
