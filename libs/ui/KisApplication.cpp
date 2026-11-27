/*
 * SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
 * SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QSplitter>
#include <QHBoxLayout>
#include <QBoxLayout>
#include "KisApplication.h"
#include "KrimbleVisualStyle.h"

#include <stdlib.h>
#ifdef Q_OS_WIN
#include <windows.h>
#include <tchar.h>
#include "KisWindowsPackageUtils.h"
#endif

#ifdef Q_OS_MACOS
#include "osx.h"
#include "KisMacosEntitlements.h"
#endif

#ifdef Q_OS_ANDROID
#include "KisAndroidDonations.h"
#endif

#include <QStandardPaths>
#include <functional>
#ifdef Q_OS_ANDROID
#include <QDialog>
#include <QDockWidget>
#include <QTouchEvent>
#include <QWindow>
#include <QAbstractSpinBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QToolTip>
#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QInputDialog>
#include <QLinearGradient>
#include <QProgressDialog>
#ifndef KRIMBLE_HARNESS
#include "kis_slider_spin_box.h"
#endif
#include <QGuiApplication>
#include <QInputMethod>
#include <QPointer>
#include <QTimer>
#endif
#include <QTabWidget>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QAbstractScrollArea>
#include <QComboBox>
#include <QGroupBox>
#include <QSpacerItem>
#include <QScreen>
#include <QScrollArea>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <KoDialog.h>
#include <QProcessEnvironment>
#include <QStringList>
#include <QStyle>
#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QPainter>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTimer>
#include <QWidget>
#include <QImageReader>
#include <QImageWriter>
#include <QThread>

#include <klocalizedstring.h>
#include <kdesktopfile.h>
#include <kconfig.h>
#include <kconfiggroup.h>

#include <KoDockRegistry.h>
#include <KoToolRegistry.h>
#include <KoColorSpaceRegistry.h>
#include <KoPluginLoader.h>
#include <KoShapeRegistry.h>
#include "KoConfig.h"
#include <QFontDatabase>
#include <QElapsedTimer>
#include <QFileInfo>
#include <KoFontRegistry.h>
#include <KoResourcePaths.h>
#include <KisMimeDatabase.h>
#include "thememanager.h"
#include "KisDocument.h"
#include "KisMainWindow.h"
#include "KisAutoSaveRecoveryDialog.h"
#include "KisPart.h"
#include <kis_icon.h>
#include "kis_splash_screen.h"
#include "kis_config.h"
#include "kis_config_notifier.h"
#include "KisWideDockSeparatorStyle.h"
#include "krimble/KrimbleButtonStyle.h"
#include "krimble/KrimbleScrollBarRidges.h"
#include "krimble/KrimbleMenuColumnDividers.h"
#include "flake/kis_shape_selection.h"
#include <filter/kis_filter.h>
#include <filter/kis_filter_registry.h>
#include <filter/kis_filter_configuration.h>
#include <generator/kis_generator_registry.h>
#include <generator/kis_generator.h>
#include <brushengine/kis_paintop_registry.h>
#include <kis_meta_data_io_backend.h>
#include <kis_meta_data_backend_registry.h>
#include "KisApplicationArguments.h"
#include <kis_debug.h>
#include "kis_action_registry.h"
#include <KoResourceServer.h>
#include <KisResourceServerProvider.h>
#include <KoResourceServerProvider.h>
#include "opengl/kis_opengl.h"
#include "kis_spin_box_unit_manager.h"
#include "kis_document_aware_spin_box_unit_manager.h"
#include "KisViewManager.h"
#include <KisUsageLogger.h>

#include <dialogs/KisSessionManagerDialog.h>

#include <KisResourceCacheDb.h>
#include <KisResourceLocator.h>
#include <KisResourceLoader.h>
#include <KisResourceLoaderRegistry.h>

#include <KisBrushTypeMetaDataFixup.h>
#include <kis_gbr_brush.h>
#include <kis_png_brush.h>
#include <kis_svg_brush.h>
#include <kis_imagepipe_brush.h>
#include <KoColorSet.h>
#include <KoSegmentGradient.h>
#include <KoStopGradient.h>
#include <KoPattern.h>
#include <kis_workspace_resource.h>
#include <KisSessionResource.h>
#include <resources/KoSvgSymbolCollectionResource.h>
#include <resources/KoFontFamily.h>
#include <resources/KoCssStylePreset.h>

#include "widgets/KisScreenColorSampler.h"
#include "KisDlgInternalColorSelector.h"
#include "KisLongPressEventFilter.h"

#include <dialogs/KisAsyncAnimationFramesSaveDialog.h>
#include <kis_image_animation_interface.h>
#include "kis_file_layer.h"
#include "kis_group_layer.h"
#include "kis_node_commands_adapter.h"
#include "KisSynchronizedConnection.h"
#include <QThreadStorage>
#include <KisWindowsPackageUtils.h>

#include <kis_psd_layer_style.h>

#include <config-seexpr.h>
#include <config-safe-asserts.h>

#include <input/KisExtendedModifiersMapperPluginInterface.h>
#include <KisPlatformPluginInterfaceFactory.h>

#include <config-qt-patches-present.h>
#include <config-use-surface-color-management-api.h>

#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API

#include <QWindow>
#include <QPlatformSurfaceEvent>
#include <KisSRGBSurfaceColorSpaceManager.h>

#endif /* KRITA_USE_SURFACE_COLOR_MANAGEMENT_API */

#if defined(Q_OS_ANDROID) && KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
#include <KisAndroidScaling.h>
#endif

namespace {
const QTime appStartTime(QTime::currentTime());
}

namespace {
struct AppRecursionInfo {
    ~AppRecursionInfo() {
        KIS_SAFE_ASSERT_RECOVER_NOOP(!eventRecursionCount);
        KIS_SAFE_ASSERT_RECOVER_NOOP(postponedSynchronizationEvents.empty());
    }

    int eventRecursionCount {0};
    std::queue<KisSynchronizedConnectionEvent> postponedSynchronizationEvents;
};

struct AppRecursionGuard {
    AppRecursionGuard(AppRecursionInfo *info)
        : m_info(info)
    {
        m_info->eventRecursionCount++;
    }

    ~AppRecursionGuard()
    {
        m_info->eventRecursionCount--;
    }
private:
    AppRecursionInfo *m_info {0};
};

}

/**
 * We cannot make the recursion info be a part of KisApplication,
 * because KisApplication also owns QQmlThread, which is destroyed
 * after KisApplication::Private, which makes QThreadStorage spit
 * a warning about being destroyed too early
 */
Q_GLOBAL_STATIC(QThreadStorage<AppRecursionInfo>, s_recursionInfo)

