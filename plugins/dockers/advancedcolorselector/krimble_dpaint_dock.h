// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-07: the DPaint panel. George: "unified DPaint style with pixelated black icons for tools. Retro nostalgia." (gray bevelled buttons, black pixel icons, the way the
// classic toolbox looked), "Build it the way it feels right. We can tweak it after a build to see it working." One panel (Window menu: DPaint) with: the tools in two columns
// (switching to the tool and showing which one is active), UNDO / redo / CLR / select all, the color indicator (the background color as a box with the foreground color as a dot in
// it: tap the dot for the foreground color, tap the box for the background color, long press to swap), and a palette of 256 colors shown as pages of 4 x 8 wells with arrows:
// tap a well for the foreground color; RANGE then the first and the last well fills the wells between them with a blend (BACK undoes the last change); EDIT then a well changes
// that well's color. The palette is saved with the program's settings. Everything is drawn here (no picture files).
#pragma once

#include <cmath>
#include <QAbstractButton>
#include <QColorDialog>
#include <QDockWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QScrollArea>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <functional>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasObserverBase.h>
#include <KoCanvasResourceProvider.h>
#include <KoColor.h>
#include <KoColorSpaceRegistry.h>
#include <KoToolManager.h>
#include <kactioncollection.h>
#include <KisViewManager.h>
#include <kis_canvas2.h>

// Krimble 2026-11-07: the pixel icons of the DPaint panel: 16 x 16, # is a black pixel. Drawn from scratch.

static const char *const krimbleDpIcon_freehand[16] = {
    "................",
    "................",
    "................",
    "........###.....",
    "........####....",
    ".......##.###...",
    "...###.##..##...",
    "...######..###..",
    "..##.####...###.",
    "..##..##.....##.",
    "..##..###....##.",
    ".##....#......##",
    ".##...........##",
    "..............##",
    "................",
    "................"
};

static const char *const krimbleDpIcon_line[16] = {
    "................",
    "................",
    ".............#..",
    "............###.",
    "...........###..",
    "..........###...",
    ".........###....",
    "........###.....",
    ".......###......",
    "......###.......",
    ".....###........",
    "....###.........",
    "...###..........",
    "..###...........",
    ".###............",
    "..#............."
};

static const char *const krimbleDpIcon_rect[16] = {
    "................",
    "................",
    "................",
    "..############..",
    "..############..",
    "..##........##..",
    "..##........##..",
    "..##........##..",
    "..##........##..",
    "..##........##..",
    "..##........##..",
    "..############..",
    "..############..",
    "................",
    "................",
    "................"
};

// static const char *const krimbleDpIcon_ellipse[16] = {   // Krimble 2026-11-11: this one was 14 x 8 with pointed ends: "I'm not sure what the football icon is." (it looked like a football)
//     "................",
//     "................",
//     "................",
//     "................",
//     ".....######.....",
//     "...##########...",
//     "..###......###..",
//     ".##..........##.",
//     ".##..........##.",
//     "..###......###..",
//     "...##########...",
//     ".....######.....",
//     "................",
//     "................",
//     "................",
//     "................"
// };
// Krimble 2026-11-11: a rounder ellipse, 14 wide and 10 tall (about 1.4 to 1), with rounded ends and a line 2 pixels thick, so that it reads as an ellipse and not as a football.
static const char *const krimbleDpIcon_ellipse[16] = {
    "................",
    "................",
    "................",
    "....########....",
    "...##......##...",
    "..##........##..",
    ".##..........##.",
    ".##..........##.",
    ".##..........##.",
    ".##..........##.",
    "..##........##..",
    "...##......##...",
    "....########....",
    "................",
    "................",
    "................"
};

static const char *const krimbleDpIcon_polygon[16] = {
    "................",
    ".......#.#......",
    "......#####.....",
    ".....###.###....",
    "...####...####..",
    "..###.......###.",
    ".###.........###",
    ".##...........##",
    "..#...........#.",
    "..##.........##.",
    "..##.........##.",
    "..##.........##.",
    "...#.........#..",
    "...###########..",
    "...###########..",
    "................"
};

static const char *const krimbleDpIcon_fill[16] = {
    "................",
    "................",
    "................",
    "....########....",
    "................",
    "....#########...",
    "....#########...",
    "....########.#..",
    "....########.#..",
    "....########....",
    "....########.#..",
    ".............#..",
    ".....######.....",
    ".....######.....",
    "................",
    "................"
};

