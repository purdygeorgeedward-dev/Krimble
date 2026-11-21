/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxDocker_p.h"
#include "KoToolBox_p.h"
#include "KoToolBoxScrollArea_p.h"
#include "KoToolBoxButton_p.h"
#include "KrimbleToolboxIcons.h"
#include <QStylePainter>
#include <QCursor>
#include <QSettings>
#include <QStyleOptionToolButton>

#include <QLabel>
#include <QFontMetrics>
#include <QFrame>
#include <QAction>
#include <QMenu>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QDebug>
#include <KisUsageLogger.h>
#include <QTimer>
#include <QApplication>
#include <QMoveEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>
#include <QEvent>
#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QPaintEvent>

// Full KisKActionCollection definition needed here (forward decl in KoToolManager.h isn't enough) —
// KoToolBoxDocker.cpp calls actionCollection()->action(...), which needs the complete type.
#include <kactioncollection.h>
#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QPointer>
#include <QSignalBlocker>
#include <kis_image.h>
#include <kis_group_layer.h>
#include <kis_selection_mask.h>
#include <kis_node_manager.h>

#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <KoDualColorButton.h>
#include <KisViewManager.h>
#include <kactioncollection.h>
#include <kis_canvas_resource_provider.h>
#include <kis_display_color_converter.h>
#include <kis_canvas2.h>
#include <kis_image.h>
#include <kis_icon_utils.h>

class KrimbleUndoButton : public QToolButton
{
public:
    explicit KrimbleUndoButton(QWidget *parent) : QToolButton(parent) {}
protected:
    void paintEvent(QPaintEvent *) override
    {
        if (KrimbleToolboxIcons::buttonStyle() == 1 && !qApp->property("krimbleNoRaisedButtons").toBool()) {
            KoToolBoxButton::krimblePaintRaisedBox(this, isDown());
        } else if (KrimbleToolboxIcons::glowFace()) {
            QPainter face(this);
            KrimbleToolboxIcons::paintGlowTile(face, QRectF(rect()).adjusted(2, 2, -2, -2));
        } else if (KrimbleToolboxIcons::customFace()) {
            QPainter face(this);
            face.fillRect(rect().adjusted(1, 1, -1, -1), KrimbleToolboxIcons::lightGrayFace());
        }
        QStylePainter stylePainter(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (!isDown()) opt.state &= ~QStyle::State_MouseOver;
        stylePainter.drawComplexControl(QStyle::CC_ToolButton, opt);
    }
};

class KrimbleToolBoxGrip : public QLabel
{
public:
    KrimbleToolBoxGrip(const QString &text, QWidget *parent) : QLabel(text, parent) {}
protected:
    void paintEvent(QPaintEvent *event) override
    {
        QLabel::paintEvent(event);
        QPainter p(this);
        const qreal dpr = devicePixelRatioF();
        const qreal thick = 2.0 / dpr;    // 2 device pixels
        const qreal gap = 6.0 / dpr;      // 6 device pixels between the lines
        // const qreal length = width() * 0.45;
        QColor c = palette().color(QPalette::WindowText);
        c.setAlpha(150);
        p.setPen(Qt::NoPen);
        // for (int i = -1; i <= 1; ++i) {
        //     const qreal y = height() / 2.0 + i * (gap + thick) - thick / 2.0;
        //     p.fillRect(QRectF((width() - length) / 2.0, y, length, thick), c);
        // }
        // for (int i = -4; i <= 4; ++i) {   // nine lines across the whole width
        //     const qreal y = height() / 2.0 + i * (gap + thick) - thick / 2.0;
        //     p.fillRect(QRectF(0, y, width(), thick), c);
        // }
        // an empty lip at the top, then six lines across the whole width ("Add a lip area at the top of the toolbox and lose a few lines of the grab.")
        const qreal lip = 18.0 / dpr;
        const qreal mid = lip + (height() - lip) / 2.0;
        for (int i = 0; i < 6; ++i) {
            const qreal y = mid + (i - 2.5) * (gap + thick) - thick / 2.0;
            p.fillRect(QRectF(0, y, width(), thick), c);
        }
    }
};

class KrimbleToolBoxFoot : public QWidget
{
public:
    explicit KrimbleToolBoxFoot(QWidget *parent) : QWidget(parent) { setFixedHeight(12); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        const qreal thick = 2.0 / devicePixelRatioF();
        QColor c = palette().color(QPalette::WindowText);
        c.setAlpha(150);
        p.fillRect(QRectF(0, height() - thick, width(), thick), c);
    }
};

class KrimbleToolBoxEdge : public QWidget
{
public:
    KrimbleToolBoxEdge(KoToolBoxDocker *dock, bool vertical)
        : QWidget(dock), m_dock(dock), m_vertical(vertical)
    {
        setAttribute(Qt::WA_NoSystemBackground, true);
        setMouseTracking(false);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        return;
#if 0
        QPainter p(this);
        // the edge: a dark line with a light line beside it, and a few dots, so it can be seen and found
        if (m_vertical) {
            p.fillRect(QRect(width() - 6, 0, 1, height()), QColor(0, 0, 0, 170));
            p.fillRect(QRect(width() - 5, 0, 1, height()), QColor(255, 255, 255, 110));
        } else {
            p.fillRect(QRect(0, height() - 6, width(), 1), QColor(0, 0, 0, 170));
            p.fillRect(QRect(0, height() - 5, width(), 1), QColor(255, 255, 255, 110));
        }
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(Qt::NoPen);
        QColor dots = palette().color(QPalette::HighlightedText);
        dots.setAlpha(190);
        p.setBrush(dots);
        for (int i = -1; i <= 1; ++i) {
            if (m_vertical) {
                p.drawEllipse(QPointF(width() - 12, height() / 2.0 + i * 10), 1.6, 1.6);
            } else {
                p.drawEllipse(QPointF(width() / 2.0 + i * 10, height() - 12), 1.6, 1.6);
            }
        }
#endif
    }
    void mousePressEvent(QMouseEvent *event) override
    {
        m_press = event->globalPos();
        m_start = m_dock->size();
        event->accept();
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        m_dock->floatingEdgeDrag(m_start, event->globalPos() - m_press, m_vertical, !m_vertical);
        event->accept();
    }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        event->accept();
    }

private:
    KoToolBoxDocker *m_dock;
    bool m_vertical;
    QPoint m_press;
    QSize m_start;
};

