/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_image_manager.h"

#include <QString>
#include <QStandardPaths>

#include <QAction>
#include <QUrl>
#include <QColorDialog>

#include <klocalizedstring.h>

#include <KoColor.h>
#include <KoFileDialog.h>

#include <kis_types.h>
#include <kis_image.h>
#include <kis_icon.h>
#include <KisImportExportManager.h>
#include "kis_import_catcher.h"
#include "KisViewManager.h"
#include "KisDocument.h"
#include "dialogs/kis_dlg_image_properties.h"
#include "commands/kis_image_commands.h"
#include "kis_action.h"
#include "kis_action_manager.h"
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QImage>
#include <QSettings>
#include "KisViewManager.h"
#include "widgets/kis_floating_message.h"
#include "kis_layer_utils.h"
#include "kis_canvas2.h"

#include "kis_signal_compressor_with_param.h"


KisImageManager::KisImageManager(KisViewManager * view)
        : m_view(view)
{
}

void KisImageManager::setView(QPointer<KisView>imageView)
{
    Q_UNUSED(imageView);
}

void KisImageManager::setup(KisActionManager *actionManager)
{

    KisAction *action  = actionManager->createAction("import_layer_from_file");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImportLayerFromFile()));

    action  = actionManager->createAction("image_properties");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImageProperties()));

    action  = actionManager->createAction("import_layer_as_paint_layer");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImportLayerFromFile()));

    action  = actionManager->createAction("import_layer_as_transparency_mask");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImportLayerAsTransparencyMask()));

    action  = actionManager->createAction("import_layer_as_filter_mask");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImportLayerAsFilterMask()));

    action  = actionManager->createAction("import_layer_as_selection_mask");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImportLayerAsSelectionMask()));

    action = actionManager->createAction("image_color");
    connect(action, SIGNAL(triggered()), this, SLOT(slotImageColor()));

    // Krimble: Image > Auto Crop
    action = actionManager->createAction("auto_crop_image");
    connect(action, SIGNAL(triggered()), this, SLOT(slotAutoCrop()));
}

// Krimble Auto Crop: finds the background from the four corners of the flattened image
// (all fully transparent, or all the same color within a tolerance) and crops to the content.
static bool krimbleAutoCropPixelIsBackground(QRgb p, QRgb bg, int tol)
{
    const int a = qAlpha(p), ba = qAlpha(bg);
    if (ba == 0) return a <= tol;                       // transparent background
    if (qAbs(a - ba) > tol) return false;
    return qAbs(qRed(p) - qRed(bg)) <= tol
        && qAbs(qGreen(p) - qGreen(bg)) <= tol
        && qAbs(qBlue(p) - qBlue(bg)) <= tol;
}

