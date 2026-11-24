/*
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MOCKMULTIFEEDRSSMODEL_H
#define MOCKMULTIFEEDRSSMODEL_H

#include <QObject>

#include <KisMultiFeedRSSModel.h>

class MockMultiFeedRssModel : public MultiFeedRssModel
{
    Q_OBJECT

public:
    explicit MockMultiFeedRssModel(QObject *parent = 0);

    void addFeed(const QString& feed) override;

    void loadFeedData(const RssItemList& feed);
};

#endif
