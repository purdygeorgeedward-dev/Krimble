/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBox_p.h"
#include "KoToolBoxLayout_p.h"
#include "KoToolBoxButton_p.h"
#include "KrimbleToolboxIcons.h"   // Krimble 2026-10-30
#include "kis_assert.h"

#include <QButtonGroup>
#include <QToolButton>
#include <QStyleOption>
#include <QActionGroup>
#include <QPainter>
#include <QHash>
#include <QApplication>
#include <QStyle>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QScreen>

#include <klocalizedstring.h>
#include <WidgetsDebug.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <KisPortingUtils.h>

#include <kactioncollection.h>
#include <KisViewManager.h>
#include <KoCanvasController.h>
#include <KoShapeLayer.h>

#define BUTTON_MARGIN 10

static int buttonSize(int screen)
{
    KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(screen < QGuiApplication::screens().size() && screen >= 0, 16);

#ifdef Q_OS_ANDROID
    // Krimble: the width-bucketed heuristic below is calibrated for desktop
    // monitor widths at desktop DPI -- a phone's raw pixel width lands it in
    // one of those buckets with no regard for its much higher DPI, so the
    // "size" it computes doesn't correspond to a similar physical/visual
    // size on a phone the way it does on a desktop monitor. Fixed,
    // deliberately generous default instead, consistent with the touch
    // targets widened elsewhere this session (crop handles, dock
    // separators, text-selection handles).
    Q_UNUSED(screen);
    // KRIMBLE 2026-10-01: default toolbox icon size changed from 32 to 16
    // (16 x 16), per project direction. Original line kept below.
    // return 32;
    return 16;
#else
    QRect rc = QGuiApplication::screens().at(screen)->availableGeometry();
    if (rc.width() <= 1024) {
        return 12;
    }
    else if (rc.width() <= 1377) {
        return 14;
    }
    else  if (rc.width() <= 1920 ) {
        return 16;
    }
    else {
        return 22;
    }
#endif
}

class KoToolBox::Private
{
public:
    void addSection(Section *section, const QString &name);

    QList<QToolButton*> buttons;
    KoToolBoxButton *selectedButton {nullptr};
    QHash<QString, KoToolBoxButton*> buttonsByToolId;
    QMap<QString, Section*> sections;
    KoToolBoxLayout *layout {0};
    QButtonGroup *buttonGroup {0};
    QHash<QToolButton*, QString> visibilityCodes;
    bool floating {false};
    int iconSize {0};
    QMap<QAction*,int> contextIconSizes;
    QAction *defaultIconSizeAction {0};
};

void KoToolBox::Private::addSection(Section *section, const QString &name)
{
    section->setName(name);
    layout->addSection(section);
    sections.insert(name, section);
}

KoToolBox::KoToolBox()
    : d(new Private)
{
    d->layout = new KoToolBoxLayout(this);
    // add defaults
    d->addSection(new Section(this), "main");
    d->addSection(new Section(this), "dynamic");

    d->buttonGroup = new QButtonGroup(this);

    // Get screen the widget exists in, but fall back to primary screen if invalid.
    const int widgetsScreen = KisPortingUtils::getScreenNumberForWidget(QApplication::activeWindow());
    const int primaryScreen = 0; //In QT, primary screen should always be the first index of QGuiApplication::screens()
    const int screen = (widgetsScreen >= 0 && widgetsScreen < QGuiApplication::screens().size()) ? widgetsScreen : primaryScreen;
    const int toolbuttonSize = buttonSize(screen);
    KConfigGroup cfg =  KSharedConfig::openConfig()->group("KoToolBox");
    d->iconSize = cfg.readEntry("iconSize", toolbuttonSize);

    Q_FOREACH (KoToolAction *toolAction, KoToolManager::instance()->toolActionList()) {
        addButton(toolAction);
    }

    applyIconSize();

    // Update visibility of buttons
    setButtonsVisible(QList<QString>());

    setCompact(cfg.readEntry<bool>("compact", false));

    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*)),
            this, SLOT(setActiveTool(KoCanvasController*)));
    connect(KoToolManager::instance(), SIGNAL(currentLayerChanged(const KoCanvasController*,const KoShapeLayer*)),
            this, SLOT(setCurrentLayer(const KoCanvasController*,const KoShapeLayer*)));
    connect(KoToolManager::instance(), SIGNAL(toolCodesSelected(QList<QString>)), this, SLOT(setButtonsVisible(QList<QString>)));
    connect(KoToolManager::instance(),
            SIGNAL(addedTool(KoToolAction*,KoCanvasController*)),
            this, SLOT(toolAdded(KoToolAction*,KoCanvasController*)));

}

