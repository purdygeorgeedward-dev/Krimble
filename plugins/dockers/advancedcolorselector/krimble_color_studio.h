// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <functional>
#include <QImage>
#include <QSizePolicy>
#include <QMenu>

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
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
#include <algorithm>
#include "kis_canvas2.h"
#include "kis_canvas_resource_provider.h"
#include "KoColor.h"
#include "KoColorSpaceRegistry.h"

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
        if (c.hsvHue() >= 0) m_hue = c.hsvHue();
        m_sat = c.hsvSaturation(); m_val = c.value(); update();
    }
    void setHue(int hue) { m_hue = hue; update(); }
    std::function<void(QColor)> onPick;
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        QImage image(size(), QImage::Format_RGB32);
        const int w = qMax(1, width()-1), h = qMax(1, height()-1);
        for (int y=0; y<height(); ++y)
            for (int x=0; x<width(); ++x)
                image.setPixelColor(x,y,QColor::fromHsv(m_hue, x*255/w, 255-y*255/h));
        p.drawImage(0,0,image);
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
        m_square = new KrimbleSvSquare(this);
        layout->addWidget(m_square);
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
        layout->addLayout(hueRow);
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
        layout->addLayout(values);
        m_preview=new QLabel(this); m_preview->setFixedHeight(24);
        layout->addWidget(m_preview);
        layout->addWidget(new QLabel(tr("My Palette  •  tap to select, long-press not required"),this));
        m_swatches=new QGridLayout; m_swatches->setSpacing(3);
        layout->addLayout(m_swatches);
        m_square->onPick=[this](QColor c){ select(c); };
        connect(m_hue,&QSlider::valueChanged,this,[this](int h) {
            m_square->setHue(h); select(m_square->selected());
        });
        for (int i=0;i<3;++i)
            connect(m_rgb[i],qOverload<int>(&QSpinBox::valueChanged),this,[this](int) {
                select(QColor(m_rgb[0]->value(),m_rgb[1]->value(),m_rgb[2]->value()));
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
        m_current=c;
        m_square->setColor(c);
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
    void rebuildSwatches() {
        while(QLayoutItem *item=m_swatches->takeAt(0)) {
            delete item->widget(); delete item;
        }
        for(int i=0;i<m_colors.size();++i) {
            QColor color=m_colors[i];
            auto *button=new QPushButton(this);
            button->setFixedHeight(28);
            button->setToolTip(color.name());
            button->setStyleSheet(QStringLiteral("background:%1;border:1px solid #7381a8;border-radius:4px;").arg(color.name()));
            connect(button,&QPushButton::clicked,this,[this,color](){select(color);});
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(button,&QWidget::customContextMenuRequested,this,[this,button,i](const QPoint &pos) {
                QMenu menu(button);
                QAction *replace=menu.addAction(tr("Replace with current color"));
                QAction *remove=menu.addAction(tr("Remove swatch"));
                QAction *chosen=menu.exec(button->mapToGlobal(pos));
                if(chosen==replace) m_colors[i]=m_current;
                else if(chosen==remove) m_colors.removeAt(i);
                else return;
                savePalette(); rebuildSwatches();
            });
            m_swatches->addWidget(button,i/6,i%6);
        }
    }
    QPointer<KisCanvas2> m_canvas;
    QColor m_current;
    KrimbleSvSquare *m_square=nullptr;
    QSlider *m_hue=nullptr;
    QSpinBox *m_rgb[3]{};
    QLineEdit *m_hex=nullptr;
    QLabel *m_preview=nullptr;
    QGridLayout *m_swatches=nullptr;
    QVector<QColor> m_colors;
};