static const char *const krimbleDpIcon_gradient[16] = {
    "................",
    "................",
    ".##############.",
    ".#.....#.#.#.##.",
    ".#..#.#.#######.",
    ".#...#.#.#.####.",
    ".#....#.#.#####.",
    ".#.....#.#.#.##.",
    ".#..#.#.#######.",
    ".#...#.#.#.####.",
    ".#....#.#.#####.",
    ".#.....#.#.#.##.",
    ".#..#.#.#######.",
    ".##############.",
    "................",
    "................"
};

static const char *const krimbleDpIcon_text[16] = {
    "................",
    "................",
    "........##......",
    "........##......",
    ".......####.....",
    ".......####.....",
    "......##..##....",
    "......##..##....",
    ".....##....##...",
    ".....########...",
    "....##########..",
    "....##......##..",
    "...##........##.",
    "...##........##.",
    "................",
    "................"
};

static const char *const krimbleDpIcon_select[16] = {
    "................",
    ".###........###.",
    ".#............#.",
    ".#............#.",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................",
    ".#............#.",
    ".#............#.",
    ".###........###.",
    "................"
};

static const char *const krimbleDpIcon_pick[16] = {
    "................",
    "................",
    "..........####..",
    "..........####..",
    ".........#####..",
    "..........###...",
    ".........#.###..",
    "........###.###.",
    ".......###...#..",
    "......###.......",
    ".....###........",
    "....###.........",
    "...###..........",
    "..###...........",
    "..##............",
    "................"
};

static const char *const krimbleDpIcon_magnify[16] = {
    "................",
    ".....###........",
    "...#######......",
    "...##...##......",
    "..##.....##.....",
    "..##.....##.....",
    "..##.....##.....",
    "...##...##......",
    "...########.....",
    ".....###.###....",
    "........#####...",
    ".........#####..",
    "..........#####.",
    "...........#####",
    "............###.",
    ".............#.."
};

static const char *const krimbleDpIcon_zoom[16] = {
    "................",
    ".##############.",
    ".##############.",
    ".##..........##.",
    ".##..........##.",
    ".##..######..##.",
    ".##..#....#..##.",
    ".##..#....#..##.",
    ".##..#....#..##.",
    ".##..#....#..##.",
    ".##..######..##.",
    ".##..........##.",
    ".##..........##.",
    ".##############.",
    ".##############.",
    "................"
};

static const char *const krimbleDpIcon_undo[16] = {
    "................",
    ".#.#..###.......",
    ".#.#..#.#.......",
    ".#.#..#.#.......",
    ".#.#..#.#.......",
    ".###..#.#.......",
    "................",
    "................",
    ".##...###.......",
    ".#.#..#.#.......",
    ".#.#..#.#.......",
    ".#.#..#.#.......",
    ".##...###.......",
    "................",
    "................",
    "................"
};

static const char *const krimbleDpIcon_redo[16] = {
    "................",
    "................",
    "................",
    "..........#.....",
    ".........###....",
    "..........####..",
    "..#############.",
    "..############..",
    "..##.....###....",
    "..##......#.....",
    "..##............",
    "..##............",
    "..##............",
    "................",
    "................",
    "................"
};

static const char *const krimbleDpIcon_clear[16] = {
    "................",
    "................",
    "................",
    "................",
    "................",
    "###..#....##....",
    "#....#....#.#...",
    "#....#....##....",
    "#....#....#.#...",
    "###..###..#.#...",
    "................",
    "................",
    "................",
    "................",
    "................",
    "................"
};

static const char *const krimbleDpIcon_selall[16] = {
    "................",
    "................",
    "..############..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..#..........#..",
    "..############..",
    "................",
    "................"
};

static const char *const krimbleDpIcon_arrow_left[16] = {
    "................",
    "................",
    "................",
    "........#.......",
    ".......##.......",
    "......###.......",
    "....###########.",
    "...############.",
    "..#############.",
    "...############.",
    "....###########.",
    "......###.......",
    ".......##.......",
    "........#.......",
    "................",
    "................"
};