class KrimblePageListDelegate : public QStyledItemDelegate
{
public:
    KrimblePageListDelegate(QAbstractItemDelegate *original, QObject *parent) : QStyledItemDelegate(parent), m_original(original) {}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = m_original ? m_original->sizeHint(option, index) : QStyledItemDelegate::sizeHint(option, index);
        const QWidget *list = option.widget;
        const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(list);
        const int side = qMax(16, view ? view->iconSize().height() : (list ? list->style() : QApplication::style())->pixelMetric(QStyle::PM_IconViewIconSize));
        QFont font = option.font;
        font.setBold(true);
        const int textWidth = qMax(40, size.width() - 4);
        const int textHeight = QFontMetrics(font).boundingRect(QRect(0, 0, textWidth, 10000), Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, index.data(Qt::DisplayRole).toString()).height();
        const int needed = 5 + side + 4 + textHeight + 6;
        if (size.height() < needed) size.setHeight(needed);
        return size;
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        const bool selected = opt.state & QStyle::State_Selected;
        const QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        const QString text = index.data(Qt::DisplayRole).toString();
        // the row's background (the highlight when selected), without the picture or the name
        QStyleOptionViewItem back(opt);
        back.icon = QIcon();
        back.text.clear();
        back.features &= ~QStyleOptionViewItem::HasDecoration;
        back.features &= ~QStyleOptionViewItem::HasDisplay;
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        Q_UNUSED(style);
        painter->save();
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        if (selected) {
            if (qApp->property("krimbleSelectionGradient").toBool()) {
                painter->fillRect(opt.rect, QColor(0x0b, 0x87, 0xdd));
            } else {
                painter->fillRect(opt.rect, QColor(opt.palette.color(QPalette::Active, QPalette::Highlight).rgb()));
            }
        }
        painter->restore();
        const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(opt.widget);
        const int side = qMax(16, qMin(view ? view->iconSize().height() : (opt.widget ? opt.widget->style() : QApplication::style())->pixelMetric(QStyle::PM_IconViewIconSize), opt.rect.width() - 8));
        QFont font = opt.font;
        font.setBold(true);
        const QFontMetrics metrics(font);
        const int textHeight = metrics.height();
        // const QRect iconRect(opt.rect.left() + (opt.rect.width() - side) / 2, opt.rect.top() + 4, side, side);
        const QRect iconRect(opt.rect.left() + (opt.rect.width() - side) / 2, opt.rect.top() + 5, side, side);   // 706th: 5 from the top, as the list's own delegate
        if (!icon.isNull()) {
            QPixmap pix = icon.pixmap(QSize(side, side), QIcon::Normal, QIcon::Off);
            const QSizeF logical = QSizeF(pix.size()) / pix.devicePixelRatio();
            painter->drawPixmap(QPointF(iconRect.left() + (side - logical.width()) / 2.0, iconRect.top() + (side - logical.height()) / 2.0), pix);
        }
        const QRect textRect(opt.rect.left() + 2, iconRect.bottom() + 4, opt.rect.width() - 4, qMax(textHeight, opt.rect.bottom() - iconRect.bottom() - 4));
        painter->save();
        painter->setFont(font);
        painter->setPen(opt.palette.color(QPalette::Active, selected ? QPalette::HighlightedText : QPalette::Text));
        painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, text);
        painter->restore();
    }
private:
    QPointer<QAbstractItemDelegate> m_original;
};

class KrimblePageListStyle : public QProxyStyle
{
public:
    KrimblePageListStyle() : QProxyStyle(QApplication::style()->objectName()) {}
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
    {
        if (metric == QStyle::PM_IconViewIconSize) return 40;
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
};

class KisApplication::Private
{
public:
    Private() {}
    QPointer<KisSplashScreen> splashScreen;
    KisAutoSaveRecoveryDialog *autosaveDialog {0};
    KisLongPressEventFilter *longPressEventFilter {nullptr};
    QPointer<KisMainWindow> mainWindow; // The first mainwindow we create on startup
    bool batchRun {false};
    QVector<QByteArray> earlyRemoteArguments;
    QVector<QString> earlyFileOpenEvents;
    QScopedPointer<KisExtendedModifiersMapperPluginInterface> extendedModifiersPluginInterface;
#ifdef Q_OS_ANDROID
    KisAndroidDonations *androidDonations {nullptr};
#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    KisAndroidScaling *androidScaling {nullptr};
#endif
#endif
};

class KisApplication::ResetStarting
{
public:
    ResetStarting(KisSplashScreen *splash, int fileCount)
        : m_splash(splash)
        , m_fileCount(fileCount)
    {
    }

    ~ResetStarting()  {

        if (m_splash) {
            m_splash->hide();
            m_splash->deleteLater();
        }
    }

    QPointer<KisSplashScreen> m_splash;
    int m_fileCount;
};

#include "krimble/KrimbleDialogs.inc"
#include "krimble/KrimbleNumberFields.inc"
KisApplication::KisApplication(const QString &key, int &argc, char **argv)
    : QtSingleApplication(key, argc, argv)
    , d(new Private)
{
#ifdef Q_OS_ANDROID
    // The hardware renderer backend on Android doesn't support proper stacking,
    // causing windows with QtQuick widgets to always stack behind everything
    // else, including our own dialog decorations.
    qputenv("QT_QUICK_BACKEND", "software");

    installEventFilter(new KrimbleDialogKeeper(this));

    installEventFilter(new KrimbleNumberFields(this));
#endif
#ifdef Q_OS_MACOS
    setMouseCoalescingEnabled(false);
#endif

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());

#ifndef Q_OS_MACOS
    setWindowIcon(KisIconUtils::loadIcon("krita-branding"));
#endif

           // if style is set from config, try to load that
    KisConfig cfg(true);
    QString widgetStyleFromConfig = cfg.widgetStyle();
    QString defaultStyle = style()->objectName().toLower();
    if (!widgetStyleFromConfig.isEmpty()) {
        qApp->setStyle(widgetStyleFromConfig);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    } else if (defaultStyle == "macintosh" || defaultStyle == "windowsvista") {
         // default to Fusion instead of styles that use native theming
        qApp->setStyle("fusion");
    }
#else
    } else if (style()->objectName().toLower() == "macos" || defaultStyle == "windowsvista") {
        // default to Fusion instead of styles that use native theming
        qApp->setStyle("fusion");
    }
#endif

    qApp->setStyle(new KrimbleButtonStyle(new KisWideDockSeparatorStyle(qApp->style())));
    KrimbleScrollBarHooks::install();
    KrimbleMenuColumnDividers::install();
#ifdef Q_OS_ANDROID
    // A single visual skin for dialogs and controls. Do not change window flags,
    // dialog movement, touch behavior, or the customizable toolbox layout.
    qApp->setStyleSheet(krimbleVisualStyleSheet());
#endif

    /**
     * Load platform plugin for modifiers fetching
     */
    {
        d->extendedModifiersPluginInterface.reset(KisPlatformPluginInterfaceFactory::instance()->createExtendedModifiersMapper());
    }

    // store the style name
    qApp->setProperty(currentUnderlyingStyleNameProperty, style()->objectName());
    KisSynchronizedConnectionBase::registerSynchronizedEventBarrier(std::bind(&KisApplication::processPostponedSynchronizationEvents, this));


#if KRITA_USE_SURFACE_COLOR_MANAGEMENT_API

    /**
     * Automatically assign sRGB color space to all Krimble windows,
     * which are not marked with a special tag.
     */
    struct PlatformWindowCreationFilter : QObject
    {
        using QObject::QObject;

        bool eventFilter(QObject *watched, QEvent *event) override {
            if (event->type() == QEvent::PlatformSurface) {
                QWidget *widget = qobject_cast<QWidget*>(watched);
                if (!widget) return false;

                /**
                 * Check for the special tag that is set for the windows that handle
                 * their color space themselves
                 */
                if (watched->property("krita_skip_srgb_surface_manager_assignment").toBool()) {
                    return false;
                }

                QPlatformSurfaceEvent *surfaceEvent = static_cast<QPlatformSurfaceEvent*>(event);
                if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
                    QWindow *nativeWindow = widget->windowHandle();
                    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(widget->windowHandle(), false);

                    if (!nativeWindow->findChild<KisSRGBSurfaceColorSpaceManager*>()) {
                        KisSRGBSurfaceColorSpaceManager::tryCreateForCurrentPlatform(widget);
                    }
                }
            }

            return false;
        }
    };

    this->installEventFilter(new PlatformWindowCreationFilter(this));
#endif /* KRITA_USE_SURFACE_COLOR_MANAGEMENT_API */
}

