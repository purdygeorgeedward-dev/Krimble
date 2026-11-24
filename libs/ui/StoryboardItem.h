/*
 *  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef STORYBOARD_ITEM
#define STORYBOARD_ITEM

#include <QVariant>
#include <QVector>
#include <QPixmap>

#include "kritaui_export.h"
#include "kis_types.h"

class StoryboardItem;
class QDomDocument;
class QDomElement;

class StoryboardComment
{
public:
    StoryboardComment() = default;

    QString name;
    bool visibility = true;
};

class CommentBox
{
public:
    CommentBox()
    : content("")
    , scrollValue(0)
    {}
    CommentBox(const CommentBox& other)
    : content(other.content)
    , scrollValue(other.scrollValue)
    {}
    ~CommentBox()
    {}

    QVariant content;
    QVariant scrollValue;
};

class ThumbnailData
{
public:
    ThumbnailData()
    : frameNum("")
    , pixmap(QPixmap())
    {}
    ThumbnailData(const ThumbnailData& other)
    : frameNum(other.frameNum)
    , pixmap(other.pixmap)
    {}
    ~ThumbnailData()
    {}

    QVariant frameNum;

    QVariant pixmap;
};

Q_DECLARE_METATYPE(CommentBox)
Q_DECLARE_METATYPE(ThumbnailData)

class StoryboardChild
{
public:
    StoryboardChild(QVariant data)
        : m_data(data)
    {}

    StoryboardChild(const StoryboardChild &rhs)
        : m_data(rhs.m_data)
    {}

    StoryboardItemSP parent()
    {
        return m_parentItem.toStrongRef();
    }

    void setParent(StoryboardItemSP parent)
    {
        m_parentItem = parent;
    }

    QVariant data()
    {
        return m_data;
    }
    void setData(QVariant value)
    {
        m_data = value;
    }

private:
    QVariant m_data;
    QWeakPointer<StoryboardItem> m_parentItem;
};

class KRITAUI_EXPORT StoryboardItem : public QEnableSharedFromThis<StoryboardItem>
{
public:
    explicit StoryboardItem();
    StoryboardItem(const StoryboardItem& other);
    ~StoryboardItem();

    void appendChild(QVariant data);
    void cloneChildrenFrom(const StoryboardItem &other);
    void insertChild(int row, QVariant data = QVariant());
    void removeChild(int row);
    void moveChild(int from, int to);
    int childCount() const;
    QSharedPointer<StoryboardChild> child(int row) const;

    QDomElement toXML(QDomDocument doc);
    void loadXML(const QDomElement &itemNode);

    static StoryboardItemList cloneStoryboardItemList(const StoryboardItemList &list);

    enum childType{

        FrameNumber,
        ItemName,
        DurationSecond,
        DurationFrame,
        Comments
    };

private:
    QVector<QSharedPointer<StoryboardChild>> m_childData;
};

#endif
