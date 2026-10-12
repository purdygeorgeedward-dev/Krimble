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
    const double resolution = cfg.defImageResolution();
    const QString colorModel = cfg.defColorModel();
    const QString colorDepth = cfg.defaultColorDepth();
    const QString colorProfile = cfg.defColorProfile();

    // Krimble 2026-10-12: kept so the dialog's height can follow this page (below)
    KisCustomImageWidget *krimbleCustomPage = new KisCustomImageWidget(this, w, h, resolution, colorModel, colorDepth, colorProfile, i18n("Unnamed"));
    addCustomDocumentWidget(
        // new KisCustomImageWidget(this, w, h, resolution, colorModel, colorDepth, colorProfile, i18n("Unnamed")),
        krimbleCustomPage,
        i18n("Custom Document"),
        "Custom Document",
        "document-new");

   addCustomDocumentWidget(
        new KisImageFromClipboardWidget(this, 0, 0, resolution, colorModel, colorDepth, colorProfile, i18n("Unnamed")),
        i18n("Create from Clipboard"),
        "Create from ClipBoard",
        "tab-new");

    connect(this, SIGNAL(documentSelected(KisDocument*)), KisPart::instance(), SLOT(startCustomDocument(KisDocument*)));
    connect(this, SIGNAL(openTemplate(QUrl)), KisPart::instance(), SLOT(openTemplate(QUrl)));

#ifdef Q_OS_ANDROID
    // KRIMBLE 2026-10-09: open no wider than 95% and no taller than 90% of the screen; the pages scroll inside.
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QSize screenSize = scr->availableGeometry().size();
        // resize(qMin(sizeHint().width(), int(screenSize.width() * 0.95)),
        //        qMin(sizeHint().height(), int(screenSize.height() * 0.90)));
        // Krimble 2026-10-12: George: "New dialog is too tall." The height came from the tall left list (icons over text) and left a
        // big empty gap in the Custom Document page. The height now follows what the page needs (its own size plus 70 px for the
        // frame), and is still at most 90% of the screen.
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
