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

V3 screenSpaceInterpolate(V3 v1, V3 v2, V3 v3, V3 r) {
    M33 m;
    m.setColumn(0, v1);
    m.setColumn(1, v2);
    m.setColumn(2, V3(1));
    return m.inverse() * r;
}

void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, 
        unsigned int color, V3 L, float ka, float se, int renderMode) {

    V3 *pverts = new V3[vertsN];
    /*
    for (int vi = 0; vi < vertsN; vi++) {
        ppc->Project(verts[vi], pverts[vi]);
    }
    */

    for (int tri = 0; tri < trisN; tri++) {
        int vinds[3];
        vinds[0] = tris[tri*3+0];
        vinds[1] = tris[tri*3+1];
        vinds[2] = tris[tri*3+2];

        // Do not render triangle if any of its vertices had an invalid projection
        if (!ppc->Project(verts[vinds[0]], pverts[vinds[0]]) ||
            !ppc->Project(verts[vinds[1]], pverts[vinds[1]]) ||
            !ppc->Project(verts[vinds[2]], pverts[vinds[2]])) {
            continue;
        }

        //Compute bounding box of projected vertices
        AABB bbox(pverts[vinds[0]]);
        bbox.AddPoint(pverts[vinds[1]]);
        bbox.AddPoint(pverts[vinds[2]]);

        //Setup edge equations from pverts[vinds]
        V3 a, b, c;
        float sidedness;
        {
            //Edge 0-1
            a[0] = pverts[vinds[1]][1]-pverts[vinds[0]][1]; 
            b[0] = -pverts[vinds[1]][0] + pverts[vinds[0]][0]; 
            c[0] = -pverts[vinds[0]][0]*pverts[vinds[1]][1] + pverts[vinds[0]][1]*pverts[vinds[1]][0];
            sidedness = a[0]*pverts[vinds[2]][0] + b[0]*pverts[vinds[2]][1] + c[0];
            if (sidedness < 0) {
                a[0] = -a[0]; b[0] = -b[0]; c[0] = -c[0];
            }

            //Edge 1-2
            a[1] = pverts[vinds[2]][1]-pverts[vinds[1]][1]; 
            b[1] = -pverts[vinds[2]][0] + pverts[vinds[1]][0]; 
            c[1] = -pverts[vinds[1]][0]*pverts[vinds[2]][1] + pverts[vinds[1]][1]*pverts[vinds[2]][0];
            sidedness = a[1]*pverts[vinds[0]][0] + b[1]*pverts[vinds[0]][1] + c[1];
            if (sidedness < 0) {
                a[1] = -a[1]; b[1] = -b[1]; c[1] = -c[1];
            }

            //Edge 2-0
            a[2] = pverts[vinds[0]][1]-pverts[vinds[2]][1]; 
            b[2] = -pverts[vinds[0]][0] + pverts[vinds[2]][0]; 
            c[2] = -pverts[vinds[2]][0]*pverts[vinds[0]][1] + pverts[vinds[2]][1]*pverts[vinds[0]][0];
            sidedness = a[2]*pverts[vinds[1]][0] + b[2]*pverts[vinds[1]][1] + c[2];
            if (sidedness < 0) {
                a[2] = -a[2]; b[2] = -b[2]; c[2] = -c[2];
            }
        }

        //Setup linear variation of depth
        V3 zr(pverts[vinds[0]][2], pverts[vinds[1]][2], 
              pverts[vinds[2]][2]);
        V3 zABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], zr);

        //Extract rgb of verts
        V3 rr, gr, br;
        rr[0] = ((cols[vinds[0]]).getColor() >> 16) & 0xff;
        gr[0] = ((cols[vinds[0]]).getColor() >> 8) & 0xff;
        br[0] = (cols[vinds[0]]).getColor() & 0xff;
        rr[1] = ((cols[vinds[1]]).getColor() >> 16) & 0xff;
        gr[1] = ((cols[vinds[1]]).getColor() >> 8) & 0xff;
        br[1] = (cols[vinds[1]]).getColor() & 0xff;
        rr[2] = ((cols[vinds[2]]).getColor() >> 16) & 0xff;
        gr[2] = ((cols[vinds[2]]).getColor() >> 8) & 0xff;
        br[2] = (cols[vinds[2]]).getColor() & 0xff;

        //Setup linear of red
        V3 redABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], rr);
            
        //Setup linear of green 
        V3 greenABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], gr);

        //Setup linear of blue 
        V3 blueABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], br);

        int left = (int) (bbox[0][0] + .5); 
        int right = (int) (bbox[0][1] - .5); 
        int top= (int) (bbox[1][0] +.5); 
        int bottom = (int) (bbox[1][1] - .5);

        int u, v; // current pixel considered
        V3 currEELS; // edge expression values for line starts
        V3 currEE; //  within line
        V3 pv; //pixel vector
        for(v = top; v <= bottom; v++) 
            for(u = left; u <= right; u++) {
                pv[0] = u + 0.5f;
                pv[1] = v + 0.5f;
                pv[2] = 1.0f;

                // Check if pixel is inside triangle
                //if (pv*a < 0 || pv*b < 0 || pv*c < 0) 
                    //continue;

                // Check if triangle is closer than previously seen
                float currz = zABC * pv;
                if (fb->IsFarther(u, v, currz))
                    continue;
                fb->SetZ(u, v, currz);

                // From model file
                if (renderMode == 0) {
                }

                //Gourad shading
                else if (renderMode == 1) {

                    unsigned int ssiRed = redABC * pv;
                    unsigned int ssiGreen = greenABC * pv;
                    unsigned int ssiBlue = blueABC * pv;
                    unsigned int rgb = ssiRed << 16 |
                                       ssiGreen << 8 | 
                                       ssiBlue;
                    fb->Set(u, v, rgb);
                }

                //From lightning expression per pixel
                else {
                }
            }
    }

    delete []pverts;

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
//      V3 pp((float)u+0.5f, (float)v+0.5f, fb->zb[uv]);
      V3 pp((float)u+0.5f, (float)v+0.5f, z0);
      verts[uv] = ppc->UnProject(pp);
      cols[uv].setFromColor(fb->Get(u, v));
    }
  }

}