KoToolBoxDocker::KoToolBoxDocker(KoToolBox *toolBox)
    : QDockWidget(i18n("Toolbox"))
    , m_toolBox(toolBox)
    , m_scrollArea(new KoToolBoxScrollArea(toolBox, this))
{
    QWidget *toolBoxContainer = new QWidget(this);
    m_containerLayout = new QVBoxLayout(toolBoxContainer);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_containerLayout->setSpacing(4);
    m_containerLayout->addWidget(m_scrollArea, 0);
    m_containerLayout->addWidget(new KrimbleToolBoxFoot(toolBoxContainer), 0);
    m_containerLayout->addStretch(1);
    setWidget(toolBoxContainer);

    m_snapTimer = new QTimer(this);
    m_snapTimer->setSingleShot(true);
    m_snapTimer->setInterval(250);
    connect(m_snapTimer, &QTimer::timeout, this, &KoToolBoxDocker::snapToColumns);
    m_restoreTimer = new QTimer(this);
    m_restoreTimer->setSingleShot(true);
    m_restoreTimer->setInterval(300);
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
        krimbleLogState(floating ? "topLevelChanged: became floating (before)" : "topLevelChanged: became docked (before)");
        if (floating) {
            if (m_orientation == Auto) setToolBoxOrientation(Qt::Vertical);
            QTimer::singleShot(350, this, [this]() { krimbleLogState("tear-off: 350 ms, before fitHeightToContents"); fitHeightToContents(true); krimbleLogState("tear-off: 350 ms, after"); });
            QTimer::singleShot(900, this, [this]() { krimbleLogState("tear-off: 900 ms, before fitHeightToContents"); fitHeightToContents(true); krimbleLogState("tear-off: 900 ms, after"); });
            QTimer::singleShot(2000, this, [this]() { krimbleLogState("tear-off: 2 s later"); });
        } else {
            m_justDocked = true;
            m_userDragged = false;
            QTimer::singleShot(1500, this, [this]() {
                m_justDocked = false;
                m_userDragged = false;
                applyColumns();
            });
        }
    });
    m_edgeRight = new KrimbleToolBoxEdge(this, true);
    m_edgeBottom = new KrimbleToolBoxEdge(this, false);
    m_edgeRight->hide();
    m_edgeBottom->hide();
    connect(this, &QDockWidget::topLevelChanged, this, &KoToolBoxDocker::setFloatingEdges);
    auto applyContainerStretch = [this](bool floating) {
        Q_UNUSED(floating);
        m_containerLayout->setStretchFactor(m_scrollArea, 1);
        for (int i = 0; i < m_containerLayout->count(); ++i) {
            if (m_containerLayout->itemAt(i)->spacerItem()) m_containerLayout->setStretch(i, 0);
        }
    };
    connect(this, &QDockWidget::topLevelChanged, this, applyContainerStretch);
    QTimer::singleShot(0, this, [this, applyContainerStretch]() { applyContainerStretch(isFloating()); });
    // the toolbox may already be floating when the app starts (saved layout): no signal comes then
    QTimer::singleShot(0, this, [this]() { setFloatingEdges(isFloating()); });
    connect(m_restoreTimer, &QTimer::timeout, this, [this]() {
        applyColumns();
        QTimer::singleShot(500, this, [this]() { m_windowResizing = false; });
    });

    QLabel *w = new KrimbleToolBoxGrip(" ", this);
    w->setFrameShape(QFrame::StyledPanel);
    w->setFrameShadow(QFrame::Raised);
    w->setFrameStyle(QFrame::Panel | QFrame::Raised);
    QFont font = qApp->font();
    // Font size may be in pixels (on Android) or points (everywhere else.)
    qreal ratio = 0.9;
    if (font.pixelSize() == -1) {
        font.setPointSizeF(font.pointSizeF() * ratio);
    } else {
        font.setPixelSize(qRound(font.pixelSize() * ratio));
    }
    int titleSize = QFontMetrics(font).height();
    // w->setMinimumSize(titleSize, titleSize + 12);
    w->setMinimumSize(titleSize, titleSize + 12 + qCeil(18.0 / qMax<qreal>(1.0, qApp->devicePixelRatio())));   // room for the lip
    setTitleBarWidget(w);

    KConfigGroup cfg =  KSharedConfig::openConfig()->group("KoToolBox");
    const int layoutDirUnchecked = cfg.readEntry<int>("layoutDir", Qt::LayoutDirectionAuto);
    switch (layoutDirUnchecked) {
    case Qt::LayoutDirectionAuto:
    case Qt::LeftToRight:
    case Qt::RightToLeft:
        m_layoutDir = static_cast<Qt::LayoutDirection>(layoutDirUnchecked);
        break;
    default:
        m_layoutDir = Qt::LayoutDirectionAuto;
        break;
    }
    updateLayoutDir();

    const int orientUnchecked = cfg.readEntry<int>("orientation", Auto);
    switch (orientUnchecked) {
    case Horizontal:
    case Vertical:
    case Auto:
        m_orientation = static_cast<Orientation>(orientUnchecked);
        break;
    default:
        m_orientation = Auto;
        break;
    }
    if (m_orientation != Auto) {
        setToolBoxOrientation(static_cast<Qt::Orientation>(m_orientation));
    }

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(updateToolBoxOrientation(Qt::DockWidgetArea)));
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(updateFloating(bool)));
}

void KoToolBoxDocker::setCanvas(KoCanvasBase *canvas)
{
    Q_UNUSED(canvas);
}

void KoToolBoxDocker::unsetCanvas()
{
}

