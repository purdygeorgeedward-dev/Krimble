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

#include <QLabel>
#include <QFontMetrics>
#include <QFrame>
#include <QAction>
#include <QMenu>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QApplication>
#include <QMoveEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>   // Krimble 2026-10-19: krimbleFitFloatingShape (the scroll bar extent)
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>

// Full KisKActionCollection definition needed here (forward decl in KoToolManager.h isn't enough) —
// KoToolBoxDocker.cpp calls actionCollection()->action(...), which needs the complete type.
#include <kactioncollection.h>
#include <QWidget>
#include <QToolButton>

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

// Krimble 2026-10-11: the toolbox's drag handle (its empty title bar) shows three light horizontal lines, so it is obvious
// that it is meant to be grabbed.
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
        const qreal length = width() * 0.45;
        QColor c = palette().color(QPalette::WindowText);
        c.setAlpha(150);
        p.setPen(Qt::NoPen);
        for (int i = -1; i <= 1; ++i) {
            const qreal y = height() / 2.0 + i * (gap + thick) - thick / 2.0;
            p.fillRect(QRectF((width() - length) / 2.0, y, length, thick), c);
        }
    }
};

// KRIMBLE 2026-10-07: an edge strip of the floating toolbox. Dragging it changes the toolbox's width (right strip) or
// height (bottom strip). It lives in a margin the toolbox gets while it floats, so it covers none of the tool icons.
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
        // Krimble 2026-10-10: George: no stripe, no dots, no empty margin. The edge is an invisible touch zone now.
        // The drawing below is kept, not deleted. It is not reached.
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
    // KRIMBLE 2026-10-02: panel renamed "Toolbox" -> "Tools".
    // : QDockWidget(i18n("Toolbox"))
    // : QDockWidget(i18n("Tools"))
    // Krimble 2026-10-11: called "Toolbox" again (George).
    : QDockWidget(i18n("Toolbox"))
    , m_toolBox(toolBox)
    , m_scrollArea(new KoToolBoxScrollArea(toolBox, this))
{
    // Krimble: wrap the tool-button scroll area in a container so a
    // Photoshop-style foreground/background color swap widget can be added
    // below it, matching Photoshop's toolbox layout. Kept alongside its
    // existing home in the classic toolbar (kis_control_frame.cpp) -- both
    // locations retained per project decision, not a relocation.
    QWidget *toolBoxContainer = new QWidget(this);
    m_containerLayout = new QVBoxLayout(toolBoxContainer);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_containerLayout->setSpacing(4);
    // KRIMBLE 2026-10-05: the tool grid no longer takes all the height (it was stretch 1, which pushed the
    // color wells and the screen mode button to the very bottom of the panel). It keeps its natural height, the
    // wells follow right under it, and the spare space goes to the stretch after them.
    // m_containerLayout->addWidget(m_scrollArea, 1);
    m_containerLayout->addWidget(m_scrollArea, 0);
    m_containerLayout->addStretch(1);
    setWidget(toolBoxContainer);

    // KRIMBLE 2026-10-06: snap to whole icon columns, and keep the columns through a screen rotation
    m_snapTimer = new QTimer(this);
    m_snapTimer->setSingleShot(true);
    m_snapTimer->setInterval(250);
    connect(m_snapTimer, &QTimer::timeout, this, &KoToolBoxDocker::snapToColumns);
    m_restoreTimer = new QTimer(this);
    m_restoreTimer->setSingleShot(true);
    m_restoreTimer->setInterval(300);
    // Krimble 2026-10-12: George: "Toolbox is too tall. Should not have empty space inside the box." Whenever the toolbox becomes
    // floating, it is cut down to the height of its tools.
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
        // if (floating) QTimer::singleShot(350, this, [this]() { fitHeightToContents(); });
        // Krimble 2026-10-23: George (test list "Mercury's Shadow", item 30): "Detaching toolbox goes to one column, then has the dangling empty bottom." The
        // height was worked out for the width the window had at that moment (the docked width: one column, so a tall pile of tools) and the width was changed
        // to the chosen columns later, so the tools reflowed into fewer rows and the window kept the taller height. Now width and height are given together,
        // the height worked out for the width that is going to be used; and once more a moment later, when the layout has settled.
        if (floating) {
            // Krimble 2026-10-23: a toolbox that becomes floating starts as columns (vertical) in Auto, whatever side it came from (see resizeEvent)
            if (m_orientation == Auto) setToolBoxOrientation(Qt::Vertical);
            QTimer::singleShot(350, this, [this]() { fitHeightToContents(true); });
            QTimer::singleShot(900, this, [this]() { fitHeightToContents(true); });
        }
    });
    m_edgeRight = new KrimbleToolBoxEdge(this, true);
    m_edgeBottom = new KrimbleToolBoxEdge(this, false);
    m_edgeRight->hide();
    m_edgeBottom->hide();
    connect(this, &QDockWidget::topLevelChanged, this, &KoToolBoxDocker::setFloatingEdges);
    // the toolbox may already be floating when the app starts (saved layout): no signal comes then
    QTimer::singleShot(0, this, [this]() { setFloatingEdges(isFloating()); });
    connect(m_restoreTimer, &QTimer::timeout, this, [this]() {
        applyColumns();
        QTimer::singleShot(500, this, [this]() { m_windowResizing = false; });
    });

    // QLabel *w = new QLabel(" ", this);
    // Krimble 2026-10-11: the same label, with three light horizontal lines on it so it is obvious it can be grabbed.
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
    w->setMinimumSize(titleSize, titleSize);
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