KoToolBox::~KoToolBox()
{
    delete d;
}

void KoToolBox::applyIconSize()
{
    Q_FOREACH (QToolButton *button, d->buttons) {
        button->setIconSize(QSize(d->iconSize, d->iconSize));
    }

    Q_FOREACH (Section *section, d->sections.values())  {
        section->setButtonSize(QSize(d->iconSize + BUTTON_MARGIN,  d->iconSize + BUTTON_MARGIN));
    }
}


void KoToolBox::setViewManager(KisViewManager *viewManager)
{
    KisKActionCollection *actionCollection = viewManager->actionCollection();
    Q_FOREACH(KoToolAction *toolAction, KoToolManager::instance()->toolActionList()) {
        QAction *toolQAction = actionCollection->action(toolAction->id());
        auto button = d->buttonsByToolId.find(toolAction->id());
        if (button == d->buttonsByToolId.end()) {
            qWarning() << "Toolbox is missing button for tool" << toolAction->id();
            continue;
        }
        (*button)->attachAction(toolQAction);
    }
}

void KoToolBox::addButton(KoToolAction *toolAction)
{
    // KRIMBLE 2026-10-03: the Select Shapes tool ("InteractionTool") stays registered because
    // the tool manager needs it as the default tool, but it gets no toolbox button, so the
    // first button is the Move tool. (An empty toolbox section is skipped by the layout.)
    if (toolAction->id() == QLatin1String("InteractionTool")) {
        return;
    }

#ifdef Q_OS_ANDROID
    // Krimble 2026-10-26: George, about the six tools under Rotate: "I question their usefulness in mobile." and "When we build the desktop version for Linux, Windows, and
    // MacOS we will enable those. For mobile, comment them out." So on Android they get no toolbox button (the tools stay registered, and the code is untouched); this block
    // is only compiled for Android, so a desktop build shows all of them. To bring one back on the phone, delete its line from the list.
    {
        static const QStringList krimbleHiddenOnMobile = {
            QStringLiteral("KritaShape/KisToolLazyBrush"),   // Colorize Mask Editing Tool
            QStringLiteral("KisToolEncloseAndFill"),         // Enclose and Fill Tool
            QStringLiteral("KisAssistantTool"),              // Assistant Tool
            QStringLiteral("KritaShape/KisToolMeasure"),     // Ruler Tool
            QStringLiteral("ToolReferenceImages"),           // Reference Images Tool
            QStringLiteral("KisToolSelectPath"),             // Path-Based Selection Tool
        };
        if (krimbleHiddenOnMobile.contains(toolAction->id())) {
            return;
        }
    }
#endif

    KoToolBoxButton *button = new KoToolBoxButton(toolAction, this);

    d->buttons << button;

    QString sectionToBeAddedTo;
    const QString section = toolAction->section();
    if (section.contains(qApp->applicationName())) {
        sectionToBeAddedTo = "main";
    } else if (section.contains("main")) {
        sectionToBeAddedTo = "main";
    }  else if (section.contains("dynamic")) {
        sectionToBeAddedTo = "dynamic";
    } else {
        sectionToBeAddedTo = section;
    }

    // Krimble 2026-10-10: George wants one continuous block of tools, no groups. Every tool goes into the one "main"
    // section. The old order is kept: the old group (main, dynamic, then by its leading digit) is folded into the priority.
    int groupRank = 12;
    if (sectionToBeAddedTo == QLatin1String("main")) {
        groupRank = 0;
    } else if (sectionToBeAddedTo == QLatin1String("dynamic")) {
        groupRank = 1;
    } else if (!sectionToBeAddedTo.isEmpty() && sectionToBeAddedTo.at(0).isDigit()) {
        groupRank = 2 + sectionToBeAddedTo.at(0).digitValue();
    }
    // (old: the tool went into its own section)
    // Section *sectionWidget = d->sections.value(sectionToBeAddedTo);
    // if (sectionWidget == 0) { sectionWidget = new Section(this); d->addSection(sectionWidget, sectionToBeAddedTo); }
    // sectionWidget->addButton(button, toolAction->toolBoxPriority());
    Section *sectionWidget = d->sections.value(QStringLiteral("main"));
    if (sectionWidget == 0) {
        sectionWidget = new Section(this);
        d->addSection(sectionWidget, QStringLiteral("main"));
    }
    sectionWidget->addButton(button, groupRank * 1000000 + toolAction->toolBoxPriority());

    d->buttonGroup->addButton(button);
    d->buttonsByToolId.insert(toolAction->id(), button);

    d->visibilityCodes.insert(button, toolAction->visibilityCode());
}

