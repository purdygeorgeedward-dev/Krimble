import numpy as np, sys, os, time, cv2
sys.path.insert(0,'/home/claude/inpaint')
from harness import *
from PIL import ImageDraw
def load(n): return np.array(Image.open(f'photo_{n}.png').convert('RGB'))
def circ(sh,cx,cy,r):
    m=np.zeros(sh[:2],np.uint8); cv2.circle(m,(cx,cy),r,1,-1); return m.astype(bool)
def ell(sh,cx,cy,a,b,ang=0):
    m=np.zeros(sh[:2],np.uint8); cv2.ellipse(m,(cx,cy),(a,b),ang,0,360,1,-1); return m.astype(bool)
def rect(sh,x0,y0,x1,y1):
    m=np.zeros(sh[:2],bool); m[y0:y1,x0:x1]=True; return m
CASES=[('grass',lambda s:circ(s,256,256,70)),('gravel',lambda s:circ(s,250,250,70)),('brick',lambda s:rect(s,170,200,330,300)),
 ('astronaut',lambda s:ell(s,400,130,50,38,20)),('coffee',lambda s:ell(s,450,230,55,40,0)),('chelsea',lambda s:circ(s,290,170,38)),
 ('stereo_motorcycle',lambda s:ell(s,250,250,60,40,0)),('camera',lambda s:rect(s,380,400,440,450))]
def bench(names=None, tag='b', pad=40):
    rows=[]
    for n,mf in CASES:
        if names and n not in names: continue
        im=load(n); m=mf(im.shape)
        out,tel,dt,err=run('bn_'+n,im,m,2,8)
        ys,xs=np.where(m); y0=max(0,ys.min()-100); y1=min(im.shape[0],ys.max()+100); x0=max(0,xs.min()-100); x1=min(im.shape[1],xs.max()+100)
        sub=im[y0:y1,x0:x1].copy(); mk=cv2.dilate(m[y0:y1,x0:x1].astype(np.uint8)*255,np.ones((5,5),np.uint8))
        res=np.zeros_like(sub); t=time.time(); cv2.xphoto.inpaint(sub,255-mk,res,cv2.xphoto.INPAINT_SHIFTMAP); ts=time.time()-t
        shift=im.copy(); shift[y0:y1,x0:x1]=res
        cy0=max(0,ys.min()-pad); cy1=min(im.shape[0],ys.max()+pad); cx0=max(0,xs.min()-pad); cx1=min(im.shape[1],xs.max()+pad)
        om=Image.fromarray(im); d=ImageDraw.Draw(om)
        cn,_=cv2.findContours(m.astype(np.uint8),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        for c in cn: d.line([tuple(p[0]) for p in c]+[tuple(c[0][0])],fill=(255,0,0),width=1)
        tiles=[Image.fromarray(np.array(om)[cy0:cy1,cx0:cx1]),Image.fromarray(tel[cy0:cy1,cx0:cx1]),Image.fromarray(shift[cy0:cy1,cx0:cx1]),Image.fromarray(out[cy0:cy1,cx0:cx1])]
        labs=[n+' + hole','blur (Telea)','OpenCV shift-map','Krimble (%.1fs)'%dt]
        Wt=sum(t.width for t in tiles)+30; s=Image.new('RGB',(Wt,tiles[0].height+18),(20,20,20)); dd=ImageDraw.Draw(s); x=0
        for t,l in zip(tiles,labs): s.paste(t,(x,18)); dd.text((x+3,3),l,fill=(255,255,0)); x+=t.width+10
        rows.append(s)
    # stack rows
    Wm=max(r.width for r in rows); Hm=sum(r.height+6 for r in rows)
    big=Image.new('RGB',(Wm,Hm),(0,0,0)); y=0
    for r in rows: big.paste(r,(0,y)); y+=r.height+6
    big.save(f'{tag}.png'); return big.size
if __name__=='__main__':
    print(bench(['grass','gravel','brick','astronaut'],'bench1')); print(bench(['coffee','chelsea','stereo_motorcycle','camera'],'bench2'))
