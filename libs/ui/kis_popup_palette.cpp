/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Vera Lukman <shicmap@gmail.com>
   SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
   SPDX-FileCopyrightText: 2016 Scott Petrovic <scottpetrovic@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-only

*/
#include <QtGui>
#include <QMenu>
#include <QWhatsThis>
#include <QVBoxLayout>

#include <KisTagModel.h>

#include "kis_canvas2.h"
#include "kis_config.h"
#include "kis_popup_palette.h"
#include "kis_favorite_resource_manager.h"
#include "kis_icon_utils.h"
#include <kis_canvas_resource_provider.h>
#include <KoTriangleColorSelector.h>
#include "KoColorDisplayRendererInterface.h"
#include <KisVisualColorSelector.h>
#include <kis_config_notifier.h>
#include "kis_signal_compressor.h"
#include "widgets/kis_round_hud_button.h"
#include "KisDockerHud.h"
#include "kis_signals_blocker.h"
#include "kis_canvas_controller.h"
#include "kis_acyclic_signal_connector.h"
#include <kis_paintop_preset.h>
#include "KisMouseClickEater.h"
#include <KisPlatformPluginInterfaceFactory.h>

static const int WIDGET_MARGIN = 16;
static const qreal BORDER_WIDTH = 3.0;

class PopupColorTriangle : public KoTriangleColorSelector
{
public:
    PopupColorTriangle(const KoColorDisplayRendererInterface *displayRenderer, QWidget* parent)
        : KoTriangleColorSelector(displayRenderer, parent)
        , m_dragging(false)
    {
    }

    ~PopupColorTriangle() override {}

    void tabletEvent(QTabletEvent* event) override {
        QMouseEvent* mouseEvent = 0;

        if(event->button() == Qt::LeftButton || event->type() == QEvent::TabletMove)
        {
            switch (event->type()) {
                case QEvent::TabletPress:
                    mouseEvent = new QMouseEvent(QEvent::MouseButtonPress, event->pos(),
                                                 Qt::LeftButton, Qt::LeftButton, event->modifiers());
                    m_dragging = true;
                    mousePressEvent(mouseEvent);
                    event->accept();
                    break;
                case QEvent::TabletMove:
                    mouseEvent = new QMouseEvent(QEvent::MouseMove, event->pos(),
                                                 (m_dragging) ? Qt::LeftButton : Qt::NoButton,
                                                 (m_dragging) ? Qt::LeftButton : Qt::NoButton, event->modifiers());
                    mouseMoveEvent(mouseEvent);
                    event->accept();
                    break;
                case QEvent::TabletRelease:
                    mouseEvent = new QMouseEvent(QEvent::MouseButtonRelease, event->pos(),
                                                 Qt::LeftButton,
                                                 Qt::LeftButton,
                                                 event->modifiers());
                    m_dragging = false;
                    mouseReleaseEvent(mouseEvent);
                    event->accept();
                    break;
                default: break;
            }
        }

        delete mouseEvent;
    }

private:
    bool m_dragging;
};

