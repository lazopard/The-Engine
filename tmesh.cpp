#include "tmesh.h"

#include <fstream>
#include <iostream>
#include <cmath>
#include <cfloat>

TMesh::TMesh() {
    verts = 0;
    vertsN = 0;
    tris = 0;
    trisN = 0;
    normals = 0;
    cols = 0;
    ss = 0;
    ts = 0;
    aabb = 0;
    enabled = false;
}

TMesh::TMesh(V3 center, V3 dims, unsigned int color) {

    enabled = true;
    vertsN = 8;
    verts = new V3[vertsN];
    cols = new V3[vertsN];
    normals = 0;

    int vi = 0;
    verts[vi++] = center + V3(-dims[0]/2.0f, +dims[1]/2.0f, +dims[2]/2.0f);
    verts[vi++] = center + V3(-dims[0]/2.0f, -dims[1]/2.0f, +dims[2]/2.0f);
    verts[vi++] = center + V3(+dims[0]/2.0f, -dims[1]/2.0f, +dims[2]/2.0f);
    verts[vi++] = center + V3(+dims[0]/2.0f, +dims[1]/2.0f, +dims[2]/2.0f);

    verts[vi++] = center + V3(-dims[0]/2.0f, +dims[1]/2.0f, -dims[2]/2.0f);
    verts[vi++] = center + V3(-dims[0]/2.0f, -dims[1]/2.0f, -dims[2]/2.0f);
    verts[vi++] = center + V3(+dims[0]/2.0f, -dims[1]/2.0f, -dims[2]/2.0f);
    verts[vi++] = center + V3(+dims[0]/2.0f, +dims[1]/2.0f, -dims[2]/2.0f);

    for (int vi = 0; vi < vertsN; vi++) {
        cols[vi].setFromColor(color);
    }

    trisN = 12;
    tris = new unsigned int[trisN * 3];
    int tri = 0;

    tris[3*tri+0] = 0;
    tris[3*tri+1] = 1;
    tris[3*tri+2] = 2;
    tri++;
    tris[3*tri+0] = 2;
    tris[3*tri+1] = 3;
    tris[3*tri+2] = 0;
    tri++;
    tris[3*tri+0] = 3;
    tris[3*tri+1] = 2;
    tris[3*tri+2] = 6;
    tri++;
    tris[3*tri+0] = 6;
    tris[3*tri+1] = 7;
    tris[3*tri+2] = 3;
    tri++;
    tris[3*tri+0] = 4;
    tris[3*tri+1] = 0;
    tris[3*tri+2] = 3;
    tri++;
    tris[3*tri+0] = 3;
    tris[3*tri+1] = 7;
    tris[3*tri+2] = 4;
    tri++;

    tris[3*tri+0] = 5;
    tris[3*tri+1] = 6;
    tris[3*tri+2] = 2;
    tri++;
    tris[3*tri+0] = 2;
    tris[3*tri+1] = 1;
    tris[3*tri+2] = 5;
    tri++;

    tris[3*tri+0] = 4;
    tris[3*tri+1] = 7;
    tris[3*tri+2] = 6;
    tri++;
    tris[3*tri+0] = 6;
    tris[3*tri+1] = 5;
    tris[3*tri+2] = 4;
    tri++;

    tris[3*tri+0] = 4;
    tris[3*tri+1] = 5;
    tris[3*tri+2] = 1;
    tri++;
    tris[3*tri+0] = 1;
    tris[3*tri+1] = 0;
    tris[3*tri+2] = 4;
    tri++;

}

void TMesh::RenderPoints(PPC *ppc, FrameBuffer *fb, int psize) {

    for (int vi = 0; vi < vertsN; vi++) {
        if (verts[vi][0] == FLT_MAX)
            continue;
        fb->Draw3DPoint(verts[vi], ppc, psize, cols[vi]);
    }

}

void TMesh::RenderWireframe(PPC *ppc, FrameBuffer *fb, unsigned int color) {


    for (int tri = 0; tri < trisN; tri++) {
        int vinds[3];
        vinds[0] = tris[tri*3+0];
        vinds[1] = tris[tri*3+1];
        vinds[2] = tris[tri*3+2];
        for (int vi = 0; vi < 3; vi++) {
            fb->Draw3DSegment(verts[vinds[vi]], verts[vinds[(vi+1)%3]], ppc,
                    color);
        }
    }

}

/*
   void TMesh::RenderWireframe(PPC *ppc, FrameBuffer *fb, unsigned int color) {


   for (int tri = 0; tri < trisN; tri++) {
   int vinds[3];
   vinds[0] = tris[tri*3+0];
   vinds[1] = tris[tri*3+1];
   vinds[2] = tris[tri*3+2];
   for (int vi = 0; vi < 3; vi++) {
   V3 col0, col1;
   if (cols) {
   col0 = cols[vinds[vi]];
   col1 = cols[vinds[(vi+1)%3]];
   }
   else {
   col0.SetFromColor(color);
   col1.SetFromColor(color);
   }
   fb->Draw3DSegment(verts[vinds[vi]], verts[vinds[(vi+1)%3]], ppc,
   col0, col1);
   }
   }

   }
   */