static const char *const krimbleDpIcon_arrow_right[16] = {
    "................",
    "................",
    "................",
    ".......#........",
    ".......##.......",
    ".......###......",
    ".###########....",
    ".############...",
    ".#############..",
    ".############...",
    ".###########....",
    ".......###......",
    ".......##.......",
    ".......#........",
    "................",
    "................"
};

namespace KrimbleDp {

inline QColor faceColor() { return QColor(170, 170, 170); }
inline QColor lightEdge() { return QColor(238, 238, 238); }
inline QColor darkEdge() { return QColor(70, 70, 70); }

// a 16 x 16 icon (# is a black pixel) drawn in the middle of an area, as large as a whole number of pixels per icon pixel allows
inline void paintIcon(QPainter &p, const char *const *rows, const QRect &area)
{
    const int cell = qMax(1, qMin(area.width(), area.height()) / 16);
    const int size = cell * 16;
    const int ox = area.x() + (area.width() - size) / 2;
    const int oy = area.y() + (area.height() - size) / 2;
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x)
            if (rows[y][x] == '#') p.drawRect(ox + x * cell, oy + y * cell, cell, cell);
    p.restore();
}

// the bevel: light edges at the top and the left, dark at the bottom and the right (swapped when pressed), a 2 px edge
inline void paintBevel(QPainter &p, const QRect &r, bool pressed, const QColor &face)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    p.fillRect(r, pressed ? face.darker(118) : face);
    const QColor hi = pressed ? darkEdge() : lightEdge();
    const QColor lo = pressed ? lightEdge() : darkEdge();
    for (int k = 0; k < 2; ++k) {
        p.setPen(hi);
        p.drawLine(r.left() + k, r.top() + k, r.right() - k, r.top() + k);
        p.drawLine(r.left() + k, r.top() + k, r.left() + k, r.bottom() - k);
        p.setPen(lo);
        p.drawLine(r.left() + k, r.bottom() - k, r.right() - k, r.bottom() - k);
        p.drawLine(r.right() - k, r.top() + k, r.right() - k, r.bottom() - k);
    }
    p.restore();
}

}

// a gray bevelled button with a black pixel icon, or with a few letters; square when it has an icon
class KrimbleDpButton : public QAbstractButton
{
public:
    KrimbleDpButton(const char *const *icon, const QString &text, QWidget *parent) : QAbstractButton(parent), m_icon(icon), m_text(text)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setMinimumSize(24, 24);
    }
    bool hasHeightForWidth() const override { return m_icon != nullptr; }
    int heightForWidth(int w) const override { return w; }
    QSize sizeHint() const override { return m_icon ? QSize(40, 40) : QSize(48, 28); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        KrimbleDp::paintBevel(p, rect(), isDown() || isChecked(), KrimbleDp::faceColor());
        const int nudge = (isDown() || isChecked()) ? 1 : 0;
        if (m_icon) {
            KrimbleDp::paintIcon(p, m_icon, rect().adjusted(4, 4 + nudge, -4, -4 + nudge));
        } else {
            QFont f = font();
            f.setBold(true);
            p.setFont(f);
            p.setPen(Qt::black);
            p.drawText(rect().adjusted(0, nudge, 0, nudge), Qt::AlignCenter, m_text);
        }
    }
private:
    const char *const *m_icon;
    QString m_text;
};

// the color indicator: the background color as a box, the foreground color as a dot in the middle of it
class KrimbleDpIndicator : public QWidget
{
public:
    explicit KrimbleDpIndicator(QWidget *parent) : QWidget(parent)
    {
        setMinimumHeight(44);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_hold.setSingleShot(true);
        m_hold.setInterval(550);
        connect(&m_hold, &QTimer::timeout, this, [this]() { m_longPressed = true; if (onSwap) onSwap(); });
    }
    void setColors(const QColor &foreground, const QColor &background) { m_fg = foreground; m_bg = background; update(); }
    std::function<void()> onForeground, onBackground, onSwap;
protected:
    QRect dotRect() const { const int d = qMin(width(), height()) - 14; return QRect((width() - d) / 2, (height() - d) / 2, d, d); }
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        p.fillRect(rect(), KrimbleDp::faceColor());
        const QRect box = rect().adjusted(3, 3, -3, -3);
        p.fillRect(box, m_bg);
        p.setPen(KrimbleDp::darkEdge());                                    // an inset edge: dark at the top and the left, light at the bottom and the right
        p.drawLine(box.topLeft(), box.topRight()); p.drawLine(box.topLeft(), box.bottomLeft());
        p.setPen(KrimbleDp::lightEdge());
        p.drawLine(box.bottomLeft(), box.bottomRight()); p.drawLine(box.topRight(), box.bottomRight());
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setBrush(m_fg);
        p.setPen(QPen(Qt::black, 2));
        p.drawEllipse(dotRect());
    }
    void mousePressEvent(QMouseEvent *event) override { m_longPressed = false; m_pressPos = event->pos(); m_hold.start(); }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        m_hold.stop();
        if (m_longPressed) return;                                          // the swap already happened
        const QRect dot = dotRect();
        const QPointF centre = dot.center();
        const qreal distance = std::hypot(event->pos().x() - centre.x(), event->pos().y() - centre.y());
        if (distance <= dot.width() / 2.0) { if (onForeground) onForeground(); }
        else { if (onBackground) onBackground(); }
    }
