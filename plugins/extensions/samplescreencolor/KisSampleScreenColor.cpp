/*
 *  SPDX-FileCopyrightText: 2023 killy |0veufOrever <80536642@qq.com>
 *  SPDX-FileCopyrightText: 2023 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KisSampleScreenColor.h"

#include <KisScreenColorSampler.h>
#include <kpluginfactory.h>
#include <kis_action.h>
#include <kis_canvas_resource_provider.h>
#include <KisViewManager.h>
#include <KoToolManager.h>

K_PLUGIN_FACTORY_WITH_JSON(KisSampleScreenColorFactory, "kritasamplescreencolor.json", registerPlugin<KisSampleScreenColor>();)

KisSampleScreenColor::KisSampleScreenColor(QObject *parent, const QVariantList &)
    : KisActionPlugin(parent)
{
    KisAction *action = createAction("sample_screen_color");
    connect(action, &QAction::triggered, [this](){ slotSampleScreenColor(false); });

    action = createAction("sample_screen_color_real_canvas");
    connect(action, &QAction::triggered, [this](){ slotSampleScreenColor(true); });
}

KisSampleScreenColor::~KisSampleScreenColor()
{}

void KisSampleScreenColor::slotSampleScreenColor(bool sampleRealCanvas)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(sampleRealCanvas);
    KoToolManager::instance()->switchToolRequested(QStringLiteral("KritaSelected/KisToolColorSampler"));
    return;
#endif
    if (m_screenColorSampler) {
        m_screenColorSampler->cancel();
        if (sampleRealCanvas == m_lastSampleRealCanvas) {
            return;
        }
    }

    m_lastSampleRealCanvas = sampleRealCanvas;
    m_screenColorSampler = new KisScreenColorSampler(false);
    m_screenColorSampler->setPerformRealColorSamplingOfCanvas(sampleRealCanvas);
    m_screenColorSampler->setCurrentColor(viewManager()->canvasResourceProvider()->fgColor());
    m_screenColorSampler->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_screenColorSampler, &KisScreenColorSampler::sigNewColorSampled, this,
        [this](KoColor sampledColor)
        {
            viewManager()->canvasResourceProvider()->slotSetFGColor(sampledColor);
            m_screenColorSampler->close();
            m_screenColorSampler = nullptr;
        }
    );
    connect(m_screenColorSampler, &KisScreenColorSampler::sigNewColorHovered, this,
        [this](KoColor sampledColor)
        {
            viewManager()->canvasResourceProvider()->slotSetFGColor(sampledColor);
        }
    );
    m_screenColorSampler->sampleScreenColor();
}

#include "KisSampleScreenColor.moc"
