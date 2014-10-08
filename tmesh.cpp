#include "tmesh.h"

#include <fstream>
#include <iostream>

TMesh::TMesh() {
    verts = 0;
    vertsN = 0;
    tris = 0;
    trisN = 0;
    normals = 0;
    cols = 0;
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
    //float Length();
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

void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, 
        unsigned int color, V3 L, float ka, int renderMode) {

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
        V3 ppt0;
        V3 ppt1;
        V3 ppt2;
        if (!ppc->Project(pverts[vinds[0]], ppt0) ||
            !ppc->Project(pverts[vinds[1]], ppt1) ||
            !ppc->Project(pverts[vinds[2]], ppt2))
            continue;

        // Compute bounding box aabb of projected vertices
        AABB bbox(pverts[vinds[0]]);
        bbox.AddPoint(pverts[vinds[1]]);
        bbox.AddPoint(pverts[vinds[2]]);

        // Clip aabb with frame
        //clipAABB();

        // Setup edge equations a, b, c
        V3 x(pverts[vinds[0]][0], pverts[vinds[1]][0], pverts[vinds[2]][0]);
        V3 y(pverts[vinds[0]][1], pverts[vinds[1]][1], pverts[vinds[2]][1]);
        V3 z(pverts[vinds[0]][2], pverts[vinds[1]][2], pverts[vinds[2]][2]);

        V3 a, b, c; // a, b, c for the 3 edge expressions

        // edge that goes through vertices 0 and 1
        a[0] = y[1]-y[0]; b[0] = -x[1] + x[0]; c[0] = -x[0]*y[1] + y[0]*x[1];
        float sidedness; 
        sidedness = a[0]*x[2] + b[0]*y[2] + c[0];
        if (sidedness < 0)
           a[0] = -a[0]; b[0] = -b[0]; c[0] = -c[0]; 

        // edge that goes through vertices 1 and 2
        a[1] = y[2]-y[1]; b[1] = -x[2] + x[1]; c[1] = -x[1]*y[2] + y[1]*x[2];
        sidedness = a[1]*x[2] + b[1]*y[2] + c[1];
        if (sidedness < 1)
           a[1] = -a[1]; b[1] = -b[1]; c[1] = -c[1];

        // edge that goes through vertices 2 and 0
        a[2] = y[1]-y[2]; b[2] = -x[1] + x[2]; c[2] = -x[2]*y[1] + y[2]*x[1];
        sidedness = a[2]*x[2] + b[2]*y[2] + c[2];
        if (sidedness < 2)
           a[2] = -a[2]; b[2] = -b[2]; c[2] = -c[2];

        // Setup screen space linear variation of depth: zABC 

        // Setup screen space linear variation of red: redABC 

        // Setup screen space linear variation of green: greenABC 

        // Setup screen space linear variation of blue: blueABC 

        int u, v;
        int left = (int) (bbox[0][0] + .5), right = (int) (bbox[0][1] - .5); 
        int top= (int) (bbox[1][0] +.5), bottom = (int) (bbox[1][1] - .5);

        for(v = top; v <= bottom; v++) {
            for (u = left; u <= right; u++) {

                // Current pixel is p(u, v)
                unsigned int p = fb->Get(u,v);

                // Current pixel vector is pv(u+0.5, v+0.5, 1.0)
                V3 pv(u+0.5f, v+0.5f, 1.0f);

                if (pv*a < 0 || pv*b || pv*c)
                    continue;

                /*
                float currz = zABC * pv
                if (currz < fb->zb[p])
                    continue;
                fb->zb[p] = currz;
                */

                //rendering mode is vertex color interpolation
                if (renderMode == 0) {
                      //ssiRed = redABC * pv
                      //ssiGreen = greenABC * pv
                      //ssiBlue = blueABC * pv
                      //FB[p] = (ssiRed, ssiGreen, ssiB)
                }

                //rendering mode is per pixel diffuse lighting
                if (renderMode == 1) {
                //      n.x = nxABC * pv
                //      n.y = nyABC * pv
                //      n.z = nzABC * pv
                //      normalize n
                //      n = n.normalize();
                //      surface point at current pixel P
                //      P = ppc->Unproject(u+0.5, v+0.5, currz)
                //      lv = (L-P).normalized()
                //      kd = lv * n; kd = (kd < 0) ? 0 : kd;
                //      FB[p] = color * (ka + (1-ka)*kd);
                }
            }
        }
    }

    delete []pverts;
}