// Krimble: the two simple gadget icons, drawn in code in the toolbox icon color.
// Quick Mask: a rectangle with a circle in it. Screen Mode: a screen with a second, smaller screen behind it.
static QIcon krimbleGadgetIcon(bool quickMask)
{
    QIcon icon;
    for (int size : {28, 56, 84}) {
        QPixmap pm(size, size);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        const qreal u = size / 28.0;
        const QColor c(225, 228, 240);
        p.setPen(QPen(c, 2.0 * u));
        p.setBrush(Qt::NoBrush);
        if (quickMask) {
            p.drawRect(QRectF(3 * u, 6 * u, 22 * u, 16 * u));
            p.setBrush(c);
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(14 * u, 14 * u), 4.5 * u, 4.5 * u);
        } else {
            p.drawRect(QRectF(9 * u, 4 * u, 16 * u, 12 * u));
            p.setBrush(QColor(56, 62, 96));
            p.drawRect(QRectF(3 * u, 10 * u, 16 * u, 12 * u));
        }
        p.end();
        icon.addPixmap(pm);
    }
    return icon;
}

void KoToolBoxDocker::setViewManager(KisViewManager *viewManager)
{
    m_toolBox->setViewManager(viewManager);
    m_viewManager = viewManager;

    {
        KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
        m_columns = qBound(1, cfg.readEntry("ToolBoxColumns", 2), 40);
        if (m_viewManager) {
            if (QWidget *window = m_viewManager->mainWindowAsQWidget()) {
                window->installEventFilter(this);
            }
        }
        m_windowResizing = true;
        m_restoreTimer->start(1200);
    }

    if (!m_dualColorButton && viewManager) {
        const KoColorDisplayRendererInterface *displayRenderer =
            KisDisplayColorConverter::dumbConverterInstance()->displayRendererInterface();
        m_dualColorButton = new KoDualColorButton(viewManager->canvasResourceProvider(), displayRenderer,
                                                    viewManager->mainWindowAsQWidget(), viewManager->mainWindowAsQWidget());
        m_dualColorButton->setFixedSize(28, 28);
        QWidget *trailingBox = new QWidget(m_toolBox);
        QVBoxLayout *trailingLayout = new QVBoxLayout(trailingBox);
        trailingLayout->setContentsMargins(0, 4, 0, 0);
        trailingLayout->setSpacing(4);
        trailingLayout->addWidget(m_dualColorButton, 0, Qt::AlignHCenter);
        {
            // Krimble: the two gadgets under the color swatches, as in the industry-standard toolbox
            // (docs/krimble-refs/2026-11-21_0428_ps-toolbox-bottom-quickmask-screenmode.jpg): Quick Mask on the left,
            // Screen Mode on the right, with very simple icons.
            QHBoxLayout *gadgets = new QHBoxLayout();
            gadgets->setContentsMargins(0, 4, 0, 0);
            gadgets->setSpacing(4);
            QToolButton *quickMask = new QToolButton(trailingBox);
            quickMask->setObjectName(QStringLiteral("krimbleQuickMaskButton"));
            quickMask->setAutoRaise(true);
            quickMask->setCheckable(true);
            // quickMask->setIcon(krimbleGadgetIcon(true));
            quickMask->setProperty("krimbleDrawnIcon", krimbleGadgetIcon(true));
            KrimbleToolboxIcons::refreshGadget(quickMask, QStringLiteral("08_02_quick_mask"));
            quickMask->setIconSize(QSize(28, 28));
            quickMask->setToolTip(i18n("Quick Mask: paint the selection"));
            QToolButton *screenMode = new QToolButton(trailingBox);
            screenMode->setObjectName(QStringLiteral("krimbleScreenModeButton"));
            screenMode->setAutoRaise(true);
            // screenMode->setIcon(krimbleGadgetIcon(false));
            screenMode->setProperty("krimbleDrawnIcon", krimbleGadgetIcon(false));
            KrimbleToolboxIcons::refreshGadget(screenMode, QStringLiteral("08_03_screen_mode"));
            screenMode->setIconSize(QSize(28, 28));
            screenMode->setToolTip(i18n("Screen Mode: the picture only (tap the X to come back)"));
            gadgets->addWidget(quickMask, 0, Qt::AlignHCenter);
            gadgets->addWidget(screenMode, 0, Qt::AlignHCenter);
            trailingLayout->addLayout(gadgets);

            QPointer<KisViewManager> vm = viewManager;
            connect(quickMask, &QToolButton::toggled, this, [vm, quickMask](bool on) {
                if (!vm) return;
                QAction *show = vm->actionCollection()->action(QStringLiteral("show-global-selection-mask"));
                if (!show) return;
                KisImageWSP image = vm->image();
                if (on && image && !image->globalSelection()) {
                    // nothing selected yet: start from an empty selection, painted in from nothing
                    if (QAction *all = vm->actionCollection()->action(QStringLiteral("select_all"))) all->trigger();
                    if (QAction *inv = vm->actionCollection()->action(QStringLiteral("invert_selection"))) inv->trigger();
                }
                if (show->isChecked() != on) show->trigger();
                if (on) {
                    QTimer::singleShot(200, quickMask, [vm]() {
                        if (!vm || !vm->image() || !vm->nodeManager()) return;
                        KisSelectionMaskSP mask = vm->image()->rootLayer()->selectionMask();
                        if (mask) vm->nodeManager()->slotNonUiActivatedNode(mask);   // paint on the mask: white selects, black deselects
                    });
                }
            });
            if (QAction *show = viewManager->actionCollection()->action(QStringLiteral("show-global-selection-mask"))) {
                quickMask->setChecked(show->isChecked());
                connect(show, &QAction::toggled, quickMask, [quickMask](bool on) {
                    if (quickMask->isChecked() == on) return;
                    QSignalBlocker block(quickMask);
                    quickMask->setChecked(on);   // stays in step when the mask is shown or hidden from a menu
                });
            }
            connect(screenMode, &QToolButton::clicked, this, [vm]() {
                if (!vm) return;
                if (QAction *canvasOnly = vm->actionCollection()->action(QStringLiteral("view_show_canvas_only"))) canvasOnly->trigger();
            });
        }
        m_toolBox->toolBoxLayout()->setTrailingWidget(trailingBox);

        QTimer::singleShot(0, this, [this, viewManager]() {
            QAction *undoAction = viewManager->actionCollection()->action("edit_undo");
            QAction *redoAction = viewManager->actionCollection()->action("edit_redo");
            if (undoAction) {
                QToolButton *undoButton = new KrimbleUndoButton(m_toolBox);
                undoButton->setObjectName(QStringLiteral("krimbleUndoButton"));
                undoButton->setAutoRaise(true);
                undoButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
                // undoButton->setIcon(KisIconUtils::loadIcon("krita_tool_undo"));
                undoButton->setProperty("krimbleDrawnIcon", KisIconUtils::loadIcon("krita_tool_undo"));
                KrimbleToolboxIcons::refreshGadget(undoButton, QStringLiteral("08_01_undo"));
                undoButton->setToolTip(i18n("Undo (long press for Redo)"));
                connect(undoButton, &QToolButton::clicked, undoAction, &QAction::trigger);
                if (redoAction) {
                    undoButton->setContextMenuPolicy(Qt::ActionsContextMenu);
                    undoButton->addAction(redoAction);
                }
                m_toolBox->addActionButton(undoButton, 99 * 1000000);
            }
        // }
        });

        connect(m_dualColorButton, SIGNAL(foregroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetFGColor(KoColor)));
        connect(m_dualColorButton, SIGNAL(backgroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetBGColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigBGColorChanged(KoColor)), m_dualColorButton, SLOT(setBackgroundColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigFGColorChanged(KoColor)), m_dualColorButton, SLOT(setForegroundColor(KoColor)));

        connect(viewManager, &KisViewManager::viewChanged, this, &KoToolBoxDocker::slotUpdateDisplayRenderer);
        slotUpdateDisplayRenderer();

        QAction *canvasOnlyAction = viewManager->actionCollection()->action("view_show_canvas_only");
    }
}

