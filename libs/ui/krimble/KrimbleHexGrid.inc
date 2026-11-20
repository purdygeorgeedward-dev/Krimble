
#ifdef Q_OS_ANDROID
static void krimbleHexCenters(qreal size, bool flatTop, const QRectF &area, const std::function<void(const QPointF&)> &visit)
{
    const qreal s3 = 1.7320508075688772;
    if (size <= 0.0) return;
    if (!flatTop) {
        const qreal colWidth = s3 * size;
        const qreal rowHeight = 1.5 * size;
        const int firstRow = int(qFloor(area.top() / rowHeight)) - 1;
        const int lastRow = int(qCeil(area.bottom() / rowHeight)) + 1;
        for (int row = firstRow; row <= lastRow; ++row) {
            const qreal shift = (row & 1) ? colWidth / 2.0 : 0.0;
            const int firstCol = int(qFloor((area.left() - shift) / colWidth)) - 1;
            const int lastCol = int(qCeil((area.right() - shift) / colWidth)) + 1;
            for (int col = firstCol; col <= lastCol; ++col) visit(QPointF(col * colWidth + shift, row * rowHeight));
        }
    } else {
        const qreal colWidth = 1.5 * size;
        const qreal rowHeight = s3 * size;
        const int firstCol = int(qFloor(area.left() / colWidth)) - 1;
        const int lastCol = int(qCeil(area.right() / colWidth)) + 1;
        for (int col = firstCol; col <= lastCol; ++col) {
            const qreal shift = (col & 1) ? rowHeight / 2.0 : 0.0;
            const int firstRow = int(qFloor((area.top() - shift) / rowHeight)) - 1;
            const int lastRow = int(qCeil((area.bottom() - shift) / rowHeight)) + 1;
            for (int row = firstRow; row <= lastRow; ++row) visit(QPointF(col * colWidth, row * rowHeight + shift));
        }
    }
}

static QPolygonF krimbleHexPolygon(const QPointF &center, qreal size, bool flatTop)
{
    QPolygonF polygon;
    for (int i = 0; i < 6; ++i) {
        const qreal angle = qDegreesToRadians((flatTop ? 0.0 : 30.0) + 60.0 * i);
        polygon << QPointF(center.x() + size * qCos(angle), center.y() + size * qSin(angle));
    }
    return polygon;
}

class KrimbleHexDecoration : public KisCanvasDecoration
{
public:
    explicit KrimbleHexDecoration(KisView *parent) : KisCanvasDecoration(QStringLiteral("krimblehex"), parent)
    {
        setPriority(1);
    }
    qreal size = 100.0;      // center to corner, in image pixels
    bool flatTop = false;
    qreal lineWidth = 2.0;
    QColor lineColor = QColor(30, 30, 30, 210);  // the color with its opacity (the alpha)
    bool adjusting = false;  // George: "Hex overlay should be pinch zoom then drop with a tap." While true, a pinch scales the hexes and a tap drops them.

protected:
    void drawDecoration(QPainter &gc, const QRectF &updateArea, const KisCoordinatesConverter *converter, KisCanvas2 *canvas) override
    {
        Q_UNUSED(canvas);
        const QTransform toWidget = converter->imageToWidgetTransform();
        const qreal scale = KoUnit::approxTransformScale(toWidget);
        if (scale * size < 6.0) return;                       // too small to be of any use: the same rule as the grid
        const QRectF imageArea = toWidget.inverted().mapRect(updateArea);
        gc.save();
        gc.setRenderHint(QPainter::Antialiasing, true);
        // QPen pen(QColor(30, 30, 30, 210), 1.0);   // before task 41: always 1 pixel, always this color
        QPen pen(lineColor, lineWidth);
        pen.setCosmetic(true);
        gc.setPen(pen);
        gc.setBrush(Qt::NoBrush);
        const qreal sz = size;
        const bool flat = flatTop;
        krimbleHexCenters(sz, flat, imageArea.adjusted(-sz, -sz, sz, sz), [&](const QPointF &center) {
            gc.drawPolygon(toWidget.map(krimbleHexPolygon(center, sz, flat)));
        });
        if (adjusting) {
            // a note while the pattern is being scaled
            const QString note = i18n("Pinch to scale the hexes, tap to drop");
            QFont noteFont = gc.font();
            noteFont.setPointSizeF(noteFont.pointSizeF() * 1.4);
            noteFont.setBold(true);
            gc.setFont(noteFont);
            const QRectF window = gc.window();
            QRectF plate = QFontMetricsF(noteFont).boundingRect(note);
            plate.adjust(-14, -8, 14, 8);
            plate.moveCenter(QPointF(window.center().x(), window.top() + plate.height()));
            gc.setPen(Qt::NoPen);
            gc.setBrush(QColor(18, 19, 34, 220));
            gc.drawRoundedRect(plate, 6, 6);
            gc.setPen(QColor(230, 230, 230));
            gc.drawText(plate, Qt::AlignCenter, note);
        }
        gc.restore();
    }
};

