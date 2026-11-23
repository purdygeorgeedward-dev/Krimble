/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2005-2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoToolDocker.h"

#include <klocalizedstring.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoToolRegistry.h>
#include <KoToolFactoryBase.h>

#include <QApplication>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include "KoDockWidgetTitleBar.h"
#include <QScrollArea>
#include <QScroller>
#include <QPointer>
#include <QAbstractScrollArea>
#include <KisKineticScroller.h>

class Q_DECL_HIDDEN KoToolDocker::Private
{
public:
    Private(KoToolDocker *dock)
        : q(dock)
    {
    }

    ~Private() {
        /// Hider widget is not part of any widget hierarchy (which is intentional
        /// due to bug 447522), so we need to delete it manually
        delete hiderWidget;
    }

    QList<QPointer<QWidget> > currentWidgetList;
    QSet<QWidget *> currentAuxWidgets;
    QScrollArea *scrollArea {nullptr};
    QWidget *hiderWidget {nullptr}; // non current widgets are hidden by being children of this
    QWidget *housekeeperWidget {nullptr};
    QGridLayout *housekeeperLayout {nullptr};
    QBoxLayout *housekeeperMainLayout{nullptr};
    QLabel *toolNameLabel {nullptr};

    // the name of the selected tool (the tool's own name, what its tool tip says) is shown at the top of the docker
    void updateToolName()
    {
        if (!toolNameLabel) return;
        QString name;
        const QString id = KoToolManager::instance()->activeToolId();
        if (!id.isEmpty()) {
            if (KoToolFactoryBase *factory = KoToolRegistry::instance()->get(id)) {
                name = factory->toolTip();
            }
        }
        if (name.endsWith(QLatin1String(" Tool"))) name.chop(5);
        // Transform and its Rotate mode: "Transform \u2013 <mode>" (the mode in use is the qApp property krimbleTransformMode, set by the Transform options)
        if (id == QLatin1String("KisToolTransform") || id == QLatin1String("KisToolFreeRotate")) {
            QString mode = id == QLatin1String("KisToolFreeRotate") ? QStringLiteral("rotate") : qApp->property("krimbleTransformMode").toString();
            if (mode.isEmpty() || (mode == QLatin1String("rotate") && id != QLatin1String("KisToolFreeRotate"))) mode = QStringLiteral("scale");
            mode[0] = mode[0].toUpper();
            name = i18n("Transform") + QString::fromUtf8(" \u2013 ") + mode;
        }
        toolNameLabel->setText(name);
        // toolNameLabel->setVisible(!name.isEmpty());
        // the name goes in the title row, before the grip ("Tool options title overlaps onto grab area."); the label stays, hidden
        // toolNameLabel->setVisible(false);
        // if (KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar *>(q->titleBarWidget())) bar->setTitleOverride(name);
        // the title says "Tool Options"; the tool (and the Transform mode) is one line at the top of the panel
        toolNameLabel->setVisible(!name.isEmpty());
        if (KoDockWidgetTitleBar *bar = dynamic_cast<KoDockWidgetTitleBar *>(q->titleBarWidget())) {
            bar->setTitleOverride(QString());
            bar->setTabbedTitle(name);   // in a tab group the tool's name sits in the title row, the grip after it
            bar->update();   // the grip starts after the tool name, so it is redrawn when the name changes
        }
    }
    KoToolDocker *q {nullptr};
    Qt::DockWidgetArea dockingArea {Qt::NoDockWidgetArea};

    void resetWidgets()
    {
        currentWidgetList.clear();
        qDeleteAll(currentAuxWidgets);
        currentAuxWidgets.clear();
    }

