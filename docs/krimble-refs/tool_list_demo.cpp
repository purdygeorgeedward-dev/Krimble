#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QFont>
static const QString K="/home/claude/krimble/krita/pics/";
static QImage sil(const QString&n,int S){QImage ic(S,S,QImage::Format_ARGB32);ic.fill(Qt::transparent);QSvgRenderer r(K+"tools/SVG/16/dark_"+n+".svg");if(!r.isValid())r.load(K+"svg/dark_"+n+".svg");if(!r.isValid())r.load(K+"svg/light_"+n+".svg");QPainter p(&ic);r.render(&p);p.end();
 for(int y=0;y<S;++y){QRgb*l=(QRgb*)ic.scanLine(y);for(int x=0;x<S;++x)l[x]=qRgba(20,20,24,qAlpha(l[x]));}return ic;}
static QImage gadget(int kind,int S){QImage pm(S,S,QImage::Format_ARGB32);pm.fill(Qt::transparent);QPainter p(&pm);p.setRenderHint(QPainter::Antialiasing);qreal u=S/28.0;QColor c(20,20,24);p.setPen(QPen(c,2*u));
 if(kind==0){p.drawRect(QRectF(3*u,6*u,22*u,16*u));p.setBrush(c);p.setPen(Qt::NoPen);p.drawEllipse(QPointF(14*u,14*u),4.5*u,4.5*u);}
 else{p.drawRect(QRectF(9*u,4*u,16*u,12*u));p.setBrush(QColor(255,255,255));p.drawRect(QRectF(3*u,11*u,16*u,12*u));}return pm;}
int main(int c,char**v){QGuiApplication app(c,v);
 QList<QPair<QString,QString>> T={{"krita_tool_move","Move"},{"tool_rect_selection","Rectangular Select"},{"tool_elliptical_selection","Elliptical Select"},{"tool_outline_selection","Freehand Select (Lasso)"},{"tool_polygonal_selection","Polygonal Select"},{"tool_magnetic_selection","Magnetic Select"},{"tool_contiguous_selection","Contiguous Select (Magic Wand)"},{"tool_similar_selection","Similar Color Select"},{"tool_crop","Crop"},{"krita_tool_color_sampler","Color Sampler (Eyedropper)"},{"krita_tool_smart_patch","Healing Brush (Smart Patch)"},{"krita_tool_freehand","Freehand Brush"},{"krita_tool_smudge","Smudge"},{"krita_tool_soften","Blur (Soften)"},{"krita_tool_eraser","Eraser"},{"krita_tool_gradient","Gradient"},{"krita_tool_color_fill","Fill (Paint Bucket)"},{"krita_draw_path","Bezier Curve (Pen)"},{"select","Shape Select"},{"draw-text","Text"},{"krita_tool_rectangle","Rectangle"},{"krita_tool_ellipse","Ellipse"},{"krita_tool_line","Line"},{"krita_tool_polygon","Polygon"},{"tool_pan","Pan (Hand)"},{"tool_zoom","Zoom"},{"krita_tool_transform","Transform"},{"krimble-free-rotate","Free Rotate"},{"krita_tool_assistant","Assistant Tool"},{"krita_tool_measure","Measure"},{"krita_tool_reference_images","Reference Images"},{"tool_path_selection","Edit Path Nodes"},{"krita_tool_undo","Undo"},{"#qm","Quick Mask"},{"#sm","Screen Mode"}};
 const int IS=56,RH=72,CW=740,rows=18; QImage out(2*CW+20,rows*RH+20,QImage::Format_RGB32);out.fill(Qt::white);QPainter p(&out);p.setRenderHint(QPainter::Antialiasing);p.setRenderHint(QPainter::TextAntialiasing);
 QFont f("DejaVu Sans",22,QFont::Bold);p.setFont(f);p.setPen(QColor(10,10,14));
 for(int i=0;i<T.size();++i){int x=10+(i/rows)*CW,y=10+(i%rows)*RH;QImage im=T[i].first=="#qm"?gadget(0,IS):T[i].first=="#sm"?gadget(1,IS):sil(T[i].first,IS);
  p.drawImage(x,y+(RH-IS)/2,im);p.drawText(QRect(x+IS+14,y,CW-IS-20,RH),Qt::AlignVCenter|Qt::AlignLeft,QString("%1. %2").arg(i+1).arg(T[i].second));}
 p.end();out.save("/tmp/tl/tool_list.png");}
