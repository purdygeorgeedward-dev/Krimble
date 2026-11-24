/*
 * SPDX-FileCopyrightText: 2016 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 * SPDX-FileCopyrightText: 2022 Mathias Wein <lynx.mw+kde@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_VISUAL_COLOR_SELECTOR_H
#define KIS_VISUAL_COLOR_SELECTOR_H

#include <QWidget>
#include <QScopedPointer>

#include <resources/KoGamutMask.h>

#include "KisColorSelectorInterface.h"
#include "KisVisualColorModel.h"
#include "kritawidgets_export.h"

class QVector4D;
class KisColorSelectorConfiguration;

class KRITAWIDGETS_EXPORT KisVisualColorSelector : public KisColorSelectorInterface
{
    Q_OBJECT
public:

    enum RenderMode {
        StaticBackground,
        DynamicBackground,
        CompositeBackground
    };

    explicit KisVisualColorSelector(QWidget *parent = 0, KisVisualColorModelSP model = KisVisualColorModelSP());
    ~KisVisualColorSelector() override;

    QSize minimumSizeHint() const override;
    void setSelectorModel(KisVisualColorModelSP model);
    KisVisualColorModelSP selectorModel() const;
    void setConfig(bool forceCircular, bool forceSelfUpdate) override;
    const KisColorSelectorConfiguration& configuration() const;
    void setConfiguration(const KisColorSelectorConfiguration *config);
    void setAcceptTabletEvents(bool on);
    KoColor getCurrentColor() const override;
    void setMinimumSliderWidth(int width);
    const KoColorDisplayRendererInterface* displayRenderer() const;
    RenderMode renderMode() const;
    void setRenderMode(RenderMode mode);
    bool autoAdjustExposure() const;
    void setAutoAdjustExposure(bool enabled);
    bool proofColors() const;
    void setProofColors(bool enabled);
    void setSliderPosition(Qt::Edge edge);
    KoGamutMask* activeGamutMask() const;

public Q_SLOTS:
    void slotSetColor(const KoColor &c) override;
    void slotSetColorSpace(const KoColorSpace *cs) override;
    void slotConfigurationChanged();
    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer) override;
    void slotGamutMaskChanged(KoGamutMaskSP mask);
    void slotGamutMaskUnset();
    void slotGamutMaskPreviewUpdate();

private Q_SLOTS:
    void slotChannelValuesChanged(const QVector4D &values, quint32 channelFlags);
    void slotColorModelChanged();
    void slotColorSpaceChanged();
    void slotCursorMoved(QPointF pos);
    void slotDisplayConfigurationChanged();
    void slotReloadConfiguration();

Q_SIGNALS:
    void sigInteraction(bool active);
    void sigGamutMaskSupportChanged(bool supported);

protected:
    void resizeEvent(QResizeEvent *) override;

private:
    bool useHorizontalSlider();
    void rebuildSelector();
    void loadACSConfig();
    void switchDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer);
    QVector4D calculateMaxChannelValues();
    static KisColorSelectorConfiguration validatedConfiguration(const KisColorSelectorConfiguration &cfg);

    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