#if defined(Q_OS_WIN) && defined(ENV32BIT)
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL isWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
                GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(0 != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            //handle error
        }
    }
    return bIsWow64;
}
#endif

void KisApplication::initializeGlobals(const KisApplicationArguments &args)
{
    Q_UNUSED(args)
    // There are no globals to initialize from the arguments now. There used
    // to be the `dpi` argument, but it doesn't do anything anymore.
}

void KisApplication::addResourceTypes()
{
    KoResourcePaths::addAssetType("markers", "data", "/styles/");
    KoResourcePaths::addAssetType("kis_pics", "data", "/pics/");
    KoResourcePaths::addAssetType("kis_images", "data", "/images/");
    KoResourcePaths::addAssetType("metadata_schema", "data", "/metadata/schemas/");
    KoResourcePaths::addAssetType("gmic_definitions", "data", "/gmic/");
    KoResourcePaths::addAssetType("kis_shortcuts", "data", "/shortcuts/");
    KoResourcePaths::addAssetType("kis_actions", "data", "/actions");
    KoResourcePaths::addAssetType("kis_actions", "data", "/pykrita");
    KoResourcePaths::addAssetType("icc_profiles", "data", "/color/icc");
    KoResourcePaths::addAssetType("icc_profiles", "data", "/profiles/");
    KoResourcePaths::addAssetType("tags", "data", "/tags/");
    KoResourcePaths::addAssetType("templates", "data", "/templates");
    KoResourcePaths::addAssetType("pythonscripts", "data", "/pykrita");
    KoResourcePaths::addAssetType("preset_icons", "data", "/preset_icons");
#if defined HAVE_SEEXPR
    KoResourcePaths::addAssetType(ResourceType::SeExprScripts, "data", "/seexpr_scripts/", true);
#endif

    // Make directories for all resources we can save, and tags
    KoResourcePaths::saveLocation("data", "/asl/", true);
    KoResourcePaths::saveLocation("data", "/css_styles/", true);
    KoResourcePaths::saveLocation("data", "/input/", true);
    KoResourcePaths::saveLocation("data", "/pykrita/", true);
    KoResourcePaths::saveLocation("data", "/color-schemes/", true);
    KoResourcePaths::saveLocation("data", "/preset_icons/", true);
    KoResourcePaths::saveLocation("data", "/preset_icons/tool_icons/", true);
    KoResourcePaths::saveLocation("data", "/preset_icons/emblem_icons/", true);
}


bool KisApplication::event(QEvent *event)
{

    #ifdef Q_OS_MACOS
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        fileOpenRequested(openEvent->file());
        return true;
    }
    #endif
    return QApplication::event(event);
}


bool KisApplication::registerResources()
{
    KisResourceLoaderRegistry *reg = KisResourceLoaderRegistry::instance();

    reg->add(new KisResourceLoader<KisPaintOpPreset>(ResourceSubType::KritaPaintOpPresets, ResourceType::PaintOpPresets, i18n("Brush presets"),
                                                     QStringList() << "application/x-krita-paintoppreset"));

    reg->add(new KisResourceLoader<KisGbrBrush>(ResourceSubType::GbrBrushes, ResourceType::Brushes, i18n("Brush tips"), QStringList() << "image/x-gimp-brush"));
    reg->add(new KisResourceLoader<KisImagePipeBrush>(ResourceSubType::GihBrushes, ResourceType::Brushes, i18n("Brush tips"), QStringList() << "image/x-gimp-brush-animated"));
    reg->add(new KisResourceLoader<KisSvgBrush>(ResourceSubType::SvgBrushes, ResourceType::Brushes, i18n("Brush tips"), QStringList() << "image/svg+xml"));
    reg->add(new KisResourceLoader<KisPngBrush>(ResourceSubType::PngBrushes, ResourceType::Brushes, i18n("Brush tips"), QStringList() << "image/png"));

    reg->add(new KisResourceLoader<KoSegmentGradient>(ResourceSubType::SegmentedGradients, ResourceType::Gradients, i18n("Gradients"), QStringList() << "application/x-gimp-gradient"));
    reg->add(new KisResourceLoader<KoStopGradient>(ResourceSubType::StopGradients, ResourceType::Gradients, i18n("Gradients"), QStringList() << "image/svg+xml"));

    reg->add(new KisResourceLoader<KoColorSet>(ResourceType::Palettes, ResourceType::Palettes, i18n("Palettes"),
                                     QStringList() << KisMimeDatabase::mimeTypeForSuffix("kpl")
                                               << KisMimeDatabase::mimeTypeForSuffix("gpl")
                                               << KisMimeDatabase::mimeTypeForSuffix("pal")
                                               << KisMimeDatabase::mimeTypeForSuffix("act")
                                               << KisMimeDatabase::mimeTypeForSuffix("aco")
                                               << KisMimeDatabase::mimeTypeForSuffix("css")
                                               << KisMimeDatabase::mimeTypeForSuffix("colors")
                                               << KisMimeDatabase::mimeTypeForSuffix("xml")
                                               << KisMimeDatabase::mimeTypeForSuffix("sbz")));


    reg->add(new KisResourceLoader<KoPattern>(ResourceType::Patterns, ResourceType::Patterns, i18n("Patterns"), {"application/x-gimp-pattern", "image/x-gimp-pat", "application/x-gimp-pattern", "image/bmp", "image/jpeg", "image/png", "image/tiff"}));
    reg->add(new KisResourceLoader<KisWorkspaceResource>(ResourceType::Workspaces, ResourceType::Workspaces, i18n("Workspaces"), QStringList() << "application/x-krita-workspace"));
    reg->add(new KisResourceLoader<KoSvgSymbolCollectionResource>(ResourceType::Symbols, ResourceType::Symbols, i18n("SVG symbol libraries"), QStringList() << "image/svg+xml"));
    reg->add(new KisResourceLoader<KisWindowLayoutResource>(ResourceType::WindowLayouts, ResourceType::WindowLayouts, i18n("Window layouts"), QStringList() << "application/x-krita-windowlayout"));
    reg->add(new KisResourceLoader<KisSessionResource>(ResourceType::Sessions, ResourceType::Sessions, i18n("Sessions"), QStringList() << "application/x-krita-session"));
    reg->add(new KisResourceLoader<KoGamutMask>(ResourceType::GamutMasks, ResourceType::GamutMasks, i18n("Gamut masks"), QStringList() << "application/x-krita-gamutmasks"));
#if defined HAVE_SEEXPR
    reg->add(new KisResourceLoader<KisSeExprScript>(ResourceType::SeExprScripts, ResourceType::SeExprScripts, i18n("SeExpr Scripts"), QStringList() << "application/x-krita-seexpr-script"));
#endif
    // XXX: this covers only individual styles, not the library itself!
    reg->add(new KisResourceLoader<KisPSDLayerStyle>(ResourceType::LayerStyles,
                                                     ResourceType::LayerStyles,
                                                     i18nc("Resource type name", "Layer styles"),
                                                     QStringList() << "application/x-photoshop-style"));

    reg->add(new KisResourceLoader<KoFontFamily>(ResourceType::FontFamilies, ResourceType::FontFamilies, i18n("Fonts"), QStringList() << "application/x-font-ttf" << "application/x-font-otf"));
    reg->add(new KisResourceLoader<KoCssStylePreset>(ResourceType::CssStyles, ResourceType::CssStyles, i18n("Style Presets"), QStringList() << "image/svg+xml"));

    reg->registerFixup(10, new KisBrushTypeMetaDataFixup());

#ifndef Q_OS_ANDROID
    QString databaseLocation = KoResourcePaths::getAppDataLocation();
#else
    // Sqlite doesn't support content URIs (obviously). So, we make database location unconfigurable on android.
    QString databaseLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif

    if (!KisResourceCacheDb::initialize(databaseLocation)) {
        QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble: Fatal error"), i18n("%1\n\nKrita will quit now.", KisResourceCacheDb::lastError()));
    }

    KisResourceLocator::LocatorError r = KisResourceLocator::instance()->initialize(KoResourcePaths::getApplicationRoot() + "/share/krita");
    connect(KisResourceLocator::instance(), SIGNAL(progressMessage(const QString&)), this, SLOT(setSplashScreenLoadingText(const QString&)));
    if (r != KisResourceLocator::LocatorError::Ok && qApp->inherits("KisApplication")) {
        QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble: Fatal error"), KisResourceLocator::instance()->errorMessages().join('\n') + i18n("\n\nKrita will quit now."));
        return false;
    }
    return true;
}

