import sys, numpy as np
def load_obj(p):
    V=[];F=[]
    with open(p) as f:
        for ln in f:
            if ln.startswith("v "):
                a=ln.split(); V.append([float(a[1]),float(a[2]),float(a[3])])
            elif ln.startswith("f "):
                idx=[int(t.split('/')[0])-1 for t in ln.split()[1:]]
                F.append(idx)
    return np.array(V,float),F
def sbj(pos,q):
    p0,p1,p2,p3=[pos[i] for i in q]
    n=np.cross(p2-p0,p3-p1); ln=np.linalg.norm(n)
    if ln<1e-8: return -1.0
    n/=ln
    pts=[p0,p1,p2,p3]; mj=1.0
    for i in range(4):
        e1=pts[(i+1)%4]-pts[i]; e2=pts[(i+3)%4]-pts[i]
        l1=np.linalg.norm(e1); l2=np.linalg.norm(e2)
        if l1<1e-8 or l2<1e-8: return -1.0
        c=np.cross(e1/l1,e2/l2); mj=min(mj,float(np.dot(c,n)))
    return mj
f=sys.argv[1]; pos,faces=load_obj(f)
quads=[q for q in faces if len(q)==4]
vals=np.array([sbj(pos,q) for q in quads])
print(f"{f}: quads={len(quads)} tri={len(faces)-len(quads)} folded<0={int((vals<0).sum())} ({100*(vals<0).mean():.2f}%) <0.5={int((vals<0.5).sum())} ({100*(vals<0.5).mean():.1f}%) min={vals.min():.3f} p05={np.percentile(vals,5):.3f} p25={np.percentile(vals,25):.3f} median={np.median(vals):.3f} mean={vals.mean():.3f}")
