/*
 *  SPDX-FileCopyrightText: 2024 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisPaintOpPresetTest.h"

#include <testimage.h>

#include <testutil.h>
#include <KisResourceModel.h>

#include "kis_paintop_preset.h"
#include "KisLocalStrokeResources.h"
#include <KisGlobalResourcesInterface.h>

#include <QFileInfo>

namespace {
template <typename C, typename T = typename C::value_type>
QSet<T> toSet(const C &container) {
    return QSet<T>(container.begin(), container.end());
}
}

void KisPaintOpPresetTest::testLoadingEmbeddedResources_data()
{
    QTest::addColumn<QString>("testFileName");
    QTest::addColumn<QStringList>("expectedSideLoadedResources");
    QTest::addColumn<QStringList>("expectedLinkedResources");
    QTest::addColumn<QStringList>("expectedEmbeddedResources");

    QTest::newRow("ver-2.2")
        << "test-embedded-resources-2.2.kpp"
        << QStringList{}
        << QStringList{"test_brush.png"}
        << QStringList{"6d8b596cfb5220b06174145fb3fbbaed"};

    QTest::newRow("ver-5.0")
        << "test-embedded-resources-5.0.kpp"
        << QStringList{"9a90b42a7bb2e7cef22689bf3abcdba6", "dc4e9099acb7c3cd33293a48f75c6ff7"}
        << QStringList{"9a90b42a7bb2e7cef22689bf3abcdba6", "dc4e9099acb7c3cd33293a48f75c6ff7"}
        << QStringList{};

        QTest::newRow("bad-md5")
            << "test-embedded-resources-bad-md5.kpp"
            << QStringList{"9a90b42a7bb2e7cef22689bf3abcdba6", "59bd2f6763fd9b1669c532a7c99ae781"}
            << QStringList{"9a90b42a7bb2e7cef22689bf3abcdba6", "59bd2f6763fd9b1669c532a7c99ae781"}
            << QStringList{};
}

void KisPaintOpPresetTest::testLoadingEmbeddedResources()
{
    QFETCH(QString, testFileName);
    QFETCH(QStringList, expectedSideLoadedResources);
    QFETCH(QStringList, expectedLinkedResources);
    QFETCH(QStringList, expectedEmbeddedResources);

    const QString fileName = QString(FILES_DATA_DIR) + QDir::separator() + testFileName;

    QVERIFY(QFileInfo(fileName).exists());

    QSharedPointer<KisLocalStrokeResources> linkedResources(new KisLocalStrokeResources());

    KisPaintOpPresetSP preset(new KisPaintOpPreset(fileName));
    preset->load(linkedResources);

    QVERIFY(preset->valid());

    QSet<QString> realSideLoadedSignatures;
    QSet<QString> realLinkedSignatures;
    QSet<QString> realEmbeddedSignatures;

    Q_FOREACH (const KoResourceLoadResult &result, preset->sideLoadedResources(linkedResources)) {
        QCOMPARE(result.type(), KoResourceLoadResult::EmbeddedResource);
        QVERIFY(result.embeddedResource().isValid());
        QVERIFY(result.embeddedResource().sanityCheckMd5());

        realSideLoadedSignatures << result.signature().md5sum;
    }

    preset->clearSideLoadedResources();
    QCOMPARE(preset->sideLoadedResources(linkedResources).size(), 0);

    Q_FOREACH (const KoResourceLoadResult &result, preset->linkedResources(linkedResources)) {

        QCOMPARE(result.type(), KoResourceLoadResult::FailedLink);

        realLinkedSignatures <<
            (!result.signature().md5sum.isEmpty() ?
                result.signature().md5sum :
                result.signature().filename);
    }

    Q_FOREACH (const KoResourceLoadResult &result, preset->embeddedResources(linkedResources)) {
        QCOMPARE(result.type(), KoResourceLoadResult::EmbeddedResource);
        realEmbeddedSignatures << result.signature().md5sum;
    }

    QCOMPARE(realSideLoadedSignatures, toSet(expectedSideLoadedResources));
    QCOMPARE(realLinkedSignatures, toSet(expectedLinkedResources));
    QCOMPARE(realEmbeddedSignatures, toSet(expectedEmbeddedResources));
}

void KisPaintOpPresetTest::testConflictingEmbeddedPatterns()
{
    QSharedPointer<KisLocalStrokeResources> emptyLocalResources(new KisLocalStrokeResources());
    KisResourcesInterfaceSP resourcesInterface = KisGlobalResourcesInterface::instance();

    KisResourceModel model(ResourceType::PaintOpPresets);

    int verticalPatternResourceId = -1;

    {

        QString presetFileName(TestUtil::fetchDataFileLazy("conflicting-patterns/preset-stripes-vert.kpp"));
        QFile file(presetFileName);
        KIS_ASSERT(file.open(QFile::ReadOnly));

        KoResourceSP resource = model.importResource(QFileInfo(presetFileName).fileName(), &file, false, "memory");
        QVERIFY(resource);

        KisPaintOpPresetSP preset = resource.dynamicCast<KisPaintOpPreset>();

        KoPatternSP loadedPattern = preset->linkedResources(resourcesInterface).first().resource<KoPattern>();
        QVERIFY(loadedPattern);
        QCOMPARE(loadedPattern->name(), "stripes-pat.png");
        QCOMPARE(loadedPattern->md5Sum(), "d3bc7abb7136295578c6b3af3da02fcc");

        QVERIFY(loadedPattern->resourceId() >= 0);
        verticalPatternResourceId = loadedPattern->resourceId();
    }

    {

        QString presetFileName(TestUtil::fetchDataFileLazy("conflicting-patterns/preset-stripes-vert-v2.kpp"));
        QFile file(presetFileName);
        KIS_ASSERT(file.open(QFile::ReadOnly));

        KoResourceSP resource = model.importResource(QFileInfo(presetFileName).fileName(), &file, false, "memory");
        QVERIFY(resource);

        KisPaintOpPresetSP preset = resource.dynamicCast<KisPaintOpPreset>();

        KoPatternSP loadedPattern = preset->linkedResources(resourcesInterface).first().resource<KoPattern>();
        QVERIFY(loadedPattern);
        QCOMPARE(loadedPattern->name(), "stripes-pat.png");
        QCOMPARE(loadedPattern->md5Sum(), "d3bc7abb7136295578c6b3af3da02fcc");

        QCOMPARE(loadedPattern->resourceId(), verticalPatternResourceId);
    }

    {

        QString presetFileName(TestUtil::fetchDataFileLazy("conflicting-patterns/preset-stripes-cross.kpp"));
        QFile file(presetFileName);
        KIS_ASSERT(file.open(QFile::ReadOnly));

        KoResourceSP resource = model.importResource(QFileInfo(presetFileName).fileName(), &file, false, "memory");
        QVERIFY(resource);

        KisPaintOpPresetSP preset = resource.dynamicCast<KisPaintOpPreset>();

        auto resourcesForEmptyEnvironment = preset->linkedResources(emptyLocalResources);
        QCOMPARE(resourcesForEmptyEnvironment.size(), 1);
        QCOMPARE(resourcesForEmptyEnvironment.first().type(), KoResourceLoadResult::FailedLink);
        const QString requiredMd5Sum = resourcesForEmptyEnvironment.first().signature().md5sum;
        QCOMPARE(requiredMd5Sum, "cca80dd27e4085ef089729462714e942");

        KoPatternSP loadedPattern = preset->linkedResources(resourcesInterface).first().resource<KoPattern>();
        QVERIFY(loadedPattern);
        QCOMPARE(loadedPattern->md5Sum(), requiredMd5Sum);

        QVERIFY(loadedPattern->resourceId() != verticalPatternResourceId);
    }
}

KISTEST_MAIN(KisPaintOpPresetTest)
