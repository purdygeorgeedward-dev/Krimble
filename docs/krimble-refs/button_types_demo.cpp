#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QFont>
#include <QPainterPath>
#include <QLinearGradient>
static const QString K="/home/claude/krimble/krita/pics/";
static QImage sil(const QString&n,int S){QImage ic(S,S,QImage::Format_ARGB32);ic.fill(Qt::transparent);QSvgRenderer r(K+"tools/SVG/16/dark_"+n+".svg");if(!r.isValid())r.load(K+"svg/dark_"+n+".svg");QPainter p(&ic);r.render(&p);return ic;}
static QImage pic(const QString&f,int S){return QImage(K+"krimble-toolbox-color/"+f+".png").scaled(S,S,Qt::KeepAspectRatio,Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);}
static void tint(QImage&im,int mode){for(int y=0;y<im.height();++y){QRgb*l=(QRgb*)im.scanLine(y);for(int x=0;x<im.width();++x){QRgb c=l[x];if(mode==1)l[x]=qRgba(24,24,28,qAlpha(c));else if(mode==3)l[x]=qRgba(225,228,240,qAlpha(c));else if(mode==2){int g=(qRed(c)*2126+qGreen(c)*7152+qBlue(c)*722)/10000;l[x]=qRgba(g,g,g,qAlpha(c));}}}}
static QColor mix(QColor a,QColor b,double t){return QColor(a.red()+(b.red()-a.red())*t,a.green()+(b.green()-a.green())*t,a.blue()+(b.blue()-a.blue())*t);}
static void aqua(QPainter&p,QRectF r){QColor base("#0b87dd");double rad=r.width()*0.12;QPainterPath path;path.addRoundedRect(r,rad,rad);
 QLinearGradient g(r.topLeft(),r.bottomLeft());g.setColorAt(0,mix(base,Qt::black,.30));g.setColorAt(.12,mix(base,Qt::black,.12));g.setColorAt(.5,mix(base,Qt::black,.05));g.setColorAt(.85,mix(base,Qt::white,.35));g.setColorAt(1,mix(base,Qt::white,.5));
 p.fillPath(path,g);QLinearGradient rim(r.topLeft(),r.bottomLeft());rim.setColorAt(0,QColor("#9bd6ff"));rim.setColorAt(.5,QColor("#0b87dd"));rim.setColorAt(1,QColor("#075a96"));p.setPen(QPen(QBrush(rim),2));p.drawPath(path);
 QRectF gl(r.left()+r.width()*.08,r.top()+2,r.width()*.84,r.height()*.45);QPainterPath gp;gp.addRoundedRect(gl,rad*.8,rad*.8);QLinearGradient gg(gl.topLeft(),gl.bottomLeft());gg.setColorAt(0,QColor(255,255,255,217));gg.setColorAt(1,QColor(255,255,255,31));p.fillPath(gp,gg);}
static void raised(QPainter&box,QRect r,QColor face){bool light=face.lightness()>150;int t=qMax(2,qRound(qMin(r.width(),r.height())*0.075));box.save();box.setRenderHint(QPainter::Antialiasing,false);box.fillRect(r,face);int hiTop=light?255:85,hiLeft=light?170:40;
 QColor topTone(255,255,255,hiTop),leftTone(255,255,255,hiLeft),rightTone(0,0,0,55),bottomTone(0,0,0,90);box.setPen(Qt::NoPen);QPolygon poly;
 poly<<r.topLeft()<<r.topRight()<<QPoint(r.right()-t,r.top()+t)<<QPoint(r.left()+t,r.top()+t);box.setBrush(topTone);box.drawPolygon(poly);
 poly.clear();poly<<r.topLeft()<<QPoint(r.left()+t,r.top()+t)<<QPoint(r.left()+t,r.bottom()-t)<<r.bottomLeft();box.setBrush(leftTone);box.drawPolygon(poly);
 poly.clear();poly<<r.topRight()<<r.bottomRight()<<QPoint(r.right()-t,r.bottom()-t)<<QPoint(r.right()-t,r.top()+t);box.setBrush(rightTone);box.drawPolygon(poly);
 poly.clear();poly<<r.bottomLeft()<<QPoint(r.left()+t,r.bottom()-t)<<QPoint(r.right()-t,r.bottom()-t)<<r.bottomRight();box.setBrush(bottomTone);box.drawPolygon(poly);box.restore();}
