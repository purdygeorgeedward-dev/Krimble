/*
 *  SPDX-FileCopyrightText: 2015 Jouni Pentikäinen <joupent@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_SCALAR_KEYFRAME_CHANNEL_H
#define _KIS_SCALAR_KEYFRAME_CHANNEL_H

#include "kis_keyframe_channel.h"

struct ScalarKeyframeLimits {
    qreal lower;
    qreal upper;

    ScalarKeyframeLimits(qreal x, qreal y){
        KIS_ASSERT(x != y);

        lower = x < y ? x : y;
        upper = x > y ? x : y;
    }

    qreal clamp(qreal value){
        value = value < lower ? lower : value;
        value = value > upper ? upper : value;
        return value;
    }
};

class KRITAIMAGE_EXPORT KisScalarKeyframe : public KisKeyframe
{
    Q_OBJECT
public:
    enum InterpolationMode {
        Constant,
        Linear,
        Bezier
    };

    enum TangentsMode {
        Sharp,
        Smooth
    };

    KisScalarKeyframe(qreal value, QSharedPointer<ScalarKeyframeLimits> limits);
    KisScalarKeyframe(qreal value, InterpolationMode interpMode, TangentsMode tangentMode, QPointF leftTangent, QPointF rightTangent, QSharedPointer<ScalarKeyframeLimits> limits);

    KisKeyframeSP duplicate(KisKeyframeChannel* newChannel = 0) override;

    void setValue(qreal val, KUndo2Command* parentUndoCmd = nullptr);
    qreal value() const;

    void setInterpolationMode(InterpolationMode mode, KUndo2Command* parentUndoCmd = nullptr);
    InterpolationMode interpolationMode() const;

    void setTangentsMode(TangentsMode mode, KUndo2Command* parentUndoCmd = nullptr);
    TangentsMode tangentsMode() const;

    void setInterpolationTangents(QPointF leftTangent, QPointF rightTangent, KUndo2Command* parentUndoCmd = nullptr);
    QPointF leftTangent() const;
    QPointF rightTangent() const;

    void setLimits(QSharedPointer<ScalarKeyframeLimits> limits);

    QMetaObject::Connection valueChangedChannelConnection;

    friend class KisScalarKeyframeUpdateCommand;

Q_SIGNALS:
    void sigChanged(const KisScalarKeyframe* scalarKey);

private:
    qreal m_value;
    InterpolationMode m_interpolationMode;
    TangentsMode m_tangentsMode;
    QPointF m_leftTangent;
    QPointF m_rightTangent;

    QWeakPointer<ScalarKeyframeLimits> m_channelLimits;
};

class KRITAIMAGE_EXPORT KisScalarKeyframeChannel : public KisKeyframeChannel
{
    Q_OBJECT
public:
    KisScalarKeyframeChannel(const KoID& id, KisDefaultBoundsBaseSP bounds);
    KisScalarKeyframeChannel(const KisScalarKeyframeChannel &rhs);
    ~KisScalarKeyframeChannel() override;

    void addScalarKeyframe(int time, qreal value, KUndo2Command *parentUndoCmd = nullptr);

    QSharedPointer<ScalarKeyframeLimits> limits() const;
    void setLimits(qreal low, qreal high);
    void removeLimits();

    qreal valueAt(int time) const;
    qreal currentValue() { return valueAt(currentTime()); }

    bool isCurrentTimeAffectedBy(int keyTime);

    void setDefaultValue(qreal value);
    void setDefaultInterpolationMode(KisScalarKeyframe::InterpolationMode mode);

    static QPointF interpolate(QPointF point1, QPointF rightTangent, QPointF leftTangent, QPointF point2, qreal t);

    virtual void insertKeyframe(int time, KisKeyframeSP keyframe, KUndo2Command *parentUndoCmd = nullptr) override;
    virtual void removeKeyframe(int time, KUndo2Command *parentUndoCmd = nullptr) override;
    virtual KisTimeSpan affectedFrames(int time) const override;
    virtual KisTimeSpan identicalFrames(int time) const override;

private:
    static qreal findCubicCurveParameter(int time0, qreal delta0, qreal delta1, int time1, int time);
    static qreal cubicBezier(qreal p0, qreal delta1, qreal delta2, qreal p3, qreal t);
    static void normalizeTangents(const QPointF point1, QPointF &rightTangent, QPointF &leftTangent, const QPointF point2);

    virtual KisKeyframeSP createKeyframe() override;
    virtual QRect affectedRect(int time) const override;
    virtual QPair<int, KisKeyframeSP> loadKeyframe(const QDomElement &keyframeNode) override;
    virtual void saveKeyframe(KisKeyframeSP keyframe, QDomElement keyframeElement, const QString &layerFilename) override;

    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
