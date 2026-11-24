/*
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_IMPORT_EXPORT_MANAGER_H
#define KIS_IMPORT_EXPORT_MANAGER_H

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QUrl>

#include "KisImportExportFilter.h"

#include "kritaui_export.h"

class KisDocument;
class KoProgressUpdater;

template <class T>
class QFuture;

class KRITAUI_EXPORT KisImportExportManager : public QObject
{
    Q_OBJECT
public:
    enum Direction { Import = 1,  Export = 2 };

    explicit KisImportExportManager(KisDocument *document);

public:

    ~KisImportExportManager() override;

    KisImportExportErrorCode importDocument(const QString &location, const QString &mimeType);

    KisImportExportErrorCode exportDocument(const QString &location, const QString& realLocation, const QByteArray &mimeType, bool showWarnings = true, KisPropertiesConfigurationSP exportConfiguration = 0, bool isAdvancedExporting = false );

    QFuture<KisImportExportErrorCode> exportDocumentAsync(const QString &location, const QString& realLocation, const QByteArray &mimeType, KisImportExportErrorCode &status, bool showWarnings = true, KisPropertiesConfigurationSP exportConfiguration = 0,bool isAdvancedExporting= false);

    static QStringList supportedMimeTypes(Direction direction);

    static KisImportExportFilter *filterForMimeType(const QString &mimetype, Direction direction);

    static void fillStaticExportConfigurationProperties(KisPropertiesConfigurationSP exportConfiguration, KisImageSP image);

    bool batchMode(void) const;

    void setUpdater(KoUpdaterPtr updater);

    static QString askForAudioFileName(const QString &defaultDir, QWidget *parent);

    static QString getUriForAdditionalFile(const QString &defaultUri, QWidget *parent);

private:

    struct ConversionResult;
    ConversionResult convert(Direction direction, const QString &location, const QString& realLocation, const QString &mimeType, bool showWarnings, KisPropertiesConfigurationSP exportConfiguration, bool isAsync, bool isAdvancedExporting= false);

    void fillStaticExportConfigurationProperties(KisPropertiesConfigurationSP exportConfiguration);
    bool askUserAboutExportConfiguration(QSharedPointer<KisImportExportFilter> filter, KisPropertiesConfigurationSP exportConfiguration, const QByteArray &from, const QByteArray &to, bool batchMode, const bool showWarnings, bool *alsoAsKra, bool isAdvancedExporting = false);

    KisImportExportErrorCode doImport(const QString &location, QSharedPointer<KisImportExportFilter> filter);

    KisImportExportErrorCode doExport(const QString &location, QSharedPointer<KisImportExportFilter> filter, KisPropertiesConfigurationSP exportConfiguration, const QString alsoAsKraLocation);
    KisImportExportErrorCode doExportImpl(const QString &location, QSharedPointer<KisImportExportFilter> filter, KisPropertiesConfigurationSP exportConfiguration);

    QString getAlsoAsKraLocation(const QString location) const;

    KisImportExportManager(const KisImportExportManager& rhs);
    KisImportExportManager &operator=(const KisImportExportManager& rhs);

    KisDocument *m_document;

    static QStringList m_importMimeTypes;
    static QStringList m_exportMimeTypes;

    class Private;
    Private * const d;
};

#endif