void KoToolBoxDocker::slotUpdateDisplayRenderer()
{
    if (!m_dualColorButton || !m_viewManager) return;
    if (m_viewManager->canvasBase()) {
        m_dualColorButton->setDisplayRenderer(m_viewManager->canvasBase()->displayColorConverter()->displayRendererInterface());
        m_dualColorButton->updateColorSpace();
        m_viewManager->canvasBase()->image()->disconnect(m_dualColorButton);
        connect(m_viewManager->canvasBase()->image(), SIGNAL(sigColorSpaceChanged(const KoColorSpace*)), m_dualColorButton, SLOT(updateColorSpace()), Qt::UniqueConnection);
    } else if (m_viewManager->viewCount() == 0) {
        m_dualColorButton->setDisplayRenderer();
    }
}

void KoToolBoxDocker::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);
    if (m_orientation == Auto && !isFloating()) {
        if (m_dockArea == Qt::TopDockWidgetArea || m_dockArea == Qt::BottomDockWidgetArea) {
            setToolBoxOrientation(Qt::Horizontal);
        } else if (m_dockArea == Qt::LeftDockWidgetArea || m_dockArea == Qt::RightDockWidgetArea) {
            setToolBoxOrientation(Qt::Vertical);
        } else {
            setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
        }
    }
    if (m_dualColorButton && m_scrollArea) {
        const int wellSize = qBound(28, m_scrollArea->viewport()->width() - 8, 72);
        if (m_dualColorButton->width() != wellSize) {
            m_dualColorButton->setFixedSize(wellSize, wellSize);
            if (m_toolBox && m_toolBox->layout()) m_toolBox->layout()->invalidate();
        }
    }
    if (m_edgeRight && m_edgeBottom && isFloating()) {
        const int e = 10;
        m_edgeRight->setGeometry(width() - e, 0, e, height() - e);
        m_edgeBottom->setGeometry(0, height() - e, width(), e);
        m_edgeRight->raise();
        m_edgeBottom->raise();
    }
    if (m_justDocked) {
    } else if (m_snapTimer && !m_windowResizing && !m_applying && !isFloating() && QApplication::mouseButtons() != Qt::NoButton) {
        if (krimbleFingerAtToolbox()) m_snapTimer->start();
    } else if (m_restoreTimer && !m_windowResizing && !m_applying && !isFloating() && isVisible() && QApplication::mouseButtons() == Qt::NoButton) {
        m_restoreTimer->start();
    }

    auto krimbleFitToolArea = [this]() {
        if (!m_scrollArea || !m_toolBox || !m_toolBox->layout()) return;
        if (!m_toolBox->layout()->hasHeightForWidth()) {
            if (!isFloating()) {
                const int rowsHeight = m_toolBox->sizeHint().height();
                const int frame = m_scrollArea->height() - m_scrollArea->viewport()->height();
                int others = qMax(0, height() - (widget() ? widget()->height() : height()));
                if (m_containerLayout) {
                    const QMargins cm = m_containerLayout->contentsMargins();
                    others += cm.top() + cm.bottom();
                }
                if (rowsHeight > 0) {
                    m_scrollArea->setMaximumHeight(rowsHeight + frame);
                    setMaximumHeight(rowsHeight + frame + others);
                }
            } else {
                m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
                setMaximumHeight(QWIDGETSIZE_MAX);
            }
            return;
        }
        if (!isFloating()) {
            m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
            setMaximumHeight(QWIDGETSIZE_MAX);
            return;
        }
        const int contentHeight = qMax(m_toolBox->layout()->heightForWidth(m_scrollArea->viewport()->width()), m_toolBox->height());
        if (contentHeight > 0) {
            const int frame = m_scrollArea->height() - m_scrollArea->viewport()->height();
            m_scrollArea->setMaximumHeight(contentHeight + frame);
            const int others = qMax(0, height() - m_scrollArea->height());
            setMaximumHeight(contentHeight + frame + others);
        }
    };
    QTimer::singleShot(0, this, krimbleFitToolArea);
    QTimer::singleShot(400, this, krimbleFitToolArea);
    QTimer::singleShot(1500, this, krimbleFitToolArea);
}