private:
    QColor m_fg = Qt::black, m_bg = Qt::white;
    QTimer m_hold;
    bool m_longPressed = false;
    QPoint m_pressPos;
};

// one page of the palette: 4 columns by 8 rows of touching wells with dark lines between them
class KrimbleDpPalette : public QWidget
{
public:
    explicit KrimbleDpPalette(QWidget *parent) : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setMinimumSize(48, 96);
    }
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return w * 2; }              // 4 x 8 wells, each as tall as wide
    QSize sizeHint() const override { return QSize(80, 160); }
    void setData(const QVector<QColor> *colors, int page) { m_colors = colors; m_page = page; update(); }
    void setMarks(int first, int second) { m_first = first; m_second = second; update(); }
    std::function<void(int)> onWell;
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        const int cell = qMax(1, width() / 4);
        for (int i = 0; i < 32; ++i) {
            const int index = m_page * 32 + i;
            const QRect r((i % 4) * cell, (i / 4) * cell, cell, cell);
            p.fillRect(r, (m_colors && index < m_colors->size()) ? m_colors->at(index) : QColor(Qt::black));
            if (index == m_first || index == m_second) { p.setPen(QPen(Qt::white, 2)); p.setBrush(Qt::NoBrush); p.drawRect(r.adjusted(2, 2, -2, -2)); }
        }
        p.setPen(QColor(20, 20, 20));
        for (int k = 0; k <= 8; ++k) p.drawLine(0, k * cell, 4 * cell, k * cell);
        for (int k = 0; k <= 4; ++k) p.drawLine(k * cell, 0, k * cell, 8 * cell);
    }
    void mousePressEvent(QMouseEvent *event) override
    {
        const int cell = qMax(1, width() / 4);
        const int column = event->pos().x() / cell, row = event->pos().y() / cell;
        if (column >= 0 && column < 4 && row >= 0 && row < 8 && onWell) onWell(m_page * 32 + row * 4 + column);
    }
private:
    const QVector<QColor> *m_colors = nullptr;
    int m_page = 0;
    int m_first = -1, m_second = -1;
};

