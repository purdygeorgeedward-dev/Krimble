/*
 * This file contains tests for:
 *
 * - KisPaintingAssistantsDecoration
 * - ParallelRulerAssistant
 * - PerspectiveAssistant
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <testutil.h>
#include "qimage_based_test.h"

#include <KoCanvasResourceProvider.h>
#include "kis_canvas_resource_provider.h"
#include <util.h>
#include <KisMainWindow.h>
#include <KisDocument.h>
#include <KisPart.h>
#include <KisView.h>
#include <KisViewManager.h>
#include <KisDecorationsManager.h>

#include "kis_painting_assistants_decoration.h"
#include "KisPart.h"
#include "testui.h"
#include <kis_config.h>
#include <testutil.h>
#include <KoResourcePaths.h>
#include "opengl/kis_opengl.h"
#include "kis_painting_assistants_decoration_test.h"

void KisPaintingAssistantsDecorationTest::initTestCase()
{
    m_document = createEmptyDocument();
    m_mainWindow = KisPart::instance()->createMainWindow();
    m_view = new KisView(m_document, m_mainWindow->viewManager(), m_mainWindow);
    m_viewManager = new KisViewManager(m_mainWindow, m_mainWindow->actionCollection());
    QApplication::processEvents();
}

void KisPaintingAssistantsDecorationTest::cleanupTestCase()
{
    m_mainWindow->hide();
    QApplication::processEvents();
    delete m_view;
    delete m_document;
    delete m_mainWindow;
}

void KisPaintingAssistantsDecorationTest::testPerspectiveAssistant()
{
    KisPaintingAssistantsDecorationSP paintingAssistantsDecoration =
        m_view->canvasBase()->paintingAssistantsDecoration();

    const KisPaintingAssistantFactoryRegistry* r = KisPaintingAssistantFactoryRegistry::instance();
    QVERIFY(r);

    KisPaintingAssistantFactory* factory = r->get("perspective");
    QVERIFY(factory);

    KisPaintingAssistantSP assistant = toQShared(factory->createPaintingAssistant());

    QVERIFY(assistant->isAssistantComplete() == false);

    KisPaintingAssistantHandleSP h1 = new KisPaintingAssistantHandle(10.0, 10.0);
    KisPaintingAssistantHandleSP h2 = new KisPaintingAssistantHandle(20.0, 90.0);
    KisPaintingAssistantHandleSP h3 = new KisPaintingAssistantHandle(60.0, 90.0);
    KisPaintingAssistantHandleSP h4 = new KisPaintingAssistantHandle(70.0, 10.0);

    assistant->addHandle(h1, NORMAL);
    assistant->addHandle(h2, NORMAL);
    assistant->addHandle(h3, NORMAL);
    assistant->addHandle(h4, NORMAL);

    QVERIFY(assistant->isAssistantComplete() == true);

    QPointF startPosition(14.0, 14.0);
    QPointF p2(14.0, 90.0);
    QPointF adjustedPosition;
    adjustedPosition = assistant->adjustPosition(p2, startPosition, true, 4.0);
    adjustedPosition = assistant->adjustPosition(p2, startPosition, true, 4.0);
    QVERIFY(adjustedPosition != p2);
}

void KisPaintingAssistantsDecorationTest::testParallelRulerAdjustPosition()
{
    KisPaintingAssistantsDecorationSP paintingAssistantsDecoration =
        m_view->canvasBase()->paintingAssistantsDecoration();

    QVERIFY(paintingAssistantsDecoration->isEditingAssistants() == false);

    const KisPaintingAssistantFactoryRegistry* r = KisPaintingAssistantFactoryRegistry::instance();
    QVERIFY(r);

    KisPaintingAssistantFactory* factory = r->get("parallel ruler");
    QVERIFY(factory);

    KisPaintingAssistantSP assistant = toQShared(factory->createPaintingAssistant());

    QVERIFY(assistant->isAssistantComplete() == false);

    KisPaintingAssistantHandleSP h1 = new KisPaintingAssistantHandle(10.0, 10.0);
    KisPaintingAssistantHandleSP h2 = new KisPaintingAssistantHandle(90.0, 10.0);
    assistant->addHandle(h1, NORMAL);
    assistant->addHandle(h2, NORMAL);
    QVERIFY(assistant->numHandles() == 2);
    QVERIFY(assistant->isAssistantComplete() == true);

    QPointF startPosition(10.0, 50.0);
    QPointF p2(90.0, 90.0);
    QPointF adjustedPosition;

    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p2, startPosition);
    QVERIFY(adjustedPosition == p2);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p2, startPosition);
    QVERIFY(adjustedPosition == p2);

    paintingAssistantsDecoration->addAssistant(assistant);

    adjustedPosition = assistant->adjustPosition(p2, startPosition, true, 4.0);
    QVERIFY(adjustedPosition.x() == 90.0);
    QVERIFY(adjustedPosition.y() == 50.0);
    QPointF adjustedPosition2 = paintingAssistantsDecoration->adjustPosition(p2, startPosition);
    QVERIFY(adjustedPosition == adjustedPosition2);

    KisPaintingAssistantSP assistant_2 = toQShared(factory->createPaintingAssistant());
    KisPaintingAssistantHandleSP h3 = new KisPaintingAssistantHandle(10.0, 10.0);
    KisPaintingAssistantHandleSP h4 = new KisPaintingAssistantHandle(10.0, 90.0);
    assistant_2->addHandle(h3, NORMAL);
    assistant_2->addHandle(h4, NORMAL);

    KisPaintingAssistantSP assistant_3 = toQShared(factory->createPaintingAssistant());
    KisPaintingAssistantHandleSP h5 = new KisPaintingAssistantHandle(10.0, 10.0);
    KisPaintingAssistantHandleSP h6 = new KisPaintingAssistantHandle(90.0, 90.0);
    assistant_3->addHandle(h5, NORMAL);
    assistant_3->addHandle(h6, NORMAL);

    paintingAssistantsDecoration->addAssistant(assistant_2);
    paintingAssistantsDecoration->addAssistant(assistant_3);

    QPointF p3(11.0, 50.1);
    QPointF p4(15.0, 50.1);
    QPointF p5(18.0, 51.1);
    QPointF p6(11.0, 65.1);

    paintingAssistantsDecoration->setOnlyOneAssistantSnap(false);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p3, startPosition);

    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p4, startPosition);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p5, startPosition);
    QVERIFY(adjustedPosition.y() == 50.0);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p6, startPosition);
    QVERIFY(adjustedPosition.x() == 10.0);
    paintingAssistantsDecoration->endStroke();

    paintingAssistantsDecoration->setOnlyOneAssistantSnap(true);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p3, startPosition);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p4, startPosition);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p5, startPosition);
    QVERIFY(adjustedPosition.y() == 50.0);
    adjustedPosition = paintingAssistantsDecoration->adjustPosition(p6, startPosition);
    QVERIFY(adjustedPosition.y() == 50.0);
    paintingAssistantsDecoration->endStroke();

}

KISTEST_MAIN(KisPaintingAssistantsDecorationTest)

