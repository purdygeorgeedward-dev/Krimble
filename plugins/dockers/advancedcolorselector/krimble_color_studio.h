// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <functional>
#include <QImage>
#include <cmath>
#include <QSizePolicy>
#include <QMenu>
#include <QTimer>
#include <QAction>
#include <QCursor>

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QHideEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QAbstractSlider>   // Krimble 2026-11-05: the vertical hue strip
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QSettings>
#include <QSignalBlocker>
#include <QColor>
#include <QVector>
#include <QGroupBox>
#include <QFrame>
#include <QPointer>
#include <QTabWidget>
#include <QStack>
#include <QComboBox>
#include <QTabletEvent>
#include <QResizeEvent>
#include <QPainterPath>
#include <QImage>
#include <QClipboard>   // Krimble 2026-11-08: the HEX copy button
#include <QApplication>
#include <KoToolManager.h>   // the eyedropper button switches to the color sampler tool
#include "krimble_color_canvas.h"   // Krimble 2026-11-08: the Picker page, built from the measurements of the mockup

#include <algorithm>
#include "kis_canvas2.h"
#include "kis_canvas_resource_provider.h"
#include "KoColor.h"
#include "KoColorSpaceRegistry.h"

// Swatch button supports a touch long-press without requiring a mouse right-click.
// Krimble 2026-11-06 (3rd): the recent colors' cell: a square that touches its neighbors (see KrimbleSwatchButton)
class KrimbleSquareCell : public QPushButton
{
public:
    explicit KrimbleSquareCell(QWidget *parent=nullptr) : QPushButton(parent) {}
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return w; }
};

class KrimbleSwatchButton : public QPushButton
{
public:
    explicit KrimbleSwatchButton(QWidget *parent=nullptr) : QPushButton(parent) {
        m_hold.setSingleShot(true);
        m_hold.setInterval(550);
        connect(&m_hold, &QTimer::timeout, this, [this]() {
            m_longPressed=true;
            if (onLongPress) onLongPress(mapToGlobal(rect().center()));
        });
    }
    std::function<void(const QPoint &)> onLongPress;
    // Krimble 2026-11-06 (3rd): George: "Why are you rendering all the elements with spaces between them? The mockups have the color wells separated by a black line." The wells
    // are cells that touch, each as tall as it is wide (the grid gives each the same width), with a 1 px dark line between them; no gap, no rounded corners.
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return w; }
protected:
    void mousePressEvent(QMouseEvent *event) override {
        m_longPressed=false;
        if (event->button()==Qt::LeftButton) m_hold.start();
        QPushButton::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (!rect().adjusted(-8,-8,8,8).contains(event->pos())) m_hold.stop();
        QPushButton::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        m_hold.stop();
        if (m_longPressed) {
            // Suppress clicked() after the long-press context menu without
            // leaving the underlying QPushButton in a pressed state.
            setDown(false);
            setCheckable(false);
            event->accept();
            return;
        }
        QPushButton::mouseReleaseEvent(event);
    }
    void hideEvent(QHideEvent *event) override {
        m_hold.stop();
        QPushButton::hideEvent(event);
    }
private:
    QTimer m_hold;
    bool m_longPressed=false;
};

// Hue/saturation/value picker. The painting is local to the widget; edits are
// committed through the same canvas resource manager as Krita's other selectors.
class KrimbleSvSquare : public QWidget
{
public:
    explicit KrimbleSvSquare(QWidget *parent = nullptr) : QWidget(parent) {
        // setMinimumHeight(100);
        // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        // Krimble 2026-10-30: George: "At the very least that color gradient box should stretch out as far as the user wants." It grows in both directions.
        // setMinimumSize(100, 100);
        // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // Krimble 2026-11-05: George: Color Studio "is a complete mess" (screenshots 2026-11-05): the square was stretched into a wide band. It
        // follows the mockup: a near-square (0.95 tall for its width) next to the vertical hue strip, and it grows with the width it gets.
        // setMinimumSize(100, 95);
        setMinimumSize(60, 57);   // Krimble 2026-11-06: narrower, so the docked panel does not need a horizontal scroll bar (was 100 x 95)
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
    }
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return int(w * 0.95); }
    QSize sizeHint() const override { return QSize(240, 228); }
    QColor selected() const { return QColor::fromHsv(m_hue, m_sat, m_val); }
    void setColor(QColor c) {
        if (c.hsvHue() >= 0 && m_hue != c.hsvHue()) { m_hue = c.hsvHue(); m_cachedHue = -1; }
        m_sat = c.hsvSaturation(); m_val = c.value(); update();
    }
    void setHue(int hue) { if (m_hue != hue) { m_hue = hue; m_cachedHue = -1; update(); } }
    std::function<void(QColor)> onPick;
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        const int w = qMax(1, width()-1), h = qMax(1, height()-1);
        // The SV field depends only on hue and dimensions; reuse its pixels
        // when moving the selection cursor or synchronizing RGB/HEX values.
        // if (m_cachedHue != m_hue || m_cachedSquare.size() != size()) {
        //     m_cachedSquare = QImage(size(), QImage::Format_RGB32);
        //     for (int y=0; y<height(); ++y)
        //         for (int x=0; x<width(); ++x)
        //             m_cachedSquare.setPixelColor(x,y,QColor::fromHsv(m_hue, x*255/w, 255-y*255/h));
        //     m_cachedHue = m_hue;
        // }
        // p.drawImage(0,0,m_cachedSquare);
        // Krimble 2026-10-30: George: "That color box should be a gradient filled vector image, right?" Two layered gradients painted at draw time, so no
        // per-pixel loop and no cached bitmap: white to the pure hue from left to right (saturation), then transparent to black from top to bottom (value).
        {
            // Krimble 2026-11-08: the mockup's color square has rounded corners and a thin dark outline: the field is clipped to a rounded rectangle (9 px)
            QPainterPath roundedField;
            // roundedField.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), 9, 9);
            roundedField.addRoundedRect(QRectF(0.5, 0.5, width() - 1, height() - 1), qMax<qreal>(6.0, width() * 0.0227), qMax<qreal>(6.0, width() * 0.0227));   // Krimble 2026-11-08: 14 of the mockup's 617 units
            p.setRenderHint(QPainter::Antialiasing, true);
            p.setClipPath(roundedField);
            const QRect field(0, 0, width(), height());
            QLinearGradient saturation(0, 0, width()-1, 0);
            saturation.setColorAt(0.0, Qt::white);
            saturation.setColorAt(1.0, QColor::fromHsv(m_hue, 255, 255));
            p.fillRect(field, saturation);
            QLinearGradient value(0, 0, 0, height()-1);
            value.setColorAt(0.0, QColor(0, 0, 0, 0));
            value.setColorAt(1.0, QColor(0, 0, 0, 255));
            p.fillRect(field, value);
            p.setClipping(false);
            p.setPen(QPen(QColor(8, 10, 24, 220), 1.5));
            p.setBrush(Qt::NoBrush);
            p.drawPath(roundedField);
        }
        p.setRenderHint(QPainter::Antialiasing);
        QPointF pt(m_sat*w/255.0, (255-m_val)*h/255.0);
        // p.setPen(QPen(Qt::black, 3)); p.setBrush(Qt::NoBrush);
        // p.drawEllipse(pt, 7, 7);
        // p.setPen(QPen(Qt::white, 2)); p.drawEllipse(pt, 6, 6);
        // Krimble 2026-11-08: the mockup's ring: outer radius 3.1% of the square's width, a white line 0.95% thick (measured: about 19 and 6 of 617 units)
        {
            const qreal ringRadius = qMax<qreal>(6.0, width() * 0.027);
            const qreal ringWidth = qMax<qreal>(2.0, width() * 0.0095);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(0, 0, 0, 150), ringWidth + 1.6)); p.drawEllipse(pt, ringRadius, ringRadius);
            p.setPen(QPen(Qt::white, ringWidth)); p.drawEllipse(pt, ringRadius, ringRadius);
        }
    }
    void mousePressEvent(QMouseEvent *e) override { pick(e->pos()); }
    void mouseMoveEvent(QMouseEvent *e) override {
        if (e->buttons() & Qt::LeftButton) pick(e->pos());
    }