void KoToolBox::setActiveTool(KoCanvasController *canvas)
{
    Q_UNUSED(canvas);

    QString id = KoToolManager::instance()->activeToolId();
    KoToolBoxButton *button = d->buttonsByToolId.value(id);
    if (button) {
        button->setChecked(true);
        button->setHighlightColor();
        if (d->selectedButton) {
            d->selectedButton->setHighlightColor();
        }
        d->selectedButton = button;
    }
    else {
        warnWidgets << "KoToolBox::setActiveTool(" << id << "): no such button found";
    }
}

void KoToolBox::setButtonsVisible(const QList<QString> &codes)
{
    Q_FOREACH (QToolButton *button, d->visibilityCodes.keys()) {
        QString code = d->visibilityCodes.value(button);

        if (code.startsWith(QLatin1String("flake/"))) {
            continue;
        }

        if (code.endsWith( QLatin1String( "/always"))) {
            button->setVisible(true);
            button->setEnabled(true);
        }
        else if (code.isEmpty()) {
            button->setVisible(true);
            button->setEnabled( codes.count() != 0 );
        }
        else {
            button->setVisible( codes.contains(code) );
        }
    }
    layout()->invalidate();
    update();
}

void KoToolBox::setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer *layer)
{
    Q_UNUSED(canvas);
    const bool enabled = layer == 0 || (layer->isShapeEditable() && layer->isVisible());
    foreach (QToolButton *button, d->visibilityCodes.keys()) {
        if (d->visibilityCodes[button].endsWith( QLatin1String( "/always") ) ) {
            continue;
        }
        button->setEnabled(enabled);
    }
}

void KoToolBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const QList<Section*> sections = d->sections.values();
    int halfSpacing = layout()->spacing();
    if (halfSpacing > 0) {
        halfSpacing /= 2;
    }
    for(auto iterator = sections.begin(); iterator != sections.end(); ++iterator) {
        Section *section = *iterator;
        if (section->separators() == Section::SeparatorNone) {
            continue;
        }

        QStyleOption styleoption;
        styleoption.palette = palette();

        if (section->separators() & Section::SeparatorTop) {
            int y = section->y() - halfSpacing;
            styleoption.state = QStyle::State_None;
            styleoption.rect = QRect(section->x(), y-1, section->width(), 2);

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        }

        if (section->separators() & Section::SeparatorLeft && section->isLeftToRight()) {
            int x = section->x() - halfSpacing;
            styleoption.state = QStyle::State_Horizontal;
            styleoption.rect = QRect(x-1, section->y(), 2, section->height());

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        } else if (section->separators() & Section::SeparatorLeft && section->isRightToLeft()) {
            int x = section->x() + section->width() + halfSpacing;
            styleoption.state = QStyle::State_Horizontal;
            styleoption.rect = QRect(x-1, section->y(), 2, section->height());

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &styleoption, &painter);
        }
    }

    painter.end();
}

