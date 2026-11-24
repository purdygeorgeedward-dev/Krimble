/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisAdaptedLockTest.h"

#include "simpletest.h"

#include "KisAdaptedLock.h"

class TestAdapter
{
public:
    struct State {
        int lockCount = 0;
        int unlockCount = 0;
        bool isLocked = false;
    };

    TestAdapter(State* state) : m_state(state) {}

    void lock() {
        m_state->lockCount++;
        m_state->isLocked = true;
    }

    void unlock() {
        m_state->unlockCount++;
        m_state->isLocked = false;
    }

    bool try_lock() {
        m_state->lockCount++;
        m_state->isLocked = true;
        return true;
    }

private:
    State* m_state;
};

KIS_DECLARE_ADAPTED_LOCK(TestAdaptedLock, TestAdapter)

void KisAdaptedLockTest::testConstructorDefault()
{
    TestAdapter::State state;
    {
        TestAdaptedLock lock(&state);
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(state.isLocked, true);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testConstructorTryLock()
{
    TestAdapter::State state;
    {
        TestAdaptedLock lock(&state, std::try_to_lock);
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(state.isLocked, true);
        QCOMPARE(lock.owns_lock(), true);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testConstructorDeferLock()
{
    TestAdapter::State state;
    {
        TestAdaptedLock lock(&state, std::defer_lock);
        QCOMPARE(state.lockCount, 0);
        QCOMPARE(state.isLocked, false);
        QCOMPARE(lock.owns_lock(), false);

        lock.lock();
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(state.isLocked, true);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testConstructorAdoptLock()
{
    TestAdapter::State state;
    {
        TestAdapter adapter(&state);
        adapter.lock();
        QCOMPARE(state.lockCount, 1);

        TestAdaptedLock lock(&state, std::adopt_lock);
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(lock.owns_lock(), true);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testMoveConstructor()
{
    TestAdapter::State state;
    {
        TestAdaptedLock lock1(&state);
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(lock1.owns_lock(), true);

        TestAdaptedLock lock2(std::move(lock1));
        QCOMPARE(lock1.owns_lock(), false);
        QCOMPARE(lock2.owns_lock(), true);
        QCOMPARE(state.unlockCount, 0);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testInPlaceMoveConstructor()
{
    TestAdapter::State state;
    {
        TestAdaptedLock lock1(&state);
        QCOMPARE(state.lockCount, 1);
        QCOMPARE(lock1.owns_lock(), true);

        TestAdaptedLock lock2 = std::move(lock1);
        QCOMPARE(lock1.owns_lock(), false);
        QCOMPARE(lock2.owns_lock(), true);
        QCOMPARE(state.unlockCount, 0);
    }
    QCOMPARE(state.unlockCount, 1);
    QCOMPARE(state.isLocked, false);
}

void KisAdaptedLockTest::testMoveAssignmentOperator()
{
    TestAdapter::State state1;
    TestAdapter::State state2;
    {
        TestAdaptedLock lock1(&state1);
        QCOMPARE(state1.lockCount, 1);

        TestAdaptedLock lock2(&state2);
        QCOMPARE(state2.lockCount, 1);

        lock2 = std::move(lock1);

        QCOMPARE(lock2.owns_lock(), true);
        QCOMPARE(lock1.owns_lock(), false);

        QCOMPARE(state1.lockCount, 1);
        QCOMPARE(state1.unlockCount, 0);
        QCOMPARE(state1.isLocked, true);
        QCOMPARE(state2.lockCount, 1);
        QCOMPARE(state2.unlockCount, 1);
        QCOMPARE(state2.isLocked, false);
    }
    QCOMPARE(state1.unlockCount, 1);
    QCOMPARE(state1.isLocked, false);
}

SIMPLE_TEST_MAIN(KisAdaptedLockTest);
