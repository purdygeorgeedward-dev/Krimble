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
#include "KrimbleToolboxIcons.h"
#include "KrimbleToolFlyout.h"
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
#include <QDynamicPropertyChangeEvent>
#include <kis_icon_utils.h>

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
    Q_UNUSED(screen);
    return 22;
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

static const QList<QStringList> &krimbleToolGroupIds()
{
    static const QList<QStringList> groups = {
        { QStringLiteral("KisToolSelectRectangular"), QStringLiteral("KisToolSelectElliptical"), QStringLiteral("KisToolSelectHexagon") },
        { QStringLiteral("KisToolSelectOutline"), QStringLiteral("KisToolSelectPolygonal"), QStringLiteral("KisToolSelectMagnetic"), QStringLiteral("KisToolSelectPath") },
        { QStringLiteral("KisToolSelectContiguous"), QStringLiteral("KisToolSelectSimilar") },
        { QStringLiteral("KritaShape/KisToolRectangle"), QStringLiteral("KritaShape/KisToolEllipse"), QStringLiteral("KritaShape/KisToolPolygonStar"), QStringLiteral("KritaShape/KisToolLine") },
        { QStringLiteral("KritaFill/KisToolGradient"), QStringLiteral("KritaFill/KisToolFill"), QStringLiteral("KisToolEncloseAndFill") },
        { QStringLiteral("KisToolPath"), QStringLiteral("KisToolPolyline"), QStringLiteral("KisToolPolygon"), QStringLiteral("KisToolPencil") },
        { QStringLiteral("KisToolTransform"), QStringLiteral("KisToolFreeRotate") },
        { QStringLiteral("KritaShape/KisToolSoften"), QStringLiteral("KritaShape/KisToolSmudge"), QStringLiteral("KritaShape/KisToolSharpen") },
        { QStringLiteral("KritaSelected/KisToolColorSampler"), QStringLiteral("KritaShape/KisToolMeasure"), QStringLiteral("ToolReferenceImages"), QStringLiteral("KisAssistantTool") },
        { QStringLiteral("KrimbleTextTool"), QStringLiteral("SvgTextTool") },
    };
    return groups;
}

static int krimbleGroupIndexOf(const QString &toolId)
{
    const QList<QStringList> &groups = krimbleToolGroupIds();
    for (int i = 0; i < groups.size(); ++i) {
        if (groups.at(i).contains(toolId)) return i;
    }
    return -1;
}

