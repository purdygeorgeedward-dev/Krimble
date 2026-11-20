// Krimble 2026-11-19 (755th): part of KisMainWindow.cpp, moved here unchanged and included at the same place (George: "Use branching. Prioritize and implement all."). It only
// compiles as part of KisMainWindow.cpp.

static QList<QDockWidget*> krimbleRightDocks(QMainWindow *win, bool visibleOnly)
{
    QList<QDockWidget*> docks;
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->isFloating() || win->dockWidgetArea(dock) != Qt::RightDockWidgetArea) {
            continue;
        }
        // KRIMBLE 2026-10-04: the toolbox is never one of the "right-hand panels".
        if (dock->objectName() == QLatin1String("ToolBox")) {
            continue;
        }
        if (visibleOnly && !dock->isVisible()) {
            continue;
        }
        docks << dock;
    }
    return docks;
}

// Sets the width of the right-hand panel column (clamped so the panels keep their smallest width
// and the canvas keeps some room).
static void krimbleSetRightPanelsWidth(QMainWindow *win, int width)
{
    const QList<QDockWidget*> docks = krimbleRightDocks(win, true);
    if (docks.isEmpty()) {
        return;
    }
    int minimum = 80;
    Q_FOREACH (QDockWidget *dock, docks) {
        minimum = qMax(minimum, dock->minimumSizeHint().width());
    }
    const int maximum = qMax(minimum, win->width() - 140);
    width = qBound(minimum, width, maximum);
    QList<int> sizes;
    Q_FOREACH (QDockWidget *dock, docks) {
        Q_UNUSED(dock);
        sizes << width;
    }
    win->resizeDocks(docks, sizes, Qt::Horizontal);
}

static int krimbleRightPanelsWidth(QMainWindow *win)
{
    int width = 0;
    Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, true)) {
        width = qMax(width, dock->width());
    }
    return width;
}

class KrimbleSideBarButton : public QWidget
{
public:
    // onTap: quick tap. onDrag(dx): finger moved dx pixels sideways since the drag began
    // (negative = to the left); onDragStart is called once when a drag begins.
    // KRIMBLE 2026-10-06: onRelease(globalX) is called when a drag ends (the panels collapse when the finger is let
    // go close to the side). The widget is now a full-height grip along the panels' edge, see paintEvent().
    KrimbleSideBarButton(QWidget *parent, std::function<void()> onTap,
                         std::function<void()> onDragStart, std::function<void(int)> onDrag,
                         std::function<void(int)> onRelease = nullptr)
        : QWidget(parent), m_onTap(onTap), m_onDragStart(onDragStart), m_onDrag(onDrag), m_onRelease(onRelease)
    {
        setObjectName(QStringLiteral("krimbleRightPanelHandle"));
        // A thin sliver on the edge of the panels. The touch area is wider than what is drawn
        // (22 x 140), so it is easy to press, but only a 6 px sliver is visible.
        // KRIMBLE 2026-10-06: a full-height strip (36 px wide) along the panels' edge instead of a 22 x 140 button;
        // the height is set by krimbleUpdateRightPanelUi(). Old line:
        // setFixedSize(22, 140);
        setFixedWidth(36);
        m_timer.setSingleShot(true);
        m_timer.setInterval(350);
        connect(&m_timer, &QTimer::timeout, this, [this]() {
            if (m_pressed && !m_dragging) {
                beginDrag();
            }
        });
    }

