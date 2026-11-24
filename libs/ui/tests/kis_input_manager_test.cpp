/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_input_manager_test.h"

#include <simpletest.h>

#include <QMouseEvent>

#include "input/kis_single_action_shortcut.h"
#include "input/kis_stroke_shortcut.h"
#include "input/kis_abstract_input_action.h"
#include "input/kis_shortcut_matcher.h"

void KisInputManagerTest::testSingleActionShortcut()
{
    KisSingleActionShortcut s(0,0);
    s.setKey(QSet<Qt::Key>() << Qt::Key_Shift, Qt::Key_Space);

    QVERIFY(s.match(QSet<Qt::Key>() << Qt::Key_Shift, Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Control, Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>(), Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Shift, Qt::Key_Escape));
    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Shift, KisSingleActionShortcut::WheelUp));

    s.setWheel(QSet<Qt::Key>() << Qt::Key_Shift, KisSingleActionShortcut::WheelUp);

    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Shift, Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Control, Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>(), Qt::Key_Space));
    QVERIFY(!s.match(QSet<Qt::Key>() << Qt::Key_Shift, Qt::Key_Escape));
    QVERIFY(s.match(QSet<Qt::Key>() << Qt::Key_Shift, KisSingleActionShortcut::WheelUp));
}

void KisInputManagerTest::testStrokeShortcut()
{
    KisStrokeShortcut s(0,0);
    s.setButtons(QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                 QSet<Qt::MouseButton>() << Qt::LeftButton);

    QVERIFY(s.matchReady(QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                         QSet<Qt::MouseButton>() << Qt::LeftButton));

    QVERIFY(s.matchReady(QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                         QSet<Qt::MouseButton>()));

    QVERIFY(!s.matchReady(QSet<Qt::Key>() << Qt::Key_Control << Qt::Key_Alt,
                         QSet<Qt::MouseButton>()));

    QVERIFY(!s.matchReady(QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                         QSet<Qt::MouseButton>() << Qt::RightButton));

    QVERIFY(s.matchBegin(Qt::LeftButton));
    QVERIFY(!s.matchBegin(Qt::RightButton));
}

struct TestingAction : public KisAbstractInputAction
{
    TestingAction() : KisAbstractInputAction("TestingAction"), m_isHighResolution(false) { reset(); }
    ~TestingAction() {}

    void begin(int shortcut, QEvent *event) override { m_beginIndex = shortcut; m_beginNonNull = event;}
    void end(QEvent *event) override { m_ended = true; m_endNonNull = event; }
    void inputEvent(QEvent* event) override { Q_UNUSED(event); m_gotInput = true; }

    void reset() {
        m_beginIndex = -1;
        m_ended = false;
        m_gotInput = false;
        m_beginNonNull = false;
        m_endNonNull = false;
    }

    bool supportsHiResInputEvents(int ) const override {
        return m_isHighResolution;
    }

    void setHighResInputEvents(bool value) {
        m_isHighResolution = value;
    }

    int m_beginIndex;
    bool m_ended;
    bool m_gotInput;
    bool m_beginNonNull;
    bool m_endNonNull;

    bool m_isHighResolution;
};

KisSingleActionShortcut* createKeyShortcut(KisAbstractInputAction *action,
                                  int shortcutIndex,
                                  const QSet<Qt::Key> &modifiers,
                                  Qt::Key key)
{
    KisSingleActionShortcut *s = new KisSingleActionShortcut(action, shortcutIndex);
    s->setKey(modifiers, key);
    return s;
}

KisStrokeShortcut* createStrokeShortcut(KisAbstractInputAction *action,
                                     int shortcutIndex,
                                     const QSet<Qt::Key> &modifiers,
                                     Qt::MouseButton button)
{
    KisStrokeShortcut *s = new KisStrokeShortcut(action, shortcutIndex);
    s->setButtons(modifiers, QSet<Qt::MouseButton>() << button);
    return s;
}