bool KoToolBoxDocker::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Resize && m_viewManager && watched == m_viewManager->mainWindowAsQWidget()) {
        m_windowResizing = true;
        if (m_snapTimer) m_snapTimer->stop();
        if (m_restoreTimer) m_restoreTimer->start();
    }
    return QDockWidget::eventFilter(watched, event);
}

void KoToolBoxDocker::setFloatingEdges(bool floating)
{
    const int e = 10;
    // The 24 px margin version, kept, not deleted:
    // const bool has = property("krimbleToolBoxEdges").toBool();
    // if (floating && !has) {
    //     m.setRight(m.right() + e);
    //     m.setBottom(m.bottom() + e);
    //     setContentsMargins(m);
    //     setProperty("krimbleToolBoxEdges", true);
    // } else if (!floating && has) {
    //     m.setRight(qMax(0, m.right() - e));
    //     m.setBottom(qMax(0, m.bottom() - e));
    //     setContentsMargins(m);
    //     setProperty("krimbleToolBoxEdges", false);
    // }
    m_edgeRight->setVisible(floating);
    m_edgeBottom->setVisible(floating);
    if (floating) {
        m_edgeRight->setGeometry(width() - e, 0, e, height() - e);
        m_edgeBottom->setGeometry(0, height() - e, width(), e);
        m_edgeRight->raise();
        m_edgeBottom->raise();
    }
}

void KoToolBoxDocker::floatingEdgeDrag(const QSize &startSize, const QPoint &delta, bool changeWidth, bool changeHeight)
{
    int w = startSize.width();
    int h = startSize.height();
    if (changeWidth) {
        // whole icon columns only (1 to 4): the icons plus everything around them
        const int icons = iconWidth();
        const int around = chromeWidth();
        const int columns = qBound(1, qRound(qreal(startSize.width() - around + delta.x()) / icons), 40);
        w = columns * icons + around;
        if (columns != m_columns) {
            m_columns = columns;
            KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
            cfg.writeEntry("ToolBoxColumns", m_columns);
            cfg.sync();
        }
    }
    if (changeHeight) {
        h = qMax(160, startSize.height() + delta.y());
    }
    if (w != width() || h != height()) {
        resize(w, h);
    }
}

int KoToolBoxDocker::iconWidth() const
{
    // the toolbox's smallest width is one icon
    return qMax(1, m_toolBox->minimumSizeHint().width());
}

int KoToolBoxDocker::chromeWidth() const
{
    // everything that is not icons: panel frame, margins, a scroll bar if there is one
    return qMax(0, width() - m_scrollArea->viewport()->width());
}

// void KoToolBoxDocker::applyColumns()   (the first version, kept, not deleted: it only worked docked at the left or right)
// {
//     QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
//     if (!mainWindow || isFloating() || !isVisible()) return;
//     const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
//     if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) return;
//
//     const int target = m_columns * iconWidth() + chromeWidth();
//     if (qAbs(width() - target) < 2) return;
//
//     m_applying = true;
//     mainWindow->resizeDocks({this}, {target}, Qt::Horizontal);
//     QTimer::singleShot(400, this, [this]() { m_applying = false; });
// }

QSize KoToolBoxDocker::sizeHint() const
{
    QSize hint = QDockWidget::sizeHint();
    if (m_toolBox && m_toolBox->layout() && m_toolBox->layout()->hasHeightForWidth()) {
        hint.setWidth(qMax(60, m_columns * iconWidth() + chromeWidth()));
    }
    return hint;
}

void KoToolBoxDocker::moveEvent(QMoveEvent *event)
{
    QDockWidget::moveEvent(event);
    if (!isFloating()) return;
    if (!m_screenTimer) {
        m_screenTimer = new QTimer(this);
        m_screenTimer->setSingleShot(true);
        m_screenTimer->setInterval(350);
        connect(m_screenTimer, &QTimer::timeout, this, [this]() {
            if (!isFloating() || !isVisible()) return;
            if (QApplication::mouseButtons() != Qt::NoButton) {   // the finger is still down: look again later
                m_screenTimer->start();
                return;
            }
            krimbleDockAtEdge();
            if (!isFloating()) return;
            QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen();
            if (!screenPtr) return;
            QRect avail = screenPtr->availableGeometry();
            avail.setTop(avail.top() + 48);        // keep a strip free at the top, as the dialogs do
            QRect g = geometry();
            g.setWidth(qMin(g.width(), avail.width()));
            if (g.top() > avail.bottom() - 36) g.moveTop(avail.bottom() - 36);
            if (g != geometry()) setGeometry(g);
        });
    }
    m_screenTimer->start();
}

#if 0
void KoToolBoxDocker::krimbleDockAtEdge()
{
    if (!isFloating() || !isVisible()) return;
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
    if (!mainWindow) return;
    QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen();
    if (!screenPtr) return;
    QRect avail = screenPtr->availableGeometry();
    avail.setTop(avail.top() + 48);
    const QRect g = geometry();
    Qt::DockWidgetArea area = Qt::NoDockWidgetArea;
    if (g.left() <= avail.left() + 4) {
        area = Qt::LeftDockWidgetArea;
    } else if (g.right() >= avail.right() - 4) {
        area = Qt::RightDockWidgetArea;
    } else if (g.top() <= avail.top() - 48 + 4) {
        area = Qt::TopDockWidgetArea;
    }
    if (area == Qt::NoDockWidgetArea) return;
    setAllowedAreas(Qt::AllDockWidgetAreas);
    mainWindow->addDockWidget(area, this);
    setFloating(false);
    show();
}
#endif

static bool krimbleDragDockingOn()
{
    QVariant v = qApp->property("krimbleDragDocking");
    if (!v.isValid()) {
        v = QSettings().value(QStringLiteral("krimble/DragDocking"), true).toBool();
        qApp->setProperty("krimbleDragDocking", v);
    }
    return v.toBool();
}