// the small window that asks for the size, the orientation and (for the layer) the line thickness and color
static bool krimbleAskHex(QMainWindow *win, bool forLayer, qreal *size, bool *flatTop, qreal *thickness, QColor *color)
{
    QDialog dialog(win);
    dialog.setWindowTitle(forLayer ? i18n("Hex Pattern") : i18n("Hex Size"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *sizeLabel = new QLabel(i18n("Hex size (center to corner, pixels):"), &dialog);
    QDoubleSpinBox *sizeBox = new QDoubleSpinBox(&dialog);
    sizeBox->setRange(4.0, 5000.0);
    sizeBox->setDecimals(1);
    sizeBox->setValue(*size);
    QCheckBox *flatBox = new QCheckBox(i18n("Flat-top (otherwise pointy-top)"), &dialog);
    flatBox->setChecked(*flatTop);
    layout->addWidget(sizeLabel);
    layout->addWidget(sizeBox);
    layout->addWidget(flatBox);
    QDoubleSpinBox *thicknessBox = nullptr;
    QPushButton *colorButton = nullptr;
    QColor chosen = color ? *color : QColor(Qt::black);
    if (forLayer) {
        QLabel *thicknessLabel = new QLabel(i18n("Line thickness (pixels):"), &dialog);
        thicknessBox = new QDoubleSpinBox(&dialog);
        thicknessBox->setRange(0.1, 200.0);
        thicknessBox->setDecimals(1);
        thicknessBox->setValue(*thickness);
        colorButton = new QPushButton(i18n("Line color..."), &dialog);
        auto paintColorButton = [colorButton, &chosen]() {
            colorButton->setStyleSheet(QStringLiteral("QPushButton { border-left: 24px solid %1; }").arg(chosen.name()));
        };
        paintColorButton();
        QObject::connect(colorButton, &QPushButton::clicked, &dialog, [&]() {
            const QColor picked = QColorDialog::getColor(chosen, &dialog, i18n("Line color"));
            if (picked.isValid()) {
                chosen = picked;
                paintColorButton();
            }
        });
        layout->addWidget(thicknessLabel);
        layout->addWidget(thicknessBox);
        layout->addWidget(colorButton);
    }
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog.resize(int(scr->availableGeometry().width() * 0.9), dialog.sizeHint().height());
    }
    if (dialog.exec() != QDialog::Accepted) return false;
    *size = sizeBox->value();
    *flatTop = flatBox->isChecked();
    if (forLayer) {
        *thickness = thicknessBox->value();
        if (color) *color = chosen;
    }
    return true;
}

static bool krimbleAskHexLines(QMainWindow *win, qreal *width, QColor *color, const std::function<void()> &onChange)
{
    const qreal oldWidth = *width;
    const QColor oldColor = *color;
    QDialog dialog(win);
    dialog.setWindowTitle(i18n("Hex Lines"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *widthLabel = new QLabel(i18n("Line thickness (pixels on the screen):"), &dialog);
    QDoubleSpinBox *widthBox = new QDoubleSpinBox(&dialog);
    widthBox->setRange(0.5, 30.0);
    widthBox->setDecimals(1);
    widthBox->setSingleStep(0.5);
    widthBox->setValue(*width);
    QPushButton *colorButton = new QPushButton(i18n("Line color..."), &dialog);
    auto paintColorButton = [colorButton, color]() {
        colorButton->setStyleSheet(QStringLiteral("QPushButton { border-left: 24px solid %1; }").arg(color->name()));
    };
    paintColorButton();
    QLabel *opacityLabel = new QLabel(i18n("Line opacity (percent):"), &dialog);
    QSlider *opacitySlider = new QSlider(Qt::Horizontal, &dialog);
    opacitySlider->setRange(10, 100);
    opacitySlider->setValue(qRound(color->alphaF() * 100.0));
    layout->addWidget(widthLabel);
    layout->addWidget(widthBox);
    layout->addWidget(colorButton);
    layout->addWidget(opacityLabel);
    layout->addWidget(opacitySlider);
    QObject::connect(widthBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [width, onChange](double v) { *width = v; onChange(); });
    QObject::connect(opacitySlider, &QSlider::valueChanged, &dialog, [color, onChange](int v) { color->setAlphaF(v / 100.0); onChange(); });
    QObject::connect(colorButton, &QPushButton::clicked, &dialog, [&]() {
        const QColor picked = QColorDialog::getColor(*color, &dialog, i18n("Line color"));
        if (picked.isValid()) {
            const qreal alpha = color->alphaF();
            *color = picked;
            color->setAlphaF(alpha);
            paintColorButton();
            onChange();
        }
    });
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog.resize(int(scr->availableGeometry().width() * 0.9), dialog.sizeHint().height());
    }
    if (dialog.exec() != QDialog::Accepted) {
        *width = oldWidth;
        *color = oldColor;
        onChange();
        return false;
    }
    return true;
}

static qreal g_krimbleHexSize = 100.0;
static bool g_krimbleHexFlat = false;
static qreal g_krimbleHexThickness = 2.0;
static bool g_krimbleHexOn = false;
static qreal g_krimbleHexLineWidth = 2.0;
static QColor g_krimbleHexLineColor = QColor(30, 30, 30, 210);
static void krimbleHexSaveState()
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleHex");
    g.writeEntry("on", g_krimbleHexOn);
    g.writeEntry("size", double(g_krimbleHexSize));
    g.writeEntry("flat", g_krimbleHexFlat);
    g.writeEntry("lineWidth", double(g_krimbleHexLineWidth));   // task 41
    g.writeEntry("lineColor", g_krimbleHexLineColor);
    g.sync();
}
static void krimbleHexLoadState()
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleHex");
    g_krimbleHexOn = g.readEntry("on", false);
    g_krimbleHexSize = g.readEntry("size", double(g_krimbleHexSize));
    g_krimbleHexFlat = g.readEntry("flat", g_krimbleHexFlat);
    g_krimbleHexLineWidth = g.readEntry("lineWidth", double(g_krimbleHexLineWidth));   // task 41
    g_krimbleHexLineColor = g.readEntry("lineColor", g_krimbleHexLineColor);
}
static QColor g_krimbleHexColor = QColor(Qt::black);
static bool krimbleHexLocked()
{
    return KSharedConfig::openConfig()->group("KrimbleHex").readEntry("locked", false);
}
static void krimbleHexSetLocked(bool on)
{
    KConfigGroup g = KSharedConfig::openConfig()->group("KrimbleHex");
    g.writeEntry("locked", on);
    g.sync();
}
static bool krimbleHexRefuseWhenLocked(QWidget *parent, const QString &title)
{
    if (!krimbleHexLocked()) return false;
    QMessageBox::information(parent, title, i18n("The hex settings are locked. Unlock them in View > Show > Lock Hex Settings, or in the options of a hexagon tool."));
    return true;
}