void KisApplication::loadPlugins()
{
    //    qDebug() << "loadPlugins();";

    KoShapeRegistry* r = KoShapeRegistry::instance();
    r->add(new KisShapeSelectionFactory());
    KoColorSpaceRegistry::instance();
    KisActionRegistry::instance();
    KisFilterRegistry::instance();
    KisGeneratorRegistry::instance();
    KisPaintOpRegistry::instance();
    KoToolRegistry::instance();
    KoDockRegistry::instance();
    KisMetadataBackendRegistry::instance();
}

// Krimble 2026-11-23: the bundled fonts (95 files, about 13 MB) are no longer loaded at every start. They are loaded the first time
// they are needed: when a text tool is chosen, or before a file that can hold text (.kra, .ora, .psd, .svg, ...) is opened
// (George: "we don't need extended fonts until the text tool loads."). The log says how long the loading took.
static void krimbleLoadBundledFonts();
void krimbleEnsureBundledFonts()
{
    static bool loaded = false;
    if (loaded) return;
    loaded = true;
    QElapsedTimer clock;
    clock.start();
    krimbleLoadBundledFonts();
    qInfo() << "Krimble: bundled fonts loaded on first use in" << clock.elapsed() << "ms";
    KisUsageLogger::log(QStringLiteral("Krimble: bundled fonts loaded on first use in %1 ms").arg(clock.elapsed()));
}

static void krimbleLoadBundledFonts()
{
    QStringList files;
    QStringList fontDirs;
    Q_FOREACH (const QString &dir, KoResourcePaths::assetDirs("data")) {
        fontDirs << dir + QStringLiteral("/fonts");
    }
#ifdef Q_OS_ANDROID
    fontDirs << QStringLiteral("assets:/krita/fonts");
    const QString copyDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/krimble-fonts");
    QDir().mkpath(copyDir);
#endif
    Q_FOREACH (const QString &dirPath, fontDirs) {
        const QDir fontDir(dirPath);
        Q_FOREACH (const QString &name, fontDir.entryList(QStringList() << QStringLiteral("*.ttf") << QStringLiteral("*.otf"), QDir::Files)) {
            QString path = fontDir.absoluteFilePath(name);
#ifdef Q_OS_ANDROID
            if (path.startsWith(QLatin1String("assets:"))) {
                const QString target = copyDir + QLatin1Char('/') + name;
                if (QFileInfo(target).size() != QFileInfo(path).size()) {
                    QFile::remove(target);
                    if (!QFile::copy(path, target)) {
                        qWarning() << "Krimble: font could not be copied:" << path;
                        continue;
                    }
                }
                path = target;
            }
#endif
            bool known = false;
            Q_FOREACH (const QString &f, files) {
                if (QFileInfo(f).fileName() == name) { known = true; break; }
            }
            if (!known) files << path;
        }
    }
    int loaded = 0;
    Q_FOREACH (const QString &path, files) {
        const int qtId = QFontDatabase::addApplicationFont(path);
        const bool engineOk = KoFontRegistry::instance()->addBundledFontFile(path);
        if (qtId >= 0 || engineOk) ++loaded;
        else qWarning() << "Krimble: font could not be loaded:" << path;
    }
    qInfo() << "Krimble: bundled fonts found:" << files.size() << "loaded:" << loaded;
}

static void krimbleResetToolOptionsIfWanted()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    if (!config->group(QStringLiteral("Krimble")).readEntry("resetToolOptionsOnStart", false)) return;
    const QStringList groups = config->groupList();
    Q_FOREACH (const QString &name, groups) {
        if (name.contains(QLatin1String("Tool")) && !name.contains(QLatin1String("ToolBox"), Qt::CaseInsensitive)) {
            config->group(name).deleteGroup();
        }
    }
    config->sync();
}

