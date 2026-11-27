/*
 *  SPDX-FileCopyrightText: 2014 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTabWidget>
#include <QTabBar>
#include <QFont>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include "kis_about_application.h"

#include <KAboutData>
#include <KLocalizedString>
#include <QFile>
#include <QStandardPaths>

#include <kis_debug.h>
#include <kis_global.h>

#include "kis_splash_screen.h"
#include "ui_wdgaboutapplication.h"
#include <KisPortingUtils.h>

class Q_DECL_HIDDEN WdgAboutApplication : public QWidget, public Ui::WdgAboutApplication
{
public:
    WdgAboutApplication(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

KisAboutApplication::KisAboutApplication(QWidget *parent)
    : KoDialog(parent)
{
    setWindowTitle(i18n("About Krimble"));
    setButtons(KoDialog::Close);

    WdgAboutApplication *wdgTab = new WdgAboutApplication(this);

    KisSplashScreen *splash = new KisSplashScreen(true);
    splash->setWindowFlags(Qt::Widget);
#ifdef Q_OS_ANDROID
    if (parent && parent->window()) {
        splash->setMaximumAboutWidth(int(parent->window()->width() * 0.90));
    }
#endif
    splash->displayLinks(true);

#ifdef Q_OS_ANDROID
    {
        QWidget *page = new QWidget(wdgTab->aboutTab);
        QVBoxLayout *pageLayout = new QVBoxLayout(page);
        pageLayout->setContentsMargins(6, 6, 6, 6);
        const qreal dpr = qApp->devicePixelRatio();
        int pageWidth = 600;
        const bool sideways = parent && parent->window() && parent->window()->width() > parent->window()->height();
        if (parent && parent->window()) {
            const QSize win = parent->window()->size();
            pageWidth = int(win.width() * (sideways ? 0.56 : 0.90));
            if (sideways) pageWidth = qMin(pageWidth, int(win.height() * 0.60 * 1672.0 / 941.0));
        }
        QPixmap picture(QStringLiteral(":/splash/hd.jpg"));
        QLabel *pictureLabel = new QLabel(page);
        if (!picture.isNull()) {
            QPixmap scaled = picture.scaledToWidth(int(pageWidth * dpr), Qt::SmoothTransformation);
            scaled.setDevicePixelRatio(dpr);
            pictureLabel->setPixmap(scaled);
        }
        pictureLabel->setAlignment(Qt::AlignCenter);
        QLabel *versionLabel = new QLabel(qApp->applicationVersion().toHtmlEscaped(), page);
        QFont versionFont = versionLabel->font();
        versionFont.setBold(true);
        versionLabel->setFont(versionFont);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setWordWrap(true);
        QLabel *linksLabel = new QLabel(page);
        linksLabel->setTextFormat(Qt::RichText);
        linksLabel->setOpenExternalLinks(true);
        linksLabel->setWordWrap(true);
        linksLabel->setText(QStringLiteral(
            "<p><a href=\"https://www.buymeacoffee.com/GeorgeEdwardPurdy\">Support Krimble's Development!</a></p>"
            "<p><a href=\"https://krimble.org/\">Krimble Website</a></p>"
            "<p><a href=\"https://krimble.org\">User Community</a></p>"
            "<p><a href=\"https://github.com/purdygeorgeedward-dev/krimble\">Source Code</a></p>"));
        QLabel *fontsLabel = new QLabel(page);
        fontsLabel->setTextFormat(Qt::RichText);
        fontsLabel->setOpenExternalLinks(true);
        fontsLabel->setWordWrap(true);
        fontsLabel->setText(QStringLiteral(
            "<p><small>Purdy Design fonts (ROBO, Cartoonist Kooky, Cyberspace, Emoticons, Emoticons Outline) &copy; George Edward Purdy, Purdy Design. "
            "Free for non-commercial use. Commercial use needs a license: <a href=\"https://purdy.design\">purdy.design</a></small></p>"
            "<p><small>Also included: 53 open-license font families (SIL Open Font License 1.1 and Apache License 2.0), free for any use. "
            "Their licenses: <a href=\"https://github.com/purdygeorgeedward-dev/krimble/tree/master/krita/data/fonts/open\">fonts/open</a></small></p>"
            "<p><small>Intel One Mono &copy; 2023-2026 The Intel One Mono Project Authors (<a href=\"https://github.com/intel/intel-one-mono\">github.com/intel/intel-one-mono</a>), "
            "licensed under the SIL Open Font License 1.1, with Reserved Font Name &quot;Intel&quot;.</small></p>"));
        if (sideways) {
            QHBoxLayout *columns = new QHBoxLayout();
            QVBoxLayout *left = new QVBoxLayout();
            left->addWidget(pictureLabel);
            left->addWidget(versionLabel);
            left->addStretch(1);
            QWidget *right = new QWidget();
            QVBoxLayout *rightLayout = new QVBoxLayout(right);
            rightLayout->addWidget(linksLabel);
            rightLayout->addWidget(fontsLabel);
            rightLayout->addStretch(1);
            QScrollArea *rightArea = new QScrollArea(page);
            rightArea->setWidgetResizable(true);
            rightArea->setFrameShape(QFrame::NoFrame);
            rightArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            rightArea->setWidget(right);
            columns->addLayout(left);
            columns->addWidget(rightArea, 1);
            pageLayout->addLayout(columns, 1);
        } else {
            pageLayout->addWidget(pictureLabel);
            pageLayout->addWidget(versionLabel);
            pageLayout->addWidget(linksLabel);
            pageLayout->addWidget(fontsLabel);
            pageLayout->addStretch(1);
        }
        if (QTabWidget *tabs = wdgTab->findChild<QTabWidget*>()) {
            tabs->tabBar()->setUsesScrollButtons(true);
            tabs->tabBar()->setElideMode(Qt::ElideNone);
        }
        wdgTab->aboutTab->layout()->addWidget(page);
    }
#else
    wdgTab->aboutTab->layout()->addWidget(splash);
#endif

    QString authors = i18n("<html>"
                          "<head/>"
                          "<body>"
                          "<h1 align=\"center\">Created By</h1></p>"
                          "<p>");

    QFile fileDevelopers(":/developers.txt");
    Q_ASSERT(fileDevelopers.exists());
    if (fileDevelopers.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream developersText(&fileDevelopers);
        KisPortingUtils::setUtf8OnStream(developersText);
        authors.append(developersText.readAll().split("\n", Qt::SkipEmptyParts).join(", "));
    }
    authors.append(".</p></body></html>");
    wdgTab->lblAuthors->setText(authors);

    KAboutData aboutData(KAboutData::applicationData());
    if (aboutData.translators().isEmpty()) {
        aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                                i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    }

    QString translatorHtml = i18n(
        "<html>"
        "<head/>"
        "<body>"
        "<h1 align=\"center\"><b>Translators</b></h1>"
        "<p><ul>");

    Q_FOREACH (const KAboutPerson &person, aboutData.translators()) {
        translatorHtml.append(QString("<li>%1</li>").arg(person.name()));
    }

    translatorHtml.append("<ul></p>");
    translatorHtml.append(
        i18n("<p>KDE is translated into many languages thanks to the work of the "
             "translation teams all over the world.</p><p>For more information on KDE "
             "internationalization visit <a href=\"http://l10n.kde.org\">http://l10n."
             "kde.org</a></p>"));
    translatorHtml.append("</body></html>");

    wdgTab->lblTranslators->setText(translatorHtml);

    QString sponsors = i18n(
        "<html><head/><body>"
        "<h1 align=\"center\">Development Fund</h1>"
        "<p align=\"center\"> <a href=\"https://intel.com\"><img src=\":/intel.png\"></a> "
        "<h2 align=\"center\">One Time Sponsors</h2>"
        "<p align=\"center\"> <a href=\"https://www.unrealengine.com/en-US/megagrants\"><img src=\":/epic.png\"></a> "
        "<p align=\"center\"> <a href=\"http://brokenrul.es/\"><img src=\":/broken_rules.png\"></a> "
        "<p align=\"center\"> <a href=\"https://game-chuck.com/\"><img src=\":/gamechuck.png\"></a> "
        "<p align=\"center\"> <a href=\"https://www.fosshub.com/Krimble.html\"><img src=\":/fosshub.png\"></a> "
        "<p align=\"center\"> <a href=\"http://www.asifa-hollywood.org/\"><img src=\":/asifa.png\"></a> "
        "</body></html>");
    wdgTab->lblKickstarter->setText(sponsors);

    QString credits = i18n("<html>"
                          "<head/>"
                          "<body>"
                          "<h1 align=\"center\">Thanks To</h1>"
                          "<p>Krimble exists because of these. They are not connected to Krimble and have not endorsed it; we are simply grateful.</p>"
                          "<p><img src=\":/thanks-claude.png\" width=\"24\" height=\"24\"> <b>Claude</b> (Anthropic): helped design and write much of Krimble's code and documentation.</p>"
                          "<p><img src=\":/thanks-openai.png\" width=\"24\" height=\"24\"> <b>ChatGPT</b> (OpenAI): ideas, art direction and troubleshooting along the way.</p>"
                          "<p><img src=\":/thanks-termux.png\" width=\"24\" height=\"24\"> <b>Termux</b>: the Android terminal that runs the whole build from a phone.</p>"
                          "<p><img src=\":/thanks-github.png\" width=\"24\" height=\"24\"> <b>GitHub</b>: hosts the code, the releases and the issue tracker. What would we do without it?</p>"
                          "<p><i>The logos belong to their owners and are shown only to say thank you.</i></p>"
                          "<p>The <b>Krita</b> team and community, whose work Krimble is built on.</p>"
                          "<p><b>Also, from the Krita project:</b> ");

    QFile fileCredits(":/credits.txt");
    Q_ASSERT(fileCredits.exists());
    if (fileCredits.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream creditsText(&fileCredits);
        KisPortingUtils::setUtf8OnStream(creditsText);

        Q_FOREACH (const QString &credit, creditsText.readAll().split('\n', Qt::SkipEmptyParts)) {

            if (credit.contains(":")) {
                QList<QString> creditSplit = credit.split(':');
                credits.append(creditSplit.at(0));
                credits.append(" (<i>" + creditSplit.at(1) + "</i>)");
                credits.append(", ");
            }
        }
        credits.chop(2);
    }
    credits.append(i18n(".</p><p><i>For advice, icons, brush sets and more, which Krimble carries on.</i></p></body></html>"));

    wdgTab->lblCredits->setText(credits);

    QString license = i18n("<html>"
                           "<head/>"
                           "<body>"
                           "<h1 align=\"center\"><b>Your Rights</b></h1>"
                           "<p>Krimble is released under the GNU General Public License (version 3 or any later version).</p>"
                           "<p>This license grants people a number of freedoms:</p>"
                           "<ul>"
                           "<li>You are free to use Krimble, for any purpose</li>"
                           "<li>You are free to distribute Krimble</li>"
                           "<li>You can study how Krimble works and change it</li>"
                           "<li>You can distribute changed versions of Krimble</li>"
                           "</ul>"
                           "<h1 align=\"center\">Your artwork</h1>"
                           "<p>What you create with Krimble is your sole property. All your artwork is free for you to use as you like.</p>"
                           "<p>That means that Krimble can be used commercially, for any purpose. There are no restrictions whatsoever.</p>"
                           "<p>Krimble’s GNU GPL license guarantees you this freedom. Nobody is ever permitted to take it away, in contrast "
                           "to trial or educational versions of commercial software that will forbid your work in commercial situations.</p>"
                           "<br/><hr/><pre>");

    QFile licenseFile(":/LICENSE");
    Q_ASSERT(licenseFile.exists());
    if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream licenseText(&licenseFile);
        KisPortingUtils::setUtf8OnStream(licenseText);
        license.append(licenseText.readAll());
    }
    license.append("</pre></body></html>");
    wdgTab->lblLicense->setText(license);

    QFile thirdPartyFile(":/libraries.txt");
    if (thirdPartyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream thirdPartyText(&thirdPartyFile);
        KisPortingUtils::setUtf8OnStream(thirdPartyText);

        QString thirdPartyHtml = i18n("<html>"
                                      "<head/>"
                                      "<body>"
                                      "<h1 align=\"center\"><b>Third-party Libraries used by Krimble</b></h1>"
                                      "<p>Krimble is built on the following free software libraries:</p><p><ul>");

        Q_FOREACH (const QString &lib, thirdPartyText.readAll().split('\n', Qt::SkipEmptyParts)) {

            if (!lib.startsWith("#")) {
                QStringList parts = lib.split(',');
                if (parts.size() >= 3) {
                    thirdPartyHtml.append(QString("<li><a href=\"%2\">%1</a>: %3</li>").arg(parts[0], parts[1], parts[2]));
                }
            }
        }
        thirdPartyHtml.append("<ul></p></body></html>");
        wdgTab->lblThirdParty->setText(thirdPartyHtml);
    }

    setMainWidget(wdgTab);
    Q_ASSERT(layout());
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect available = screen->availableGeometry();
        resize(int(available.width() * 0.92), int(available.height() * 0.90));
    }
}
