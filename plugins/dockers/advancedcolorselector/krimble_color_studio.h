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
#include <QAbstractSlider>
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
#include <QScrollArea>
#include <QStack>
#include <QComboBox>
#include <QTabletEvent>
#include <QResizeEvent>
#include <QPainterPath>
#include <QImage>
#include <QClipboard>
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <KoToolManager.h>
#include "krimble_color_canvas.h"
#include "krimble_color_palettes.h"
#include "krimble_color_palette_page.h"
#include "krimble_color_history.h"

#include <algorithm>
#include "kis_canvas2.h"
#include "kis_canvas_resource_provider.h"
#include "KoColor.h"
#include "KoColorSpaceRegistry.h"

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

class KrimbleSvSquare : public QWidget
{
public:
    explicit KrimbleSvSquare(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(60, 57);
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
        {
            QPainterPath roundedField;
            roundedField.addRect(QRectF(0.5, 0.5, width() - 1, height() - 1));
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

class KrimbleMixingPad : public QWidget
{
public:
    explicit KrimbleMixingPad(QWidget *parent=nullptr) : QWidget(parent) {
        setMinimumHeight(140);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setAttribute(Qt::WA_AcceptTouchEvents, false);
        setAutoFillBackground(false);
    }
    void setColor(const QColor &color) { if (color.isValid()) m_color=color; }
    void setBrushSize(int value) { m_size=qBound(1,value,100); }
    void setOpacity(int value) { m_opacity=qBound(1,value,100); }
    void setEraser(bool enabled) { m_eraser=enabled; m_sampling=false; }
    void setSampling(bool enabled) { m_sampling=enabled; if (enabled) m_eraser=false; }
    std::function<void(QColor)> onSample;
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
#if 0
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
            const QPointF delta=QPointF(to-from);
            const qreal length=std::hypot(delta.x(),delta.y());
            const qreal radius=qMax(1.0,m_size/2.0);
            const qreal spacing=qMax(1.0,m_size*0.12);
            const int steps=qMax(1,int(length/spacing));
            QColor inner=c;
            inner.setAlphaF(qBound(0.02,(m_opacity/100.0)*0.28,1.0));
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

class KrimbleHueStrip : public QAbstractSlider
{
public:
    explicit KrimbleHueStrip(QWidget *parent = nullptr) : QAbstractSlider(parent)
    {
        setOrientation(Qt::Vertical);
        setRange(0, 359);
        setMinimumHeight(60);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const qreal margin = width() * 4.0 / 90.0;
        const QRectF strip = QRectF(rect()).adjusted(margin, margin, -margin, -margin);
        QLinearGradient hues(0, strip.top(), 0, strip.bottom());
        const int stops[7] = {0, 60, 120, 180, 240, 300, 359};
        for (int i = 0; i < 7; ++i) hues.setColorAt(i / 6.0, QColor::fromHsv(stops[i], 255, 255));
        p.setBrush(hues);
        p.setPen(QPen(QColor(8, 10, 28, 230), 1.4));
        p.drawRoundedRect(strip, strip.width() / 2, strip.width() / 2);
        const qreal y = strip.top() + strip.height() * value() / 359.0;
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
    void setFromY(int y) {
        const qreal margin = width() * 4.0 / 90.0;
        setValue(qBound(0, int(359.0 * (y - margin) / qMax<qreal>(1.0, height() - 2 * margin)), 359));
    }
};

class KrimbleStudioLogoLabel : public QWidget
{
public:
    explicit KrimbleStudioLogoLabel(const QString &resource, QWidget *parent = nullptr) : QWidget(parent), m_pix(resource)
    {
        QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy.setHeightForWidth(true);
        setSizePolicy(policy);
        setAccessibleName(QStringLiteral("ColorStudio"));
        setMaximumWidth(340);
    }
    bool hasHeightForWidth() const override { return !m_pix.isNull(); }
    int heightForWidth(int w) const override { return m_pix.isNull() ? 0 : int(double(w) * m_pix.height() / m_pix.width()); }
    QSize sizeHint() const override { return QSize(200, heightForWidth(200)); }
protected:
    void resizeEvent(QResizeEvent *) override
    {
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
        setObjectName(QStringLiteral("KrimbleColorStudio"));
        {
            QFont smaller = font();
            if (smaller.pixelSize() > 0) smaller.setPixelSize(qMax(8, int(smaller.pixelSize() * 0.75)));
            else smaller.setPointSizeF(qMax(6.0, smaller.pointSizeF() * 0.75));
            setFont(smaller);
        }
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(3,3,3,3); layout->setSpacing(2);
#if 0
        {
            auto *logoRow = new QHBoxLayout;
            KrimbleStudioLogoLabel *krimbleLogo = new KrimbleStudioLogoLabel(QStringLiteral(":/dark_krimble-colorstudio-logo.png"), this);
            krimbleLogo->setObjectName(QStringLiteral("KrimbleStudioLogo"));
            logoRow->addWidget(krimbleLogo, 4);
            logoRow->addStretch(6);
            layout->addLayout(logoRow);
        }
#endif
#if 0
        auto *add = new QPushButton(tr("+ Swatch"), this);
        add->setToolTip(tr("Add foreground color to palette"));
#endif
#if 0
        auto *picker = new QWidget(this);
        picker->setObjectName(QStringLiteral("krimbleStudioCard"));
        picker->setAttribute(Qt::WA_StyledBackground, true);
        auto *pickerLayout = new QVBoxLayout(picker);
        pickerLayout->setContentsMargins(6,8,6,6);
        pickerLayout->setSpacing(2);
        layout->addWidget(picker, 1);
#endif
        auto *header = new KrimbleColorHeader(this);
        layout->addWidget(header);
        m_band = header;
        m_tabs = new KrimbleColorTabBar(this);
        layout->addWidget(m_tabs);
        m_ui = new KrimbleColorCanvas(this);
        layout->addWidget(m_ui);
        m_palettePage = new KrimblePalettePage(this);
        m_historyPage = new KrimbleHistoryPage(this);
        layout->addWidget(m_palettePage);
        layout->addWidget(m_historyPage);
        m_palettePage->hide();
        m_historyPage->hide();
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
        m_palettePage->state = &m_state;
        m_palettePage->recent = &m_recent;
        m_palettePage->onColor = [this](const QColor &c) { select(c); };
        m_palettePage->onPaletteChanged = [this]() { m_colors.clear(); loadPalette(); };
        m_palettePage->onPickFromImage = []() { KoToolManager::instance()->switchToolRequested(QStringLiteral("KritaSelected/KisToolColorSampler")); };
        m_historyPage->state = &m_state;
        m_historyPage->onColor = [this](const QColor &c) { select(c); };
        m_historyPage->onPaletteChanged = [this]() { m_colors.clear(); loadPalette(); if (m_palettePage) m_palettePage->refresh(); };
        m_recentTimer.setSingleShot(true);
        m_recentTimer.setInterval(700);
        connect(&m_recentTimer, &QTimer::timeout, this, [this]() {
            const QColor c = m_current;
            if (!c.isValid()) return;
            if (m_canvas) {
                const KoColor edited = m_editBackground ? m_canvas->resourceManager()->backgroundColor() : m_canvas->resourceManager()->foregroundColor();
                if (edited.toQColor().rgb() != c.rgb()) return;
            }
            if (m_recent.contains(c)) return;
            m_recent.prepend(c);
            while (m_recent.size()>10) m_recent.removeLast();
            rebuildRecent();
        });
        m_historyTimer.setSingleShot(true);
        m_historyTimer.setInterval(900);
        connect(&m_historyTimer, &QTimer::timeout, this, [this]() {
            KrimbleHistoryStore::add(m_state.color, m_state.alpha, krimbleToolName(KoToolManager::instance()->activeToolId()));
            if (m_tab == 4 && m_historyPage) m_historyPage->refresh();
        });
#if 0
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
#endif
#if 0
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
            "QSlider::handle:horizontal { width:20px; height:20px; margin:-5px 0; border:none; background:transparent;"
            " border-image: url(:/krimble-ball-bearing.png) 0 0 0 0 stretch stretch; }"));
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
#endif
#if 0
        m_square = new KrimbleSvSquare(this);
        m_hue = new KrimbleHueStrip(this);
        auto *topRow = new QHBoxLayout;
        topRow->setSpacing(3);
        topRow->addWidget(m_square, 5);
        topRow->addWidget(m_hue);
        auto *values = new QGridLayout;
        values->setSpacing(1);
        const char *hsvNames[] = {"H", "S", "V"};
        const char *names[]={"R","G","B"};
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(hsvNames[i]),this),i,0);
            m_hsv[i] = new QSpinBox(this);
            m_hsv[i]->setRange(0, i==0 ? 359 : 100);
            m_hsv[i]->setSuffix(i==0 ? QStringLiteral("°") : QStringLiteral("%"));
            m_hsv[i]->setFixedWidth(66);
            values->addWidget(m_hsv[i],i,1);
        }
        for (int i=0;i<3;++i) {
            values->addWidget(new QLabel(QString::fromLatin1(names[i]),this),i+3,0);
            m_rgb[i]=new QSpinBox(this); m_rgb[i]->setRange(0,255);
            m_rgb[i]->setFixedWidth(66);
            values->addWidget(m_rgb[i],i+3,1);
        }
        values->addWidget(new QLabel(tr("HEX"),this),6,0);
        m_hex=new QLineEdit(this); m_hex->setMaxLength(7);
        m_hex->setPlaceholderText(QStringLiteral("#RRGGBB"));
        m_hex->setFixedWidth(78);
        values->addWidget(m_hex,6,1);
        for (QSpinBox *box : {m_hsv[0], m_hsv[1], m_hsv[2], m_rgb[0], m_rgb[1], m_rgb[2]}) box->setFixedHeight(24);
        m_hex->setFixedHeight(24);
        auto *valuesBox = new QWidget(this);
        valuesBox->setLayout(values);
        topRow->addWidget(valuesBox, 0, Qt::AlignTop);
        pickerLayout->addLayout(topRow);
        auto *colorRow = new QHBoxLayout;
        colorRow->setSpacing(3);
        m_preview=new QLabel(this); m_preview->setFixedSize(28,28);
        m_background=new QLabel(this); m_background->setFixedSize(28,28);
        auto *swap = new QToolButton(this);
        swap->setText(QStringLiteral("\u21C4"));
        swap->setToolTip(tr("Swap foreground and background colors"));
        colorRow->addWidget(m_preview);
        colorRow->addWidget(swap);
        colorRow->addWidget(m_background);
        auto *recentBox = new QVBoxLayout;
        recentBox->addWidget(new QLabel(tr("RECENT"),this));
        m_recentLayout = new QGridLayout;
        m_recentLayout->setSpacing(0);
        recentBox->addLayout(m_recentLayout);
        colorRow->addLayout(recentBox, 1);
        pickerLayout->addLayout(colorRow);
        auto *paletteHeader = new QHBoxLayout;
        paletteHeader->addWidget(new QLabel(tr("MY PALETTE"),this));
        paletteHeader->addStretch(1);
        paletteHeader->addWidget(add);
        pickerLayout->addLayout(paletteHeader);
        m_swatches=new QGridLayout; m_swatches->setSpacing(0);
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
        connect(m_ui->paletteMenu,&QToolButton::clicked,this,[this]() {
            QMenu menu(m_ui->paletteMenu);
            for (const QString &name : KrimblePaletteStore::names()) {
                QAction *a = menu.addAction(name);
                a->setCheckable(true);
                a->setChecked(name == m_paletteName);
            }
            QAction *chosen = menu.exec(m_ui->paletteMenu->mapToGlobal(QPoint(0, m_ui->paletteMenu->height())));
            if (chosen) { KrimblePaletteStore::setActive(chosen->text()); m_colors.clear(); loadPalette(); }
        });
        connect(m_ui->pencil,&QToolButton::clicked,this,[this]() {
            if (KrimblePaletteActions::renamePalette(this)) { m_colors.clear(); loadPalette(); }
        });
        connect(m_ui->duplicate,&QToolButton::clicked,this,[this]() {
            if (KrimblePaletteActions::duplicatePalette(this)) { m_colors.clear(); loadPalette(); }
        });
        connect(m_ui->more,&QToolButton::clicked,this,[this]() {
            QMenu menu(m_ui->more);
            QAction *newAction = menu.addAction(tr("New Palette..."));
            QAction *openAction = menu.addAction(tr("Open Palette..."));
            QAction *importAction = menu.addAction(tr("Import..."));
            QAction *exportAction = menu.addAction(tr("Export..."));
            menu.addSeparator();
            QAction *deleteAction = menu.addAction(tr("Delete Palette..."));
            QAction *chosen = menu.exec(m_ui->more->mapToGlobal(QPoint(0, m_ui->more->height())));
            if (!chosen) return;
            bool changed = false;
            if (chosen == newAction) changed = KrimblePaletteActions::newPalette(this);
            else if (chosen == openAction) changed = KrimblePaletteActions::openPalette(this);
            else if (chosen == importAction) changed = KrimblePaletteActions::importPalette(this);
            else if (chosen == exportAction) KrimblePaletteActions::exportPalette(this);
            else if (chosen == deleteAction) changed = KrimblePaletteActions::deletePalette(this);
            if (changed) { m_colors.clear(); loadPalette(); }
        });
        m_ui->paletteMenu->setToolTip(tr("Choose a palette"));
        m_ui->pencil->setToolTip(tr("Rename the palette"));
        m_ui->duplicate->setToolTip(tr("Duplicate the palette"));
        m_ui->more->setToolTip(tr("New, Open, Import, Export, Delete"));
        connect(m_ui->trash,&QToolButton::clicked,this,[this]() {
            if (m_selectedSwatch < 0 || m_selectedSwatch >= m_colors.size()) return;
            { KrimblePalette pal = currentPalette(); pal.removeAt(m_selectedSwatch); takePalette(pal); }
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
                if (m_hslMode)
                    select(QColor::fromHsl(m_hsv[0]->value(),
                                           qRound(m_hsv[1]->value()*255.0/100.0),
                                           qRound(m_hsv[2]->value()*255.0/100.0)));
                else
                select(QColor::fromHsv(m_hsv[0]->value(),
                                      qRound(m_hsv[1]->value()*255.0/100.0),
                                      qRound(m_hsv[2]->value()*255.0/100.0)));
            });
        {
            auto applyHex=[this](bool finished) {
                QString t=m_hex->text().trimmed();
                if(t.startsWith(QLatin1Char('#'))) t.remove(0,1);
                if(t.size()==3 && finished) t=QString(t[0])+t[0]+t[1]+t[1]+t[2]+t[2];
                if(t.size()!=6) return;
                bool ok=false;
                t.toUInt(&ok,16);
                if(!ok) return;
                const QColor c(QStringLiteral("#")+t);
                if(c.isValid()) select(c);
            };
            connect(m_hex,&QLineEdit::textEdited,this,[applyHex](const QString &) { applyHex(false); });
            connect(m_hex,&QLineEdit::editingFinished,this,[applyHex]() { applyHex(true); });
        }
        connect(m_ui->add,&QToolButton::clicked,this,[this]() {
            {
                KrimblePalette pal = currentPalette();
                const QColor shown = pal.isCmyk() ? krimbleColorOfCmyk(krimbleCmykOf(m_current)) : m_current;
                if (!pal.colors.contains(shown)) { pal.append(m_current); takePalette(pal); }
            }
            savePalette(); rebuildSwatches();
        });
        showBackground(QColor("#1b1c30"));
        loadPalette();
        select(QColor("#459ad3"),false);
        m_syncTimer.setInterval(350);
        connect(&m_syncTimer, &QTimer::timeout, this, [this]() {
            if (!m_canvas || !isVisible()) return;
            {
                if (KrimblePaletteStore::signature() != m_paletteSignature) { m_colors.clear(); loadPalette(); }
            }
            const QColor foreground=(m_editBackground ? m_canvas->resourceManager()->backgroundColor() : m_canvas->resourceManager()->foregroundColor()).toQColor();
            if (foreground.isValid()) {
                QColor opaque = foreground;
                opaque.setAlpha(255);
                if (opaque != m_current || foreground.alpha() != m_state.alpha) {
                    m_state.alpha = foreground.alpha();
                    select(opaque, false);
                }
            }
            showBackground((m_editBackground ? m_canvas->resourceManager()->foregroundColor() : m_canvas->resourceManager()->backgroundColor()).toQColor());
        });
        m_syncTimer.start();
        m_ready = true;
        showTab(qBound(0, QSettings().value(QStringLiteral("Krimble/ColorStudioTab"), 0).toInt(), 4));
    }
    bool event(QEvent *e) override {
        if (e->type() == QEvent::DynamicPropertyChange) {
            auto *pe = static_cast<QDynamicPropertyChangeEvent *>(e);
            if (pe->propertyName() == "krimbleEditTarget") {
                editWell((property("krimbleEditTarget").toInt() & 1) != 0);
            }
        }
        return QWidget::event(e);
    }
    void editWell(bool background) {
        m_editBackground = background;
        QWidget *w = this;
        while (w && !qobject_cast<QDockWidget *>(w)) w = w->parentWidget();
        if (auto *dock = qobject_cast<QDockWidget *>(w)) { dock->show(); dock->raise(); }
        if (m_canvas) {
            const QColor edited = (m_editBackground ? m_canvas->resourceManager()->backgroundColor() : m_canvas->resourceManager()->foregroundColor()).toQColor();
            if (edited.isValid()) {
                QColor opaque = edited;
                opaque.setAlpha(255);
                m_state.alpha = edited.alpha();
                select(opaque, false);
            }
            showBackground((m_editBackground ? m_canvas->resourceManager()->foregroundColor() : m_canvas->resourceManager()->backgroundColor()).toQColor());
        }
    }
    void setCanvas(KisCanvas2 *canvas) {
        m_canvas=canvas;
        if (m_canvas) {
            KoColor color=m_editBackground ? m_canvas->resourceManager()->backgroundColor() : m_canvas->resourceManager()->foregroundColor();
            select(color.toQColor(), false);
        }
    }
    int krimbleNeededViewHeight(int extraRows) const {
        if (!m_ui) return -1;
        const qreal s = m_ui->scale();
        if (s <= 0) return -1;
        const int bottomMargin = layout() ? layout()->contentsMargins().bottom() : 0;
        return int(std::ceil(m_ui->y() + bottomMargin + (KrimbleColorCanvas::DH + extraRows * KrimbleColorCanvas::PaletteRowH) * s)) + 1;
    }
    QWidget *krimbleRoomView() const { return m_paletteRoomView; }
protected:
    void showEvent(QShowEvent *event) override {
        QWidget::showEvent(event);
        if (!m_paletteRoomWatched) {
            for (QWidget *w = parentWidget(); w; w = w->parentWidget()) {
                if (QScrollArea *scroll = qobject_cast<QScrollArea *>(w)) {
                    scroll->viewport()->installEventFilter(this);
                    m_paletteRoomView = scroll->viewport();
                    m_paletteRoomWatched = true;
                    break;
                }
            }
        }
        QTimer::singleShot(0, this, [this]() { fitPaletteRows(); });
    }
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (watched == m_paletteRoomView && event->type() == QEvent::Resize) {
            QTimer::singleShot(0, this, [this]() { fitPaletteRows(); });
        }
        return QWidget::eventFilter(watched, event);
    }
