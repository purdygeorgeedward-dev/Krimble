/*
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_COLOR_SELECTOR_COMPONENT_H
#define KIS_COLOR_SELECTOR_COMPONENT_H

#include <QObject>
#include <QColor>

#include <resources/KoGamutMask.h>

#include "kis_color_selector.h"

class KoColorSpace;

class QPainter;

class KisColorSelectorComponent : public QObject
{
    Q_OBJECT
public:
    typedef KisColorSelectorConfiguration::Parameters Parameter;
    typedef KisColorSelectorConfiguration::Type Type;

    explicit KisColorSelectorComponent(KisColorSelector* parent);
    void setGeometry(int x, int y, int width, int height);
    void paintEvent(QPainter*);

    virtual void mouseEvent(int x, int y);

    KoColor currentColor();

    int width() const;
    int height() const;

    void setConfiguration(Parameter param, Type type);

    virtual void setColor(const KoColor& color);

    void setDirty();

    bool wantsGrab(int x, int y) {return containsPointInComponentCoords(x-m_x, y-m_y);}

    void setGamutMask(KoGamutMaskSP gamutMask);
    void unsetGamutMask();
    void updateGamutMaskPreview();
    void toggleGamutMask(bool state);

public Q_SLOTS:
    void setParam(qreal hue, qreal hsvSaturation, qreal value, qreal hslSaturation, qreal lightness, qreal hsiSaturation, qreal intensity, qreal hsySaturation, qreal luma);
Q_SIGNALS:
    void update();
    void paramChanged(qreal hue, qreal hsvSaturation, qreal value, qreal hslSaturation, qreal lightness, qreal hsiSaturation, qreal intensity, qreal hsySaturation, qreal luma);
protected:
    const KoColorSpace* colorSpace() const;
    bool isDirty() const;

    virtual KoColor selectColor(int x, int y) = 0;

    virtual void paint(QPainter*) = 0;

    virtual bool containsPointInComponentCoords(int x, int y) const;

    virtual bool allowsColorSelectionAtPoint(const QPoint &) const;

    void setLastMousePosition(int x, int y);

    qreal m_hue {0.0};
    qreal m_hsvSaturation {0.0};
    qreal m_value {0.0};
    qreal m_hslSaturation {0.0};
    qreal m_lightness {0.0};
    qreal m_hsiSaturation {0.0};
    qreal m_intensity {0.0};
    qreal m_hsySaturation {0.0};
    qreal m_luma {0.0};
    Parameter m_parameter;
    Type m_type;
    KisColorSelector* m_parent {0};
    bool m_gamutMaskOn {false};
    KoGamutMaskSP m_currentGamutMask;
    bool m_maskPreviewActive {false};
    qreal m_lastX {0.0};
    qreal m_lastY {0.0};
    int m_x {0};
    int m_y {0};
private:
    int m_width {0};
    int m_height {0};
    bool m_dirty {false};
    const KoColorSpace* m_lastColorSpace {0};
    KoColor m_lastSelectedColor;
};

#endif
