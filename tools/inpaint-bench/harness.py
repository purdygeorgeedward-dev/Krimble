import numpy as np, subprocess, sys, time, cv2
from PIL import Image, ImageDraw

def save_ppm(path, rgb):
    h,w,_=rgb.shape
    with open(path,'wb') as f: f.write(b'P6\n%d %d\n255\n'%(w,h)); f.write(rgb.tobytes())
def save_pgm(path, m):
    h,w=m.shape
    with open(path,'wb') as f: f.write(b'P5\n%d %d\n255\n'%(w,h)); f.write((m.astype(np.uint8)*255).tobytes())
def load_ppm(path):
    with open(path,'rb') as f:
        data=f.read()
    parts=data.split(b'\n',3)
    w,h=map(int,parts[1].split()); return np.frombuffer(parts[3],dtype=np.uint8).reshape(h,w,3)

def run(name, rgb, mask, expand=2, sharp=1.0):
    save_ppm(f'{name}_in.ppm', rgb); save_pgm(f'{name}_mask.pgm', mask)
    t=time.time()
    p=subprocess.run(['./inpaint_test',f'{name}_in.ppm',f'{name}_mask.pgm',f'{name}_out.ppm',str(expand),str(sharp)],capture_output=True,text=True)
    dt=time.time()-t
    out=load_ppm(f'{name}_out.ppm')
    m=(mask.astype(np.uint8)*255)
    tel=cv2.inpaint(cv2.cvtColor(rgb,cv2.COLOR_RGB2BGR), cv2.dilate(m,np.ones((5,5),np.uint8)), 5, cv2.INPAINT_TELEA)
    tel=cv2.cvtColor(tel,cv2.COLOR_BGR2RGB)
    return out, tel, dt, p.stderr.strip()

def sheet(name, rgb, mask, out, tel, pad=40, scale=1.0):
    ys,xs=np.where(mask); y0=max(0,ys.min()-pad); y1=min(rgb.shape[0],ys.max()+pad); x0=max(0,xs.min()-pad); x1=min(rgb.shape[1],xs.max()+pad)
    orig=rgb.copy(); 
    om=Image.fromarray(orig); d=ImageDraw.Draw(om)
    cnts,_=cv2.findContours(mask.astype(np.uint8),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
    for c in cnts: d.line([tuple(p[0]) for p in c]+[tuple(c[0][0])],fill=(255,0,0),width=2)
    tiles=[Image.fromarray(np.array(om)[y0:y1,x0:x1]),Image.fromarray(tel[y0:y1,x0:x1]),Image.fromarray(out[y0:y1,x0:x1])]
    W=sum(t.width for t in tiles)+20; H=tiles[0].height+22
    s=Image.new('RGB',(W,H),(20,20,20)); dd=ImageDraw.Draw(s); x=0
    for t,lab in zip(tiles,['original + hole','standard blur fill (Telea)','Krimble PatchMatch']):
        s.paste(t,(x,22)); dd.text((x+4,4),lab,fill=(255,255,0)); x+=t.width+10
    if scale!=1.0: s=s.resize((int(s.width*scale),int(s.height*scale)),Image.LANCZOS)
    s.save(f'{name}_sheet.png'); return s.size