bool KisApplication::start(const KisApplicationArguments &args)
{
#ifdef Q_OS_ANDROID
    KisAndroidDonations::showDonationDialog(true);
#endif

    krimbleResetToolOptionsIfWanted();

    KisConfig cfg(false);

    // krimbleLoadBundledFonts();   // at every start until 2026-11-23; now on first use (krimbleEnsureBundledFonts)

    auto iconsInMenuMode = cfg.iconsInMenu();
    if (iconsInMenuMode == KisConfig::IIM_Yes) {
        QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
    } else if (iconsInMenuMode == KisConfig::IIM_No) {
        QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    }

#if defined(Q_OS_WIN)
#ifdef ENV32BIT

    if (isWow64() && !cfg.readEntry("WarnedAbout32Bits", false)) {
        QMessageBox::information(qApp->activeWindow(),
                                 i18nc("@title:window", "Krimble: Warning"),
                                 i18n("You are running a 32 bits build on a 64 bits Windows.\n"
                                      "This is not recommended.\n"
                                      "Please download and install the x64 build instead."));
        cfg.writeEntry("WarnedAbout32Bits", true);

    }
#endif
#endif

    QString opengl = cfg.canvasState();
    if (opengl == "OPENGL_NOT_TRIED" ) {
        cfg.setCanvasState("TRY_OPENGL");
    }
    else if (opengl != "OPENGL_SUCCESS" && opengl != "TRY_OPENGL") {
        cfg.setCanvasState("OPENGL_FAILED");
    }

    setSplashScreenLoadingText(i18n("Initializing Globals..."));
    processEvents();
    initializeGlobals(args);

#if defined(Q_OS_ANDROID) && KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    d->androidScaling = new KisAndroidScaling(cfg, this);
#endif

    const bool doNewImage = args.doNewImage();
    const bool doTemplate = args.doTemplate();
    const bool exportAs = args.exportAs();
    const bool exportSequence = args.exportSequence();
    const QString exportFileName = args.exportFileName();

    d->batchRun = (exportAs || exportSequence || !exportFileName.isEmpty());
    const bool needsMainWindow = (!exportAs && !exportSequence);
    // only show the mainWindow when no command-line mode option is passed
    bool showmainWindow = (!exportAs && !exportSequence); // would be !batchRun;

#ifndef Q_OS_ANDROID
    const bool showSplashScreen = !d->batchRun && qEnvironmentVariableIsEmpty("NOSPLASH");
    if (showSplashScreen && d->splashScreen) {
        d->splashScreen->show();
        d->splashScreen->repaint();
        processEvents();
    }
#endif

    KConfigGroup group(KSharedConfig::openConfig(), "theme");
#ifndef Q_OS_HAIKU
    Digikam::ThemeManager themeManager;
    themeManager.setCurrentTheme(group.readEntry("Theme", "Blue-Gray"));
#endif

    ResetStarting resetStarting(d->splashScreen, args.filenames().count()); // remove the splash when done
    Q_UNUSED(resetStarting);

    // Make sure we can save resources and tags
    setSplashScreenLoadingText(i18n("Adding resource types..."));
    processEvents();
    addResourceTypes();

    setSplashScreenLoadingText(i18n("Loading plugins..."));
    processEvents();
    // Load the plugins
    loadPlugins();

    // Load all resources
    setSplashScreenLoadingText(i18n("Loading resources..."));
    processEvents();
    if (!registerResources()) {
        return false;
    }

    KisPart *kisPart = KisPart::instance();
    if (needsMainWindow) {
        // show a mainWindow asap, if we want that
        setSplashScreenLoadingText(i18n("Loading Main Window..."));
        processEvents();


        bool sessionNeeded = true;
        auto sessionMode = cfg.sessionOnStartup();

        if (!args.session().isEmpty()) {
            sessionNeeded = !kisPart->restoreSession(args.session());
        } else if (sessionMode == KisConfig::SOS_ShowSessionManager) {
            showmainWindow = false;
            sessionNeeded = false;
            kisPart->showSessionManager();
        } else if (sessionMode == KisConfig::SOS_PreviousSession) {
            KConfigGroup sessionCfg = KSharedConfig::openConfig()->group("session");
            const QString &sessionName = sessionCfg.readEntry("previousSession");

            sessionNeeded = !kisPart->restoreSession(sessionName);
        }

        if (sessionNeeded) {
            kisPart->startBlankSession();
        }

        if (!args.windowLayout().isEmpty()) {
            KoResourceServer<KisWindowLayoutResource> * rserver = KisResourceServerProvider::instance()->windowLayoutServer();
            KisWindowLayoutResourceSP windowLayout = rserver->resource("", "", args.windowLayout());
            if (windowLayout) {
                windowLayout->applyLayout();
            }
        }

        setSplashScreenLoadingText(i18n("Launching..."));

        if (showmainWindow) {
            d->mainWindow = kisPart->currentMainwindow();

            if (!args.workspace().isEmpty()) {
                KoResourceServer<KisWorkspaceResource> * rserver = KisResourceServerProvider::instance()->workspaceServer();
                KisWorkspaceResourceSP workspace = rserver->resource("", "", args.workspace());
                if (workspace) {
                    d->mainWindow->restoreWorkspace(workspace);
                }
            }

            if (args.workspace().isEmpty()) {
                KConfigGroup krimbleGroup = KSharedConfig::openConfig()->group("Krimble");
                if (!krimbleGroup.readEntry("DefaultWorkspaceApplied", false)) {
                    KoResourceServer<KisWorkspaceResource> * rserver = KisResourceServerProvider::instance()->workspaceServer();
                    KisWorkspaceResourceSP workspace = rserver->resource("", "", "Default");
                    if (workspace) {
                        d->mainWindow->restoreWorkspace(workspace);
                    }
                    krimbleGroup.writeEntry("DefaultWorkspaceApplied", true);
                    krimbleGroup.sync();
                }
            }

            if (args.canvasOnly()) {
                d->mainWindow->viewManager()->switchCanvasOnly(true);
            }

            if (args.fullScreen()) {
                d->mainWindow->showFullScreen();
            }
        } else {
            d->mainWindow = kisPart->createMainWindow();
        }
    }

/*
// Check for autosave files that can be restored, if we're not running a batch run (test)
if (!d->batchRun) {
    checkAutosaveFiles();
}
*/

    setSplashScreenLoadingText(QString()); // done loading, so clear out label
#ifdef Q_OS_ANDROID
    KisAndroidDonations::setLoaded(true);
#endif
    processEvents();

    //configure the unit manager
    KisSpinBoxUnitManagerFactory::setDefaultUnitManagerBuilder(new KisDocumentAwareSpinBoxUnitManagerBuilder());
    connect(this, &KisApplication::aboutToQuit, &KisSpinBoxUnitManagerFactory::clearUnitManagerBuilder); //ensure the builder is destroyed when the application leave.
    //the new syntax slot syntax allow to connect to a non q_object static method.

    // Long-press emulation.
    KisConfigNotifier *cfgNotifier = KisConfigNotifier::instance();
    connect(cfgNotifier, &KisConfigNotifier::sigLongPressChanged, this, &KisApplication::slotSetLongPress);
    slotSetLongPress(cfg.longPressEnabled());

    // Xiaomi workaround: their stylus inexplicably inputs page up and down keys
    // when pressing stylus buttons. This flag causes the Android platform
    // integration to turn those into right and middle clicks instead.
#if KRITA_QT_HAS_ANDROID_EMULATE_MOUSE_BUTTONS_FOR_PAGE_UP_DOWN
    auto setPageUpDownMouseButtonEmulationWorkaround = [](bool enabled) {
        QCoreApplication::setKritaAttribute(KRITA_QATTRIBUTE_ANDROID_EMULATE_MOUSE_BUTTONS_FOR_PAGE_UP_DOWN, enabled);
    };
    connect(cfgNotifier,
            &KisConfigNotifier::sigUsePageUpDownMouseButtonEmulationWorkaroundChanged,
            this,
            setPageUpDownMouseButtonEmulationWorkaround);
    setPageUpDownMouseButtonEmulationWorkaround(cfg.usePageUpDownMouseButtonEmulationWorkaround());
#endif

#if KRITA_QT_HAS_ANDROID_EMULATE_MOUSE_BUTTONS_FOR_HIGH_FUNCTION_KEYS
    QCoreApplication::setKritaAttribute(KRITA_QATTRIBUTE_ANDROID_EMULATE_MOUSE_BUTTONS_FOR_HIGH_FUNCTION_KEYS, true);
    auto setHighFunctionKeyMouseButtonEmulationWorkaround = [](bool enabled) {
        // QCoreApplication::setKritaAttribute(KRITA_QATTRIBUTE_ANDROID_EMULATE_MOUSE_BUTTONS_FOR_HIGH_FUNCTION_KEYS, enabled);
    };
    connect(cfgNotifier,
            &KisConfigNotifier::sigUseHighFunctionKeyMouseButtonEmulationWorkaroundChanged,
            this,
            setHighFunctionKeyMouseButtonEmulationWorkaround);
    setHighFunctionKeyMouseButtonEmulationWorkaround(cfg.useHighFunctionKeyMouseButtonEmulationWorkaround());
#endif

    // Xiaomi workaround: historic tablet motion events are garbage, they just
    // connect the actual points that the tablet sampled with a straight line
    // and no pressure emulation, leading to jagged curves that don't get
    // smoothed out. This flag disables reading those historic events.
#if KRITA_QT_HAS_ANDROID_IGNORE_HISTORIC_TABLET_EVENTS
    auto setIgnoreHistoricTabletEventsWorkaround = [](bool enabled) {
        QCoreApplication::setKritaAttribute(KRITA_QATTRIBUTE_ANDROID_IGNORE_HISTORIC_TABLET_EVENTS, enabled);
    };
    connect(cfgNotifier,
            &KisConfigNotifier::sigUseIgnoreHistoricTabletEventsWorkaroundChanged,
            this,
            setIgnoreHistoricTabletEventsWorkaround);
    setIgnoreHistoricTabletEventsWorkaround(cfg.useIgnoreHistoricTabletEventsWorkaround());
#endif

    // Create a new image, if needed
    if (doNewImage) {
        KisDocument *doc = args.createDocumentFromArguments();
        if (doc) {
            kisPart->addDocument(doc);
            d->mainWindow->addViewAndNotifyLoadingCompleted(doc);
        }
    }

    // Get the command line arguments which we have to parse
    int argsCount = args.filenames().count();
    if (argsCount > 0) {
        // Loop through arguments
        for (int argNumber = 0; argNumber < argsCount; argNumber++) {
            QString fileName = args.filenames().at(argNumber);
            // are we just trying to open a template?
            if (doTemplate) {
                // called in mix with batch options? ignore and silently skip
                if (d->batchRun) {
                    continue;
                }
                createNewDocFromTemplate(fileName, d->mainWindow);
                // now try to load
            }
            else {
                if (exportAs) {
                    QString outputMimetype = KisMimeDatabase::mimeTypeForFile(exportFileName, false);
                    if (outputMimetype == "application/octetstream") {
                        dbgKrita << i18n("Mimetype not found, try using the -mimetype option") << Qt::endl;
                        return false;
                    }

                    KisDocument *doc = kisPart->createDocument();
                    doc->setFileBatchMode(d->batchRun);
                    bool result = doc->openPath(fileName);

                    if (!result) {
                        errKrita << "Could not load " << fileName << ":" << doc->errorMessage();
                        QTimer::singleShot(0, this, SLOT(quit()));
                        return false;
                    }

                    if (exportFileName.isEmpty()) {
                        errKrita << "Export destination is not specified for" << fileName << "Please specify export destination with --export-filename option";
                        QTimer::singleShot(0, this, SLOT(quit()));
                        return false;
                    }

                    qApp->processEvents(); // For vector layers to be updated

                    doc->setFileBatchMode(true);
                    doc->image()->waitForDone();

                    if (!doc->exportDocumentSync(exportFileName, outputMimetype.toLatin1())) {
                        errKrita << "Could not export " << fileName << "to" << exportFileName << ":" << doc->errorMessage();
                    }
                    QTimer::singleShot(0, this, SLOT(quit()));
                    return true;
                }
                else if (exportSequence) {
                    KisDocument *doc = kisPart->createDocument();
                    doc->setFileBatchMode(d->batchRun);
                    doc->openPath(fileName);
                    qApp->processEvents(); // For vector layers to be updated
                    
                    if (!doc->image()->animationInterface()->hasAnimation()) {
                        errKrita << "This file has no animation." << Qt::endl;
                        QTimer::singleShot(0, this, SLOT(quit()));
                        return false;
                    }

                    doc->setFileBatchMode(true);
                    int sequenceStart = 0;


                    qDebug() << ppVar(exportFileName);
                    KisAsyncAnimationFramesSaveDialog exporter(doc->image(),
                                               doc->image()->animationInterface()->documentPlaybackRange(),
                                               exportFileName,
                                               sequenceStart,
                                               false,
                                               0);

                    exporter.setBatchMode(d->batchRun);

                    KisAsyncAnimationFramesSaveDialog::Result result = exporter.regenerateRange(nullptr);
                    qDebug() << ppVar(result);

                    if (result != KisAsyncAnimationFramesSaveDialog::RenderComplete) {
                        errKrita << i18n("Failed to render animation frames!") << Qt::endl;
                    }

                    QTimer::singleShot(0, this, SLOT(quit()));
                    return true;
                }
                else if (d->mainWindow) {
                    if (QFileInfo(fileName).fileName().endsWith(".bundle", Qt::CaseInsensitive)) {
                        d->mainWindow->installBundle(fileName);
                    }
                    else {
                        KisMainWindow::OpenFlags flags = d->batchRun ? KisMainWindow::BatchMode : KisMainWindow::None;

                        d->mainWindow->openDocument(fileName, flags);
                    }
                }
            }
        }
    }

    //add an image as file-layer
    if (!args.fileLayer().isEmpty()){
        if (d->mainWindow->viewManager()->image()){
            KisFileLayer *fileLayer = new KisFileLayer(d->mainWindow->viewManager()->image(), "",
                                                    args.fileLayer(), KisFileLayer::None, "Bicubic",
                                                    d->mainWindow->viewManager()->image()->nextLayerName(i18n("File layer")), OPACITY_OPAQUE_U8);
            QFileInfo fi(fileLayer->path());
            if (fi.exists()){
                KisNodeCommandsAdapter adapter(d->mainWindow->viewManager());
                adapter.addNode(fileLayer, d->mainWindow->viewManager()->activeNode()->parent(),
                                    d->mainWindow->viewManager()->activeNode());
            }
            else{
                QMessageBox::warning(qApp->activeWindow(), i18nc("@title:window", "Krimble:Warning"),
                                            i18n("Cannot add %1 as a file layer: the file does not exist.", fileLayer->path()));
            }
        }
        else if (this->isRunning()){
            QMessageBox::warning(qApp->activeWindow(), i18nc("@title:window", "Krimble:Warning"),
                                i18n("Cannot add the file layer: no document is open.\n\n"
"You can create a new document using the --new-image option, or you can open an existing file.\n\n"
"If you instead want to add the file layer to a document in an already running instance of Krimble, check the \"Allow only one instance of Krimble\" checkbox in the settings (Settings -> General -> Window)."));
        }
        else {
            QMessageBox::warning(qApp->activeWindow(), i18nc("@title:window", "Krimble: Warning"),
                                i18n("Cannot add the file layer: no document is open.\n"
                                     "You can either create a new file using the --new-image option, or you can open an existing file."));
        }
    }

    if (d->splashScreen) {
        d->splashScreen->displayLinks(true);
        d->splashScreen->displayRecentFiles(true);
    }

    Q_FOREACH(const QByteArray &message, d->earlyRemoteArguments) {
        executeRemoteArguments(message, d->mainWindow);
    }

    KisUsageLogger::writeSysInfo(KisUsageLogger::screenInformation());

    // process File open event files
    if (!d->earlyFileOpenEvents.isEmpty()) {
        hideSplashScreen();
        Q_FOREACH(QString fileName, d->earlyFileOpenEvents) {
            d->mainWindow->openDocument(fileName, QFlags<KisMainWindow::OpenFlag>());
        }
    }

    verifyMetatypeRegistration();

    // not calling this before since the program will quit there.
    return true;
}