    void recreateLayout(const QList<QPointer<QWidget> > &optionWidgetList)
    {
        Q_FOREACH (QPointer<QWidget> widget, currentWidgetList) {
            if (!widget.isNull() && widget && hiderWidget) {
                widget->setParent(hiderWidget);
            }
        }
        qDeleteAll(currentAuxWidgets);
        currentAuxWidgets.clear();

        currentWidgetList = optionWidgetList;

        // need to unstretch row that have previously been stretched
        housekeeperLayout->setRowStretch(housekeeperLayout->rowCount()-1, 0);

        int cnt = 0;
        QFrame *s;
        QLabel *l;
        switch (dockingArea) {
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            housekeeperMainLayout->setDirection(QBoxLayout::LeftToRight);
            housekeeperMainLayout->setStretch(0, 1);
            housekeeperMainLayout->setStretch(1, 0);
            housekeeperLayout->setHorizontalSpacing(2);
            housekeeperLayout->setVerticalSpacing(0);
            Q_FOREACH (QPointer<QWidget> widget, currentWidgetList) {
                if (widget.isNull() || widget->objectName().isEmpty()) {
                    continue;
                }
                // if (!widget->windowTitle().isEmpty()) {
                // the tool name is already the first line of the panel: no second copy of it (Crop showed "Crop" twice)
                if (!widget->windowTitle().isEmpty() && !(toolNameLabel && widget->windowTitle() == toolNameLabel->text())) {
                    housekeeperLayout->addWidget(
                        l = new QLabel(widget->windowTitle()),
                        0,
                        2 * cnt);
                    currentAuxWidgets.insert(l);
                }
                housekeeperLayout->addWidget(widget, 1, 2 * cnt);
                widget->show();
                if (widget != currentWidgetList.last()) {
                    housekeeperLayout->addWidget(s = new QFrame(),
                                                 0,
                                                 2 * cnt + 1,
                                                 2,
                                                 1);
                    s->setFrameShape(QFrame::VLine);
                    currentAuxWidgets.insert(s);
                }
                cnt++;
            }
            break;
        case Qt::NoDockWidgetArea:
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea: {
            housekeeperMainLayout->setDirection(QBoxLayout::TopToBottom);
            housekeeperMainLayout->setStretch(0, 0);
            housekeeperMainLayout->setStretch(1, 1);
            housekeeperLayout->setHorizontalSpacing(0);
            housekeeperLayout->setVerticalSpacing(2);
            Q_FOREACH (QPointer<QWidget> widget, currentWidgetList) {
                if (widget.isNull() || widget->objectName().isEmpty()) {
                    continue;
                }
                // if (!widget->windowTitle().isEmpty()) {
                // the tool name is already the first line of the panel: no second copy of it (Crop showed "Crop" twice)
                if (!widget->windowTitle().isEmpty() && !(toolNameLabel && widget->windowTitle() == toolNameLabel->text())) {
                    housekeeperLayout->addWidget(
                        l = new QLabel(widget->windowTitle()),
                        cnt++,
                        0);
                    currentAuxWidgets.insert(l);
                }
                housekeeperLayout->addWidget(widget, cnt++, 0);
                widget->show();
                if (widget != currentWidgetList.last()) {
                    housekeeperLayout->addWidget(s = new QFrame(), cnt++, 0);
                    s->setFrameStyle(QFrame::HLine | QFrame::Sunken);
                    currentAuxWidgets.insert(s);
                }
            }
            break;
        }
        default:
            break;
        }

        housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        housekeeperLayout->invalidate();
    }

    void locationChanged(Qt::DockWidgetArea area)
    {
        dockingArea = area;
        recreateLayout(currentWidgetList);
    }

};

// Krimble 2026-11-22: sliders in Tool Options could not be dragged with a finger (George: "Can't move this tiny little \"magnetism\" control."):
// the panel's finger scrolling took the drag. While a finger or the mouse is down on a slider, the panel does not scroll.
class KrimbleSliderScrollGuard : public QObject
{
public:
    KrimbleSliderScrollGuard(QAbstractScrollArea *area, QObject *parent) : QObject(parent), m_area(area) {}
    void watch(QWidget *root)
    {
        if (!root) return;
        Q_FOREACH (QWidget *w, root->findChildren<QWidget *>()) {
            if ((w->inherits("KisAbstractSliderSpinBox") || w->inherits("QAbstractSlider")) && !w->property("krimbleScrollGuard").toBool()) {
                w->setProperty("krimbleScrollGuard", true);
                w->installEventFilter(this);
            }
        }
    }
protected:
    bool eventFilter(QObject *, QEvent *event) override
    {
        if (!m_area) return false;
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::TouchBegin:
            if (QScroller::hasScroller(m_area)) QScroller::scroller(m_area)->stop();
            QScroller::ungrabGesture(m_area);
            break;
        case QEvent::MouseButtonRelease:
        case QEvent::TouchEnd:
        case QEvent::TouchCancel:
            QTimer::singleShot(0, this, [this]() { if (m_area) QScroller::grabGesture(m_area, KisKineticScroller::getConfiguredGestureType()); });
            break;
        default:
            break;
        }
        return false;
    }