void KoToolBoxDocker::setViewManager(KisViewManager *viewManager)
{
    m_toolBox->setViewManager(viewManager);
    m_viewManager = viewManager;

    // KRIMBLE 2026-10-06: remembered number of columns, applied once at start, and the rotation watcher
    {
        KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
        // Krimble 2026-10-09: was default 2. No column count was ever saved (the handle can't be dragged on touch), so every rotation forced 2 columns. Default is now 1.
        // m_columns = qBound(1, cfg.readEntry("ToolBoxColumns", 2), 4);
        // m_columns = qBound(1, cfg.readEntry("ToolBoxColumns", 1), 4);
        // Krimble 2026-10-10: the most columns is now 40 (was 4), so the toolbox can be stretched as wide as the screen and go horizontal.
        // Krimble 2026-10-10 (2nd): default is 2 columns (George).
        // m_columns = qBound(1, cfg.readEntry("ToolBoxColumns", 1), 40);
        m_columns = qBound(1, cfg.readEntry("ToolBoxColumns", 2), 40);
        if (m_viewManager) {
            if (QWidget *window = m_viewManager->mainWindowAsQWidget()) {
                window->installEventFilter(this);
            }
        }
        m_windowResizing = true;
        m_restoreTimer->start(1200);
    }

    // Krimble: construct the color swap widget once a real KisViewManager
    // (and its canvasResourceProvider) is available -- not possible any
    // earlier, since the docker only gets a real one here.
    if (!m_dualColorButton && viewManager) {
        const KoColorDisplayRendererInterface *displayRenderer =
            KisDisplayColorConverter::dumbConverterInstance()->displayRendererInterface();
        m_dualColorButton = new KoDualColorButton(viewManager->canvasResourceProvider(), displayRenderer,
                                                    viewManager->mainWindowAsQWidget(), viewManager->mainWindowAsQWidget());
        m_dualColorButton->setFixedSize(28, 28);
        // KRIMBLE 2026-10-05: inserted right after the tool grid (index 1), before the stretch
        // m_containerLayout->addWidget(m_dualColorButton, 0, Qt::AlignHCenter);
        // m_containerLayout->insertWidget(1, m_dualColorButton, 0, Qt::AlignHCenter);
        // Krimble 2026-10-11: the wells and the screen mode button now sit in one box that is part of the tool grid's own
        // layout: right after the last tool, scrolling with the tools, not stuck to the bottom of the panel (George).
        QWidget *trailingBox = new QWidget(m_toolBox);
        QVBoxLayout *trailingLayout = new QVBoxLayout(trailingBox);
        trailingLayout->setContentsMargins(0, 4, 0, 0);
        trailingLayout->setSpacing(4);
        trailingLayout->addWidget(m_dualColorButton, 0, Qt::AlignHCenter);
        m_toolBox->toolBoxLayout()->setTrailingWidget(trailingBox);

        connect(m_dualColorButton, SIGNAL(foregroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetFGColor(KoColor)));
        connect(m_dualColorButton, SIGNAL(backgroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetBGColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigBGColorChanged(KoColor)), m_dualColorButton, SLOT(setBackgroundColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigFGColorChanged(KoColor)), m_dualColorButton, SLOT(setForegroundColor(KoColor)));

        connect(viewManager, &KisViewManager::viewChanged, this, &KoToolBoxDocker::slotUpdateDisplayRenderer);
        slotUpdateDisplayRenderer();

        // Krimble: Screen Mode toggle at the bottom of the toolbox, matching
        // Photoshop's toolbox layout. Reuses the existing view_show_canvas_only
        // action (checkable, Tab shortcut) -- no new action needed, just a
        // toolbox entry point for it. Icon overridden locally to view-fullscreen
        // since the action's own icon (document-new) is a placeholder; the
        // action definition itself in kritamenu.action is left untouched.
        QAction *canvasOnlyAction = viewManager->actionCollection()->action("view_show_canvas_only");
        if (canvasOnlyAction) {
            QToolButton *screenModeButton = new QToolButton(this);
            screenModeButton->setDefaultAction(canvasOnlyAction);
            screenModeButton->setIcon(KisIconUtils::loadIcon("view-fullscreen"));
            screenModeButton->setFixedSize(28, 28);
            screenModeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
            screenModeButton->setAutoRaise(true);
            // KRIMBLE 2026-10-05: inserted right after the color wells (index 2), before the stretch
            // m_containerLayout->addWidget(screenModeButton, 0, Qt::AlignHCenter);
            // m_containerLayout->insertWidget(2, screenModeButton, 0, Qt::AlignHCenter);
            trailingLayout->addWidget(screenModeButton, 0, Qt::AlignHCenter);   // Krimble 2026-10-11: in the trailing box
        }
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
    // if (m_orientation == Auto) {
    //     setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
    // }
    // Krimble 2026-10-23: George (1.0.42), asked what he was doing when "Toolbox got caught in horizontal mode for a minute": "Dragging it." In Auto the layout followed the SHAPE of the
    // window on every resize (wider than tall = horizontal), and the window changes size all the time while it is dragged or reshaped, so one wide moment turned it horizontal, the
    // horizontal layout made it wider still, and it stayed so (the 1030 px wide strip of his screenshot). While the toolbox is floating the layout is no longer changed by the
    // shape; docked, it follows the side it is docked at (top or bottom: horizontal; left or right: vertical).
    if (m_orientation == Auto && !isFloating()) {
        if (m_dockArea == Qt::TopDockWidgetArea || m_dockArea == Qt::BottomDockWidgetArea) {
            setToolBoxOrientation(Qt::Horizontal);
        } else if (m_dockArea == Qt::LeftDockWidgetArea || m_dockArea == Qt::RightDockWidgetArea) {
            setToolBoxOrientation(Qt::Vertical);
        } else {
            setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
        }
    }
    // Krimble 2026-10-10: the foreground/background color wells at the bottom grow with the toolbox width:
    // one icon wide at 1 column (as before), up to 72 px when the toolbox is wider.
    if (m_dualColorButton && m_scrollArea) {
        const int wellSize = qBound(28, m_scrollArea->viewport()->width() - 8, 72);
        if (m_dualColorButton->width() != wellSize) {
            m_dualColorButton->setFixedSize(wellSize, wellSize);
            // Krimble 2026-10-11: the wells are part of the tool layout now; tell it they changed size
            if (m_toolBox && m_toolBox->layout()) m_toolBox->layout()->invalidate();
        }
    }
    if (m_edgeRight && m_edgeBottom && isFloating()) {
        // const int e = 24;
        const int e = 10;   // Krimble 2026-10-10: invisible edge zone, 10 px
        m_edgeRight->setGeometry(width() - e, 0, e, height() - e);
        m_edgeBottom->setGeometry(0, height() - e, width(), e);
        m_edgeRight->raise();
        m_edgeBottom->raise();
    }
    // KRIMBLE 2026-10-06: when the docked toolbox is resized by hand (not by a rotation, not by us), fit it to a
    // whole number of icon columns a moment after the resize stops.
    // Krimble 2026-10-10: only a resize made with a finger or mouse down may change the column count. A rotation has none down.
    // if (m_snapTimer && !m_windowResizing && !m_applying && !isFloating()) {
    if (m_snapTimer && !m_windowResizing && !m_applying && !isFloating() && QApplication::mouseButtons() != Qt::NoButton) {
        m_snapTimer->start();
    } else if (m_restoreTimer && !m_windowResizing && !m_applying && !isFloating() && isVisible() && QApplication::mouseButtons() == Qt::NoButton) {
        // Krimble 2026-10-10: a width change with no finger down (a workspace or layout restore, a rotation): put the saved
        // column count back a moment later. Only a drag with a finger or the Columns menu can change the saved count.
        m_restoreTimer->start();
    }

    // Krimble 2026-10-10: keep the tool area exactly as tall as the tools are at this width, so the color wells cling to the
    // bottom of the tools instead of sitting far below them when the toolbox gets wider (the tool area kept the height it
    // had at a narrower width). Done a moment later, when the layout has settled. Only for the vertical layout.
    QTimer::singleShot(0, this, [this]() {
        if (!m_scrollArea || !m_toolBox || !m_toolBox->layout()) return;
        if (!m_toolBox->layout()->hasHeightForWidth()) {
            // m_scrollArea->setMaximumHeight(QWIDGETSIZE_MAX);
            // setMaximumHeight(QWIDGETSIZE_MAX);
            // Krimble 2026-10-18: George (1.0.37): "5. Toolbox has a big empty bottom for no reason." (his screenshot: the toolbox docked at the top, 2 rows
            // of tools and a tall empty space under them). In the rows layout (docked at the top or the bottom) the toolbox is not taller than its
            // rows, the title bar and the frame. A floating toolbox in rows is left free.
            if (!isFloating()) {
                const int rowsHeight = m_toolBox->sizeHint().height();
                const int frame = m_scrollArea->height() - m_scrollArea->viewport()->height();
                // const int others = qMax(0, height() - m_scrollArea->height());
                // Krimble 2026-10-19: George (1.0.39, test list, item 6, screenshot): the docked toolbox, 2 rows of tools, and the same tall empty space
                // under them. "others" was the toolbox's height minus the tool area's, and that difference INCLUDES the empty space itself (the
                // stretch under the tools in m_containerLayout), so the cap was as tall as the toolbox already was and never shrank it. Now "others"
                // is only what is really around the tools: the title bar and frame of the panel (its height minus its contents') and the margins
                // of the contents' layout.
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
        const int contentHeight = m_toolBox->layout()->heightForWidth(m_scrollArea->viewport()->width());
        if (contentHeight > 0) {
            const int frame = m_scrollArea->height() - m_scrollArea->viewport()->height();
            m_scrollArea->setMaximumHeight(contentHeight + frame);
            // Krimble 2026-10-11: the toolbox itself is never taller than its contents (the tools, the wells and the title bar)
            const int others = qMax(0, height() - m_scrollArea->height());
            setMaximumHeight(contentHeight + frame + others);
        }
    });
}

bool KoToolBoxDocker::eventFilter(QObject *watched, QEvent *event)
{
    // KRIMBLE 2026-10-06: a resize of the main window (screen rotation) must not change how many columns the
    // toolbox has: remember that it is happening, and put the width back when it has settled.
    if (event->type() == QEvent::Resize && m_viewManager && watched == m_viewManager->mainWindowAsQWidget()) {
        m_windowResizing = true;
        if (m_snapTimer) m_snapTimer->stop();
        if (m_restoreTimer) m_restoreTimer->start();
    }
    return QDockWidget::eventFilter(watched, event);
}

void KoToolBoxDocker::setFloatingEdges(bool floating)
{
    // KRIMBLE 2026-10-07: floating: no corner gadget, a 24 px margin at the right and at the bottom holds the two edge
    // strips; docked: the margins and the strips go away.
    // Krimble 2026-10-10: invisible edge zones of 10 px laid over the toolbox's own edge, no margin (was 24 and a margin).
    const int e = 10;
    // The 24 px margin version, kept, not deleted:
    // const int e = 24;
    // QMargins m = contentsMargins();
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
        // const int columns = qBound(1, qRound(qreal(startSize.width() - around + delta.x()) / icons), 4);
        // Krimble 2026-10-10: cap raised from 4 to 40 columns
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

// Krimble 2026-10-11: the Columns choice always works (George): docked left or right, floating, and docked at the top or
// bottom. When the tools are laid out in a column (the usual way) the number is the number of columns, and it sets the
// width. When they are laid out in a row, the number is the number of rows, and it sets the height.
// Krimble 2026-10-11: QA of 1.0.32: in portrait the docked toolbox stayed hundreds of pixels wider than its columns (one column
// of tools with a wide empty bar beside it), which left no room to move the strips next to it. Its preferred width is now the
// width its saved column count needs, so the dock layout starts from the right size.
QSize KoToolBoxDocker::sizeHint() const
{
    QSize hint = QDockWidget::sizeHint();
    if (m_toolBox && m_toolBox->layout() && m_toolBox->layout()->hasHeightForWidth()) {
        hint.setWidth(qMax(60, m_columns * iconWidth() + chromeWidth()));
    }
    return hint;
}

// Krimble 2026-10-11: a floating toolbox that is let go partly off the screen bounces back inside it (George).
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
            // Krimble 2026-10-13 (7th): George: "Get rid of bounces. They're annoying." The toolbox is no longer pushed back after it is moved:
            // it stays where it is let go. The old rule (pushed back off the top and the sides, 120 px kept on screen) is kept below,
            // not run.
            // return;
            // Krimble 2026-10-14: George (1.0.36): "Bounce toolbox off sides and top. Not bottom. It creates weird anomalous artifacts when it slides
            // off the side." The rule runs again: pushed back off the sides and the top; the bottom is free, but its title area (36 px) stays on screen.
            // Krimble 2026-10-17: George (1.0.37): "I would rather the toolbar dock at the edge than bounce." At the left or right edge, or above the
            // top strip, the toolbox is docked there (krimbleDockAtEdge) and nothing is pushed back; the old sideways and top push-back is off.
            krimbleDockAtEdge();
            if (!isFloating()) return;
            QScreen *screenPtr = screen() ? screen() : QGuiApplication::primaryScreen();
            if (!screenPtr) return;
            QRect avail = screenPtr->availableGeometry();
            avail.setTop(avail.top() + 48);        // keep a strip free at the top, as the dialogs do
            QRect g = geometry();
            g.setWidth(qMin(g.width(), avail.width()));
            // g.setHeight(qMin(g.height(), avail.height()));
            // g.moveLeft(qBound(avail.left(), g.left(), avail.right() - g.width() + 1));
            // g.moveTop(qBound(avail.top(), g.top(), avail.bottom() - g.height() + 1));
            // Krimble 2026-10-12: George: "I think the toolbox shouldn't bounce off the bottom. Only the top or sides." The bottom edge is
            // free; the top edge pushes it back down, and at least 120 px of it must stay on screen.
            // if (g.top() < avail.top()) g.moveTop(avail.top());
            // if (g.top() > avail.bottom() - 120) g.moveTop(avail.bottom() - 120);
            if (g.top() > avail.bottom() - 36) g.moveTop(avail.bottom() - 36);
            if (g != geometry()) setGeometry(g);
        });
    }
    m_screenTimer->start();
}

