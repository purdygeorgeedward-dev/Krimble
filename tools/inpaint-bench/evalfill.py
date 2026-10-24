import subprocess, sys, time, numpy as np, cv2, os
os.chdir('/home/claude/kr/tools/inpaint-bench')
def load(p):
    return cv2.imread(p, cv2.IMREAD_UNCHANGED)
def lapvar(g, m):
    l = cv2.Laplacian(g, cv2.CV_32F)
    return float(l[m].var()) if m.sum() > 50 else float('nan')
def run(binp, name, outdir):
    inp, mk = f'{name}_in.ppm', f'{name}_mask.pgm'
    out = f'{outdir}/{name}.ppm'
    t = time.time()
    r = subprocess.run([binp, inp, mk, out], capture_output=True, text=True)
    dt = time.time() - t
    img = cv2.imread(inp); res = cv2.imread(out); mask = cv2.imread(mk, 0) > 127
    g0 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY).astype(np.float32); g1 = cv2.cvtColor(res, cv2.COLOR_BGR2GRAY).astype(np.float32)
    ring = cv2.dilate(mask.astype(np.uint8), np.ones((61, 61), np.uint8)).astype(bool) & ~cv2.dilate(mask.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool)
    sh = lapvar(g1, mask) / max(lapvar(g0, ring), 1e-6)
    return dt, sh, int(mask.sum())
if __name__ == '__main__':
    binp, outdir = sys.argv[1], sys.argv[2]
    os.makedirs(outdir, exist_ok=True)
    names = sys.argv[3:] or ['circus_v2','face_v2','catfur_t2','w','spoon_t1','flag_t1','moto_t1','tripod_t1','grass5','brick5','coffee_v1','rocket_t1']
    for n in names:
        try:
            dt, sh, hs = run(binp, n, outdir)
            print(f'{n:12s} hole={hs:7d}px  {dt:5.1f}s  sharpness(hole/ring)={sh:5.2f}', flush=True)
        except Exception as e:
            print(n, 'error', e)