    void setCollapsed(bool collapsed)
    {
        if (m_collapsed != collapsed) {
            m_collapsed = collapsed;
            update();
        }
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton) {
            event->ignore();
            return;
        }
        m_pressed = true;
        m_dragging = false;
        m_press = event->globalPos();
        m_timer.start();
        update();
        event->accept();
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!m_pressed) {
            return;
        }
        const int dx = event->globalPos().x() - m_press.x();
        // KRIMBLE 2026-10-06: a drag starts after 8 px (was 16)
        if (!m_dragging && qAbs(dx) > 8) {
            beginDrag();
        }
        if (m_dragging && m_onDrag) {
            m_onDrag(dx);
        }
        event->accept();
    }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        m_timer.stop();
        const bool wasDragging = m_dragging;
        const bool wasPressed = m_pressed;
        m_pressed = false;
        m_dragging = false;
        update();
        if (wasPressed && !wasDragging && (event->globalPos() - m_press).manhattanLength() <= 16 && m_onTap) {
            const std::function<void()> action = m_onTap;
            QTimer::singleShot(0, this, [action]() { action(); });
        }
        if (wasDragging && m_onRelease) {
            const std::function<void(int)> release = m_onRelease;
            const int globalX = event->globalPos().x();
            QTimer::singleShot(0, this, [release, globalX]() { release(globalX); });
        }
        event->accept();
    }
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QColor background = palette().color(QPalette::Highlight);
        background.setAlpha(m_dragging ? 255 : (m_pressed ? 230 : 170));
        p.setPen(Qt::NoPen);
        p.setBrush(background);
        QColor grip = palette().color(QPalette::HighlightedText);
        grip.setAlpha(230);
        if (!m_collapsed) {
            // KRIMBLE 2026-10-06: the grip is the panels' edge: a dark line with a light line beside it (so it can be
            // seen), a faint band while it is held, and a few dots in the middle. Old drawing (a 6 px sliver):
            // const QRectF sliver(width() - 10, (height() - 90) / 2.0, 6, 90);
            // p.drawRoundedRect(sliver, 3, 3);
            // p.setBrush(grip);
            // for (int i = -1; i <= 1; ++i) {
            //     p.drawEllipse(QPointF(sliver.center().x(), height() / 2.0 + i * 9), 1.2, 1.2);
            // }
            p.setRenderHint(QPainter::Antialiasing, false);
            if (m_pressed || m_dragging) {
                QColor band = palette().color(QPalette::Highlight);
                band.setAlpha(m_dragging ? 90 : 60);
                p.fillRect(rect(), band);
            }
            p.fillRect(QRect(width() - 3, 0, 1, height()), QColor(0, 0, 0, 170));          // the darker line
            p.fillRect(QRect(width() - 2, 0, 1, height()), QColor(255, 255, 255, 110));    // the highlight beside it
            p.setRenderHint(QPainter::Antialiasing, true);
            QColor dots = palette().color(QPalette::HighlightedText);
            dots.setAlpha(200);
            p.setPen(Qt::NoPen);
            p.setBrush(dots);
            for (int i = -3; i <= 3; ++i) {
                p.drawEllipse(QPointF(width() - 9, height() / 2.0 + i * 12), 1.8, 1.8);
            }
        } else {
            // Panels hidden: a slightly wider tab on the window's right edge with an arrow.
            const QRectF tab(width() - 12, (height() - 90) / 2.0, 14, 90);
            p.drawRoundedRect(tab, 5, 5);
            p.setBrush(grip);
            QPolygonF arrow;
            arrow << QPointF(width() - 4.5, height() / 2.0 - 5) << QPointF(width() - 4.5, height() / 2.0 + 5)
                  << QPointF(width() - 9.5, height() / 2.0);
            p.drawPolygon(arrow);
        }
    }

private:
    void beginDrag()
    {
        m_dragging = true;
        update();
        if (m_onDragStart) {
            m_onDragStart();
        }
    }

    std::function<void()> m_onTap;
    std::function<void()> m_onDragStart;
    std::function<void(int)> m_onDrag;
    std::function<void(int)> m_onRelease;
    QTimer m_timer;
    QPoint m_press;
    bool m_pressed {false};
    bool m_dragging {false};
    bool m_collapsed {false};
};

static KrimbleSideBarButton *krimbleRightPanelHandle(QMainWindow *win)
{
    // KRIMBLE 2026-10-04: the button hangs on the canvas area (a child of it), so look in all children.
    // return win->findChild<KrimbleSideBarButton*>(QStringLiteral("krimbleRightPanelHandle"),
    //                                               Qt::FindDirectChildrenOnly);
    return win->findChild<KrimbleSideBarButton*>(QStringLiteral("krimbleRightPanelHandle"));
}

// Hides (hide == true) or shows the right-hand panels. When hiding, the panels that were visible
// are remembered so exactly those come back.
static void krimbleSetRightPanelsHidden(QMainWindow *win, bool hide)
{
    if (hide) {
        QStringList names;
        Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, true)) {
            names << dock->objectName();
            dock->hide();
        }
        if (!names.isEmpty()) {
            win->setProperty("krimbleHiddenRightDocks", names);
        }
    } else {
        QStringList names = win->property("krimbleHiddenRightDocks").toStringList();
        if (names.isEmpty()) {
            // Nothing remembered (for example after a restart): show the usual three.
            names << QStringLiteral("sharedtooldocker") << QStringLiteral("KisLayerBox")
                  << QStringLiteral("ColorSelectorNg");
        }
        Q_FOREACH (const QString &name, names) {
            if (QDockWidget *dock = win->findChild<QDockWidget*>(name)) {
                if (!dock->isFloating() && win->dockWidgetArea(dock) == Qt::RightDockWidgetArea) {
                    dock->show();
                }
            }
        }
        win->setProperty("krimbleHiddenRightDocks", QStringList());
    }
}