private:
    void pick(QPoint point) {
        m_sat = qBound(0, point.x()*255/qMax(1,width()-1), 255);
        m_val = 255-qBound(0, point.y()*255/qMax(1,height()-1), 255);
        update();
        if (onPick) onPick(selected());
    }
    int m_hue=210, m_sat=150, m_val=220;
    int m_cachedHue=-1;
    QImage m_cachedSquare;
};


/**
 * Independent mixing scratchpad: its raster is never connected to the
 * document's projection or undo stack. Color picking is the only operation
 * that can explicitly send a sampled color back to Color Studio.
 */
class KrimbleMixingPad : public QWidget
{
public:
    explicit KrimbleMixingPad(QWidget *parent=nullptr) : QWidget(parent) {
        // setMinimumHeight(220);
        setMinimumHeight(140);   // Krimble 2026-11-06 (2nd): George: "Make them fit on screen." The pad may be shorter (was 220)
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setAttribute(Qt::WA_AcceptTouchEvents, false); // Qt synthesizes mouse strokes
        setAutoFillBackground(false);
    }
    void setColor(const QColor &color) { if (color.isValid()) m_color=color; }
    void setBrushSize(int value) { m_size=qBound(1,value,100); }
    void setOpacity(int value) { m_opacity=qBound(1,value,100); }
    void setEraser(bool enabled) { m_eraser=enabled; m_sampling=false; }
    void setSampling(bool enabled) { m_sampling=enabled; if (enabled) m_eraser=false; }
    std::function<void(QColor)> onSample;
    // Krimble 2026-11-05: George (Mixer screenshots): strokes only showed on the left part of the pad, and the right part stayed blank. The saved
    // undo and redo pictures keep the size the pad had when they were taken, so after the panel was resized an undo or a redo put a smaller picture
    // back, and everything drawn after that was cut off at its edge. The picture is made the size of the pad again whenever it is replaced.
    // Krimble 2026-11-06: George: "Can we get things to render in native resolution so they're sharp? ... bluR". The pad's picture was one pixel per logical pixel and
    // was stretched 3.4 times to the screen (the phone's density), so strokes looked soft and blocky. The picture now has one pixel per SCREEN pixel
    // (devicePixelRatio set), and everything is still drawn in logical coordinates, so nothing else changes.
    QImage newPaintImage(const QSize &logicalSize) const {
        const qreal ratio=devicePixelRatioF();
        QImage image(QSize(qMax(1,qRound(logicalSize.width()*ratio)),qMax(1,qRound(logicalSize.height()*ratio))),QImage::Format_ARGB32_Premultiplied);
        image.setDevicePixelRatio(ratio);
        image.fill(Qt::transparent);
        return image;
    }
    void ensureSize() {
        const QImage wanted=newPaintImage(size());
        if (m_paint.size()==wanted.size()) return;
        QImage fitted=wanted;
        if (!m_paint.isNull()) { QPainter painter(&fitted); painter.drawImage(0,0,m_paint); }
        m_paint=fitted;
    }
    void clearPad() { snapshot(); ensureSize(); m_paint.fill(Qt::transparent); update(); }
    void undo() {
        if (m_undo.isEmpty()) return;
        m_redo.append(m_paint);
        m_paint=m_undo.takeLast();
        ensureSize();
        update();
    }
    void redo() {
        if (m_redo.isEmpty()) return;
        m_undo.append(m_paint);
        m_paint=m_redo.takeLast();
        ensureSize();
        update();
    }
protected:
    void resizeEvent(QResizeEvent *event) override {
        // if (m_paint.size()!=event->size()) {
        //     QImage newImage(event->size(),QImage::Format_ARGB32_Premultiplied);
        //     newImage.fill(Qt::transparent);
        //     if (!m_paint.isNull()) {
        //         QPainter painter(&newImage);
        //         painter.drawImage(0,0,m_paint);
        //     }
        //     m_paint=newImage;
        // }
        {
            QImage newImage=newPaintImage(event->size());
            if (m_paint.size()!=newImage.size()) {
                if (!m_paint.isNull()) {
                    QPainter painter(&newImage);
                    painter.drawImage(0,0,m_paint);
                }
                m_paint=newImage;
            }
        }
        QWidget::resizeEvent(event);
    }
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.fillRect(rect(), QColor("#eee9df"));
        const int cell=20;
        p.setPen(QColor(0,0,0,12));
        for (int x=cell;x<width();x+=cell) p.drawLine(x,0,x,height());
        for (int y=cell;y<height();y+=cell) p.drawLine(0,y,width(),y);
        p.drawImage(0,0,m_paint);
    }
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button()!=Qt::LeftButton) return;
        if (m_sampling) {
            QColor sampled=sample(event->pos());
            if (sampled.isValid() && onSample) onSample(sampled);
            return;
        }
        snapshot();
        ++m_strokeId;
        m_drawing=true;
        m_last=event->pos();
        stroke(m_last,m_last);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (!m_drawing || !(event->buttons() & Qt::LeftButton)) return;
        stroke(m_last,event->pos());
        m_last=event->pos();
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button()==Qt::LeftButton) m_drawing=false;
    }