void KisImageManager::slotAutoCrop()
{
    KisImageSP image = m_view->image();
    if (!image) return;

    QSettings cfg;
    int tol = cfg.value("krimble/AutoCropTolerance", 8).toInt();
    int pad = cfg.value("krimble/AutoCropPadding", 0).toInt();

    QDialog dlg(m_view->mainWindowAsQWidget());
    dlg.setWindowTitle(i18n("Auto Crop"));
    QFormLayout *form = new QFormLayout(&dlg);
    QSpinBox *tolBox = new QSpinBox(&dlg);
    tolBox->setRange(0, 255);
    tolBox->setValue(tol);
    QSpinBox *padBox = new QSpinBox(&dlg);
    padBox->setRange(0, 10000);
    padBox->setSuffix(i18n(" px"));
    padBox->setValue(pad);
    form->addRow(i18n("Tolerance:"), tolBox);
    form->addRow(i18n("Padding:"), padBox);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(box);
    connect(box, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(box, SIGNAL(rejected()), &dlg, SLOT(reject()));
    if (dlg.exec() != QDialog::Accepted) return;

    tol = tolBox->value();
    pad = padBox->value();
    cfg.setValue("krimble/AutoCropTolerance", tol);
    cfg.setValue("krimble/AutoCropPadding", pad);

    image->barrierLock();
    image->unlock();

    const QRect bounds = image->bounds();
    QImage img = image->convertToQImage(bounds, 0).convertToFormat(QImage::Format_ARGB32);
    if (img.isNull() || img.width() < 2 || img.height() < 2) return;

    const int w = img.width(), h = img.height();
    const QRgb c[4] = { img.pixel(0, 0), img.pixel(w - 1, 0), img.pixel(0, h - 1), img.pixel(w - 1, h - 1) };

    bool cornersMatch = true;
    for (int i = 1; i < 4; ++i) {
        const bool bothClear = qAlpha(c[0]) <= tol && qAlpha(c[i]) <= tol;
        if (!bothClear && !krimbleAutoCropPixelIsBackground(c[i], c[0], tol)) cornersMatch = false;
    }
    if (!cornersMatch) {
        m_view->showFloatingMessage(i18n("Auto Crop: the four corners are not the same color or all transparent."),
                                    QIcon(), 3000, KisFloatingMessage::Low);
        return;
    }
    // Treat a transparent corner as the transparent background.
    const QRgb bg = (qAlpha(c[0]) <= tol) ? qRgba(0, 0, 0, 0) : c[0];

    int top = h, bottom = -1, left = w, right = -1;
    for (int y = 0; y < h; ++y) {
        const QRgb *line = reinterpret_cast<const QRgb*>(img.constScanLine(y));
        for (int x = 0; x < w; ++x) {
            if (!krimbleAutoCropPixelIsBackground(line[x], bg, tol)) {
                if (y < top) top = y;
                if (y > bottom) bottom = y;
                if (x < left) left = x;
                if (x > right) right = x;
            }
        }
    }
    if (bottom < 0) {
        m_view->showFloatingMessage(i18n("Auto Crop: the image is all background."),
                                    QIcon(), 3000, KisFloatingMessage::Low);
        return;
    }

    QRect content(left, top, right - left + 1, bottom - top + 1);
    content.adjust(-pad, -pad, pad, pad);
    content = content.intersected(QRect(0, 0, w, h));
    content.translate(bounds.topLeft());

    if (content == bounds) {
        m_view->showFloatingMessage(i18n("Auto Crop: nothing to crop."), QIcon(), 2000, KisFloatingMessage::Low);
        return;
    }
    image->cropImage(content);
}

void KisImageManager::slotImportLayerFromFile()
{
    importImage(QUrl(), "KisPaintLayer");
}

void KisImageManager::slotImportLayerAsTransparencyMask()
{
    importImage(QUrl(), "KisTransparencyMask");
}

void KisImageManager::slotImportLayerAsFilterMask()
{
    importImage(QUrl(), "KisFilterMask");
}

void KisImageManager::slotImportLayerAsSelectionMask()
{
    importImage(QUrl(), "KisSelectionMask");
}


qint32 KisImageManager::importImage(const QUrl &urlArg, const QString &layerType)
{
    KisImageWSP currentImage = m_view->image();

    if (!currentImage) {
        return 0;
    }

    QList<QString> paths;
    qint32 rc = 0;

    if (urlArg.isEmpty()) {
        KoFileDialog dialog(m_view->mainWindowAsQWidget(), KoFileDialog::OpenFiles, "OpenDocument");
        dialog.setCaption(i18n("Import Image"));
        dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
        dialog.setMimeTypeFilters(KisImportExportManager::supportedMimeTypes(KisImportExportManager::Import));
        QStringList fileNames = dialog.filenames();
        Q_FOREACH (const QString &fileName, fileNames) {
            paths << fileName;
        }

    } else {
        paths.push_back(urlArg.toLocalFile());
    }

    if (paths.empty()) {
        return 0;
    }

    Q_FOREACH(const QString &path, paths) {
        if (path.endsWith("svg")) {
            rc += (new KisImportCatcher(path, m_view, "KisShapeLayer"))->numLayersImported();
        }
        else {
            rc += (new KisImportCatcher(path, m_view, layerType))->numLayersImported();
        }
    }

    m_view->canvas()->update();

    return rc;
}

void KisImageManager::resizeCurrentImage(qint32 w, qint32 h, qint32 xOffset, qint32 yOffset)
{
    if (!m_view->image()) return;

    m_view->image()->resizeImage(QRect(-xOffset, -yOffset, w, h));
}

void KisImageManager::scaleCurrentImage(const QSize &size, qreal xres, qreal yres, KisFilterStrategy *filterStrategy)
{
    if (!m_view->image()) return;
    m_view->image()->scaleImage(size, xres, yres, filterStrategy);
}

void KisImageManager::rotateCurrentImage(double radians)
{
    if (!m_view->image()) return;
    m_view->image()->rotateImage(radians);
}

void KisImageManager::shearCurrentImage(double angleX, double angleY)
{
    if (!m_view->image()) return;
    m_view->image()->shear(angleX, angleY);
}


void KisImageManager::slotImageProperties()
{
    KisImageWSP image = m_view->image();
    if (!image) return;

    QPointer<KisDlgImageProperties> dlg = new KisDlgImageProperties(image, m_view->canvasBase()->displayColorConverter(), m_view->mainWindowAsQWidget());
    if (dlg->exec() == QDialog::Accepted) {
        if (dlg->convertLayerPixels()) {
            image->convertImageColorSpace(dlg->colorSpace(),
                                          KoColorConversionTransformation::internalRenderingIntent(),
                                          KoColorConversionTransformation::internalConversionFlags());

        } else {
            image->convertImageProjectionColorSpace(dlg->colorSpace());
        }
    }
    delete dlg;
}

void updateImageBackgroundColor(KisImageSP image, const QColorDialog *dlg)
{
    QColor newColor = dlg->currentColor();
    KoColor bg = image->defaultProjectionColor();
    bg.fromQColor(newColor);

    KisLayerUtils::changeImageDefaultProjectionColor(image, bg);
}

void KisImageManager::slotImageColor()
{
    KisImageWSP image = m_view->image();
    if (!image) return;

    QColorDialog dlg;
    dlg.setOption(QColorDialog::ShowAlphaChannel, true);
    dlg.setWindowTitle(i18n("Select a Color"));
    KoColor oldBgColor = image->defaultProjectionColor();
    dlg.setCurrentColor(oldBgColor.toQColor());

    KisSignalCompressor compressor(200, KisSignalCompressor::FIRST_INACTIVE);

    std::function<void ()> updateCall(std::bind(updateImageBackgroundColor, image, &dlg));
    SignalToFunctionProxy proxy(updateCall);

    connect(&dlg, SIGNAL(currentColorChanged(QColor)), &compressor, SLOT(start()));
    connect(&compressor, SIGNAL(timeout()), &proxy, SLOT(start()));

    if (dlg.exec() == QDialog::Accepted) {
        if (compressor.isActive()) {
            compressor.stop();
            updateCall();
        }
    } else {
        KisLayerUtils::changeImageDefaultProjectionColor(image, oldBgColor);
    }
}


