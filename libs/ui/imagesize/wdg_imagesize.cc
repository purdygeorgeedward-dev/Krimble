/*
 *  dlg_imagesize.cc - part of KimageShop^WKrayon^WKrita
 *
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2013 Juan Palacios <jpalaciosdev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "wdg_imagesize.h"

#include <QLocale>
#include <QAbstractItemView>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontMetrics>
#include <QGridLayout>
#include <QLayout>
#include <QSpacerItem>
#include <kis_config.h>

#include <KoUnit.h>
#include <kis_size_group.h>
#include <klocalizedstring.h>

#include <kis_filter_strategy.h>
#include <KisViewManager.h>
#include <kis_image.h>

#include "kis_aspect_ratio_locker.h"
#include "kis_acyclic_signal_connector.h"
#include "kis_signals_blocker.h"

#include "kis_double_parse_unit_spin_box.h"
#include "kis_document_aware_spin_box_unit_manager.h"

static const int maxImagePixelSize = 100000000;
static KisFilterStrategy *lastUsedFilter = nullptr;

const QString WdgImageSize::PARAM_PREFIX = "imagesizedlg";
const QString WdgImageSize::PARAM_IMSIZE_UNIT = WdgImageSize::PARAM_PREFIX + "_imsizeunit";
const QString WdgImageSize::PARAM_SIZE_UNIT = WdgImageSize::PARAM_PREFIX + "_sizeunit";
const QString WdgImageSize::PARAM_RES_UNIT = WdgImageSize::PARAM_PREFIX + "_resunit";
const QString WdgImageSize::PARAM_RATIO_LOCK = WdgImageSize::PARAM_PREFIX + "_ratioLock";
const QString WdgImageSize::PARAM_PRINT_SIZE_SEPARATE = WdgImageSize::PARAM_PREFIX + "_printSizeSeparatly";

#include "ui_wdg_imagesize.h"

class PageImageSize : public QWidget, public Ui::WdgImageSize
{
    Q_OBJECT

public:
    PageImageSize(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};


static QString pixelsInchStr()
{
    static QString str = i18n("Pixels/Inch");
    return str;
}

static const QString pixelsCentimeterStr()
{
    static QString str = i18n("Pixels/Centimeter");
    return str;
}

// Krimble 2026-11-12: George (two screenshots of Image Size, one with four white boxes over the empty parts): "Yes, white areas represent wasted space. Please eliminate as much as you can."
// What was empty (measured in the screenshot, in logical pixels of a 708 px wide window): about 34 px of margin left of the labels, about 33 px inside every number field (a spin box is as wide as the
// widest number it could ever show, and the "size group" made all five the same), about 50 px inside every unit box (they were all made as wide as "Pixels/Inch"), and a blank band of about 135 px right
// of the chain buttons (the window was wider than its contents). Now: a number field is as wide as eight characters and its arrows, a unit box as wide as its own text and its arrow, the filter box
// is narrow, the spacers that pushed the columns apart are zero, the margins and the spacing between columns are small, and the window is made as wide as its contents need.
template <class Page>
static void krimbleCompactImageSizeWidgets(QWidget *root, Page *page)
{
    const QFontMetrics fm(root->font());
    const QList<QDoubleSpinBox *> spins = { page->pixelWidthDouble, page->pixelHeightDouble, page->printWidth, page->printHeight, page->printResolution };
    Q_FOREACH (QDoubleSpinBox *spin, spins) {
        // what a spin box needs besides its text (frame, padding, arrows): its own width minus the width of the widest number it could ever show
        const int extra = spin->sizeHint().width() - qMax(fm.horizontalAdvance(spin->textFromValue(spin->maximum())), fm.horizontalAdvance(spin->textFromValue(spin->minimum())));
        spin->setMinimumWidth(0);
        spin->setFixedWidth(fm.horizontalAdvance(QStringLiteral("00000.00")) + qMax(extra, 0));   // five digits and two decimals
    }
    const QList<QComboBox *> unitBoxes = { page->pixelSizeUnit, page->printWidthUnit, page->printResolutionUnit };
    Q_FOREACH (QComboBox *box, unitBoxes) {
        int widest = 0;
        for (int i = 0; i < box->count(); ++i) widest = qMax(widest, fm.horizontalAdvance(box->itemText(i)));
        const int extra = qMax(box->sizeHint().width() - widest, 0);   // arrow and padding
        // the box that is shut is as wide as its usual text ("Pixels/Inch": "Pixels/Centimeter" is longer and was what made the box wide); the list that opens is as wide as its widest text
        const int shown = qMax(fm.horizontalAdvance(QStringLiteral("0000")), (box == page->printResolutionUnit) ? qMin(widest, fm.horizontalAdvance(QStringLiteral("Pixels/Inch"))) : widest);
        box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        box->setMinimumWidth(0);
        box->setFixedWidth(shown + extra);
        if (box->view()) box->view()->setMinimumWidth(widest + extra);
    }
    page->pixelFilterCmb->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    page->pixelFilterCmb->setMinimumContentsLength(6);
    page->pixelFilterCmb->setMinimumWidth(0);
    Q_FOREACH (QLayout *layout, root->findChildren<QLayout *>()) {
        for (int i = 0; i < layout->count(); ++i) {
            if (QSpacerItem *spacer = layout->itemAt(i)->spacerItem()) {
                if (spacer->sizeHint().width() > 0) spacer->changeSize(0, spacer->sizeHint().height(), QSizePolicy::Minimum, QSizePolicy::Minimum);
            }
        }
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(layout)) {
            grid->setHorizontalSpacing(4);
            grid->setContentsMargins(4, 6, 4, 4);
        }
    }
    // the contents are no wider than they need (and a little more): a window that holds them opens at that width, and not wider as it did (the blank band at the right)
    if (root->layout()) {
        root->layout()->invalidate();
        root->layout()->activate();
        root->setMaximumWidth(root->layout()->totalMinimumSize().width() + 24);
    }
}

WdgImageSize::WdgImageSize(QWidget *parent, int width, int height, double resolution)
    : KisOperationUIWidget(i18n("Image Size"), parent)
{
    // Store original size.
    m_originalSize.setWidth(width);
    m_originalSize.setHeight(height);

    m_page = new PageImageSize(this);

    Q_CHECK_PTR(m_page);
    m_page->layout()->setContentsMargins(0, 0, 0, 0);
    m_page->setObjectName("image_size");

    m_page->pixelFilterCmb->setIDList(KisFilterStrategyRegistry::instance()->listKeys());
    m_page->pixelFilterCmb->allowAuto(true);
    m_page->pixelFilterCmb->setToolTip(KisFilterStrategyRegistry::instance()->formattedDescriptions());

    if (lastUsedFilter) { // Restore or Init..
        m_page->pixelFilterCmb->setCurrent(lastUsedFilter->id());
    } else {
        m_page->pixelFilterCmb->setCurrent(KisCmbIDList::AutoOptionID);
    }

    connect(this, &WdgImageSize::sigDesiredSizeChanged, [this](qint32 width, qint32 height, double){
        KisFilterStrategy *filterStrategy = KisFilterStrategyRegistry::instance()->autoFilterStrategy(m_originalSize, QSize(width, height));
        m_page->pixelFilterCmb->setAutoHint(filterStrategy->name());
    });

    /**
     * Initialize Pixel Width and Height fields
     */

    m_widthUnitManager = new KisDocumentAwareSpinBoxUnitManager(this);
    m_heightUnitManager = new KisDocumentAwareSpinBoxUnitManager(this, KisDocumentAwareSpinBoxUnitManager::PIX_DIR_Y);

    KisConfig cfg(true);

    /// configure the unit to image length, default unit is pixel and printing units are forbidden.
    m_widthUnitManager->setUnitDimension(KisSpinBoxUnitManager::IMLENGTH);
    m_heightUnitManager->setUnitDimension(KisSpinBoxUnitManager::IMLENGTH);

    m_widthUnitManager->syncWithOtherUnitManager(m_heightUnitManager); //sync the two managers, so that the units will be the same, but each manager will know a different reference for percents.

    m_widthUnitManager->setApparentUnitFromSymbol("px"); //set unit to pixel.

    m_page->pixelWidthDouble->setUnitManager(m_widthUnitManager);
    m_page->pixelHeightDouble->setUnitManager(m_heightUnitManager);
    m_page->pixelWidthDouble->setMaximum(maxImagePixelSize);
    m_page->pixelHeightDouble->setMaximum(maxImagePixelSize);
    m_page->pixelWidthDouble->changeValue(width);
    m_page->pixelHeightDouble->changeValue(height);
    m_page->pixelWidthDouble->setDisplayUnit(false);
    m_page->pixelHeightDouble->setDisplayUnit(false);
    m_page->pixelWidthDouble->setFocus();

    connect(m_page->pixelWidthDouble, qOverload<double>(&KisDoubleParseUnitSpinBox::valueChanged), [this](){
        Q_EMIT sigDesiredSizeChanged(desiredWidth(), desiredHeight(), desiredResolution());
    });
    connect(m_page->pixelHeightDouble, qOverload<double>(&KisDoubleParseUnitSpinBox::valueChanged), [this](){
        Q_EMIT sigDesiredSizeChanged(desiredWidth(), desiredHeight(), desiredResolution());
    });

    /// add custom units

    int unitId = m_widthUnitManager->getApparentUnitId();

    m_page->pixelSizeUnit->setModel(m_widthUnitManager);
    m_page->pixelSizeUnit->setCurrentIndex(unitId);

    /**
     * Connect Pixel Unit switching controls
     */

    KisAcyclicSignalConnector *pixelUnitConnector = new KisAcyclicSignalConnector(this);
    pixelUnitConnector->connectForwardInt(m_page->pixelSizeUnit, SIGNAL(currentIndexChanged(int)),
                                          m_widthUnitManager, SLOT(selectApparentUnitFromIndex(int)));
    pixelUnitConnector->connectBackwardInt(m_widthUnitManager, SIGNAL(unitChanged(int)),
                                           m_page->pixelSizeUnit, SLOT(setCurrentIndex(int)));

    QString imSizeUnit = cfg.readEntry<QString>(PARAM_IMSIZE_UNIT, "px");

    m_widthUnitManager->setApparentUnitFromSymbol(imSizeUnit);

    /**
     * Initialize Print Width, Height and Resolution fields
     */

    m_printSizeUnitManager = new KisSpinBoxUnitManager(this);

    m_page->printWidth->setUnitManager(m_printSizeUnitManager);
    m_page->printHeight->setUnitManager(m_printSizeUnitManager);
    m_page->printWidth->setDisplayUnit(false);
    m_page->printHeight->setDisplayUnit(false);
    m_page->printResolution->setAlignment(Qt::AlignRight);

    m_page->printWidthUnit->setModel(m_printSizeUnitManager);

    //TODO: create a resolution dimension in the unit manager.
    m_page->printResolutionUnit->addItem(pixelsInchStr());
    m_page->printResolutionUnit->addItem(pixelsCentimeterStr());


    /**
     * Initialize labels and layout
     */
    KisSizeGroup *labelsGroup = new KisSizeGroup(this);
    labelsGroup->addWidget(m_page->lblPixelWidth);
    labelsGroup->addWidget(m_page->lblPixelHeight);
    labelsGroup->addWidget(m_page->lblPixelFilter);
    labelsGroup->addWidget(m_page->lblPrintWidth);
    labelsGroup->addWidget(m_page->lblPrintHeight);
    labelsGroup->addWidget(m_page->lblResolution);

    // Krimble 2026-11-12: the two size groups below made all five number fields as wide as the widest of them, and all three unit boxes as wide as the widest (the "wasted space" George marked in the
    // Image Size screenshots); krimbleCompactImageSizeWidgets() sizes each field and box to what it needs, so they are not used (the code is kept):
    // KisSizeGroup *spinboxesGroup = new KisSizeGroup(this);
    // spinboxesGroup->addWidget(m_page->pixelWidthDouble);
    // spinboxesGroup->addWidget(m_page->pixelHeightDouble);
    // spinboxesGroup->addWidget(m_page->printWidth);
    // spinboxesGroup->addWidget(m_page->printHeight);
    // spinboxesGroup->addWidget(m_page->printResolution);

    // KisSizeGroup *comboboxesGroup = new KisSizeGroup(this);
    // comboboxesGroup->addWidget(m_page->pixelSizeUnit);
    // comboboxesGroup->addWidget(m_page->printWidthUnit);
    // comboboxesGroup->addWidget(m_page->printResolutionUnit);
    connect(this, SIGNAL(okClicked()), this, SLOT(accept()));

    /**
     * Initialize aspect ratio buttons and lockers
     */

    m_page->pixelAspectRatioBtn->setKeepAspectRatio(true);
    m_page->printAspectRatioBtn->setKeepAspectRatio(true);
    m_page->constrainProportionsCkb->setChecked(true);

    m_pixelSizeLocker = new KisAspectRatioLocker(this);
    m_pixelSizeLocker->connectSpinBoxes(m_page->pixelWidthDouble, m_page->pixelHeightDouble, m_page->pixelAspectRatioBtn);

    m_printSizeLocker = new KisAspectRatioLocker(this);
    m_printSizeLocker->connectSpinBoxes(m_page->printWidth, m_page->printHeight, m_page->printAspectRatioBtn);

    /**
     * Connect Keep Aspect Lock buttons
     */

    KisAcyclicSignalConnector *constrainsConnector = new KisAcyclicSignalConnector(this);
    constrainsConnector->connectBackwardBool(
        m_page->constrainProportionsCkb, SIGNAL(toggled(bool)),
        this, SLOT(slotLockAllRatioSwitched(bool)));

    constrainsConnector->connectForwardBool(
        m_pixelSizeLocker, SIGNAL(aspectButtonToggled(bool)),
        this, SLOT(slotLockPixelRatioSwitched(bool)));

    constrainsConnector->createCoordinatedConnector()->connectBackwardBool(
        m_printSizeLocker, SIGNAL(aspectButtonToggled(bool)),
        this, SLOT(slotLockPrintRatioSwitched(bool)));

    constrainsConnector->createCoordinatedConnector()->connectBackwardBool(
        m_page->adjustPrintSizeSeparatelyCkb, SIGNAL(toggled(bool)),
        this, SLOT(slotAdjustSeparatelySwitched(bool)));

    /**
     * Connect Print Unit switching controls
     */

    KisAcyclicSignalConnector *printUnitConnector = new KisAcyclicSignalConnector(this);
    printUnitConnector->connectForwardInt(
        m_page->printWidthUnit, SIGNAL(currentIndexChanged(int)),
        m_printSizeUnitManager, SLOT(selectApparentUnitFromIndex(int)));

    printUnitConnector->connectBackwardInt(
        m_printSizeUnitManager, SIGNAL(unitChanged(int)),
        m_page->printWidthUnit, SLOT(setCurrentIndex(int)));

    /// connect resolution
    connect(m_page->printResolutionUnit, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotPrintResolutionUnitChanged()));

    /**
     * Create syncing connections between Pixel and Print values
     */
    KisAcyclicSignalConnector *syncConnector = new KisAcyclicSignalConnector(this);
    syncConnector->connectForwardVoid(
        m_pixelSizeLocker, SIGNAL(sliderValueChanged()),
        this, SLOT(slotSyncPixelToPrintSize()));

    syncConnector->connectBackwardVoid(
        m_printSizeLocker, SIGNAL(sliderValueChanged()),
        this, SLOT(slotSyncPrintToPixelSize()));

    syncConnector->createCoordinatedConnector()->connectBackwardVoid(
        m_page->printResolution, SIGNAL(valueChanged(double)),
        this, SLOT(slotPrintResolutionChanged()));


    /**
     * Initialize printing values from the predefined image values
     */
    QString printSizeUnit;

    if (QLocale().measurementSystem() == QLocale::MetricSystem) {
        printSizeUnit = "cm";
    } else { // Imperial
        printSizeUnit = "in";
    }

    printSizeUnit = cfg.readEntry<QString>(PARAM_SIZE_UNIT, printSizeUnit);

    m_printSizeUnitManager->setApparentUnitFromSymbol(printSizeUnit);

    setCurrentResolutionPPI(resolution);
    slotSyncPixelToPrintSize();

    /**
     * Initialize aspect ratio lockers with the current proportion.
     */
    m_pixelSizeLocker->updateAspect();
    m_printSizeLocker->updateAspect();

    QString printResUnit = cfg.readEntry<QString>(PARAM_RES_UNIT, "");
    m_page->printResolutionUnit->setCurrentText(printResUnit);

    m_page->constrainProportionsCkb->setChecked(cfg.readEntry<bool>(PARAM_RATIO_LOCK, true));
    m_page->adjustPrintSizeSeparatelyCkb->setChecked(cfg.readEntry<bool>(PARAM_PRINT_SIZE_SEPARATE, false));

    QHBoxLayout *l = new QHBoxLayout();
    l->addWidget(m_page);
    this->setLayout(l);

    krimbleCompactImageSizeWidgets(this, m_page);   // Krimble 2026-11-12: no wasted space (see the function above)

    m_page->pixelWidthDouble->setFocus();
}