void KoToolBox::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        Q_FOREACH (QToolButton *button, d->buttons) {
            KoToolBoxButton* toolBoxButton = qobject_cast<KoToolBoxButton*>(button);
            if (toolBoxButton) {
                toolBoxButton->setHighlightColor();
            }
        }
    }
}

void KoToolBox::setOrientation(Qt::Orientation orientation)
{
    d->layout->setOrientation(orientation);
    QTimer::singleShot(0, this, SLOT(update()));
    Q_FOREACH (Section* section, d->sections) {
        section->setOrientation(orientation);
    }
}

void KoToolBox::setCompact(bool state)
{
    d->layout->setCompact(state);
}

bool KoToolBox::isCompact()
{
    return d->layout->isCompact();
}

void KoToolBox::setFloating(bool v)
{
    d->floating = v;
}

void KoToolBox::toolAdded(KoToolAction *toolAction, KoCanvasController *canvas)
{
    Q_UNUSED(canvas);
    addButton(toolAction);
    setButtonsVisible(QList<QString>());

}

void KoToolBox::slotContextIconSize()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        int iconSize = -1;
        if (action == d->defaultIconSizeAction) {
            iconSize = buttonSize(KisPortingUtils::getScreenNumberForWidget(QApplication::activeWindow()));
            QAction *action = d->contextIconSizes.key(iconSize);
            if (action) {
                action->setChecked(true);
            }
        } else if (d->contextIconSizes.contains(action)) {
            iconSize = d->contextIconSizes.value(action);
        }
        KIS_SAFE_ASSERT_RECOVER(iconSize >= 0) { iconSize = 16; }

        KConfigGroup cfg =  KSharedConfig::openConfig()->group("KoToolBox");
        cfg.writeEntry("iconSize", iconSize);
        d->iconSize = iconSize;

        applyIconSize();
    }
}

void KoToolBox::setupIconSizeMenu(QMenu *menu)
{
    if (d->contextIconSizes.isEmpty()) {
        d->defaultIconSizeAction = menu->addAction(i18nc("@item:inmenu Icon size", "Default"),
                                                   this, SLOT(slotContextIconSize()));

        QActionGroup *sizeGroup = new QActionGroup(menu);
        QList<int> sizes;
        sizes << 12 << 14 << 16 << 22 << 32 << 48 << 64; //<< 96 << 128 << 192 << 256;
        Q_FOREACH (int i, sizes) {
            QAction *action = menu->addAction(i18n("%1x%2", i, i), this, SLOT(slotContextIconSize()));
            d->contextIconSizes.insert(action, i);
            action->setActionGroup(sizeGroup);
            action->setCheckable(true);
            if (d->iconSize == i) {
                action->setChecked(true);
            }
        }
    }
}

// Krimble 2026-10-30: George: "I want the user to be able to switch between the full color themed toolbox icons and your clear icons." Two items in the toolbox's
// menu (touch and hold): Full color (the picture icons) and Clear (the program's own line icons). The choice is saved and applies at once.
void KoToolBox::setupIconStyleMenu(QMenu *menu)
{
    QActionGroup *styleGroup = new QActionGroup(menu);
    const bool color = KrimbleToolboxIcons::colorMode();
    QAction *colorAction = menu->addAction(i18n("Full color"));
    QAction *clearAction = menu->addAction(i18n("Clear"));
    for (QAction *action : {colorAction, clearAction}) {
        action->setActionGroup(styleGroup);
        action->setCheckable(true);
    }
    colorAction->setChecked(color);
    clearAction->setChecked(!color);
    auto apply = [this](bool useColor) {
        KrimbleToolboxIcons::setColorMode(useColor);
        Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) {
            button->krimbleRefreshIcon();
        }
    };
    connect(colorAction, &QAction::triggered, this, [apply]() { apply(true); });
    connect(clearAction, &QAction::triggered, this, [apply]() { apply(false); });
}

KoToolBoxLayout *KoToolBox::toolBoxLayout() const
{
    return d->layout;
}

#include "moc_KoToolBoxScrollArea_p.cpp"