class KrimbleDPaintDock : public QDockWidget, public KoCanvasObserverBase
{
public:
    KrimbleDPaintDock() : QDockWidget(QObject::tr("DPaint"))
    {
        loadPalette();
        auto *panel = new QWidget(this);
        auto *column = new QVBoxLayout(panel);
        column->setContentsMargins(3, 3, 3, 3);
        column->setSpacing(3);
        panel->setStyleSheet(QStringLiteral("QWidget { background: #aaaaaa; }"));   // the gray of the classic toolbox
        // Krimble 2026-11-11: George (screenshots of the DPaint panel, build 56): "...but look at this..." The panel filled the whole width of the docker, and its two columns of square buttons (and the
        // 4 column palette) grew with it, so each button was about 190 px across and every pixel of a 16 x 16 icon was a block of about 12 px: huge, chunky icons. The panel is now at most 104 px
        // wide (two buttons of about 49 px, so a pixel of an icon is 3 px, as in the classic toolbox), at the left of the docker; the rest of the docker is empty.
        panel->setMaximumWidth(104);

        // tools, two columns: the tool id and its pixel icon
        struct ToolEntry { const char *id; const char *const *icon; const char *tip; };
        const ToolEntry tools[] = {
            { "KritaShape/KisToolBrush",      krimbleDpIcon_freehand, "Freehand" },
            { "KritaShape/KisToolLine",       krimbleDpIcon_line,     "Line" },
            { "KritaShape/KisToolRectangle",  krimbleDpIcon_rect,     "Rectangle" },
            { "KritaShape/KisToolEllipse",    krimbleDpIcon_ellipse,  "Ellipse" },
            { "KisToolPolygon",               krimbleDpIcon_polygon,  "Polygon" },
            { "KritaFill/KisToolFill",        krimbleDpIcon_fill,     "Fill" },
            { "KritaFill/KisToolGradient",    krimbleDpIcon_gradient, "Gradient" },
            { "KrimbleTextTool",              krimbleDpIcon_text,     "Text" },
            { "KisToolSelectRectangular",     krimbleDpIcon_select,   "Select" },
            { "KritaSelected/KisToolColorSampler", krimbleDpIcon_pick, "Pick a color" },
            { "ZoomTool",                     krimbleDpIcon_magnify,  "Magnify" },
            { "PanTool",                      krimbleDpIcon_zoom,     "Pan" },
        };
        auto *grid = new QGridLayout;
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(0);
        int slot = 0;
        for (const ToolEntry &entry : tools) {
            auto *button = new KrimbleDpButton(entry.icon, QString(), panel);
            button->setCheckable(true);
            button->setToolTip(QObject::tr(entry.tip));
            const QString toolId = QString::fromLatin1(entry.id);
            QObject::connect(button, &QAbstractButton::clicked, button, [this, toolId]() {
                KoToolManager::instance()->switchToolRequested(toolId);
                syncActiveTool();
            });
            m_toolButtons.append(qMakePair(button, toolId));
            grid->addWidget(button, slot / 2, slot % 2);
            ++slot;
        }
        // the actions: UNDO, redo, CLR, select all
        struct ActionEntry { const char *name; const char *const *icon; const char *tip; };
        const ActionEntry actions[] = {
            { "edit_undo",  krimbleDpIcon_undo,   "Undo" },
            { "edit_redo",  krimbleDpIcon_redo,   "Redo" },
            { "clear",      krimbleDpIcon_clear,  "Clear (delete the selection or the layer's contents)" },
            { "select_all", krimbleDpIcon_selall, "Select all" },
        };
        for (const ActionEntry &entry : actions) {
            auto *button = new KrimbleDpButton(entry.icon, QString(), panel);
            button->setToolTip(QObject::tr(entry.tip));
            const QByteArray actionName(entry.name);
            QObject::connect(button, &QAbstractButton::clicked, button, [this, actionName]() { triggerAction(actionName); });
            grid->addWidget(button, slot / 2, slot % 2);
            ++slot;
        }
        column->addLayout(grid);

        // the color indicator
        m_indicator = new KrimbleDpIndicator(panel);
        m_indicator->onForeground = [this]() {
            if (!m_canvas) return;
            const QColor now = m_canvas->resourceManager()->foregroundColor().toQColor();
            const QColor picked = QColorDialog::getColor(now, this, QObject::tr("Foreground color"));
            if (picked.isValid()) setForeground(picked);
        };
        m_indicator->onBackground = [this]() {
            if (!m_canvas) return;
            const QColor now = m_canvas->resourceManager()->backgroundColor().toQColor();
            const QColor picked = QColorDialog::getColor(now, this, QObject::tr("Background color"));
            if (picked.isValid()) m_canvas->resourceManager()->setBackgroundColor(KoColor(picked, KoColorSpaceRegistry::instance()->rgb8()));
        };
        m_indicator->onSwap = [this]() {
            if (!m_canvas) return;
            const KoColor fg = m_canvas->resourceManager()->foregroundColor();
            const KoColor bg = m_canvas->resourceManager()->backgroundColor();
            m_canvas->resourceManager()->setForegroundColor(bg);
            m_canvas->resourceManager()->setBackgroundColor(fg);
        };
        column->addWidget(m_indicator);

        // the palette: a page of 4 x 8 wells and the page arrows
        m_wells = new KrimbleDpPalette(panel);
        m_wells->setData(&m_palette, m_page);
        m_wells->onWell = [this](int index) { wellTapped(index); };
        column->addWidget(m_wells);
        auto *arrows = new QHBoxLayout;
        arrows->setSpacing(0);
        auto *left = new KrimbleDpButton(krimbleDpIcon_arrow_left, QString(), panel);
        auto *right = new KrimbleDpButton(krimbleDpIcon_arrow_right, QString(), panel);
        left->setToolTip(QObject::tr("Previous page of colors"));
        right->setToolTip(QObject::tr("Next page of colors"));
        QObject::connect(left, &QAbstractButton::clicked, left, [this]() { setPage((m_page + 7) % 8); });
        QObject::connect(right, &QAbstractButton::clicked, right, [this]() { setPage((m_page + 1) % 8); });
        arrows->addWidget(left);
        arrows->addWidget(right);
        column->addLayout(arrows);
        // RANGE, EDIT, BACK
        auto *editRow = new QHBoxLayout;
        editRow->setSpacing(0);
        m_rangeButton = new KrimbleDpButton(nullptr, QObject::tr("RANGE"), panel);
        m_editButton = new KrimbleDpButton(nullptr, QObject::tr("EDIT"), panel);
        auto *backButton = new KrimbleDpButton(nullptr, QObject::tr("BACK"), panel);
        m_rangeButton->setCheckable(true);
        m_editButton->setCheckable(true);
        m_rangeButton->setToolTip(QObject::tr("Range: tap the first well, then the last; the wells between them become a blend"));
        m_editButton->setToolTip(QObject::tr("Edit: tap a well to change its color"));
        backButton->setToolTip(QObject::tr("Undo the last change to the palette"));
        editRow->addWidget(m_rangeButton);
        editRow->addWidget(m_editButton);
        editRow->addWidget(backButton);
        column->addLayout(editRow);
        m_status = new QLabel(panel);
        m_status->setStyleSheet(QStringLiteral("QLabel { color: black; font-weight: bold; }"));
        column->addWidget(m_status);
        column->addStretch(1);
        QObject::connect(m_rangeButton, &QAbstractButton::clicked, m_rangeButton, [this]() {
            m_editButton->setChecked(false);
            m_rangeStart = -1;
            m_wells->setMarks(-1, -1);
            m_status->setText(m_rangeButton->isChecked() ? QObject::tr("RANGE: tap the first well") : QString());
        });
        QObject::connect(m_editButton, &QAbstractButton::clicked, m_editButton, [this]() {
            m_rangeButton->setChecked(false);
            m_rangeStart = -1;
            m_wells->setMarks(-1, -1);
            m_status->setText(m_editButton->isChecked() ? QObject::tr("EDIT: tap a well") : QString());
        });
        QObject::connect(backButton, &QAbstractButton::clicked, backButton, [this]() {
            if (m_undoPalette.isEmpty()) return;
            m_palette = m_undoPalette;
            m_undoPalette.clear();
            savePalette();
            m_wells->setData(&m_palette, m_page);
            m_status->setText(QObject::tr("Palette change undone"));
        });

        auto *scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(panel);
        setWidget(scroll);

        // follows the colors and the active tool changed elsewhere
        m_syncTimer.setInterval(350);
        QObject::connect(&m_syncTimer, &QTimer::timeout, this, [this]() {
            if (!m_canvas || !isVisible()) return;
            m_indicator->setColors(m_canvas->resourceManager()->foregroundColor().toQColor(), m_canvas->resourceManager()->backgroundColor().toQColor());
            syncActiveTool();
        });
        m_syncTimer.start();
    }