private:
    void snapshot() {
        if (m_paint.isNull()) return;
        m_undo.append(m_paint);
        while (m_undo.size()>15) m_undo.removeFirst();
        m_redo.clear();
    }
    QColor sample(QPoint at) const {
        // the picture has one pixel per screen pixel now: the logical point is scaled to it (before it was one to one)
        const QPoint deviceAt=QPointF(at*m_paint.devicePixelRatio()).toPoint();
        if (!m_paint.rect().contains(deviceAt)) return QColor();
        const QColor c=m_paint.pixelColor(deviceAt);
        return c.alpha() ? c : QColor("#eee9df");
    }
    void stroke(const QPoint &from,const QPoint &to) {
        if (m_paint.isNull()) return;
        QPainter p(&m_paint);
        p.setRenderHint(QPainter::Antialiasing);
        p.setCompositionMode(m_eraser ? QPainter::CompositionMode_Clear
                                      : QPainter::CompositionMode_SourceOver);
        QColor c=m_color;
        c.setAlphaF(m_opacity/100.0);
        if (m_eraser) {
            QPen pen(c, m_size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p.setPen(pen);
            p.drawLine(from,to);
        } else {
#if 0   // Krimble 2026-11-06: the oil-bristle default (strands with ridges and broken coverage); George: "Default brush for the scratch pad panel should be a soft generic commonly used brush, nothing weird."
            // Default oil-bristle simulation: individual semi-transparent
            // strands create ridges and broken coverage as colors overlap.
            // This is local QPainter rendering, not a Krita brush preset.
            const QPointF delta=QPointF(to-from);
            const qreal length=std::hypot(delta.x(),delta.y());
            const QPointF tangent=length>0.01 ? delta/length : QPointF(1,0);
            const QPointF normal(-tangent.y(),tangent.x());
            const int bristles=qBound(5,m_size/2,34);
            for (int strand=0;strand<bristles;++strand) {
                const qreal t=(strand+0.5)/bristles-0.5;
                const qreal offset=t*m_size;
                const quint32 seed=quint32(strand*7919u + m_strokeId*104729u);
                const qreal wobble=qreal(seed%17)/17.0-0.5;
                QColor pigment=c;
                pigment.setAlphaF(qBound(0.03,(m_opacity/100.0)*(0.18+0.56*qreal((seed>>4)%101)/100.0),1.0));
                if (strand%4==0) pigment=pigment.lighter(120);
                else if (strand%5==0) pigment=pigment.darker(115);
                p.setPen(QPen(pigment,qMax(1.0,m_size/qreal(bristles)*1.4),
                              Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
                const QPointF spread=normal*(offset+wobble*m_size*0.12);
                if ((seed%11)!=0)
                    p.drawLine(QPointF(from)+spread,QPointF(to)+spread);
            }
#endif
            // Krimble 2026-11-06: a soft round brush, the ordinary one: round stamps with a soft edge (a radial fade from the middle to nothing at the rim), placed
            // close together along the stroke so they run into a smooth line, and building up smoothly where strokes overlap. Size and opacity work as before.
            const QPointF delta=QPointF(to-from);
            const qreal length=std::hypot(delta.x(),delta.y());
            const qreal radius=qMax(1.0,m_size/2.0);
            const qreal spacing=qMax(1.0,m_size*0.12);
            const int steps=qMax(1,int(length/spacing));
            QColor inner=c;
            inner.setAlphaF(qBound(0.02,(m_opacity/100.0)*0.28,1.0));   // each stamp is light; the stamps along the line add up to the chosen opacity
            QColor middle=inner;
            middle.setAlphaF(inner.alphaF()*0.55);
            QColor rim=inner;
            rim.setAlphaF(0.0);
            p.setPen(Qt::NoPen);
            for (int step=0;step<=steps;++step) {
                const QPointF at=QPointF(from)+delta*(qreal(step)/steps);
                QRadialGradient fade(at,radius);
                fade.setColorAt(0.0,inner);
                fade.setColorAt(0.55,middle);
                fade.setColorAt(1.0,rim);
                p.setBrush(fade);
                p.drawEllipse(at,radius,radius);
            }
        }
        update();
    }
    QImage m_paint;
    QVector<QImage> m_undo,m_redo;
    QColor m_color=QColor("#459ad3");
    QPoint m_last;
    int m_size=18,m_opacity=85;
    quint32 m_strokeId=0;
    bool m_drawing=false,m_eraser=false,m_sampling=false;
};

// Krimble 2026-11-05: the mockup's vertical rainbow hue strip (red at the top, through yellow, green, cyan, blue and magenta, back to red at the
// bottom) with a white ring handle. It is a slider (0 to 359), so the code that used the old horizontal Hue slider works unchanged.
class KrimbleHueStrip : public QAbstractSlider
{
public:
    explicit KrimbleHueStrip(QWidget *parent = nullptr) : QAbstractSlider(parent)
    {
        setOrientation(Qt::Vertical);
        setRange(0, 359);
        // setFixedWidth(24);
        // setFixedWidth(16);   // Krimble 2026-11-06: George: "Thinner hue rainbow." The rainbow is 10 px wide (was 18; the strip is 16 px wide with 3 px each side, was 24)
        // Krimble 2026-11-08: the width is set by the measured layout (KrimbleColorCanvas): the strip is 82 of the card's 1178 units wide
        setMinimumHeight(60);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        // Krimble 2026-11-08: the widget is 90 units wide and the strip 82: the strip has a margin of 4 units (4/90 of the width) on every side, so the white handle can reach beyond it
        const qreal margin = width() * 4.0 / 90.0;
        // const QRectF strip = QRectF(rect()).adjusted(3, 1, -3, -1);
        const QRectF strip = QRectF(rect()).adjusted(margin, margin, -margin, -margin);
        QLinearGradient hues(0, strip.top(), 0, strip.bottom());
        const int stops[7] = {0, 60, 120, 180, 240, 300, 359};
        for (int i = 0; i < 7; ++i) hues.setColorAt(i / 6.0, QColor::fromHsv(stops[i], 255, 255));
        p.setBrush(hues);
        // p.setPen(QPen(QColor("#8498bd"), 1));
        // p.drawRoundedRect(strip, 5, 5);
        p.setPen(QPen(QColor(8, 10, 28, 230), 1.4));
        p.drawRoundedRect(strip, strip.width() / 2, strip.width() / 2);   // a pill: fully rounded ends (the mockup's)
        const qreal y = strip.top() + strip.height() * value() / 359.0;
        // p.setBrush(Qt::NoBrush);
        // p.setPen(QPen(Qt::black, 3));
        // p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
        // p.setPen(QPen(Qt::white, 2));
        // p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
        // the handle of the mockup: a white pill outline, 28% as tall as the strip is wide, reaching 4 units beyond the strip at each side
        {
            const qreal handleHeight = strip.width() * 0.28;
            const qreal line = qMax<qreal>(2.0, strip.width() * 0.06);
            const QRectF handle(line / 2, y - handleHeight / 2, width() - line, handleHeight);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(0, 0, 0, 150), line + 1.6));
            p.drawRoundedRect(handle, handleHeight / 2, handleHeight / 2);
            p.setPen(QPen(Qt::white, line));
            p.drawRoundedRect(handle, handleHeight / 2, handleHeight / 2);
        }
    }
    void mousePressEvent(QMouseEvent *e) override { setFromY(e->pos().y()); }
    void mouseMoveEvent(QMouseEvent *e) override { if (e->buttons() & Qt::LeftButton) setFromY(e->pos().y()); }
private:
    // void setFromY(int y) { setValue(qBound(0, int(359.0 * (y - 1) / qMax(1, height() - 2)), 359)); }
    void setFromY(int y) {
        const qreal margin = width() * 4.0 / 90.0;   // Krimble 2026-11-08: the strip is inside the widget by this margin (see paintEvent)
        setValue(qBound(0, int(359.0 * (y - margin) / qMax<qreal>(1.0, height() - 2 * margin)), 359));
    }
};

// Krimble 2026-11-05: a picture that fills the width it is given and keeps its shape (George's ColorStudio logo, white on a transparent background),
// drawn smoothly at the screen's own resolution so it is much bigger than the text and stays sharp.
class KrimbleStudioLogoLabel : public QWidget
{
public:
    explicit KrimbleStudioLogoLabel(const QString &resource, QWidget *parent = nullptr) : QWidget(parent), m_pix(resource)
    {
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
        setAccessibleName(QStringLiteral("ColorStudio"));
        setMaximumWidth(340);   // Krimble 2026-11-05: large, but not wider than 340 px of layout on a wide panel
    }
    bool hasHeightForWidth() const override { return !m_pix.isNull(); }
    int heightForWidth(int w) const override { return m_pix.isNull() ? 0 : int(double(w) * m_pix.height() / m_pix.width()); }
    QSize sizeHint() const override { return QSize(200, heightForWidth(200)); }
protected:
    void resizeEvent(QResizeEvent *) override
    {
        // the height follows the width, so the logo keeps its shape whatever layout it sits in
        const int h = heightForWidth(width());
        if (h > 0 && (height() != h || minimumHeight() != h)) setFixedHeight(h);
    }
    void paintEvent(QPaintEvent *) override
    {
        if (m_pix.isNull()) return;
        const qreal ratio = devicePixelRatioF();
        const QSize target = size() * ratio;
        if (m_cache.isNull() || m_cacheSize != target) {
            m_cache = m_pix.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_cache.setDevicePixelRatio(ratio);
            m_cacheSize = target;
        }
        QPainter p(this);
        p.drawPixmap(QPointF(0, 0), m_cache);
    }
private:
    QPixmap m_pix;
    QPixmap m_cache;
    QSize m_cacheSize;
};