static int krimbleRankedPriority(KoToolAction *toolAction)
{
    const QString section = toolAction->section();
    QString sectionToBeAddedTo;
    if (section.contains(qApp->applicationName())) sectionToBeAddedTo = "main";
    else if (section.contains("main")) sectionToBeAddedTo = "main";
    else if (section.contains("dynamic")) sectionToBeAddedTo = "dynamic";
    else sectionToBeAddedTo = section;
    int groupRank = 12;
    if (sectionToBeAddedTo == QLatin1String("main")) groupRank = 0;
    else if (sectionToBeAddedTo == QLatin1String("dynamic")) groupRank = 1;
    else if (!sectionToBeAddedTo.isEmpty() && sectionToBeAddedTo.at(0).isDigit()) groupRank = 2 + sectionToBeAddedTo.at(0).digitValue();
    return groupRank * 1000000 + toolAction->toolBoxPriority();
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
    struct KrimbleGroup {
        QList<KoToolBoxButton*> buttons;
        KoToolBoxButton *shown {nullptr};
    };
    QList<KrimbleGroup> groups;
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
    d->addSection(new Section(this), "main");
    d->addSection(new Section(this), "dynamic");

    d->buttonGroup = new QButtonGroup(this);
    qApp->installEventFilter(this);

    const int widgetsScreen = KisPortingUtils::getScreenNumberForWidget(QApplication::activeWindow());
    const int primaryScreen = 0;
    const int screen = (widgetsScreen >= 0 && widgetsScreen < QGuiApplication::screens().size()) ? widgetsScreen : primaryScreen;
    const int toolbuttonSize = buttonSize(screen);
    KConfigGroup cfg =  KSharedConfig::openConfig()->group("KoToolBox");
    d->iconSize = cfg.readEntry("iconSize", toolbuttonSize);

    Q_FOREACH (KoToolAction *toolAction, KoToolManager::instance()->toolActionList()) {
        addButton(toolAction);
    }

    applyIconSize();

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
    if (toolAction->id() == QLatin1String("PathTool")) {
        return;
    }
    if (toolAction->id() == QLatin1String("KisToolFreeRotate")) {
        return;
    }

#ifdef Q_OS_ANDROID
    {
        static const QStringList krimbleHiddenOnMobile = {
            QStringLiteral("KritaShape/KisToolLazyBrush"),
            QStringLiteral("KisToolEncloseAndFill"),
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

    int groupRank = 12;
    if (sectionToBeAddedTo == QLatin1String("main")) {
        groupRank = 0;
    } else if (sectionToBeAddedTo == QLatin1String("dynamic")) {
        groupRank = 1;
    } else if (!sectionToBeAddedTo.isEmpty() && sectionToBeAddedTo.at(0).isDigit()) {
        groupRank = 2 + sectionToBeAddedTo.at(0).digitValue();
    }
    Section *sectionWidget = d->sections.value(QStringLiteral("main"));
    if (sectionWidget == 0) {
        sectionWidget = new Section(this);
        d->addSection(sectionWidget, QStringLiteral("main"));
    }
    int krimblePriority = groupRank * 1000000 + toolAction->toolBoxPriority();
    const int krimbleGroup = krimbleGroupIndexOf(toolAction->id());
    if (krimbleGroup >= 0) {
        const QString leaderId = krimbleToolGroupIds().at(krimbleGroup).first();
        Q_FOREACH (KoToolAction *other, KoToolManager::instance()->toolActionList()) {
            if (other->id() == leaderId) { krimblePriority = krimbleRankedPriority(other); break; }
        }
    }
    sectionWidget->addButton(button, krimblePriority);

    d->buttonGroup->addButton(button);
    d->buttonsByToolId.insert(toolAction->id(), button);
    if (krimbleGroup >= 0) {
        while (d->groups.size() <= krimbleGroup) d->groups.append(Private::KrimbleGroup());
        d->groups[krimbleGroup].buttons << button;
        button->krimbleSetLongPress(true);
        connect(button, &KoToolBoxButton::krimbleLongPressed, this, [this, krimbleGroup]() { krimbleShowFlyout(krimbleGroup); });
    }

    d->visibilityCodes.insert(button, toolAction->visibilityCode());
}

void KoToolBox::addActionButton(QToolButton *button, int priority)
{
    d->buttons << button;
    Section *sectionWidget = d->sections.value(QStringLiteral("main"));
    if (sectionWidget == 0) {
        sectionWidget = new Section(this);
        d->addSection(sectionWidget, QStringLiteral("main"));
    }
    sectionWidget->addButton(button, priority);
    button->setIconSize(QSize(d->iconSize, d->iconSize));
    sectionWidget->setButtonSize(QSize(d->iconSize + BUTTON_MARGIN, d->iconSize + BUTTON_MARGIN));
}

void KoToolBox::setActiveTool(KoCanvasController *canvas)
{
    Q_UNUSED(canvas);

    QString id = KoToolManager::instance()->activeToolId();
    if (id == QLatin1String("KisToolFreeRotate")) id = QStringLiteral("KisToolTransform");
    KoToolBoxButton *button = d->buttonsByToolId.value(id);
    if (button) {
        for (int g = 0; g < d->groups.size(); ++g) {
            Private::KrimbleGroup &group = d->groups[g];
            if (group.buttons.contains(button) && group.shown != button) {
                group.shown = button;
                button->setVisible(true);
                krimbleApplyGroups();
                layout()->invalidate();
                update();
            }
        }
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
    krimbleApplyGroups();
    layout()->invalidate();
    update();
}

void KoToolBox::krimbleApplyGroups()
{
    for (int g = 0; g < d->groups.size(); ++g) {
        Private::KrimbleGroup &group = d->groups[g];
        if (group.buttons.isEmpty()) continue;
        if (!group.shown) {
            const QString leaderId = krimbleToolGroupIds().at(g).first();
            group.shown = group.buttons.first();
            Q_FOREACH (KoToolBoxButton *b, group.buttons) {
                if (b->krimbleToolAction()->id() == leaderId) { group.shown = b; break; }
            }
        }
        Q_FOREACH (KoToolBoxButton *b, group.buttons) {
            if (b != group.shown) b->setVisible(false);
        }
    }
}

struct KrimbleTransformMode { const char *key; const char *name; const char *icon; };
static const QList<KrimbleTransformMode> &krimbleTransformModes()
{
    static const QList<KrimbleTransformMode> modes = {
        { "scale", "Scale", "transform_icons_main" },
        { "stretch", "Stretch", "krimble-transform-stretch" },
        { "rotate", "Rotate", "krimble-free-rotate" },
        { "shear", "Shear", "krimble-transform-shear" },
        { "perspective", "Perspective", "transform_icons_perspective" },
        { "warp", "Warp", "transform_icons_warp" },
        { "cage", "Cage", "transform_icons_cage" },
        { "liquify", "Liquify", "transform_icons_liquify_main" },
        { "mesh", "Mesh", "transform_icons_mesh" },
    };
    return modes;
}

bool KoToolBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp && event->type() == QEvent::DynamicPropertyChange
            && static_cast<QDynamicPropertyChangeEvent*>(event)->propertyName() == "krimbleTransformMode") {
        if (KoToolBoxButton *transform = d->buttonsByToolId.value(QStringLiteral("KisToolTransform"))) {
            const QString key = qApp->property("krimbleTransformMode").toString();
            QString icon;
            Q_FOREACH (const KrimbleTransformMode &mode, krimbleTransformModes()) {
                if (key == QLatin1String(mode.key) && key != QLatin1String("scale")) icon = QLatin1String(mode.icon);
            }
            transform->krimbleSetIconOverride(icon);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void KoToolBox::krimbleShowFlyout(int groupIndex)
{
    if (groupIndex < 0 || groupIndex >= d->groups.size()) return;
    Private::KrimbleGroup &group = d->groups[groupIndex];
    if (!group.shown) return;
    if (krimbleToolGroupIds().at(groupIndex).first() == QLatin1String("KisToolTransform")) {
        const QString activeId = KoToolManager::instance()->activeToolId();
        QString current = qApp->property("krimbleTransformMode").toString();
        if (current.isEmpty()) current = QStringLiteral("scale");
        const bool transformActive = (activeId == QLatin1String("KisToolTransform") || activeId == QLatin1String("KisToolFreeRotate"));
        KrimbleToolFlyout *flyout = new KrimbleToolFlyout(QList<KoToolBoxButton*>(), nullptr, this);
        Q_FOREACH (const KrimbleTransformMode &mode, krimbleTransformModes()) {
            const QString key = QLatin1String(mode.key);
            flyout->addRow(KisIconUtils::loadIcon(QLatin1String(mode.icon)), i18n(mode.name), group.shown->iconSize(), group.shown->height(),
                           transformActive && key == current, [key]() {
                qApp->setProperty("krimbleTransformModeRequest", key);
                KoToolManager::instance()->switchToolRequested(key == QLatin1String("rotate") ? QStringLiteral("KisToolFreeRotate") : QStringLiteral("KisToolTransform"));
            });
        }
        flyout->showBeside(group.shown);
        return;
    }
    QList<KoToolBoxButton*> ordered;
    Q_FOREACH (const QString &id, krimbleToolGroupIds().at(groupIndex)) {
        Q_FOREACH (KoToolBoxButton *b, group.buttons) {
            if (b->krimbleToolAction()->id() == id) ordered << b;
        }
    }
    KrimbleToolFlyout *flyout = new KrimbleToolFlyout(ordered, group.shown, this);
    flyout->showBeside(group.shown);
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
        KrimbleToolboxIcons::refreshGadgets(window());
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
        sizes << 12 << 14 << 16 << 18 << 20 << 22 << 32 << 48 << 64;
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

void KoToolBox::setupButtonStyleMenu(QMenu *menu)
{
    QActionGroup *styleGroup = new QActionGroup(menu);
    QAction *flatAction = menu->addAction(i18n("Flat"));
    QAction *raisedAction = menu->addAction(i18n("Raised"));
    for (QAction *action : {flatAction, raisedAction}) {
        action->setActionGroup(styleGroup);
        action->setCheckable(true);
    }
    const int current = KrimbleToolboxIcons::buttonStyle();
    flatAction->setChecked(current == 0);
    raisedAction->setChecked(current == 1);
    auto apply = [this](int style) {
        KrimbleToolboxIcons::setButtonStyle(style);
        Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) {
            button->update();
        }
        Q_FOREACH (QToolButton *button, d->buttons) button->update();
        update();
    };
    connect(flatAction, &QAction::triggered, this, [apply]() { apply(0); });
    connect(raisedAction, &QAction::triggered, this, [apply]() { apply(1); });
    menu->addSeparator();
    QActionGroup *faceGroup = new QActionGroup(menu);
    const QStringList faces = {i18n("Panel background"), i18n("Light gray"), i18n("White"), i18n("Dark gray")};
    for (int face = 0; face < faces.size(); ++face) {
        QAction *action = menu->addAction(faces[face]);
        action->setActionGroup(faceGroup);
        action->setCheckable(true);
        action->setChecked(KrimbleToolboxIcons::buttonFace() == face);
        connect(action, &QAction::triggered, this, [this, face]() {
            KrimbleToolboxIcons::setButtonFace(face);
            Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) button->krimbleRefreshIcon();
            Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) button->update();
            Q_FOREACH (QToolButton *button, d->buttons) button->update();
            update();
        });
    }
}

void KoToolBox::setupIconStyleMenu(QMenu *menu)
{
    QActionGroup *styleGroup = new QActionGroup(menu);
    struct Choice { QString name; int style; };
    const QList<Choice> choices = {
        {i18n("White"), KrimbleToolboxIcons::White},
        {i18n("Black"), KrimbleToolboxIcons::Black},
        {i18n("Grayscale"), KrimbleToolboxIcons::Grayscale},
    };
    for (const Choice &choice : choices) {
        QAction *action = menu->addAction(choice.name);
        action->setActionGroup(styleGroup);
        action->setCheckable(true);
        action->setChecked(KrimbleToolboxIcons::iconStyle() == choice.style);
        const int style = choice.style;
        connect(action, &QAction::triggered, this, [this, style]() {
            KrimbleToolboxIcons::setIconStyle(style);
            Q_FOREACH (KoToolBoxButton *button, d->buttonsByToolId) button->krimbleRefreshIcon();
            KrimbleToolboxIcons::refreshGadgets(window());
        });
    }
}

KoToolBoxLayout *KoToolBox::toolBoxLayout() const
{
    return d->layout;
}

#include "moc_KoToolBoxScrollArea_p.cpp"
