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

#include <algorithm>
#include "kis_canvas2.h"
#include "kis_canvas_resource_provider.h"
#include "KoColor.h"
#include "KoColorSpaceRegistry.h"

// Swatch button supports a touch long-press without requiring a mouse right-click.
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
        setMinimumSize(100, 95);
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
            const QRect field(0, 0, width(), height());
            QLinearGradient saturation(0, 0, width()-1, 0);
            saturation.setColorAt(0.0, Qt::white);
            saturation.setColorAt(1.0, QColor::fromHsv(m_hue, 255, 255));
            p.fillRect(field, saturation);
            QLinearGradient value(0, 0, 0, height()-1);
            value.setColorAt(0.0, QColor(0, 0, 0, 0));
            value.setColorAt(1.0, QColor(0, 0, 0, 255));
            p.fillRect(field, value);
        }
        p.setRenderHint(QPainter::Antialiasing);
        QPointF pt(m_sat*w/255.0, (255-m_val)*h/255.0);
        p.setPen(QPen(Qt::black, 3)); p.setBrush(Qt::NoBrush);
        p.drawEllipse(pt, 7, 7);
        p.setPen(QPen(Qt::white, 2)); p.drawEllipse(pt, 6, 6);
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
        setMinimumHeight(220);
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
    void ensureSize() {
        if (m_paint.size()==size()) return;
        QImage fitted(size(),QImage::Format_ARGB32_Premultiplied);
        fitted.fill(Qt::transparent);
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
        if (m_paint.size()!=event->size()) {
            QImage newImage(event->size(),QImage::Format_ARGB32_Premultiplied);
            newImage.fill(Qt::transparent);
            if (!m_paint.isNull()) {
                QPainter painter(&newImage);
                painter.drawImage(0,0,m_paint);
            }
            m_paint=newImage;
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
        if (!m_paint.rect().contains(at)) return QColor();
        const QColor c=m_paint.pixelColor(at);
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
        setFixedWidth(24);
        setMinimumHeight(95);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF strip = QRectF(rect()).adjusted(3, 1, -3, -1);
        QLinearGradient hues(0, strip.top(), 0, strip.bottom());
        const int stops[7] = {0, 60, 120, 180, 240, 300, 359};
        for (int i = 0; i < 7; ++i) hues.setColorAt(i / 6.0, QColor::fromHsv(stops[i], 255, 255));
        p.setBrush(hues);
        p.setPen(QPen(QColor("#8498bd"), 1));
        p.drawRoundedRect(strip, 5, 5);
        const qreal y = strip.top() + strip.height() * value() / 359.0;
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::black, 3));
        p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
        p.setPen(QPen(Qt::white, 2));
        p.drawRoundedRect(QRectF(1, y - 4, width() - 2, 8), 4, 4);
    }
    void mousePressEvent(QMouseEvent *e) override { setFromY(e->pos().y()); }
    void mouseMoveEvent(QMouseEvent *e) override { if (e->buttons() & Qt::LeftButton) setFromY(e->pos().y()); }
