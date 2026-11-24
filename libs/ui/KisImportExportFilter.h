/*
    This file is part of the Calligra libraries

    SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>
    SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIS_IMPORT_EXPORT_FILTER_H
#define KIS_IMPORT_EXPORT_FILTER_H

#include <QObject>
#include <QIODevice>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QPair>
#include <QList>
#include <KoID.h>
#include <kis_properties_configuration.h>
#include <kis_types.h>
#include <KisExportCheckBase.h>

class KoUpdater;
class KisDocument;
class KisConfigWidget;
class KisImportUserFeedbackInterface;

#include "kritaui_export.h"
#include "KisImportExportErrorCode.h"

class KRITAUI_EXPORT KisImportExportFilter : public QObject
{
    Q_OBJECT
public:
    static const QString ImageContainsTransparencyTag;
    static const QString ColorModelIDTag;
    static const QString ColorDepthIDTag;
    static const QString sRGBTag;
    static const QString HDRTag;
    static const QString CICPPrimariesTag;
    static const QString CICPTransferCharacteristicsTag;
public:

    ~KisImportExportFilter() override;

    void setBatchMode(bool batchmode);
    void setImportUserFeedBackInterface(KisImportUserFeedbackInterface *interface);
    void setFilename(const QString &filename);
    void setRealFilename(const QString &filename);
    void setMimeType(const QString &mime);
    void setUpdater(QPointer<KoUpdater> updater);
    QPointer<KoUpdater> updater();

    virtual KisImportExportErrorCode convert(KisDocument *document, QIODevice *io, KisPropertiesConfigurationSP configuration = 0) = 0;

    virtual KisPropertiesConfigurationSP defaultConfiguration(const QByteArray& from = "", const QByteArray& to = "") const;

    KisPropertiesConfigurationSP lastSavedConfiguration(const QByteArray &from = "", const QByteArray &to = "") const;

    virtual KisConfigWidget *createConfigurationWidget(QWidget *parent, const QByteArray& from = "", const QByteArray& to = "") const;

    virtual QMap<QString, KisExportCheckBase*> exportChecks();

    virtual bool exportSupportsGuides() const;

    virtual bool supportsIO() const { return true; }

    virtual QString verify(const QString &fileName) const;

protected:
    KisImportExportFilter(QObject *parent = 0);

    QString filename() const;
    QString realFilename() const;
    bool batchMode() const;
    KisImportUserFeedbackInterface* importUserFeedBackInterface() const;
    QByteArray mimeType() const;

    void setProgress(int value);
    virtual void initializeCapabilities();
    void addCapability(KisExportCheckBase *capability);
    void addSupportedColorModels(QList<QPair<KoID, KoID> > supportedColorModels, const QString &name, KisExportCheckBase::Level level = KisExportCheckBase::PARTIALLY);

    QString verifyZiPBasedFiles(const QString &fileName, const QStringList &filesToCheck) const;

private:

    KisImportExportFilter(const KisImportExportFilter& rhs);
    KisImportExportFilter& operator=(const KisImportExportFilter& rhs);

    class Private;
    Private *const d;

};

#endif
