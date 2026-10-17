/*
 *  SPDX-FileCopyrightText: 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QGridLayout>
#include "kis_png_export.h"

#include <QCheckBox>
#include <QSlider>
#include <QApplication>

#include <kpluginfactory.h>

#include <KoColorSpace.h>
#include <KisImportExportManager.h>
#include <KisImportExportErrorCode.h>
#include <KoColorProfile.h>
#include <KoColorModelStandardIds.h>
#include <KoColorSpaceRegistry.h>

#include <KisExportCheckRegistry.h>

#include <kis_properties_configuration.h>
#include <kis_paint_device.h>
#include <KisDocument.h>
#include <kis_image.h>
#include <kis_paint_layer.h>
#include <kis_group_layer.h>
#include <kis_config.h>
#include <kis_meta_data_store.h>
#include <kis_meta_data_filter_registry_model.h>
#include <kis_exif_info_visitor.h>
#include "kis_png_converter.h"
#include <kis_iterator_ng.h>

K_PLUGIN_FACTORY_WITH_JSON(KisPNGExportFactory, "krita_png_export.json", registerPlugin<KisPNGExport>();)

KisPNGExport::KisPNGExport(QObject *parent, const QVariantList &) : KisImportExportFilter(parent)
{
}

KisPNGExport::~KisPNGExport()
{
}

KisImportExportErrorCode KisPNGExport::convert(KisDocument *document, QIODevice *io,  KisPropertiesConfigurationSP configuration)
{
    KisImageSP image = document->savingImage();

    KisPNGOptions options;

    options.alpha = configuration->getBool("alpha", true);
    options.interlace = configuration->getBool("interlaced", false);
    options.compression = configuration->getInt("compression", 3);
    options.tryToSaveAsIndexed = configuration->getBool("indexed", false);
    KoColor c(KoColorSpaceRegistry::instance()->rgb8());
    c.fromQColor(Qt::white);
    options.transparencyFillColor = configuration->getColor("transparencyFillcolor", c).toQColor();
    options.saveSRGBProfile = configuration->getBool("saveSRGBProfile", false);
    options.forceSRGB = configuration->getBool("forceSRGB", true);
    options.storeAuthor = configuration->getBool("storeAuthor", false);
    options.storeMetaData = configuration->getBool("storeMetaData", false);
    options.saveAsHDR = configuration->getBool("saveAsHDR", false);
    options.downsample = configuration->getBool("downsample", false);

    vKisAnnotationSP_it beginIt = image->beginAnnotations();
    vKisAnnotationSP_it endIt = image->endAnnotations();

    KisExifInfoVisitor eIV;
    eIV.visit(image->rootLayer().data());
    KisMetaData::Store *eI = 0;
    if (eIV.metaDataCount() == 1) {
        eI = eIV.exifInfo();
    }
    if (eI) {
        KisMetaData::Store* copy = new KisMetaData::Store(*eI);
        eI = copy;
    }

    KisPNGConverter pngConverter(document);

    KisImportExportErrorCode res = pngConverter.buildFile(io, image->bounds(), image->xRes(), image->yRes(), image->projection(), beginIt, endIt, options, eI);
    delete eI;
    dbgFile << " Result =" << res;
    return res;
}

KisPropertiesConfigurationSP KisPNGExport::defaultConfiguration(const QByteArray &, const QByteArray &) const
{
    KisPropertiesConfigurationSP cfg = new KisPropertiesConfiguration();
    cfg->setProperty("alpha", true);
    cfg->setProperty("indexed", false);
    cfg->setProperty("compression", 3);
    cfg->setProperty("interlaced", false);

    KoColor fill_color(KoColorSpaceRegistry::instance()->rgb8());
    fill_color = KoColor();
    fill_color.fromQColor(Qt::white);
    QVariant v;
    v.setValue(fill_color);

    cfg->setProperty("transparencyFillcolor", v);
    cfg->setProperty("saveSRGBProfile", false);
    cfg->setProperty("forceSRGB", true);
    cfg->setProperty("saveAsHDR", false);
    cfg->setProperty("storeMetaData", false);
    cfg->setProperty("storeAuthor", false);
    cfg->setProperty("downsample", false);
    return cfg;
}

KisConfigWidget *KisPNGExport::createConfigurationWidget(QWidget *parent, const QByteArray &, const QByteArray &) const
{
    return new KisWdgOptionsPNG(parent);
}

void KisPNGExport::initializeCapabilities()
{
    addCapability(KisExportCheckRegistry::instance()->get("sRGBProfileCheck")->create(KisExportCheckBase::SUPPORTED));
    QList<QPair<KoID, KoID> > supportedColorModels;
    supportedColorModels << QPair<KoID, KoID>()
            << QPair<KoID, KoID>(RGBAColorModelID, Integer8BitsColorDepthID)
            << QPair<KoID, KoID>(RGBAColorModelID, Integer16BitsColorDepthID)
            << QPair<KoID, KoID>(GrayAColorModelID, Integer8BitsColorDepthID)
            << QPair<KoID, KoID>(GrayAColorModelID, Integer16BitsColorDepthID);
    addSupportedColorModels(supportedColorModels, "PNG");
}

KisWdgOptionsPNG::KisWdgOptionsPNG(QWidget *parent)
    : KisConfigWidget(parent)
{
    setupUi(this);

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-13: George, about the PNG options window: "This has lots of dead space in it (fat) and it's super wide. I'm sure
    // on a desktop screen it seems fine, but on small screens it's too much." The window had a column of labels on the left
    // ("Compression (Lossless):", "Transparent color:") with nothing next to the check boxes, and the slider row was as wide as the
    // slider's two end labels and its label together. On Android the labels are stacked above their controls and the check boxes
    // use the full width, so the window is only as wide as its longest line ("Store alpha channel (transparency)").
    {
        QWidget *moved[] = {textLabel1, textLabel3, textLabel4_2, compressionLevel, tryToSaveAsIndexed, interlacing, chkSaveAsHDR,
                            chkSRGB, chkForceSRGB, alpha, chkMetaData, chkAuthor, chkDownsample, label_4, bnTransparencyFillColor};
        for (QWidget *w : moved) {
            gridLayout->removeWidget(w);
        }
        textLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(textLabel1, 0, 0, 1, 2);            // Compression (Lossless):
        gridLayout->addWidget(textLabel3, 1, 0);                  // Large file size
        gridLayout->addWidget(textLabel4_2, 1, 1);                // Small file size
        gridLayout->addWidget(compressionLevel, 2, 0, 1, 2);      // the slider, full width
        QWidget *checks[] = {tryToSaveAsIndexed, interlacing, chkSaveAsHDR, chkSRGB, chkForceSRGB, alpha, chkMetaData, chkAuthor, chkDownsample};
        int row = 3;
        for (QWidget *w : checks) {
            gridLayout->addWidget(w, row++, 0, 1, 2);
        }
        gridLayout->addWidget(label_4, row, 0);                   // Transparent color:
        gridLayout->addWidget(bnTransparencyFillColor, row, 1);
        // Krimble 2026-10-18 (9th): George (1.0.37, c666301; two screenshots): "The PNG save dialog is super wide." The 103rd entry above moved the widgets, but the form also holds a vertical
        // spacer (row 6) whose design size is 414 x 16 and whose horizontal policy is Minimum, which makes the window at least 414 px wide: a spacer is not a widget and was not moved.
        // Spacers are made 0 wide.
        for (int i = 0; i < gridLayout->count(); ++i) {
            if (QSpacerItem *spacer = gridLayout->itemAt(i)->spacerItem()) {
                spacer->changeSize(0, spacer->sizeHint().height(), QSizePolicy::Minimum, QSizePolicy::Expanding);
            }
        }
    }
#endif

    connect(chkSaveAsHDR, SIGNAL(toggled(bool)), this, SLOT(slotUseHDRChanged(bool)));
}

void KisWdgOptionsPNG::setConfiguration(const KisPropertiesConfigurationSP cfg)
{
    // the export manager should have prepared some info for us!
    KIS_SAFE_ASSERT_RECOVER_NOOP(cfg->hasProperty(KisImportExportFilter::ImageContainsTransparencyTag));
    KIS_SAFE_ASSERT_RECOVER_NOOP(cfg->hasProperty(KisImportExportFilter::ColorModelIDTag));
    KIS_SAFE_ASSERT_RECOVER_NOOP(cfg->hasProperty(KisImportExportFilter::sRGBTag));

    const bool isThereAlpha = cfg->getBool(KisImportExportFilter::ImageContainsTransparencyTag);

    alpha->setChecked(cfg->getBool("alpha", isThereAlpha));

    bnTransparencyFillColor->setEnabled(!alpha->isChecked());

    if (cfg->getString(KisImportExportFilter::ColorModelIDTag) == RGBAColorModelID.id()) {
        tryToSaveAsIndexed->setVisible(true);
        if (alpha->isChecked()) {
            tryToSaveAsIndexed->setChecked(false);
        }
        else {
            tryToSaveAsIndexed->setChecked(cfg->getBool("indexed", false));
        }
    }
    else {
        tryToSaveAsIndexed->setVisible(false);
    }
    interlacing->setChecked(cfg->getBool("interlaced", false));
    compressionLevel->setValue(cfg->getInt("compression", 3));
    compressionLevel->setRange(1, 9, 0);

    tryToSaveAsIndexed->setVisible(!isThereAlpha);

    //const bool sRGB = cfg->getBool(KisImportExportFilter::sRGBTag, false);

    //chkSRGB->setEnabled(sRGB);
    chkSRGB->setChecked(cfg->getBool("saveSRGBProfile", true));

    //chkForceSRGB->setEnabled(!sRGB);
    chkForceSRGB->setChecked(cfg->getBool("forceSRGB", false));

    chkSaveAsHDR->setChecked(cfg->getBool("saveAsHDR", false));
    slotUseHDRChanged(chkSaveAsHDR->isChecked());

    chkAuthor->setChecked(cfg->getBool("storeAuthor", false));
    chkMetaData->setChecked(cfg->getBool("storeMetaData", false));

    KoColor background(KoColorSpaceRegistry::instance()->rgb8());
    background.fromQColor(Qt::white);
    bnTransparencyFillColor->setDefaultColor(background);
    bnTransparencyFillColor->setColor(cfg->getColor("transparencyFillcolor", background));

    chkDownsample->setChecked(cfg->getBool("downsample", false));
}

KisPropertiesConfigurationSP KisWdgOptionsPNG::configuration() const
{

    KisPropertiesConfigurationSP cfg(new KisPropertiesConfiguration());

    bool alpha = this->alpha->isChecked();
    bool interlace = interlacing->isChecked();
    int compression = (int)compressionLevel->value();
    bool saveAsHDR = chkSaveAsHDR->isChecked();
    bool tryToSaveAsIndexed = !saveAsHDR && this->tryToSaveAsIndexed->isChecked();
    bool saveSRGB = !saveAsHDR && chkSRGB->isChecked();
    bool forceSRGB = !saveAsHDR && chkForceSRGB->isChecked();
    bool storeAuthor = chkAuthor->isChecked();
    bool storeMetaData = chkMetaData->isChecked();
    bool downsample = chkDownsample->isChecked();


    QVariant transparencyFillcolor;
    transparencyFillcolor.setValue(bnTransparencyFillColor->color());

    cfg->setProperty("alpha", alpha);
    cfg->setProperty("indexed", tryToSaveAsIndexed);
    cfg->setProperty("compression", compression);
    cfg->setProperty("interlaced", interlace);
    cfg->setProperty("transparencyFillcolor", transparencyFillcolor);
    cfg->setProperty("saveAsHDR", saveAsHDR);
    cfg->setProperty("saveSRGBProfile", saveSRGB);
    cfg->setProperty("forceSRGB", forceSRGB);
    cfg->setProperty("storeAuthor", storeAuthor);
    cfg->setProperty("storeMetaData", storeMetaData);
    cfg->setProperty("downsample", downsample);
    return cfg;
}

void KisWdgOptionsPNG::on_alpha_toggled(bool checked)
{
    bnTransparencyFillColor->setEnabled(!checked);
}

void KisWdgOptionsPNG::slotUseHDRChanged(bool value)
{
    tryToSaveAsIndexed->setDisabled(value);
    chkForceSRGB->setDisabled(value);
    chkSRGB->setDisabled(value);
}

#include "kis_png_export.moc"