WdgImageSize::WdgImageSize(QWidget *parent, KisViewManager *view, KisOperationConfigurationSP config)
    : WdgImageSize(parent, view->image()->width(), view->image()->height(), view->image()->yRes())
{
    Q_UNUSED(config);
}

WdgImageSize::~WdgImageSize()
{
    KisConfig cfg(false);
    cfg.writeEntry<bool>(PARAM_PRINT_SIZE_SEPARATE, m_page->adjustPrintSizeSeparatelyCkb->isChecked());
    cfg.writeEntry<bool>(PARAM_RATIO_LOCK, m_page->constrainProportionsCkb->isChecked());

    cfg.writeEntry<QString>(PARAM_IMSIZE_UNIT, m_widthUnitManager->getApparentUnitSymbol());
    cfg.writeEntry<QString>(PARAM_SIZE_UNIT, m_printSizeUnitManager->getApparentUnitSymbol());
    cfg.writeEntry<QString>(PARAM_RES_UNIT, m_page->printResolutionUnit->currentText());

    delete m_page;
}

qint32 WdgImageSize::desiredWidth()
{
    return qRound(m_page->pixelWidthDouble->value());
}

qint32 WdgImageSize::desiredHeight()
{
    return qRound(m_page->pixelHeightDouble->value());
}

