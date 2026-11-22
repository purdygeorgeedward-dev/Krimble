# Builds the Krimble comic presets (.kpp = PNG thumbnail + preset XML) from the shipped Pencil-2 and Fineliner presets.
import re, hashlib, base64, math, random
from PIL import Image, ImageDraw, ImageFilter
from PIL.PngImagePlugin import PngInfo
R='/home/claude/krimble/krita/data/'
P=R+'paintoppresets/'
def load(f): return Image.open(P+f).info['preset']
def setp(x,n,v):
    pat=re.compile(r'(<param (?:name="%s" type="string"|type="string" name="%s")><!\[CDATA\[)(.*?)(\]\]></param>)'%(re.escape(n),re.escape(n)),re.S)
    if pat.search(x): return pat.sub(lambda m:m.group(1)+v+m.group(3),x,count=1)
    return x.replace('</Preset>',' <param name="%s" type="string"><![CDATA[%s]]></param> </Preset>'%(n,v))
def sensor(curve,id='pressure'): return '<!DOCTYPE params> <params id="%s"> <curve>%s</curve> </params> '%(id,curve)
def rename(x,name): return re.sub(r'<Preset name="[^"]*"','<Preset name="%s"'%name,x,count=1)
def brushdef(x,diam,fade,aa,spacing,auto,gauss=False):
    bd='<Brush autoSpacingCoeff="0.8" density="1" BrushVersion="2" type="auto_brush" spacing="%s" useAutoSpacing="%d" randomness="0" angle="0"> <MaskGenerator ratio="1" hfade="%s" spikes="2" vfade="%s" diameter="%s" type="circle" id="%s" antialiasEdges="%d"/> </Brush> '%(spacing,auto,fade,fade,diam,'gauss' if gauss else 'default',aa)
    return setp(x,'brush_definition',bd)
def paper(x,strength_curve):
    png=open(R+'patterns/03_default-paper.png','rb').read()
    md5=hashlib.md5(png).hexdigest()
    for n,v in [('Texture/Pattern/Enabled','true'),('Texture/Pattern/Scale','0.6'),('Texture/Pattern/Brightness','0'),('Texture/Pattern/Contrast','1.4'),
                ('Texture/Pattern/NeutralPoint','0.5'),('Texture/Pattern/OffsetX','0'),('Texture/Pattern/OffsetY','0'),('Texture/Pattern/isRandomOffsetX','true'),
                ('Texture/Pattern/isRandomOffsetY','true'),('Texture/Pattern/TexturingMode','1'),('Texture/Pattern/UseSoftTexturing','false'),
                ('Texture/Pattern/CutoffLeft','0'),('Texture/Pattern/CutoffRight','255'),('Texture/Pattern/CutoffPolicy','0'),('Texture/Pattern/Invert','false'),
                ('Texture/Pattern/AutoInvertOnErase','false'),('Texture/Pattern/PatternMD5Sum',md5),('Texture/Pattern/PatternMD5',base64.b64encode(bytes.fromhex(md5)).decode()),
                ('Texture/Pattern/PatternFileName','03_default-paper.png'),('Texture/Pattern/Name','03_default-paper'),
                ('PressureTexture/Strength/','true'),('Texture/Strength/Sensor',sensor(strength_curve)),('Texture/Strength/UseCurve','true'),
                ('Texture/Strength/UseSameCurve','true'),('Texture/Strength/Value','1'),('Texture/Strength/curveMode','0')]:
        x=setp(x,n,v)
    return x
# ---- thumbnails: a sample stroke on white, drawn to look like the tool
def thumb(kind,col):
    S=200; im=Image.new('RGB',(S,S),'white'); d=ImageDraw.Draw(im); rnd=random.Random(7)
    pts=[(20+i*1.6, 100+55*math.sin(i/100*math.pi*1.6)*(1-i/220)) for i in range(101)]
    for i,(px,py) in enumerate(pts):
        t=i/100
        if kind=='pencil':
            w=3.2+1.5*math.sin(t*math.pi)
            for k in range(14):
                ox,oy=rnd.uniform(-w,w),rnd.uniform(-w,w)
                if ox*ox+oy*oy>w*w: continue
                a=rnd.uniform(0.25,0.75)
                c=tuple(int(255-(255-cc)*a) for cc in col)
                d.point((px+ox,py+oy),fill=c)
        elif kind=='tech':
            r=2.6; d.ellipse((px-r,py-r,px+r,py+r),fill=col)
        else:
            r=0.6+9*math.sin(t*math.pi)**1.4; d.ellipse((px-r,py-r,px+r,py+r),fill=col)
    if kind!='pencil': im=im.filter(ImageFilter.SMOOTH)
    return im
def save(name,x,im):
    info=PngInfo(); info.add_text('version','2.2'); info.add_text('preset',x)
    im.save(P+name+'.kpp',format='PNG',pnginfo=info)
ink=load('d)_Ink-2_Fineliner.kpp'); pen=load('c)_Pencil-2.kpp')
# HB Pencil: graphite (Pencil-2's scatter, sharpness and building opacity), smaller, paper grain that shows most at light pressure
x=rename(pen,'c)_Pencil_HB'); x=brushdef(x,4,0.2,1,0.12,0,gauss=True)
x=setp(x,'SizeSensor',sensor('0,0.55;1,1;')); x=setp(x,'OpacitySensor',sensor('0,0;0.2,0.18;0.6,0.62;1,0.85;'))
x=paper(x,'0,1;1,0.35;'); save('c)_Pencil_HB',x,thumb('pencil',(70,70,70)))
# Blue Pencil: the same pencil; choosing it sets the color to non-photo blue (#A4DDED, KisPaintopBox::resourceSelected)
x=x.replace('<Preset name="c)_Pencil_HB"','<Preset name="c)_Pencil_Blue_Non-Photo"'); save('c)_Pencil_Blue_Non-Photo',x,thumb('pencil',(0x5a,0xb4,0xd8)))
# Technical Pen: one width at any pressure, solid black, crisp antialiased edge
x=rename(ink,'d)_Ink_Technical_Pen'); x=brushdef(x,3,1,1,0.05,1)
x=setp(x,'PressureSize','false'); x=setp(x,'PressureOpacity','false'); x=setp(x,'OpacityUseCurve','false')
save('d)_Ink_Technical_Pen',x,thumb('tech',(0,0,0)))
# Brush Pen: size strongly from pressure (concave: thin lines come easily, tapered ends), solid black
x=rename(ink,'d)_Ink_Brush_Pen'); x=brushdef(x,16,1,1,0.05,1)
x=setp(x,'PressureSize','true'); x=setp(x,'SizeUseSameCurve','true'); x=setp(x,'SizeSensor',sensor('0,0;0.3,0.08;0.7,0.5;1,1;'))
x=setp(x,'PressureOpacity','false'); save('d)_Ink_Brush_Pen',x,thumb('brush',(0,0,0)))
print('ok')