KisPopupPalette::KisPopupPalette(KisViewManager* viewManager, KisCoordinatesConverter* coordinatesConverter ,KisFavoriteResourceManager* manager,
                                 const KoColorDisplayRendererInterface *displayRenderer, QWidget *parent)
    : QWidget(parent,

        KisPlatformPluginInterfaceFactory::instance()->surfaceColorManagedByOS() ?
            Qt::Popup :
            Qt::FramelessWindowHint)
    , m_coordinatesConverter(coordinatesConverter)
    , m_viewManager(viewManager)
    , m_actionManager(viewManager->actionManager())
    , m_resourceManager(manager)
    , m_displayRenderer(displayRenderer)
    , m_colorChangeCompressor(new KisSignalCompressor(50, KisSignalCompressor::FIRST_ACTIVE))
    , m_actionCollection(viewManager->actionCollection())
    , m_acyclicConnector(new KisAcyclicSignalConnector(this))
    , m_clicksEater(new KisMouseClickEater(Qt::RightButton, 1, this))
{
    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_colorChangeCompressor.data(), SIGNAL(timeout()),
            SLOT(slotEmitColorChanged()));

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), this, SLOT(slotConfigurationChanged()));
    connect(m_displayRenderer,  SIGNAL(displayConfigurationChanged()), this, SLOT(slotDisplayConfigurationChanged()));

    m_acyclicConnector->connectForwardKoColor(m_resourceManager, SIGNAL(sigChangeFGColorSelector(KoColor)),
                                              this, SLOT(slotExternalFgColorChanged(KoColor)));

    m_acyclicConnector->connectBackwardKoColor(this, SIGNAL(sigChangefGColor(KoColor)),
                                               m_resourceManager, SIGNAL(sigSetFGColor(KoColor)));
    connect(viewManager->canvasResourceProvider(), SIGNAL(sigBGColorChanged(KoColor)), SLOT(update()));

    connect(this, SIGNAL(sigChangeActivePaintop(int)), m_resourceManager, SLOT(slotChangeActivePaintop(int)));
    connect(this, SIGNAL(sigUpdateRecentColor(int)), m_resourceManager, SLOT(slotUpdateRecentColor(int)));

    connect(m_resourceManager, SIGNAL(setSelectedColor(int)), this, SLOT(slotSetSelectedColor(int)));
    connect(m_resourceManager, SIGNAL(updatePalettes()), this, SLOT(slotUpdate()));
    connect(m_resourceManager, SIGNAL(hidePalettes()), this, SIGNAL(finished()));

    connect(parent, SIGNAL(destroyed(QObject *)), this, SIGNAL(finished()), Qt::DirectConnection);

    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    setHoveredPreset(-1);
    setHoveredColor(-1);
    setSelectedColor(-1);

    m_dockerHud = new KisDockerHud(i18n("Popup Palette"), "popuppalette");

    m_tagsButton = new KisRoundHudButton(this);

    connect(m_tagsButton, SIGNAL(clicked()), SLOT(slotShowTagsPopup()));

    m_dockerHudButton = new KisRoundHudButton(this);
    m_dockerHudButton->setCheckable(true);

    connect(m_dockerHudButton, SIGNAL(toggled(bool)), SLOT(showHudWidget(bool)));

    m_bottomBarWidget = new QWidget(this);

    m_bottomBarButton = new KisRoundHudButton(this);
    m_bottomBarButton->setCheckable(true);

    connect( m_bottomBarButton, SIGNAL(toggled(bool)), SLOT(showBottomBarWidget(bool)));

    m_clearColorHistoryButton = new KisRoundHudButton(this);
    m_clearColorHistoryButton->setToolTip(i18n("Clear color history"));

    connect(m_clearColorHistoryButton, SIGNAL(clicked(bool)), m_resourceManager, SLOT(slotClearHistory()));
    connect(m_clearColorHistoryButton, SIGNAL(released()), this, SLOT(slotUpdate()));

    QGridLayout* gLayout = new QGridLayout(this);
    gLayout->setSizeConstraint(QLayout::SetFixedSize);
    gLayout->setSpacing(0);
    gLayout->setContentsMargins(QMargins());
    m_mainArea = new QSpacerItem(m_popupPaletteSize, m_popupPaletteSize);
    gLayout->addItem(m_mainArea, 0, 0);
    gLayout->setColumnMinimumWidth(1, WIDGET_MARGIN);
    gLayout->addWidget(m_dockerHud, 0, 2);
    gLayout->setRowMinimumHeight(1, WIDGET_MARGIN);
    gLayout->addWidget(m_bottomBarWidget, 2, 0);

    QHBoxLayout* hLayout = new QHBoxLayout(m_bottomBarWidget);

    mirrorMode = new KisHighlightedToolButton(this);
    mirrorMode->setFixedSize(35, 35);

    mirrorMode->setToolTip(i18n("Mirror Canvas"));
    mirrorMode->setDefaultAction(m_actionCollection->action("mirror_canvas_around_cursor"));
    connect(mirrorMode, SIGNAL(clicked(bool)), this, SLOT(slotUpdate()));
    connect(mirrorMode, SIGNAL(pressed()), this, SLOT(slotSetMirrorPos()));
    connect(mirrorMode, SIGNAL(clicked()), this, SLOT(slotRemoveMirrorPos()));

    canvasOnlyButton = new KisHighlightedToolButton(this);
    canvasOnlyButton->setFixedSize(35, 35);

    canvasOnlyButton->setToolTip(i18n("Canvas Only"));
    canvasOnlyButton->setDefaultAction(m_actionCollection->action("view_show_canvas_only"));

    zoomToOneHundredPercentButton = new QPushButton(this);
    zoomToOneHundredPercentButton->setText(i18n("100%"));
    zoomToOneHundredPercentButton->setFixedHeight(35);

    zoomToOneHundredPercentButton->setToolTip(i18n("Zoom to 100%"));
    connect(zoomToOneHundredPercentButton, SIGNAL(clicked(bool)), this, SLOT(slotZoomToOneHundredPercentClicked()));

    fitToViewButton = new QPushButton(this);
    fitToViewButton->setFixedHeight(35);

    fitToViewButton->setToolTip(i18n("Fit Canvas to View"));
    connect(fitToViewButton, SIGNAL(clicked(bool)), this, SLOT(slotFitToViewClicked()));

    zoomCanvasSlider = new QSlider(Qt::Horizontal, this);
    zoomSliderMinValue = 10;
    zoomSliderMaxValue = 200;

    zoomCanvasSlider->setRange(zoomSliderMinValue, zoomSliderMaxValue);
    zoomCanvasSlider->setFixedHeight(35);
    zoomCanvasSlider->setValue(m_coordinatesConverter->zoomInPercent());

    zoomCanvasSlider->setSingleStep(1);
    zoomCanvasSlider->setPageStep(1);

    connect(zoomCanvasSlider, SIGNAL(valueChanged(int)), this, SLOT(slotZoomSliderChanged(int)));
    connect(zoomCanvasSlider, SIGNAL(sliderPressed()), this, SLOT(slotZoomSliderPressed()));
    connect(zoomCanvasSlider, SIGNAL(sliderReleased()), this, SLOT(slotZoomSliderReleased()));

    slotUpdateIcons();

    hLayout->setSpacing(2);
    hLayout->setContentsMargins(0, 6, 0, 0);
    hLayout->addWidget(mirrorMode);
    hLayout->addWidget(canvasOnlyButton);
    hLayout->addWidget(zoomCanvasSlider);
    hLayout->addWidget(zoomToOneHundredPercentButton);
    hLayout->addWidget(fitToViewButton);

    setVisible(false);
    reconfigure();

    opacityChange = new QGraphicsOpacityEffect(this);
    opacityChange->setOpacity(1);
    setGraphicsEffect(opacityChange);

    installEventFilter(m_clicksEater);

    setAttribute(Qt::WA_NoMousePropagation, true);

    setAttribute(Qt::WA_AcceptTouchEvents, true);
    installEventFilter(this);
    const QList<QWidget *> childrenWidgets = findChildren<QWidget *>();
    for (const auto &child: childrenWidgets) {
        child->setAttribute(Qt::WA_AcceptTouchEvents, true);
        child->installEventFilter(this);
    }

    setContextMenuPolicy(Qt::PreventContextMenu);

    KisConfig cfg(true);
    m_dockerHudButton->setChecked(cfg.showBrushHud());
    m_bottomBarButton->setChecked(cfg.showPaletteBottomBar());

    m_dockerHud->setVisible(m_dockerHudButton->isChecked());
    m_bottomBarWidget->setVisible(m_bottomBarButton->isChecked());
}

KisPopupPalette::~KisPopupPalette()
{
}

void KisPopupPalette::slotConfigurationChanged()
{
    reconfigure();
    layout()->invalidate();
}