// Krimble 2026-10-17: George (1.0.37): "I would rather the toolbar dock at the edge than bounce." A floating toolbox that has reached the left or the
// right edge of the screen (within 4 px), or gone above the top strip, is docked there, as the Dock menu of its long press does, and is no longer
// pushed back. The bottom edge stays free (its title area, 36 px, stays on screen).
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
    // } else if (g.top() < avail.top()) {
    // Krimble 2026-10-19: George (1.0.39): "The toolbox isn't close to an edge and it snaps to one aggressively." It docked at the top as soon
    // as its top went into the 48 px strip under the screen's top (the menu bar is there, so a move near the menus docked it). Now only when
    // its top reaches the screen's top (within 4 px), like the sides.
    } else if (g.top() <= avail.top() - 48 + 4) {
        area = Qt::TopDockWidgetArea;
    }
    if (area == Qt::NoDockWidgetArea) return;
    setAllowedAreas(Qt::AllDockWidgetAreas);
    mainWindow->addDockWidget(area, this);
    setFloating(false);
    show();
}

// Krimble 2026-10-12: cuts a floating toolbox (vertical layout, columns) down to the height of its tools, so there is no empty space
// inside the box: the height of the tools for the current width, plus the title bar and the margins around them.
void KoToolBoxDocker::fitHeightToContents(bool setWidthToo)
{
    if (!isFloating() || !isVisible() || !m_toolBox || !m_toolBox->layout() || !m_toolBox->layout()->hasHeightForWidth()) return;
    if (!widget() || !widget()->layout()) return;
    // const int toolsHeight = m_toolBox->layout()->heightForWidth(m_scrollArea->viewport()->width());
    // Krimble 2026-10-23: with setWidthToo, the height is worked out for the width the window is going to have (the chosen number of columns), not the one it has now
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
    // Krimble 2026-10-18: George (1.0.37): "5. Toolbox has a big empty bottom for no reason. Hard to resize away." The floating toolbox is also never
    // taller than this: a window cannot be made taller than its maximum height, so the empty space cannot come back (the cut with resize() alone
    // was overruled by an older, taller size).
    if (target > 0) {
        setMaximumHeight(target);
    }
    // if (target > 0 && qAbs(height() - target) > 2) {
    //     m_applying = true;
    //     resize(width(), target);
    //     QTimer::singleShot(400, this, [this]() { m_applying = false; });
    // }
    const int wantedWidth = setWidthToo ? columnsWidth + chromeWidth() : width();   // Krimble 2026-10-23
    if (target > 0 && (qAbs(height() - target) > 2 || qAbs(width() - wantedWidth) > 2)) {
        m_applying = true;
        resize(wantedWidth, target);
        QTimer::singleShot(400, this, [this]() { m_applying = false; });
    }
}

