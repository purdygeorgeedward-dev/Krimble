/*
 * SPDX-FileCopyrightText: 2025 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KisQQuickWidget.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickItem>
#include <QQmlFileSelector>
#include <QFileSelector>

#include <KLocalizedContext>

#include <KoResourcePaths.h>
#include <KisSurfaceColorSpaceWrapper.h>
#include <kis_config_notifier.h>

KisQQuickWidget::KisQQuickWidget(QWidget *parent): QQuickWidget(parent)
{
#if !defined(Q_OS_MACOS) || QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QSurfaceFormat format;

    {
        format.setRedBufferSize(8);
        format.setGreenBufferSize(8);
        format.setBlueBufferSize(8);
        format.setAlphaBufferSize(8);
        format.setColorSpace(KisSurfaceColorSpaceWrapper::makeSRGBColorSpace());
    }

    setFormat(format);
#endif

    engine()->rootContext()->setContextProperty("mainWindow", parent);
    engine()->rootContext()->setContextObject(new KLocalizedContext(parent));

    setClearColor(palette().window().color());
    connect(KisConfigNotifier::instance(), SIGNAL(signalColorThemeChanged(QString)), this, SLOT(updatePaletteFromConfig()));

    const QString fusion = "Fusion";
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") && QQuickStyle::name() != fusion) {
         QQuickStyle::setStyle(fusion);
    }

    engine()->addImportPath(KoResourcePaths::getApplicationRoot() + "/lib/qml/");
    engine()->addImportPath(KoResourcePaths::getApplicationRoot() + "/lib64/qml/");

    engine()->addPluginPath(KoResourcePaths::getApplicationRoot() + "/lib/qml/");
    engine()->addPluginPath(KoResourcePaths::getApplicationRoot() + "/lib64/qml/");

    QQmlFileSelector* selector = new QQmlFileSelector(engine());
    QStringList extraSelectors;
    extraSelectors << QQuickStyle::name();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    extraSelectors << "qt6";
#elif QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    extraSelectors << "qt5";
#endif
    extraSelectors << "qquickwidget";

    selector->setExtraSelectors(extraSelectors);

    setResizeMode(QQuickWidget::SizeRootObjectToView);
}

KisQQuickWidget::~KisQQuickWidget()
{
    setParent(nullptr);
}

void KisQQuickWidget::connectMinimumHeightToRootObject()
{
    if (rootObject()) {
        connect(rootObject(), SIGNAL(implicitHeightChanged()), this, SLOT(setMinimumHeightFromRoot()));
        setMinimumHeightFromRoot();
    }
}

void KisQQuickWidget::connectMinimumWidthToRootObject()
{
    if (rootObject()) {
        connect(rootObject(), SIGNAL(implicitWidthChanged()), this, SLOT(setMinimumWidthFromRoot()));
        setMinimumWidthFromRoot();
    }
}

void KisQQuickWidget::updatePaletteFromConfig()
{
    setClearColor(palette().window().color());
}

void KisQQuickWidget::setMinimumHeightFromRoot()
{
    if (rootObject()) {
        setMinimumHeight(rootObject()->implicitHeight());
    }
}

void KisQQuickWidget::setMinimumWidthFromRoot()
{
    if (rootObject()) {
        setMinimumWidth(rootObject()->implicitWidth());
    }
}
