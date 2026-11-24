/*
    SPDX-FileCopyrightText: 2000 Matthias Elter <elter@kde.org>
    SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
    SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
    SPDX-FileCopyrightText: 2004, 2007 Sven Langkamp <sven.langkamp@gmail.com>
    SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOSEGMENTGRADIENT_H
#define KOSEGMENTGRADIENT_H

#include <QList>
#include <QColor>

#include <KoResource.h>
#include <resources/KoAbstractGradient.h>
#include "KoColor.h"

#include <kritapigment_export.h>

enum {
    INTERP_LINEAR = 0,
    INTERP_CURVED,
    INTERP_SINE,
    INTERP_SPHERE_INCREASING,
    INTERP_SPHERE_DECREASING
};

enum {
    COLOR_INTERP_RGB,
    COLOR_INTERP_HSV_CCW,
    COLOR_INTERP_HSV_CW
};

enum KoGradientSegmentEndpointType {
    COLOR_ENDPOINT,
    FOREGROUND_ENDPOINT,
    FOREGROUND_TRANSPARENT_ENDPOINT,
    BACKGROUND_ENDPOINT,
    BACKGROUND_TRANSPARENT_ENDPOINT
};

struct KoGradientSegmentEndpoint {
    KoGradientSegmentEndpoint(qreal _off, KoColor _color, KoGradientSegmentEndpointType _type) :
        offset(_off), color(_color), type(_type)
    {

    }

    qreal offset;
    KoColor color;
    KoGradientSegmentEndpointType type;

};

class KRITAPIGMENT_EXPORT KoGradientSegment
{
public:
    KoGradientSegment(int interpolationType, int colorInterpolationType, KoGradientSegmentEndpoint start, KoGradientSegmentEndpoint end, qreal middleOffset);

    void colorAt(KoColor&, qreal t) const;

    const KoColor& startColor() const;
    const KoColor& endColor() const;
    KoGradientSegmentEndpointType startType() const;
    KoGradientSegmentEndpointType endType() const;

    void setStartColor(const KoColor& color) {
        m_start.color = color;
        if (m_start.type == FOREGROUND_TRANSPARENT_ENDPOINT || m_start.type == BACKGROUND_TRANSPARENT_ENDPOINT) {
            m_start.color.setOpacity(quint8(0));
        } else if (m_start.type == FOREGROUND_ENDPOINT || m_start.type == BACKGROUND_ENDPOINT) {
            m_start.color.setOpacity(quint8(255));
        }
    }
    void setEndColor(const KoColor& color) {
        m_end.color = color;
        if (m_end.type == FOREGROUND_TRANSPARENT_ENDPOINT || m_end.type == BACKGROUND_TRANSPARENT_ENDPOINT) {
            m_end.color.setOpacity(quint8(0));
        } else if (m_end.type == FOREGROUND_ENDPOINT || m_end.type == BACKGROUND_ENDPOINT) {
            m_end.color.setOpacity(quint8(255));
        }
    }

    void setStartType(KoGradientSegmentEndpointType type);
    void setEndType(KoGradientSegmentEndpointType type);

    qreal startOffset() const;
    qreal middleOffset() const;
    qreal endOffset() const;

    void setStartOffset(qreal t);
    void setMiddleOffset(qreal t);
    void setEndOffset(qreal t);

    void setVariableColors(const KoColor& foreground, const KoColor& background);
    bool hasVariableColors();

    qreal length() {
        return m_length;
    }

    int interpolation() const;
    int colorInterpolation() const;

    void setInterpolation(int interpolationType);
    void setColorInterpolation(int colorInterpolationType);

    void mirrorSegment();

    bool isValid() const;

protected:

    class ColorInterpolationStrategy
    {
    public:
        ColorInterpolationStrategy() {}
        virtual ~ColorInterpolationStrategy() {}

        virtual void colorAt(KoColor& dst, qreal t, const KoColor& start, const KoColor& end) const = 0;
        virtual int type() const = 0;
    };

    class RGBColorInterpolationStrategy : public ColorInterpolationStrategy
    {
    public:
        static RGBColorInterpolationStrategy *instance();

        void colorAt(KoColor& dst, qreal t, const KoColor& start, const KoColor& end) const override;
        int type() const override {
            return COLOR_INTERP_RGB;
        }

    private:
        RGBColorInterpolationStrategy();

        static RGBColorInterpolationStrategy *m_instance;
        const KoColorSpace * const m_colorSpace;
    };

    class HSVCWColorInterpolationStrategy : public ColorInterpolationStrategy
    {
    public:
        static HSVCWColorInterpolationStrategy *instance();

        void colorAt(KoColor& dst, qreal t, const KoColor& start, const KoColor& end) const override;
        int type() const override {
            return COLOR_INTERP_HSV_CW;
        }
    private:
        HSVCWColorInterpolationStrategy();

        static HSVCWColorInterpolationStrategy *m_instance;
        const KoColorSpace * const m_colorSpace;
    };

    class HSVCCWColorInterpolationStrategy : public ColorInterpolationStrategy
    {
    public:
        static HSVCCWColorInterpolationStrategy *instance();

        void colorAt(KoColor& dst, qreal t, const KoColor& start, const KoColor& end) const override;
        int type() const override {
            return COLOR_INTERP_HSV_CCW;
        }
    private:
        HSVCCWColorInterpolationStrategy();

        static HSVCCWColorInterpolationStrategy *m_instance;
        const KoColorSpace * const m_colorSpace;
    };

    class InterpolationStrategy
    {
    public:
        InterpolationStrategy() {}
        virtual ~InterpolationStrategy() {}

        virtual qreal valueAt(qreal t, qreal middle) const = 0;
        virtual int type() const = 0;
    };

    class LinearInterpolationStrategy : public InterpolationStrategy
    {
    public:
        static LinearInterpolationStrategy *instance();

        qreal valueAt(qreal t, qreal middle) const override;
        int type() const override {
            return INTERP_LINEAR;
        }

        static qreal calcValueAt(qreal t, qreal middle);

    private:
        LinearInterpolationStrategy() {}

        static LinearInterpolationStrategy *m_instance;
    };

    class CurvedInterpolationStrategy : public InterpolationStrategy
    {
    public:
        static CurvedInterpolationStrategy *instance();

        qreal valueAt(qreal t, qreal middle) const override;
        int type() const override {
            return INTERP_CURVED;
        }
    private:
        CurvedInterpolationStrategy();

        static CurvedInterpolationStrategy *m_instance;
        qreal m_logHalf;
    };

    class SphereIncreasingInterpolationStrategy : public InterpolationStrategy
    {
    public:
        static SphereIncreasingInterpolationStrategy *instance();

        qreal valueAt(qreal t, qreal middle) const override;
        int type() const override {
            return INTERP_SPHERE_INCREASING;
        }
    private:
        SphereIncreasingInterpolationStrategy() {}

        static SphereIncreasingInterpolationStrategy *m_instance;
    };

    class SphereDecreasingInterpolationStrategy : public InterpolationStrategy
    {
    public:
        static SphereDecreasingInterpolationStrategy *instance();

        qreal valueAt(qreal t, qreal middle) const override;
        int type() const override {
            return INTERP_SPHERE_DECREASING;
        }
    private:
        SphereDecreasingInterpolationStrategy() {}

        static SphereDecreasingInterpolationStrategy *m_instance;
    };

    class SineInterpolationStrategy : public InterpolationStrategy
    {
    public:
        static SineInterpolationStrategy *instance();

        qreal valueAt(qreal t, qreal middle) const override;
        int type() const override {
            return INTERP_SINE;
        }
    private:
        SineInterpolationStrategy() {}

        static SineInterpolationStrategy *m_instance;
    };
private:
    InterpolationStrategy *m_interpolator;
    ColorInterpolationStrategy *m_colorInterpolator;

    qreal m_middleOffset;
    qreal m_length;
    qreal m_middleT;

    KoGradientSegmentEndpoint m_start, m_end;
    bool m_hasVariableColors = false;

};

class KRITAPIGMENT_EXPORT KoSegmentGradient : public KoAbstractGradient
{

public:
    explicit KoSegmentGradient(const QString &file = QString());
    ~KoSegmentGradient() override;
    KoSegmentGradient(const KoSegmentGradient &rhs);
    KoSegmentGradient &operator=(const KoSegmentGradient &rhs) = delete;
    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    QPair<QString, QString> resourceType() const override {
        return QPair<QString, QString>(ResourceType::Gradients, ResourceSubType::SegmentedGradients);
    }

    void colorAt(KoColor& dst, qreal t) const override;

    QList<int> requiredCanvasResources() const override;
    void bakeVariableColors(KoCanvasResourcesInterfaceSP canvasResourcesInterface) override;
    void updateVariableColors(KoCanvasResourcesInterfaceSP canvasResourcesInterface) override;

    KoGradientSegment *segmentAt(qreal t) const;

    QGradient* toQGradient() const override;

    QString defaultFileExtension() const override;

    void toXML(QDomDocument& doc, QDomElement& gradientElt) const;
    static KoSegmentGradient fromXML(const QDomElement& elt);

    void createSegment(int interpolation, int colorInterpolation, double startOffset, double endOffset, double middleOffset,
                       const QColor & leftColor, const QColor & rightColor,
                       KoGradientSegmentEndpointType leftType = COLOR_ENDPOINT, KoGradientSegmentEndpointType rightType = COLOR_ENDPOINT);

    void createSegment(int interpolation, int colorInterpolation, double startOffset, double endOffset, double middleOffset,
                       const KoColor & leftColor, const KoColor & rightColor,
                       KoGradientSegmentEndpointType leftType = COLOR_ENDPOINT, KoGradientSegmentEndpointType rightType = COLOR_ENDPOINT);

    const QList<double> getHandlePositions() const;

    const QList<double> getMiddleHandlePositions() const;

    void moveSegmentStartOffset(KoGradientSegment* segment, double t);

    void moveSegmentEndOffset(KoGradientSegment* segment, double t);

    void moveSegmentMiddleOffset(KoGradientSegment* segment, double t);

    void splitSegment(KoGradientSegment* segment);

    void duplicateSegment(KoGradientSegment* segment);

    void mirrorSegment(KoGradientSegment* segment);

    KoGradientSegment* removeSegment(KoGradientSegment* segment);

    KoGradientSegment* collapseSegment(KoGradientSegment* segment);

    bool removeSegmentPossible() const;

    const QList<KoGradientSegment *>& segments() const;
    void setSegments(const QList<KoGradientSegment*> &segments);

protected:

    inline void pushSegment(KoGradientSegment* segment) {
        m_segments.push_back(segment);
    }

    QList<KoGradientSegment *> m_segments;

private:
    bool init();
};

typedef QSharedPointer<KoSegmentGradient> KoSegmentGradientSP;

#endif