class KrimbleColorStudio : public QWidget
{
public:
    explicit KrimbleColorStudio(QWidget *parent=nullptr) : QWidget(parent) {
        // Krimble 2026-11-06: George: "Smaller font for color panel." The text of this panel is 15% smaller than the program's (labels, number boxes, HEX).
        // Krimble 2026-11-06 (4th): George: "use smaller fonts": 25% smaller (was 15%).
        {
            QFont smaller = font();
            if (smaller.pixelSize() > 0) smaller.setPixelSize(qMax(8, int(smaller.pixelSize() * 0.75)));
            else smaller.setPointSizeF(qMax(6.0, smaller.pointSizeF() * 0.75));
            setFont(smaller);
        }
        auto *layout = new QVBoxLayout(this);
        // layout->setContentsMargins(5,5,5,5); layout->setSpacing(5);
        layout->setContentsMargins(3,3,3,3); layout->setSpacing(2);   // Krimble 2026-11-06 (3rd): George: "more and more unnecessary negative space": 3 px margins and 2 px between the parts (was 5 and 5)
        // auto *header = new QHBoxLayout;
        // auto *title = new QLabel(tr("COLOR STUDIO"), this);
        // QFont f=title->font(); f.setBold(true); title->setFont(f);
        // header->addWidget(title); header->addStretch();
        // auto *add = new QPushButton(tr("+ Swatch"), this);
        // add->setToolTip(tr("Add foreground color to palette"));
        // header->addWidget(add);
        // layout->addLayout(header);
        // Krimble 2026-11-05: George's ColorStudio logo graphic, 70% of the panel width, replaces the text heading ("+ Swatch" is in the palette heading below).
        // Krimble 2026-11-06: George: "Remove Color Studio logo." The logo (KrimbleStudioLogoLabel and the picture) stays in the program, not shown.
#if 0
        {
            auto *logoRow = new QHBoxLayout;
            // logoRow->addStretch(1);
            // logoRow->addWidget(new KrimbleStudioLogoLabel(QStringLiteral(":/dark_krimble-colorstudio-logo.png"), this), 7);
            // logoRow->addStretch(1);
            // Krimble 2026-11-05 (2nd): George: the title logo is twice the size it should be and should be left aligned: 40% of the panel width, at the left.
            logoRow->addWidget(new KrimbleStudioLogoLabel(QStringLiteral(":/dark_krimble-colorstudio-logo.png"), this), 4);
            logoRow->addStretch(6);
            layout->addLayout(logoRow);
        }
#endif
#if 0   // Krimble 2026-11-08: the plus button of the measured layout (KrimbleColorCanvas::add) replaces the "+ Swatch" button
        auto *add = new QPushButton(tr("+ Swatch"), this);
        add->setToolTip(tr("Add foreground color to palette"));
#endif
        // Krimble 2026-10-30: George: "Color docker designed by ChatGPT was too ambitious and doesn't need tabs. Should be all in one design." and, of the mixing
        // pad: "It should be a panel." One page: the picker, values, recent colors and palette. The mixing pad is its own panel, "Mixer"
        // (krimble_mixer_dock.h); the code that built the Mixer tab is kept below inside #if 0.
        // auto *tabs = new QTabWidget(this);
        // auto *picker = new QWidget(tabs);
        // auto *mixer = new QWidget(tabs);
#if 0   // Krimble 2026-11-08: the old picker widget with its box layout (a styled frame and margins), replaced by the measured canvas below
        auto *picker = new QWidget(this);
        // Krimble 2026-11-08: George: "color panel is underwhelming" and, of the Color mockup: "look at the beautiful gradients and the great designs, layouts". The picker is in a rounded
        // frame with a smooth gradient (the mockup's inner panel): #krimbleStudioCard in KrimbleVisualStyle.h (a plain widget needs WA_StyledBackground to show it).
        picker->setObjectName(QStringLiteral("krimbleStudioCard"));
        picker->setAttribute(Qt::WA_StyledBackground, true);
        auto *pickerLayout = new QVBoxLayout(picker);
        // pickerLayout->setContentsMargins(0,4,0,0);
        pickerLayout->setContentsMargins(6,8,6,6);
        // pickerLayout->setSpacing(5);
        pickerLayout->setSpacing(2);   // Krimble 2026-11-06 (3rd): was 5
        // tabs->addTab(picker,tr("Picker"));
        // tabs->addTab(mixer,tr("Mixer"));
        // layout->addWidget(tabs);
        layout->addWidget(picker, 1);
#endif
        // Krimble 2026-11-08: George: "Measure everything for proportions. Don't wing it." and "Remember this app asks the user to scale it as a percentage. Issues of scale are not a
        // problem." The Picker page is the mockup's card, measured (color_panel_measurements.md), placed by KrimbleColorCanvas at the mockup's proportions at any width.
        // Krimble 2026-11-08: George: "make the new tabs work just like the mockups." A tab strip above the card (Picker, Sliders, Values, Palette, History); the first three share
        // the card (its top part changes), Palette and History are cards of their own.
        m_tabs = new KrimbleColorTabBar(this);
        layout->addWidget(m_tabs);
        m_ui = new KrimbleColorCanvas(this);
        layout->addWidget(m_ui);
        layout->addStretch(1);
        m_sliders = new KrimbleSlidersTop(m_ui);
        m_values = new KrimbleValuesTop(m_ui);
        m_sliders->state = &m_state;
        m_values->state = &m_state;
        m_ui->sliders = m_sliders;
        m_ui->values = m_values;
        m_sliders->onColor = [this](const QColor &c) { select(c); };
        m_values->onColor = [this](const QColor &c) { select(c); };
        m_sliders->onAlpha = [this](int a) { setAlpha(a); };
        m_values->onAlpha = [this](int a) { setAlpha(a); };
        m_tabs->onTab = [this](int i) { showTab(i); };
#if 0   // Krimble 2026-10-30: the Mixer tab, now the Mixer panel (krimble_mixer_dock.h)
        auto *mixer = new QWidget(this);
        auto *mixLayout = new QVBoxLayout(mixer);
        mixLayout->setContentsMargins(2,6,2,2);
        auto *mixTools=new QHBoxLayout;
        auto *paintButton=new QPushButton(tr("Paint"),mixer);
        auto *eraseButton=new QPushButton(tr("Erase"),mixer);
        auto *sampleButton=new QPushButton(tr("Sample"),mixer);
        for (QPushButton *button : {paintButton,eraseButton,sampleButton}) {
            button->setCheckable(true);
            mixTools->addWidget(button);
        }
        paintButton->setChecked(true);
        mixLayout->addLayout(mixTools);
        m_mixingPad=new KrimbleMixingPad(mixer);
        mixLayout->addWidget(new QLabel(tr("Oil Bristle  •  paint and blend colors"),mixer));
        mixLayout->addWidget(m_mixingPad,1);
        auto *brushControls=new QHBoxLayout;
        brushControls->addWidget(new QLabel(tr("Size"),mixer));
        auto *brushSize=new QSpinBox(mixer);
        brushSize->setRange(1,100); brushSize->setValue(18);
        brushControls->addWidget(brushSize);
        brushControls->addWidget(new QLabel(tr("Opacity"),mixer));
        auto *brushOpacity=new QSpinBox(mixer);
        brushOpacity->setRange(1,100); brushOpacity->setValue(85);
        brushOpacity->setSuffix(QStringLiteral("%"));
        brushControls->addWidget(brushOpacity);
        mixLayout->addLayout(brushControls);
        auto *padActions=new QHBoxLayout;
        auto *undoButton=new QPushButton(tr("Undo"),mixer);
        auto *redoButton=new QPushButton(tr("Redo"),mixer);
        auto *clearButton=new QPushButton(tr("Clear"),mixer);
        auto *addMixed=new QPushButton(tr("+ Palette"),mixer);
        for (QPushButton *button : {undoButton,redoButton,clearButton,addMixed}) padActions->addWidget(button);
        mixLayout->addLayout(padActions);
        connect(paintButton,&QPushButton::clicked,this,[=]() {
            paintButton->setChecked(true); eraseButton->setChecked(false);
            sampleButton->setChecked(false); m_mixingPad->setEraser(false);
        });
        connect(eraseButton,&QPushButton::clicked,this,[=]() {
            paintButton->setChecked(false); eraseButton->setChecked(true);
            sampleButton->setChecked(false); m_mixingPad->setEraser(true);
        });
        connect(sampleButton,&QPushButton::clicked,this,[=]() {
            paintButton->setChecked(false); eraseButton->setChecked(false);
            sampleButton->setChecked(true); m_mixingPad->setSampling(true);
        });
        connect(brushSize,qOverload<int>(&QSpinBox::valueChanged),m_mixingPad,&KrimbleMixingPad::setBrushSize);
        connect(brushOpacity,qOverload<int>(&QSpinBox::valueChanged),m_mixingPad,&KrimbleMixingPad::setOpacity);
        connect(undoButton,&QPushButton::clicked,m_mixingPad,&KrimbleMixingPad::undo);
        connect(redoButton,&QPushButton::clicked,m_mixingPad,&KrimbleMixingPad::redo);
        connect(clearButton,&QPushButton::clicked,m_mixingPad,&KrimbleMixingPad::clearPad);
        connect(addMixed,&QPushButton::clicked,this,[this]() {
            if (!m_colors.contains(m_current)) m_colors.append(m_current);
            while (m_colors.size()>24) m_colors.remove(0);
            savePalette(); rebuildSwatches();
        });
        m_mixingPad->onSample=[this](QColor c){ select(c); };
#endif   // Krimble 2026-10-30
#if 0   // Krimble 2026-11-05: the first all-in-one picker (a stretched square, a horizontal Hue slider, full-width value rows, a wide color bar); see the new one below
        m_square = new KrimbleSvSquare(this);
        pickerLayout->addWidget(m_square);
        auto *hueRow = new QHBoxLayout;
        hueRow->addWidget(new QLabel(tr("Hue"),this));
        m_hue = new QSlider(Qt::Horizontal,this);
        m_hue->setRange(0,359);
        m_hue->setStyleSheet(QStringLiteral(
            "QSlider::groove:horizontal { height:12px;"
            "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "stop:0 red,stop:0.167 yellow,stop:0.333 lime,"
            "stop:0.5 cyan,stop:0.667 blue,stop:0.833 magenta,stop:1 red); }"
            "QSlider::handle:horizontal { width:20px; margin:-5px 0;"
            "border-radius:9px; border:2px solid white; background:#3d507c; }"));
        hueRow->addWidget(m_hue);
        pickerLayout->addLayout(hueRow);
        auto *values = new QGridLayout;
        const char *names[]={"R","G","B"};
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(names[i]),this),0,i*2);
            m_rgb[i]=new QSpinBox(this); m_rgb[i]->setRange(0,255);
            values->addWidget(m_rgb[i],0,i*2+1);
        }
        values->addWidget(new QLabel(tr("HEX"),this),1,0);
        m_hex=new QLineEdit(this); m_hex->setMaxLength(7);
        m_hex->setPlaceholderText(QStringLiteral("#RRGGBB"));
        values->addWidget(m_hex,1,1,1,5);
        const char *hsvNames[] = {"H", "S", "V"};
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(hsvNames[i]),this),2,i*2);
            m_hsv[i] = new QSpinBox(this);
            m_hsv[i]->setRange(0, i==0 ? 359 : 100);
            m_hsv[i]->setSuffix(i==0 ? QStringLiteral("°") : QStringLiteral("%"));
            values->addWidget(m_hsv[i],2,i*2+1);
        }
        pickerLayout->addLayout(values);
        m_preview=new QLabel(this); m_preview->setFixedHeight(24);
        pickerLayout->addWidget(m_preview);
        pickerLayout->addWidget(new QLabel(tr("RECENT COLORS"),this));
        m_recentLayout = new QGridLayout;
        m_recentLayout->setSpacing(3);
        pickerLayout->addLayout(m_recentLayout);
        pickerLayout->addWidget(new QLabel(tr("MY PALETTE  •  tap to select, long-press to edit"),this));
        m_swatches=new QGridLayout; m_swatches->setSpacing(3);
        pickerLayout->addLayout(m_swatches);