private:
    void setFromY(int y) { setValue(qBound(0, int(359.0 * (y - 1) / qMax(1, height() - 2)), 359)); }
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
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(5,5,5,5); layout->setSpacing(5);
        // auto *header = new QHBoxLayout;
        // auto *title = new QLabel(tr("COLOR STUDIO"), this);
        // QFont f=title->font(); f.setBold(true); title->setFont(f);
        // header->addWidget(title); header->addStretch();
        // auto *add = new QPushButton(tr("+ Swatch"), this);
        // add->setToolTip(tr("Add foreground color to palette"));
        // header->addWidget(add);
        // layout->addLayout(header);
        // Krimble 2026-11-05: George's ColorStudio logo graphic, 70% of the panel width, replaces the text heading ("+ Swatch" is in the palette heading below).
        {
            auto *logoRow = new QHBoxLayout;
            logoRow->addStretch(1);
            logoRow->addWidget(new KrimbleStudioLogoLabel(QStringLiteral(":/dark_krimble-colorstudio-logo.png"), this), 7);
            logoRow->addStretch(1);
            layout->addLayout(logoRow);
        }
        auto *add = new QPushButton(tr("+ Swatch"), this);
        add->setToolTip(tr("Add foreground color to palette"));
        // Krimble 2026-10-30: George: "Color docker designed by ChatGPT was too ambitious and doesn't need tabs. Should be all in one design." and, of the mixing
        // pad: "It should be a panel." One page: the picker, values, recent colors and palette. The mixing pad is its own panel, "Mixer"
        // (krimble_mixer_dock.h); the code that built the Mixer tab is kept below inside #if 0.
        // auto *tabs = new QTabWidget(this);
        // auto *picker = new QWidget(tabs);
        // auto *mixer = new QWidget(tabs);
        auto *picker = new QWidget(this);
        auto *pickerLayout = new QVBoxLayout(picker);
        pickerLayout->setContentsMargins(0,4,0,0);
        pickerLayout->setSpacing(5);
        // tabs->addTab(picker,tr("Picker"));
        // tabs->addTab(mixer,tr("Mixer"));
        // layout->addWidget(tabs);
        layout->addWidget(picker, 1);
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
        m_square = new KrimbleSvSquare(this);
        m_hue = new KrimbleHueStrip(this);
        auto *topRow = new QHBoxLayout;
        topRow->setSpacing(6);
        topRow->addWidget(m_square, 5);
        topRow->addWidget(m_hue);
        auto *values = new QGridLayout;
        values->setSpacing(4);
        const char *hsvNames[] = {"H", "S", "V"};
        const char *names[]={"R","G","B"};
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(hsvNames[i]),this),i,0);
            m_hsv[i] = new QSpinBox(this);
            m_hsv[i]->setRange(0, i==0 ? 359 : 100);
            m_hsv[i]->setSuffix(i==0 ? QStringLiteral("°") : QStringLiteral("%"));
            m_hsv[i]->setFixedWidth(76);
            values->addWidget(m_hsv[i],i,1);
        }
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(names[i]),this),i+3,0);
            m_rgb[i]=new QSpinBox(this); m_rgb[i]->setRange(0,255);
            m_rgb[i]->setFixedWidth(76);
            values->addWidget(m_rgb[i],i+3,1);
        }
        values->addWidget(new QLabel(tr("HEX"),this),6,0);
        m_hex=new QLineEdit(this); m_hex->setMaxLength(7);
        m_hex->setPlaceholderText(QStringLiteral("#RRGGBB"));
        m_hex->setFixedWidth(76);
        values->addWidget(m_hex,6,1);
        auto *valuesBox = new QWidget(this);
        valuesBox->setLayout(values);
        topRow->addWidget(valuesBox, 0, Qt::AlignTop);
        pickerLayout->addLayout(topRow);
        auto *colorRow = new QHBoxLayout;
        colorRow->setSpacing(5);
        m_preview=new QLabel(this); m_preview->setFixedSize(38,38);          // the foreground color
        m_background=new QLabel(this); m_background->setFixedSize(38,38);    // the background color
        auto *swap = new QToolButton(this);
        swap->setText(QStringLiteral("\u21C4"));
        swap->setToolTip(tr("Swap foreground and background colors"));
        colorRow->addWidget(m_preview);
        colorRow->addWidget(swap);
        colorRow->addWidget(m_background);
        auto *recentBox = new QVBoxLayout;
        recentBox->addWidget(new QLabel(tr("RECENT COLORS"),this));
        m_recentLayout = new QGridLayout;
        m_recentLayout->setSpacing(3);
        recentBox->addLayout(m_recentLayout);
        colorRow->addLayout(recentBox, 1);
        pickerLayout->addLayout(colorRow);
        auto *paletteHeader = new QHBoxLayout;
        paletteHeader->addWidget(new QLabel(tr("MY PALETTE"),this));
        paletteHeader->addStretch(1);
        paletteHeader->addWidget(add);
        pickerLayout->addLayout(paletteHeader);
        m_swatches=new QGridLayout; m_swatches->setSpacing(3);
        pickerLayout->addLayout(m_swatches);
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
                select(QColor::fromHsv(m_hsv[0]->value(),
                                      qRound(m_hsv[1]->value()*255.0/100.0),
                                      qRound(m_hsv[2]->value()*255.0/100.0)));
            });
        connect(m_hex,&QLineEdit::editingFinished,this,[this]() {
            QColor c(m_hex->text()); if(c.isValid()) select(c);
        });
        connect(add,&QPushButton::clicked,this,[this]() {
            if (!m_colors.contains(m_current)) m_colors.append(m_current);
            while(m_colors.size()>24) m_colors.remove(0);
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
            if (foreground.isValid() && foreground != m_current)
                select(foreground, false);
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
        const bool changed = c != m_current;
        m_current=c;
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
            m_hsv[i]->setValue(i==0 ? qMax(0,c.hsvHue()) :
                               qRound((i==1 ? c.hsvSaturation() : c.value())*100.0/255.0));
        }
        { QSignalBlocker b(m_hue); m_hue->setValue(qMax(0,c.hsvHue())); }
        for(int i=0;i<3;++i) {
            QSignalBlocker b(m_rgb[i]);
            m_rgb[i]->setValue(i==0?c.red():(i==1?c.green():c.blue()));
        }
        { QSignalBlocker b(m_hex); m_hex->setText(c.name(QColor::HexRgb).toUpper()); }
        m_preview->setStyleSheet(QStringLiteral("background:%1;border:1px solid #8498bd;border-radius:4px;").arg(c.name()));
        if(commit && m_canvas)
            m_canvas->resourceManager()->setForegroundColor(KoColor(c,KoColorSpaceRegistry::instance()->rgb8()));
    }
    // Krimble 2026-11-05: the background color box next to the foreground one
    void showBackground(const QColor &c) {
        if (m_background && c.isValid())
            m_background->setStyleSheet(QStringLiteral("background:%1;border:1px solid #8498bd;border-radius:4px;").arg(c.name()));
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
    void rebuildRecent() {
        while (QLayoutItem *item=m_recentLayout->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for (int i=0;i<m_recent.size();++i) {
            QColor color=m_recent[i];
            auto *button=new QPushButton(this);
            button->setFixedHeight(28);
            button->setMinimumWidth(10);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral(
                "background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            // m_recentLayout->addWidget(button,i/6,i%6);
            m_recentLayout->addWidget(button,0,i);   // Krimble 2026-11-05: one row, like the mockup (the list keeps the last 10)
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
            button->setFixedHeight(30);   // Krimble 2026-11-05: 10 across and a little shorter, like the mockup
            button->setMinimumWidth(10);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
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
            m_swatches->addWidget(button,i/10,i%10);
        }
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
    QGridLayout *m_recentLayout=nullptr;
    QVector<QColor> m_recent;
    QVector<QColor> m_colors;
};
