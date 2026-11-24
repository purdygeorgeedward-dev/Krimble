/* This file is part of the Krita project
 *
 * SPDX-FileCopyrightText: 2014 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISDOCUMENT_H
#define KISDOCUMENT_H

#include <QDateTime>
#include <QList>
#include <QFileInfo>

#include <klocalizedstring.h>

#include <kundo2stack.h>
#include <KoColorSet.h>

#include <kis_image.h>
#include <KisImportExportFilter.h>
#include <kis_properties_configuration.h>
#include <kis_types.h>
#include <kis_painting_assistant.h>
#include <KisReferenceImage.h>
#include <kis_debug.h>
#include <KisImportExportUtils.h>
#include <kis_config.h>
#include "StoryboardItem.h"

#include "kritaui_export.h"

#include <memory>

class QString;

class KUndo2Command;
class KoUnit;

class KoColor;
class KoColorSpace;
class KoShapeControllerBase;
class KoShapeLayer;
class KoStore;
class KoDocumentInfo;
class KoDocumentInfoDlg;
class KisImportExportManager;
class KisUndoStore;
class KisPart;
class KisGridConfig;
class KisGuidesConfig;
class KisMirrorAxisConfig;
class QDomDocument;
class KisReferenceImagesLayer;
class KisUniqueColorSet;

#define KIS_MIME_TYPE "application/x-krita"

class KRITAUI_EXPORT KisDocument : public QObject
{
    Q_OBJECT
protected:

    explicit KisDocument(bool addStorage = true);

    explicit KisDocument(const KisDocument &rhs, bool addStorage);

public:
    enum OpenFlag {
        None = 0,
        DontAddToRecent = 0x1,
        RecoveryFile = 0x2
    };
    Q_DECLARE_FLAGS(OpenFlags, OpenFlag)

    ~KisDocument();

    QString embeddedResourcesStorageId() const;

    QString linkedResourcesStorageId() const;

    KisDocument *clone(bool addStorage = false);

    bool openPath(const QString &path, OpenFlags flags = None);

    bool importDocument(const QString &path);

    bool exportDocument(const QString &path, const QByteArray &mimeType,bool isAdvancedExporting = false, bool showWarnings = false, KisPropertiesConfigurationSP exportConfiguration = 0);
    bool exportDocumentSync(const QString &path, const QByteArray &mimeType, KisPropertiesConfigurationSP exportConfiguration = 0);

private:
    bool exportDocumentImpl(const KritaUtils::ExportFileJob &job, KisPropertiesConfigurationSP exportConfiguration, bool isAdvancedExporting= false);

public:
    void setReadWrite(bool readwrite = true);

    static QByteArray nativeFormatMimeType() { return KIS_MIME_TYPE; }

    bool isNativeFormat(const QByteArray& mimeType) const;

    static QStringList extraNativeMimeTypes() { return QStringList() << KIS_MIME_TYPE; }

    QByteArray mimeType() const;

    void setMimeType(const QByteArray & mimeType);

    bool fileBatchMode() const;

    void setFileBatchMode(const bool batchMode);

    void setErrorMessage(const QString& errMsg);

    QString errorMessage() const;

    void setWarningMessage(const QString& warningMsg);

    QString warningMessage() const;

    QPixmap generatePreview(const QSize& size);

    void setEmpty(bool empty = true);

    QDomDocument createDomDocument(const QString& tagName, const QString& version) const;

    static QDomDocument createDomDocument(const QString& appName, const QString& tagName, const QString& version);

    bool loadNativeFormat(const QString & file);

    void setAutoSaveActive(bool autoSaveIsActive);

    bool isAutoSaveActive();

    void setNormalAutoSaveInterval();

    void setEmergencyAutoSaveInterval();

    void setInfiniteAutoSaveInterval();

    KoDocumentInfo *documentInfo() const;

    void removeAutoSaveFiles(const QString &autosaveBaseName, bool wasRecovered);

    bool isModified() const;

    QString caption() const;

    void resetPath();

    void setMimeTypeAfterLoading(const QString& mimeType);

    KoUnit unit() const;

    void setUnit(const KoUnit &unit);

    KisGridConfig gridConfig() const;
    void setGridConfig(const KisGridConfig &config);

    const KisGuidesConfig& guidesConfig() const;
    void setGuidesConfig(const KisGuidesConfig &data);

    QList<KoResourceLoadResult> linkedDocumentResources();

    void setPaletteList(const QList<KoColorSetSP> &paletteList, bool emitSignal = false);

    StoryboardItemList getStoryboardItemList();

    void setStoryboardItemList(const StoryboardItemList &storyboardItemList, bool emitSignal = false);

    QVector<StoryboardComment> getStoryboardCommentsList();

    void setStoryboardCommentList(const QVector<StoryboardComment> &storyboardCommentList, bool emitSignal = false);

    QVector<QFileInfo> getAudioTracks() const;
    void setAudioTracks(QVector<QFileInfo> f);

    void setAudioVolume(qreal level);
    qreal getAudioLevel();

    const KisMirrorAxisConfig& mirrorAxisConfig() const;
    void setMirrorAxisConfig(const KisMirrorAxisConfig& config);

    void clearUndoHistory();

    void setModified(bool _mod);

    void setRecovered(bool value);
    bool isRecovered() const;

    void updateEditingTime(bool forceStoreElapsed);

    KUndo2Stack *undoStack();

    KisImportExportManager *importExportManager() const;

    QByteArray serializeToNativeByteArray();

    bool isInSaving() const;

Q_SIGNALS:

    void unitChanged(const KoUnit &unit);

    void statusBarMessage(const QString& text, int timeout = 0);

    void clearStatusBarMessage();

    void modified(bool);

    void sigReadWriteChanged(bool value);
    void sigRecoveredChanged(bool value);
    void sigPathChanged(const QString &path);

    void sigLoadingFinished();

    void sigSavingFinished(const QString &filePath);

    void sigGuidesConfigChanged(const KisGuidesConfig &config);

    void sigBackgroundSavingFinished(KisImportExportErrorCode status, const QString &errorMessage, const QString &warningMessage);

    void sigCompleteBackgroundSaving(const KritaUtils::ExportFileJob &job, KisImportExportErrorCode status, const QString &errorMessage, const QString &warningMessage);

    void sigReferenceImagesChanged();

    void sigMirrorAxisConfigChanged();

    void sigGridConfigChanged(const KisGridConfig &config);

    void sigReferenceImagesLayerChanged(KisSharedPtr<KisReferenceImagesLayer> layer);

    void sigPaletteListChanged(const QList<KoColorSetSP> &oldPaletteList, const QList<KoColorSetSP> &newPaletteList);

    void sigAssistantsChanged();

    void sigStoryboardItemListChanged();

    void sigStoryboardCommentListChanged();

    void sigAudioTracksChanged();

    void sigAudioLevelChanged(qreal level);

private Q_SLOTS:
    void finishExportInBackground();
    void slotChildCompletedSavingInBackground(KisImportExportErrorCode status, const QString &errorMessage, const QString &warningMessage);
    void slotCompleteAutoSaving(const KritaUtils::ExportFileJob &job, KisImportExportErrorCode status, const QString &errorMessage, const QString &warningMessage);

    void slotCompleteSavingDocument(const KritaUtils::ExportFileJob &job, KisImportExportErrorCode status, const QString &errorMessage, const QString &warningMessage);

    void slotInitiateAsyncAutosaving(KisDocument *clonedDocument);
    void slotDocumentCloningCancelled();

    void slotPerformIdleRoutines();

private:

    friend class KisPart;
    friend class SafeSavingLocker;

    KritaUtils::BackgroudSavingStartResult initiateSavingInBackground(const QString actionName,
                                    const QObject *receiverObject, const char *receiverMethod,
                                    const KritaUtils::ExportFileJob &job,
                                    KisPropertiesConfigurationSP exportConfiguration,
                                    std::unique_ptr<KisDocument> &&optionalClonedDocument, bool isAdvancedExporting = false);

    KritaUtils::BackgroudSavingStartResult initiateSavingInBackground(const QString actionName,
                                    const QObject *receiverObject, const char *receiverMethod,
                                    const KritaUtils::ExportFileJob &job,
                                    KisPropertiesConfigurationSP exportConfiguration, bool isAdvancedExporting =false );

    KisImportExportErrorCode startExportInBackground(const QString &actionName, const QString &location,
                                 const QString &realLocation,
                                 const QByteArray &mimeType,
                                 bool showWarnings,
                                 KisPropertiesConfigurationSP exportConfiguration, bool isAdvancedExporting= false);

    void setAutoSaveDelay(int delay);

    QString newObjectName();

    QString generateAutoSaveFileName(const QString & path) const;

    bool openFile();

public:

    bool isAutosaving() const;

public:

    QString localFilePath() const;
    void setLocalFilePath( const QString &localFilePath );

    KoDocumentInfoDlg* createDocumentInfoDialog(QWidget *parent, KoDocumentInfo *docInfo) const;

    bool isReadWrite() const;

    QString path() const;
    void setPath(const QString &path);

    bool closePath(bool promptToSave = true);

    bool saveAs(const QString &path, const QByteArray &mimeType, bool showWarnings, KisPropertiesConfigurationSP exportConfiguration = 0);

    bool newImage(const QString& name, qint32 width, qint32 height, const KoColorSpace * cs, const KoColor &bgColor, KisConfig::BackgroundStyle bgStyle,
                  int numberOfLayers, const QString &imageDescription, const double imageResolution);

    bool isSaving() const;
    void waitForSavingToComplete();

    KisImageWSP image() const;

    KisImageSP savingImage() const;

    void setCurrentImage(KisImageSP image, bool forceInitialUpdate = true, KisNodeSP preActivatedNode = nullptr);

    void hackPreliminarySetImage(KisImageSP image);

    KisUndoStore* createUndoStore();

    KoShapeControllerBase *shapeController() const;

    KoShapeLayer *shapeForNode(KisNodeSP layer) const;

    void setPreActivatedNode(KisNodeSP activatedNode);

    KisNodeSP preActivatedNode() const;

    QList<KisPaintingAssistantSP> assistants() const;

    void setAssistants(const QList<KisPaintingAssistantSP> &value);

    void setAssistantsGlobalColor(QColor color);
    QColor assistantsGlobalColor();

    void setColorHistoryColors(const QList<KoColor> &colors);

    QList<KoColor> colorHistoryColors() const;

    KisUniqueColorSet *colorHistoryModel();

    KisSharedPtr<KisReferenceImagesLayer> referenceImagesLayer() const;

    void setReferenceImagesLayer(KisSharedPtr<KisReferenceImagesLayer> layer, bool updateImage);

    bool save(bool showWarnings, KisPropertiesConfigurationSP exportConfiguration);

    QRectF documentBounds() const;

    void autoSaveOnPause();

Q_SIGNALS:

    void completed();
    void canceled(const QString &);

private Q_SLOTS:

    void setImageModified();
    void setImageModifiedWithoutUndo();

    void slotAutoSave();

    void slotUndoStackCleanChanged(bool value);

    void slotConfigChanged();

    void slotImageRootChanged();

public:
    KisDocument *lockAndCloneForSaving();

    KisDocument *lockAndCreateSnapshot();

    void copyFromDocument(const KisDocument &rhs);

private:

    enum CopyPolicy {
        CONSTRUCT = 0,
        REPLACE
    };

    void copyFromDocumentImpl(const KisDocument &rhs, CopyPolicy policy);

    QString exportErrorToUserMessage(KisImportExportErrorCode status, const QString &errorMessage);

    QString prettyPath() const;

    bool openPathInternal(const QString &path);

    void slotAutoSaveImpl(std::unique_ptr<KisDocument> &&optionalClonedDocument);

    bool resourceSavingFilter(const QString &path, const QByteArray &mimeType, KisPropertiesConfigurationSP exportConfiguration);

    class Private;
    Private *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisDocument::OpenFlags)
Q_DECLARE_METATYPE(KisDocument*)

#endif
