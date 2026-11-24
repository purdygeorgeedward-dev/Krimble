/* This file is part of the KDE project
* SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "TestShapeStrokeCommand.h"

#include <MockShapes.h>
#include "KoShapeStrokeModel.h"
#include "KoShapeStroke.h"
#include "KoShapeStrokeCommand.h"
#include <KoInsets.h>

#include <simpletest.h>

void TestShapeStrokeCommand::refCounting()
{
    MockShape * shape1 = new MockShape();
    KoShapeStrokeModelSP whiteStroke(new KoShapeStroke(1.0, QColor(Qt::white)));
    KoShapeStrokeModelSP blackStroke(new KoShapeStroke(1.0, QColor(Qt::black)));
    KoShapeStrokeModelSP redStroke(new KoShapeStroke(1.0, QColor(Qt::red)));

    shape1->setStroke(whiteStroke);
    QVERIFY(shape1->stroke() == whiteStroke);

    KUndo2Command *cmd1 = new KoShapeStrokeCommand(shape1, blackStroke);
    cmd1->redo();
    QVERIFY(shape1->stroke() == blackStroke);

    cmd1->undo();
    QVERIFY(shape1->stroke() == whiteStroke);

    KUndo2Command *cmd2 = new KoShapeStrokeCommand(shape1, redStroke);
    cmd2->redo();
    QVERIFY(shape1->stroke() == redStroke);

    delete cmd1;

    cmd2->undo();
    QVERIFY(shape1->stroke() == whiteStroke);

    KoInsets insets;
    whiteStroke->strokeInsets(shape1, insets);

    delete cmd2;
    delete shape1;
}

SIMPLE_TEST_MAIN(TestShapeStrokeCommand)