inline V3 screenSpaceInterpolate(V3 v1, V3 v2, V3 v3, V3 r) {
    M33 m;
    m.setColumn(0, v1);
    m.setColumn(1, v2);
    m.setColumn(2, V3(1));
    return m.inverse() * r;
}

void TMesh::SetEEQS(V3 pv0, V3 pv1, V3 pv2, V3 *eeqs) {

    V3 pvs[3];
    pvs[0] = pv0;
    pvs[1] = pv1;
    pvs[2] = pv2;
    for (int i = 0; i < 3; i++) {
        int i1 = (i+1)%3;
        int i2 = (i+2)%3;
        eeqs[i][0] = -pvs[i][1] + pvs[i1][1]; // -Ay + By; 
        eeqs[i][1] = pvs[i][0] - pvs[i1][0]; // Ax - Bx; 
        eeqs[i][2] = -pvs[i][0]*eeqs[i][0] - pvs[i][1]*eeqs[i][1];

        // plug in third vertex, flip edge equation if negative
        V3 v2(pvs[i2][0], pvs[i2][1], 1.0f);
        if (eeqs[i]*v2 < 0.0f)
            eeqs[i] = eeqs[i] * -1.0f;
    }
}

void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, 
        unsigned int color, V3 L, 
        float ka, float se, int renderMode) {

    V3 *pverts = new V3[vertsN];
    for (int vi = 0; vi < vertsN; vi++) {
        ppc->Project(verts[vi], pverts[vi]);
    }

    for (int tri = 0; tri < trisN; tri++) {
        int vinds[3];
        vinds[0] = tris[tri*3+0];
        vinds[1] = tris[tri*3+1];
        vinds[2] = tris[tri*3+2];

        // Do not render triangle if any of its vertices had an invalid projection
        if (pverts[vinds[0]][0] == FLT_MAX ||
                pverts[vinds[1]][0] == FLT_MAX ||
                pverts[vinds[2]][0] == FLT_MAX)
            continue;

        // Compute bounding box aabb of projected vertices
        AABB aabb(pverts[vinds[0]]);
        aabb.AddPoint(pverts[vinds[1]]);
        aabb.AddPoint(pverts[vinds[2]]);

        // Clip aabb with frame
        aabb.Clip(0.0f, (float) fb->w, 0.0f, (float) fb->h);

        // Setup edge equations ee0, ee1, ee2
        V3 eeqs[3];
        SetEEQS(pverts[vinds[0]], pverts[vinds[1]], pverts[vinds[2]], eeqs);

        M33 ptm;
        ptm[0] = pverts[vinds[0]];
        ptm[1] = pverts[vinds[1]];
        ptm[2] = pverts[vinds[2]];
        ptm.setColumn(2, V3(1.0f, 1.0f, 1.0f));
        ptm = ptm.inverse();

        // Setup screen space linear variation of depth: zABC 
        V3 zABC = ptm*V3(pverts[vinds[0]][2],
                pverts[vinds[1]][2], pverts[vinds[2]][2]);

        // Setup screen space linear variation of red: redABC 
        V3 redABC = ptm*V3(cols[vinds[0]][0],
                cols[vinds[1]][0], cols[vinds[2]][0]);

        // Setup screen space linear variation of red: greenABC 
        V3 greenABC = ptm*V3(cols[vinds[0]][1],
                cols[vinds[1]][1], cols[vinds[2]][1]);

        // Setup screen space linear variation of red: blueABC 
        V3 blueABC = ptm*V3(cols[vinds[0]][2],
                cols[vinds[1]][2], cols[vinds[2]][2]);

        // Setup screen space linear variation of normals
        V3 nxABC = ptm*V3(normals[vinds[0]][0],
                normals[vinds[1]][0], normals[vinds[2]][0]);
        V3 nyABC = ptm*V3(normals[vinds[0]][1],
                normals[vinds[1]][1], normals[vinds[2]][1]);
        V3 nzABC = ptm*V3(normals[vinds[0]][2],
                normals[vinds[1]][2], normals[vinds[2]][2]);

        int top = (int) (aabb.corners[0][1] + 0.5f);
        int bottom = (int) (aabb.corners[1][1] - 0.5f);
        int left = (int) (aabb.corners[0][0] + 0.5f);
        int right = (int) (aabb.corners[1][0] - 0.5f);
        for (int v = top; v <= bottom; v++) {
            for (int u = left; u <= right; u++) {

                V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);

                if (eeqs[0]*pixv < 0.0f ||
                        eeqs[1]*pixv < 0.0f ||
                        eeqs[2]*pixv < 0.0f)
                    continue;

                float currz = zABC * pixv;
                if (fb->IsFarther(u, v, currz))
                    continue;
                fb->SetZ(u, v, currz);

                V3 currColor;

                // vertex color interpolation from model file
                if (renderMode == 0) { 
                    currColor = V3(redABC * pixv, greenABC * pixv, blueABC * pixv);
                }

                // lightning expression per vertex
                else if (renderMode == 1)  { 

                }

                // lightning expression per pixel by vertex normals
                else if (renderMode == 2) {
                    V3 fullColor;
                    fullColor.setFromColor(color);
                    V3 currNormal, lv;
                    V3 pp(pixv);
                    pp[2] = currz;
                    lv = (L - ppc->UnProject(pp)).normalize();
                    currNormal = V3(nxABC*pixv, nyABC*pixv, nzABC*pixv).normalize();
                    float kd = lv * currNormal;
                    kd = (kd < 0.0f) ? 0.0f : kd;
                    currColor = fullColor * (ka + (1.0f-ka)*kd);
                }
                fb->Set(u, v, currColor.getColor());
            }
        }
    }

    delete []pverts;

}

