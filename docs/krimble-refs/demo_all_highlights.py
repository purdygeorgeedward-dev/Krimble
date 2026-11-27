exec(open('/tmp/aqua_demo.py').read().split('bg=(56,62,96)')[0])
from PIL import Image, ImageDraw, ImageFont
F=lambda s: ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',s)
FB=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',30)
BG=(56,62,96); FIELD=(26,32,50); TXT=(235,238,248)
OLD_OUT=(59,213,247); OLD_HL=(19,96,146); BLUE=(11,135,221)
rows=["Text field (focused)","Drop-down (focused)","Check box (checked)","Tooltip","Progress bar","List / menu highlight","Selected layer row","Color Lab border"]
cols=["Before 780 (cyan)","Now (flat blue)","Aqua"]
CW,RH,LW=430,120,360
W=LW+CW*3; H=90+RH*len(rows)
im=Image.new('RGB',(W,H),BG); d=ImageDraw.Draw(im)
for j,c in enumerate(cols): d.text((LW+j*CW+CW/2,45),c,fill=TXT,font=FB,anchor='mm')
def paste_aqua(x,y,w,h,r):
    a=aqua(w,h,r); im.paste(a,(x,y),a)
def outline(x,y,w,h,col,width=3,r=6):
    d.rounded_rectangle([x,y,x+w,y+h],radius=r,outline=col,width=width)
def aqua_outline(x,y,w,h):
    # glossy rim: aqua strip ring
    paste_aqua(x,y,w,h,8); d.rounded_rectangle([x+5,y+5,x+w-5,y+h-5],radius=4,fill=FIELD)
for i,name in enumerate(rows):
    y0=90+i*RH; d.text((20,y0+RH/2),name,fill=TXT,font=F(26),anchor='lm')
    for j in range(3):
        x=LW+j*CW+30; y=y0+25; w=CW-60; h=RH-50
        if i in (0,1):
            if j==2: aqua_outline(x,y,w,h)
            else:
                d.rounded_rectangle([x,y,x+w,y+h],radius=6,fill=FIELD)
                outline(x,y,w,h,OLD_OUT if j==0 else BLUE)
            d.text((x+20,y+h/2),"Width: 142 px" if i==0 else "Normal",fill=TXT,font=F(26),anchor='lm')
            if i==1: d.polygon([(x+w-40,y+h/2-6),(x+w-20,y+h/2-6),(x+w-30,y+h/2+6)],fill=TXT)
        elif i==2:
            s=h; 
            if j==2: paste_aqua(x,y,s,s,6)
            else:
                d.rounded_rectangle([x,y,x+s,y+s],radius=4,fill=OLD_OUT if j==0 else BLUE)
            d.line([(x+12,y+s/2),(x+s/2-2,y+s-14),(x+s-10,y+12)],fill='white',width=6)
            d.text((x+s+20,y+s/2),"Enable",fill=TXT,font=F(26),anchor='lm')
        elif i==3:
            if j==2: paste_aqua(x,y,w-60,h,10); d.text((x+20,y+h/2),"Rectangular Selection",fill=(10,14,24),font=F(24),anchor='lm')
            else:
                d.rounded_rectangle([x,y,x+w-60,y+h],radius=6,fill=(35,42,66),outline=OLD_OUT if j==0 else BLUE,width=3)
                d.text((x+20,y+h/2),"Rectangular Selection",fill=TXT,font=F(24),anchor='lm')
        elif i==4:
            d.rounded_rectangle([x,y+12,x+w,y+h-12],radius=8,fill=FIELD)
            if j==2: paste_aqua(x,y+12,int(w*0.62),h-24,8)
            else: d.rounded_rectangle([x,y+12,x+int(w*0.62),y+h-12],radius=8,fill=OLD_OUT if j==0 else BLUE)
        elif i in (5,6):
            if j==2: paste_aqua(x-30,y-5,w+60,h+10,4)
            else: d.rectangle([x-30,y-5,x+w+30,y+h+5],fill=OLD_HL if j==0 else BLUE)
            d.text((x+10,y+h/2),"Fill" if i==5 else "Background",fill='white',font=F(28),anchor='lm')
        elif i==7:
            d.rectangle([x,y,x+w,y+h],fill=(30,36,60))
            if j==2: aqua_outline(x,y,w,h); 
            else: outline(x,y,w,h,(53,216,248) if j==0 else BLUE,4,8)
            d.text((x+w/2,y+h/2),"HSV",fill=TXT,font=F(26),anchor='mm')
im.save('/tmp/demo_all_highlights.png')