// Keeps the side button's place and the menu text in step with what is on screen.
static void krimbleUpdateRightPanelUi(QMainWindow *win)
{
    const QList<QDockWidget*> visibleDocks = krimbleRightDocks(win, true);
    const bool anyVisible = !visibleDocks.isEmpty();
    const bool remembered = !win->property("krimbleHiddenRightDocks").toStringList().isEmpty();

    if (QAction *action = win->findChild<QAction*>(QStringLiteral("settings_toggle_right_panels"))) {
        const QString text = anyVisible ? i18nc("@action:inmenu", "Hide Right Panels")
                                        : i18nc("@action:inmenu", "Show Right Panels");
        if (action->text() != text) {
            action->setText(text);
        }
    }
    // KRIMBLE 2026-10-04 (new way): the button is a child of the canvas area and sits at its right edge,
    // which is exactly where the right-hand panels begin (or the edge of the screen while they are
    // hidden). Its place comes from the canvas area's own size, not from the panels' positions, so it
    // cannot end up anywhere else. Works the same with tabs and with floating picture windows.
    // (The older version worked the place out from the panels' geometry; kept below as a comment.)
    KrimbleSideBarButton *button = krimbleRightPanelHandle(win);
    if (!button) {
        return;
    }
    QWidget *area = button->parentWidget();
    if (!area) {
        return;
    }
    button->setCollapsed(!anyVisible);
    // KRIMBLE 2026-10-06: full height along the panels' edge. Old line:
    // button->move(area->width() - button->width(), (area->height() - button->height()) / 2);
    button->setFixedHeight(area->height());
    button->move(area->width() - button->width(), 0);
    button->show();
    button->raise();
}

#ifdef Q_OS_ANDROID
// Krimble 2026-10-10: dragging the strip between the canvas and the right panels all the way to the screen edge shuts the
// panels (same as Hide Right Panels). No gadget: it watches the finger. A press within the strip next to the right
// panels arms it; letting go within 56 px of the right edge of the window hides them.
class KrimbleEdgeCollapseFilter : public QObject
{
public:
    explicit KrimbleEdgeCollapseFilter(QMainWindow *win) : QObject(win), m_win(win) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::MouseButtonPress) {
            const int x = static_cast<QMouseEvent*>(event)->globalPos().x();
            m_armed = false;
            const QList<QDockWidget*> docks = krimbleRightDocks(m_win, true);
            if (!docks.isEmpty()) {
                int left = docks.first()->mapToGlobal(QPoint(0, 0)).x();
                Q_FOREACH (QDockWidget *dock, docks) {
                    left = qMin(left, dock->mapToGlobal(QPoint(0, 0)).x());
                }
                m_armed = (x >= left - 24 && x <= left + 20);   // Krimble 2026-10-12: the grab zone is inside the panel now
            }
        } else if (event->type() == QEvent::MouseButtonRelease && m_armed) {
            m_armed = false;
            const int x = static_cast<QMouseEvent*>(event)->globalPos().x();
            const int rightEdge = m_win->mapToGlobal(QPoint(m_win->width(), 0)).x();
            if (x >= rightEdge - 56) {
                QMainWindow *win = m_win;
                QTimer::singleShot(0, win, [win]() {
                    krimbleSetRightPanelsHidden(win, true);
                    krimbleUpdateRightPanelUi(win);
                });
            }
        }
        return false;
    }
private:
    QMainWindow *m_win;
    bool m_armed {false};
};
#endif

#ifdef Q_OS_ANDROID
// ============================================================================================================================
// Krimble 2026-10-12: Startup layouts (George's idea, Claude's design). "Save Layout as Startup" keeps the way the panels, toolbars and
// the toolbox are arranged for the screen's current orientation (portrait or landscape); at start, and each time the screen is rotated,
// the saved layout for that orientation is put back. "Reset Layout" forgets both and returns to the layout the app first started with.
// ============================================================================================================================
static bool krimbleIsLandscape(QMainWindow *win)
{
    return win->width() > win->height();
}

static void krimbleSaveLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    g.writeEntry(krimbleIsLandscape(win) ? "landscape" : "portrait", win->saveState().toBase64());
    g.sync();
}

static void krimbleRestoreSavedLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    const QByteArray saved = g.readEntry(krimbleIsLandscape(win) ? "landscape" : "portrait", QByteArray());
    if (!saved.isEmpty()) {
        win->restoreState(QByteArray::fromBase64(saved));
    }
}

static void krimbleResetLayout(QMainWindow *win)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
    g.deleteEntry("portrait");
    g.deleteEntry("landscape");
    g.sync();
    const QByteArray def = g.readEntry("default", QByteArray());
    if (!def.isEmpty()) {
        win->restoreState(QByteArray::fromBase64(def));
    }
}