private:
    QPointer<QWidget> m_paletteRoomView;
    bool m_paletteRoomWatched = false;
    void fitPaletteRows() {
        if (!m_ui || !m_paletteRoomView || !m_ui->isVisible()) return;
        const qreal s = m_ui->scale();
        if (s <= 0) return;
        const int bottomMargin = layout() ? layout()->contentsMargins().bottom() : 0;
        const double room = m_paletteRoomView->height() - m_ui->y() - bottomMargin - KrimbleColorCanvas::DH * s;
        const int rows = room > 0 ? int(room / (KrimbleColorCanvas::PaletteRowH * s)) : 0;
        m_ui->setExtraPaletteRows(rows);
    }
    void select(QColor c,bool commit=true) {
        if(!c.isValid()) return;
        c.setAlpha(255);
        const bool changed = c != m_current;
        m_current=c;
        m_state.color = c;
        if (c.hsvHue() >= 0) m_state.hue = c.hsvHue();
        if (m_mixingPad) m_mixingPad->setColor(c);
        if (commit && changed) m_recentTimer.start();
        m_square->setColor(c);
        for (int i=0;i<3;++i) {
            QSignalBlocker b(m_hsv[i]);
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
        m_ui->setForeground(c);
        refreshPages();
        if (changed && m_ready) m_historyTimer.start();
        if(commit && m_canvas) {
            QColor withAlpha = c;
            withAlpha.setAlpha(m_state.alpha);
            writeEditedColor(KoColor(withAlpha,KoColorSpaceRegistry::instance()->rgb8()));
        }
    }
    void writeEditedColor(const KoColor &c) {
        if (!m_canvas) return;
        if (m_editBackground) m_canvas->resourceManager()->setBackgroundColor(c);
        else m_canvas->resourceManager()->setForegroundColor(c);
    }
    void setAlpha(int a) {
        m_state.alpha = qBound(0, a, 255);
        if (m_canvas) {
            QColor withAlpha = m_current;
            withAlpha.setAlpha(m_state.alpha);
            writeEditedColor(KoColor(withAlpha,KoColorSpaceRegistry::instance()->rgb8()));
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
        if (m_palettePage) { m_palettePage->setVisible(m_tab == 3); if (m_tab == 3) m_palettePage->refresh(); }
        if (m_historyPage) { m_historyPage->setVisible(m_tab == 4); if (m_tab == 4) m_historyPage->refresh(); }
        refreshPages();
        QSettings().setValue(QStringLiteral("Krimble/ColorStudioTab"), m_tab);
    }
    void showBackground(const QColor &c) {
        if (m_background && c.isValid())
            m_ui->setBackground(c);
    }
#if 0
    void loadPalette() {
        QSettings settings;
        QStringList items=settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
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
#endif
    void loadPalette() {
        KrimblePaletteStore::ensureDefaults();
        KrimblePalette pal;
        m_colors.clear();
        if (KrimblePaletteStore::load(KrimblePaletteStore::active(), pal)) {
            m_colors = pal.colors;
            m_cmyk = pal.cmyk;
            m_paletteModel = pal.model;
            m_paletteName = pal.name;
            m_paletteColumns = pal.columns;
        }
        if (m_ui) m_ui->paletteName = m_paletteName;
        m_paletteSignature = KrimblePaletteStore::signature();
        if (m_selectedSwatch >= m_colors.size()) m_selectedSwatch = -1;
        rebuildSwatches();
        if (m_palettePage && m_palettePage->isVisible()) m_palettePage->refresh();
    }
    void savePalette() {
        KrimblePalette pal;
        pal.name = m_paletteName;
        pal.columns = m_paletteColumns;
        pal.model = m_paletteModel;
        pal.colors = m_colors;
        pal.cmyk = m_cmyk;
        KrimblePaletteStore::save(pal);
        m_paletteSignature = KrimblePaletteStore::signature();
    }
#if 0
    void rebuildRecent() {
        while (QLayoutItem *item=m_recentLayout->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for (int i=0;i<m_recent.size();++i) {
            QColor color=m_recent[i];
            auto *button=new KrimbleSquareCell(this);
            button->setMinimumWidth(1);
            button->setMaximumWidth(20);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral(
                "background:%1;border:none;border-right:1px solid #000000;border-bottom:1px solid #000000;border-radius:0;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            m_recentLayout->addWidget(button,0,i);
        }
    }
    void rebuildSwatches() {
        while(QLayoutItem *item=m_swatches->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new KrimbleSwatchButton(this);
            button->setMinimumWidth(1);
            button->setMaximumWidth(20);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral("background:%1;border:none;border-right:1px solid #000000;border-bottom:1px solid #000000;border-radius:0;").arg(color.name()));
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
            m_swatches->addWidget(button,i/14,i%14);
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
            button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #05060f;border-radius:0px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            m_recentCells.append(button);
        }
        if (m_ui) m_ui->relayout();
    }
    static QString swatchSheet(const QColor &color, bool selected) {
        return selected
            ? QStringLiteral("background:%1;border:2px solid #0b87dd;border-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9bd6ff, stop:0.5 #0b87dd, stop:1 #075a96);border-radius:0px;").arg(color.name())
            : QStringLiteral("background:%1;border:none;border-right:1px solid #000000;border-bottom:1px solid #000000;border-radius:0px;").arg(color.name());
    }
    void rebuildSwatches() {
        for (QWidget *cell : m_swatchCells) { cell->hide(); cell->deleteLater(); }
        m_swatchCells.clear();
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new KrimbleSwatchButton(m_ui);
            button->setToolTip(color.name());
            button->setStyleSheet(swatchSheet(color, i==m_selectedSwatch));
            connect(button,&QPushButton::clicked,this,[this,color,i](){
                const int old=m_selectedSwatch;
                m_selectedSwatch=i;
                select(color);
                if (old>=0 && old<m_swatchCells.size() && old<m_colors.size()) krimbleSetSheet(m_swatchCells[old], swatchSheet(m_colors[old], false));
                if (i<m_swatchCells.size()) krimbleSetSheet(m_swatchCells[i], swatchSheet(color, true));
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
                KrimblePalette pal = currentPalette();
                if(chosen==replace) pal.replace(i,m_current);
                else if(chosen==remove) pal.removeAt(i);
                else if(chosen==moveLeft && i>0) pal.swap(i,i-1);
                else if(chosen==moveRight && i+1<m_colors.size()) pal.swap(i,i+1);
                else return;
                takePalette(pal);
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
    KrimbleHueStrip *m_hue=nullptr;
    QLabel *m_background=nullptr;
    QSpinBox *m_rgb[3]{};
    QSpinBox *m_hsv[3]{};
    QLineEdit *m_hex=nullptr;
    QLabel *m_preview=nullptr;
    QGridLayout *m_swatches=nullptr;
    KrimbleColorCanvas *m_ui=nullptr;
    KrimbleColorTabBar *m_tabs=nullptr;
public:
    QWidget *krimbleBand() const { return m_band; }
private:
    QWidget *m_band=nullptr;
    KrimblePalettePage *m_palettePage=nullptr;
    KrimbleHistoryPage *m_historyPage=nullptr;
    QTimer m_historyTimer;
    QTimer m_recentTimer;
    bool m_ready=false;
    bool m_editBackground=false;
    KrimbleSlidersTop *m_sliders=nullptr;
    KrimbleValuesTop *m_values=nullptr;
    KrimbleColorState m_state;
    int m_tab=0;
    QVector<QWidget *> m_recentCells;
    QVector<QWidget *> m_swatchCells;
    int m_selectedSwatch=-1;
    QString m_paletteName = QStringLiteral("My Palette");
    int m_paletteColumns = 19;
    QString m_paletteModel = QStringLiteral("RGB");
    QVector<KrimbleCmyk> m_cmyk;
    KrimblePalette currentPalette() const { KrimblePalette pal; pal.name = m_paletteName; pal.columns = m_paletteColumns; pal.model = m_paletteModel; pal.colors = m_colors; pal.cmyk = m_cmyk; return pal; }
    void takePalette(const KrimblePalette &pal) { m_colors = pal.colors; m_cmyk = pal.cmyk; }
    QString m_paletteSignature;
    bool m_hslMode=false;
    QGridLayout *m_recentLayout=nullptr;
    QVector<QColor> m_recent;
    QVector<QColor> m_colors;
};