Qt::DockWidgetArea KoToolBoxDocker::krimbleEdgeArea() const
{
    QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen();
    if (!screenPtr) return Qt::NoDockWidgetArea;
    QRect avail = screenPtr->availableGeometry();
    avail.setTop(avail.top() + 48);
    const QRect g = geometry();
    if (g.left() <= avail.left() + 4) return Qt::LeftDockWidgetArea;
    if (g.right() >= avail.right() - 4) return Qt::RightDockWidgetArea;
    if (g.top() <= avail.top() - 48 + 4) return Qt::TopDockWidgetArea;
    return Qt::NoDockWidgetArea;
}

void KoToolBoxDocker::krimbleDockAtEdge()
{
    if (!isFloating() || !isVisible()) return;
    if (!krimbleDragDockingOn()) { ++m_edgeDockToken; return; }
    const Qt::DockWidgetArea area = krimbleEdgeArea();
    const int token = ++m_edgeDockToken;
    if (area == Qt::NoDockWidgetArea) return;
    QTimer::singleShot(1000, this, [this, token, area]() {
        if (token != m_edgeDockToken || !isFloating() || !isVisible()) return;
        if (QApplication::mouseButtons() != Qt::NoButton) return;     // a finger is down again: the screen timer looks again when it lifts
        if (krimbleEdgeArea() != area) return;                         // it was moved away from the edge meanwhile
        QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
        if (!mainWindow) return;
        setAllowedAreas(Qt::AllDockWidgetAreas);
        mainWindow->addDockWidget(area, this, (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) ? Qt::Horizontal : Qt::Vertical);
        setFloating(false);
        show();
    });
}

void KoToolBoxDocker::fitHeightToContents(bool setWidthToo)
{
    if (!isFloating() || !isVisible() || !m_toolBox || !m_toolBox->layout() || !m_toolBox->layout()->hasHeightForWidth()) return;
    if (!widget() || !widget()->layout()) return;
    const int columnsWidth = qMax(1, m_columns) * iconWidth();
    const int toolsHeight = m_toolBox->layout()->heightForWidth(setWidthToo ? columnsWidth : m_scrollArea->viewport()->width());
    if (toolsHeight <= 0) return;
    const QMargins margins = widget()->layout()->contentsMargins();
    const int titleAndFrame = height() - widget()->height();
    const int scrollFrame = m_scrollArea->height() - m_scrollArea->viewport()->height();
    int target = titleAndFrame + margins.top() + margins.bottom() + scrollFrame + toolsHeight;
    if (QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen()) {
        target = qMin(target, int(screenPtr->availableGeometry().height() * 0.95));
    }
    if (target > 0) {
        setMaximumHeight(target);
    }
    int wantedWidth = setWidthToo ? columnsWidth + chromeWidth() : width();
    const int shortfall = setWidthToo ? columnsWidth - m_scrollArea->viewport()->width() : 0;
    if (shortfall > 0) wantedWidth = qMax(wantedWidth, width() + shortfall);
    if (target > 0 && (qAbs(height() - target) > 2 || shortfall > 0 || qAbs(width() - wantedWidth) > 2)) {
        m_applying = true;
        resize(wantedWidth, target);
        QTimer::singleShot(400, this, [this]() { m_applying = false; });
    }
}

void KoToolBoxDocker::krimbleLogState(const char *where) const
{
    KisUsageLogger::log(QStringLiteral("KRIMBLE-TOOLBOX %1 | floating %2 | dockArea %3 | orientation setting %4 layout %5 | columns %6 | size %7 x %8 | iconWidth %9 chromeWidth %10 | justDocked %11 applying %12 userDragged %13 | viewport %14 columnsNeed %15")
        .arg(QString::fromLatin1(where))
        .arg(isFloating())
        .arg(int(m_dockArea))
        .arg(int(m_orientation))
        .arg(m_scrollArea ? (m_scrollArea->orientation() == Qt::Horizontal ? QStringLiteral("horizontal") : QStringLiteral("vertical")) : QStringLiteral("none"))
        .arg(m_columns)
        .arg(width()).arg(height())
        .arg(iconWidth()).arg(chromeWidth())
        .arg(m_justDocked).arg(m_applying).arg(m_userDragged).arg(m_scrollArea ? m_scrollArea->viewport()->width() : -1).arg(qMax(1, m_columns) * iconWidth()));
}

void KoToolBoxDocker::applyColumns()
{
    krimbleLogState("applyColumns (start)");
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
    if (!mainWindow || !isVisible() || !m_toolBox || !m_toolBox->layout()) return;
    const bool columns = m_toolBox->layout()->hasHeightForWidth();   // true: vertical layout (columns); false: rows

    if (columns) {
        const int target = m_columns * iconWidth() + chromeWidth();
        if (qAbs(width() - target) < 2) return;
        if (isFloating()) {
            m_applying = true;
            resize(target, height());
            QTimer::singleShot(400, this, [this]() { m_applying = false; });
            QTimer::singleShot(450, this, [this]() { fitHeightToContents(); });
        } else {
            const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
            if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) return;
            m_applying = true;
            mainWindow->resizeDocks({this}, {target}, Qt::Horizontal);
            setMinimumWidth(target);
            setMaximumWidth(target);
            QTimer::singleShot(250, this, [this]() {
                setMinimumWidth(0);
                setMaximumWidth(QWIDGETSIZE_MAX);
            });
            QTimer::singleShot(400, this, [this]() { m_applying = false; });
        }
    } else {
        const int rowHeight = qMax(1, m_toolBox->minimumSizeHint().height());
        const QMargins containerMargins = widget() && widget()->layout() ? widget()->layout()->contentsMargins() : QMargins();
        const int dockFrame = widget() ? qMax(0, height() - widget()->height()) : 0;
        const int scrollFrame = qMax(0, m_scrollArea->height() - m_scrollArea->viewport()->height());
        const int chromeHeight = dockFrame + containerMargins.top() + containerMargins.bottom() + scrollFrame;
        const int target = m_columns * rowHeight + chromeHeight;
        if (qAbs(height() - target) < 2) return;
        if (isFloating()) {
            m_applying = true;
            resize(width(), target);
            QTimer::singleShot(400, this, [this]() { m_applying = false; });
        } else {
            const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
            if (area != Qt::TopDockWidgetArea && area != Qt::BottomDockWidgetArea) return;
            m_applying = true;
            mainWindow->resizeDocks({this}, {target}, Qt::Vertical);
            setMinimumHeight(target);
            setMaximumHeight(target);
            QTimer::singleShot(250, this, [this]() {
                setMinimumHeight(0);
                setMaximumHeight(QWIDGETSIZE_MAX);
            });
            QTimer::singleShot(400, this, [this]() { m_applying = false; });
        }
    }
}