// ============================================================================================================================
// Krimble 2026-10-12: Layer Style Presets (Claude's idea; George: "Layer Styles - sounds like Alien Skin plugins. I love it. Yes."):
// one tap puts a ready-made layer style on the active layer (with undo). They add to the layer's existing style; "Clear Style"
// switches the whole style off. The full Layer Style window (Layer > Layer Style...) can still fine-tune any of them.
// ============================================================================================================================
static void krimbleApplyLayerStyle(KisMainWindow *win, int preset)
{
    KisViewManager *view = win->viewManager();
    if (!view) return;
    KisImageSP image = view->image();
    if (!image) return;
    KisLayerSP layer = qobject_cast<KisLayer*>(view->activeNode().data());
    if (!layer) return;
    if (!view->blockUntilOperationsFinished(image)) return;

    KoCanvasResourcesInterfaceSP canvasResourcesInterface = view->canvasBase()->resourceManager()->canvasResourcesInterface();
    KisPSDLayerStyleSP oldStyle;
    if (layer->layerStyle()) {
        oldStyle = layer->layerStyle()->clone().dynamicCast<KisPSDLayerStyle>();
    } else {
        oldStyle = toQShared(new KisPSDLayerStyle("", KisGlobalResourcesInterface::instance()))
                       ->cloneWithResourcesSnapshot(KisGlobalResourcesInterface::instance(), canvasResourcesInterface);
    }
    KisPSDLayerStyleSP newStyle = oldStyle->clone().dynamicCast<KisPSDLayerStyle>();
    newStyle->setUuid(QUuid::createUuid());

    const KoColor white = KoColor::fromXML(QStringLiteral("<color channeldepth='U8'><sRGB r='1.0' g='1.0' b='1.0'/></color>"));
    const KoColor black = KoColor::fromXML(QStringLiteral("<color channeldepth='U8'><sRGB r='0.0' g='0.0' b='0.0'/></color>"));
    newStyle->setEnabled(true);
    switch (preset) {
    case 0: // Drop Shadow
        newStyle->dropShadow()->setEffectEnabled(true);
        newStyle->dropShadow()->setDistance(12);
        newStyle->dropShadow()->setSize(14);
        newStyle->dropShadow()->setOpacity(70);
        break;
    case 1: // Inner Shadow
        newStyle->innerShadow()->setEffectEnabled(true);
        break;
    case 2: // Outer Glow (white)
        newStyle->outerGlow()->setEffectEnabled(true);
        newStyle->outerGlow()->setColor(white);
        break;
    case 3: // Inner Glow
        newStyle->innerGlow()->setEffectEnabled(true);
        break;
    case 4: // Stroke, white, 4 px, outside
        newStyle->stroke()->setEffectEnabled(true);
        newStyle->stroke()->setSize(4);
        newStyle->stroke()->setColor(white);
        newStyle->stroke()->setPosition(psd_stroke_outside);
        break;
    case 5: // Stroke, black, 4 px, outside
        newStyle->stroke()->setEffectEnabled(true);
        newStyle->stroke()->setSize(4);
        newStyle->stroke()->setColor(black);
        newStyle->stroke()->setPosition(psd_stroke_outside);
        break;
    case 6: // Bevel and Emboss
        newStyle->bevelAndEmboss()->setEffectEnabled(true);
        break;
    default: // Clear Style
        newStyle->setEnabled(false);
        break;
    }
    KisPSDLayerStyleSP finalStyle = newStyle->cloneWithResourcesSnapshot(KisGlobalResourcesInterface::instance(), canvasResourcesInterface);
    KUndo2Command *cmd = new KisSetLayerStyleCommand(layer, layer->layerStyle(), finalStyle);
    KisProcessingApplicator::runSingleCommandStroke(image, cmd);
    image->waitForDone();
}

// Krimble 2026-11-11: George: "I really want [industry standard] style interfaces for RGB to CMYK." Adds "Custom CMYK..." to the Edit menu, just above Preferences (once, when the menu first opens).
// It is added here, in code, because krita5.xmlgui is George's own file. It will become Edit > Color Settings... (with the working spaces, as in the industry standard dialog) when that dialog is built.
static void krimbleInstallColorSettings(KisMainWindow *win)
{
    QMenu *editMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Edit")) editMenu = top->menu();
    }
    if (!editMenu || editMenu->property("krimbleColorSettings").toBool()) return;
    editMenu->setProperty("krimbleColorSettings", true);
    QObject::connect(editMenu, &QMenu::aboutToShow, win, [win, editMenu]() {
        if (editMenu->findChild<QAction*>(QStringLiteral("krimbleCustomCmyk"))) return;
        QAction *cmyk = new QAction(i18n("Custom CMYK..."), editMenu);
        cmyk->setObjectName(QStringLiteral("krimbleCustomCmyk"));
        QObject::connect(cmyk, &QAction::triggered, win, [win]() { krimbleShowCustomCmykDialog(win); });
        QAction *before = nullptr;
        Q_FOREACH (QAction *action, editMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (!before && text.startsWith(QLatin1String("Preferences"))) before = action;
        }
        if (before) {
            QAction *divider = new QAction(editMenu);
            divider->setSeparator(true);
            editMenu->insertAction(before, cmyk);
            editMenu->insertAction(before, divider);
        } else {
            editMenu->addSeparator();
            editMenu->addAction(cmyk);
        }
    });
}

