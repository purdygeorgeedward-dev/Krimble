/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_CHANNEL_H
#define LIBKIS_CHANNEL_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"

#include <KoChannelInfo.h>
#include <kis_node.h>

class KRITALIBKIS_EXPORT Channel : public QObject
{
    Q_OBJECT

public:
    explicit Channel(KisNodeSP node, KoChannelInfo *channel, QObject *parent = 0);
    ~Channel() override;

    bool operator==(const Channel &other) const;
    bool operator!=(const Channel &other) const;

    bool visible() const;

    void setVisible(bool value);

    QString name() const;

    int position() const;

    int channelSize() const;

    QRect bounds() const;

    QByteArray pixelData(const QRect &rect) const;

    void setPixelData(QByteArray value, const QRect &rect);

private:

    struct Private;
    Private *const d;

};

#endif
