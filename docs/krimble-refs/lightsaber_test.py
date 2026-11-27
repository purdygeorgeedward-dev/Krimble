import numpy as np
from PIL import Image
W,H=1080,640; yy,xx=np.mgrid[0:H,0:W]+0.5
bg=np.zeros((H,W,3)); bg[...]=np.array([14,16,30])/255
def saber(img,a,b,col,wf=10,glow=1,br=1):
    diag=np.hypot(W,H); w=diag*wf/1000
    vx,vy=b[0]-a[0],b[1]-a[1]; px,py=xx-a[0],yy-a[1]; t=np.clip((px*vx+py*vy)/(vx*vx+vy*vy),0,1)
    d=np.hypot(px-t*vx,py-t*vy); core=np.clip((w*.5-d)/max(1,w*.15)+.5,0,1)
    inner=np.exp(-(d/(w*1.3))**2)*glow; outer=np.exp(-(d/(w*4.5))**2)*.55*glow
    c=np.array(col)/255; L=np.clip((core[...,None]+(inner+outer)[...,None]*c)*br,0,1)
    return 1-(1-img)*(1-L)
img=saber(bg,(200,560),(560,120),(40,120,255)); img=saber(img,(880,560),(560,140),(255,30,30)); img=saber(img,(520,600),(980,420),(40,230,60),wf=7)
Image.fromarray((img*255).astype('uint8')).save('/tmp/saber.png')