double WdgImageSize::desiredResolution()
{
    return currentResolutionPPI();
}

KisFilterStrategy *WdgImageSize::filterType()
{
    KoID filterID = m_page->pixelFilterCmb->currentItem();

    KisFilterStrategy *filter;
    if (filterID == KisCmbIDList::AutoOptionID) {
        filter = KisFilterStrategyRegistry::instance()->autoFilterStrategy(m_originalSize, QSize(desiredWidth(), desiredHeight()));
    } else {
        filter = KisFilterStrategyRegistry::instance()->value(filterID.id());
        lastUsedFilter = filter;  // Save for next time!
    }

    return filter;
}

void WdgImageSize::getConfiguration(KisOperationConfigurationSP config)
{
    // Krimble: everything ImageSizeOperation needs to genuinely replay this
    // step later, not just re-trigger the menu item. filterType() always
    // resolves "Auto" to a concrete strategy already, so the saved id is
    // never ambiguous on replay.
    config->setProperty("width", desiredWidth());
    config->setProperty("height", desiredHeight());
    config->setProperty("resolution", desiredResolution());
    KisFilterStrategy *filter = filterType();
    config->setProperty("filterStrategyId", filter ? filter->id() : QString());
}

void WdgImageSize::slotSyncPrintToPixelSize()
{
    const bool printIsSeparate = m_page->adjustPrintSizeSeparatelyCkb->isChecked();

    if (!printIsSeparate) {
        KisSignalsBlocker b(m_page->pixelWidthDouble, m_page->pixelHeightDouble);
        m_page->pixelWidthDouble->changeValue(m_page->printWidth->value() * currentResolutionPPI());
        m_page->pixelHeightDouble->changeValue(m_page->printHeight->value() * currentResolutionPPI());
    } else if (m_page->pixelWidthDouble->value() != 0.0) {
        const qreal resolution = qMax(0.001, m_page->pixelWidthDouble->value() / m_page->printWidth->value());
        setCurrentResolutionPPI(resolution);
    }
}

