
static QList<QDockWidget*> krimbleRightDocks(QMainWindow *win, bool visibleOnly)
{
    QList<QDockWidget*> docks;
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (dock->isFloating() || win->dockWidgetArea(dock) != Qt::RightDockWidgetArea) {
            continue;
        }
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
    KrimbleSideBarButton(QWidget *parent, std::function<void()> onTap,
                         std::function<void()> onDragStart, std::function<void(int)> onDrag,
                         std::function<void(int)> onRelease = nullptr)
        : QWidget(parent), m_onTap(onTap), m_onDragStart(onDragStart), m_onDrag(onDrag), m_onRelease(onRelease)
    {
        setObjectName(QStringLiteral("krimbleRightPanelHandle"));
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
    KrimbleSideBarButton *button = krimbleRightPanelHandle(win);
    if (!button) {
        return;
    }
    QWidget *area = button->parentWidget();
    if (!area) {
        return;
    }
    button->setCollapsed(!anyVisible);
    button->setFixedHeight(area->height());
    button->move(area->width() - button->width(), 0);
    button->show();
    button->raise();
}

#ifdef Q_OS_ANDROID
class KrimbleEdgeCollapseFilter : public QObject
{
public:
    explicit KrimbleEdgeCollapseFilter(QMainWindow *win) : QObject(win), m_win(win) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            const ulong stamp = static_cast<QMouseEvent*>(event)->timestamp();
            const bool press = event->type() == QEvent::MouseButtonPress;
            if (stamp != 0 && stamp == (press ? m_lastPress : m_lastRelease) && event == m_lastEvent) return false;   // the same event, passed up to a parent
            (press ? m_lastPress : m_lastRelease) = stamp;
            m_lastEvent = event;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            const int x = static_cast<QMouseEvent*>(event)->globalPos().x();
            m_armed = false;
            const QList<QDockWidget*> docks = krimbleRightDocks(m_win, true);
            if (!docks.isEmpty()) {
                int left = docks.first()->mapToGlobal(QPoint(0, 0)).x();
                Q_FOREACH (QDockWidget *dock, docks) {
                    left = qMin(left, dock->mapToGlobal(QPoint(0, 0)).x());
                }
                m_armed = (x >= left - 24 && x <= left + 20);
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
    ulong m_lastPress {0};
    ulong m_lastRelease {0};
    const QEvent *m_lastEvent {nullptr};
};
#endif

#ifdef Q_OS_ANDROID
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
    g.deleteEntry("portrait_last");
    g.deleteEntry("landscape_last");
    g.sync();
    const QByteArray def = g.readEntry("default", QByteArray());
    if (!def.isEmpty()) {
        win->restoreState(QByteArray::fromBase64(def));
    }
}

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
        if (!imageMenu->findChild<QAction*>(QStringLiteral("krimbleQuickAdjustTop"))) {
            QAction *quickTop = new QAction(i18n("Quick Adjust..."), imageMenu);
            quickTop->setObjectName(QStringLiteral("krimbleQuickAdjustTop"));
            QObject::connect(quickTop, &QAction::triggered, win, [win]() { krimbleShowQuickAdjustDialog(win); });
            QAction *topFirst = imageMenu->actions().value(0);
            QAction *topDivider = new QAction(imageMenu);
            topDivider->setSeparator(true);
            imageMenu->insertAction(topFirst, quickTop);
            // moved to the File menu as "A. I. Image generation" ("Actually, file has much fewer items.")
            // // Image > Generate Image (Image Donkey)...: opens George's Perchance image generator in the browser; the address is a setting
            // // (krimblerc, [Krimble] ImageDonkeyUrl) so it can change ("Just use one or the other. It's just a url." / "Add it now and document it." / "Image menu")
            // QAction *imageDonkey = new QAction(i18n("Generate Image (Image Donkey)..."), imageMenu);
            // imageDonkey->setObjectName(QStringLiteral("krimbleImageDonkey"));
            // QObject::connect(imageDonkey, &QAction::triggered, win, []() {
            //     KConfigGroup group = KSharedConfig::openConfig()->group("Krimble");
            //     QString address = group.readEntry("ImageDonkeyUrl", QString());
            //     if (address.trimmed().isEmpty()) {
            //         address = QStringLiteral("https://perchance.org/image-donkey");
            //         group.writeEntry("ImageDonkeyUrl", address);
            //         group.config()->sync();
            //     }
            //     QDesktopServices::openUrl(QUrl::fromUserInput(address.trimmed()));
            // });
            // imageMenu->insertAction(topFirst, imageDonkey);
            imageMenu->insertAction(topFirst, topDivider);
        }
        if (imageMenu->findChild<QAction*>(QStringLiteral("krimbleBatchProcess"))) return;
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
                    QAction *firstAction = adjustments->actions().value(0);
                    QAction *divider = new QAction(adjustments);
                    divider->setSeparator(true);
                    adjustments->insertAction(firstAction, quick);
                    adjustments->insertAction(firstAction, divider);
                });
            }
        }
        Q_FOREACH (QAction *action, imageMenu->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            // if (text == QLatin1String("Image Rotation") && action->menu()) {
            if ((text == QLatin1String("Image Rotation") || text == QLatin1String("Rotate")) && action->menu()) {   // George is renaming it "Rotate"
                QMenu *rotation = action->menu();
                QObject::connect(rotation, &QMenu::aboutToShow, win, [win, rotation]() {
                    if (rotation->findChild<QAction*>(QStringLiteral("krimbleFreeRotate"))) return;
                    // QAction *freeRotate = new QAction(i18n("Free Rotate"), rotation);
                    QAction *freeRotate = new QAction(i18n("Rotate"), rotation);
                    freeRotate->setObjectName(QStringLiteral("krimbleFreeRotate"));
                    QObject::connect(freeRotate, &QAction::triggered, win, [win]() {
                        KisViewManager *view = win->viewManager();
                        if (!view) return;
                        QAction *transform = view->actionCollection()->action(QStringLiteral("KisToolFreeRotate"));
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

class KrimbleClosePopupsOnResizeFilter : public QObject
{
public:
    explicit KrimbleClosePopupsOnResizeFilter(QMainWindow *win) : QObject(win), m_win(win), m_landscape(krimbleIsLandscape(win)), m_screenLandscape(screenIsLandscape()) {}
    // the screen itself, not the window: the window's size can change for a moment without the phone being turned
    static bool screenIsLandscape()
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        return screen && screen->size().width() > screen->size().height();
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_win && event->type() == QEvent::Resize) {
            if (krimbleIsLandscape(m_win) != m_landscape) {
                m_landscape = krimbleIsLandscape(m_win);
                QMainWindow *win = m_win;
                // QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
                // Krimble 2026-11-22: closed panels came back (George: Layers "kept reopening... triggered by opening new image files").
                // This was the only code that reopens panels: it put the saved layout back whenever the window's shape flipped, and the
                // window's shape can flip for a moment when the app comes back from the file picker. Now only a real turn of the screen counts.
                if (screenIsLandscape() != m_screenLandscape && qApp->applicationState() == Qt::ApplicationActive) {
                    m_screenLandscape = screenIsLandscape();
                    // QTimer::singleShot(500, win, [win]() { krimbleRestoreSavedLayout(win); });
                    // Krimble 2026-11-22: each direction remembers its panels as they were last left (George: "Do them all in order as needed.").
                    // The layout of the direction being left is kept; the new direction gets its own last layout of this run, or, the first
                    // time it is turned to in this run, the saved startup layout.
                    // Krimble 2026-11-23: turning the phone keeps the panels as they are (George: "You shouldn't have a completely different
                    // layout with a rotate."). The per-direction layouts of the 857th are switched off:
                    // {
                    // KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
                    // g.writeEntry(m_screenLandscape ? "portrait_last" : "landscape_last", win->saveState().toBase64());
                    // g.sync();
                    // }
                    // const bool toLandscape = m_screenLandscape;
                    // QTimer::singleShot(500, win, [win, toLandscape]() {
                    // KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleLayouts");
                    // const QByteArray last = g.readEntry(toLandscape ? "landscape_last" : "portrait_last", QByteArray());
                    // if (!last.isEmpty()) win->restoreState(QByteArray::fromBase64(last));
                    // else krimbleRestoreSavedLayout(win);
                    // });
                } else {
                    qWarning() << "Krimble: window shape changed without the screen being turned; the saved layout is not put back";
                }
            }
            while (QWidget *popup = QApplication::activePopupWidget()) {
                popup->close();
            }
            Q_FOREACH (QWidget *top, QApplication::topLevelWidgets()) {
                if (QMenu *shownMenu = qobject_cast<QMenu*>(top)) {
                    if (shownMenu->isVisible()) shownMenu->hide();
                }
            }
            if (QMenuBar *bar = m_win->findChild<QMenuBar*>()) {
                bar->setActiveAction(nullptr);
            }
        }
        return false;
    }
private:
    QMainWindow *m_win;
    bool m_landscape;
    bool m_screenLandscape {false};
};
#endif


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

#ifdef Q_OS_ANDROID
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
#endif
