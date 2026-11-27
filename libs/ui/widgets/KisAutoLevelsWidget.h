/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_AUTO_LEVELS_WIDGET_H
#define KIS_AUTO_LEVELS_WIDGET_H

#include <QWidget>

#include <KoColor.h>
#include <KisAutoLevels.h>
#include <kritaui_export.h>

#include "ui_KisAutoLevelsWidget.h"

class KRITAUI_EXPORT KisAutoLevelsWidget : public QWidget
{
    Q_OBJECT

public:
    KisAutoLevelsWidget(QWidget *parent);
    ~KisAutoLevelsWidget() override;

    KisAutoLevels::ShadowsAndHighlightsAdjustmentMethod shadowsAndHighlightsAdjustmentMethod() const;
    qreal shadowsClipping() const;
    qreal highlightsClipping() const;
    qreal maximumInputBlackAndWhiteOffset() const;
    KisAutoLevels::MidtonesAdjustmentMethod midtonesAdjustmentMethod() const;
    qreal midtonesAdjustmentAmount() const;
    KoColor outputShadowsColor() const;
    KoColor outputHighlightsColor() const;
    KoColor outputMidtonesColor() const;

public Q_SLOTS:
    void setShadowsAndHighlightsAdjustmentMethod(KisAutoLevels::ShadowsAndHighlightsAdjustmentMethod newMethod);
    void setShadowsClipping(qreal newShadowsClipping);
    void setHighlightsClipping(qreal newHighlightsClipping);
    void setMaximumInputBlackAndWhiteOffset(qreal newMaximumInputBlackAndWhiteOffset);
    void setMidtonesAdjustmentMethod(KisAutoLevels::MidtonesAdjustmentMethod newMethod);
    void setMidtonesAdjustmentAmount(qreal newMidtonesAdjustmentAmount);
    void setShadowsColor(const KoColor &newShadowsColor);
    void setHighlightsColor(const KoColor &newHighlightsColor);
    void setMidtonesColor(const KoColor &newMidtonesColor);
    void lockShadowsAndHighlightsAdjustmentMethod();
    void unlockShadowsAndHighlightsAdjustmentMethod();

Q_SIGNALS:
    void parametersChanged();

private:
    Ui_KisAutoLevelsWidget m_ui;
};

#endif
