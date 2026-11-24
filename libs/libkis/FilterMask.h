/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_FILTERMASK_H
#define LIBKIS_FILTERMASK_H

#include <QObject>
#include "Node.h"
#include "Filter.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT FilterMask : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(FilterMask)

public:
    explicit FilterMask(KisImageSP image, QString name, Filter &filter, QObject *parent = 0);
    explicit FilterMask(KisImageSP image, KisFilterMaskSP mask, QObject *parent=0);
    ~FilterMask() override;
public Q_SLOTS:

    QString type() const override;

    void setFilter(Filter &filter);
    Filter *filter();
};

#endif

