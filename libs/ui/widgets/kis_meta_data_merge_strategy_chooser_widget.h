/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempot <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_META_DATA_MERGE_STRATEGY_CHOOSER_WIDGET_H_
#define _KIS_META_DATA_MERGE_STRATEGY_CHOOSER_WIDGET_H_

#include <QWidget>

namespace KisMetaData
{
class MergeStrategy;
}

class KisMetaDataMergeStrategyChooserWidget : public QWidget
{
    Q_OBJECT
public:
    KisMetaDataMergeStrategyChooserWidget(QWidget* parent);
    ~KisMetaDataMergeStrategyChooserWidget() override;

    const KisMetaData::MergeStrategy* currentStrategy();

    static const KisMetaData::MergeStrategy* showDialog(QWidget* parent);

private Q_SLOTS:

    void setCurrentDescription(int index);

private:

    const KisMetaData::MergeStrategy* mergeStrategy(int index);

    struct Private;
    Private* const d;
};

#endif