private:
    QPointer<QAbstractScrollArea> m_area;
};

KoToolDocker::KoToolDocker(QWidget *parent)
    : QDockWidget(i18n("Tool Options"), parent),
      d(new Private(this))
{
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(locationChanged(Qt::DockWidgetArea)));

    d->housekeeperWidget = new QWidget();
    d->housekeeperLayout = new QGridLayout;
    // d->housekeeperLayout->setContentsMargins(4,4,4,0);
    // Krimble 2026-11-22: 28 real px on the left like the panel titles, 12 on the right (the options touched the panel's edges)
    const qreal krimbleMarginDpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
    const int krimbleLeft = qRound(28.0 / krimbleMarginDpr), krimbleRight = qRound(12.0 / krimbleMarginDpr);
    d->housekeeperLayout->setContentsMargins(krimbleLeft, 4, krimbleRight, 0);
    d->housekeeperLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->housekeeperMainLayout =
        new QBoxLayout(QBoxLayout::TopToBottom, d->housekeeperWidget);
    d->housekeeperMainLayout->setContentsMargins(0, 0, 0, 0);
    d->housekeeperMainLayout->setSpacing(0);
    d->housekeeperMainLayout->addLayout(d->housekeeperLayout, 0);
    d->housekeeperMainLayout->addStretch(1);

    d->toolNameLabel = new QLabel(d->housekeeperWidget);
    d->toolNameLabel->setObjectName(QStringLiteral("krimbleToolOptionsName"));
    {
        QFont nameFont = d->toolNameLabel->font();
        nameFont.setBold(true);
        d->toolNameLabel->setFont(nameFont);
    }
    d->toolNameLabel->setStyleSheet(QStringLiteral("QLabel#krimbleToolOptionsName { color: palette(window-text); padding: 3px 0px; border-bottom: 1px solid #343c57; }"));
    d->toolNameLabel->setContentsMargins(krimbleLeft, 0, krimbleRight, 0);   // the tool name lines up with the options under it
    d->toolNameLabel->setIndent(0);   // a label with a border otherwise indents its text by half an "x"
    d->housekeeperMainLayout->insertWidget(0, d->toolNameLabel);
    d->updateToolName();
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, [this](KoCanvasController *) { d->updateToolName(); });
    QTimer::singleShot(0, this, [this]() { d->updateToolName(); });   // the title row is given to the docker after it is made

    d->hiderWidget = new QWidget();
    d->hiderWidget->setVisible(false);

    d->scrollArea = new QScrollArea();
    d->scrollArea->setWidget(d->housekeeperWidget);
    d->scrollArea->setFrameShape(QFrame::NoFrame);
    d->scrollArea->setWidgetResizable(true);
    d->scrollArea->setFocusPolicy(Qt::NoFocus);

    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(d->scrollArea);
    if( scroller ) {
        connect(scroller, SIGNAL(stateChanged(QScroller::State)), this, SLOT(slotScrollerStateChange(QScroller::State)));
    }

    setWidget(d->scrollArea);
}

KoToolDocker::~KoToolDocker()
{
    delete d;
}

bool KoToolDocker::hasOptionWidget()
{
    return !d->currentWidgetList.isEmpty();
}

void KoToolDocker::setOptionWidgets(const QList<QPointer<QWidget> > &optionWidgetList)
{
    d->recreateLayout(optionWidgetList);
    if (d->scrollArea) {
        KrimbleSliderScrollGuard *guard = d->scrollArea->findChild<KrimbleSliderScrollGuard *>(QString(), Qt::FindDirectChildrenOnly);
        if (!guard) guard = new KrimbleSliderScrollGuard(d->scrollArea, d->scrollArea);
        guard->watch(d->housekeeperWidget);
    }
}

void KoToolDocker::slotScrollerStateChange(QScroller::State state)
{
    KisKineticScroller::updateCursor(d->scrollArea, state);
}

void KoToolDocker::resetWidgets()
{
    d->resetWidgets();
}


void KoToolDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != 0);
}

void KoToolDocker::unsetCanvas()
{
    setEnabled(false);
}

//have to include this because of Q_PRIVATE_SLOT
#include <moc_KoToolDocker.cpp>