#endif   // Krimble 2026-11-05
        // Krimble 2026-11-05: George: "Do your best to make panels that match the mockups." The Picker page of the Color mockup, all in one (no tabs: George,
        // 2026-10-30, "should be all in one design"): the color square, the vertical hue strip and a column of H S V / R G B / HEX boxes; under them the
        // foreground and background colors with a swap button and the recent colors in one row; then the palette with "+ Swatch" in its heading.
#if 0   // Krimble 2026-11-08: the box layout of the picker (square and hue strip, value column, color row, recent colors, palette heading and cells), replaced by the measured canvas
        m_square = new KrimbleSvSquare(this);
        m_hue = new KrimbleHueStrip(this);
        auto *topRow = new QHBoxLayout;
        topRow->setSpacing(3);   // Krimble 2026-11-06 (3rd): was 6
        topRow->addWidget(m_square, 5);
        topRow->addWidget(m_hue);
        auto *values = new QGridLayout;
        values->setSpacing(1);   // Krimble 2026-11-06 (3rd): was 4
        const char *hsvNames[] = {"H", "S", "V"};
        const char *names[]={"R","G","B"};
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(hsvNames[i]),this),i,0);
            m_hsv[i] = new QSpinBox(this);
            m_hsv[i]->setRange(0, i==0 ? 359 : 100);
            m_hsv[i]->setSuffix(i==0 ? QStringLiteral("°") : QStringLiteral("%"));
            // m_hsv[i]->setFixedWidth(46);   // Krimble 2026-11-06 (4th): George: "narrower areas for the values at the top" 46 (was 62, 76 first)
            m_hsv[i]->setFixedWidth(66);   // Krimble 2026-11-08: George (screenshot): the boxes showed ONE digit (H 2, S 7, V 9, R 7, G 1, B 2): the fields of the style sheet have padding and a frame now, so 46 px was too narrow; 66 px shows "316°"
            values->addWidget(m_hsv[i],i,1);
        }
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(names[i]),this),i+3,0);
            m_rgb[i]=new QSpinBox(this); m_rgb[i]->setRange(0,255);
            // m_rgb[i]->setFixedWidth(46);   // Krimble 2026-11-06 (4th): 46 (was 62, 76 first)
            m_rgb[i]->setFixedWidth(66);   // Krimble 2026-11-08: see above (the three digits of "240")
            values->addWidget(m_rgb[i],i+3,1);
        }
        values->addWidget(new QLabel(tr("HEX"),this),6,0);
        m_hex=new QLineEdit(this); m_hex->setMaxLength(7);
        m_hex->setPlaceholderText(QStringLiteral("#RRGGBB"));
        // m_hex->setFixedWidth(58);   // Krimble 2026-11-06 (4th): 58 (was 76); the text is 25% smaller now
        m_hex->setFixedWidth(78);   // Krimble 2026-11-08: "#4BBCFD" was cut off (78 px shows "#F035BE" whole; 66 cut its first letters: tested in real Qt)
        values->addWidget(m_hex,6,1);
        // Krimble 2026-11-06 (3rd): the number boxes are 20 px tall (they were about 24), so the column of six and the HEX box take less height
        // for (QSpinBox *box : {m_hsv[0], m_hsv[1], m_hsv[2], m_rgb[0], m_rgb[1], m_rgb[2]}) box->setFixedHeight(20);
        // m_hex->setFixedHeight(20);
        for (QSpinBox *box : {m_hsv[0], m_hsv[1], m_hsv[2], m_rgb[0], m_rgb[1], m_rgb[2]}) box->setFixedHeight(24);   // Krimble 2026-11-08: 24 px (was 20; the framed fields need it)
        m_hex->setFixedHeight(24);
        auto *valuesBox = new QWidget(this);
        valuesBox->setLayout(values);
        topRow->addWidget(valuesBox, 0, Qt::AlignTop);
        pickerLayout->addLayout(topRow);
        auto *colorRow = new QHBoxLayout;
        colorRow->setSpacing(3);   // Krimble 2026-11-06 (3rd): was 5
        // m_preview=new QLabel(this); m_preview->setFixedSize(32,32);          // the foreground color (32 px, was 38, so the panel can be narrower)
        // m_background=new QLabel(this); m_background->setFixedSize(32,32);    // the background color
        // m_preview=new QLabel(this); m_preview->setFixedSize(18,18);            // the foreground color: a square, 24 px (was 32; George 2026-11-06: smaller and square)
        // m_background=new QLabel(this); m_background->setFixedSize(18,18);      // the background color: a square, 24 px
        m_preview=new QLabel(this); m_preview->setFixedSize(28,28);              // Krimble 2026-11-08: 28 px (the mockup's are large, with a bright outline on the foreground)
        m_background=new QLabel(this); m_background->setFixedSize(28,28);
        auto *swap = new QToolButton(this);
        swap->setText(QStringLiteral("\u21C4"));
        swap->setToolTip(tr("Swap foreground and background colors"));
        colorRow->addWidget(m_preview);
        colorRow->addWidget(swap);
        colorRow->addWidget(m_background);
        auto *recentBox = new QVBoxLayout;
        // recentBox->addWidget(new QLabel(tr("RECENT COLORS"),this));
        recentBox->addWidget(new QLabel(tr("RECENT"),this));   // Krimble 2026-11-06: shorter, so the panel can be narrower (it was "RECENT COLORS")
        m_recentLayout = new QGridLayout;
        // m_recentLayout->setSpacing(3);
        m_recentLayout->setSpacing(0);   // Krimble 2026-11-06 (3rd): no gap between the cells; the dark line is the border of each cell
        recentBox->addLayout(m_recentLayout);
        colorRow->addLayout(recentBox, 1);
        pickerLayout->addLayout(colorRow);
        auto *paletteHeader = new QHBoxLayout;
        paletteHeader->addWidget(new QLabel(tr("MY PALETTE"),this));
        paletteHeader->addStretch(1);
        paletteHeader->addWidget(add);
        pickerLayout->addLayout(paletteHeader);
        // m_swatches=new QGridLayout; m_swatches->setSpacing(3);
        m_swatches=new QGridLayout; m_swatches->setSpacing(0);   // Krimble 2026-11-06 (3rd): no gap between the cells
        pickerLayout->addLayout(m_swatches);