KisApplication::~KisApplication()
{
    if (!isRunning()) {
        KisResourceCacheDb::deleteTemporaryResources();
        KisResourceCacheDb::performHouseKeepingOnExit();
    }
}

void KisApplication::setSplashScreen(QWidget *splashScreen)
{
    d->splashScreen = qobject_cast<KisSplashScreen*>(splashScreen);
}

void KisApplication::setSplashScreenLoadingText(const QString &textToLoad)
{
    if (d->splashScreen) {
        d->splashScreen->setLoadingText(textToLoad);
        d->splashScreen->repaint();
    }
#ifdef Q_OS_ANDROID
    KisAndroidDonations::setLoadingText(textToLoad);
#endif
}

void KisApplication::hideSplashScreen()
{
#ifdef Q_OS_ANDROID
    KisAndroidDonations::setLoaded(true);
#endif
    if (d->splashScreen) {
        // hide the splashscreen to see the dialog
        d->splashScreen->hide();
    }
}


bool KisApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        bool result = true;

        /**
         * KisApplication::notify() is called for every event loop processed in
         * any thread, so we need to make sure our counters and postponed events
         * queues are stored in a per-thread way.
         */
        AppRecursionInfo &info = s_recursionInfo->localData();

        {
            // QApplication::notify() can throw, so use RAII for counters
            AppRecursionGuard guard(&info);

            if (event->type() == KisSynchronizedConnectionBase::eventType()) {

                if (info.eventRecursionCount > 1) {
                    KisSynchronizedConnectionEvent *typedEvent = static_cast<KisSynchronizedConnectionEvent*>(event);
                    KIS_SAFE_ASSERT_RECOVER_NOOP(typedEvent->destination == receiver);

                    info.postponedSynchronizationEvents.emplace(KisSynchronizedConnectionEvent(*typedEvent));
                } else {
                    result = QApplication::notify(receiver, event);
                }
            } else {
                result = QApplication::notify(receiver, event);
            }
        }

        if (!info.eventRecursionCount) {
            processPostponedSynchronizationEvents();

        }

        return result;

    } catch (std::exception &e) {
        qWarning("Error %s sending event %i to object %s",
                 e.what(), event->type(), qPrintable(receiver->objectName()));
    } catch (...) {
        qWarning("Error <unknown> sending event %i to object %s",
                 event->type(), qPrintable(receiver->objectName()));
    }
    return false;
}