    QString observerName() override { return QStringLiteral("KrimbleDPaintDock"); }
    void setCanvas(KoCanvasBase *canvas) override
    {
        setEnabled(canvas != nullptr);
        m_canvas = dynamic_cast<KisCanvas2*>(canvas);
    }
    void unsetCanvas() override { m_canvas = nullptr; setEnabled(false); }

private:
    void syncActiveTool()
    {
        const QString active = KoToolManager::instance()->activeToolId();
        for (const QPair<KrimbleDpButton *, QString> &entry : m_toolButtons) entry.first->setChecked(entry.second == active);
    }
    void triggerAction(const QByteArray &name)
    {
        if (!m_canvas || !m_canvas->viewManager()) return;
        if (QAction *action = m_canvas->viewManager()->actionCollection()->action(QString::fromLatin1(name))) action->trigger();
    }
    void setForeground(const QColor &color)
    {
        if (m_canvas && color.isValid()) m_canvas->resourceManager()->setForegroundColor(KoColor(color, KoColorSpaceRegistry::instance()->rgb8()));
    }
    void setPage(int page)
    {
        m_page = qBound(0, page, 7);
        QSettings().setValue(QStringLiteral("Krimble/DPaintPage"), m_page);
        m_wells->setData(&m_palette, m_page);
    }
    void wellTapped(int index)
    {
        if (index < 0 || index >= m_palette.size()) return;
        if (m_rangeButton->isChecked()) {
            if (m_rangeStart < 0) {
                m_rangeStart = index;
                m_wells->setMarks(index, -1);
                m_status->setText(QObject::tr("RANGE: tap the last well"));
            } else {
                fillRange(m_rangeStart, index);
                m_rangeStart = -1;
                m_wells->setMarks(-1, -1);
                m_rangeButton->setChecked(false);
                m_status->setText(QObject::tr("Range filled (BACK undoes it)"));
            }
        } else if (m_editButton->isChecked()) {
            const QColor picked = QColorDialog::getColor(m_palette.at(index), this, QObject::tr("Palette color"));
            if (picked.isValid()) {
                m_undoPalette = m_palette;
                m_palette[index] = picked;
                savePalette();
                m_wells->update();
            }
        } else {
            setForeground(m_palette.at(index));
        }
    }
    // the wells between the first and the last become a straight blend of the two (in RGB)
    void fillRange(int first, int second)
    {
        const int a = qMin(first, second), b = qMax(first, second);
        if (b - a < 2) return;
        m_undoPalette = m_palette;
        const QColor from = m_palette.at(a), to = m_palette.at(b);
        for (int i = a + 1; i < b; ++i) {
            const qreal t = qreal(i - a) / qreal(b - a);
            m_palette[i] = QColor::fromRgbF(from.redF() * (1.0 - t) + to.redF() * t, from.greenF() * (1.0 - t) + to.greenF() * t, from.blueF() * (1.0 - t) + to.blueF() * t);
        }
        savePalette();
        m_wells->update();
    }
    // 256 colors: 16 grays, 16 bright colors, then 14 hues of 16 lightnesses; saved with the program's settings
    void loadPalette()
    {
        QSettings settings;
        const QStringList saved = settings.value(QStringLiteral("Krimble/DPaintPalette")).toStringList();
        m_page = qBound(0, settings.value(QStringLiteral("Krimble/DPaintPage"), 0).toInt(), 7);
        if (saved.size() == 256) {
            for (const QString &hex : saved) m_palette.append(QColor(hex));
            return;
        }
        for (int i = 0; i < 16; ++i) { const int v = i * 255 / 15; m_palette.append(QColor(v, v, v)); }
        const int bright[16][3] = { {255,0,0},{255,128,0},{255,255,0},{128,255,0},{0,255,0},{0,255,128},{0,255,255},{0,128,255},{0,0,255},{128,0,255},{255,0,255},{255,0,128},{128,64,0},{255,170,120},{120,60,90},{60,90,120} };
        for (int i = 0; i < 16; ++i) m_palette.append(QColor(bright[i][0], bright[i][1], bright[i][2]));
        for (int hue = 0; hue < 14; ++hue)
            for (int light = 0; light < 16; ++light)
                m_palette.append(QColor::fromHslF(hue / 14.0, 0.85, 0.12 + 0.76 * light / 15.0));
    }
    void savePalette()
    {
        QStringList list;
        for (const QColor &color : m_palette) list << color.name(QColor::HexRgb);
        QSettings().setValue(QStringLiteral("Krimble/DPaintPalette"), list);
    }

    QPointer<KisCanvas2> m_canvas;
    QVector<QColor> m_palette, m_undoPalette;
    int m_page = 0;
    int m_rangeStart = -1;
    QList<QPair<KrimbleDpButton *, QString> > m_toolButtons;
    KrimbleDpIndicator *m_indicator = nullptr;
    KrimbleDpPalette *m_wells = nullptr;
    KrimbleDpButton *m_rangeButton = nullptr;
    KrimbleDpButton *m_editButton = nullptr;
    QLabel *m_status = nullptr;
    QTimer m_syncTimer;
};