int main(int c,char**v){QGuiApplication app(c,v);
 QList<QPair<QString,QString>> T={{"krita_tool_move","01_01_move"},{"krita_tool_freehand","03_02_freehand_brush"},{"krita_tool_color_sampler","02_05_color_sampler"},{"draw-text","04_05_text"},{"tool_rect_selection","01_02_rectangular_select"},{"krita_tool_gradient","04_01_gradient"}};
 QStringList line={"krita_tool_move","tool_rect_selection","draw-text"};
 QStringList styles={"White","Black","Grayscale","Classic"};
 struct F{QString n;QColor c;bool r;}; QList<F> faces={{"Panel, flat",QColor(56,62,96),false},{"Panel, raised",QColor(56,62,96),true},{"Light gray, flat",QColor(198,198,202),false},{"Light gray, raised",QColor(198,198,202),true},{"White, flat",QColor(250,250,252),false},{"White, raised",QColor(250,250,252),true},{"Dark gray, flat",QColor(78,78,84),false},{"Dark gray, raised",QColor(78,78,84),true}};
 const int B=72,IS=50,cols=3,rowsT=2,pad=14,lab=150,head=44; int cw=cols*B+pad*2, ch=rowsT*B+pad*2;
 QImage out(lab+faces.size()*cw+10,head+styles.size()*ch+10,QImage::Format_RGB32);out.fill(QColor(36,40,62));QPainter p(&out);p.setRenderHint(QPainter::Antialiasing);p.setRenderHint(QPainter::SmoothPixmapTransform);
 QFont f("DejaVu Sans",15,QFont::Bold);p.setFont(f);p.setPen(Qt::white);
 for(int j=0;j<faces.size();++j)p.drawText(QRect(lab+j*cw,4,cw,head-8),Qt::AlignCenter|Qt::TextWordWrap,faces[j].n);
 for(int i=0;i<styles.size();++i){p.setPen(Qt::white);p.setFont(f);p.drawText(QRect(8,head+i*ch,lab-12,ch),Qt::AlignVCenter|Qt::AlignLeft,styles[i]);
  for(int j=0;j<faces.size();++j){int x0=lab+j*cw+pad,y0=head+i*ch+pad;p.fillRect(QRect(x0-6,y0-6,cols*B+12,rowsT*B+12),QColor(56,62,96));
   for(int k=0;k<T.size();++k){QRect r(x0+(k%cols)*B,y0+(k/cols)*B,B-4,B-4);bool sel=(k==1);
    if(sel)aqua(p,QRectF(r).adjusted(3,3,-3,-3));else{QColor fc=faces[j].c;if(faces[j].r&&j<2)fc=fc.lighter(150);if(faces[j].r)raised(p,r.adjusted(1,1,-1,-1),fc);else if(j>=2)p.fillRect(r.adjusted(1,1,-1,-1),fc);}
    QImage im;int s=styles[i]=="White"?0:styles[i]=="Black"?1:styles[i]=="Grayscale"?2:3;
    if(s==0){im=sil(T[k].first,IS);tint(im,faces[j].c.lightness()>150?1:3);}else if(s==1){im=sil(T[k].first,IS);tint(im,1);}else if(s==2){im=pic(T[k].second,IS);tint(im,2);}else{if(line.contains(T[k].first)){im=sil(T[k].first,IS);tint(im,1);}else{im=pic(T[k].second,IS);tint(im,2);}}
    int is=IS;if(T[k].first=="krita_tool_color_sampler"){is=IS*1.2;im=im.scaled(is,is,Qt::KeepAspectRatio,Qt::SmoothTransformation);}
    p.drawImage(r.x()+(r.width()-im.width())/2,r.y()+(r.height()-im.height())/2,im);}}}
 p.end();out.save("/tmp/bt/button_types2.png");}
