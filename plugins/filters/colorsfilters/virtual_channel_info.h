/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __VIRTUAL_CHANNEL_INFO_H
#define __VIRTUAL_CHANNEL_INFO_H

#include <KoChannelInfo.h>
class KoColorSpace;

class VirtualChannelInfo
{
public:
    enum Type {
        REAL,
        HUE,
        SATURATION,
        LIGHTNESS,
        ALL_COLORS
    };

    VirtualChannelInfo();

    VirtualChannelInfo(Type type, int pixelIndex, KoChannelInfo *realChannelInfo, const KoColorSpace *cs);

    KoChannelInfo* channelInfo() const;

    int pixelIndex() const;

    Type type() const;
    QString name() const;

    KoChannelInfo::enumChannelValueType valueType() const;
    int channelSize() const;

    bool isAlpha() const;

private:
    Type m_type {REAL};
    int m_pixelIndex {0};
    KoChannelInfo *m_realChannelInfo {nullptr};

    QString m_nameOverride;
    KoChannelInfo::enumChannelValueType m_valueTypeOverride {KoChannelInfo::FLOAT32};
    int m_channelSizeOverride {4};
};

#endif