// Adds "Batch Process..." at the end of the Image menu, after a divider (once, when the menu first opens). It was at the end of the
// File menu, below Exit (George: "I think it should be in the image menu, not below exit.").
static void krimbleInstallImageExtras(KisMainWindow *win)
{
    QMenu *imageMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Image")) imageMenu = top->menu();
    }
    if (!imageMenu || imageMenu->property("krimbleImageExtras").toBool()) return;
    imageMenu->setProperty("krimbleImageExtras", true);
    QObject::connect(imageMenu, &QMenu::aboutToShow, win, [win, imageMenu]() {
        // Krimble 2026-10-30: George: "QuickAdjust should be the first item in the Image menu. It's a shortcut to adjustments." First item, then a divider
        // (the same item stays first in Image > Adjustments).
        if (!imageMenu->findChild<QAction*>(QStringLiteral("krimbleQuickAdjustTop"))) {
            QAction *quickTop = new QAction(i18n("Quick Adjust..."), imageMenu);
            quickTop->setObjectName(QStringLiteral("krimbleQuickAdjustTop"));
            QObject::connect(quickTop, &QAction::triggered, win, [win]() { krimbleShowQuickAdjustDialog(win); });
            QAction *topFirst = imageMenu->actions().value(0);
            QAction *topDivider = new QAction(imageMenu);
            topDivider->setSeparator(true);
            imageMenu->insertAction(topFirst, quickTop);
            imageMenu->insertAction(topFirst, topDivider);
        }
        if (imageMenu->findChild<QAction*>(QStringLiteral("krimbleBatchProcess"))) return;
        // Krimble 2026-10-13 (11th): Quick Adjust... at the bottom of the Image > Adjustments submenu (George: "launched from Image Adjust";
        // the place inside the submenu, the bottom, is my choice).
        Q_FOREACH (QAction *action, imageMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Adjustments") && action->menu()) {
                QMenu *adjustments = action->menu();
                QObject::connect(adjustments, &QMenu::aboutToShow, win, [win, adjustments]() {
                    if (adjustments->findChild<QAction*>(QStringLiteral("krimbleQuickAdjust"))) return;
                    QAction *quick = new QAction(i18n("Quick Adjust..."), adjustments);
                    quick->setObjectName(QStringLiteral("krimbleQuickAdjust"));
                    QObject::connect(quick, &QAction::triggered, win, [win]() { krimbleShowQuickAdjustDialog(win); });
                    // adjustments->addAction(quick);          (bottom of the submenu, 1.0.36)
                    // Krimble 2026-10-14: George (1.0.36): "Should be first instead of last under Image - Adjust menu." First, then a divider.
                    QAction *firstAction = adjustments->actions().value(0);
                    QAction *divider = new QAction(adjustments);
                    divider->setSeparator(true);
                    adjustments->insertAction(firstAction, quick);
                    adjustments->insertAction(firstAction, divider);
                });
            }
        }
        // Krimble 2026-10-15: George (1.0.36): "I want Free rotation in the image rotation menu." and "Rotate the graphic like the old default transform
        // behavior. Free transform - rotate." A new item, "Free Rotate", first in Image > Image Rotation (the place inside the submenu is my choice),
        // starts the Free Transform tool, where dragging outside the box turns the graphic by any angle.
        Q_FOREACH (QAction *action, imageMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Image Rotation") && action->menu()) {
                QMenu *rotation = action->menu();
                QObject::connect(rotation, &QMenu::aboutToShow, win, [win, rotation]() {
                    if (rotation->findChild<QAction*>(QStringLiteral("krimbleFreeRotate"))) return;
                    QAction *freeRotate = new QAction(i18n("Free Rotate"), rotation);
                    freeRotate->setObjectName(QStringLiteral("krimbleFreeRotate"));
                    QObject::connect(freeRotate, &QAction::triggered, win, [win]() {
                        KisViewManager *view = win->viewManager();
                        if (!view) return;
                        QAction *transform = view->actionCollection()->action(QStringLiteral("KisToolFreeRotate"));   // Krimble 2026-10-18: the tool that turns with two fingers
                        if (transform) transform->trigger();
                    });
                    QAction *firstAction = rotation->actions().value(0);
                    QAction *divider = new QAction(rotation);
                    divider->setSeparator(true);
                    rotation->insertAction(firstAction, freeRotate);
                    rotation->insertAction(firstAction, divider);
                });
            }
        }
        QAction *batch = new QAction(i18n("Batch Process..."), imageMenu);
        batch->setObjectName(QStringLiteral("krimbleBatchProcess"));
        QObject::connect(batch, &QAction::triggered, win, [win]() { krimbleShowBatch(win); });
        // Krimble 2026-10-13: Upscale... (Claude's idea)
        QAction *upscale = new QAction(i18n("Upscale..."), imageMenu);
        QObject::connect(upscale, &QAction::triggered, win, [win]() { krimbleShowUpscale(win); });
        imageMenu->addSeparator();
        imageMenu->addAction(upscale);
        imageMenu->addAction(batch);
    });
}

