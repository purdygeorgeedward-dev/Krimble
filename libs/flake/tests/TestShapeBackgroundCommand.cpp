/* This file is part of the KDE project
* SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "TestShapeBackgroundCommand.h"

#include <MockShapes.h>
#include "KoShapeBackgroundCommand.h"
#include "KoColorBackground.h"

#include <QPainterPath>
#include <simpletest.h>

void TestShapeBackgroundCommand::refCounting()
{
    MockShape * shape1 = new MockShape();
    QSharedPointer<KoShapeBackground> whiteFill(new KoColorBackground(QColor(Qt::white)));
    QSharedPointer<KoShapeBackground> blackFill(new KoColorBackground(QColor(Qt::black)));
    QSharedPointer<KoShapeBackground> redFill  (new KoColorBackground(QColor(Qt::red)));

    shape1->setBackground(whiteFill);
    QVERIFY(shape1->background() == whiteFill);

    KUndo2Command *cmd1 = new KoShapeBackgroundCommand(shape1, blackFill);
    cmd1->redo();
    QVERIFY(shape1->background() == blackFill);

    cmd1->undo();
    QVERIFY(shape1->background() == whiteFill);

    KUndo2Command *cmd2 = new KoShapeBackgroundCommand(shape1, redFill);
    cmd2->redo();
    QVERIFY(shape1->background() == redFill);

    delete cmd1;

    cmd2->undo();
    QVERIFY(shape1->background() == whiteFill);

    QPainter p;
    QPainterPath path;
    path.addRect( QRectF(0,0,100,100) );

    whiteFill->paint( p, path );

    delete cmd2;
    delete shape1;
}

SIMPLE_TEST_MAIN(TestShapeBackgroundCommand)