void KisApplication::processPostponedSynchronizationEvents()
{
    AppRecursionInfo &info = s_recursionInfo->localData();

    while (!info.postponedSynchronizationEvents.empty()) {
        // QApplication::notify() can throw, so use RAII for counters
        AppRecursionGuard guard(&info);

        /// We must pop event from the queue **before** we call
        /// QApplication::notify(), because it can throw!
        KisSynchronizedConnectionEvent typedEvent = info.postponedSynchronizationEvents.front();
        info.postponedSynchronizationEvents.pop();

        if (!typedEvent.destination) {
            qWarning() << "WARNING: the destination object of KisSynchronizedConnection has been destroyed during postponed delivery";
            continue;
        }

        QApplication::notify(typedEvent.destination, &typedEvent);
    }
}

bool KisApplication::isStoreApplication()
{
    if (qEnvironmentVariableIsSet("STEAMAPPID") || qEnvironmentVariableIsSet("SteamAppId")) {
        return true;
    }

    if (applicationDirPath().toLower().contains("steam")) {
        return true;
    }

#ifdef Q_OS_WIN
    // This is also true for user-installed MSIX, but that's
    // likely only true in institutional situations, where
    // we don't want to show the beginning banner either.
    if (KisWindowsPackageUtils::isRunningInPackage()) {
        return true;
    }
#endif

#ifdef Q_OS_MACOS
    KisMacosEntitlements entitlements;
    if (entitlements.sandbox()) {
       return true;
    }
#endif

    return false;
}

void KisApplication::verifyMetatypeRegistration()
{
    /**
     * Verify that all our statically registered types are actually registered.
     * This check is skipped in release builds, when HIDE_SAFE_ASSERTS is defined
     */
#if !defined(HIDE_SAFE_ASSERTS) || defined(CRASH_ON_SAFE_ASSERTS)

    auto verifyTypeRegistered = [] (const char *type) {
        const int typeId = QMetaType::type(type);

        if (typeId <= 0) {
            qFatal("ERROR: type-id for metatype %s is not found", type);
        }

        if (!QMetaType::isRegistered(typeId)) {
            qFatal("ERROR: metatype %s is not registered", type);
        }
    };

    verifyTypeRegistered("KisBrushSP");
    verifyTypeRegistered("KoSvgText::AutoValue");
    verifyTypeRegistered("KoSvgText::BackgroundProperty");
    verifyTypeRegistered("KoSvgText::StrokeProperty");
    verifyTypeRegistered("KoSvgText::TextTransformInfo");
    verifyTypeRegistered("KoSvgText::TextIndentInfo");
    verifyTypeRegistered("KoSvgText::TabSizeInfo");
    verifyTypeRegistered("KoSvgText::LineHeightInfo");
    verifyTypeRegistered("KisPaintopLodLimitations");
    verifyTypeRegistered("KisImageSP");
    verifyTypeRegistered("KisImageSignalType");
    verifyTypeRegistered("KisNodeSP");
    verifyTypeRegistered("KisNodeList");
    verifyTypeRegistered("KisPaintDeviceSP");
    verifyTypeRegistered("KisTimeSpan");
    verifyTypeRegistered("KoColor");
    verifyTypeRegistered("KoResourceSP");
    verifyTypeRegistered("KoResourceCacheInterfaceSP");
    verifyTypeRegistered("KisAsyncAnimationRendererBase::CancelReason");
    verifyTypeRegistered("KisGridConfig");
    verifyTypeRegistered("KisGuidesConfig");
    verifyTypeRegistered("KisUpdateInfoSP");
    verifyTypeRegistered("KisToolChangesTrackerDataSP");
    verifyTypeRegistered("QVector<QImage>");
    verifyTypeRegistered("SnapshotDirInfoList");
    verifyTypeRegistered("TransformTransactionProperties");
    verifyTypeRegistered("ToolTransformArgs");
    verifyTypeRegistered("QPainterPath");
#endif
}

void KisApplication::executeRemoteArguments(QByteArray message, KisMainWindow *mainWindow)
{
    KisApplicationArguments args = KisApplicationArguments::deserialize(message);
    const bool doTemplate = args.doTemplate();
    const bool doNewImage = args.doNewImage();
    const int argsCount = args.filenames().count();
    bool documentCreated = false;

    // Create a new image, if needed
    if (doNewImage) {
        KisDocument *doc = args.createDocumentFromArguments();
        if (doc) {
            KisPart::instance()->addDocument(doc);
            d->mainWindow->addViewAndNotifyLoadingCompleted(doc);
        }
    }
    if (argsCount > 0) {
        // Loop through arguments
        for (int argNumber = 0; argNumber < argsCount; ++argNumber) {
            QString filename = args.filenames().at(argNumber);
            // are we just trying to open a template?
            if (doTemplate) {
                documentCreated |= createNewDocFromTemplate(filename, mainWindow);
            }
            else if (QFile(filename).exists()) {
                KisMainWindow::OpenFlags flags = d->batchRun ? KisMainWindow::BatchMode : KisMainWindow::None;
                documentCreated |= mainWindow->openDocument(filename, flags);
            }
        }
    }

    //add an image as file-layer if called in another process and singleApplication is enabled
    if (!args.fileLayer().isEmpty()){
        if (argsCount > 0  && !documentCreated){
            //arg was passed but document was not created so don't add the file layer.
            QMessageBox::warning(mainWindow, i18nc("@title:window", "Krimble:Warning"),
                                            i18n("Couldn't open file %1",args.filenames().at(argsCount - 1)));
        }
        else if (mainWindow->viewManager()->image()){
            KisFileLayer *fileLayer = new KisFileLayer(mainWindow->viewManager()->image(), "",
                                                    args.fileLayer(), KisFileLayer::None, "Bicubic",
                                                    mainWindow->viewManager()->image()->nextLayerName(i18n("File layer")), OPACITY_OPAQUE_U8);
            QFileInfo fi(fileLayer->path());
            if (fi.exists()){
                KisNodeCommandsAdapter adapter(d->mainWindow->viewManager());
                adapter.addNode(fileLayer, d->mainWindow->viewManager()->activeNode()->parent(),
                                    d->mainWindow->viewManager()->activeNode());
            }
            else{
                QMessageBox::warning(mainWindow, i18nc("@title:window", "Krimble:Warning"),
                                            i18n("Cannot add %1 as a file layer: the file does not exist.", fileLayer->path()));
            }
        }
        else {
            QMessageBox::warning(mainWindow, i18nc("@title:window", "Krimble:Warning"),
                                            i18n("Cannot add the file layer: no document is open."));
        }
    }
}


void KisApplication::remoteArguments(const QString &message)
{
    // check if we have any mainwindow
    KisMainWindow *mw = qobject_cast<KisMainWindow*>(qApp->activeWindow());

    if (!mw && KisPart::instance()->mainWindows().size() > 0) {
        mw = KisPart::instance()->mainWindows().first();
    }

    const QByteArray unpackedMessage =
        QByteArray::fromBase64(message.toLatin1());

    if (!mw) {
        d->earlyRemoteArguments << unpackedMessage;
        return;
    }
    executeRemoteArguments(unpackedMessage, mw);
}

void KisApplication::fileOpenRequested(const QString &url)
{
    if (!d->mainWindow) {
        d->earlyFileOpenEvents.append(url);
        return;
    }

    KisMainWindow::OpenFlags flags = d->batchRun ? KisMainWindow::BatchMode : KisMainWindow::None;
    d->mainWindow->openDocument(url, flags);
}