void KisPopupPalette::reconfigure()
{
    KisConfig config(true);
    m_useDynamicSlotCount = config.readEntry("popuppalette/useDynamicSlotCount", true);
    m_maxPresetSlotCount = config.favoritePresets();
    if (m_useDynamicSlotCount) {
        int presetCount = m_resourceManager->numFavoritePresets();
        m_presetSlotCount = presetCount == 0
            ? m_maxPresetSlotCount
            : qMin(m_maxPresetSlotCount, m_resourceManager->numFavoritePresets());
    } else {
        m_presetSlotCount = m_maxPresetSlotCount;
    }
    m_popupPaletteSize = config.readEntry("popuppalette/size", 385);
    qreal selectorRadius = config.readEntry("popuppalette/selectorSize", 140) / 2.0;

    m_showColorHistory = config.readEntry("popuppalette/showColorHistory", true);
    m_showRotationTrack = config.readEntry("popuppalette/showRotationTrack", true);

    m_colorHistoryInnerRadius = selectorRadius + m_presetRingMargin;
    m_colorHistoryOuterRadius = m_colorHistoryInnerRadius;
    if (m_showColorHistory) {
         m_colorHistoryOuterRadius += 20;
         m_clearColorHistoryButton->setVisible(true);
    } else {
         m_clearColorHistoryButton->setVisible(false);
    }

    m_mainArea->changeSize(m_popupPaletteSize, m_popupPaletteSize);

    bool useVisualSelector = config.readEntry<bool>("popuppalette/usevisualcolorselector", false);
    if (m_colorSelector) {
        bool haveVisualSelector = qobject_cast<KisVisualColorSelector*>(m_colorSelector) != 0;
        if (useVisualSelector != haveVisualSelector) {
            delete m_colorSelector;
            m_colorSelector = 0;
        }
    }
    if (!m_colorSelector) {
        if (useVisualSelector) {
            KisVisualColorSelector *selector = new KisVisualColorSelector(this);
            selector->setAcceptTabletEvents(true);
            connect(KisConfigNotifier::instance(), SIGNAL(configChanged()),
                    selector, SLOT(slotConfigurationChanged()));
            m_colorSelector = selector;
        }
        else {
            m_colorSelector  = new PopupColorTriangle(m_displayRenderer, this);
            connect(m_colorSelector, SIGNAL(requestCloseContainer()), this, SIGNAL(finished()));
            connect(KisConfigNotifier::instance(), SIGNAL(configChanged()),
                    m_colorSelector, SLOT(configurationChanged()));
        }
        m_colorSelector->setDisplayRenderer(m_displayRenderer);
        m_colorSelector->setConfig(true,false);
        m_colorSelector->setVisible(true);
        slotDisplayConfigurationChanged();
        connect(m_colorSelector, SIGNAL(sigNewColor(KoColor)),
                m_colorChangeCompressor.data(), SLOT(start()));

    }

    if (KoTriangleColorSelector *srgbColorSelector = dynamic_cast<KoTriangleColorSelector*>(m_colorSelector)) {
        bool fixTriangleRotation = config.readEntry("popuppalette/fixTriangleRotation", false);
        if (fixTriangleRotation) {
            int triangleRotationAngle = config.readEntry("popuppalette/triangleRotationAngle", 0);
            srgbColorSelector->setRotation(triangleRotationAngle);
        }
        else srgbColorSelector->setFollowHue(true);
    }

    const int auxButtonSize = 35;
    m_colorSelector->move(m_popupPaletteSize/2 - selectorRadius, m_popupPaletteSize/2 - selectorRadius);
    m_colorSelector->resize(m_popupPaletteSize - 2*m_colorSelector->x(), m_popupPaletteSize - 2*m_colorSelector->y());

    QRegion maskedEllipse(-2, -2, m_colorSelector->width() + 4, m_colorSelector->height() + 4, QRegion::Ellipse );
    QRegion maskedRectangle(0, 0, m_colorSelector->width(), m_colorSelector->height(), QRegion::Rectangle);
    QRegion maskedRegion = maskedEllipse.intersected(maskedRectangle);

    m_colorSelector->setMask(maskedRegion);

    m_dockerHud->setFixedHeight(int(m_popupPaletteSize));

    qreal margin = 10.0;
    qreal center = m_popupPaletteSize/2 - auxButtonSize/2.0;
    qreal length = m_popupPaletteSize/2 + auxButtonSize/2.0 + 5;
    {
        int buttonCount = 2;
        int arcLength = 90;
        qreal start = arcLength/2 - (buttonCount/2) * margin;
        if (buttonCount % 2 == 0) start += margin / 2;
        int place = 0;
        m_dockerHudButton->setGeometry(
            center + qCos(qDegreesToRadians(start + place*margin))*length,
            center + qSin(qDegreesToRadians(start + place*margin))*length,
            auxButtonSize, auxButtonSize
        );
        place++;
        m_bottomBarButton->setGeometry (
            center + qCos(qDegreesToRadians(start + place*margin))*length,
            center + qSin(qDegreesToRadians(start + place*margin))*length,
            auxButtonSize, auxButtonSize
        );
    }
    {
        int buttonCount = m_showColorHistory ? 2 : 1 ;
        int arcLength = 90;
        int shiftArc = 90;
        qreal start = shiftArc + arcLength / 2 - (buttonCount/2) * margin;
        if (buttonCount % 2 == 0) start += margin / 2;
        int place = 0;
        if (m_showColorHistory) {
            m_clearColorHistoryButton->setGeometry(
                center + qCos(qDegreesToRadians(start + place * margin)) * length,
                center + qSin(qDegreesToRadians(start + place * margin)) * length,
                auxButtonSize, auxButtonSize);
            place++;
        }
        m_tagsButton->setGeometry(
            center + qCos(qDegreesToRadians(start + place*margin))*length,
            center + qSin(qDegreesToRadians(start + place*margin))*length,
            auxButtonSize, auxButtonSize
        );
    }
    calculatePresetLayout();
}

void KisPopupPalette::slotDisplayConfigurationChanged()
{
    KoColor col = m_viewManager->canvasResourceProvider()->fgColor();
    const KoColorSpace *paintingCS = m_displayRenderer->getPaintingColorSpace();
    if (paintingCS->colorChannelCount()>3) {
        paintingCS = KoColorSpaceRegistry::instance()->rgb8();
    }
    m_colorSelector->slotSetColorSpace(paintingCS);
    m_colorSelector->slotSetColor(col);
}

void KisPopupPalette::slotExternalFgColorChanged(const KoColor &color)
{
    KisSignalsBlocker b(m_colorSelector);
    m_colorSelector->slotSetColor(color);
    update();
}

void KisPopupPalette::slotEmitColorChanged()
{
    if (isVisible()) {
        update();
        Q_EMIT sigChangefGColor(m_colorSelector->getCurrentColor());
    }
}

void KisPopupPalette::slotUpdate() {
    int presetCount = m_resourceManager->numFavoritePresets();
    if (m_useDynamicSlotCount && presetCount != m_presetSlotCount) {
         m_presetSlotCount = presetCount == 0
            ? m_maxPresetSlotCount
            : qMin(m_maxPresetSlotCount, presetCount);
        calculatePresetLayout();
    }
    m_canvasRotationIndicatorRect = rotationIndicatorRect(m_coordinatesConverter->rotationAngle());
    update();
}

int KisPopupPalette::hoveredPreset() const
{
    return m_hoveredPreset;
}

void KisPopupPalette::setHoveredPreset(int x)
{
    m_hoveredPreset = x;
}

int KisPopupPalette::hoveredColor() const
{
    return m_hoveredColor;
}

void KisPopupPalette::setHoveredColor(int x)
{
    m_hoveredColor = x;
}

int KisPopupPalette::selectedColor() const
{
    return m_selectedColor;
}

void KisPopupPalette::setSelectedColor(int x)
{
    m_selectedColor = x;
}

void KisPopupPalette::slotZoomSliderChanged(int zoom) {
    Q_EMIT zoomLevelChanged(zoom);
}

void KisPopupPalette::slotZoomSliderPressed()
{
   m_isZoomingCanvas = true;
}

void KisPopupPalette::slotZoomSliderReleased()
{
    m_isZoomingCanvas = false;
}

void KisPopupPalette::slotUpdateIcons()
{
    this->setPalette(qApp->palette());

    for(int i=0; i<this->children().size(); i++) {
        QWidget *w = qobject_cast<QWidget*>(this->children().at(i));
        if (w) {
            w->setPalette(qApp->palette());
        }
    }
    zoomToOneHundredPercentButton->setIcon(m_actionCollection->action("zoom_to_100pct")->icon());
    fitToViewButton->setIcon(m_actionCollection->action("zoom_to_fit")->icon());
    m_dockerHud->slotUpdateIcons();
    m_tagsButton->setIcon(KisIconUtils::loadIcon("tag"));
    m_clearColorHistoryButton->setIcon(KisIconUtils::loadIcon("reload-preset-16"));
    m_bottomBarButton->setOnOffIcons(KisIconUtils::loadIcon("arrow-up"), KisIconUtils::loadIcon("arrow-down"));
    m_dockerHudButton->setOnOffIcons(KisIconUtils::loadIcon("arrow-left"), KisIconUtils::loadIcon("arrow-right"));
}

void KisPopupPalette::showHudWidget(bool visible)
{
    m_dockerHud->setIsShown(m_dockerHudButton->isChecked());
    const bool reallyVisible = visible && m_dockerHudButton->isChecked();
    m_dockerHud->setVisible(reallyVisible);

    KisConfig cfg(false);
    cfg.setShowBrushHud(visible);
}

void KisPopupPalette::showBottomBarWidget(bool visible)
{
    const bool reallyVisible = visible && m_bottomBarButton->isChecked();

    m_bottomBarWidget->setVisible(reallyVisible);

    KisConfig cfg(false);
    cfg.setShowPaletteBottomBar(visible);
}