bool KoToolBoxDocker::krimbleFingerAtToolbox() const
{
    const QPoint p = mapFromGlobal(QCursor::pos());
    return rect().adjusted(-48, -48, 48, 48).contains(p);
}

void KoToolBoxDocker::snapToColumns()
{
    if (isFloating() || !isVisible()) return;
    if (m_justDocked) { m_userDragged = false; return; }
    if (m_windowResizing || m_applying) return;
    // do not fight a finger that is still down on the separator
    if (QApplication::mouseButtons() != Qt::NoButton) {
        if (!krimbleFingerAtToolbox()) return;
        m_userDragged = true;
        m_snapTimer->start();
        return;
    }
    const Qt::DockWidgetArea area = m_dockArea;
    if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) return;

    const int columns = qBound(1, qRound(qreal(m_scrollArea->viewport()->width()) / iconWidth()), 40);
    if (m_userDragged && columns != m_columns) {
        m_columns = columns;
        KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
        cfg.writeEntry("ToolBoxColumns", m_columns);
        cfg.sync();
    }
    m_userDragged = false;
    applyColumns();
}

void KoToolBoxDocker::updateToolBoxOrientation(Qt::DockWidgetArea area)
{
    m_dockArea = area;
    updateLayoutDir();
    if (m_orientation == Auto) {
        if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
            setToolBoxOrientation(Qt::Horizontal);
        } else if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
            setToolBoxOrientation(Qt::Vertical);
        } else if (!isFloating()) {
            setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
        }
    }
}

void KoToolBoxDocker::updateLayoutDir()
{
    if (m_layoutDir == Qt::LayoutDirectionAuto) {
        if (m_dockArea == Qt::RightDockWidgetArea) {
            m_scrollArea->setLayoutDirection(Qt::RightToLeft);
        } else if (m_dockArea == Qt::LeftDockWidgetArea) {
            m_scrollArea->setLayoutDirection(Qt::LeftToRight);
        } else {
            m_scrollArea->unsetLayoutDirection();
        }
    } else {
        m_scrollArea->setLayoutDirection(m_layoutDir);
    }
}

void KoToolBoxDocker::changeLayoutDir(Qt::LayoutDirection dir)
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<int>("layoutDir", dir);
    m_layoutDir = dir;
    updateLayoutDir();
}

void KoToolBoxDocker::changeOrientation(const Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<int>("orientation", orientation);
    m_orientation = orientation;
    if (m_orientation == Auto) {
        setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
    } else {
        setToolBoxOrientation(static_cast<Qt::Orientation>(m_orientation));
    }
}

void KoToolBoxDocker::changeCompact(const bool state)
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<bool>("compact", state);
    m_scrollArea->setCompact(state);
}

void KoToolBoxDocker::setToolBoxOrientation(Qt::Orientation orientation)
{
    if (m_scrollArea->orientation() == orientation) {
        return;
    }
    krimbleLogState(orientation == Qt::Horizontal ? "setToolBoxOrientation: changing to HORIZONTAL (before)" : "setToolBoxOrientation: changing to vertical (before)");
    if (orientation == Qt::Horizontal) {
        setFeatures(features() | QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Horizontal);
    } else {
        setFeatures(features() & ~QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Vertical);
    }
    if (isFloating()) {
        QTimer::singleShot(200, this, [this]() { krimbleFitFloatingShape(); });
    }
}

void KoToolBoxDocker::krimbleFitFloatingShape()
{
    if (!isFloating() || !isVisible() || !m_toolBox || !m_scrollArea || !widget() || !widget()->layout()) return;
    if (m_scrollArea->orientation() != Qt::Horizontal) {
        setMaximumHeight(QWIDGETSIZE_MAX);
        m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
        fitHeightToContents();
        return;
    }
    const QSize tools = m_toolBox->sizeHint();
    if (tools.isEmpty()) return;
    const QMargins m = widget()->layout()->contentsMargins();
    const int titleW = qMax(0, width() - widget()->width());        // the title bar is at the side in the horizontal layout
    const int titleH = qMax(0, height() - widget()->height());
    const int frameW = m_scrollArea->width() - m_scrollArea->viewport()->width();
    const int frameH = m_scrollArea->height() - m_scrollArea->viewport()->height();
    int w = titleW + m.left() + m.right() + frameW + tools.width();
    int h = titleH + m.top() + m.bottom() + frameH + tools.height();
    if (QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen()) {
        const QRect avail = screenPtr->availableGeometry();
        if (w > int(avail.width() * 0.95)) {
            w = int(avail.width() * 0.95);
            h += style()->pixelMetric(QStyle::PM_ScrollBarExtent);      // room for the sideways scroll bar
        }
        h = qMin(h, int(avail.height() * 0.95));
    }
    m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
    setMaximumHeight(h);
    m_applying = true;
    resize(w, h);
    QTimer::singleShot(400, this, [this]() { m_applying = false; });
}

void KoToolBoxDocker::updateFloating(bool v)
{
    m_toolBox->setFloating(v);
}

