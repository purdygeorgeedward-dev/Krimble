/*
 *  SPDX-FileCopyrightText: 2014 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2021 Alvin Wong <alvin@alvinhc.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "kis_splash_screen.h"

#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QGuiApplication>
#include <QPainter>
#include <QCheckBox>
#include <QDate>
#include <kis_debug.h>
#include <QFile>
#include <QScreen>
#include <QWindow>
#include <QSvgWidget>

#include <KisPart.h>
#include <KisApplication.h>

#include <kis_icon.h>

#include <klocalizedstring.h>
#include <kconfig.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#ifdef Q_OS_MACOS
#include "libs/macosutils/KisMacosEntitlements.h"
#endif

static void addDropShadow(QWidget *widget)
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(widget);
    effect->setBlurRadius(4);
    effect->setOffset(0.5);
    effect->setColor(QColor(0, 0, 0, 255));
    widget->setGraphicsEffect(effect);
}

KisSplashScreen::KisSplashScreen(bool themed, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, Qt::SplashScreen | Qt::FramelessWindowHint | f)
      , m_themed(themed)
      , m_versionHtml(qApp->applicationVersion().toHtmlEscaped())
{

    setupUi(this);
#ifndef Q_OS_MACOS
    setWindowIcon(KisIconUtils::loadIcon("krita-branding"));
#endif

    m_loadingTextLabel = new QLabel(lblSplash);
    m_loadingTextLabel->setTextFormat(Qt::RichText);
    m_loadingTextLabel->setStyleSheet(QStringLiteral("QLabel { color: #fff; background-color: transparent; }"));
    m_loadingTextLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    addDropShadow(m_loadingTextLabel);

    // KRIMBLE 2026-10-06: the logo (orange K with paw) and the "KRITA" wordmark (banner.svg, now removed from the
    // source) are no longer drawn on the splash / About screen (George: "Remove the Krita logo completely from the
    // source and keep that paw icon out of that screen"). Original lines:
    // m_brandingSvg = new QSvgWidget(QStringLiteral(":/krita-branding.svgz"), lblSplash);
    // m_bannerSvg = new QSvgWidget(QStringLiteral(":/splash/banner.svg"), lblSplash);
    // addDropShadow(m_bannerSvg);
    m_brandingSvg = nullptr;
    m_bannerSvg = nullptr;

    m_artCreditsLabel = new QLabel(lblSplash);
    m_artCreditsLabel->setTextFormat(Qt::PlainText);
    m_artCreditsLabel->setStyleSheet(QStringLiteral("QLabel { color: #fff; background-color: transparent; font: 10pt; }"));
    m_artCreditsLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    addDropShadow(m_artCreditsLabel);

    updateSplashImage();
    setLoadingText(QString());

    bnClose->hide();
    connect(bnClose, SIGNAL(clicked()), this, SLOT(close()));
    chkShowAtStartup->hide();
    connect(chkShowAtStartup, SIGNAL(toggled(bool)), this, SLOT(toggleShowAtStartup(bool)));

    KConfigGroup cfg( KSharedConfig::openConfig(), "SplashScreen");
    bool hideSplash = cfg.readEntry("HideSplashAfterStartup", false);
    chkShowAtStartup->setChecked(hideSplash);

    connect(lblRecent, SIGNAL(linkActivated(QString)), SLOT(linkClicked(QString)));
    connect(&m_timer, SIGNAL(timeout()), SLOT(raise()));

    // hide these labels by default
    displayLinks(false);
    displayRecentFiles(false);

    m_timer.setSingleShot(true);
    m_timer.start(10);
}

void KisSplashScreen::updateSplashImage()
{
    constexpr int SPLASH_HEIGHT_LOADING = 480;
    constexpr int SPLASH_HEIGHT_ABOUT = 320;

    int splashHeight;
    if (m_displayLinks) {
        splashHeight = SPLASH_HEIGHT_ABOUT;
    } else {
        splashHeight = SPLASH_HEIGHT_LOADING;
    }
    // const int bannerHeight = splashHeight * 0.16875;
    // const int marginTop = splashHeight * 0.05;
    // const int marginRight = splashHeight * 0.1;
    // Krimble 2026-10-11: no longer const, they are worked out again when the About image is made smaller (below).
    int bannerHeight = splashHeight * 0.16875;
    int marginTop = splashHeight * 0.05;
    int marginRight = splashHeight * 0.1;

    Source source = getImageSource();
    QPixmap img(source.resourcePath);

    if (img.isNull() || img.height() == 0) return;

    // Preserve aspect ratio of splash.
    // const int height = splashHeight;
    // const int width = height * img.width() / img.height();
    int height = splashHeight;
    int width = height * img.width() / img.height();
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-11: the splash in the About window is scaled to at most 80% of the screen width (it was a fixed 320 px high
    // and about 480 px wide, wider than a phone in portrait, which made the whole About window bigger than the screen).
    if (m_displayLinks) {
        if (QScreen *screenPtr = QGuiApplication::primaryScreen()) {
            // const int maxWidth = int(screenPtr->availableGeometry().width() * 0.80);
            // Krimble 2026-10-18: George (1.0.37): "8. About Krimble text is all over the place." The picture was sized from the screen, and came out
            // wider than the window it is in; the About dialog now gives the width of the window (see kis_about_application.cpp).
            const int maxWidth = m_maxAboutWidth > 0 ? m_maxAboutWidth : int(screenPtr->availableGeometry().width() * 0.80);
            if (maxWidth > 0 && width > maxWidth) {
                width = maxWidth;
                height = width * img.height() / img.width();
                bannerHeight = height * 0.16875;
                marginTop = height * 0.05;
                marginRight = height * 0.1;
            }
        }
    }
#endif

    setFixedWidth(width);
    setFixedHeight(height);
    lblSplash->setFixedWidth(width);
    lblSplash->setFixedHeight(height);

    // Get a downscaled pixmap of the splash.
    const int pixelWidth = width * devicePixelRatioF();
    const int pixelHeight = height * devicePixelRatioF();
    img = img.scaled(pixelWidth, pixelHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    img.setDevicePixelRatio(devicePixelRatioF());
    lblSplash->setPixmap(img);

    // KRIMBLE 2026-10-06: no logo and no banner any more; the loading text keeps the place it had (just below
    // where they used to be). Original lines:
    // // Align banner to top-left with margin.
    // m_bannerSvg->setFixedHeight(bannerHeight);
    // m_bannerSvg->setFixedWidth(bannerHeight * m_bannerSvg->sizeHint().width() / m_bannerSvg->sizeHint().height());
    // m_bannerSvg->move(width - m_bannerSvg->width() - marginRight, marginTop);
    //
    // // Place logo to the left of banner.
    // m_brandingSvg->setFixedSize(bannerHeight, bannerHeight);
    // m_brandingSvg->move(m_bannerSvg->x() - m_brandingSvg->width(), marginTop);
    //
    // // Place loading text immediately below.
    // m_loadingTextLabel->move(marginRight, m_brandingSvg->geometry().bottom());
    // m_loadingTextLabel->setFixedWidth(m_bannerSvg->geometry().right() - marginRight);
    m_loadingTextLabel->move(marginRight, marginTop + bannerHeight - 1);
    m_loadingTextLabel->setFixedWidth((width - marginRight - 1) - marginRight);

    // Place credits text on bottom right with similar margins.
    m_artCreditsLabel->setText(source.artistCredit);
    m_artCreditsLabel->setFixedWidth(m_loadingTextLabel->width());
    m_artCreditsLabel->setFixedHeight(20);
    m_artCreditsLabel->move(m_loadingTextLabel->x(), height - marginTop - m_artCreditsLabel->height());

    if (m_displayLinks) {
        setFixedSize(sizeHint());
    }
}

void KisSplashScreen::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateText();
}

void KisSplashScreen::updateText()
{
    QString color = colorString();

    KConfigGroup cfg2( KSharedConfig::openConfig(), "RecentFiles");
    int i = 1;

    QString recent = i18n("<html>"
                          "<head/>"
                          "<body>"
                          "<p><b><span style=\" color:%1;\">Recent Files</span></b></p>", color);

    QString path;
    QStringList recentfiles;

    QFontMetrics metrics(lblRecent->font());

    do {
        path = cfg2.readPathEntry(QString("File%1").arg(i), QString());
        if (!path.isEmpty()) {
            QString name = cfg2.readPathEntry(QString("Name%1").arg(i), QString());
            QUrl url(path);
            if (name.isEmpty()) {
                name = url.fileName();
            }

            name = metrics.elidedText(name, Qt::ElideMiddle, lblRecent->width());

            if (!url.isLocalFile() || QFile::exists(url.toLocalFile())) {
                recentfiles.insert(0, QString("<p><a href=\"%1\"><span style=\"color:%3;\">%2</span></a></p>").arg(path).arg(name).arg(color));
            }
        }

        i++;
    } while (!path.isEmpty() || i <= 8);

    recent += recentfiles.join("\n");
    recent += "</body>"
        "</html>";
    lblRecent->setText(recent);
}

void KisSplashScreen::displayLinks(bool show) {

    if (show) {
        QString color = colorString();
        QStringList lblLinksText;
        lblLinksText    << "<html>"
                        << "<head/>"
                        << "<body><table style=\"width:100%\" cellpadding=\"10\"><tr><td>"
                        << i18n("<p><span style=\" color:%1;\"><b>Using Krimble</b></span></p>",color);

#ifdef Q_OS_MACOS
        // macOS store version should not contain external links containing donation buttons or forms
        if (!KisMacosEntitlements().sandbox()) {
#endif

            lblLinksText    << i18n("<p><a href=\"https://www.buymeacoffee.com/GeorgeEdwardPurdy\"><span style=\" text-decoration: underline; color:%1;\">Support Krimble's Development!</span></a></p>",color)
                            << i18n("<p><a href=\"https://krimble.org/\"><span style=\" text-decoration: underline; color:%1;\">Krimble Website</span></a></p>",color);
#ifdef Q_OS_MACOS
        }
#endif
        lblLinksText    << i18n("<p><a href=\"https://krimble.org\"><span style=\" text-decoration: underline; color:%1;\">Getting Started</span></a></p>",color)
                        << i18n("<p><a href=\"https://krimble.org\"><span style=\" text-decoration: underline; color:%1;\">Manual</span></a></p>",color)
                        << "</td><td>"
                        << i18n("<p><span style=\" color:%1;\"><b>Coding Krimble</b></span></p>",color)
                        << i18n("<p><a href=\"https://krimble.org\"><span style=\" text-decoration: underline; color:%1;\">User Community</span></a></p>",color)
                        << i18n("<p><a href=\"https://github.com/purdygeorgeedward-dev/krimble\"><span style=\" text-decoration: underline; color:%1;\">Source Code</span></a></p>",color)
                        << i18n("<p><a href=\"https://krimble.org\"><span style=\" text-decoration: underline; color:%1;\">Scripting API</span></a></p>",color)
                        << i18n("<p><a href=\"https://krimble.org\"><span style=\" text-decoration: underline; color:%1;\">Scripting School</span></a></p>",color)
                        << "</td></tr></table></body>"
                        << "</html>";


        lblLinks->setTextFormat(Qt::RichText);
        lblLinks->setText(lblLinksText.join(""));

        filesLayout->setContentsMargins(10,10,10,10);
        actionControlsLayout->setContentsMargins(5,5,5,5);

    } else {
        // eliminating margins here allows for the splash screen image to take the entire area with nothing underneath
        filesLayout->setContentsMargins(0,0,0,0);
        actionControlsLayout->setContentsMargins(0,0,0,0);
    }

    lblLinks->setVisible(show);

    updateText();

    if (m_displayLinks != show) {
        m_displayLinks = show;
        updateSplashImage();
    }
}


void KisSplashScreen::displayRecentFiles(bool show) {
    lblRecent->setVisible(show);
    line->setVisible(show);
}

void KisSplashScreen::setLoadingText(QString text)
{
    int larger = 12;
    int notAsLarge = larger - 1;
    QString htmlText = QStringLiteral("<span style='font: %3pt;'><span style='font: bold %4pt;'>%1</span><br><i>%2</i></span>")
            .arg(m_versionHtml, text.toHtmlEscaped(), QString::number(notAsLarge), QString::number(larger));
    m_loadingTextLabel->setText(htmlText);
}

KisSplashScreen::Source KisSplashScreen::getImageSource()
{
    QString artistCredit = QString();
    // Loading the ginormous 4K PNG splash image increases the startup time on
    // Android by several seconds and at the same time looks really bad when
    // scaled down to a dinky size. Instead of overengineering this into an
    // Enterprise Splash Screen Solution where we choose the image based on
    // screen size or something, we'll just use a HD JPEG instead. It's fine.
#ifdef Q_OS_ANDROID
    // KRIMBLE 2026-10-02: splash images are JPG now (see splash-android.qrc).
    // QString resourcePath = QStringLiteral(":/splash/hd.png");
    QString resourcePath = QStringLiteral(":/splash/hd.jpg");
    // Krimble: holiday splash, active Dec 1 through Dec 26 inclusive.
    QDate currentDate = QDate::currentDate();
    if (currentDate >= QDate(currentDate.year(), 12, 1) &&
            currentDate <= QDate(currentDate.year(), 12, 26)) {
        // resourcePath = QStringLiteral(":/splash/holiday.png");
        resourcePath = QStringLiteral(":/splash/holiday.jpg");
    }
#else
    // QString resourcePath = QStringLiteral(":/splash/0.png");
    QString resourcePath = QStringLiteral(":/splash/0.jpg");
    // TODO: Re-add the holiday splash...
#if 0
    QDate currentDate = QDate::currentDate();
    if (currentDate > QDate(currentDate.year(), 12, 4) ||
            currentDate < QDate(currentDate.year(), 1, 9)) {
        // resourcePath = QStringLiteral(":/splash/1.png");
        resourcePath = QStringLiteral(":/splash/1.jpg");
        artistCredit = QStringLiteral("???")};
    }
#endif
#endif
    if (!artistCredit.isEmpty()) {
        artistCredit = i18nc("splash image credit", "Artwork by: %1", artistCredit);
    }
    return Source{resourcePath, artistCredit};
}


QString KisSplashScreen::colorString() const
{
    QString color = "#FFFFFF";
    if (m_themed && qApp->palette().window().color().value() > 100) {
        color = "#000000";
    }

    return color;
}


void KisSplashScreen::repaint()
{
    QWidget::repaint();
    qApp->sendPostedEvents();
}

void KisSplashScreen::show()
{
    if (!this->parentWidget()) {
        this->winId(); // Force creation of native window
        QWindow *windowHandle = this->windowHandle();
        QScreen *screen = windowHandle ? windowHandle->screen() : nullptr;
        if (windowHandle && !screen) {
            // At least on Windows, the window may be created on a non-primary
            // screen with a different scale factor. If we don't explicitly
            // move it to the primary screen, the position will be scaled with
            // the wrong factor and the splash will be offset.
            // XXX: In theory this branch should be unreachable, but leaving
            //      this here just in case.
            windowHandle->setScreen(QApplication::primaryScreen());
        }
        if (!screen) {
            screen = QApplication::primaryScreen();
        }
        // Reinitialize the splash image as the screen may have a different
        // devicePixelRatio.
        updateSplashImage();
        QRect r(QPoint(), size());
        move(screen->availableGeometry().center() - r.center());
    }
    if (isVisible()) {
        repaint();
    }
    m_timer.setSingleShot(true);
    m_timer.start(1);
    QWidget::show();
}

void KisSplashScreen::toggleShowAtStartup(bool toggle)
{
    KConfigGroup cfg( KSharedConfig::openConfig(), "SplashScreen");
    cfg.writeEntry("HideSplashAfterStartup", toggle);
}

void KisSplashScreen::linkClicked(const QString &link)
{
    KisPart::instance()->openExistingFile(link);
    if (isWindow()) {
        close();
    }
}