void KisInputManagerTest::testKeyEvents()
{
    KisShortcutMatcher m;
    m.enterEvent();

    TestingAction *a = new TestingAction();

    m.addShortcut(
        createKeyShortcut(a, 10,
                          QSet<Qt::Key>() << Qt::Key_Shift,
                          Qt::Key_Enter));

    m.addShortcut(
        createKeyShortcut(a, 11,
                          QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                          Qt::Key_Enter));

    m.addShortcut(
        createStrokeShortcut(a, 12,
                             QSet<Qt::Key>() << Qt::Key_Shift,
                             Qt::RightButton));

    m.addShortcut(
        createStrokeShortcut(a, 13,
                             QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                             Qt::LeftButton));

    QCOMPARE(a->m_beginIndex, -1);

    QMouseEvent mouseEvent(QEvent::MouseMove, QPoint(),
                           Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QVERIFY(!m.keyPressed(Qt::Key_Shift));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.keyPressed(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(m.keyPressed(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, 11);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    QVERIFY(!m.buttonPressed(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);
    QVERIFY(!m.buttonReleased(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.keyReleased(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(m.buttonPressed(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, 13);
    QCOMPARE(a->m_ended, false);
    QCOMPARE(a->m_beginNonNull, true);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    QVERIFY(m.buttonReleased(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, true);
    a->reset();

    QVERIFY(!m.buttonPressed(Qt::RightButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.buttonReleased(Qt::RightButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(m.keyPressed(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, 11);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    QVERIFY(m.autoRepeatedKeyPressed(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, 11);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    QVERIFY(!m.keyReleased(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.keyReleased(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.buttonPressed(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.buttonReleased(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(m.buttonPressed(Qt::RightButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, 12);
    QCOMPARE(a->m_ended, false);
    QCOMPARE(a->m_beginNonNull, true);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    QVERIFY(m.buttonReleased(Qt::RightButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, true);
    a->reset();

    QVERIFY(m.keyPressed(Qt::Key_Enter));
    QCOMPARE(a->m_beginIndex, 10);
    QCOMPARE(a->m_ended, true);
    QCOMPARE(a->m_beginNonNull, false);
    QCOMPARE(a->m_endNonNull, false);
    a->reset();

    m.leaveEvent();
}

void KisInputManagerTest::testReleaseUnnecessaryModifiers()
{
    KisShortcutMatcher m;
    m.enterEvent();

    TestingAction *a = new TestingAction();

    m.addShortcut(
        createStrokeShortcut(a, 13,
                             QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                             Qt::LeftButton));

    QMouseEvent mouseEvent(QEvent::MouseMove, QPoint(),
                           Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QVERIFY(!m.keyPressed(Qt::Key_Shift));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.keyPressed(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(m.buttonPressed(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, 13);
    QCOMPARE(a->m_ended, false);
    a->reset();

    QVERIFY(!m.keyReleased(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, false);

    QVERIFY(!m.keyReleased(Qt::Key_Shift));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, false);

    QVERIFY(m.buttonReleased(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, true);
    a->reset();

    m.leaveEvent();
}

void KisInputManagerTest::testMouseMoves()
{
    KisShortcutMatcher m;
    m.enterEvent();

    TestingAction *a = new TestingAction();

    m.addShortcut(
        createStrokeShortcut(a, 13,
                             QSet<Qt::Key>() << Qt::Key_Shift << Qt::Key_Control,
                             Qt::LeftButton));

    QMouseEvent mouseEvent(QEvent::MouseMove, QPoint(),
                           Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QVERIFY(!m.keyPressed(Qt::Key_Shift));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.keyPressed(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);

    QVERIFY(!m.pointerMoved(&mouseEvent));
    QCOMPARE(a->m_gotInput, false);

    QVERIFY(m.buttonPressed(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, 13);
    QCOMPARE(a->m_ended, false);
    QCOMPARE(a->m_gotInput, false);
    a->reset();

    QVERIFY(m.pointerMoved(&mouseEvent));
    QCOMPARE(a->m_gotInput, true);
    a->reset();

    QVERIFY(!m.keyReleased(Qt::Key_Control));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, false);
    QCOMPARE(a->m_gotInput, false);

    QVERIFY(!m.keyReleased(Qt::Key_Shift));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, false);

    QVERIFY(m.buttonReleased(Qt::LeftButton, &mouseEvent));
    QCOMPARE(a->m_beginIndex, -1);
    QCOMPARE(a->m_ended, true);
    a->reset();

    m.leaveEvent();
}

#include "../input/wintab/kis_incremental_average.h"

void KisInputManagerTest::testIncrementalAverage()
{
    KisIncrementalAverage avg(3);

    QCOMPARE(avg.pushThrough(10), 10);
    QCOMPARE(avg.pushThrough(20), 13);
    QCOMPARE(avg.pushThrough(30), 20);
    QCOMPARE(avg.pushThrough(30), 26);
    QCOMPARE(avg.pushThrough(30), 30);

}

SIMPLE_TEST_MAIN(KisInputManagerTest)