// George: "Hex overlay should be pinch zoom then drop with a tap." While the overlay is being adjusted, two fingers scale the hexes (the distance between
// them against its last value), a short single tap that does not move drops them (the size is kept; the pattern then follows the canvas as usual), and
// other touches on the canvas are held back so that the canvas does not zoom or paint at the same time.
class KrimbleHexAdjustFilter : public QObject
{
public:
    KrimbleHexAdjustFilter(QWidget *widget, KrimbleHexDecoration *deco, KisCanvas2 *canvas)
        : QObject(widget), m_widget(widget), m_deco(deco), m_canvas(canvas)
    {
        widget->installEventFilter(this);
    }
    // George: "Yes" to: when the overlay is dropped with a tap, ask "Make this a layer?" (a transparent vector layer with the hexes at that size)
    void setOnDrop(const std::function<void()> &onDrop) { m_onDrop = onDrop; }
    void finish(bool dropped = false)
    {
        krimbleHexSaveState();
        if (m_deco) m_deco->adjusting = false;
        if (m_widget) m_widget->removeEventFilter(this);
        if (m_canvas) m_canvas->updateCanvas();
        const std::function<void()> callback = m_onDrop;
        if (dropped && callback) {
            QTimer::singleShot(0, qApp, [callback]() { callback(); });
        }
        deleteLater();
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (!m_deco || !m_deco->adjusting) {
            finish();
            return false;
        }
        const QEvent::Type type = event->type();
        if (type != QEvent::TouchBegin && type != QEvent::TouchUpdate && type != QEvent::TouchEnd) return false;
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        const QList<QTouchEvent::TouchPoint> points = touch->touchPoints();
        if (points.isEmpty()) return true;
        if (type == QEvent::TouchBegin) {
            m_pinched = false;
            m_moved = false;
            m_start = points.first().pos();
            m_lastDistance = 0.0;
            m_clock.start();
        }
        if (points.size() >= 2) {
            m_pinched = true;
            const qreal distance = QLineF(points.at(0).pos(), points.at(1).pos()).length();
            if (m_lastDistance > 0.0 && distance > 0.0) {
                m_deco->size = qBound<qreal>(8.0, m_deco->size * distance / m_lastDistance, 3000.0);
                g_krimbleHexSize = m_deco->size;
                if (m_canvas) m_canvas->updateCanvas();
            }
            m_lastDistance = distance;
        } else {
            m_lastDistance = 0.0;
            if (QLineF(points.first().pos(), m_start).length() > 24.0) m_moved = true;
            if (type == QEvent::TouchEnd && !m_pinched && !m_moved && m_clock.elapsed() < 600) {
                finish(true);
            }
        }
        return true;
    }

private:
    QPointer<QWidget> m_widget;
    QPointer<KrimbleHexDecoration> m_deco;
    QPointer<KisCanvas2> m_canvas;
    QPointF m_start;
    qreal m_lastDistance {0.0};
    bool m_pinched {false};
    bool m_moved {false};
    QElapsedTimer m_clock;
    std::function<void()> m_onDrop;
};

