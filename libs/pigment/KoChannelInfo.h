/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOCHANNELINFO_H_
#define KOCHANNELINFO_H_

#include <limits>

#include <QColor>
#include <QString>
#include <QList>

class KoChannelInfo
{
public:
    struct DoubleRange
    {
        public:
            double minVal, maxVal;
        public:
            DoubleRange(void) : minVal(0), maxVal(0) { }
            DoubleRange(qreal _minVal, qreal _maxVal) : minVal(_minVal), maxVal(_maxVal) { Q_ASSERT(minVal <= maxVal); }
            bool isValid(void) const { return minVal < maxVal; }
    };
public:
    enum enumChannelType {
        COLOR,
        ALPHA
    };
    enum enumChannelValueType {
        UINT8,
        UINT16,
        UINT32,
        FLOAT16,
        FLOAT32,
        FLOAT64,
        INT8,
        INT16,
        OTHER
    };

public:
    KoChannelInfo() { }
    KoChannelInfo(const QString & name,
                  qint32 npos,
                  qint32 displayPosition,
                  enumChannelType channelType,
                  enumChannelValueType channelValueType,
                  qint32 size = -1,
                  const QColor &color = QColor(0, 0, 0),
                  const DoubleRange &uiMinMax = DoubleRange())
        : m_name(name)
        , m_pos(npos)
        , m_displayPosition(displayPosition)
        , m_channelType(channelType)
        , m_channelValueType(channelValueType)
        , m_size(size)
        , m_color(color)
        , m_uiMinMax(uiMinMax)
    {
        switch(m_channelValueType)
        {
        case UINT8:
        case INT8:
            Q_ASSERT(m_size == -1 || m_size == 1);
            m_size = 1;
            break;
        case UINT16:
        case INT16:
            Q_ASSERT(m_size == -1 || m_size == 2);
            m_size = 2;
            break;
        case UINT32:
            Q_ASSERT(m_size == -1 || m_size == 4);
            m_size = 4;
            break;
        case FLOAT16:
            Q_ASSERT(m_size == -1 || m_size == 2);
            m_size = 2;
            break;
        case FLOAT32:
            Q_ASSERT(m_size == -1 || m_size == 4);
            m_size = 4;
            break;
        case FLOAT64:
            Q_ASSERT(m_size == -1 || m_size == 8);
            m_size = 8;
            break;
        case OTHER:
            Q_ASSERT(m_size != -1);
        }
        if (!uiMinMax.isValid()) {
            switch (m_channelValueType) {
                case UINT8:
                    m_uiMinMax.minVal = std::numeric_limits<quint8>::min();
                    m_uiMinMax.maxVal = std::numeric_limits<quint8>::max();
                    break;
                case INT8:
                    m_uiMinMax.minVal = std::numeric_limits<qint8>::min();
                    m_uiMinMax.maxVal = std::numeric_limits<qint8>::max();
                    break;
                case UINT16:
                    m_uiMinMax.minVal = std::numeric_limits<quint16>::min();
                    m_uiMinMax.maxVal = std::numeric_limits<quint16>::max();
                    break;
                case INT16:
                    m_uiMinMax.minVal = std::numeric_limits<qint16>::min();
                    m_uiMinMax.maxVal = std::numeric_limits<qint16>::max();
                    break;
                case UINT32:
                    m_uiMinMax.minVal = std::numeric_limits<quint32>::min();
                    m_uiMinMax.maxVal = std::numeric_limits<quint32>::max();
                    break;
                default:
                    m_uiMinMax.minVal = 0.0;
                    m_uiMinMax.maxVal = 1.0;
                    break;
            }
        }
        Q_ASSERT(m_uiMinMax.isValid());
    }
public:

    static int displayPositionToChannelIndex(int displayPosition, const QList<KoChannelInfo*> &channels)
    {
        for (int i = 0; i < channels.size(); ++i) {
            if (channels.at(i)->displayPosition() == displayPosition) {
                return i;
            }
        }
        return -1;
    }

    static QList<KoChannelInfo*> displayOrderSorted(const QList<KoChannelInfo*> &channels)
    {
        QList <KoChannelInfo*> sortedChannels;
        for (int i = 0; i < channels.size(); ++i) {
            Q_FOREACH (KoChannelInfo* channel, channels) {
                if (channel->displayPosition() == i) {
                    sortedChannels << channel;
                    break;
                }
            }
        }
        Q_ASSERT(channels.size() == sortedChannels.size());
        return sortedChannels;
    }

    inline QString name() const {
        return m_name;
    }

    inline qint32 pos() const {
        return m_pos;
    }

    inline qint32 displayPosition() const {
        return m_displayPosition;
    }

    inline qint32 size() const {
        return m_size;
    }

    inline enumChannelType channelType() const {
        return m_channelType;
    }
    inline enumChannelValueType channelValueType() const {
        return m_channelValueType;
    }
    inline QColor color() const {
        return m_color;
    }

    inline bool operator<(const KoChannelInfo & info) {
        return m_pos < info.m_pos;
    }

    inline double getUIMin(void) const {
        return m_uiMinMax.minVal;
    }

    inline double getUIMax(void) const {
        return m_uiMinMax.maxVal;
    }

    inline double getUIUnitValue(void) const {
        return m_uiMinMax.maxVal - m_uiMinMax.minVal;
    }

private:

    QString m_name;
    qint32 m_pos;
    qint32 m_displayPosition;
    enumChannelType m_channelType;
    enumChannelValueType m_channelValueType;
    qint32 m_size;
    QColor m_color;
    DoubleRange m_uiMinMax;

};

#endif
