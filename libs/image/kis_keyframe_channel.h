/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Emmet O 'Neill <emmetoneill.pdx@gmail.com>
 *  SPDX-FileCopyrightText: 2020 Eoin O 'Neill <eoinoneill1991@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_KEYFRAME_CHANNEL_H
#define KIS_KEYFRAME_CHANNEL_H

#include <QVariant>
#include <QDomElement>
#include <kundo2command.h>

#include "kis_types.h"
#include "KoID.h"
#include "kis_keyframe.h"
#include "kis_default_bounds.h"
#include "kis_default_bounds_node_wrapper.h"

#include "kritaimage_export.h"

class KisTimeSpan;

class KRITAIMAGE_EXPORT KisKeyframeChannel : public QObject
{
    Q_OBJECT

public:
    static const KoID Raster;
    static const KoID Opacity;
    static const KoID TransformArguments;
    static const KoID PositionX;
    static const KoID PositionY;
    static const KoID ScaleX;
    static const KoID ScaleY;
    static const KoID ShearX;
    static const KoID ShearY;
    static const KoID RotationX;
    static const KoID RotationY;
    static const KoID RotationZ;

    KisKeyframeChannel(const KoID &id, KisDefaultBoundsBaseSP bounds);
    KisKeyframeChannel(const KisKeyframeChannel &rhs);
    ~KisKeyframeChannel() override;

    void addKeyframe(int time, KUndo2Command *parentUndoCmd = nullptr);

    virtual void insertKeyframe(int time, KisKeyframeSP keyframe, KUndo2Command *parentUndoCmd = nullptr);

    virtual void removeKeyframe(int time, KUndo2Command *parentUndoCmd = nullptr);

    static void moveKeyframe(KisKeyframeChannel *sourceChannel, int sourceTime, KisKeyframeChannel *targetChannel, int targetTime, KUndo2Command* parentUndoCmd = nullptr);

    static void copyKeyframe(const KisKeyframeChannel *sourceChannel, int sourceTime, KisKeyframeChannel *targetChannel, int targetTime, KUndo2Command* parentUndoCmd = nullptr);

    static void swapKeyframes(KisKeyframeChannel *channelA, int timeA, KisKeyframeChannel *channelB, int timeB, KUndo2Command* parentUndoCmd = nullptr);

    void moveKeyframe(int sourceTime, int targetTime, KUndo2Command* parentUndoCmd = nullptr) { moveKeyframe(this, sourceTime, this, targetTime, parentUndoCmd); }
    void copyKeyframe(int sourceTime, int targetTime, KUndo2Command* parentUndoCmd = nullptr) { copyKeyframe(this, sourceTime, this, targetTime, parentUndoCmd); }
    void swapKeyframes(int timeA, int timeB, KUndo2Command* parentUndoCmd = nullptr) { swapKeyframes(this, timeA, this, timeB, parentUndoCmd); }

    KisKeyframeSP keyframeAt(int time) const;
    KisKeyframeSP activeKeyframeAt(int time) const { return keyframeAt(activeKeyframeTime(time)); }

    template <class KeyframeType>
    QSharedPointer<KeyframeType> keyframeAt(int time) const {
        return keyframeAt(time).dynamicCast<KeyframeType>();
    }

    template <class KeyframeType>
    QSharedPointer<KeyframeType> activeKeyframeAt(int time) const {
        return activeKeyframeAt(time).dynamicCast<KeyframeType>();
    }

    int keyframeCount() const;

    int activeKeyframeTime(int time) const;
    int activeKeyframeTime() const { return activeKeyframeTime(currentTime()); }

    int lookupKeyframeTime(KisKeyframeSP toLookup);

    int firstKeyframeTime() const;
    int previousKeyframeTime(const int time) const;
    int nextKeyframeTime(const int time) const;
    int lastKeyframeTime() const;

    QSet<int> allKeyframeTimes() const;

    QString id() const;
    QString name() const;

    void setNode(KisNodeWSP node);
    KisNodeWSP node() const;
    void setDefaultBounds(KisDefaultBoundsBaseSP bounds);

    int channelHash() const;

    virtual KisTimeSpan affectedFrames(int time) const;

    virtual KisTimeSpan identicalFrames(int time) const;

    virtual QRect affectedRect(int time) const = 0;

    virtual QDomElement toXML(QDomDocument doc, const QString &layerFilename);
    virtual void loadXML(const QDomElement &channelNode);

    static KoID channelIdToKoId(const QString &id);

Q_SIGNALS:
    void sigAddedKeyframe(const KisKeyframeChannel *channel, int time);

    void sigKeyframeAboutToBeRemoved(const KisKeyframeChannel *channel, int time);

    void sigKeyframeHasBeenRemoved(const KisKeyframeChannel *channel, int time);

    void sigKeyframeChanged(const KisKeyframeChannel *channel, int time);

    void sigAnyKeyframeChange();

protected:
    typedef QMap<int, KisKeyframeSP> TimeKeyframeMap;
    TimeKeyframeMap &keys();
    const TimeKeyframeMap &constKeys() const;
    int currentTime() const;

    virtual void removeKeyframeImpl(int time, KUndo2Command *parentUndoCmd);

    Q_DECL_DEPRECATED void workaroundBrokenFrameTimeBug(int *time);

private:
    struct Private;
    QScopedPointer<Private> m_d;

    virtual KisKeyframeSP createKeyframe() = 0;
    virtual QPair<int, KisKeyframeSP> loadKeyframe(const QDomElement &keyframeNode) = 0;
    virtual void saveKeyframe(KisKeyframeSP keyframe, QDomElement keyframeElement, const QString &layerFilename) = 0;
};

#endif
