/*
 *  SPDX-FileCopyrightText: 2026 Luna Lovecraft <ciubix8514@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KisDlgCreateNewDocument.h"
#include "KisImportExportManager.h"
#include "KisOpenPane.h"
#include "dialogs/kis_dlg_preferences.h"
#include "kis_custom_image_widget.h"
#include "kis_image_from_clipboard_widget.h"
#include "KisPart.h"
#include "kis_clipboard.h"

#include <QGuiApplication>
#include <QScreen>

KisDlgCreateNewDocument::KisDlgCreateNewDocument(QWidget* parent)
    : KisOpenPane(parent, KisImportExportManager::supportedMimeTypes(KisImportExportManager::Import), QStringLiteral("templates/"))
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(i18n("Create new document"));

    KisConfig cfg(true);

    int w = cfg.defImageWidth();
    int h = cfg.defImageHeight();
#ifdef Q_OS_ANDROID
    {
        static bool krimbleFirstNewThisRun = true;
        if (krimbleFirstNewThisRun) {
            krimbleFirstNewThisRun = false;
            if (QScreen *screen = QGuiApplication::primaryScreen()) {
                const QSize real = screen->geometry().size() * screen->devicePixelRatio();
                if (real.width() > 0 && real.height() > 0) {
                    w = real.width();
                    h = real.height();
                }
            }
        }
    }
#endif
    const double resolution = cfg.defImageResolution();
    const QString colorModel = cfg.defColorModel();
    const QString colorDepth = cfg.defaultColorDepth();
    const QString colorProfile = cfg.defColorProfile();

    KisClipboard *krimbleClipboard = KisClipboard::instance();
    if (cfg.readEntry<int>("krimble_new_from_clipboard_size", 1) != 0 && krimbleClipboard && krimbleClipboard->hasClip()) {
        const QSize clipSize = krimbleClipboard->clipSize();
        if (clipSize.isValid() && clipSize.width() > 0 && clipSize.height() > 0) {
            w = clipSize.width();
            h = clipSize.height();
        }
    }

    KisCustomImageWidget *krimbleCustomPage = new KisCustomImageWidget(this, w, h, resolution, colorModel, colorDepth, colorProfile, i18n("Unnamed"));
    addCustomDocumentWidget(
        krimbleCustomPage,
        i18n("Custom Document"),
        "Custom Document",
        "krimble-new-document");

   addCustomDocumentWidget(
        new KisImageFromClipboardWidget(this, 0, 0, resolution, colorModel, colorDepth, colorProfile, i18n("Unnamed")),
        i18n("Create from Clipboard"),
        "Create from ClipBoard",
        "krimble-new-from-clipboard");

    connect(this, SIGNAL(documentSelected(KisDocument*)), KisPart::instance(), SLOT(startCustomDocument(KisDocument*)));
    connect(this, SIGNAL(openTemplate(QUrl)), KisPart::instance(), SLOT(openTemplate(QUrl)));

#ifdef Q_OS_ANDROID
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QSize screenSize = scr->availableGeometry().size();
        const int pageHeight = krimbleCustomPage->sizeHint().height() + 70;
        resize(qMin(sizeHint().width(), int(screenSize.width() * 0.95)),
               qMin(qMin(sizeHint().height(), pageHeight), int(screenSize.height() * 0.90)));
    }
#endif
}

void KisDlgCreateNewDocument::SelectPage(Page page)
{
    switch (page) {
    case CreateNewDocument:
        selectItem("Custom Document");
        break;
    case CreateFromClipboard:
        selectItem("Create from ClipBoard");
        break;
    }
}