void TMesh::mapToST(unsigned int *texture, int w, int h) {
}

int *mirrorTexture(unsigned int *texture, int w, int h) {
    return NULL;
}

void TMesh::RenderTM(PPC *ppc, FrameBuffer *fb, unsigned int *texture, 
        int w, int h, int tm_mode, int tl_mode) {
}

void TMesh::LoadBin(const char *fname) {

    ifstream ifs(fname, ios::binary);
    if (ifs.fail()) {
        cerr << "INFO: cannot open file: " << fname << endl;
        return;
    }

    ifs.read((char*)&vertsN, sizeof(int));
    char yn;
    ifs.read(&yn, 1); // always xyz
    if (yn != 'y') {
        cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
        return;
    }
    if (verts)
        delete verts;
    verts = new V3[vertsN];

    ifs.read(&yn, 1); // cols 3 floats
    if (cols)
        delete cols;
    cols = 0;
    if (yn == 'y') {
        cols = new V3[vertsN];
    }
    ifs.read(&yn, 1); // normals 3 floats
    if (normals)
        delete normals;
    normals = 0;
    if (yn == 'y') {
        normals = new V3[vertsN];
    }

    ifs.read(&yn, 1); // texture coordinates 2 floats
    float *tcs = 0; // don't have texture coordinates for now
    if (tcs)
        delete tcs;
    tcs = 0;
    if (yn == 'y') {
        tcs = new float[vertsN*2];
    }

    ifs.read((char*)verts, vertsN*3*sizeof(float)); // load verts

    if (cols) {
        ifs.read((char*)cols, vertsN*3*sizeof(float)); // load cols
    }

    if (normals)
        ifs.read((char*)normals, vertsN*3*sizeof(float)); // load normals

    if (tcs)
        ifs.read((char*)tcs, vertsN*2*sizeof(float)); // load texture coordinates

    ifs.read((char*)&trisN, sizeof(int));
    if (tris)
        delete tris;
    tris = new unsigned int[trisN*3];
    ifs.read((char*)tris, trisN*3*sizeof(unsigned int)); // read tiangles

    ifs.close();

    SetAABB();

    enabled = true;
}

void TMesh::SetAABB() {

    if (aabb) {
        delete aabb;
    }

    aabb = new AABB(verts[0]);

    for (int vi = 0; vi < vertsN; vi++)
        aabb->AddPoint(verts[vi]);
}

void TMesh::Translate(V3 tv) {

    for (int vi = 0; vi < vertsN; vi++)
        verts[vi] = verts[vi] + tv;
    SetAABB();
}

V3 TMesh::GetCenter() {

    V3 ret = (aabb->corners[0] + aabb->corners[1])/2.0f;
    return ret;
}

void TMesh::Position(V3 newCenter) {

    V3 oldCenter = GetCenter();
    Translate(newCenter-oldCenter);
    SetAABB();
}

void TMesh::ScaleToNewDiagonal(float newDiagonal) {

    float oldDiagonal = (aabb->corners[1] - aabb->corners[0]).length();
    float sf = newDiagonal / oldDiagonal;
    V3 oldCenter = GetCenter();
    Position(V3(0.0f, 0.0f, 0.0f));
    Scale(sf);
    Position(oldCenter);
    SetAABB();
}

void TMesh::Scale(float scf) {

    for (int vi = 0; vi < vertsN; vi++) {
        verts[vi] = verts[vi] * scf;
    }

    SetAABB();
}

void TMesh::SetFromFB(FrameBuffer *fb, PPC *ppc) {

    vertsN = fb->w*fb->h;
    verts = new V3[vertsN];
    cols = new V3[vertsN];

    float z0 = ppc->GetF() / 100.0f;

    for (int v = 0; v < fb->h; v++) {
        for (int u = 0; u < fb->w; u++) {
            int uv = (fb->h-1-v)*fb->w+u;
            if (fb->zb[uv] == 0.0f) {
                verts[uv] = V3(FLT_MAX, FLT_MAX, FLT_MAX);
                cols[uv].setFromColor(fb->Get(u, v));
                continue;
            }
            //V3 pp((float)u+0.5f, (float)v+0.5f, fb->zb[uv]);
            V3 pp((float)u+0.5f, (float)v+0.5f, z0);
            verts[uv] = ppc->UnProject(pp);
            cols[uv].setFromColor(fb->Get(u, v));
        }
    }

}

