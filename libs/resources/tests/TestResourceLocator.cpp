/*
 * SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TestResourceLocator.h"

#include <simpletest.h>
#include <QVersionNumber>
#include <QDirIterator>
#include <QSqlError>
#include <QSqlQuery>
#include <QBuffer>
#include <QTemporaryFile>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <KritaVersionWrapper.h>

#include <KisResourceCacheDb.h>
#include <KisResourceLocator.h>
#include <KisResourceLoaderRegistry.h>
#include <KisMemoryStorage.h>
#include <KisTagModel.h>
#include <KisTagResourceModel.h>
#include <KisResourceModel.h>
#include <KisResourceTypes.h>

#include <DummyResource.h>
#include <ResourceTestHelper.h>

#include <kis_debug.h>
#include <KisResourceModelProvider.h>
#include <KoMD5Generator.h>

#include <KisResourceMetaDataModel.h>
#include <KisResourceModelProvider.h>
#include <KisSqlQueryLoader.h>

#ifndef FILES_DATA_DIR
#error "FILES_DATA_DIR not set. A directory with the data used for testing installing resources"
#endif

void TestResourceLocator::initTestCase()
{
    const_cast<QLoggingCategory&>(_30010()).setEnabled(QtDebugMsg, false);
    const_cast<QLoggingCategory&>(_30010()).setEnabled(QtInfoMsg, false);
    qputenv("KRITA_OVERRIDE_USE_FOREIGN_KEYS", "1");

    ResourceTestHelper::initTestDb();

    m_srcLocation = QString(FILES_DATA_DIR);
    QVERIFY2(QDir(m_srcLocation).exists(), m_srcLocation.toUtf8());

    m_dstLocation = ResourceTestHelper::filesDestDir();
    ResourceTestHelper::cleanDstLocation(m_dstLocation);

    KConfigGroup cfg(KSharedConfig::openConfig(), "");
    cfg.writeEntry(KisResourceLocator::resourceLocationKey, m_dstLocation);

    m_locator = KisResourceLocator::instance();

    ResourceTestHelper::createDummyLoaderRegistry();
}

void TestResourceLocator::init()
{
    QVERIFY(ResourceTestHelper::recreateDatabaseForATest(m_locator, m_srcLocation, m_dstLocation));
}

void TestResourceLocator::testForeignKeysAreEnabled()
{

    try {

        QCOMPARE(KisResourceCacheDb::getForeignKeysStateImpl(), true);

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();

        QFAIL("SQL Error");
    }
}

void TestResourceLocator::testLocatorInitialization()
{
    QVERIFY(QDir(m_dstLocation).exists());
    Q_FOREACH(const QString &folder, KisResourceLoaderRegistry::instance()->resourceTypes()) {
        QDir dstDir(m_dstLocation + '/' + folder + '/');
        QDir srcDir(m_srcLocation + '/' + folder + '/');

        QVERIFY(dstDir.exists());
        QVERIFY(dstDir.entryList(QDir::Files | QDir::NoDotAndDotDot) == srcDir.entryList(QDir::Files | QDir::NoDotAndDotDot));
    }

    QFile f(m_dstLocation + '/' + "KRITA_RESOURCE_VERSION");
    QVERIFY(f.exists());
    KIS_ASSERT(f.open(QFile::ReadOnly));
    QVersionNumber version = QVersionNumber::fromString(QString::fromUtf8(f.readAll()));
    QVERIFY(version == QVersionNumber::fromString(KritaVersionWrapper::versionString()));

    {
        QSqlQuery query;
        bool r = query.exec("SELECT COUNT(*) FROM storages");
        QVERIFY(r);
        QVERIFY(query.lastError() == QSqlError());
        query.first();
        QCOMPARE(query.value(0).toInt(), 4);
    }

    {
        QSqlQuery query;
        bool r = query.exec("SELECT COUNT(*) FROM resources");
        QVERIFY(r);
        QVERIFY(query.lastError() == QSqlError());
        query.first();
        QCOMPARE(query.value(0).toInt(), 7);
    }

    {
        QSqlQuery query;
        bool r = query.exec("SELECT COUNT(*) FROM tags");
        QVERIFY(r);
        QVERIFY(query.lastError() == QSqlError());
        query.first();
        QCOMPARE(query.value(0).toInt(), 1);
    }
}

void TestResourceLocator::testResourceLocationBase()
{
    QCOMPARE(m_locator->resourceLocationBase(), m_dstLocation);
}

void TestResourceLocator::testResource()
{
    KoResourceSP res = m_locator->resource("", ResourceType::PaintOpPresets, "test0.kpp");
    QVERIFY(res);
}

void TestResourceLocator::testResourceForId()
{
    KoResourceSP res = m_locator->resource("", ResourceType::PaintOpPresets, "test0.kpp");
    int resourceId = KisResourceCacheDb::resourceIdForResource("test0.kpp", ResourceType::PaintOpPresets, "");
    QVERIFY(resourceId > -1);
    KoResourceSP res2 = m_locator->resourceForId(resourceId);
    QCOMPARE(res, res2);
}

void TestResourceLocator::testDocumentStorage()
{
    const QString &documentName("document");

    KisResourceModel model(ResourceType::PaintOpPresets);
    int rowcount = model.rowCount();

    KisResourceStorageSP documentStorage = QSharedPointer<KisResourceStorage>::create(documentName);
    QVERIFY(documentStorage->valid());
    KoResourceSP resource(new DummyResource("test.kpp", ResourceType::PaintOpPresets));
    documentStorage->saveAsNewVersion(resource);

    m_locator->addStorage(documentName, documentStorage);

    QVERIFY(m_locator->hasStorage(documentName));
    QVERIFY(model.rowCount() > rowcount);

    m_locator->removeStorage(documentName);
    QVERIFY(!m_locator->hasStorage(documentName));

    QVERIFY(model.rowCount() == rowcount);
}

int countMetaDataForResourceImpl(int resourceId, const QString &tableName)
{
    try {
        KisSqlQueryLoader loader("inline://count_metadata_for_resource",
                                 QString("SELECT COUNT(*) FROM metadata\n"
                                 "WHERE foreign_id = :resource_id AND table_name = \"%1\"").arg(tableName),
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":resource_id", resourceId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countMetaDataForResource(int resourceId)
{
    return countMetaDataForResourceImpl(resourceId, "resources");
}

int countMetaDataForStorage(int  storageId)
{
    return countMetaDataForResourceImpl(storageId, "storages");
}

int countCurrentResourcesForResourceId(int resourceId)
{
    try {
        KisSqlQueryLoader loader("inline://count_current_resource_for_resource_id",
                                 "SELECT COUNT(*) FROM resources WHERE id = :resource_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":resource_id", resourceId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countVersionedResourcesForResourceId(int resourceId)
{
    try {
        KisSqlQueryLoader loader("inline://count_current_resource_for_resource_id",
                                 "SELECT COUNT(*) FROM versioned_resources WHERE resource_id = :resource_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":resource_id", resourceId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countStorageRecordsForStorageId(int storageId)
{
    try {
        KisSqlQueryLoader loader("inline://count_storage_records_for_storage_id",
                                 "SELECT COUNT(*) FROM storages WHERE id = :storage_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":storage_id", storageId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countStorageRecordsInTagsStoragesForStorageId(int storageId)
{
    try {
        KisSqlQueryLoader loader("inline://count_storage_records_in_tags_records_for_storage_id",
                                 "SELECT COUNT(*) FROM tags_storages WHERE storage_id = :storage_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":storage_id", storageId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countTagRecordsInTags(int tagId)
{
    try {
        KisSqlQueryLoader loader("inline://count_tag_records_in_tags",
                                 "SELECT COUNT(*) FROM tags WHERE id = :tag_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":tag_id", tagId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countTagRecordsInTagTranslations(int tagId)
{
    try {
        KisSqlQueryLoader loader("inline://count_tag_records_in_tag_translations",
                                 "SELECT COUNT(*) FROM tag_translations WHERE tag_id = :tag_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":tag_id", tagId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countTagRecordsInResourceTags(int tagId)
{
    try {
        KisSqlQueryLoader loader("inline://count_tag_records_in_resource_tags",
                                 "SELECT COUNT(*) FROM resource_tags WHERE tag_id = :tag_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":tag_id", tagId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

int countTagRecordsInTagStorages(int tagId)
{
    try {
        KisSqlQueryLoader loader("inline://count_tag_records_in_tag_storages",
                                 "SELECT COUNT(*) FROM tags_storages WHERE tag_id = :tag_id",
                                 KisSqlQueryLoader::single_statement_mode);
        loader.query().bindValue(":tag_id", tagId);
        loader.exec();

        loader.query().first();
        return loader.query().value(0).toInt();

    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();
        return -1;
    }
}

enum MetaDataTestFlag
{
    None = 0x0,
    NewVersionViaLocator = 0x1,
    NewVersionViaStorageSync = 0x2,
    RemoveNewVersionViaStorageSync = 0x4,
    DeleteStorageNormally = 0x8,
    DeleteAllTemporaryStorages = 0x10
};

Q_DECLARE_FLAGS(MetaDataTestFlags, MetaDataTestFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(MetaDataTestFlags)
Q_DECLARE_METATYPE(MetaDataTestFlags)

void TestResourceLocator::testLoadResourceMetadataFromStorage_data()
{
    QTest::addColumn<MetaDataTestFlags>("flags");

    QTest::newRow("no_modifications") << MetaDataTestFlags(None);
    QTest::newRow("locator+delete_storage") <<
        MetaDataTestFlags(NewVersionViaLocator | DeleteStorageNormally);
    QTest::newRow("locator+delete_temporary") <<
        MetaDataTestFlags(NewVersionViaLocator | DeleteAllTemporaryStorages);
    QTest::newRow("storage+delete_storage") <<
        MetaDataTestFlags(NewVersionViaStorageSync | DeleteStorageNormally);
    QTest::newRow("storage+delete_temporary") <<
        MetaDataTestFlags(NewVersionViaStorageSync | DeleteAllTemporaryStorages);
    QTest::newRow("storage+remove+delete_storage") <<
        MetaDataTestFlags(NewVersionViaStorageSync | RemoveNewVersionViaStorageSync | DeleteStorageNormally);
    QTest::newRow("storage+remove+delete_temporary") <<
        MetaDataTestFlags(NewVersionViaStorageSync | RemoveNewVersionViaStorageSync | DeleteAllTemporaryStorages);

}

void TestResourceLocator::testLoadResourceMetadataFromStorage()
{
    QFETCH(MetaDataTestFlags, flags);

    const QString &documentName("document");

    KisResourceMetaDataModel *metadataModel = KisResourceModelProvider::resourceMetadataModel();

    KisResourceModel model(ResourceType::PaintOpPresets);
    const int initialRowCount = model.rowCount();

    KisResourceStorageSP documentStorage = QSharedPointer<KisResourceStorage>::create(documentName);
    QVERIFY(documentStorage->valid());

    documentStorage->setMetaData("test_metadata", "test_storage_metadata_value");

    QSharedPointer<DummyResource> resource(new DummyResource("metadata_test.kpp", ResourceType::PaintOpPresets));
    resource->setSomething("123456789012345678901234567890");

    documentStorage->addResource(resource);

    m_locator->addStorage(documentName, documentStorage);

    QVERIFY(m_locator->hasStorage(documentName));
    QCOMPARE(model.rowCount(), initialRowCount + 1);

    const int documentStorageId = documentStorage->storageId();
    QCOMPARE(countStorageRecordsForStorageId(documentStorageId), 1);
    QCOMPARE(countMetaDataForStorage(documentStorageId), 1);

    QSharedPointer<DummyResource> loadedResource;
    int loadedResourceId = -1;

    {
        auto loadedResources = model.resourcesForFilename("metadata_test.kpp");
        QCOMPARE(loadedResources.size(), 1);
        loadedResource = loadedResources.first().dynamicCast<DummyResource>();
        loadedResourceId = loadedResource->resourceId();
        QVERIFY(loadedResourceId >= 0);

        QCOMPARE(loadedResource->metadata()["test_metadata"], "12345678");
        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "12345678");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);
        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 1);
    }

    KIS_ASSERT(!flags.testFlag(RemoveNewVersionViaStorageSync) ||
                flags.testFlag(NewVersionViaStorageSync));

    if (flags.testFlag(NewVersionViaLocator)) {
        loadedResource->setSomething("098765432109876543210987654321");
        model.updateResource(loadedResource);

        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "09876543");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);

        loadedResource->setSomething("6666666666666666666666666");
        model.updateResource(loadedResource);

        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "66666666");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);

        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 3);
    }

    if (flags.testFlag(NewVersionViaStorageSync)) {
        const QString resourceType = loadedResource->resourceType().first;

        loadedResource->setSomething("098765432109876543210987654321");

        resource->setVersion(resource->version() + 1);
        documentStorage->saveAsNewVersion(loadedResource);
        resource->setMD5Sum(documentStorage->resourceMd5(resourceType + "/" + resource->filename()));
        resource->setDirty(false);

        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "12345678");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);
        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 1);

        KisResourceCacheDb::synchronizeStorage(documentStorage);
        Q_EMIT m_locator->storageResynchronized(documentStorage->location(), false);

        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "09876543");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);
        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 2);

        loadedResource->setSomething("6666666666666666666666666");
        resource->setVersion(resource->version() + 1);
        documentStorage->saveAsNewVersion(loadedResource);
        resource->setMD5Sum(documentStorage->resourceMd5(resourceType + "/" + resource->filename()));
        resource->setDirty(false);

        KisResourceCacheDb::synchronizeStorage(documentStorage);
        Q_EMIT m_locator->storageResynchronized(documentStorage->location(), false);

        QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "66666666");
        QCOMPARE(countMetaDataForResource(loadedResourceId), 1);
        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 3);

        if (flags.testFlag(RemoveNewVersionViaStorageSync)) {

            KisMemoryStorage *memoryStorageBackend =
                dynamic_cast<KisMemoryStorage *>(documentStorage->testingGetStoragePlugin());

            memoryStorageBackend->testingRemoveResource(resourceType + "/" + resource->filename());

            KisResourceCacheDb::synchronizeStorage(documentStorage);
            Q_EMIT m_locator->storageResynchronized(documentStorage->location(), false);

            QCOMPARE(metadataModel->metaDataValue(loadedResourceId, "test_metadata"), "09876543");
            QCOMPARE(countMetaDataForResource(loadedResourceId), 1);
            QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 1);
            QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 2);
        }
    }

    if (flags.testFlag(DeleteStorageNormally)) {
        m_locator->removeStorage(documentName);
    }

    if (flags.testFlag(DeleteAllTemporaryStorages)) {
        KisResourceCacheDb::deleteTemporaryResources();
    }

    if (loadedResourceId >= 0 && flags & (DeleteStorageNormally | DeleteAllTemporaryStorages)) {
        QVERIFY(!metadataModel->metaDataValue(loadedResourceId, "test_metadata").isValid());

        QCOMPARE(countMetaDataForResource(loadedResourceId), 0);
        QCOMPARE(countCurrentResourcesForResourceId(loadedResourceId), 0);
        QCOMPARE(countVersionedResourcesForResourceId(loadedResourceId), 0);
    }

    if (flags & (DeleteStorageNormally | DeleteAllTemporaryStorages)) {
        QCOMPARE(countStorageRecordsForStorageId(documentStorageId), 0);
        QCOMPARE(countMetaDataForStorage(documentStorageId), 0);
    }

    if (flags.testFlag(DeleteStorageNormally)) {
        QCOMPARE(model.rowCount(), initialRowCount);
    }
}

enum TagDeletionMethod
{
    StorageExplicitDelete = 0,
    StorageDeleteAllTemporary,
    StorageDeleteTagAndResync
};
Q_DECLARE_METATYPE(TagDeletionMethod)

class TestStorageWrapper {
public:

    const int storageIndex = 0;
    const int resourceIndex = 0;
    const int tagIndex = 0;
    KisResourceStorageSP storage;

    QString storageLocation() const {
        return QString("document_%1").arg(storageIndex);
    }
    QString tagUrl() const {
        return QString("test_tag_url_%1").arg(tagIndex);
    }

    QString resourceType() const {
        return ResourceType::PaintOpPresets;
    }

    QString resourceName() const {
        return QString("metadata_test_%1.kpp").arg(resourceIndex);
    }

    KisMemoryStorage* memoryStorageBackend() const {
        return dynamic_cast<KisMemoryStorage *>(storage->testingGetStoragePlugin());
    }

    TestStorageWrapper(int storageIndexArg, int resourceIndexArg, int tagIndexTag)
        : storageIndex(storageIndexArg)
        , resourceIndex(resourceIndexArg)
        , tagIndex(tagIndexTag)
    {
        storage = QSharedPointer<KisResourceStorage>::create(storageLocation());
        QVERIFY(storage->valid());

        KisMemoryStorage *memoryStorageBackend =
            dynamic_cast<KisMemoryStorage *>(storage->testingGetStoragePlugin());

        QSharedPointer<DummyResource> resource(new DummyResource(resourceName(), resourceType()));
        resource->setSomething(QString("123456789012345678901234567890_%1").arg(resourceIndex));

        storage->addResource(resource);

        {
            KisTagSP tag(new KisTag());
            tag->setName(QString("Test Tag %1").arg(tagIndex));
            tag->setNames({{"ru", QString("Тестовый тег %1").arg(tagIndex)}});
            tag->setUrl(tagUrl());
            tag->setResourceType(resourceType());
            tag->setDefaultResources({resource->filename()});
            tag->setValid(true);
            memoryStorageBackend->testingAddTag(ResourceType::PaintOpPresets, tag);
        }
    }
};

void TestResourceLocator::testRemoveTagsFromStorage_data()
{
    QTest::addColumn<TagDeletionMethod>("tagDeletionMethod");

    QTest::newRow("storage_explicit_delete") << StorageExplicitDelete;
    QTest::newRow("delete_all_temporary") << StorageDeleteAllTemporary;
    QTest::newRow("delete_tag_and_resync") << StorageDeleteTagAndResync;
}

void TestResourceLocator::testRemoveTagsFromStorage()
{
    QFETCH(TagDeletionMethod, tagDeletionMethod);

    TestStorageWrapper storageWrapper( 1, 1, 1);

    KisTagModel tagModel(ResourceType::PaintOpPresets);
    const int initialTagRowCount = tagModel.rowCount();

    KisTagResourceModel tagResourceModel(ResourceType::PaintOpPresets);
    const int initialTagResourceRowCount = tagResourceModel.rowCount();

    KisResourceModel resourceModel(ResourceType::PaintOpPresets);
    const int initialResourceRowCount = resourceModel.rowCount();

    m_locator->addStorage(storageWrapper.storageLocation(), storageWrapper.storage);

    QVERIFY(m_locator->hasStorage(storageWrapper.storageLocation()));
    QCOMPARE(resourceModel.rowCount(), initialResourceRowCount + 1);
    QCOMPARE(tagModel.rowCount(), initialTagRowCount + 1);
    QCOMPARE(tagResourceModel.rowCount(), initialTagResourceRowCount + 1);

    const int documentStorageId = storageWrapper.storage->storageId();
    QCOMPARE(countStorageRecordsForStorageId(documentStorageId), 1);
    QCOMPARE(countStorageRecordsInTagsStoragesForStorageId(documentStorageId), 1);

    KisTagSP loadedTag = tagModel.tagForUrl(storageWrapper.tagUrl());
    QVERIFY(loadedTag);

    const int testTagId = loadedTag->id();

    QCOMPARE(countTagRecordsInTags(testTagId), 1);
    QCOMPARE(countTagRecordsInTagTranslations(testTagId), 1);
    QCOMPARE(countTagRecordsInResourceTags(testTagId), 1);
    QCOMPARE(countTagRecordsInTagStorages(testTagId), 1);

    if (tagDeletionMethod == StorageExplicitDelete || tagDeletionMethod == StorageDeleteAllTemporary) {
        if (tagDeletionMethod == StorageExplicitDelete) {
            m_locator->removeStorage(storageWrapper.storageLocation());
        } else if (tagDeletionMethod == StorageDeleteAllTemporary) {
            KisResourceCacheDb::deleteTemporaryResources();
        }

        QEXPECT_FAIL("delete_all_temporary", "Deletion of temporary storages with tag is not yet implemented", Abort);

        QCOMPARE(countStorageRecordsForStorageId(documentStorageId), 0);
        QCOMPARE(countStorageRecordsInTagsStoragesForStorageId(documentStorageId), 0);

        QCOMPARE(countTagRecordsInTags(testTagId), 0);
        QCOMPARE(countTagRecordsInTagTranslations(testTagId), 0);
        QCOMPARE(countTagRecordsInResourceTags(testTagId), 0);
        QCOMPARE(countTagRecordsInTagStorages(testTagId), 0);
    } else if (tagDeletionMethod == StorageDeleteTagAndResync) {
        storageWrapper.memoryStorageBackend()->testingRemoveTag(ResourceType::PaintOpPresets, storageWrapper.tagUrl());
        KisResourceCacheDb::synchronizeStorage(storageWrapper.storage);
        Q_EMIT m_locator->storageResynchronized(storageWrapper.storageLocation(), false);

        QCOMPARE(countStorageRecordsForStorageId(documentStorageId), 1);

        QEXPECT_FAIL("delete_tag_and_resync", "Deletion of tags on storage resync is not yet implemented", Abort);

        QCOMPARE(countStorageRecordsInTagsStoragesForStorageId(documentStorageId), 0);
        QCOMPARE(countTagRecordsInTags(testTagId), 0);
        QCOMPARE(countTagRecordsInTagTranslations(testTagId), 0);
        QCOMPARE(countTagRecordsInResourceTags(testTagId), 0);
        QCOMPARE(countTagRecordsInTagStorages(testTagId), 0);
    }
}

void TestResourceLocator::testTagsForStorageUnique()
{
    TestStorageWrapper storageWrapper1( 1, 1, 1);
    TestStorageWrapper storageWrapper2( 2, 2, 2);

    m_locator->addStorage(storageWrapper1.storageLocation(), storageWrapper1.storage);
    m_locator->addStorage(storageWrapper2.storageLocation(), storageWrapper2.storage);

    QVERIFY(m_locator->hasStorage(storageWrapper1.storageLocation()));
    QVERIFY(m_locator->hasStorage(storageWrapper2.storageLocation()));

    {
        KisTagModel tagModel(ResourceType::PaintOpPresets);
        KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
        QVERIFY(tag1);
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(storageWrapper1.resourceType(), storageWrapper1.storageLocation());
        QCOMPARE(unique, {tag1->id()});
        QCOMPARE(shared, {});
    }

    {
        KisTagModel tagModel(ResourceType::PaintOpPresets);
        KisTagSP tag2 = tagModel.tagForUrl(storageWrapper2.tagUrl());
        QVERIFY(tag2);
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(storageWrapper2.resourceType(), storageWrapper2.storageLocation());
        QCOMPARE(unique, {tag2->id()});
        QCOMPARE(shared, {});
    }

    {
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(ResourceType::Brushes, storageWrapper1.storageLocation());
        QCOMPARE(unique, {});
        QCOMPARE(shared, {});
    }
}
void TestResourceLocator::testTagsForStorageShared()
{

    TestStorageWrapper storageWrapper1( 1, 1, 1);
    TestStorageWrapper storageWrapper2( 2, 2, 1);

    m_locator->addStorage(storageWrapper1.storageLocation(), storageWrapper1.storage);
    m_locator->addStorage(storageWrapper2.storageLocation(), storageWrapper2.storage);

    QVERIFY(m_locator->hasStorage(storageWrapper1.storageLocation()));
    QVERIFY(m_locator->hasStorage(storageWrapper2.storageLocation()));

    {
        KisTagModel tagModel(ResourceType::PaintOpPresets);
        KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
        QVERIFY(tag1);
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(storageWrapper1.resourceType(), storageWrapper1.storageLocation());
        QCOMPARE(unique, {});
        QCOMPARE(shared, {tag1->id()});
    }

    {
        KisTagModel tagModel(ResourceType::PaintOpPresets);
        KisTagSP tag2 = tagModel.tagForUrl(storageWrapper2.tagUrl());
        QVERIFY(tag2);
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(storageWrapper2.resourceType(), storageWrapper2.storageLocation());
        QCOMPARE(unique, {});
        QCOMPARE(shared, {tag2->id()});
    }

    {
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(ResourceType::Brushes, storageWrapper1.storageLocation());
        QCOMPARE(unique, {});
        QCOMPARE(shared, {});
    }

    m_locator->removeStorage(storageWrapper2.storageLocation());

    {
        KisTagModel tagModel(ResourceType::PaintOpPresets);
        KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
        QVERIFY(tag1);
        auto [unique, shared] = KisResourceCacheDb::tagsForStorage(storageWrapper1.resourceType(), storageWrapper1.storageLocation());
        QCOMPARE(unique, {tag1->id()});
        QCOMPARE(shared, {});
    }
}

void TestResourceLocator::testDeleteStorageWithCrossLinkedTags()
{

    const TagDeletionMethod tagDeletionMethod = StorageExplicitDelete;

    TestStorageWrapper storageWrapper1( 1, 1, 1);
    TestStorageWrapper storageWrapper2( 2, 2, 2);

    KisTagModel tagModel(ResourceType::PaintOpPresets);
    KisTagResourceModel tagResourceModel(ResourceType::PaintOpPresets);

    m_locator->addStorage(storageWrapper1.storageLocation(), storageWrapper1.storage);
    m_locator->addStorage(storageWrapper2.storageLocation(), storageWrapper2.storage);

    QVERIFY(m_locator->hasStorage(storageWrapper1.storageLocation()));
    QVERIFY(m_locator->hasStorage(storageWrapper2.storageLocation()));

    KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
    QVERIFY(tag1);

    KoResourceSP resource2 = m_locator->resource(storageWrapper2.storageLocation(),  storageWrapper2.resourceType(), storageWrapper2.resourceName());
    QVERIFY(resource2);

    tagResourceModel.tagResources(tag1, {resource2->resourceId()});

    QCOMPARE(countTagRecordsInTags(tag1->id()), 1);
    QCOMPARE(countTagRecordsInTagTranslations(tag1->id()), 1);
    QCOMPARE(countTagRecordsInResourceTags(tag1->id()), 2);
    QCOMPARE(countTagRecordsInTagStorages(tag1->id()), 1);

    tagResourceModel.setResourcesFilter({resource2});
    QCOMPARE(tagResourceModel.rowCount(), 2);

    if (tagDeletionMethod == StorageExplicitDelete || tagDeletionMethod == StorageDeleteAllTemporary) {
        const int deletedStorageId = storageWrapper1.storage->storageId();
        const int deletedTagId = [&]() {
            KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
            KIS_ASSERT(tag1);
            return tag1->id();
        }();

        if (tagDeletionMethod == StorageExplicitDelete) {
            m_locator->removeStorage(storageWrapper1.storageLocation());
        } else if (tagDeletionMethod == StorageDeleteAllTemporary) {
            KisResourceCacheDb::deleteTemporaryResources();
        }

        QCOMPARE(countStorageRecordsForStorageId(deletedStorageId), 0);
        QCOMPARE(countStorageRecordsInTagsStoragesForStorageId(deletedStorageId), 0);

        QCOMPARE(countTagRecordsInTags(deletedTagId), 0);
        QCOMPARE(countTagRecordsInTagTranslations(deletedTagId), 0);
        QCOMPARE(countTagRecordsInResourceTags(deletedTagId), 0);
        QCOMPARE(countTagRecordsInTagStorages(deletedTagId), 0);

        {
            KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
            QVERIFY(!tag1);
        }

        {
            KisTagSP tag2 = tagModel.tagForUrl(storageWrapper2.tagUrl());
            QVERIFY(tag2);
        }

        QCOMPARE(tagResourceModel.rowCount(), 1);
    }
}

void TestResourceLocator::testDeleteStorageWithSharedTags()
{
    const TagDeletionMethod tagDeletionMethod = StorageExplicitDelete;

    TestStorageWrapper storageWrapper1( 1, 1, 1);
    TestStorageWrapper storageWrapper2( 2, 2, 1);

    KisTagModel tagModel(ResourceType::PaintOpPresets);
    KisTagResourceModel tagResourceModel(ResourceType::PaintOpPresets);

    m_locator->addStorage(storageWrapper1.storageLocation(), storageWrapper1.storage);
    m_locator->addStorage(storageWrapper2.storageLocation(), storageWrapper2.storage);

    QVERIFY(m_locator->hasStorage(storageWrapper1.storageLocation()));
    QVERIFY(m_locator->hasStorage(storageWrapper2.storageLocation()));

    KisTagSP tag1 = tagModel.tagForUrl(storageWrapper1.tagUrl());
    QVERIFY(tag1);

    QCOMPARE(countTagRecordsInTags(tag1->id()), 1);
    QCOMPARE(countTagRecordsInTagTranslations(tag1->id()), 1);
    QCOMPARE(countTagRecordsInResourceTags(tag1->id()), 2);
    QCOMPARE(countTagRecordsInTagStorages(tag1->id()), 2);

    {
        KoResourceSP resource2 = m_locator->resource(storageWrapper2.storageLocation(),
                                                     storageWrapper2.resourceType(),
                                                     storageWrapper2.resourceName());
        QVERIFY(resource2);

        tagResourceModel.setTagsFilter(QVector<KisTagSP>{});
        tagResourceModel.setResourcesFilter({resource2});
        QCOMPARE(tagResourceModel.rowCount(), 1);
    }

    {
        KoResourceSP resource1 = m_locator->resource(storageWrapper2.storageLocation(),
                                                     storageWrapper2.resourceType(),
                                                     storageWrapper2.resourceName());
        QVERIFY(resource1);

        tagResourceModel.setTagsFilter(QVector<KisTagSP>{});
        tagResourceModel.setResourcesFilter({resource1});
        QCOMPARE(tagResourceModel.rowCount(), 1);
    }

    {
        tagResourceModel.setTagsFilter({tag1});
        tagResourceModel.setResourcesFilter(QVector<KoResourceSP>{});
        QCOMPARE(tagResourceModel.rowCount(), 2);
    }

    if (tagDeletionMethod == StorageExplicitDelete || tagDeletionMethod == StorageDeleteAllTemporary) {
        const int deletedStorageId = storageWrapper1.storage->storageId();

        if (tagDeletionMethod == StorageExplicitDelete) {
            m_locator->removeStorage(storageWrapper1.storageLocation());
        } else if (tagDeletionMethod == StorageDeleteAllTemporary) {
            KisResourceCacheDb::deleteTemporaryResources();
        }

        QCOMPARE(countStorageRecordsForStorageId(deletedStorageId), 0);
        QCOMPARE(countStorageRecordsInTagsStoragesForStorageId(deletedStorageId), 0);

        KisTagSP tag2 = tagModel.tagForUrl(storageWrapper2.tagUrl());
        QVERIFY(tag2);

        QCOMPARE(countTagRecordsInTags(tag2->id()), 1);
        QCOMPARE(countTagRecordsInTagTranslations(tag2->id()), 1);
        QCOMPARE(countTagRecordsInResourceTags(tag2->id()), 1);
        QCOMPARE(countTagRecordsInTagStorages(tag2->id()), 1);

        {
            KisTagResourceModel tagResourceModel(ResourceType::PaintOpPresets);
            KoResourceSP resource2 = m_locator->resource(storageWrapper2.storageLocation(),
                                                         storageWrapper2.resourceType(),
                                                         storageWrapper2.resourceName());
            QVERIFY(resource2);

            tagResourceModel.setTagsFilter(QVector<KisTagSP>{});
            tagResourceModel.setResourcesFilter({resource2});
            QCOMPARE(tagResourceModel.rowCount(), 1);
        }
    }
}

void TestResourceLocator::testSyncVersions()
{
    int resourceId = -1;
    QString storageLocation;

    {
        KisResourceModel model(ResourceType::PaintOpPresets);
        model.setResourceFilter(KisResourceModel::ShowAllResources);

        KoResourceSP res = m_locator->resource("", ResourceType::PaintOpPresets, "test0.kpp");
        resourceId = KisResourceCacheDb::resourceIdForResource("test0.kpp", ResourceType::PaintOpPresets, "");
        storageLocation = res->storageLocation();

        {
            bool result = m_locator->updateResource(res->resourceType().first, res);
            QVERIFY(result);
            QCOMPARE(res->version(), 1);
        }

        {
            bool result = m_locator->updateResource(res->resourceType().first, res);
            QVERIFY(result);
            QCOMPARE(res->version(), 2);
        }

        QCOMPARE(model.rowCount(), 3);

        {
            KoResourceSP res1 = model.resourceForIndex(model.index(0, 0));
            QVERIFY(res1);
            QCOMPARE(res1->resourceId(), resourceId);
            QCOMPARE(res1->version(), 2);
        }

    }

    {
        bool result = QFile::remove(storageLocation + "/paintoppresets/test0.0002.kpp");
        QVERIFY(result);

        m_locator->synchronizeDb();
        KisResourceModelProvider::testingResetAllModels();

        KisResourceModel model(ResourceType::PaintOpPresets);
        model.setResourceFilter(KisResourceModel::ShowAllResources);

        QCOMPARE(model.rowCount(), 3);

        KoResourceSP res1 = model.resourceForIndex(model.index(0, 0));
        QVERIFY(res1);
        QCOMPARE(res1->resourceId(), resourceId);
        QCOMPARE(res1->version(), 1);

    }

    {
        bool result = QFile::copy(storageLocation + "/paintoppresets/test0.0001.kpp",
                                  storageLocation + "/paintoppresets/test0.0006.kpp");
        QVERIFY(result);

        m_locator->synchronizeDb();
        KisResourceModelProvider::testingResetAllModels();

        KisResourceModel model(ResourceType::PaintOpPresets);
        model.setResourceFilter(KisResourceModel::ShowAllResources);

        QCOMPARE(model.rowCount(), 3);

        KoResourceSP res1 = model.resourceForIndex(model.index(0, 0));
        QVERIFY(res1);
        QVERIFY(res1->filename().startsWith("test0"));
        QCOMPARE(res1->resourceId(), resourceId);
        QCOMPARE(res1->version(), 6);
    }

    {
        bool result = QFile::copy(storageLocation + "/paintoppresets/test0.0001.kpp",
                                  storageLocation + "/paintoppresets/test5.0004.kpp");
        QVERIFY(result);

        result = QFile::copy(storageLocation + "/paintoppresets/test0.0001.kpp",
                             storageLocation + "/paintoppresets/test6.0003.kpp");
        QVERIFY(result);

        m_locator->synchronizeDb();
        KisResourceModelProvider::testingResetAllModels();

        KisResourceModel model(ResourceType::PaintOpPresets);
        model.setResourceFilter(KisResourceModel::ShowAllResources);

        QCOMPARE(model.rowCount(), 5);

        {
            KoResourceSP res1 = model.resourceForIndex(model.index(3, 0));
            QVERIFY(res1->filename().startsWith("test5"));
            QCOMPARE(res1->version(), 4);
        }

        {
            KoResourceSP res1 = model.resourceForIndex(model.index(4, 0));
            QVERIFY(res1->filename().startsWith("test6"));
            QCOMPARE(res1->version(), 3);
        }
    }

    {
        bool result = QFile::remove(storageLocation + "/paintoppresets/test5.0004.kpp");
        QVERIFY(result);

        m_locator->synchronizeDb();
        KisResourceModelProvider::testingResetAllModels();

        KisResourceModel model(ResourceType::PaintOpPresets);
        model.setResourceFilter(KisResourceModel::ShowAllResources);

        QCOMPARE(model.rowCount(), 4);

        KoResourceSP res1 = model.resourceForIndex(model.index(3, 0));
        QVERIFY(res1->filename().startsWith("test6"));
        QCOMPARE(res1->version(), 3);
    }
}

void TestResourceLocator::testImportExportResource()
{
    QTemporaryFile f(QDir::tempPath() + "/testresourcemodel-testimportresourcefile-XXXXXX.kpp");
    KIS_ASSERT(f.open());
    f.write("mysimpletestresource");
    f.close();

    const QString dataMd5 = KoMD5Generator::generateHash(f.fileName());

    KoResourceSP resource = KisResourceLocator::instance()->importResourceFromFile(ResourceType::PaintOpPresets, f.fileName(), false);
    QVERIFY(resource);

    QCOMPARE(resource->md5Sum(false), dataMd5);

    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        resource->saveToDevice(&buffer);
        buffer.close();

        QCOMPARE(KoMD5Generator::generateHash(buffer.data()), dataMd5);
    }

    QSharedPointer<DummyResource> dummyResource = resource.dynamicCast<DummyResource>();
    KIS_ASSERT(dummyResource);

    dummyResource->setSomething("some weird data to change MD5");

    QCOMPARE(resource->md5Sum(false), dataMd5);

    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        resource->saveToDevice(&buffer);
        buffer.close();

        QVERIFY(KoMD5Generator::generateHash(buffer.data()) != dataMd5);
    }

    {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);

        KisResourceLocator::instance()->exportResource(resource, &buffer);
        QCOMPARE(KoMD5Generator::generateHash(buffer.data()), dataMd5);
    }

}

void TestResourceLocator::testImportDuplicatedResource()
{
    QTemporaryFile f(QDir::tempPath() + "/testresourcelocator-testimportduplicated-XXXXXX.kpp");
    KIS_ASSERT(f.open());
    f.write("mysimpletestresource_version1");
    f.close();

    const QString dataMd5 = KoMD5Generator::generateHash(f.fileName());
    const QString fileName = QFileInfo(f.fileName()).fileName();

    KoResourceSP resource = KisResourceLocator::instance()->importResourceFromFile(ResourceType::PaintOpPresets, f.fileName(), false);
    QVERIFY(resource);

    QCOMPARE(resource->md5Sum(false), dataMd5);
    QCOMPARE(resource->version(), 0);

    KoResourceSP res0 = KisResourceLocator::instance()->importResourceFromFile(ResourceType::PaintOpPresets, f.fileName(), false);
    QVERIFY(!res0);

    KoResourceSP res1 = KisResourceLocator::instance()->importResourceFromFile(ResourceType::PaintOpPresets, f.fileName(), true);

    QCOMPARE(res1, resource);

    bool result = KisResourceLocator::instance()->updateResource(ResourceType::PaintOpPresets, resource);
    QVERIFY(result);
    QCOMPARE(resource->version(), 1);

    KoResourceSP res3 = KisResourceLocator::instance()->importResourceFromFile(ResourceType::PaintOpPresets, f.fileName(), true);
    QVERIFY(res3);
    QVERIFY(res3 != resource);
    QCOMPARE(res3->md5Sum(false), dataMd5);
    QCOMPARE(res3->version(), 0);

    int resourceId = -1;
    result = KisResourceCacheDb::getResourceIdFromVersionedFilename(resource->filename(), resource->resourceType().first, resource->storageLocation(), resourceId);
    QVERIFY(!result);
    QVERIFY(resourceId < 0);

#if defined Q_OS_WIN || defined Q_OS_MACOS

    KIS_ASSERT(f.open());
    KoResourceSP res4 = KisResourceLocator::instance()->importResource(ResourceType::PaintOpPresets, fileName.toUpper(), &f, false);
    f.close();

    QVERIFY(!res4);

    KIS_ASSERT(f.open());
    KoResourceSP res5 = KisResourceLocator::instance()->importResource(ResourceType::PaintOpPresets, fileName.toUpper(), &f, true);
    f.close();

    QVERIFY(res5);
    QVERIFY(res5 != res3);
    QCOMPARE(res5->md5Sum(false), dataMd5);
    QCOMPARE(res5->version(), 0);
    QCOMPARE(res5->filename(), fileName.toUpper());

#endif
}

void TestResourceLocator::testOrphanedMetadataRemoval_data()
{
    QTest::addColumn<bool>("useMigrationScript");

    QTest::newRow("migration") << true;
    QTest::newRow("explicit") << false;
}

void TestResourceLocator::testOrphanedMetadataRemoval()
{
    QFETCH(bool, useMigrationScript);

    QCOMPARE(countCurrentResourcesForResourceId(5), 1);
    QCOMPARE(countVersionedResourcesForResourceId(5), 1);
    QCOMPARE(countMetaDataForResource(5), 1);

    try {
        KisResourceCacheDb::setForeignKeysStateImpl(false);

        KisDatabaseTransactionLock transactionLock(QSqlDatabase::database());

        KisSqlQueryLoader loader("inline://make_resource_orphaned",
                                 "DELETE FROM resources WHERE id = 5;"
                                 "DELETE FROM versioned_resources WHERE resource_id = 5;");
        loader.exec();

        transactionLock.commit();

        KisResourceCacheDb::setForeignKeysStateImpl(true);
    } catch (const KisSqlQueryLoader::SQLException &e) {
        qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
        qWarning().noquote() << "       file:" << e.filePath;
        qWarning().noquote() << "       statement:" << e.statementIndex;
        qWarning().noquote() << "       error:" << e.sqlError.text();

        QFAIL("SQL query failed");
    }

    QCOMPARE(countCurrentResourcesForResourceId(5), 0);
    QCOMPARE(countVersionedResourcesForResourceId(5), 0);
    QCOMPARE(countMetaDataForResource(5), 1);

    if (!useMigrationScript) {
        KisResourceCacheDb::removeOrphanedMetaData();
    } else {
        try {
            KisDatabaseTransactionLock transactionLock(QSqlDatabase::database());

            KisSqlQueryLoader loader(":/0_0_18_0001_cleanup_metadata_table.sql");
            loader.exec();

            transactionLock.commit();

        } catch (const KisSqlQueryLoader::FileException &e) {
            qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
            qWarning().noquote() << "       file:" << e.filePath;
            qWarning().noquote() << "       file-error:" << e.fileErrorString;
            QFAIL("SQL query failed");
        } catch (const KisSqlQueryLoader::SQLException &e) {
            qWarning().noquote() << "ERROR: failed to execute query:" << e.message;
            qWarning().noquote() << "       file:" << e.filePath;
            qWarning().noquote() << "       statement:" << e.statementIndex;
            qWarning().noquote() << "       error:" << e.sqlError.text();
            QFAIL("SQL query failed");
        }
    }

    QCOMPARE(countCurrentResourcesForResourceId(5), 0);
    QCOMPARE(countVersionedResourcesForResourceId(5), 0);
    QCOMPARE(countMetaDataForResource(5), 0);

}

void TestResourceLocator::cleanupTestCase()
{
    ResourceTestHelper::rmTestDb();
    ResourceTestHelper::cleanDstLocation(m_dstLocation);
}

SIMPLE_TEST_MAIN(TestResourceLocator)

