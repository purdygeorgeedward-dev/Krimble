/*
 * SPDX-FileCopyrightText: 2016 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDLGINTERNALCOLORSELECTOR_H
#define KISDLGINTERNALCOLORSELECTOR_H

#include "kritawidgets_export.h"
#include "KoColor.h"
#include "KoColorSpace.h"
#include "KoColorDisplayRendererInterface.h"
#include "KoColorSet.h"

#include <QScopedPointer>
#include <QDialog>

#include "KisScreenColorSamplerBase.h"

class Ui_WdgDlgInternalColorSelector;
class KoColorPatch;

class KRITAWIDGETS_EXPORT KisDlgInternalColorSelector : public QDialog
{
    Q_OBJECT

    static std::function<KisScreenColorSamplerBase *(QWidget *)> s_screenColorSamplerFactory;

public:

    static void setScreenColorSamplerFactory(std::function<KisScreenColorSamplerBase *(QWidget *)> f) {
        s_screenColorSamplerFactory = f;
    }

    struct Config
    {
        Config() :
            modal(true),
            visualColorSelector(true),
            paletteBox(true),
            screenColorSampler(true),
            prevNextButtons(true),
            hexInput(true),
            useAlpha(false){}
        bool modal;
        bool visualColorSelector;
        bool paletteBox;
        bool screenColorSampler;
        bool prevNextButtons;
        bool hexInput;
        bool useAlpha;
    };

    KisDlgInternalColorSelector(QWidget* parent, KoColor color, Config config, const QString &caption, const KoColorDisplayRendererInterface *displayRenderer = KoDumbColorDisplayRenderer::instance());
    ~KisDlgInternalColorSelector() override;

    void colorSpaceChanged(const KoColorSpace *cs);

    void lockUsedColorSpace(const KoColorSpace *cs);

    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer);

    KoColor getCurrentColor();

    void chooseAlpha(bool chooseAlpha);

Q_SIGNALS:

    void signalForegroundColorChosen(KoColor color);

public Q_SLOTS:
    void slotColorUpdated(KoColor newColor);

    void slotSetColorFromPatch(KoColorPatch* patch);

    void setPreviousColor(KoColor c);

    void reject() override;

private Q_SLOTS:

    void slotSelectorModelChanged();
    void endUpdateWithNewColor();

    void slotFinishUp();

    void slotSetColorFromHex();

    void slotChangePalette(KoColorSetSP set);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void focusInEvent(QFocusEvent *) override;
    void updateAllElements(QObject *source);

private:
    Ui_WdgDlgInternalColorSelector *m_ui;
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
