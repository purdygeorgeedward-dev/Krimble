import numpy as np
from PIL import Image, ImageDraw, ImageFont
src=np.asarray(Image.open('/home/claude/krimble/docs/krimble-refs/2026-11-20_2247_lens-flare-reference.png').convert('RGB')).astype(np.float64)/255
src=src[::2,::2]
# make it dull with a green-yellow cast and low contrast
img=np.clip(0.25+src*0.55*np.array([1.0,1.05,0.8])+np.array([0.0,0.04,0.0]),0,1)
def lum(a): return 0.2126*a[...,0]+0.7152*a[...,1]+0.0722*a[...,2]
def cuts(v,f): return np.quantile(v,f),np.quantile(v,1-f)
def auto(img,mode):
    b=np.zeros(3);w=np.ones(3);p=np.ones(3)
    for c in range(3):
        if mode=='contrast': b[c],w[c]=cuts(img.reshape(-1),0.001)
        else: b[c],w[c]=cuts(img[...,c],0.001)
    if mode=='color':
        L=lum(img).reshape(-1); flat=img.reshape(-1,3); o=np.argsort(L); k=max(1,int(len(L)*0.005))
        d=flat[o[:k]].mean(0); l=flat[o[-k:]].mean(0)
        for c in range(3):
            if l[c]-d[c]>0.05: b[c],w[c]=d[c],l[c]
    m=np.clip((img-b)/(w-b),0,1)
    if mode=='color':
        L=lum(m); ch=m.max(-1)-m.min(-1); sel=(L>=.25)&(L<=.75)&(ch<=.2)
        if sel.sum()<max(16,m.shape[0]*m.shape[1]/200): sel=(L>=.25)&(L<=.75)
        mm=m[sel].mean(0); t=mm.mean()
        p=np.clip(np.log(t)/np.log(mm),0.5,2)
        m=m**p
        L=lum(m)[...,None]; sat=(m.max(-1)-m.min(-1))[...,None]
        m=np.clip(L+(m-L)*(1+0.25*(1-sat)),0,1)
    return m
f=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',22)
tiles=[('Before (dull, color cast)',img),('Auto Tone',auto(img,'tone')),('Auto Contrast',auto(img,'contrast')),('Auto Color',auto(img,'color'))]
h,w=img.shape[:2]
sheet=Image.new('RGB',(w*2+10,h*2+10),'black')
for i,(n,a) in enumerate(tiles):
    t=Image.fromarray((a*255).astype('uint8')); ImageDraw.Draw(t).text((10,8),n,fill='white',font=f,stroke_width=2,stroke_fill='black')
    sheet.paste(t,((i%2)*(w+10),(i//2)*(h+10)))
sheet.save('/tmp/auto_adjust_test.png')
