/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_RESOURCE_H
#define LIBKIS_RESOURCE_H

#include <QObject>
#include <QScopedPointer>
#include <kis_types.h>
#include "kritalibkis_export.h"
#include "libkis.h"
#include <KoResource.h>

class KRITALIBKIS_EXPORT Resource : public QObject
{
    Q_OBJECT

public:
    Resource(int resourceId, const QString &type, const QString &name, const QString &filename, const QImage &image, QObject *parent = 0);
    Resource(KoResourceSP resource, const QString &type, QObject *parent = 0);
    ~Resource() override;
    Resource(const Resource &rhs);

    bool operator==(const Resource &other) const;
    bool operator!=(const Resource &other) const;
    Resource operator=(const Resource &rhs);

public Q_SLOTS:

    QString type() const;

    QString name() const;

    void setName(QString value);

    QString filename() const;

    QImage image() const;

    void setImage(QImage image);

private:

    friend class PresetChooser;
    friend class View;
    friend class Palette;
    friend class Preset;
    KoResourceSP resource() const;

    struct Private;
    QScopedPointer<Private> d;

};

#endif
