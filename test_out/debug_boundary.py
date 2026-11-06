import sys
import numpy as np

def inspect(obj_path):
    verts, faces = [], []
    with open(obj_path) as f:
        for line in f:
            if line.startswith("v "):
                verts.append([float(x) for x in line.split()[1:4]])
            elif line.startswith("f "):
                idx = [int(tok.split("/")[0]) - 1 for tok in line.split()[1:]]
                faces.append(idx)
    
    edge_faces = {}
    for fi, f in enumerate(faces):
        m = len(f)
        for i in range(m):
            u = f[i]
            v = f[(i + 1) % m]
            edge = (min(u, v), max(u, v))
            edge_faces.setdefault(edge, []).append((fi, u, v))
            
    bnd_edges = [e for e, fl in edge_faces.items() if len(fl) == 1]
    print(f"Total verts: {len(verts)}, faces: {len(faces)}")
    print(f"Total undirected edges: {len(edge_faces)}")
    print(f"Boundary edges (len == 1): {len(bnd_edges)}")
    
    # Check if boundary edges share vertices with duplicate positions (unwelded duplicates)
    V = np.array(verts)
    bnd_u = [e[0] for e in bnd_edges]
    bnd_v = [e[1] for e in bnd_edges]
    bnd_verts = list(set(bnd_u + bnd_v))
    print(f"Vertices on boundary edges: {len(bnd_verts)}")
    
    # Check the faces that have boundary edges: how many boundary edges per face?
    face_bnd_count = {}
    for e in bnd_edges:
        fi = edge_faces[e][0][0]
        face_bnd_count[fi] = face_bnd_count.get(fi, 0) + 1
        
    counts = {}
    for fi, c in face_bnd_count.items():
        counts[c] = counts.get(c, 0) + 1
    print(f"Faces with boundary edges breakdown (bnd_edges_in_face: num_faces): {counts}")

    # For faces with 4 boundary edges:
    print("\nSample faces with 4 boundary edges:")
    for fi, c in face_bnd_count.items():
        if c == 4:
            print(f"Face {fi}: verts={faces[fi]}")
            for vi in faces[fi]:
                print(f"  v{vi}: {V[vi]}")
            break

    # For faces with 2 boundary edges:
    print("\nSample face with 2 boundary edges:")
    for fi, c in face_bnd_count.items():
        if c == 2:
            print(f"Face {fi}: verts={faces[fi]}")
            for vi in faces[fi]:
                print(f"  v{vi}: {V[vi]}")
            break

if __name__ == "__main__":
    inspect("test_out/test_run.obj")