void KoToolBoxDocker::applyColumns()
{
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
            QTimer::singleShot(450, this, [this]() { fitHeightToContents(); });   // Krimble 2026-10-12: the height follows the new columns
        } else {
            const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
            if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) return;
            m_applying = true;
            mainWindow->resizeDocks({this}, {target}, Qt::Horizontal);
            // Krimble 2026-10-11: resizeDocks alone did not always take effect in portrait; pin the width for a moment so the
            // dock layout has to use it, then let go again so a drag can change it.
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
        // const int chromeHeight = qMax(0, height() - m_scrollArea->viewport()->height());
        // Krimble 2026-10-13: George: "If I pick 2 columns it does nothing." That line counted the empty space below the tools as
        // "chrome" (the tool area is shorter than the dock, the rest is a stretch), so the target height came out the same as the
        // current, too tall, height and nothing changed. The chrome is now only what really surrounds the tools: the dock's own
        // frame, the margins around the tool area and the tool area's frame.
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
            // Krimble 2026-10-13: pin the height for a moment so the dock layout has to use it (as is done for the width), then let go.
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

void KoToolBoxDocker::snapToColumns()
{
    if (isFloating() || !isVisible()) return;
    // Krimble 2026-10-10: never change the saved column count during a rotation or while we are setting the width ourselves.
    if (m_windowResizing || m_applying) return;
    // do not fight a finger that is still down on the separator
    if (QApplication::mouseButtons() != Qt::NoButton) {
        m_userDragged = true;      // Krimble 2026-10-13: someone is dragging the edge: the width that follows is theirs
        m_snapTimer->start();
        return;
    }
    const Qt::DockWidgetArea area = m_dockArea;
    if (area != Qt::LeftDockWidgetArea && area != Qt::RightDockWidgetArea) return;

    // const int columns = qBound(1, qRound(qreal(m_scrollArea->viewport()->width()) / iconWidth()), 4);
    // Krimble 2026-10-10: cap raised from 4 to 40 columns
    const int columns = qBound(1, qRound(qreal(m_scrollArea->viewport()->width()) / iconWidth()), 40);
    // Krimble 2026-10-13: George: "Fix snapToColumns behavior. Yes." The saved column count was written from the width the toolbox happened
    // to have, so when the window could not give it the room (portrait: the toolbox squeezed to one column) the count was overwritten with 1,
    // and a choice from the Columns menu (2) snapped back to 1 at once. The count is now taken from the width only when someone dragged the
    // dock's edge; otherwise the saved count stays, and applyColumns() below makes the dock as wide as that count asks, as far as there
    // is room (and later, when there is more room, in full).
    // if (columns != m_columns) {
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
        // setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
        if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {        // Krimble 2026-10-23: by the side it is docked at (see resizeEvent)
            setToolBoxOrientation(Qt::Horizontal);
        } else if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
            setToolBoxOrientation(Qt::Vertical);
        } else {
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
    if (orientation == Qt::Horizontal) {
        setFeatures(features() | QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Horizontal);
    } else {
        setFeatures(features() & ~QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Vertical);
    }
    // Krimble 2026-10-19: George (1.0.39, test list, item 7): "Switching to horizontal in the toolbox menu changes how the icons line up, but not the
    // shape of the toolbox." A floating toolbox is reshaped to its new layout a moment later, when the layout has settled.
    if (isFloating()) {
        QTimer::singleShot(200, this, [this]() { krimbleFitFloatingShape(); });
    }
}