void KisPopupPalette::setParent(QWidget *parent) {
    QWidget::setParent(parent);
}

QSize KisPopupPalette::sizeHint() const
{
    return QSize(m_popupPaletteSize, m_popupPaletteSize);
}

void KisPopupPalette::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter painter(this);

    QPen pen(palette().color(QPalette::Text), BORDER_WIDTH);
    painter.setPen(pen);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (m_isOverFgBgColors) {
        painter.save();
        painter.setPen(QPen(palette().color(QPalette::Highlight), BORDER_WIDTH));
    }

    QPainterPath bgColor(drawFgBgColorIndicator(0));
    painter.fillPath(bgColor, m_displayRenderer->toQColor(m_resourceManager->bgColor()));
    painter.drawPath(bgColor);

    QPainterPath fgColor(drawFgBgColorIndicator(1));
    painter.fillPath(fgColor, m_displayRenderer->toQColor(m_colorSelector->getCurrentColor()));
    painter.drawPath(fgColor);

    if (m_isOverFgBgColors) painter.restore();

    QPainterPath backgroundContainer;

    QRectF circleRect(BORDER_WIDTH/2, BORDER_WIDTH/2, m_popupPaletteSize - BORDER_WIDTH, m_popupPaletteSize - BORDER_WIDTH);
    backgroundContainer.addEllipse(circleRect);
    painter.fillPath(backgroundContainer, palette().brush(QPalette::Window));
    painter.drawPath(backgroundContainer);

    if (m_showRotationTrack) {
        painter.save();
        QPen pen(palette().color(QPalette::Window).lighter(150), 2);
        painter.setPen(pen);

        if (m_isRotatingCanvasIndicator) {
            for (QLineF &line: m_snapLines) {
                painter.drawLine(line);
            }
        }
        QPainterPath rotationTrackPath;
        QRectF circleRect2(m_rotationTrackSize, m_rotationTrackSize,
                        m_popupPaletteSize - m_rotationTrackSize * 2, m_popupPaletteSize - m_rotationTrackSize * 2);

        rotationTrackPath.addEllipse(circleRect2);
        painter.drawPath(rotationTrackPath);

        QRectF resetRotationIndicator = m_resetCanvasRotationIndicatorRect;

        pen.setColor(m_isOverResetCanvasRotationIndicator
            ? palette().color(QPalette::Highlight)
            : palette().color(QPalette::Text));
        painter.setBrush(palette().brush(QPalette::Window));
        painter.setPen(pen);
        painter.drawEllipse(resetRotationIndicator);

        pen.setColor(m_isOverCanvasRotationIndicator || m_isRotatingCanvasIndicator
            ? palette().color(QPalette::Highlight)
            : palette().color(QPalette::Text));
        painter.setPen(pen);

        painter.setBrush(m_isRotatingCanvasIndicator && m_snapRotation
            ? palette().brush(QPalette::Highlight)
            : palette().brush(QPalette::Text));

        m_canvasRotationIndicatorRect = rotationIndicatorRect(m_coordinatesConverter->rotationAngle());
        painter.drawEllipse(m_canvasRotationIndicatorRect);

        painter.restore();
    }

    painter.translate(m_popupPaletteSize / 2, m_popupPaletteSize / 2);

    QList<QImage> images(m_resourceManager->favoritePresetImages());

    QPainterPath presetPath;
    int presetCount = images.size();
    bool isTagEmpty = presetCount == 0;
    for (int pos = 0; pos < m_presetSlotCount; pos++) {
        painter.save();
        presetPath = createPathFromPresetIndex(pos);

        if (pos < presetCount) {
            painter.setClipPath(presetPath);

            QRect bounds = presetPath.boundingRect().toAlignedRect();
            if (!images.at(pos).isNull()) {
                QImage previewHighDPI = images.at(pos).scaled(bounds.size()*devicePixelRatioF() , Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                previewHighDPI.setDevicePixelRatio(devicePixelRatioF());
                painter.drawImage(bounds.topLeft(), previewHighDPI);
            }
        } else {
            painter.fillPath(presetPath, palette().brush(QPalette::Window));
        }
        painter.restore();
        QColor color = isTagEmpty || pos >= presetCount
            ? palette().color(QPalette::Window).lighter(150)
            : palette().color(QPalette::Text);
        painter.setPen(QPen(color, 1));
        painter.drawPath(presetPath);
    }
    if (hoveredPreset() > -1) {
        presetPath = createPathFromPresetIndex(hoveredPreset());
        painter.setPen(QPen(palette().color(QPalette::Highlight), BORDER_WIDTH));
        painter.drawPath(presetPath);
    }

    if (m_showColorHistory) {
        painter.setPen(Qt::NoPen);
        const qreal rotationAngle = 360.0 / m_resourceManager->recentColorsTotal();
        const qreal rotationOffset = 180.0;

        painter.rotate(rotationOffset);

        if (m_resourceManager->recentColorsTotal() == 0) {
            painter.setBrush(Qt::transparent);

            QPainterPath emptyRecentColorsPath(drawDonutPathFull(0, 0, m_colorHistoryInnerRadius, m_colorHistoryOuterRadius));
            painter.setPen(QPen(palette().color(QPalette::Window).lighter(150), 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
            painter.drawPath(emptyRecentColorsPath);
        } else {
            for (int pos = 0; pos < m_resourceManager->recentColorsTotal(); pos++) {
                QPainterPath recentColorsPath(drawDonutPathAngle(m_colorHistoryInnerRadius, m_colorHistoryOuterRadius, m_resourceManager->recentColorsTotal()));

                painter.fillPath(recentColorsPath, m_displayRenderer->toQColor( m_resourceManager->recentColorAt(pos) ));
                painter.drawPath(recentColorsPath);
                painter.rotate(rotationAngle);
            }
        }

        if (hoveredColor() > -1) {
            painter.setPen(QPen(palette().color(QPalette::Highlight), 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

            if (m_resourceManager->recentColorsTotal() == 1) {
                QPainterPath path_ColorDonut(drawDonutPathFull(0, 0, m_colorHistoryInnerRadius, m_colorHistoryOuterRadius));
                painter.drawPath(path_ColorDonut);
            } else {
                painter.rotate((m_resourceManager->recentColorsTotal() + hoveredColor()) * rotationAngle);
                QPainterPath path(drawDonutPathAngle(m_colorHistoryInnerRadius, m_colorHistoryOuterRadius, m_resourceManager->recentColorsTotal()));
                painter.drawPath(path);
                painter.rotate(hoveredColor() * -1 * rotationAngle);
            }
        }

        if (selectedColor() > -1) {
            painter.setPen(QPen(palette().color(QPalette::Highlight).darker(130), 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

            if (m_resourceManager->recentColorsTotal() == 1) {
                QPainterPath path_ColorDonut(drawDonutPathFull(0, 0, m_colorHistoryInnerRadius, m_colorHistoryOuterRadius));
                painter.drawPath(path_ColorDonut);
            } else {
                painter.rotate((m_resourceManager->recentColorsTotal() + selectedColor()) * rotationAngle);
                QPainterPath path(drawDonutPathAngle(m_colorHistoryInnerRadius, m_colorHistoryOuterRadius, m_resourceManager->recentColorsTotal()));
                painter.drawPath(path);
                painter.rotate(selectedColor() * -1 * rotationAngle);
            }
        }
    }

    if(m_isRotatingCanvasIndicator || m_isZoomingCanvas) {
        opacityChange->setOpacity(0.4);
    } else {
        opacityChange->setOpacity(1.0);
    }

}

void KisPopupPalette::resizeEvent(QResizeEvent* resizeEvent) {
    Q_UNUSED(resizeEvent);
    calculateRotationSnapAreas();
    m_resetCanvasRotationIndicatorRect = rotationIndicatorRect(0);
    m_canvasRotationIndicatorRect = rotationIndicatorRect(m_coordinatesConverter->rotationAngle());

    const QPoint globalTopLeft = windowHandle() ?
        geometry().topLeft() :
        parentWidget()->mapToGlobal(geometry().topLeft());
    ensureWithinParent(globalTopLeft, true);
}

QPainterPath KisPopupPalette::drawDonutPathFull(int x, int y, int inner_radius, int outer_radius)
{
    QPainterPath path;
    path.addEllipse(QPointF(x, y), outer_radius, outer_radius);
    path.addEllipse(QPointF(x, y), inner_radius, inner_radius);
    path.setFillRule(Qt::OddEvenFill);

    return path;
}

QPainterPath KisPopupPalette::drawDonutPathAngle(int inner_radius, int outer_radius, int limit)
{
    QPainterPath path;
    path.moveTo(-0.999 * outer_radius * sin(M_PI / limit), 0.999 * outer_radius * cos(M_PI / limit));
    path.arcTo(-1 * outer_radius, -1 * outer_radius, 2 * outer_radius, 2 * outer_radius, -90.0 - 180.0 / limit,
               360.0 / limit);
    path.arcTo(-1 * inner_radius, -1 * inner_radius, 2 * inner_radius, 2 * inner_radius, -90.0 + 180.0 / limit,
               - 360.0 / limit);
    path.closeSubpath();

    return path;
}

QPainterPath KisPopupPalette::drawFgBgColorIndicator(int type) const
{
    QPointF edgePoint = QPointF(0.14645, 0.14645) * (m_popupPaletteSize);

    QPainterPath indicator;
    switch (type) {
        case 0: {
            indicator.addEllipse(edgePoint + QPointF(-4, 15), 30, 30);
            break;
        }
        case 1: {
            indicator.addEllipse(edgePoint + QPointF(6, -15), 30, 30);
            break;
        }
    }
    return indicator;
}

QRectF KisPopupPalette::rotationIndicatorRect(qreal rotationAngle) const
{
    qreal paletteRadius = 0.5 * m_popupPaletteSize;
    QPointF rotationDialPosition(drawPointOnAngle(rotationAngle, paletteRadius - 10));
    rotationDialPosition += QPointF(paletteRadius, paletteRadius);

    QPointF indicatorDiagonal(7.5, 7.5);
    return QRectF(rotationDialPosition - indicatorDiagonal, rotationDialPosition + indicatorDiagonal);
}

void KisPopupPalette::mouseMoveEvent(QMouseEvent *event)
{
    QPointF point = event->localPos();
    event->accept();

    if (m_showRotationTrack) {
        bool wasOverRotationIndicator = m_isOverCanvasRotationIndicator;
        m_isOverCanvasRotationIndicator = m_canvasRotationIndicatorRect.contains(point);
        bool wasOverResetRotationIndicator = m_isOverResetCanvasRotationIndicator;
        m_isOverResetCanvasRotationIndicator = m_resetCanvasRotationIndicatorRect.contains(point);

        if (
            wasOverRotationIndicator != m_isOverCanvasRotationIndicator ||
            wasOverResetRotationIndicator != m_isOverResetCanvasRotationIndicator
        ) {
            update();
        }

        if (m_isRotatingCanvasIndicator) {
            m_snapRotation = false;
            int i = 0;
            for (QRect &rect: m_snapRects) {
                QPainterPath circle;
                circle.addEllipse(rect);
                if (circle.contains(point)) {
                    m_snapRotation = true;
                    m_rotationSnapAngle = i * 15;
                    break;
                }
                i++;
            }
            qreal finalAngle = 0.0;
            if (m_snapRotation) {
                finalAngle = m_rotationSnapAngle;
                if (finalAngle >= 270) {
                    finalAngle = finalAngle - 360;
                }
            } else {
                QPoint widgetCenterPoint = QPoint(m_popupPaletteSize/2, m_popupPaletteSize/2);

                qreal dX = point.x() - widgetCenterPoint.x();
                qreal dY = point.y() - widgetCenterPoint.y();

                finalAngle = qAtan2(dY,dX) * 180 / M_PI;
                finalAngle = finalAngle + 90;
            }
            qreal angleDifference = finalAngle - m_coordinatesConverter->rotationAngle();

            KisCanvasController *canvasController =
                dynamic_cast<KisCanvasController*>(m_viewManager->canvasBase()->canvasController());
            KIS_ASSERT_RECOVER_RETURN(canvasController);
            canvasController->rotateCanvas(angleDifference);
            m_canvasRotationIndicatorRect = rotationIndicatorRect(finalAngle);

            update();
            Q_EMIT sigUpdateCanvas();
        }
    }

    if (m_isRotatingCanvasIndicator == false) {
        QPainterPath bgColor(drawFgBgColorIndicator(0));
        QPainterPath fgColor(drawFgBgColorIndicator(1));
        QPainterPath backgroundContainer;
        QRectF circleRect(BORDER_WIDTH / 2, BORDER_WIDTH / 2, m_popupPaletteSize - BORDER_WIDTH, m_popupPaletteSize - BORDER_WIDTH);
        backgroundContainer.addEllipse(circleRect);

        QPainterPath fgBgColors = (fgColor + bgColor) - backgroundContainer;

        if (fgBgColors.contains(point)) {
            if (!m_isOverFgBgColors) {
                m_isOverFgBgColors = true;
                setToolTip(i18n("Click to swap foreground and background colors.\nRight click to set to black and white."));
                update();
            }
        } else {
            if (m_isOverFgBgColors) {
                m_isOverFgBgColors = false;
                setToolTip(QString());
                update();
            }
        }

        QPainterPath colorHistoryPath(drawDonutPathFull(m_popupPaletteSize / 2, m_popupPaletteSize / 2, m_colorHistoryInnerRadius, m_colorHistoryOuterRadius));
        if (colorHistoryPath.contains(point)) {
            if (hoveredPreset() >= 0) {
                setToolTip(QString());
                setHoveredPreset(-1);
            }

            int pos = calculateColorIndex(point, m_resourceManager->recentColorsTotal());

            if (pos != hoveredColor()) {
                setHoveredColor(pos);
                update();
            }
        }
        else {
            if (hoveredColor() >= 0) {
                setHoveredColor(-1);
                update();
            }

            int pos = findPresetSlot(point);

            if (pos != hoveredPreset()) {

                if (pos >= 0 && pos < m_resourceManager->numFavoritePresets()) {
                    setToolTip(m_resourceManager->favoritePresetNamesList().at(pos));
                    setHoveredPreset(pos);
                }
                else {
                    setToolTip(QString());
                    setHoveredPreset(-1);
                }

                update();
            }
        }
    }
}

void KisPopupPalette::mousePressEvent(QMouseEvent *event)
{
    event->accept();

    if (event->button() == Qt::LeftButton) {
        if (m_showRotationTrack) {
            if (m_isOverCanvasRotationIndicator) {
                m_isRotatingCanvasIndicator = true;
                update();
            }

            if (m_isOverResetCanvasRotationIndicator) {
                qreal angleDifference = -m_coordinatesConverter->rotationAngle();
                KisCanvasController *canvasController =
                        dynamic_cast<KisCanvasController*>(m_viewManager->canvasBase()->canvasController());
                KIS_ASSERT_RECOVER_RETURN(canvasController);
                canvasController->rotateCanvas(angleDifference);
                m_canvasRotationIndicatorRect = rotationIndicatorRect(0);

                Q_EMIT sigUpdateCanvas();
            }
        }
    }
}

bool KisPopupPalette::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
        m_touchBeginReceived = true;
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
        if (static_cast<QMouseEvent *>(event)->source() == Qt::MouseEventSynthesizedBySystem && !m_touchBeginReceived) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (static_cast<QMouseEvent *>(event)->source() == Qt::MouseEventSynthesizedBySystem && !m_touchBeginReceived) {
            event->accept();
            return true;
        }
    case QEvent::Show:
    case QEvent::FocusOut:
        m_touchBeginReceived = false;
        break;
    default:
        break;
    }
    return false;
}

void KisPopupPalette::slotShowTagsPopup()
{
    KisTagModel model (ResourceType::PaintOpPresets);
    QVector<QString> tags;
    for (int i = 0; i < model.rowCount(); ++i) {
        QModelIndex idx = model.index(i, 0);
        tags << model.data(idx, Qt::DisplayRole).toString();
    }

    if (!tags.isEmpty()) {
        QMenu menu;
        Q_FOREACH (const QString& tag, tags) {
            menu.addAction(tag)->setData(tag);
        }

        QAction *action = menu.exec(QCursor::pos());
        if (action) {

            for (int i = 0; i < model.rowCount(); ++i) {
                QModelIndex idx = model.index(i, 0);
                if (model.data(idx, Qt::DisplayRole).toString() == action->data()) {
                    m_resourceManager->setCurrentTag(model.tagForIndex(idx));
                    reconfigure();
                    break;
                }
            }
        }
    } else {
        QWhatsThis::showText(QCursor::pos(),
                             i18n("There are no tags available to show in this popup. To add presets, you need to tag them and then select the tag here."));
    }

}

void KisPopupPalette::slotZoomToOneHundredPercentClicked() {
    QAction *action = m_actionCollection->action("zoom_to_100pct");

    if (action) {
        action->trigger();
    }

    zoomCanvasSlider->setValue(100);
}

void KisPopupPalette::slotFitToViewClicked() {
    QAction *action = m_actionCollection->action("zoom_to_fit");

    if (action) {
        action->trigger();
    }

    zoomCanvasSlider->setValue(m_coordinatesConverter->zoomInPercent());
}

void KisPopupPalette::slotSetMirrorPos() {
    m_actionCollection->action("mirror_canvas_around_cursor")->setProperty("customPosition", QVariant(m_mirrorPos));
}
void KisPopupPalette::slotRemoveMirrorPos() {
    m_actionCollection->action("mirror_canvas_around_cursor")->setProperty("customPosition", QVariant());
}

void KisPopupPalette::popup(const QPoint &position) {
    setVisible(true);
    KIS_SAFE_ASSERT_RECOVER_RETURN(parentWidget());
    const QPoint globalPos = parentWidget()->mapToGlobal(position);
    ensureWithinParent(globalPos, false);
    m_mirrorPos = globalPos;
}

void KisPopupPalette::dismiss()
{
    setVisible(false);
}

bool KisPopupPalette::onScreen()
{
    return isVisible();
}

void KisPopupPalette::ensureWithinParent(const QPoint& globalPos, bool useUpperLeft) {
    if (isVisible())  {
        const QSize paletteSize = geometry().size();
        const QPoint paletteCenterOffset(paletteSize.width() / 2, paletteSize.height() / 2);

        QPoint paletteGlobalPos = globalPos;
        if (!useUpperLeft) {
            paletteGlobalPos -= paletteCenterOffset;
        }

        if (parentWidget()) {
            const qreal widgetMargin = -20.0;

            const QPoint paletteParentPos = parentWidget()->mapFromGlobal(paletteGlobalPos);
            QRect paletteParentRect(paletteParentPos, paletteSize);

            const QRect fitRect = kisGrowRect(parentWidget()->rect(), widgetMargin);
            paletteParentRect = kisEnsureInRect(paletteParentRect, fitRect);
            paletteGlobalPos = parentWidget()->mapToGlobal(paletteParentRect.topLeft());
        }

        const QPoint moveToPoint = this->windowHandle() ? paletteGlobalPos : parentWidget()->mapFromGlobal(paletteGlobalPos);

        move(moveToPoint);
    }
}

void KisPopupPalette::showEvent(QShowEvent *event)
{
    raise();

    m_clicksEater->reset();

    if (m_coordinatesConverter->zoomInPercent() > zoomSliderMinValue &&
            m_coordinatesConverter->zoomInPercent() < zoomSliderMaxValue){
        KisSignalsBlocker b(zoomCanvasSlider);
        zoomCanvasSlider->setValue(m_coordinatesConverter->zoomInPercent());
    }

    QWidget::showEvent(event);
}

void KisPopupPalette::tabletEvent(QTabletEvent *event)
{
    event->ignore();

    if (this->windowHandle()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const QPointF localPos = event->posF();
#else
        const QPointF localPos = event->position();
#endif

        if (event->type() == QEvent::TabletRelease && !QRectF(rect()).contains(localPos)) {
            event->accept();
            Q_EMIT finished();
            return;
        }
    }
}

void KisPopupPalette::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF point = event->localPos();
    event->accept();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const QPointF localPos = event->localPos();
#else
    const QPointF localPos = event->position();
#endif

     if (this->windowHandle() && !QRectF(rect()).contains(localPos)) {
        Q_EMIT finished();
        return;
    }

    if (m_isRotatingCanvasIndicator) {
        update();
    }

    m_isRotatingCanvasIndicator = false;

    if (event->button() == Qt::LeftButton) {
        if (m_isOverFgBgColors) {
            m_viewManager->slotToggleFgBg();
        }

        if (hoveredPreset() > -1) {
            Q_EMIT sigChangeActivePaintop(hoveredPreset());
        }

        if (m_showColorHistory) {
            QPainterPath pathColor(drawDonutPathFull(m_popupPaletteSize / 2, m_popupPaletteSize / 2, m_colorHistoryInnerRadius, m_colorHistoryOuterRadius));
            if (pathColor.contains(point)) {
                int pos = calculateColorIndex(point, m_resourceManager->recentColorsTotal());

                if (pos >= 0 && pos < m_resourceManager->recentColorsTotal()) {
                    Q_EMIT sigUpdateRecentColor(pos);
                }
            }
        }
    } else if (event->button() == Qt::RightButton) {
        Q_EMIT finished();
    }
}

int KisPopupPalette::calculateColorIndex(QPointF position, int numColors) const
{
    if (numColors < 1) {
        return -1;
    }
    QPointF relPosition = position - QPointF(0.5 * m_popupPaletteSize, 0.5 * m_popupPaletteSize);

    qreal angle = M_PI - qAtan2(relPosition.x(), relPosition.y()) + M_PI / numColors;
    angle = normalizeAngle(angle);

    int index = floor(angle * numColors / (2 * M_PI));
    return qBound(0, index, numColors - 1);
}

bool KisPopupPalette::isPointInPixmap(QPointF &point, int pos)
{
    if (createPathFromPresetIndex(pos).contains(point + QPointF(-m_popupPaletteSize / 2, -m_popupPaletteSize / 2))) {
        return true;
    }
    return false;
}

QPointF KisPopupPalette::drawPointOnAngle(qreal angle, qreal radius) const
{
    QPointF p(
        radius * qCos(qDegreesToRadians(angle - 90)),
        radius * qSin(qDegreesToRadians(angle - 90))
    );
    return p;
}

void KisPopupPalette::calculatePresetLayout()
{
    if (m_presetSlotCount == 0) {
        m_cachedPresetLayout = {};
        return;
    }
    qreal angleSlice = 360.0 / qMax(m_presetSlotCount, 2);
    qreal outerRadius = m_popupPaletteSize/2 - m_presetRingMargin - (m_showRotationTrack ? m_rotationTrackSize + 1 : BORDER_WIDTH);
    qreal innerRadius = m_colorHistoryOuterRadius +
    (m_showColorHistory
        ? 1 + m_presetRingMargin
        : 0
    );

    qreal ringWidth = outerRadius - innerRadius;
    qreal halfRingWidth = 0.5 * ringWidth;
    qreal ringMidRadius = innerRadius + halfRingWidth;

    m_cachedPresetLayout = {};
    CachedPresetLayout& c = m_cachedPresetLayout;

    int margin = 2;

    qreal oneRowAngleSlice = angleSlice / 2;
    qreal oneRowMaxRadius = ringMidRadius * qSin(qDegreesToRadians(oneRowAngleSlice));

    if (oneRowMaxRadius - margin > halfRingWidth - oneRowMaxRadius * 0.2) {
        c.ringCount = 1;
        c.firstRowRadius = qMin(halfRingWidth, oneRowMaxRadius - margin);
        c.firstRowPos = ringMidRadius;
        return;
    }

    qreal tempRadius = halfRingWidth;
    {
        qreal twoRowInnerCount = ceil(qMax(m_presetSlotCount, 2) / 2.0);
        qreal twoRowAngleSlice = 360.0 / twoRowInnerCount;

        while (tempRadius >= 0) {
            tempRadius -= 0.2;
            QPointF r1p1(drawPointOnAngle(twoRowAngleSlice / 2, innerRadius + tempRadius));
            QPointF r1p2(drawPointOnAngle(twoRowAngleSlice / 2 * 3, innerRadius + tempRadius));
            QPointF r2p(drawPointOnAngle(twoRowAngleSlice, outerRadius - tempRadius));
            qreal row1SiblingDistance = kisDistance(r1p1, r1p2);
            qreal row1To2Distance = kisDistance(r1p1, r2p);
            if (row1To2Distance >= (tempRadius + margin) * 2) {
                if (row1SiblingDistance < (tempRadius + margin) * 2) {
                    break;
                }
                c.ringCount = 2;
                c.secondRowRadius = tempRadius;
                c.secondRowPos = outerRadius - tempRadius;
                c.firstRowRadius = tempRadius;
                c.firstRowPos = innerRadius + tempRadius;
                return;
            }
        }
    }

    qreal threeRowInnerCount = ceil(qMax(m_presetSlotCount, 2) / 3.0);
    qreal threeRowAngleSlice = 360.0 / threeRowInnerCount;

    while (tempRadius >= 0) {
        QPointF r1p1(drawPointOnAngle(threeRowAngleSlice / 2, innerRadius + tempRadius));
        QPointF r1p2(drawPointOnAngle(threeRowAngleSlice / 2 * 3, innerRadius + tempRadius));
        QPointF r2p1(drawPointOnAngle(threeRowAngleSlice, ringMidRadius));
        QPointF r2p2(drawPointOnAngle(threeRowAngleSlice * 2, ringMidRadius));
        QPointF r3p(drawPointOnAngle(threeRowAngleSlice / 2, outerRadius - tempRadius));

        qreal row1SiblingDistance = kisDistance(r1p1, r1p2);
        qreal row1to2Distance = kisDistance(r1p1, r2p1);
        qreal row2to3Distance = kisDistance(r2p1, r3p);
        qreal row1to3Distance = kisDistance(r1p1, r3p);

        if (
            row1to2Distance >= tempRadius * 2 &&
            row2to3Distance >= tempRadius * 2 &&
            row1to3Distance >= tempRadius * 2 &&
            row1SiblingDistance >= tempRadius * 2
        ) {

            qreal row2SiblingDistance = kisDistance(r2p1, r2p2);

            qreal firstRowRadius = tempRadius;
            qreal thirdRowRadius = tempRadius;
            qreal secondRowRadius = tempRadius;

            bool firstRowTouching = row1SiblingDistance - firstRowRadius * 2 < 1;
            if (firstRowTouching) {
                QPointF tempR3p = r3p;
                qreal tempSecondThirdRowRadius = secondRowRadius;
                qreal tempRow2to3Distance = row2to3Distance;
                qreal tempRow1to3Distance = row1to3Distance;
                while (
                    tempSecondThirdRowRadius * 2 < tempRow2to3Distance &&
                    tempSecondThirdRowRadius * 2 < row2SiblingDistance &&
                    tempSecondThirdRowRadius * 2 < tempRow1to3Distance &&
                    tempSecondThirdRowRadius + firstRowRadius < row1to2Distance
                ) {
                    r3p = tempR3p;
                    row2to3Distance = tempRow2to3Distance;
                    row1to3Distance = tempRow1to3Distance;
                    secondRowRadius = tempSecondThirdRowRadius;

                    tempSecondThirdRowRadius += 1;

                    tempR3p = drawPointOnAngle(threeRowAngleSlice / 2, outerRadius - tempSecondThirdRowRadius);

                    tempRow2to3Distance = kisDistance(r2p1, tempR3p);
                    tempRow1to3Distance = kisDistance(r1p1, tempR3p);
                }
                thirdRowRadius = secondRowRadius;
            }

            {
                qreal tempThirdRowRadius = thirdRowRadius;
                QPointF tempR3p = r3p;
                qreal tempRow2to3Distance = row2to3Distance;
                qreal tempRow1to3Distance = row1to3Distance;
                while (
                    tempThirdRowRadius < halfRingWidth &&
                    secondRowRadius + tempThirdRowRadius < tempRow2to3Distance &&
                    firstRowRadius + tempThirdRowRadius < tempRow1to3Distance
                ) {
                    r3p = tempR3p;
                    row2to3Distance = tempRow2to3Distance;
                    row1to3Distance = tempRow1to3Distance;
                    thirdRowRadius = tempThirdRowRadius;

                    tempThirdRowRadius += 1;

                    tempR3p = drawPointOnAngle(threeRowAngleSlice / 2, outerRadius - tempThirdRowRadius);
                    tempRow2to3Distance = kisDistance(r2p1, tempR3p);
                    tempRow1to3Distance = kisDistance(r1p1, tempR3p);
                }
            }
            qreal thirdRowPos = outerRadius - thirdRowRadius;

            qreal firstRowPos = innerRadius + tempRadius;
            {
                qreal tempFirstRowPos = firstRowPos;
                qreal tempFirstRowRadius = firstRowRadius;
                qreal tempRow1SiblingDistance = row1SiblingDistance;
                qreal tempRow1to3Distance = row1to3Distance;
                qreal tempRow1to2Distance = row1to2Distance;
                QPointF tempR1p1 = r1p1;
                QPointF tempR1p2 = r1p2;

                while (
                    tempFirstRowPos < ringMidRadius &&
                    tempFirstRowRadius + secondRowRadius < tempRow1to2Distance &&
                    tempFirstRowRadius + thirdRowRadius < tempRow1to3Distance
                ) {
                    firstRowPos = tempFirstRowPos;
                    firstRowRadius = tempFirstRowRadius;
                    row1to2Distance = tempRow1to2Distance;
                    r1p1 = tempR1p1;

                    tempFirstRowPos += 1;

                    tempR1p1 = drawPointOnAngle(threeRowAngleSlice / 2, tempFirstRowPos);
                    tempR1p2 = drawPointOnAngle(threeRowAngleSlice / 2 * 3, tempFirstRowPos);
                    tempRow1SiblingDistance = kisDistance(tempR1p1, tempR1p2);
                    tempFirstRowRadius = tempRow1SiblingDistance / 2;
                    tempRow1to2Distance = kisDistance(tempR1p2, r2p1);
                    tempRow1to3Distance = kisDistance(tempR1p1, r3p);
                }
            }

            qreal secondRowPos = ringMidRadius;
            bool row2touching1 = row1to2Distance - (firstRowRadius + secondRowRadius) < 1;
            bool row2touching3 = row2to3Distance - (thirdRowRadius + secondRowRadius) < 1;
            if (!row2touching1 && !row2touching3) {
                qreal knownAngleRatio = qSin(qDegreesToRadians(threeRowAngleSlice / 2)) /
                                        (firstRowRadius + secondRowRadius);
                qreal angleRow1Row2Center = qAsin(knownAngleRatio * firstRowPos);
                qreal angleCenterRow2Row1 = 180 - threeRowAngleSlice / 2 - qRadiansToDegrees(angleRow1Row2Center);
                secondRowPos = qSin(qDegreesToRadians(angleCenterRow2Row1)) / knownAngleRatio;
            }
            if (!row2touching3) {
                QPointF tempR2p1 = r2p1;
                qreal tempRadius = secondRowRadius;
                qreal tempRow1to2Distance = row1to2Distance;
                qreal tempRow2to3Distance = row2to3Distance;
                qreal tempSecondRowPos = secondRowPos;
                while (
                    tempSecondRowPos < thirdRowPos &&
                    tempRadius + thirdRowRadius < tempRow2to3Distance &&
                    tempRadius < thirdRowRadius
                ) {
                    secondRowRadius = tempRadius;
                    secondRowPos = tempSecondRowPos;

                    tempSecondRowPos += 1;

                    tempR2p1 = drawPointOnAngle(threeRowAngleSlice, secondRowPos + 1);
                    tempRow1to2Distance = kisDistance(tempR2p1, r1p1);
                    tempRow2to3Distance = kisDistance(tempR2p1, r3p);
                    tempRadius = tempRow1to2Distance - firstRowRadius;
                }
            }
            c = {
                3,
                firstRowRadius - margin,
                secondRowRadius - margin,
                thirdRowRadius - margin,
                firstRowPos,
                secondRowPos,
                thirdRowPos
            };
            return;
        }
        tempRadius -= 0.2;
    }
}

QPainterPath KisPopupPalette::createPathFromPresetIndex(int index) const
{
    qreal angleSlice = 360.0 / qMax(m_presetSlotCount, 2);
    qreal startingAngle = -(index * angleSlice) + 90;
    qreal length = m_cachedPresetLayout.firstRowPos;
    qreal radius = m_cachedPresetLayout.firstRowRadius;
    switch (m_cachedPresetLayout.ringCount) {
    case 1: break;
    case 2: {
        angleSlice = 180.0/((m_presetSlotCount+1) / 2);
        startingAngle = -(index * angleSlice) + 90;

        if (index % 2) {
            length = m_cachedPresetLayout.secondRowPos;
            radius = m_cachedPresetLayout.secondRowRadius;
        }
        break;
    }
    case 3: {
        int triplet = index / 3;
        angleSlice = 180.0 / ((m_presetSlotCount + 2) / 3);
        switch (index % 3) {
        case 0:
            startingAngle = -(triplet * 2 * angleSlice) + 90;
            length = m_cachedPresetLayout.firstRowPos;
            radius = m_cachedPresetLayout.firstRowRadius;
            break;
        case 1:
            startingAngle = -(triplet * 2 * angleSlice) + 90;
            length = m_cachedPresetLayout.thirdRowPos;
            radius = m_cachedPresetLayout.thirdRowRadius;
            break;
        case 2:
            startingAngle = -((triplet * 2 + 1) * angleSlice) + 90;
            length = m_cachedPresetLayout.secondRowPos;
            radius = m_cachedPresetLayout.secondRowRadius;
            break;
        default:
            KIS_ASSERT(false);
        }
        break;
    }
    default:
        KIS_ASSERT_RECOVER_NOOP(false);
    }
    QPainterPath path;
    qreal pathX = length * qCos(qDegreesToRadians(startingAngle)) - radius;
    qreal pathY = -(length) * qSin(qDegreesToRadians(startingAngle)) - radius;
    qreal pathDiameter = 2 * radius;
    path.addEllipse(pathX, pathY, pathDiameter, pathDiameter);
    return path;
}

void KisPopupPalette::calculateRotationSnapAreas() {
    int i = 0;
    for (QRect &rect: m_snapRects) {
        QPointF point(drawPointOnAngle(i * 15, m_popupPaletteSize / 2 - BORDER_WIDTH - m_snapRadius/2));
        point += QPointF(m_popupPaletteSize / 2 - m_snapRadius, m_popupPaletteSize / 2 - m_snapRadius);
        rect = QRect(point.x(), point.y(), m_snapRadius*2, m_snapRadius*2);
        i++;
    }
    i = 0;
    for (QLineF &line: m_snapLines) {
        qreal penWidth = BORDER_WIDTH / 2;
        QPointF point1 = drawPointOnAngle(i * 15, m_popupPaletteSize / 2 - m_rotationTrackSize + penWidth);
        point1 += QPointF(m_popupPaletteSize / 2, m_popupPaletteSize / 2);
        QPointF point2 = drawPointOnAngle(i * 15, m_popupPaletteSize / 2 - BORDER_WIDTH - penWidth);
        point2 += QPointF(m_popupPaletteSize / 2, m_popupPaletteSize / 2);
        line = QLineF(point1, point2);
        i++;
    }
}

int KisPopupPalette::findPresetSlot(QPointF position) const
{
    QPointF adjustedPoint = position - QPointF(m_popupPaletteSize/2, m_popupPaletteSize/2);
    for (int i = 0; i < m_presetSlotCount; i++) {
        if (createPathFromPresetIndex(i).contains(adjustedPoint)) {
            return i;
        }
    }
    return -1;
}
