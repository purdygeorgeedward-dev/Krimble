/*
 *  SPDX-FileCopyrightText: 2014 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QApplication>
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
    // Krimble 2026-10-18: the picture is not wider than 90% of the window this dialog is opened from (George, 1.0.37: "About Krimble text is all over the place").
    if (parent && parent->window()) {
        splash->setMaximumAboutWidth(int(parent->window()->width() * 0.90));
    }
#endif
    splash->displayLinks(true);

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-18 (2nd): George (1.0.37, fc126d8): "9. About Krimble still sucks." On the phone the About page is built here and not from the
    // splash widget: the picture is as wide as the window and keeps its shape, the version is written UNDER it (not over the art), and the four links
    // are one column. (Getting Started, Manual, Scripting API and Scripting School are left out: they pointed at the same web page, or at scripting, which
    // the Android build does not have.)
    {
        QWidget *page = new QWidget(wdgTab->aboutTab);
        QVBoxLayout *pageLayout = new QVBoxLayout(page);
        pageLayout->setContentsMargins(6, 6, 6, 6);
        const qreal dpr = qApp->devicePixelRatio();
        int pageWidth = 600;
        if (parent && parent->window()) pageWidth = int(parent->window()->width() * 0.86);
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
        pageLayout->addWidget(pictureLabel);
        pageLayout->addWidget(versionLabel);
        pageLayout->addWidget(linksLabel);
        pageLayout->addStretch(1);
        wdgTab->aboutTab->layout()->addWidget(page);
    }
#else
    wdgTab->aboutTab->layout()->addWidget(splash);
#endif
    // wdgTab->aboutTab->layout()->addWidget(splash);   (the original line; the phone uses the page above)

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

    // Translators
    // TODO: move to KisApplication after string freeze is lifted
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
                          "<p>");

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
    credits.append(i18n(".</p><p><i>For supporting Krimble development with advice, icons, brush sets and more.</i></p></body></html>"));

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
                           "<p>The Krimble Project is <b>committed</b> to preserving Krimble as free software.</p>"
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
    setMinimumSize(sizeHint());
    Q_ASSERT(layout());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}