void KisApplication::slotSetLongPress(bool enabled)
{
    if (enabled && !d->longPressEventFilter) {
        d->longPressEventFilter = new KisLongPressEventFilter(this);
        installEventFilter(d->longPressEventFilter);
    } else if (!enabled && d->longPressEventFilter) {
        removeEventFilter(d->longPressEventFilter);
        d->longPressEventFilter->deleteLater();
        d->longPressEventFilter = nullptr;
    }
}

void KisApplication::checkAutosaveFiles()
{
    if (d->batchRun) return;

    QDir dir = KisAutoSaveRecoveryDialog::autoSaveLocation();

    // Check for autosave files from a previous run. There can be several, and
    // we want to offer a restore for every one. Including a nice thumbnail!

    // Hidden autosave files
    QStringList filters = QStringList() << QString(".krita-*-*-autosave.kra");

    // all autosave files for our application
    QStringList autosaveFiles = dir.entryList(filters, QDir::Files | QDir::Hidden);

    // Visible autosave files
    filters = QStringList() << QString("krita-*-*-autosave.kra");
    autosaveFiles += dir.entryList(filters, QDir::Files);

    // Allow the user to make their selection
    if (autosaveFiles.size() > 0) {
        if (d->splashScreen) {
            // hide the splashscreen to see the dialog
            hideSplashScreen();
        }
        d->autosaveDialog = new KisAutoSaveRecoveryDialog(autosaveFiles, activeWindow());
        QDialog::DialogCode result = (QDialog::DialogCode) d->autosaveDialog->exec();

        if (result == QDialog::Accepted) {
            QStringList filesToRecover = d->autosaveDialog->recoverableFiles();
            Q_FOREACH (const QString &autosaveFile, autosaveFiles) {
                if (!filesToRecover.contains(autosaveFile)) {
                    KisUsageLogger::log(QString("Removing autosave file %1").arg(dir.absolutePath() + "/" + autosaveFile));
                    QFile::remove(dir.absolutePath() + "/" + autosaveFile);
                }
            }
            autosaveFiles = filesToRecover;
        } else {
            autosaveFiles.clear();
        }

        if (autosaveFiles.size() > 0) {
            QList<QString> autosavePaths;
            Q_FOREACH (const QString &autoSaveFile, autosaveFiles) {
                const QString path = dir.absolutePath() + QLatin1Char('/') + autoSaveFile;
                autosavePaths << path;
            }
            if (d->mainWindow) {
                Q_FOREACH (const QString &path, autosavePaths) {
                    KisMainWindow::OpenFlags flags = d->batchRun ? KisMainWindow::BatchMode : KisMainWindow::None;
                    d->mainWindow->openDocument(path, flags | KisMainWindow::RecoveryFile);
                }
            }
        }
        // cleanup
        delete d->autosaveDialog;
        d->autosaveDialog = nullptr;
    }
}

bool KisApplication::createNewDocFromTemplate(const QString &fileName, KisMainWindow *mainWindow)
{
    QString templatePath;

    if (QFile::exists(fileName)) {
        templatePath = fileName;
        dbgUI << "using full path...";
    }
    else {
        QString desktopName(fileName);
        const QString templatesResourcePath =  QStringLiteral("templates/");

        QStringList paths = KoResourcePaths::findAllAssets("data", templatesResourcePath + "*/" + desktopName);
        if (paths.isEmpty()) {
            paths = KoResourcePaths::findAllAssets("data", templatesResourcePath + desktopName);
        }

        if (paths.isEmpty()) {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"),
                                  i18n("No template found for: %1", desktopName));
        } else if (paths.count() > 1) {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"),
                                  i18n("Too many templates found for: %1", desktopName));
        } else {
            templatePath = paths.at(0);
        }
    }

    if (!templatePath.isEmpty()) {
        KDesktopFile templateInfo(templatePath);

        KisMainWindow::OpenFlags batchFlags = d->batchRun ? KisMainWindow::BatchMode : KisMainWindow::None;
        if (mainWindow->openDocument(templatePath, KisMainWindow::Import | batchFlags)) {
            dbgUI << "Template loaded...";
            return true;
        }
        else {
            QMessageBox::critical(qApp->activeWindow(), i18nc("@title:window", "Krimble"),
                                  i18n("Template %1 failed to load.", fileName));
        }
    }

    return false;
}

void KisApplication::resetConfig()
{
    KIS_ASSERT_RECOVER_RETURN(qApp->thread() == QThread::currentThread());

    KSharedConfigPtr config =  KSharedConfig::openConfig();
    config->markAsClean();
    
    // find user settings file
    const QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    QString kritarcPath = configPath + QStringLiteral("/kritarc");
    
    QFile kritarcFile(kritarcPath);
    
    if (kritarcFile.exists()) {
        if (kritarcFile.open(QFile::ReadWrite)) {
            QString backupKritarcPath = kritarcPath + QStringLiteral(".backup");
    
            QFile backupKritarcFile(backupKritarcPath);
    
            if (backupKritarcFile.exists()) {
                backupKritarcFile.remove();
            }

            QMessageBox::information(qApp->activeWindow(),
                                 i18nc("@title:window", "Krimble"),
                                 i18n("Krimble configurations reset!\n\n"
                                      "Backup file was created at: %1\n\n"
                                      "Restart Krimble for changes to take effect.",
                                      backupKritarcPath),
                                 QMessageBox::Ok, QMessageBox::Ok);

            // clear file
            kritarcFile.rename(backupKritarcPath);

            kritarcFile.close();
        }
        else {
            QMessageBox::warning(qApp->activeWindow(),
                                 i18nc("@title:window", "Krimble"),
                                 i18n("Failed to clear %1\n\n"
                                      "Please make sure no other program is using the file and try again.",
                                      kritarcPath),
                                 QMessageBox::Ok, QMessageBox::Ok);
        }
    }

    // reload from disk; with the user file settings cleared,
    // this should load any default configuration files shipping with the program
    config->reparseConfiguration();
    config->sync();

    // Restore to default workspace
    KConfigGroup cfg = KSharedConfig::openConfig()->group("MainWindow");

    QString currentWorkspace = cfg.readEntry<QString>("CurrentWorkspace", "Default");
    KoResourceServer<KisWorkspaceResource> * rserver = KisResourceServerProvider::instance()->workspaceServer();
    KisWorkspaceResourceSP workspace = rserver->resource("", "", currentWorkspace);

    if (workspace) {
        d->mainWindow->restoreWorkspace(workspace);
    }
}

void KisApplication::askResetConfig()
{
    bool ok = QMessageBox::question(qApp->activeWindow(),
                                    i18nc("@title:window", "Krimble"),
                                    i18n("Do you want to clear the settings file?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes;
    if (ok) {
        resetConfig();
    }
}

KisExtendedModifiersMapperPluginInterface* KisApplication::extendedModifiersPluginInterface()
{
    return d->extendedModifiersPluginInterface.data();
}

#ifdef Q_OS_ANDROID
KisAndroidDonations *KisApplication::androidDonations()
{
    if (!d->androidDonations) {
        d->androidDonations = new KisAndroidDonations(this);
        d->androidDonations->syncState();
    }
    return d->androidDonations;
}

KisAndroidScaling *KisApplication::androidScaling()
{
#if KRITA_QT_HAS_ANDROID_QPLATFORMSCREEN_DENSITY_ADJUSTMENT
    // Should get initialized during startup and not accessed before.
    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(d->androidScaling, nullptr);
    return d->androidScaling;
#else
    return nullptr;
#endif
}
#endif