static KrimbleHexDecoration *krimbleHexDecorationOf(KisMainWindow *win, bool create)
{
    KisViewManager *manager = win->viewManager();
    KisView *view = win->activeView();
    if (!view || !view->canvasBase()) return nullptr;
    KisCanvasDecorationSP existing = view->canvasBase()->decoration(QStringLiteral("krimblehex"));
    KrimbleHexDecoration *hex = dynamic_cast<KrimbleHexDecoration*>(existing.data());
    if (!hex && create) {
        hex = new KrimbleHexDecoration(view);
        view->canvasBase()->addDecoration(hex);
    }
    return hex;
}

// puts the hex overlay on the picture that is open now, if the hexes are on
static void krimbleHexFollowView(KisMainWindow *win)
{
    if (!g_krimbleHexOn) return;
    KrimbleHexDecoration *deco = krimbleHexDecorationOf(win, true);
    if (!deco) return;
    deco->size = g_krimbleHexSize;
    deco->flatTop = g_krimbleHexFlat;
    deco->lineWidth = g_krimbleHexLineWidth;
    deco->lineColor = g_krimbleHexLineColor;
    deco->adjusting = false;
    deco->setVisible(true);
    if (win->activeView() && win->activeView()->canvasBase()) win->activeView()->canvasBase()->updateCanvas();
}

static void krimbleMakeHexLayer(KisMainWindow *win)
{
    KisViewManager *manager = win->viewManager();
    if (!manager || !manager->image() || !manager->nodeManager()) {
        QMessageBox::information(win, i18n("Hex Pattern"), i18n("Open a picture first."));
        return;
    }
    if (!krimbleAskHex(win, true, &g_krimbleHexSize, &g_krimbleHexFlat, &g_krimbleHexThickness, &g_krimbleHexColor)) return;
    KisImageSP image = manager->image();
    const QRectF bounds = image->bounds();
    const qreal size = g_krimbleHexSize;
    const qreal estimate = (bounds.width() / (1.7320508 * size) + 2.0) * (bounds.height() / (1.5 * size) + 2.0);
    if (estimate > 15000.0) {
        QMessageBox::information(win, i18n("Hex Pattern"), i18n("That would be %1 hexes. Choose a bigger size.", qRound(estimate)));
        return;
    }
    KisNodeSP node = manager->nodeManager()->createNode(QStringLiteral("KisShapeLayer"));
    KisShapeLayer *layer = dynamic_cast<KisShapeLayer*>(node.data());
    if (!layer) {
        QMessageBox::warning(win, i18n("Hex Pattern"), i18n("The vector layer could not be made."));
        return;
    }
    // a vector shape lives in points: image pixels per point is xRes()
    const qreal pointsPerPixel = image->xRes() > 0.0 ? 1.0 / image->xRes() : 1.0;
    KoPathShape *path = new KoPathShape();
    path->setShapeId(KoPathShapeId);
    const bool flat = g_krimbleHexFlat;
    krimbleHexCenters(size, flat, bounds.adjusted(-size, -size, size, size), [&](const QPointF &center) {
        const QPolygonF hex = krimbleHexPolygon(center, size, flat);
        path->moveTo(hex.at(0) * pointsPerPixel);
        for (int i = 1; i < hex.size(); ++i) path->lineTo(hex.at(i) * pointsPerPixel);
        path->close();
    });
    path->normalize();
    path->setStroke(toQShared(new KoShapeStroke(g_krimbleHexThickness * pointsPerPixel, g_krimbleHexColor)));
    path->setBackground(QSharedPointer<KoShapeBackground>());
    path->setName(QStringLiteral("Hex pattern"));
    layer->addShape(path);
    node->setName(image->nextLayerName(i18n("Hex Pattern")));
}

