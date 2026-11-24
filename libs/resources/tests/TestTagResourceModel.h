/*
 * SPDX-FileCopyrightText: 2020 boud <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTAGFILTERTRESOURCEPROXYMODEL_H
#define TESTAGFILTERTRESOURCEPROXYMODEL_H

#include <QObject>
#include <QString>
#include "KisResourceTypes.h"

class KisResourceLocator;

class TestTagResourceModel : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void initTestCase();
    void testWithTagModelTester();
    void testRowCount();
    void testData();
    void testTagResource();
    void testUntagResource();
    void testIsResourceTagged();

    void testFilterTagResource();

    void testBeginEndInsert();

    void cleanupTestCase();

private:

    QString m_srcLocation;
    QString m_dstLocation;

    KisResourceLocator *m_locator;
    const QString resourceType = ResourceType::PaintOpPresets;

};

class ModelSignalChecker : public QObject
{
    Q_OBJECT

public:

    enum TYPE
    {
        AboutInsert,
        Insert,
        AboutRemove,
        Remove
    };

    ModelSignalChecker(QList<int> _expectedFirsts, QList<int> _expectedLasts, QList<TYPE> _expectedTypes);

    void setInfo(QList<int> _expectedFirsts, QList<int> _expectedLasts, QList<TYPE> _expectedTypes);
    void reset();
    bool isCorrect();
    void printOut();

public Q_SLOTS:
    void rowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void rowsRemoved(const QModelIndex &parent, int first, int last);

private:
    void addSignalInfo(int first, int last, TYPE type);

public:
    QList<int> firsts;
    QList<int> lasts;
    QList<TYPE> types;
    int amount;

    QList<int> expectedFirsts;
    QList<int> expectedLasts;
    QList<TYPE> expectedTypes;
    int expectedAmount;

};

#endif
