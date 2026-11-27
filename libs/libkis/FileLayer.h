/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_FILELAYER_H
#define LIBKIS_FILELAYER_H

#include <QObject>
#include "Node.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT FileLayer : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(FileLayer)

public:
    explicit FileLayer(KisImageSP image,
                        const QString name = QString(),
                        const QString baseName=QString(),
                        const QString fileName=QString(),
                        const QString scalingMethod=QString(),
                        const QString scalingFilter=QString(),
                        QObject *parent = 0);
    explicit FileLayer(KisFileLayerSP layer, QObject *parent = 0);
    ~FileLayer() override;
public Q_SLOTS:

    QString type() const override;

    void setProperties(QString fileName, QString scalingMethod = QString("None"), QString scalingFilter = QString("Bicubic"));

    void resetCache();

    QString path() const;

    QString scalingMethod() const;

    QString scalingFilter() const;

private:
    QString getFileNameFromAbsolute(const QString &basePath, QString filePath);
    QString m_baseName;
};

#endif