// Krimble 2026-10-19: George (1.0.39, test list, item 7). A floating toolbox takes the shape of its layout. Horizontal (rows): as wide as its rows
// (at most 95% of the screen; beyond that the rows scroll sideways) and as high as the rows, its frame and its title bar (which is at the
// side in this layout). Vertical (columns): the height is cut to the tools again (fitHeightToContents), keeping the width.
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

        // Krimble 2026-10-09: "Columns" choice. The size handle is hard to grab on touch, so the number of icon columns (1 to 4)
        // can be picked here. The choice is saved ("krimble/ToolBoxColumns") and kept through a rotation.
        m_contextMenu->addSection(i18n("Columns"));
        QActionGroup *columnsActionGroup = new QActionGroup(m_contextMenu);
        for (int i = 0; i < 4; ++i) {
            QAction *columnsAction = m_contextMenu->addAction(i18np("1 column", "%1 columns", i + 1));
            columnsAction->setActionGroup(columnsActionGroup);
            columnsAction->setCheckable(true);
            m_columnActions[i] = columnsAction;
            connect(columnsAction, &QAction::triggered, this, [this, i]() {
                m_columns = i + 1;
                KConfigGroup cfg(KSharedConfig::openConfig(), "krimble");
                cfg.writeEntry("ToolBoxColumns", m_columns);
                cfg.sync();
                applyColumns();
            });
        }

        // Krimble 2026-10-10: "Dock" section: Dock Left / Right / Top / Bottom and Undock, same as the long-press menu on other panels' title bars.
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
            // connect(dockAction, &QAction::triggered, this, [this, area]() {
            //     if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
            //         mainWindow->addDockWidget(area, this);
            //         show();
            //     }
            // });
            // Krimble 2026-10-13: George: "Dock right from the long press menu did nothing." The toolbox now starts floating, and
            // addDockWidget alone leaves a floating panel floating; it has to be told to dock. All areas are allowed first.
            connect(dockAction, &QAction::triggered, this, [this, area]() {
                if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
                    setAllowedAreas(Qt::AllDockWidgetAreas);
                    mainWindow->addDockWidget(area, this);
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
    }

    // Krimble 2026-10-09: tick the current column count each time the menu opens
    for (int i = 0; i < 4; ++i) {
        if (m_columnActions[i]) m_columnActions[i]->setChecked(m_columns == i + 1);
    }

    // Krimble 2026-10-10: tick where the toolbox is now, grey out places it may not go
    {
        QMainWindow *dockMainWindow = qobject_cast<QMainWindow*>(parentWidget());
        const Qt::DockWidgetArea dockCurrent = (dockMainWindow && !isFloating()) ? dockMainWindow->dockWidgetArea(this) : Qt::NoDockWidgetArea;
        const Qt::DockWidgetArea dockAreas[4] = {Qt::LeftDockWidgetArea, Qt::RightDockWidgetArea, Qt::TopDockWidgetArea, Qt::BottomDockWidgetArea};
        for (int i = 0; i < 4; ++i) {
            if (m_dockActions[i]) {
                m_dockActions[i]->setChecked(dockCurrent == dockAreas[i]);
                // m_dockActions[i]->setEnabled(allowedAreas() & dockAreas[i]);
                m_dockActions[i]->setEnabled(true);   // Krimble 2026-10-13: a choice from this menu is always allowed
            }
        }
        if (m_dockActions[4]) m_dockActions[4]->setChecked(isFloating());
    }

    m_contextMenu->exec(event->globalPos());
}