#endif
        m_square = new KrimbleSvSquare(m_ui);
        m_hue = new KrimbleHueStrip(m_ui);
        for (int i=0;i<3;++i) {
            m_hsv[i] = new QSpinBox(m_ui);
            m_hsv[i]->setRange(0, i==0 ? 359 : 100);
            m_rgb[i] = new QSpinBox(m_ui);
            m_rgb[i]->setRange(0, 255);
        }
        m_hex = new QLineEdit(m_ui);
        m_hex->setMaxLength(7);
        m_hex->setPlaceholderText(QStringLiteral("#RRGGBB"));
        m_preview = new QLabel(m_ui);
        m_background = new QLabel(m_ui);
        m_ui->square = m_square;
        m_ui->hue = m_hue;
        for (int i=0;i<3;++i) { m_ui->hsv[i] = m_hsv[i]; m_ui->rgb[i] = m_rgb[i]; }
        m_ui->hex = m_hex;
        m_ui->fg = m_preview;
        m_ui->bg = m_background;
        m_ui->recentCells = &m_recentCells;
        m_ui->paletteCells = &m_swatchCells;
        m_ui->createSteppers();
        auto *swap = m_ui->swap;
        swap->setToolTip(tr("Swap foreground and background colors"));
        m_ui->eyedropper->setToolTip(tr("Pick a color from the image"));
        m_ui->copy->setToolTip(tr("Copy the HEX code"));
        m_ui->mode->setToolTip(tr("Color model of the first three boxes"));
        m_ui->add->setToolTip(tr("Add foreground color to palette"));
        m_ui->trash->setToolTip(tr("Remove the selected swatch"));
        connect(m_ui->copy,&QToolButton::clicked,this,[this]() { QApplication::clipboard()->setText(m_hex->text()); });
        connect(m_ui->eyedropper,&QToolButton::clicked,this,[]() {
            KoToolManager::instance()->switchToolRequested(QStringLiteral("KritaSelected/KisToolColorSampler"));
        });
        connect(m_ui->mode,&QToolButton::clicked,this,[this]() {
            QMenu menu(m_ui->mode);
            menu.addAction(QStringLiteral("HSV"));
            QAction *hslAction = menu.addAction(QStringLiteral("HSL"));
            QAction *chosen = menu.exec(m_ui->mode->mapToGlobal(QPoint(0, m_ui->mode->height())));
            if (!chosen) return;
            m_hslMode = (chosen == hslAction);
            m_ui->modeText = m_hslMode ? QStringLiteral("HSL") : QStringLiteral("HSV");
            m_ui->update();
            select(m_current, false);
        });
        connect(m_ui->trash,&QToolButton::clicked,this,[this]() {
            if (m_selectedSwatch < 0 || m_selectedSwatch >= m_colors.size()) return;
            m_colors.removeAt(m_selectedSwatch);
            m_selectedSwatch = -1;
            savePalette();
            rebuildSwatches();
        });
        connect(swap,&QToolButton::clicked,this,[this]() {
            if (!m_canvas) return;
            auto *resources = m_canvas->resourceManager();
            const KoColor fg = resources->foregroundColor();
            const KoColor bg = resources->backgroundColor();
            resources->setForegroundColor(bg);
            resources->setBackgroundColor(fg);
            select(bg.toQColor(), false);
            showBackground(fg.toQColor());
        });
        m_square->onPick=[this](QColor c){ select(c); };
        connect(m_hue,&QAbstractSlider::valueChanged,this,[this](int h) {
            m_square->setHue(h); select(m_square->selected());
        });
        for (int i=0;i<3;++i)
            connect(m_rgb[i],qOverload<int>(&QSpinBox::valueChanged),this,[this](int) {
                select(QColor(m_rgb[0]->value(),m_rgb[1]->value(),m_rgb[2]->value()));
            });
        for (int i=0;i<3;++i)
            connect(m_hsv[i],qOverload<int>(&QSpinBox::valueChanged),this,[this](int) {
                // Krimble 2026-11-08: the first three boxes are HSV or HSL (the drop-down of the mockup)
                if (m_hslMode)
                    select(QColor::fromHsl(m_hsv[0]->value(),
                                           qRound(m_hsv[1]->value()*255.0/100.0),
                                           qRound(m_hsv[2]->value()*255.0/100.0)));
                else
                select(QColor::fromHsv(m_hsv[0]->value(),
                                      qRound(m_hsv[1]->value()*255.0/100.0),
                                      qRound(m_hsv[2]->value()*255.0/100.0)));
            });
        connect(m_hex,&QLineEdit::editingFinished,this,[this]() {
            QColor c(m_hex->text()); if(c.isValid()) select(c);
        });
        // connect(add,&QPushButton::clicked,this,[this]() {
        connect(m_ui->add,&QToolButton::clicked,this,[this]() {
            if (!m_colors.contains(m_current)) m_colors.append(m_current);
            // while(m_colors.size()>24) m_colors.remove(0);
            while(m_colors.size()>76) m_colors.remove(0);   // Krimble 2026-11-08: the palette grid of the mockup holds 19 x 4 = 76 swatches (was 24)
            savePalette(); rebuildSwatches();
        });
        showBackground(QColor("#1b1c30"));   // Krimble 2026-11-05: the background color box has a color until the canvas gives the real one
        loadPalette();
        select(QColor("#459ad3"),false);
        // Keep the controls in sync when another docker or the eyedropper changes
        // the foreground color. Only poll while the Studio is visible.
        m_syncTimer.setInterval(350);
        connect(&m_syncTimer, &QTimer::timeout, this, [this]() {
            if (!m_canvas || !isVisible()) return;
            // Krimble 2026-10-30: a swatch added from the Mixer panel (it writes the same saved list) shows here too
            {
                const QStringList stored = QSettings().value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
                QStringList mine;
                for (const QColor &c : m_colors) mine << c.name(QColor::HexRgb);
                if (stored != mine) { m_colors.clear(); loadPalette(); }
            }
            const QColor foreground=m_canvas->resourceManager()->foregroundColor().toQColor();
            if (foreground.isValid()) {
                QColor opaque = foreground;
                opaque.setAlpha(255);
                if (opaque != m_current || foreground.alpha() != m_state.alpha) {
                    m_state.alpha = foreground.alpha();   // Krimble 2026-11-08: a foreground color from another panel keeps its alpha here too
                    select(opaque, false);
                }
            }
            showBackground(m_canvas->resourceManager()->backgroundColor().toQColor());   // Krimble 2026-11-05
        });
        m_syncTimer.start();
    }
    void setCanvas(KisCanvas2 *canvas) {
        m_canvas=canvas;
        if (m_canvas) {
            // Read the existing document foreground color on canvas switch.
            KoColor color=m_canvas->resourceManager()->foregroundColor();
            select(color.toQColor(), false);
        }
    }
