/*
* SPDX-FileCopyrightText: 1999 Matthias Elter <me@kde.org>
* SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
* SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
*
*  SPDX-License-Identifier: GPL-2.0-or-later
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <KLocalizedTranslator>
#include <QByteArray>
#include <QDate>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QOperatingSystemVersion>
#include <QPixmap>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <QTranslator>
#include <QImageReader>

#include <kaboutdata.h>

#include <KisApplication.h>
#include <KisMainWindow.h>
#include <KisSupportedArchitectures.h>
#include <KisUsageLogger.h>
#include <KoConfig.h>
#include <KoResourcePaths.h>
#include <kis_config.h>
#include <kis_debug.h>
#include <kis_image_config.h>
#include <opengl/kis_opengl.h>
#include "KritaVersionWrapper.h"

#include "KisApplicationArguments.h"
#include "KisDocument.h"
#include "KisPart.h"
#include "KisUiFont.h"
#include "input/KisQtWidgetsTweaker.h"
#include "kis_splash_screen.h"
#include "config-qt-patches-present.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <KisAndroidCrashHandler.h>
#include <KisAndroidUtils.h>
#endif

#if defined Q_OS_WIN
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  include "config_qt5_has_wintab_switch.h"
#else
#  include <QtGui/private/qguiapplication_p.h>
#  include <QtGui/qpa/qplatformintegration.h>
#endif
#include <windows.h>
#include <winuser.h>
#include <dialogs/KisDlgCustomTabletResolution.h>
#include "config-high-dpi-scale-factor-rounding-policy.h"
#include "config-set-has-border-in-full-screen-default.h"
#ifdef HAVE_SET_HAS_BORDER_IN_FULL_SCREEN_DEFAULT
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif
#include <QLibrary>
#endif

#ifdef Q_OS_MACOS
#include "libs/macosutils/KisMacosEntitlements.h"
#include "libs/macosutils/KisMacosSystemProber.h"
#endif

#ifdef Q_OS_HAIKU
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

#if defined HAVE_KCRASH
#include <kcrash.h>
#elif defined USE_DRMINGW
namespace
{
template<typename T, typename U>
inline T cast_to_function(U v) noexcept
{
    return reinterpret_cast<T>(reinterpret_cast<void *>(v));
}

void tryInitDrMingw()
{
    const QString pathStr = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("exchndl.dll");

    QLibrary hMod(pathStr);
    if (!hMod.load()) {
        return;
    }

    using ExcHndlSetLogFileNameA_type = BOOL(APIENTRY *)(const char *);

    const auto myExcHndlSetLogFileNameA = cast_to_function<ExcHndlSetLogFileNameA_type>(hMod.resolve("ExcHndlSetLogFileNameA"));
    if (!myExcHndlSetLogFileNameA) {
        return;
    }

    const QString logFile = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)).absoluteFilePath("kritacrash.log");
    const QByteArray logFilePath = QDir::toNativeSeparators(logFile).toLocal8Bit();
    myExcHndlSetLogFileNameA(logFilePath.data());
}
}
#endif

namespace
{

void installTranslators(KisApplication &app);

}

#ifdef Q_OS_WIN
namespace
{
using pSetDisplayAutoRotationPreferences_t = decltype(&SetDisplayAutoRotationPreferences);

void resetRotation()
{
    QLibrary user32Lib("user32");
    if (!user32Lib.load()) {
        qWarning() << "Failed to load user32.dll! This really should not happen.";
        return;
    }
    auto pSetDisplayAutoRotationPreferences = cast_to_function<pSetDisplayAutoRotationPreferences_t>(user32Lib.resolve("SetDisplayAutoRotationPreferences"));
    if (!pSetDisplayAutoRotationPreferences) {
        dbgKrita << "Failed to load function SetDisplayAutoRotationPreferences";
        return;
    }
    bool result = pSetDisplayAutoRotationPreferences(ORIENTATION_PREFERENCE_NONE);
    dbgKrita << "SetDisplayAutoRotationPreferences(ORIENTATION_PREFERENCE_NONE) returned" << result;
}
}
#endif

#ifdef Q_OS_ANDROID
extern "C" JNIEXPORT void JNICALL
Java_org_krimble_android_JNIWrappers_saveState(JNIEnv* ,
                                             jobject ,
                                             jint    )
{
    if (!KisPart::exists()) return;

    KisPart *kisPart = KisPart::instance();
    QList<QPointer<KisDocument>> list = kisPart->documents();
    for (QPointer<KisDocument> &doc: list)
    {
        doc->autoSaveOnPause();
    }

    const QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    QSettings kritarc(configPath + QStringLiteral("/kritadisplayrc"), QSettings::IniFormat);
    kritarc.setValue("canvasState", "OPENGL_SUCCESS");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_krimble_android_JNIWrappers_hasMainWindowLoaded(JNIEnv * ,
                                                       jobject ,
                                                       jint )
{
    if (!KisPart::exists()) {
        return false;
    }

    KisMainWindow *mainWindow = KisPart::instance()->currentMainwindow();
    return (bool)mainWindow;
}

extern "C" JNIEXPORT void JNICALL
Java_org_krimble_android_JNIWrappers_openFileFromIntent(JNIEnv* ,
                                                      jobject ,
                                                      jstring str)
{
    QAndroidJniObject jUri(str);
    if (jUri.isValid()) {
        QString uri = jUri.toString();
        QMetaObject::invokeMethod(KisApplication::instance(), "fileOpenRequested",
                                  Qt::QueuedConnection, Q_ARG(QString, uri));
    }
}

#define MAIN_EXPORT __attribute__ ((visibility ("default")))
#define MAIN_FN main
#elif defined Q_OS_WIN
#define MAIN_EXPORT __declspec(dllexport)
#define MAIN_FN krita_main
#else
#define MAIN_EXPORT
#define MAIN_FN main
#endif

extern "C" MAIN_EXPORT int MAIN_FN(int argc, char **argv)
{
#ifdef Q_OS_ANDROID
    KisAndroidUtils::performInitialSetup();
#endif

#ifdef Q_OS_WIN
    if (GetACP() == CP_UTF8) {
        SetConsoleOutputCP(CP_UTF8);
    }
#endif

    bool runningInKDE = !qgetenv("KDE_FULL_SESSION").isEmpty();

#if defined HAVE_X11
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
#elif defined Q_OS_WIN
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
    }
#endif

    qputenv("QT_BEARER_POLL_TIMEOUT", QByteArray::number(-1));

    QString key = "Krita5" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation).replace("/", "_");
    key = key.replace(":", "_").replace("\\","_");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache, true);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QImageReader::setAllocationLimit(0);
#endif

#if defined(Q_OS_WIN) && defined(HAVE_HIGH_DPI_SCALE_FACTOR_ROUNDING_POLICY)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);
#endif

#ifdef Q_OS_ANDROID
    const QString write_permission = "android.permission.WRITE_EXTERNAL_STORAGE";
    const QStringList permissions = { write_permission };
    const QtAndroid::PermissionResultMap resultHash =
            QtAndroid::requestPermissionsSync(QStringList(permissions));

    if (resultHash[write_permission] == QtAndroid::PermissionResult::Denied) {
        dbgKrita << "Permission denied by the user";
    }
    else {
        dbgKrita << "Permission granted";
    }

    KisAndroidCrashHandler::handler_init();

    qputenv("FONTCONFIG_PATH",
            QFile::encodeName(KoResourcePaths::getApplicationRoot()) + "/share/etc/fonts/");
    qputenv("XDG_CACHE_HOME",
            QFile::encodeName(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)));
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    const bool isInAppimage = qEnvironmentVariableIsSet("APPIMAGE");
    if (isInAppimage) {
        QString appimageMountDir = qgetenv("APPDIR");

        {
            const QString mltLibs = "/usr/lib/mlt-7";
            const QString mltData = "/usr/share/mlt-7";

            qputenv("MLT_ROOT_DIR", (appimageMountDir + "/usr").toUtf8());
            qputenv("MLT_REPOSITORY", (appimageMountDir + mltLibs).toUtf8());
            qputenv("MLT_DATA", (appimageMountDir + mltData).toUtf8());
            qputenv("MLT_PROFILES_PATH", (appimageMountDir + mltData + "/profiles").toUtf8());
            qputenv("MLT_PRESETS_PATH", (appimageMountDir + mltData + "/presets").toUtf8());
        }

        {
            if (qgetenv("FONTCONFIG_PATH").isEmpty()) {
                const QString defaultFontsConfig = "/etc/fonts/fonts.conf";
                const QFileInfo info(defaultFontsConfig);
                if (info.exists()) {
                    qputenv("FONTCONFIG_PATH", QFile::encodeName(QDir::toNativeSeparators(info.absolutePath())));
                }
            }
        }
    }
#endif

    const QDir configPath(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
    QSettings kritarc(configPath.absoluteFilePath("kritadisplayrc"), QSettings::IniFormat);

#ifdef Q_OS_ANDROID
    static constexpr bool ALLOW_FALLBACK_LANGUAGES = false;
#else
    static constexpr bool ALLOW_FALLBACK_LANGUAGES = true;
#endif

    QString root;
    QString language;
    {
        QCoreApplication app(argc, argv);
        Q_UNUSED(app);
        root = KoResourcePaths::getApplicationRoot();
        QSettings languageoverride(configPath.absoluteFilePath("klanguageoverridesrc"), QSettings::IniFormat);
        languageoverride.beginGroup("Language");
        language = languageoverride.value(qAppName(), "").toString();
    }

    bool enableOpenGLDebug = false;
    bool openGLDebugSynchronous = false;
    bool logUsage = true;
    {
        bool enableHighDpiScaling = kritarc.value("EnableHiDPI", true).toBool() || !qgetenv("KRITA_HIDPI").isEmpty();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (!enableHighDpiScaling && !qEnvironmentVariableIsSet("QT_ENABLE_HIGHDPI_SCALING")) {
            qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
        }
#else
        if (enableHighDpiScaling) {
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        }
#endif
#if defined(Q_OS_WIN) && defined(HAVE_HIGH_DPI_SCALE_FACTOR_ROUNDING_POLICY)
        if (kritarc.value("EnableHiDPIFractionalScaling", false).toBool()) {
            QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
        }
#endif

#ifdef HAVE_X11
    if (!qEnvironmentVariableIsSet("QT_XCB_GL_INTEGRATION")) {
        if (KisConfig::preferXcbEglProvider(&kritarc)) {
            qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl");
        }
    }
#endif

if (!qEnvironmentVariableIsEmpty("KRITA_OPENGL_DEBUG")) {
            enableOpenGLDebug = true;
        } else {
            enableOpenGLDebug = kritarc.value("EnableOpenGLDebug", false).toBool();
        }
        if (enableOpenGLDebug && (qgetenv("KRITA_OPENGL_DEBUG") == "sync" || kritarc.value("OpenGLDebugSynchronous", false).toBool())) {
            openGLDebugSynchronous = true;
        }

        KisConfig::RootSurfaceFormat rootSurfaceFormat = KisConfig::rootSurfaceFormat(&kritarc);
        KisOpenGL::OpenGLRenderer preferredRenderer = KisOpenGL::RendererAuto;

        logUsage = kritarc.value("LogUsage", true).toBool();

#ifdef Q_OS_WIN
        const QString preferredRendererString = kritarc.value("OpenGLRenderer", "angle").toString();
#else
        const QString preferredRendererString = kritarc.value("OpenGLRenderer", "auto").toString();
#endif
        preferredRenderer = KisOpenGL::convertConfigToOpenGLRenderer(preferredRendererString);

        const KisConfig::CanvasSurfaceBitDepthMode bitDepthMode =
            KisConfig::canvasSurfaceBitDepthMode(&kritarc);

        const KisOpenGL::RendererConfig config =
            KisOpenGL::selectSurfaceConfig(preferredRenderer, rootSurfaceFormat, bitDepthMode, enableOpenGLDebug);

        KisOpenGL::setDefaultSurfaceConfig(config);
        KisOpenGL::setDebugSynchronous(openGLDebugSynchronous);

#if defined Q_OS_WIN || defined Q_OS_MACOS
    qputenv("QT_WIDGETS_RHI", "1");
    qputenv("QT_WIDGETS_RHI_BACKEND", "opengl");
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (config.rendererId() == KisOpenGL::RendererOpenGLES) {
        if (!qEnvironmentVariableIsSet("QT_USE_PREMATURE_RESIZE_EVENTS")) {
            qInfo() << "INFO: activating QT_USE_PREMATURE_RESIZE_EVENTS...";
            qputenv("QT_USE_PREMATURE_RESIZE_EVENTS", "1");
        }
        if (!qEnvironmentVariableIsSet("QT_ANGLE_MANUALLY_UPDATE_SURFACE_SIZE")) {
            qInfo() << "INFO: activating QT_ANGLE_MANUALLY_UPDATE_SURFACE_SIZE...";
            qputenv("QT_ANGLE_MANUALLY_UPDATE_SURFACE_SIZE", "1");
        }
        if (!qEnvironmentVariableIsSet("QT_PREFILL_RHI_SURFACE")) {
            qInfo() << "INFO: activating QT_PREFILL_RHI_SURFACE...";
            qputenv("QT_PREFILL_RHI_SURFACE", "1");
        }
    }
#endif

#if defined Q_OS_WIN
    if (!qEnvironmentVariableIsSet("QT_DISABLE_ACCESSIBILITY")) {
        if (kritarc.value("DisableAccessibilityInQt", false).toBool()) {
            qInfo() << "INFO: activating QT_DISABLE_ACCESSIBILITY via kritadisplayrc...";
            qputenv("QT_DISABLE_ACCESSIBILITY", "1");
        }
    } else {
        if (qEnvironmentVariableIntValue("QT_DISABLE_ACCESSIBILITY")) {
            qInfo() << "INFO: QT_DISABLE_ACCESSIBILITY is activated via environment...";
        }
    }
#endif

#if KRITA_QT_HAS_UPDATE_COMPRESSION_PATCH
    if (!qEnvironmentVariableIsSet("QT_BACKING_STORE_USE_FAST_QIMAGE_TRANSFER")) {
        qputenv("QT_BACKING_STORE_USE_FAST_QIMAGE_TRANSFER", "1");
    }

    if (!qEnvironmentVariableIsSet("QT_FRAME_RATE_OVERRIDE")) {
        KisImageConfig cfg(true);
        if (!cfg.detectFpsLimit()) {
            qputenv("QT_FRAME_RATE_OVERRIDE", QString::number(cfg.fpsLimit()).toLatin1());
        }
    }
#endif

#ifdef Q_OS_WIN
        resetRotation();
#endif
    }

    if (logUsage) {
        KisUsageLogger::initialize();
    }

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    {
        QByteArray originalXdgDataDirs = qgetenv("XDG_DATA_DIRS");
        if (originalXdgDataDirs.isEmpty()) {
            originalXdgDataDirs = "/usr/local/share/:/usr/share/";
        }

        qputenv("XDG_DATA_DIRS", QString(QFile::encodeName(root + "share") + ":" + originalXdgDataDirs).toUtf8());
    }
#elif defined(Q_OS_HAIKU)
	qputenv("KRITA_PLUGIN_PATH", QString(QFile::encodeName(root + "lib")).toUtf8());
    qputenv("XDG_DATA_DIRS", QString(QFile::encodeName(root + "share") + ":" + qgetenv("XDG_DATA_DIRS")).toUtf8());
#else
    qputenv("XDG_DATA_DIRS", QFile::encodeName(QDir(root + "share").absolutePath()));
#endif

    dbgKrita << "Setting XDG_DATA_DIRS" << qgetenv("XDG_DATA_DIRS");

    dbgLocale << "Override language:" << language;
    bool rightToLeft = false;
    if (!language.isEmpty()) {
        QStringList languageList = language.split(QStringLiteral(":"));
        const QString &firstLanguage = languageList.constFirst();
        if (ALLOW_FALLBACK_LANGUAGES) {
            KLocalizedString::setLanguages(languageList);
        } else {
            KLocalizedString::setLanguages(QStringList() << firstLanguage);
        }

        QLocale locale(firstLanguage);
        QLocale::setDefault(locale);
#ifdef Q_OS_MAC
        qputenv("LANG", (locale.name() + ".UTF-8").toLocal8Bit());
#else
        qputenv("LANG", locale.name().toLocal8Bit());
#endif

        const QStringList rtlLanguages = QStringList()
                << "ar" << "dv" << "he" << "ha" << "ku" << "fa" << "ps" << "ur" << "yi";

        if (rtlLanguages.contains(firstLanguage)) {
            rightToLeft = true;
        }
    }
    else {
        dbgLocale << "Qt UI languages:" << QLocale::system().uiLanguages() << qgetenv("LANG");

        QLocale locale = QLocale::system();

#ifdef Q_OS_ANDROID
        if (locale.name() == QLocale::c().name()) {
            QAndroidJniObject localeJniObj = QAndroidJniObject::callStaticObjectMethod(
                "java/util/Locale", "getDefault", "()Ljava/util/Locale;");

            if (localeJniObj.isValid()) {
                QAndroidJniObject tag = localeJniObj.callObjectMethod("toLanguageTag",
                                                                      "()Ljava/lang/String;");
                if (tag.isValid()) {
                    locale = QLocale(tag.toString());
                }
            }
        }
#endif
        if (locale.name() != QStringLiteral("en")) {
            QStringList uiLanguages = locale.uiLanguages();
            for (QString &uiLanguage : uiLanguages) {

                if (uiLanguage.startsWith("en") || uiLanguage.startsWith("pt")) {
                    uiLanguage.replace(QChar('-'), QChar('_'));
                }
                else if (uiLanguage.startsWith("zh-Hant") || uiLanguage.startsWith("zh-TW")) {
                    uiLanguage = "zh_TW";
                }
                else if (uiLanguage.startsWith("zh-Hans") || uiLanguage.startsWith("zh-CN")) {
                    uiLanguage = "zh_CN";
                }
            }

            if (!uiLanguages.empty()) {
                QString envLanguage = uiLanguages.first();
                envLanguage.replace(QChar('-'), QChar('_'));

                for (int i = 0; i < uiLanguages.size(); i++) {
                    QString uiLanguage = uiLanguages[i];
                    int idx = uiLanguage.indexOf(QChar('-'));

                    if (idx != -1) {
                        uiLanguage = uiLanguage.left(idx);
                        uiLanguages.replace(i, uiLanguage);
                    }
                }
                dbgLocale << "Converted ui languages:" << uiLanguages;
#ifdef Q_OS_MAC
                KLocalizedString::setLanguages(QStringList() << uiLanguages.first());
                qputenv("LANG", (envLanguage + ".UTF-8").toLocal8Bit());
                qputenv("LANGUAGE", (envLanguage + ".UTF-8").toLocal8Bit());
#else
                if (ALLOW_FALLBACK_LANGUAGES) {
                    KLocalizedString::setLanguages(QStringList() << uiLanguages.first());
                } else {
                    KLocalizedString::setLanguages(QStringList() << uiLanguages);
                }
                qputenv("LANG", envLanguage.toLocal8Bit());
                qputenv("LANGUAGE", envLanguage.toLocal8Bit());
#endif
            }
        }
    }

    KisUsageLogger::writeLocaleSysInfo();

#if defined Q_OS_WIN && QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && defined QT5_HAS_WINTAB_SWITCH
    const bool forceWinTab = !KisConfig::useWin8PointerInputNoApp(&kritarc);
    QCoreApplication::setAttribute(Qt::AA_MSWindowsUseWinTabAPI, forceWinTab);

    if (qEnvironmentVariableIsEmpty("QT_WINTAB_DESKTOP_RECT") &&
        qEnvironmentVariableIsEmpty("QT_IGNORE_WINTAB_MAPPING")) {

        QRect customTabletRect;
        KisDlgCustomTabletResolution::Mode tabletMode =
            KisDlgCustomTabletResolution::getTabletMode(&customTabletRect);
        KisDlgCustomTabletResolution::applyConfiguration(tabletMode, customTabletRect);
    }
#endif

    KisApplication app(key, argc, argv);

#if defined Q_OS_WIN && QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    const bool forceWinTab = !KisConfig::useWin8PointerInputNoApp(&kritarc);
    using QWindowsApplication = QNativeInterface::Private::QWindowsApplication;
    if (auto nativeWindowsApp = dynamic_cast<QWindowsApplication *>(QGuiApplicationPrivate::platformIntegration())) {
        nativeWindowsApp->setWinTabEnabled(forceWinTab);
    }
#endif

    installTranslators(app);

    KisUsageLogger::writeHeader();
    KisOpenGL::initialize();

#ifdef HAVE_SET_HAS_BORDER_IN_FULL_SCREEN_DEFAULT
    if (QCoreApplication::testAttribute(Qt::AA_UseDesktopOpenGL)) {
        QWindowsWindowFunctions::setHasBorderInFullScreenDefault(true);
    }
#endif

    if (!language.isEmpty()) {
        if (rightToLeft) {
            KisApplication::setLayoutDirection(Qt::RightToLeft);
        }
        else {
            KisApplication::setLayoutDirection(Qt::LeftToRight);
        }
    }
#ifdef Q_OS_ANDROID
    KisApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) || defined Q_OS_WIN || defined Q_OS_MACOS
    KLocalizedString::addDomainLocaleDir("krita", QDir(root + "share/locale").absolutePath());
#endif

    KLocalizedString::setApplicationDomain("krita");

    dbgLocale << "Available translations" << KLocalizedString::availableApplicationTranslations();
    dbgLocale << "Available domain translations" << KLocalizedString::availableDomainTranslations("krita");

    KAboutData aboutData("krita",
                            i18n("Krita"),
                            KritaVersionWrapper::versionString(true),
                            i18n("Krimble is your creative companion, a free graphics workhorse."),
                            KAboutLicense::GPL,
                            i18nc("@info:credit", "Krimble is based on Krita, © 1999–2026 The Krita Developers.\nKrimble modifications © 2026 Purdy Design.\nLicensed under the GNU General Public License."));
    aboutData.setHomepage(QStringLiteral("https://krimble.org"));
    aboutData.setOrganizationDomain("krita.org");
    aboutData.setBugAddress("https://forum.krimble.org/t/bug-reports");

    KAboutData::setApplicationData(aboutData);

    KIS_SAFE_ASSERT_RECOVER(app.organizationName().isEmpty()) {
        app.setOrganizationName("");
    }

#ifdef Q_OS_WIN
    QDir appdir(KoResourcePaths::getApplicationRoot());
    QString path = qgetenv("PATH");
    qputenv("PATH",
            QFile::encodeName(
                QDir::toNativeSeparators(appdir.absolutePath() + "/bin") + ";" +
                QDir::toNativeSeparators(appdir.absolutePath() + "/lib") + ";" +
                QDir::toNativeSeparators(appdir.absolutePath()) + ";" +
                path)
            );

    dbgKrita << "PATH" << qgetenv("PATH");
#endif

    if (KisApplication::applicationDirPath().contains(KRITA_BUILD_DIR)) {
        qFatal("FATAL: You're trying to run krita from the build location. You can only run Krita from the installation location.");
    }

#if defined HAVE_KCRASH
    KCrash::initialize();
#elif defined USE_DRMINGW
    tryInitDrMingw();
#endif
#if defined Q_OS_ANDROID
    qputenv("MLT_REPOSITORY", QFile::encodeName(qApp->applicationDirPath()));

    QString loc;
    if (QStandardPaths::standardLocations(QStandardPaths::HomeLocation).size() > 1) {
        loc = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[1];
    } else {
        loc = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0];
    }
    qputenv("MLT_DATA", QFile::encodeName(loc + "/share/mlt-7/"));
    qputenv("MLT_ROOT_DIR", QFile::encodeName(loc));
    qputenv("MLT_PROFILES_PATH", QFile::encodeName(loc + "/share/mlt-7/profiles/"));
    qputenv("MLT_PRESETS_PATH", QFile::encodeName(loc + "/share/mlt-7/presets/"));
    qputenv("MLT_PLUGIN_FILTER_STRING", "lib_mltplugin_");
#endif
    KisApplicationArguments args(app);

    if (app.isRunning()) {
        const bool batchRun = args.exportAs() || args.exportSequence();

        if (!batchRun) {
            if (app.sendMessage(args.serialize().toBase64())) {
                return 0;
            }
        }
    }
#ifdef Q_OS_MACOS
    else if (KisMacosEntitlements().sandbox()) {
        if(iskritaRunningActivate()) {
            return 0;
        }
    }
#endif

    if (!runningInKDE) {
        KisApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    KisApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif
    app.installEventFilter(KisQtWidgetsTweaker::instance());
    app.setDesktopFileName(QStringLiteral("org.kde.krita"));

    if (!args.noSplash()) {
        QWidget *splash = new KisSplashScreen();
        app.setSplashScreen(splash);
    }

#if defined Q_OS_WIN
    KisConfig cfg(false);
    QOperatingSystemVersion osVersion = QOperatingSystemVersion::current();
    if (osVersion.type() == QOperatingSystemVersion::Windows) {
        if (osVersion.majorVersion() < QOperatingSystemVersion::Windows7.majorVersion()) {
            if (cfg.readEntry("WarnedAboutUnsupportedWindows", false)) {
                QMessageBox::information(nullptr,
                                         i18nc("@title:window", "Krita: Warning"),
                                         i18n("You are running an unsupported version of Windows: %1.\n"
                                              "This is not recommended. Do not report any bugs.\n"
                                              "Please update to a supported version of Windows: Windows 7, 8, 8.1 or 10.", osVersion.name()));
                cfg.writeEntry("WarnedAboutUnsupportedWindows", true);

            }
        }
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && defined QT5_HAS_WINTAB_SWITCH
    const bool useWinInkAPI = !KisApplication::testAttribute(Qt::AA_MSWindowsUseWinTabAPI);

    if (useWinInkAPI != cfg.useWin8PointerInput()) {
        KisUsageLogger::log("WARNING: WinTab tablet protocol is not supported on this device. Switching to WinInk...");

        cfg.setUseWin8PointerInput(useWinInkAPI);
        cfg.setUseRightMiddleTabletButtonWorkaround(true);
    }
#endif
#endif
    KisApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents, false);

    QObject::connect(&app, &KisApplication::messageReceived,
                     &app, &KisApplication::remoteArguments);

    KisUsageLogger::writeSysInfo("\nHardware Information\n");
    KisUsageLogger::writeSysInfo(QString("  GPU Acceleration: %1").arg(kritarc.value("OpenGLRenderer", "auto").toString()));
    KisUsageLogger::writeSysInfo(QString("  Memory: %1 Mb").arg(KisImageConfig::totalRAM()));
    KisUsageLogger::writeSysInfo(QString("  Number of Cores: %1").arg(QThread::idealThreadCount()));
    KisUsageLogger::writeSysInfo(QString("  Swap Location: %1").arg(KisImageConfig(true).swapDir()));
    KisUsageLogger::writeSysInfo(
        QString("  Built for: %1")
            .arg(KisSupportedArchitectures::baseArchName()));
    KisUsageLogger::writeSysInfo(
        QString("  Base instruction set: %1")
            .arg(KisSupportedArchitectures::bestArchName()));
    KisUsageLogger::writeSysInfo(
        QString("  Supported instruction sets: %1")
            .arg(KisSupportedArchitectures::supportedInstructionSets()));
    KisUsageLogger::writeSysInfo("");

    KisConfig(true).logImportantSettings();

    KisApplication::setFont(KisUiFont::normalFont());

    if (!app.start(args)) {
        KisUsageLogger::log("Could not start Krita Application");
        return 1;
    }

    int state = KisApplication::exec();

    {
        QSettings kritarc(configPath.absoluteFilePath("kritadisplayrc"), QSettings::IniFormat);
        kritarc.setValue("canvasState", "OPENGL_SUCCESS");
    }

    if (logUsage) {
        KisUsageLogger::close();
    }

    KisPart::instance()->unloadPlaybackEngine();

#ifdef Q_OS_HAIKU
	kill(::getpid(), SIGKILL);
#endif

    return state;
}

namespace
{

void removeInstalledTranslators(KisApplication &app)
{

    QList<QTranslator *> translators = app.findChildren<QTranslator *>(QString(), Qt::FindDirectChildrenOnly);
    Q_FOREACH(const auto &translator, translators) {
        KisApplication::removeTranslator(translator);
    }
    dbgLocale << "Removed" << translators.size() << "QTranslator's";
}

void installPythonPluginUITranslator(KisApplication &app)
{
    KLocalizedTranslator *translator = new KLocalizedTranslator(&app);
    translator->setObjectName(QStringLiteral("KLocalizedTranslator.pykrita_plugin_ui"));
    translator->setTranslationDomain(QStringLiteral("krita"));
    translator->addContextToMonitor(QStringLiteral("pykrita_plugin_ui"));
    KisApplication::installTranslator(translator);
}

void installQtTranslations(KisApplication &app)
{
    const QStringList qtCatalogs = {
        QStringLiteral("qt_"),
        QStringLiteral("qtbase_"),
        QStringLiteral("qtdeclarative_"),
    };
    QList<QLocale> localeList;
    localeList.append(QLocale(QLocale::English));
    QLocale defaultLocale;
    if (defaultLocale.language() != QLocale::English) {
        localeList.append(defaultLocale);
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QString translationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#else
    QString translationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#endif
    dbgLocale << "Qt translations path:" << translationsPath;

    Q_FOREACH(const auto &localeToLoad, localeList) {
        Q_FOREACH(const auto &catalog, qtCatalogs) {
            QTranslator *translator = new QTranslator(&app);
            if (translator->load(localeToLoad, catalog, QString(), translationsPath)) {
                dbgLocale << "Loaded Qt translations for" << localeToLoad << catalog;
                translator->setObjectName(QStringLiteral("QTranslator.%1.%2").arg(localeToLoad.name(), catalog));
                KisApplication::installTranslator(translator);
            } else {
                delete translator;
            }
        }
    }
}

void installEcmTranslations(KisApplication &app)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const QString catalogVersion = "5";
#else
    const QString catalogVersion = "6";
#endif
    QStringList ecmCatalogs = {
        QStringLiteral("kcompletion%1_qt").arg(catalogVersion),
        QStringLiteral("kconfig%1_qt").arg(catalogVersion),
        QStringLiteral("kcoreaddons%1_qt").arg(catalogVersion),
        QStringLiteral("kitemviews%1_qt").arg(catalogVersion),
        QStringLiteral("kwidgetsaddons%1_qt").arg(catalogVersion),
        QStringLiteral("kwindowsystem%1_qt").arg(catalogVersion),
        QStringLiteral("seexpr2_qt"),
    };

    QStringList ki18nLangs = KLocalizedString::languages();
    const QString langEn = QStringLiteral("en");
    int indexOfEnUs = ki18nLangs.indexOf(QStringLiteral("en_US"));
    if (indexOfEnUs != -1) {
        ki18nLangs[indexOfEnUs] = langEn;
    }
    int indexOfEn = ki18nLangs.indexOf(langEn);
    if (indexOfEn != -1) {
        for (int i = ki18nLangs.size() - indexOfEn - 1; i > 0; i--) {
            ki18nLangs.removeLast();
        }
    } else {
        ki18nLangs.append(langEn);
    }

    QStringListIterator langIter(ki18nLangs);
    langIter.toBack();

    while (langIter.hasPrevious()) {
        const QString &localeDirName = langIter.previous();
        Q_FOREACH(const auto &catalog, ecmCatalogs) {
            QString subPath = QStringLiteral("locale/") % localeDirName % QStringLiteral("/LC_MESSAGES/") % catalog % QStringLiteral(".qm");
#if defined(Q_OS_ANDROID)
            const QString fullPath = QStringLiteral("assets:/") + subPath;
#else
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            const QString root = QLibraryInfo::location(QLibraryInfo::PrefixPath);
#else
            const QString root = QLibraryInfo::path(QLibraryInfo::PrefixPath);
#endif

            QString fullPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, subPath);

            if (fullPath.isEmpty()) {
                fullPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, subPath);
            }

            if (fullPath.isEmpty()) {
                fullPath = root + "/share/" + subPath;
            }
#endif
            if (!QFile::exists(fullPath)) {
                continue;
            }

            QTranslator *translator = new QTranslator(&app);
            if (translator->load(fullPath)) {
                dbgLocale << "Loaded ECM translations for" << localeDirName << catalog;
                translator->setObjectName(QStringLiteral("QTranslator.%1.%2").arg(localeDirName, catalog));
                KisApplication::installTranslator(translator);
            } else {
                delete translator;
            }
        }
    }
}

void installTranslators(KisApplication &app)
{
    removeInstalledTranslators(app);
    installPythonPluginUITranslator(app);
    installQtTranslations(app);
    installEcmTranslations(app);
}

}