void KoToolBoxDocker::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_contextMenu) {
        m_contextMenu = new QMenu(this);

        QAction *compact = m_contextMenu->addAction(i18n("Compact"));
        compact->setCheckable(true);
        compact->setChecked(m_toolBox->isCompact());
        connect(compact, &QAction::triggered, this, &KoToolBoxDocker::changeCompact);

        m_contextMenu->addSection(i18n("Icon Size"));
        m_toolBox->setupIconSizeMenu(m_contextMenu);

        m_contextMenu->addSection(i18n("Icon Style"));
        m_toolBox->setupIconStyleMenu(m_contextMenu);

        m_contextMenu->addSection(i18n("Button Style"));
        m_toolBox->setupButtonStyleMenu(m_contextMenu);

        m_contextMenu->addSection(i18nc("Toolbox layout", "Layout"));
        QActionGroup *layoutActionGroup = new QActionGroup(m_contextMenu);

        QAction *layoutAuto = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Automatic"));
        layoutAuto->setActionGroup(layoutActionGroup);
        layoutAuto->setCheckable(true);
        connect(layoutAuto, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::LayoutDirectionAuto);
        });

        QAction *layoutLtr = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Left-to-right"));
        layoutLtr->setActionGroup(layoutActionGroup);
        layoutLtr->setCheckable(true);
        connect(layoutLtr, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::LeftToRight);
        });

        QAction *layoutRtl = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Right-to-left"));
        layoutRtl->setActionGroup(layoutActionGroup);
        layoutRtl->setCheckable(true);
        connect(layoutRtl, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::RightToLeft);
        });

        switch (m_layoutDir) {
        case Qt::LayoutDirectionAuto:
            layoutAuto->setChecked(true);
            break;
        case Qt::LeftToRight:
            layoutLtr->setChecked(true);
            break;
        case Qt::RightToLeft:
            layoutRtl->setChecked(true);
            break;
        }

        m_contextMenu->addSection(i18nc("Toolbox orientation", "Orientation"));
        QActionGroup *orientActionGroup = new QActionGroup(m_contextMenu);

        QAction *orientAuto = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Automatic"));
        orientAuto->setActionGroup(orientActionGroup);
        orientAuto->setCheckable(true);
        connect(orientAuto, &QAction::triggered, this, [this]() {
            changeOrientation(Auto);
        });

        QAction *orientHorizontal = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Horizontal"));
        orientHorizontal->setActionGroup(orientActionGroup);
        orientHorizontal->setCheckable(true);
        connect(orientHorizontal, &QAction::triggered, this, [this]() {
            changeOrientation(Horizontal);
        });

        QAction *orientVertical = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Vertical"));
        orientVertical->setActionGroup(orientActionGroup);
        orientVertical->setCheckable(true);
        connect(orientVertical, &QAction::triggered, this, [this]() {
            changeOrientation(Vertical);
        });

        switch (m_orientation) {
        case Horizontal:
            orientHorizontal->setChecked(true);
            break;
        case Vertical:
            orientVertical->setChecked(true);
            break;
        case Auto:
            orientAuto->setChecked(true);
            break;
        }

        m_contextMenu->addSection(i18n("Columns"));
        QActionGroup *columnsActionGroup = new QActionGroup(m_contextMenu);
        for (int i = 0; i < 4; ++i) {
            QAction *columnsAction = m_contextMenu->addAction(i18np("1 column", "%1 columns", i + 1));
            columnsAction->setActionGroup(columnsActionGroup);
            columnsAction->setCheckable(true);
            m_columnActions[i] = columnsAction;
            connect(columnsAction, &QAction::triggered, this, [this, i]() {
                krimbleLogState(qPrintable(QStringLiteral("Columns menu: picked %1 (before)").arg(i + 1)));
                m_columns = i + 1;
                KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
                cfg.writeEntry("ToolBoxColumns", m_columns);
                cfg.sync();
                applyColumns();
            });
        }

        m_contextMenu->addSection(i18n("Dock"));
        const struct { QString text; Qt::DockWidgetArea area; } dockEntries[] = {
            {i18n("Dock Left"), Qt::LeftDockWidgetArea},
            {i18n("Dock Right"), Qt::RightDockWidgetArea},
            {i18n("Dock Top"), Qt::TopDockWidgetArea},
            {i18n("Dock Bottom"), Qt::BottomDockWidgetArea}
        };
        for (int i = 0; i < 4; ++i) {
            QAction *dockAction = m_contextMenu->addAction(dockEntries[i].text);
            dockAction->setCheckable(true);
            m_dockActions[i] = dockAction;
            const Qt::DockWidgetArea area = dockEntries[i].area;
            connect(dockAction, &QAction::triggered, this, [this, area]() {
                if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
                    setAllowedAreas(Qt::AllDockWidgetAreas);
                    mainWindow->addDockWidget(area, this, (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) ? Qt::Horizontal : Qt::Vertical);
                    if (isFloating()) {
                        setFloating(false);
                    }
                    show();
                }
            });
        }
        m_dockActions[4] = m_contextMenu->addAction(i18n("Undock"));
        m_dockActions[4]->setCheckable(true);
        connect(m_dockActions[4], &QAction::triggered, this, [this]() { setFloating(true); });
        m_dragDockAction = m_contextMenu->addAction(i18n("Dock by Dragging"));
        m_dragDockAction->setCheckable(true);
        connect(m_dragDockAction, &QAction::triggered, this, [](bool on) {
            QSettings().setValue(QStringLiteral("krimble/DragDocking"), on);
            qApp->setProperty("krimbleDragDocking", on);
        });
    }

    if (m_dragDockAction) m_dragDockAction->setChecked(krimbleDragDockingOn());
    for (int i = 0; i < 4; ++i) {
        if (m_columnActions[i]) m_columnActions[i]->setChecked(m_columns == i + 1);
    }

    {
        QMainWindow *dockMainWindow = qobject_cast<QMainWindow*>(parentWidget());
        const Qt::DockWidgetArea dockCurrent = (dockMainWindow && !isFloating()) ? dockMainWindow->dockWidgetArea(this) : Qt::NoDockWidgetArea;
        const Qt::DockWidgetArea dockAreas[4] = {Qt::LeftDockWidgetArea, Qt::RightDockWidgetArea, Qt::TopDockWidgetArea, Qt::BottomDockWidgetArea};
        for (int i = 0; i < 4; ++i) {
            if (m_dockActions[i]) {
                m_dockActions[i]->setChecked(dockCurrent == dockAreas[i]);
                m_dockActions[i]->setEnabled(true);
            }
        }
        if (m_dockActions[4]) m_dockActions[4]->setChecked(isFloating());
    }

    m_contextMenu->exec(event->globalPos());
}