// Adds "Rotation Gadget" (a check mark) at the end of the View menu (once, when the menu first opens): turns the canvas rotation
// dial in the status bar off and on (Claude's design; George: "I sometimes accidentally trigger the rotation gadget on the bottom.
// Can we turn it off and on at will?").
static void krimbleInstallViewExtras(KisMainWindow *win)
{
    QMenu *viewMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("View")) viewMenu = top->menu();
    }
    if (!viewMenu || viewMenu->property("krimbleViewExtras").toBool()) return;
    viewMenu->setProperty("krimbleViewExtras", true);
    QObject::connect(viewMenu, &QMenu::aboutToShow, win, [win, viewMenu]() {
        QAction *gadget = viewMenu->findChild<QAction*>(QStringLiteral("krimbleRotationGadget"));
        if (!gadget) {
            gadget = new QAction(i18n("Rotation Gadget"), viewMenu);
            gadget->setObjectName(QStringLiteral("krimbleRotationGadget"));
            gadget->setCheckable(true);
            viewMenu->addSeparator();
            viewMenu->addAction(gadget);
            QObject::connect(gadget, &QAction::toggled, win, [win](bool on) {
                KisConfig cfg(false);
                cfg.writeEntry<int>("krimble_show_rotation_gadget", on ? 1 : 0);
                // (only the status bar's dial, not the angle selectors in the panels)
                if (KisAngleSelector *dial = win->statusBar() ? win->statusBar()->findChild<KisAngleSelector*>() : nullptr) {
                    if (!on) dial->setVisible(false);
                    else if (win->viewManager() && win->viewManager()->image()) dial->setVisible(true);
                }
            });
        }
        QSignalBlocker blocker(gadget);
        // gadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_rotation_gadget", 1) != 0);   (shown by default)
        gadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_rotation_gadget", 0) != 0);   // George: hidden by default

        // Krimble 2026-10-20: George (1.0.40): "Thumb tripped over the gadget on the bottom bar and blew up the image accidentally. Toggle off just
        // like the little rotate gadget on the bar." View > Zoom Gadget, right after Rotation Gadget: the zoom slider in the status bar on or off
        // (setting krimble_show_zoom_gadget, hidden by default).
        QAction *zoomGadget = viewMenu->findChild<QAction*>(QStringLiteral("krimbleZoomGadget"));
        if (!zoomGadget) {
            zoomGadget = new QAction(i18n("Zoom Gadget"), viewMenu);
            zoomGadget->setObjectName(QStringLiteral("krimbleZoomGadget"));
            zoomGadget->setCheckable(true);
            viewMenu->addAction(zoomGadget);
            QObject::connect(zoomGadget, &QAction::toggled, win, [win](bool on) {
                KisConfig cfg(false);
                cfg.writeEntry<int>("krimble_show_zoom_gadget", on ? 1 : 0);
                if (QWidget *slider = win->statusBar() ? win->statusBar()->findChild<QWidget*>(QStringLiteral("krimbleZoomGadget")) : nullptr) {
                    slider->setVisible(on);
                }
            });
        }
        QSignalBlocker zoomBlocker(zoomGadget);
        zoomGadget->setChecked(KisConfig(true).readEntry<int>("krimble_show_zoom_gadget", 0) != 0);
    });
}