private:
    void select(QColor c,bool commit=true) {
        if(!c.isValid()) return;
        c.setAlpha(255);   // Krimble 2026-11-08: the color is opaque RGB; its alpha is m_state.alpha (the Sliders and Values pages set it)
        const bool changed = c != m_current;
        m_current=c;
        m_state.color = c;
        if (c.hsvHue() >= 0) m_state.hue = c.hsvHue();
        if (m_mixingPad) m_mixingPad->setColor(c);
        if (commit && changed) {
            m_recent.removeAll(c);
            m_recent.prepend(c);
            // while (m_recent.size()>12) m_recent.removeLast();
            while (m_recent.size()>10) m_recent.removeLast();   // Krimble 2026-11-05: 10 across, one row
            rebuildRecent();
        }
        m_square->setColor(c);
        for (int i=0;i<3;++i) {
            QSignalBlocker b(m_hsv[i]);
            // m_hsv[i]->setValue(i==0 ? qMax(0,c.hsvHue()) :
            //                    qRound((i==1 ? c.hsvSaturation() : c.value())*100.0/255.0));
            if (m_hslMode)
                m_hsv[i]->setValue(i==0 ? qMax(0,c.hslHue()) :
                                   qRound((i==1 ? c.hslSaturation() : c.lightness())*100.0/255.0));
            else
            m_hsv[i]->setValue(i==0 ? qMax(0,c.hsvHue()) :
                               qRound((i==1 ? c.hsvSaturation() : c.value())*100.0/255.0));
        }
        { QSignalBlocker b(m_hue); m_hue->setValue(qMax(0,c.hsvHue())); }
        for(int i=0;i<3;++i) {
            QSignalBlocker b(m_rgb[i]);
            m_rgb[i]->setValue(i==0?c.red():(i==1?c.green():c.blue()));
        }
        { QSignalBlocker b(m_hex); m_hex->setText(c.name(QColor::HexRgb).toUpper()); }
        // m_preview->setStyleSheet(QStringLiteral("background:%1;border:1px solid #8498bd;border-radius:4px;").arg(c.name()));
        // m_preview->setStyleSheet(QStringLiteral("background:%1;border:2px solid #35d8f8;border-radius:5px;").arg(c.name()));
        m_ui->setForeground(c);   // Krimble 2026-11-08: the frame's size follows the panel's scale   // Krimble 2026-11-08: the foreground has the bright cyan outline of a selected item (mockup)
        refreshPages();
        if(commit && m_canvas) {
            QColor withAlpha = c;
            withAlpha.setAlpha(m_state.alpha);
            m_canvas->resourceManager()->setForegroundColor(KoColor(withAlpha,KoColorSpaceRegistry::instance()->rgb8()));
        }
    }
    // Krimble 2026-11-08: the alpha channel of the foreground color (0 to 255): the A slider and the Alpha boxes
    void setAlpha(int a) {
        m_state.alpha = qBound(0, a, 255);
        if (m_canvas) {
            QColor withAlpha = m_current;
            withAlpha.setAlpha(m_state.alpha);
            m_canvas->resourceManager()->setForegroundColor(KoColor(withAlpha,KoColorSpaceRegistry::instance()->rgb8()));
        }
        refreshPages();
    }
    void refreshPages() {
        if (m_sliders) m_sliders->refresh();
        if (m_values) m_values->refresh();
        if (m_ui) m_ui->update();
    }
    void showTab(int i) {
        m_tab = qBound(0, i, 4);
        if (m_tabs) m_tabs->setCurrent(m_tab);
        if (m_ui) { m_ui->setVisible(m_tab <= 2); if (m_tab <= 2) m_ui->setTopPage(m_tab); }
        refreshPages();
        QSettings().setValue(QStringLiteral("Krimble/ColorStudioTab"), m_tab);
    }
    // Krimble 2026-11-05: the background color box next to the foreground one
    void showBackground(const QColor &c) {
        if (m_background && c.isValid())
            // m_background->setStyleSheet(QStringLiteral("background:%1;border:1px solid #8498bd;border-radius:4px;").arg(c.name()));
            // m_background->setStyleSheet(QStringLiteral("background:%1;border:1px solid #e6ecfa;border-radius:5px;").arg(c.name()));
            m_ui->setBackground(c);   // Krimble 2026-11-08   // Krimble 2026-11-08: a thin light outline (mockup)
    }
    void loadPalette() {
        QSettings settings;
        QStringList items=settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
        // Krimble 2026-11-05: George: "make panels that match the mockups"; the mockup's palette is full of colors. The first time Color Studio opens (and
        // only then) "My Palette" is filled with 40 starting swatches: 10 grays from white to black, then three rows of 10 hues (bright, soft, dark).
        // The user's own swatches are never replaced: after this the saved list is the user's, even if they remove every swatch.
        if (items.isEmpty() && !settings.value(QStringLiteral("Krimble/ColorStudioSwatchesSeeded"), false).toBool()) {
            for (int i = 0; i < 10; ++i) items << QColor::fromHsv(0, 0, 255 - i * 255 / 9).name(QColor::HexRgb);
            const int saturation[3] = {255, 120, 255};
            const int brightness[3] = {255, 255, 140};
            for (int row = 0; row < 3; ++row)
                for (int col = 0; col < 10; ++col)
                    items << QColor::fromHsv(col * 36, saturation[row], brightness[row]).name(QColor::HexRgb);
            settings.setValue(QStringLiteral("Krimble/ColorStudioSwatches"), items);
            settings.setValue(QStringLiteral("Krimble/ColorStudioSwatchesSeeded"), true);
        }
        for(const QString &item:items) {
            QColor c(item); if(c.isValid()) m_colors.append(c);
        }
        rebuildSwatches();
    }
    void savePalette() {
        QStringList items;
        for(const QColor &c:m_colors) items<<c.name(QColor::HexRgb);
        QSettings settings; settings.setValue(QStringLiteral("Krimble/ColorStudioSwatches"),items);
    }
