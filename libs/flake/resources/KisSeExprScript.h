/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SEEXPR_SCRIPT_H
#define KIS_SEEXPR_SCRIPT_H

#include <KoResource.h>
#include <QMetaType>
#include <kritaflake_export.h>

class KisSeExprScript;
typedef QSharedPointer<KisSeExprScript> KisSeExprScriptSP;

class KRITAFLAKE_EXPORT KisSeExprScript : public KoResource
{
public:
    KisSeExprScript(const QString &filename);

    KisSeExprScript(const QImage &image, const QString &script, const QString &name, const QString &folderName);

    KisSeExprScript(KisSeExprScript *rhs);
    KisSeExprScript(const KisSeExprScript &rhs);

    ~KisSeExprScript();

public:
    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;

    bool saveToDevice(QIODevice *dev) const override;

    QPair<QString, QString> resourceType() const override;

    QString defaultFileExtension() const override;

    QString script() const;

    QString name() const override;

    void setScript(const QString &script);

    KoResourceSP clone() const override;

private:
    struct Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KisSeExprScript *)

#endif
