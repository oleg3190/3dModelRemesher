import sys, numpy as np
def load_obj(p):
    V=[];F=[]
    for ln in open(p):
        if ln.startswith("v "): a=ln.split(); V.append([float(a[1]),float(a[2]),float(a[3])])
        elif ln.startswith("f "): F.append([int(t.split('/')[0])-1 for t in ln.split()[1:]])
    return np.array(V),F
pos,faces=load_obj(sys.argv[1])
Q=[f for f in faces if len(f)==4]
def analyze(q):
    p=[pos[i] for i in q]
    p0,p1,p2,p3=p
    n=np.cross(p2-p0,p3-p1); ln=np.linalg.norm(n)
    if ln<1e-9: return None
    n/=ln
    # WARP: max out-of-plane deviation of corners / mean edge
    dev=[abs(np.dot(pi-p0,n)) for pi in p]; dev[0]=0
    # project to best-fit plane -> 2D
    u=(p1-p0); u-=np.dot(u,n)*n; un=np.linalg.norm(u); u=u/(un+1e-12)
    v=np.cross(n,u)
    P2=np.array([np.array([np.dot(pi-p0,u),np.dot(pi-p0,v)]) for pi in p])
    # angles (skew): interior angles of planar quad
    ang=[]
    for i in range(4):
        a=P2[i-1]-P2[i]; b=P2[(i+1)%4]-P2[i]
        c=np.dot(a,b)/(np.linalg.norm(a)*np.linalg.norm(b)+1e-12)
        ang.append(np.degrees(np.arccos(np.clip(c,-1,1))))
    ang=np.array(ang)
    skew=180-np.abs(ang-90).max()*0  # measure: max deviation from 90
    angle_pen=np.abs(ang-90).max()
    # planar-shape scaled jacobian (2D analogue): edge-length ratios & orthogonality
    e=np.linalg.norm(P2-np.roll(P2,1,axis=0),axis=1)
    edgelen_ratio=e.max()/(e.min()+1e-12)
    return float(max(dev)/(un+1e-12)), angle_pen, edgelen_ratio
W=[];A=[];E=[]
for q in Q:
    r=analyze(q)
    if r: W.append(r[0]);A.append(r[1]);E.append(r[2])
W=np.array(W);A=np.array(A);E=np.array(E)
print(f"quads={len(W)}")
print(f"WARP (oob dev/edge): median={np.median(W):.3f} p90={np.percentile(W,90):.3f} >0.15%={100*(W>0.15).mean():.1f}")
print(f"ANGLE dev from 90: median={np.median(A):.1f} p90={np.percentile(A,90):.1f}  >20deg={100*(A>20).mean():.1f}%  >35deg={100*(A>35).mean():.1f}%")
print(f"EDGELEN ratio: median={np.median(E):.2f} p90={np.percentile(E,90):.2f} >2={100*(E>2).mean():.1f}%")