#if 0   // Krimble 2026-11-08: the recent colors and the palette cells in box layouts (up to 20 px wide, 14 across); replaced by cells the measured canvas places
    void rebuildRecent() {
        while (QLayoutItem *item=m_recentLayout->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for (int i=0;i<m_recent.size();++i) {
            QColor color=m_recent[i];
            // auto *button=new QPushButton(this);
            auto *button=new KrimbleSquareCell(this);
            // button->setFixedHeight(28);
            // button->setMinimumWidth(6);   // Krimble 2026-11-06: was 10 (narrower panel, no horizontal scroll bar)
            // Krimble 2026-11-06 (2nd): George: "Color is too big overall ... Make the little color wells square and smaller." Square, 20 x 20 (were as wide as the
            // panel allowed, 28 tall).
            // button->setFixedSize(20, 20);
            button->setMinimumWidth(1);   // 1, not 0: a minimum of 0 means "use the button's own minimum", which is wider
            button->setMaximumWidth(20);   // the cells grow with the panel, up to 20 px (was 26)
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            button->setToolTip(color.name());
            // button->setStyleSheet(QStringLiteral(
            //     "background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
            button->setStyleSheet(QStringLiteral(
                "background:%1;border:none;border-right:1px solid #0b0c18;border-bottom:1px solid #0b0c18;border-radius:0;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            // m_recentLayout->addWidget(button,i/6,i%6);
            // m_recentLayout->addWidget(button,0,i);   // Krimble 2026-11-05: one row, like the mockup (the list keeps the last 10)
            // m_recentLayout->addWidget(button,0,i,Qt::AlignLeft | Qt::AlignTop);   // Krimble 2026-11-06 (2nd): square wells sit side by side at the left
            m_recentLayout->addWidget(button,0,i);   // Krimble 2026-11-06 (3rd): cells that touch
        }
    }
    void rebuildSwatches() {
        while(QLayoutItem *item=m_swatches->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new KrimbleSwatchButton(this);
            // button->setFixedHeight(36);
            // button->setFixedHeight(30);   // Krimble 2026-11-05: 10 across and a little shorter, like the mockup
            // button->setMinimumWidth(6);   // Krimble 2026-11-06: was 10 (narrower panel, no horizontal scroll bar)
            // button->setFixedSize(20, 20);   // Krimble 2026-11-06 (2nd): square and smaller (they stretched to the panel's width, 30 tall)
            button->setMinimumWidth(1);   // 1, not 0: a minimum of 0 means "use the button's own minimum", which is wider
            button->setMaximumWidth(20);   // the cells grow with the panel, up to 20 px (was 26)
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            button->setToolTip(color.name());
            // button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
            button->setStyleSheet(QStringLiteral("background:%1;border:none;border-right:1px solid #0b0c18;border-bottom:1px solid #0b0c18;border-radius:0;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            auto editSwatch=[this,button,i](const QPoint &globalPos) {
                QMenu menu(button);
                QAction *replace=menu.addAction(tr("Replace with current color"));
                QAction *remove=menu.addAction(tr("Remove swatch"));
                QAction *moveLeft=menu.addAction(tr("Move left"));
                QAction *moveRight=menu.addAction(tr("Move right"));
                moveLeft->setEnabled(i>0);
                moveRight->setEnabled(i+1<m_colors.size());
                QAction *chosen=menu.exec(globalPos);
                if(chosen==replace) m_colors[i]=m_current;
                else if(chosen==remove) m_colors.removeAt(i);
                else if(chosen==moveLeft && i>0) m_colors.swapItemsAt(i,i-1);
                else if(chosen==moveRight && i+1<m_colors.size()) m_colors.swapItemsAt(i,i+1);
                else return;
                savePalette(); rebuildSwatches();
            };
            button->onLongPress=editSwatch;
            connect(button,&QWidget::customContextMenuRequested,this,[button,editSwatch](const QPoint &pos) {
                editSwatch(button->mapToGlobal(pos));
            });
            // m_swatches->addWidget(button,i/6,i%6);
            // m_swatches->addWidget(button,i/10,i%10);
            // m_swatches->addWidget(button,i/10,i%10,Qt::AlignLeft | Qt::AlignTop);   // Krimble 2026-11-06 (2nd): square wells sit side by side at the left
            // m_swatches->addWidget(button,i/10,i%10);   // Krimble 2026-11-06 (3rd): cells that touch
            m_swatches->addWidget(button,i/14,i%14);   // Krimble 2026-11-06 (4th): George: "smaller color wells": 14 across, so each cell is about 17 px (was 10 across, 23 px)
        }
    }
#endif
    void rebuildRecent() {
        for (QWidget *cell : m_recentCells) { cell->hide(); cell->deleteLater(); }
        m_recentCells.clear();
        for (int i=0;i<m_recent.size();++i) {
            QColor color=m_recent[i];
            auto *button=new QPushButton(m_ui);
            button->setFocusPolicy(Qt::NoFocus);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #05060f;border-radius:5px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            m_recentCells.append(button);
        }
        if (m_ui) m_ui->relayout();
    }
    void rebuildSwatches() {
        for (QWidget *cell : m_swatchCells) { cell->hide(); cell->deleteLater(); }
        m_swatchCells.clear();
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new KrimbleSwatchButton(m_ui);
            button->setToolTip(color.name());
            // the selected swatch has the bright outline of a selected item; it is the one the trash button removes
            button->setStyleSheet(i==m_selectedSwatch
                ? QStringLiteral("background:%1;border:2px solid #35d8f8;border-radius:3px;").arg(color.name())
                : QStringLiteral("background:%1;border:none;border-radius:3px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color,i](){
                m_selectedSwatch=i;
                select(color);
                rebuildSwatches();
            });
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            auto editSwatch=[this,button,i](const QPoint &globalPos) {
                QMenu menu(button);
                QAction *replace=menu.addAction(tr("Replace with current color"));
                QAction *remove=menu.addAction(tr("Remove swatch"));
                QAction *moveLeft=menu.addAction(tr("Move left"));
                QAction *moveRight=menu.addAction(tr("Move right"));
                moveLeft->setEnabled(i>0);
                moveRight->setEnabled(i+1<m_colors.size());
                QAction *chosen=menu.exec(globalPos);
                if(chosen==replace) m_colors[i]=m_current;
                else if(chosen==remove) m_colors.removeAt(i);
                else if(chosen==moveLeft && i>0) m_colors.swapItemsAt(i,i-1);
                else if(chosen==moveRight && i+1<m_colors.size()) m_colors.swapItemsAt(i,i+1);
                else return;
                m_selectedSwatch=-1;
                savePalette(); rebuildSwatches();
            };
            button->onLongPress=editSwatch;
            connect(button,&QWidget::customContextMenuRequested,this,[button,editSwatch](const QPoint &pos) {
                editSwatch(button->mapToGlobal(pos));
            });
            m_swatchCells.append(button);
        }
        if (m_ui) m_ui->relayout();
    }
    QPointer<KisCanvas2> m_canvas;
    QTimer m_syncTimer;
    QColor m_current;
    KrimbleSvSquare *m_square=nullptr;
    KrimbleMixingPad *m_mixingPad=nullptr;
    // QSlider *m_hue=nullptr;
    KrimbleHueStrip *m_hue=nullptr;   // Krimble 2026-11-05: the vertical hue strip
    QLabel *m_background=nullptr;
    QSpinBox *m_rgb[3]{};
    QSpinBox *m_hsv[3]{};
    QLineEdit *m_hex=nullptr;
    QLabel *m_preview=nullptr;
    QGridLayout *m_swatches=nullptr;
    KrimbleColorCanvas *m_ui=nullptr;   // Krimble 2026-11-08: the measured Picker page
    KrimbleColorTabBar *m_tabs=nullptr;
    KrimbleSlidersTop *m_sliders=nullptr;
    KrimbleValuesTop *m_values=nullptr;
    KrimbleColorState m_state;
    int m_tab=0;
    QVector<QWidget *> m_recentCells;
    QVector<QWidget *> m_swatchCells;
    int m_selectedSwatch=-1;
    bool m_hslMode=false;
    QGridLayout *m_recentLayout=nullptr;
    QVector<QColor> m_recent;
    QVector<QColor> m_colors;
};
