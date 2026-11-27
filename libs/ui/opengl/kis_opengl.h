/*
 *  SPDX-FileCopyrightText: 2007 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_OPENGL_H_
#define KIS_OPENGL_H_

#include <QtGlobal>
#include <QFlags>

#include <QSurfaceFormat>
#include <QStringList>

#include "kis_config.h"
#include <KisQStringListFwd.h>

#include "kritaui_export.h"

class QOpenGLContext;
class QString;
class QSurfaceFormat;

class KRITAUI_EXPORT KisOpenGL
{
public:
    enum FilterMode {
        NearestFilterMode,
        BilinearFilterMode,
        TrilinearFilterMode,
        HighQualityFiltering
    };

    enum OpenGLRenderer {
        RendererNone = 0x00,
        RendererAuto = 0x01,
        RendererDesktopGL = 0x02,
        RendererOpenGLES = 0x04,
        RendererSoftware = 0x08
    };
    Q_DECLARE_FLAGS(OpenGLRenderers, OpenGLRenderer)

    enum AngleRenderer {
        AngleRendererDefault    = 0x0000,
        AngleRendererD3d11      = 0x0002,
        AngleRendererD3d9       = 0x0004,
        AngleRendererD3d11Warp  = 0x0008,
    };

    struct KRITAUI_EXPORT RendererConfig {
        QSurfaceFormat format;
        AngleRenderer angleRenderer = AngleRendererDefault;

        OpenGLRenderer rendererId() const;
    };

    enum XcbGLProviderProtocol {
        XCB_GLX = 0,
        XCB_EGL
    };

public:
    static RendererConfig selectSurfaceConfig(KisOpenGL::OpenGLRenderer preferredRenderer,
                                              KisConfig::RootSurfaceFormat preferredRootSurfaceFormat,
                                              KisConfig::CanvasSurfaceBitDepthMode preferredCanvasSurfaceBitMode,
                                              bool enableDebug);

    static void setDefaultSurfaceConfig(const RendererConfig &config);

    static OpenGLRenderer getCurrentOpenGLRenderer();
    static OpenGLRenderer getQtPreferredOpenGLRenderer();
    static OpenGLRenderers getSupportedOpenGLRenderers();
    static OpenGLRenderer getUserPreferredOpenGLRendererConfig();
    static void setUserPreferredOpenGLRendererConfig(OpenGLRenderer renderer);
    static QString convertOpenGLRendererToConfig(OpenGLRenderer renderer);
    static OpenGLRenderer convertConfigToOpenGLRenderer(QString renderer);

    static void initialize();

    static void initializeContext(QOpenGLContext *ctx);

    static const QString &getDebugText();

    static QStringList getOpenGLWarnings();

    static QString currentDriver();
    static bool supportsLoD();
    static bool hasOpenGL3();
    static bool hasOpenGLES();
    static bool supportsVAO();

    static bool hasOpenGL();

    static bool supportsFenceSync();

    static bool supportsBufferMapping();

    static bool forceDisableTextureBuffers();
    static bool shouldUseTextureBuffers(bool userPreference);

    static bool useTextureBufferInvalidation();

    static bool useFBOForToolOutlineRendering();

    static bool needsFenceWorkaround();

    static void testingInitializeDefaultSurfaceFormat();
    static void setDebugSynchronous(bool value);

    static void glInvalidateBufferData(uint buffer);

    static std::optional<XcbGLProviderProtocol> xcbGlProviderProtocol();

private:
    static void fakeInitWindowsOpenGL(KisOpenGL::OpenGLRenderers supportedRenderers, KisOpenGL::OpenGLRenderer preferredByQt);

    KisOpenGL();

};

#ifdef Q_OS_WIN
Q_DECLARE_OPERATORS_FOR_FLAGS(KisOpenGL::OpenGLRenderers);
#endif

Q_DECLARE_METATYPE(KisOpenGL::XcbGLProviderProtocol)

#endif