// Adds "Style Presets" right after "Layer Style..." in the Layer menu (once, when the menu first opens).
static void krimbleInstallLayerExtras(KisMainWindow *win)
{
    QMenu *layerMenu = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Layer")) layerMenu = top->menu();
    }
    if (!layerMenu || layerMenu->property("krimbleLayerExtras").toBool()) return;
    layerMenu->setProperty("krimbleLayerExtras", true);
    QObject::connect(layerMenu, &QMenu::aboutToShow, win, [win, layerMenu]() {
        if (layerMenu->property("krimbleLayerExtrasAdded").toBool()) return;
        layerMenu->setProperty("krimbleLayerExtrasAdded", true);
        QAction *before = nullptr;
        const QList<QAction*> actions = layerMenu->actions();
        for (int i = 0; i < actions.size(); ++i) {
            QString text = actions.at(i)->text();
            text.remove(QLatin1Char('&'));
            if (text.startsWith(QLatin1String("Layer Style")) && i + 1 < actions.size()) before = actions.at(i + 1);
        }
        QMenu *sub = new QMenu(i18n("Style Presets"), layerMenu);
        static const char *const names[] = { "Drop Shadow", "Inner Shadow", "Outer Glow", "Inner Glow", "Stroke (white, 4 px)",
                                             "Stroke (black, 4 px)", "Bevel and Emboss", "Clear Style" };
        for (int i = 0; i < 8; ++i) {
            QAction *a = sub->addAction(i18n(names[i]));
            QObject::connect(a, &QAction::triggered, win, [win, i]() { krimbleApplyLayerStyle(win, i); });
        }
        layerMenu->insertMenu(before, sub);
    });
}

// Krimble 2026-10-11: after a screen rotation an open menu (the Window menu) stayed on screen and could not be closed.
// Any popup that is open when the main window changes size is closed.
class KrimbleClosePopupsOnResizeFilter : public QObject
{
public:
    explicit KrimbleClosePopupsOnResizeFilter(QMainWindow *win) : QObject(win), m_win(win), m_landscape(krimbleIsLandscape(win)) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win && event->type() == QEvent::Resize) {
            // Krimble 2026-10-12: after the screen turns, put back the layout saved for the new orientation (if there is one)
            if (krimbleIsLandscape(m_win) != m_landscape) {
                m_landscape = krimbleIsLandscape(m_win);
                QMainWindow *win = m_win;
                QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
            }
            while (QWidget *popup = QApplication::activePopupWidget()) {
                popup->close();
            }
            // Krimble 2026-10-18 (5th): a menu that is on the screen but is not the active popup (the case of "stuck") is hidden too
            Q_FOREACH (QWidget *top, QApplication::topLevelWidgets()) {
                if (QMenu *shownMenu = qobject_cast<QMenu*>(top)) {
                    if (shownMenu->isVisible()) shownMenu->hide();
                }
            }
            // Krimble 2026-10-11 (2nd): also let go of the menu bar entry (the "Window" button stayed pressed after a rotation).
            if (QMenuBar *bar = m_win->findChild<QMenuBar*>()) {
                bar->setActiveAction(nullptr);
            }
        }
        return false;
    }
private:
    QMainWindow *m_win;
    bool m_landscape;
};
#endif

// ---- Older placement code (2026-10-03/04), kept for reference ----
//     // Watch the right-hand panels too, so the button follows them when they move or resize.
//     if (QObject *watcher = win->findChild<QObject*>(QStringLiteral("krimbleRightPanelsFilter"))) {
//         Q_FOREACH (QDockWidget *dock, krimbleRightDocks(win, false)) {
//             if (!dock->property("krimbleWatched").toBool()) {
//                 dock->installEventFilter(watcher);
//                 dock->setProperty("krimbleWatched", true);
//             }
//         }
//     }
//     KrimbleSideBarButton *button = krimbleRightPanelHandle(win);
//     if (!button) {
//         return;
//     }
//     const bool show = anyVisible || remembered;
//     button->setCollapsed(!anyVisible);
//     if (!show) {
//         button->hide();
//         return;
//     }
//     int x = win->width() - button->width();
//     int y = (win->height() - button->height()) / 2;
//     if (anyVisible) {
//         // Centre it on the bar between the canvas and the left edge of the right-hand panels,
//         // half way down the panels.
//         // KRIMBLE 2026-10-04: only panels that really lie to the right of the middle of the canvas
//         // area count. A panel hidden behind a tab, or not yet laid out, has an old position
//         // (left edge 0) and used to pull the sliver over the toolbox. Without any usable panel
//         // the sliver goes to the right edge of the canvas area.
//         QWidget *central = win->centralWidget();
//         const int canvasMiddle = central ? central->geometry().center().x() : win->width() / 2;
//         int dockLeft = win->width();
//         int top = win->height();
//         int bottom = 0;
//         bool found = false;
//         Q_FOREACH (QDockWidget *dock, visibleDocks) {
//             const QRect g = dock->geometry();
//             if (g.width() < 20 || g.left() <= canvasMiddle) {
//                 continue;
//             }
//             found = true;
//             dockLeft = qMin(dockLeft, g.left());
//             top = qMin(top, g.top());
//             bottom = qMax(bottom, g.bottom());
//         }
//         if (!found) {
//             button->hide();
//             return;
//         }
//         x = dockLeft - button->width() + 4;      // 18 px left of the edge, 4 px over it
//         y = (top + bottom) / 2 - button->height() / 2;
//     }
//     button->move(qBound(0, x, win->width() - button->width()), qBound(0, y, win->height() - button->height()));
//     button->show();
//     button->raise();
// }
//

