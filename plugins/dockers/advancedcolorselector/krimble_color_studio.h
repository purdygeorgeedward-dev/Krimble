// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <functional>
#include <QImage>
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
        setMinimumHeight(100);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
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
        if (m_cachedHue != m_hue || m_cachedSquare.size() != size()) {
            m_cachedSquare = QImage(size(), QImage::Format_RGB32);
            for (int y=0; y<height(); ++y)
                for (int x=0; x<width(); ++x)
                    m_cachedSquare.setPixelColor(x,y,QColor::fromHsv(m_hue, x*255/w, 255-y*255/h));
            m_cachedHue = m_hue;
        }
        p.drawImage(0,0,m_cachedSquare);
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
    void clearPad() { snapshot(); m_paint.fill(Qt::transparent); update(); }
    void undo() {
        if (m_undo.isEmpty()) return;
        m_redo.append(m_paint);
        m_paint=m_undo.takeLast();
        update();
    }
    void redo() {
        if (m_redo.isEmpty()) return;
        m_undo.append(m_paint);
        m_paint=m_redo.takeLast();
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
        QPen pen(c, m_size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.drawLine(from,to);
        update();
    }
    QImage m_paint;
    QVector<QImage> m_undo,m_redo;
    QColor m_color=QColor("#459ad3");
    QPoint m_last;
    int m_size=18,m_opacity=85;
    bool m_drawing=false,m_eraser=false,m_sampling=false;
};

class KrimbleColorStudio : public QWidget
{
public:
    explicit KrimbleColorStudio(QWidget *parent=nullptr) : QWidget(parent) {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(5,5,5,5); layout->setSpacing(5);
        auto *header = new QHBoxLayout;
        auto *title = new QLabel(tr("COLOR STUDIO"), this);
        QFont f=title->font(); f.setBold(true); title->setFont(f);
        header->addWidget(title); header->addStretch();
        auto *add = new QPushButton(tr("+ Swatch"), this);
        add->setToolTip(tr("Add foreground color to palette"));
        header->addWidget(add);
        layout->addLayout(header);
        auto *tabs = new QTabWidget(this);
        auto *picker = new QWidget(tabs);
        auto *mixer = new QWidget(tabs);
        auto *pickerLayout = new QVBoxLayout(picker);
        pickerLayout->setContentsMargins(0,4,0,0);
        pickerLayout->setSpacing(5);
        tabs->addTab(picker,tr("Picker"));
        tabs->addTab(mixer,tr("Mixer"));
        layout->addWidget(tabs);
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
        m_square->onPick=[this](QColor c){ select(c); };
        connect(m_hue,&QSlider::valueChanged,this,[this](int h) {
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
        loadPalette();
        select(QColor("#459ad3"),false);
        // Keep the controls in sync when another docker or the eyedropper changes
        // the foreground color. Only poll while the Studio is visible.
        m_syncTimer.setInterval(350);
        connect(&m_syncTimer, &QTimer::timeout, this, [this]() {
            if (!m_canvas || !isVisible()) return;
            const QColor foreground=m_canvas->resourceManager()->foregroundColor().toQColor();
            if (foreground.isValid() && foreground != m_current)
                select(foreground, false);
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
            while (m_recent.size()>12) m_recent.removeLast();
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
    void loadPalette() {
        QSettings settings;
        QStringList items=settings.value(QStringLiteral("Krimble/ColorStudioSwatches")).toStringList();
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
            button->setFixedHeight(24);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral(
                "background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            m_recentLayout->addWidget(button,i/6,i%6);
        }
    }
    void rebuildSwatches() {
        while(QLayoutItem *item=m_swatches->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new KrimbleSwatchButton(this);
            button->setFixedHeight(36);
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
            m_swatches->addWidget(button,i/6,i%6);
        }
    }
    QPointer<KisCanvas2> m_canvas;
    QTimer m_syncTimer;
    QColor m_current;
    KrimbleSvSquare *m_square=nullptr;
    KrimbleMixingPad *m_mixingPad=nullptr;
    QSlider *m_hue=nullptr;
    QSpinBox *m_rgb[3]{};
    QSpinBox *m_hsv[3]{};
    QLineEdit *m_hex=nullptr;
    QLabel *m_preview=nullptr;
    QGridLayout *m_swatches=nullptr;
    QGridLayout *m_recentLayout=nullptr;
    QVector<QColor> m_recent;
    QVector<QColor> m_colors;
};