void WdgImageSize::slotSyncPixelToPrintSize()
{
    const qreal resolution = currentResolutionPPI();
    if (resolution != 0.0) {
        KisSignalsBlocker b(m_page->printWidth, m_page->printHeight);
        m_page->printWidth->changeValue(m_page->pixelWidthDouble->value() / resolution);
        m_page->printHeight->changeValue(m_page->pixelHeightDouble->value() / resolution);
    }
}

void WdgImageSize::slotPrintResolutionChanged()
{
    const bool printIsSeparate = m_page->adjustPrintSizeSeparatelyCkb->isChecked();

    if (printIsSeparate) {
        slotSyncPixelToPrintSize();
    } else {
        slotSyncPrintToPixelSize();
    }

    updatePrintSizeMaximum();
}

void WdgImageSize::slotPrintResolutionUnitChanged()
{
    qreal resolution = m_page->printResolution->value();

    if (m_page->printResolutionUnit->currentText() == pixelsInchStr()) {
        resolution = KoUnit::convertFromUnitToUnit(resolution, KoUnit(KoUnit::Inch), KoUnit(KoUnit::Centimeter));
    } else {
        resolution = KoUnit::convertFromUnitToUnit(resolution, KoUnit(KoUnit::Centimeter), KoUnit(KoUnit::Inch));
    }

    {
        KisSignalsBlocker b(m_page->printResolution);
        m_page->printResolution->setValue(resolution);
    }
}

