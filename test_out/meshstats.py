#!/usr/bin/env python3
"""Topology stats for an OBJ polygon soup: boundary/non-manifold edges, components.

Usage: meshstats.py file.obj [file2.obj ...]
Components are reported both edge-based (2 faces share an edge) and vertex-based.
"""
import sys
from collections import Counter, defaultdict


def load_faces(path):
    verts, faces = [], []
    with open(path) as fh:
        for line in fh:
            if line.startswith("v "):
                verts.append(1)
            elif line.startswith("f "):
                idx = [int(t.split("/")[0]) - 1 for t in line.split()[1:]]
                faces.append(idx)
    return verts, faces


def stats(path):
    verts, faces = load_faces(path)
    edge_faces = defaultdict(list)
    for fi, f in enumerate(faces):
        n = len(f)
        for i in range(n):
            a, b = f[i], f[(i + 1) % n]
            if a == b:
                continue
            edge_faces[(min(a, b), max(a, b))].append(fi)
    cnt = Counter(len(v) for v in edge_faces.values())
    boundary = cnt[1]
    nonman = sum(v for k, v in cnt.items() if k > 2)

    # edge-based components (union-find over faces)
    parent = list(range(len(faces)))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    for fl in edge_faces.values():
        for g in fl[1:]:
            a, b = find(fl[0]), find(g)
            if a != b:
                parent[b] = a
    comps = len({find(i) for i in range(len(faces))})
    sizes = Counter(find(i) for i in range(len(faces)))
    big = sizes.most_common(3)

    arity = Counter(len(f) for f in faces)
    return dict(verts=len(verts), polys=len(faces), arity=dict(arity),
                edges=len(edge_faces), boundary=boundary,
                boundary_pct=100.0 * boundary / max(1, len(edge_faces)),
                nonmanifold=nonman, components=comps,
                biggest_components=big)


def main():
    for p in sys.argv[1:]:
        s = stats(p)
        print(f"{p}")
        print(f"  verts={s['verts']} polys={s['polys']} arity={s['arity']}")
        print(f"  edges={s['edges']} boundary={s['boundary']} ({s['boundary_pct']:.2f}%) "
              f"nonmanifold={s['nonmanifold']}")
        print(f"  components(edge-based)={s['components']} biggest={s['biggest_components']}")


if __name__ == "__main__":
    main()