static int g_krimbleStarPoints = 5;
static int g_krimbleStarInner = 40;      // percent of the outer circle
static int g_krimbleStarSize = 0;        // outer circle diameter in pixels; 0 = half of the picture's shorter side

static void krimbleMakeStarLayer(KisMainWindow *win)
{
    KisViewManager *manager = win->viewManager();
    if (!manager || !manager->image() || !manager->nodeManager()) {
        QMessageBox::information(win, i18n("Star"), i18n("Open a picture first."));
        return;
    }
    KisImageSP image = manager->image();
    const QRectF bounds = image->bounds();
    if (g_krimbleStarSize <= 0) g_krimbleStarSize = qMax(10, int(qMin(bounds.width(), bounds.height()) / 2.0));

    QDialog dialog(win);
    dialog.setWindowTitle(i18n("Star"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QSpinBox *pointsBox = new QSpinBox(&dialog);
    pointsBox->setRange(3, 100);
    pointsBox->setValue(g_krimbleStarPoints);
    QSpinBox *innerBox = new QSpinBox(&dialog);
    innerBox->setRange(5, 95);
    innerBox->setSuffix(QStringLiteral(" %"));
    innerBox->setValue(g_krimbleStarInner);
    QSpinBox *sizeBox = new QSpinBox(&dialog);
    sizeBox->setRange(10, qMax(100, int(qMax(bounds.width(), bounds.height()) * 2.0)));
    sizeBox->setSuffix(QStringLiteral(" px"));
    sizeBox->setValue(g_krimbleStarSize);
    layout->addWidget(new QLabel(i18n("Points:"), &dialog));
    layout->addWidget(pointsBox);
    layout->addWidget(new QLabel(i18n("Inner circle (percent of the outer circle):"), &dialog));
    layout->addWidget(innerBox);
    layout->addWidget(new QLabel(i18n("Size (width of the outer circle):"), &dialog));
    layout->addWidget(sizeBox);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    g_krimbleStarPoints = pointsBox->value();
    g_krimbleStarInner = innerBox->value();
    g_krimbleStarSize = sizeBox->value();

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(QStringLiteral("StarShape"));
    if (!factory) {
        QMessageBox::warning(win, i18n("Star"), i18n("The star shape is not available in this build."));
        return;
    }
    KisNodeSP node = manager->nodeManager()->createNode(QStringLiteral("KisShapeLayer"));
    KisShapeLayer *layer = dynamic_cast<KisShapeLayer*>(node.data());
    if (!layer) {
        QMessageBox::warning(win, i18n("Star"), i18n("The vector layer could not be made."));
        return;
    }
    // a vector shape lives in points: image pixels per point is xRes() (as in the Hex Pattern layer)
    const qreal pointsPerPixel = image->xRes() > 0.0 ? 1.0 / image->xRes() : 1.0;
    const qreal tipRadius = (g_krimbleStarSize / 2.0) * pointsPerPixel;
    KoProperties props;
    props.setProperty("corners", g_krimbleStarPoints);
    props.setProperty("convex", false);
    props.setProperty("tipRadius", tipRadius);
    props.setProperty("baseRadius", tipRadius * g_krimbleStarInner / 100.0);
    props.setProperty("baseRoundness", 0.0);
    props.setProperty("tipRoundness", 0.0);
    QVariant fill;
    fill.setValue(manager->canvasResourceProvider()->fgColor().toQColor());   // the foreground color is the fill
    props.setProperty("background", fill);
    KoShape *shape = factory->createShape(&props, nullptr);
    if (!shape) {
        QMessageBox::warning(win, i18n("Star"), i18n("The star could not be made."));
        return;
    }
    shape->setStroke(KoShapeStrokeModelSP());   // no outline
    shape->setName(QStringLiteral("Star"));
    shape->setAbsolutePosition(QPointF(bounds.center().x() * pointsPerPixel, bounds.center().y() * pointsPerPixel), KoFlake::Center);
    layer->addShape(shape);
    node->setName(image->nextLayerName(i18n("Star")));
    // so that it can be moved, rotated and its circles changed at once
    QTimer::singleShot(0, win, []() { KoToolManager::instance()->switchToolRequested(QStringLiteral("InteractionTool")); });
}
#endif