// Watches the main window and keeps the side button and menu text up to date.
class KrimbleRightPanelsFilter : public QObject
{
public:
    explicit KrimbleRightPanelsFilter(QMainWindow *win) : QObject(win), m_win(win)
    {
        setObjectName(QStringLiteral("krimbleRightPanelsFilter"));
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        // KRIMBLE 2026-10-04: the update must run AFTER the window has laid out its panels. A
        // layout request reaches this filter before that, so the panels' positions were still
        // the old (empty) ones and the side button was drawn at the left edge, on top of the
        // toolbox. Now every change is handled one step later, and moves/resizes of the
        // right-hand panels themselves also trigger it.
        bool relevant = false;
        if (watched == m_win) {
            relevant = (event->type() == QEvent::LayoutRequest || event->type() == QEvent::Resize
                        || event->type() == QEvent::Show);
        } else if (watched->objectName() == QLatin1String("krimbleCanvasArea")) {
            // the canvas area changed size or was shown: its right edge moved
            relevant = (event->type() == QEvent::Resize || event->type() == QEvent::Show);
        } else if (qobject_cast<QDockWidget*>(watched)) {
            relevant = (event->type() == QEvent::Move || event->type() == QEvent::Resize
                        || event->type() == QEvent::Show || event->type() == QEvent::Hide);
        }
        if (relevant && !m_pending) {
            m_pending = true;
            QTimer::singleShot(0, this, [this]() {
                m_pending = false;
                krimbleUpdateRightPanelUi(m_win);
            });
        }
        return false;
    }

private:
    QMainWindow *m_win;
    bool m_pending {false};
};
// END KRIMBLE RIGHT PANELS

#ifdef Q_OS_ANDROID
// KRIMBLE 2026-10-04: the toolbox docked too eagerly (George: "the toolbox wanting a bit too desperately to
// dock"). While the toolbox is being dragged as a floating window, it may dock in a side of the window only when
// the finger is within a small distance of that side; anywhere else in the window it just stays a floating
// window. It works by changing the dock areas the toolbox is allowed in while the finger moves (the window
// system asks for them each time). When the finger is lifted, all areas are allowed again.
// KrimbleDockTolerance::Distance is that distance in pixels (logical, so about 2.5 times as many on the screen).
// Krimble 2026-10-15: George (1.0.36): "Floating windows have very small buttons for close and stuff, and ugly aqua colored title bar. Make
// tiny adjustment to increase size, then make the bar blue." and, asked whether the color should match the UI: "I guess the blue gray." The title bar
// of an image window (QMdiSubWindow) is drawn by the style: its height, which also sizes its buttons, is `PM_TitleBarHeight`, and its color is the
// palette's highlight (the aqua). This style is put on the image windows only: the title bar is 25% higher (so its buttons are 25% bigger) and
// drawn in the app's blue-gray, rgb(55,57,82), "Krimble dark". It is my best guess that this is the bar George means; not seen on a phone.
// Krimble 2026-10-17: George (1.0.37): "1. Giant mega lines again for bevels instead of one pixel." and "8. Docked panels have pointless mega bevel lines.
// Detached panels have no bevel lines at all. See how the Tool Options panel disappears into the toolbar cluster because there are no hilight or
// shadow lines on the edges? And they should be only one pixel thick on each edge. Toolbars need them too." A transparent overlay is put on every
// panel and every toolbar. A docked panel gets a light line along its top and a dark line along its bottom; a detached panel and a toolbar get a
// light line along the top and left edges and a dark line along the bottom and right edges (a raised look). Each line is ONE DEVICE PIXEL thick.
// (This replaces the lines drawn by KoDockWidgetTitleBar of the 135th entry, which were 3 device px each and only on docked panels.)
// Krimble 2026-11-15 (3rd): true if the docked panel is in a tab group: its name is on a visible tab of a tab row of the main window that has two or more tabs (a lone panel can have a tab bar object with one tab that is not drawn).
static bool krimbleDockInTabGroup(QDockWidget *dock)
{
    if (!dock || dock->isFloating()) return false;
    QMainWindow *mainWin = qobject_cast<QMainWindow*>(dock->parentWidget());
    if (!mainWin) return false;
    Q_FOREACH (QTabBar *tabBar, mainWin->findChildren<QTabBar*>(QString(), Qt::FindDirectChildrenOnly)) {
        if (!tabBar->isVisible() || tabBar->count() < 2) continue;
        for (int i = 0; i < tabBar->count(); ++i) {
            if (tabBar->tabText(i) == dock->windowTitle()) return true;
        }
    }
    return false;
}
#endif   // Krimble 2026-11-19 (755th): the Android-only part continues across the split files
