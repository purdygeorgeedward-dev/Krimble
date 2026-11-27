import numpy as np
from PIL import Image, ImageDraw, ImageFont
W,H=1080,640
yy,xx=np.mgrid[0:H,0:W].astype(np.float32)
def background():
    t=yy/H
    top=np.array([12,16,40]); bot=np.array([70,40,60])
    img=top[None,None,:]*(1-t[...,None])+bot[None,None,:]*t[...,None]
    # skyline silhouette
    rng=np.random.default_rng(3)
    x=0
    while x<W:
        w=rng.integers(40,110); h=rng.integers(80,260)
        img[H-h:,x:x+w]=img[H-h:,x:x+w]*0.25
        x+=w+rng.integers(2,12)
    return img/255.0
def screen(a,b): return 1-(1-a)*(1-b)
def gauss(r,s): return np.exp(-(r/s)**2)
def hsv(h): # hue 0..1 -> rgb
    import colorsys; return np.array(colorsys.hsv_to_rgb(h,0.8,1.0))
def polygon_disc(cx,cy,R,sides,rot,soft):
    dx,dy=xx-cx,yy-cy
    r=np.hypot(dx,dy); th=np.arctan2(dy,dx)-rot
    seg=2*np.pi/sides
    a=np.mod(th,seg)-seg/2
    rpoly=R*np.cos(seg/2)/np.cos(a)
    return np.clip((rpoly-r)/soft+0.5,0,1)
def flare(img,sx,sy,kind,bright=1.0,seed=1):
    rng=np.random.default_rng(seed)
    cx,cy=W/2,H/2
    dx,dy=xx-sx,yy-sy; r=np.hypot(dx,dy); th=np.arctan2(dy,dx)
    add=np.zeros((H,W,3),np.float32)
    warm=np.array([1.0,0.85,0.6]); cool=np.array([0.55,0.75,1.0])
    tint=warm if kind!='anamorphic' else cool
    # core and glow
    add+=gauss(r,10)[...,None]*np.array([1,1,1])*1.6
    add+=gauss(r,45)[...,None]*tint*0.9
    add+=gauss(r,170)[...,None]*tint*0.35
    # starburst rays
    if kind in ('zoom','prime','star'):
        n=180 if kind!='star' else 6
        angs=rng.uniform(0,2*np.pi,n) if kind!='star' else np.arange(6)*np.pi/3+0.3
        lens=rng.uniform(0.15,1.0,n) if kind!='star' else np.ones(6)
        ray=np.zeros((H,W),np.float32)
        width=0.004 if kind!='star' else 0.012
        for a,l in zip(angs,lens):
            d=np.angle(np.exp(1j*(th-a)))
            ray+=np.exp(-(d/width)**2)*l*np.exp(-r/(260*l if kind!='star' else 420))
        add+=np.clip(ray,0,3)[...,None]*tint*(0.55 if kind!='star' else 1.1)
    # anamorphic streak
    if kind=='anamorphic':
        add+=(np.exp(-(dy/5)**2)*np.exp(-np.abs(dx)/520))[...,None]*np.array([0.45,0.7,1.0])*1.3
        add+=(np.exp(-(dy/18)**2)*np.exp(-np.abs(dx)/300))[...,None]*np.array([0.3,0.5,1.0])*0.4
    # rainbow ring
    if kind in ('zoom','prime'):
        R=230 if kind=='zoom' else 150
        ring=np.zeros((H,W,3),np.float32)
        for k,h in enumerate(np.linspace(0,0.8,6)):
            ring+=gauss(r-(R+k*6),6)[...,None]*hsv(h)*0.06
        add+=ring
    # ghosts along the axis through the center
    vx,vy=cx-sx,cy-sy
    ghosts={'zoom':[(0.35,22,0.30,0.55),(0.6,10,0.20,0.15),(0.9,55,0.12,0.72),(1.15,30,0.18,0.35),(1.4,90,0.07,0.6),(1.7,16,0.22,0.1),(2.0,70,0.09,0.9)],
            'prime':[(0.5,40,0.14,0.08),(1.2,24,0.18,0.55),(1.6,60,0.08,0.75)],
            'anamorphic':[(0.6,30,0.10,0.6),(1.3,50,0.08,0.62),(1.8,20,0.12,0.58)],
            'star':[(0.8,18,0.10,0.12),(1.5,35,0.07,0.6)]}[kind]
    for t,R,a,h in ghosts:
        gx,gy=sx+vx*t,sy+vy*t
        if kind=='anamorphic':
            ex=np.hypot((xx-gx)/2.2,(yy-gy)); m=np.clip((R-ex)/3+0.5,0,1)
        else:
            m=polygon_disc(gx,gy,R,6,0.4,2.5)
        # brighter rim, softer middle
        rr=np.hypot(xx-gx,yy-gy)
        rim=0.6+0.4*np.clip(rr/R,0,1)**3
        add+=(m*rim)[...,None]*hsv(h)*a
    # tiny bokeh specks
    for k in range(6 if kind=='zoom' else 2):
        t=rng.uniform(0.2,2.1); R=rng.uniform(3,8)
        add+=gauss(np.hypot(xx-(sx+vx*t+rng.normal(0,15)),yy-(sy+vy*t+rng.normal(0,15))),R)[...,None]*hsv(rng.uniform())*0.25
    add*=bright
    return screen(img,np.clip(add,0,1))
f=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',34)
names={'zoom':'Zoom lens','prime':'Prime lens','anamorphic':'Cinematic (anamorphic)','star':'Starburst'}
outs=[]
for kind,(sx,sy) in [('zoom',(780,170)),('prime',(800,190)),('anamorphic',(760,220)),('star',(790,180))]:
    im=flare(background(),sx,sy,kind)
    p=Image.fromarray((np.clip(im,0,1)*255).astype('uint8'))
    ImageDraw.Draw(p).text((24,20),names[kind],fill=(255,255,255),font=f)
    p.save(f'/tmp/flare/{kind}.png'); outs.append(p)
sheet=Image.new('RGB',(W*2+10,H*2+10),(0,0,0))
for i,p in enumerate(outs): sheet.paste(p,((i%2)*(W+10),(i//2)*(H+10)))
sheet.save('/tmp/flare/lens_flare_demo.png')
