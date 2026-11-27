import numpy as np
from PIL import Image, ImageDraw, ImageFont
SS=3
def rrect_mask(w,h,x0,y0,x1,y1,r):
    yy,xx=np.mgrid[0:h,0:w]+0.5
    cx=np.clip(xx,x0+r,x1-r); cy=np.clip(yy,y0+r,y1-r)
    return (((xx-cx)**2+(yy-cy)**2)<=r*r)&(xx>=x0)&(xx<=x1)&(yy>=y0)&(yy<=y1)
def mix(c,t,a): c=np.array(c,float); return c+(np.array(t,float)-c)*a
def aqua(w,h,r,base=(11,135,221)):
    W,H,R=w*SS,h*SS,r*SS
    img=np.zeros((H,W,4))
    body=rrect_mask(W,H,0,0,W,H,R)
    t=(np.arange(H)+0.5)/H
    stops=[(0,mix(base,(0,0,0),0.30)),(0.12,mix(base,(0,0,0),0.12)),(0.5,mix(base,(0,0,0),0.05)),(0.85,mix(base,(255,255,255),0.35)),(1.0,mix(base,(255,255,255),0.50))]
    col=np.zeros((H,3))
    for i in range(3): col[:,i]=np.interp(t,[s[0] for s in stops],[s[1][i] for s in stops])
    rgb=np.repeat(col[:,None,:],W,1)
    # bottom glow
    yy,xx=np.mgrid[0:H,0:W]+0.5
    g=np.clip(1-(((xx-W/2)/(W*0.55))**2+((yy-H*1.02)/(H*0.55))**2),0,1)**1.5
    rgb=rgb+(np.array(mix(base,(255,255,255),0.75))-rgb)*(g[...,None]*0.55)
    # edge darkening (rim)
    edge=body & ~rrect_mask(W,H,SS*1.5,SS*1.5,W-SS*1.5,H-SS*1.5,max(R-SS*1.5,1))
    rgb[edge]=mix(base,(0,0,0),0.40)
    # gloss
    gx0,gx1=W*0.07,W*0.93; gy0,gy1=H*0.05,H*0.50
    gm=rrect_mask(W,H,gx0,gy0,gx1,gy1,min(R*0.85,(gy1-gy0)/2))
    ga=np.interp(yy,[gy0,gy1],[0.85,0.12])
    a=(gm*ga)[...,None]
    rgb=rgb*(1-a)+255*a
    img[...,:3]=rgb; img[...,3]=body*255
    im=Image.fromarray(np.clip(img,0,255).astype('uint8'),'RGBA')
    return im.resize((w,h),Image.LANCZOS)
bg=(56,62,96)
out=Image.new('RGB',(1080,560),bg); d=ImageDraw.Draw(out)
f=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',30)
src=Image.open('docs/krimble-refs/2026-11-20_2252_batch-02.jpg').convert('L')
mask=src.crop((112,838,170,896)).point(lambda v: 0 if v<110 else min(255,(v-110)*2)).resize((150,150),Image.LANCZOS)
b=aqua(240,240,22); out.paste(b,(60,50),b); out.paste((240,242,250),(105,95),mask)
d.text((180,330),"Toolbox button",fill=(225,228,240),font=f,anchor='mm')
p=aqua(600,120,60); out.paste(p,(400,80),p)
d.text((700,140),"AQUA",fill=(10,14,24),font=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',70),anchor='mm')
d.text((700,240),"Pill, like your picture",fill=(225,228,240),font=f,anchor='mm')
m=aqua(600,70,6); out.paste(m,(400,300),m)
d.text((430,335),"Fill",fill=(255,255,255),font=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',36),anchor='lm')
d.text((700,410),"Menu highlight",fill=(225,228,240),font=f,anchor='mm')
out.save('/tmp/aqua_gradient_demo.png')
