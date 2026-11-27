/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISMULTISURFACESTATEMANAGER_H
#define KISMULTISURFACESTATEMANAGER_H

#include <kis_config.h>
#include <KisDisplayConfig.h>
#include <KisProofingConfiguration.h>

#include <config-use-surface-color-management-api.h>

#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API

#include <QPointer>
class KisRootSurfaceInfoProxy;

#endif

class KisMultiSurfaceStateManager
{
public:
    struct State {
        bool isCanvasOpenGL = false;
        KisProofingConfigurationSP proofingConfig;
        KisConfig::CanvasSurfaceMode surfaceMode;
        KisDisplayConfig::Options optionsFromConfig;
        KisMultiSurfaceDisplayConfig multiConfig;

        bool operator==(const State& other) const {
            return isCanvasOpenGL == other.isCanvasOpenGL &&
                   proofingConfig == other.proofingConfig &&
                   surfaceMode == other.surfaceMode &&
                   optionsFromConfig == other.optionsFromConfig &&
                   multiConfig == other.multiConfig;
        }

        bool operator!=(const State& other) const {
            return !(*this == other);
        }
    };

public:
    KisMultiSurfaceStateManager();
    ~KisMultiSurfaceStateManager();

#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API
    void setRootSurfaceInfoProxy(KisRootSurfaceInfoProxy *proxy);
#endif

    State createInitializingConfig(bool isCanvasOpenGL, int screenId, KisProofingConfigurationSP proofingConfig) const;

    State onCanvasSurfaceFormatChanged(const State &oldState,
                                       const KisDisplayConfig &canvasConfig) const;

    State onGuiSurfaceFormatChanged(const State &oldState,
                                    const KoColorProfile *uiProfile) const;

    State onProofingChanged(const State &oldState, KisProofingConfigurationSP proofingConfig) const;

    State onConfigChanged(const State &oldState,
                          int screenId,
                          KisConfig::CanvasSurfaceMode surfaceMode,
                          const KisDisplayConfig::Options &options) const;

    State onScreenChanged(const State &oldState, int screenId) const;

private:
    KisDisplayConfig::Options overriddenWithProofingConfig(const KisDisplayConfig::Options &options, KisProofingConfigurationSP proofingConfig) const;

private:
#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API
    QPointer<KisRootSurfaceInfoProxy> m_rootSurfaceInfoProxy;
#endif
};

#endif