void WdgImageSize::slotLockPixelRatioSwitched(bool value)
{
    const bool printIsSeparate = m_page->adjustPrintSizeSeparatelyCkb->isChecked();

    if (!printIsSeparate) {
        m_page->printAspectRatioBtn->setKeepAspectRatio(value);
    }
    m_page->constrainProportionsCkb->setChecked(value);
}

void WdgImageSize::slotLockPrintRatioSwitched(bool value)
{
    m_page->pixelAspectRatioBtn->setKeepAspectRatio(value);
    m_page->constrainProportionsCkb->setChecked(value);
}

void WdgImageSize::slotLockAllRatioSwitched(bool value)
{
    const bool printIsSeparate = m_page->adjustPrintSizeSeparatelyCkb->isChecked();

    m_page->pixelAspectRatioBtn->setKeepAspectRatio(value);

    if (!printIsSeparate) {
        m_page->printAspectRatioBtn->setKeepAspectRatio(value);
    }
}

void WdgImageSize::slotAdjustSeparatelySwitched(bool value)
{
    m_page->printAspectRatioBtn->setEnabled(!value);
    m_page->printAspectRatioBtn->setKeepAspectRatio(!value ? m_page->constrainProportionsCkb->isChecked() : true);
}

qreal WdgImageSize::currentResolutionPPI() const
{
    qreal resolution = m_page->printResolution->value();

    if (m_page->printResolutionUnit->currentText() == pixelsInchStr()) {
        resolution = KoUnit::convertFromUnitToUnit(resolution, KoUnit(KoUnit::Point), KoUnit(KoUnit::Inch));
    } else {
        resolution = KoUnit::convertFromUnitToUnit(resolution, KoUnit(KoUnit::Point), KoUnit(KoUnit::Centimeter));
    }

    return resolution;
}

void WdgImageSize::setCurrentResolutionPPI(qreal value)
{
    qreal newValue = value;

    if (m_page->printResolutionUnit->currentText() == pixelsInchStr()) {
        newValue = KoUnit::convertFromUnitToUnit(value, KoUnit(KoUnit::Inch), KoUnit(KoUnit::Point));
    } else {
        newValue = KoUnit::convertFromUnitToUnit(value, KoUnit(KoUnit::Centimeter), KoUnit(KoUnit::Point));
    }

    {
        KisSignalsBlocker b(m_page->printResolution);
        m_page->printResolution->setValue(newValue);
    }

    updatePrintSizeMaximum();
}

void WdgImageSize::updatePrintSizeMaximum()
{
    const qreal value = currentResolutionPPI();
    if (value == 0.0) return;

    m_page->printWidth->setMaximum(maxImagePixelSize / value);
    m_page->printHeight->setMaximum(maxImagePixelSize / value);
}

#include "wdg_imagesize.moc"
