/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_MODEL_INDEX_CONVERTER_BASE_H
#define __KIS_MODEL_INDEX_CONVERTER_BASE_H

#include <QModelIndex>
#include "kritaui_export.h"
class KisNodeDummy;

class KRITAUI_EXPORT KisModelIndexConverterBase
{
public:
    virtual ~KisModelIndexConverterBase();

    virtual KisNodeDummy* dummyFromRow(int row, QModelIndex parent) = 0;

    virtual KisNodeDummy* dummyFromIndex(QModelIndex index) = 0;

    virtual QModelIndex indexFromDummy(KisNodeDummy *dummy) = 0;

    virtual bool indexFromAddedDummy(KisNodeDummy *parentDummy, int index, const QString &newNodeMetaObjectType, QModelIndex &parentIndex, int &row) = 0;

    virtual int rowCount(QModelIndex parent) = 0;
};

#endif
