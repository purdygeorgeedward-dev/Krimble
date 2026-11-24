/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_FILTER_H
#define LIBKIS_FILTER_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"
#include <kis_filter_configuration.h>

class KRITALIBKIS_EXPORT Filter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Filter)

public:
    explicit Filter();
    ~Filter() override;

    bool operator==(const Filter &other) const;
    bool operator!=(const Filter &other) const;

public Q_SLOTS:

    QString name() const;

    void setName(const QString &name);

    InfoObject* configuration() const;

    void setConfiguration(InfoObject* value);

    bool apply(Node *node, int x, int y, int w, int h);

    bool startFilter(Node *node, int x, int y, int w, int h);

private:
    friend class FilterLayer;
    friend class FilterMask;

    struct Private;
    Private *const d;

    KisFilterConfigurationSP filterConfig();

};

#endif
