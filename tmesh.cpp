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
    tcs = 0;
    texture = 0;
    texID = -1;
    aabb = 0;
    enabled = false;
}

TMesh::TMesh(V3 center, V3 dims, unsigned int color) {

    enabled = true;
    vertsN = 8;
    verts = new V3[vertsN];
    cols = new V3[vertsN];
    normals = 0;
    tcs = 0;
    texture = 0;
    texID = -1;
        
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

TMesh::TMesh(V3 *vs, V3 *colors) {
        verts = 0; vertsN = 0; tris = 0; trisN = 0;
        normals = 0; cols = 0; aabb = 0; enabled = false; tcs = 0; texture = 0; texID = -1;

        vertsN = 4;
        verts = new V3[vertsN];
        cols = new V3[vertsN];
        normals = new V3[vertsN];
        tcs = new float[2*vertsN];

        trisN = 2;
        tris = new unsigned int[trisN*3];
        int i = 0;
        tris[i++] = 0;
        tris[i++] = 1;
        tris[i++] = 2;
        tris[i++] = 2;
        tris[i++] = 3;
        tris[i++] = 0;

        V3 n = (vs[1]-vs[0])%(vs[2]-vs[0]).normalize();
        for (int i = 0; i < vertsN; i++) {
            verts[i] = vs[i];
            cols[i] = colors[i];
            normals[i] = n;
        }

        tcs[0] = 0.0f;
        tcs[1] = 0.0f;
        tcs[2] = 0.0f;
        tcs[3] = 1.0f;
        tcs[4] = 1.0f;
        tcs[5] = 1.0f;
        tcs[6] = 1.0f;
        tcs[7] = 0.0f;

        enabled = true;
        SetAABB();
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
                    cols[vinds[vi]].getColor());
        }
    }
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

M33 TMesh::GetMSIQ(V3 v0, V3 v1, V3 v2, PPC *ppc) {

    M33 camM;
    camM.setColumn(0, ppc->a);
    camM.setColumn(1, ppc->b);
    camM.setColumn(2, ppc->c);
    M33 vmcM;
    vmcM.setColumn(0, v0-ppc->C);
    vmcM.setColumn(1, v1-ppc->C);
    vmcM.setColumn(2, v2-ppc->C);

    M33 Q = vmcM.inverse()*camM;
    return Q;
}

void TMesh::AddTexture(FrameBuffer *tex) {
    //texture = new FrameBuffer(0, 0, tex->w, tex->h);
    texture = new FrameBuffer(tex->pix, 0, 0, tex->w, tex->h);
}

void TMesh::AddTextureHW(unsigned int tname) {
    texID = tname;
}

void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, unsigned int color, V3 L, 
                         float ka, float se, int tm, int tl) 
{

    //Project all vertices
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
        if (!aabb.Clip(0.0f, (float) fb->w, 0.0f, (float) fb->h)) {
            continue;
        }

        // Setup edge equations ee0, ee1, ee2
        V3 eeqs[3];
        SetEEQS(pverts[vinds[0]], pverts[vinds[1]], pverts[vinds[2]], eeqs);

        //Setup coefficient matrix
        M33 ptm;
        ptm[0] = pverts[vinds[0]];
        ptm[1] = pverts[vinds[1]];
        ptm[2] = pverts[vinds[2]];
        ptm.setColumn(2, V3(1.0f, 1.0f, 1.0f));
        ptm = ptm.inverse();

        //Get colors at verts
        V3 reds(cols[vinds[0]][0], cols[vinds[1]][0], cols[vinds[2]][0]);
        V3 greens(cols[vinds[0]][1], cols[vinds[1]][1], cols[vinds[2]][1]);
        V3 blues(cols[vinds[0]][2], cols[vinds[1]][2], cols[vinds[2]][2]);

        // Setup screen space interpolation of depth: zABC 
        V3 zABC = ptm*V3(pverts[vinds[0]][2],
                pverts[vinds[1]][2], pverts[vinds[2]][2]);

        //Setup screen space interpolation of normals
        V3 nxABC = ptm*V3(normals[vinds[0]][0],
                normals[vinds[1]][0], normals[vinds[2]][0]);
        V3 nyABC = ptm*V3(normals[vinds[0]][1],
                normals[vinds[1]][1], normals[vinds[2]][1]);
        V3 nzABC = ptm*V3(normals[vinds[0]][2],
                normals[vinds[1]][2], normals[vinds[2]][2]);

        //Model space interpolation parameters
        M33 msiQ = GetMSIQ(verts[vinds[0]], verts[vinds[1]], verts[vinds[2]], ppc); 
        V3 denABC = msiQ[0] + msiQ[1] + msiQ[2];
        V3 redNABC(msiQ.getColumn(0) * reds, msiQ.getColumn(1) * reds, 
                msiQ.getColumn(2) * reds);
        V3 greenNABC(msiQ.getColumn(0) * greens, msiQ.getColumn(1) * greens, 
                msiQ.getColumn(2) * greens);
        V3 blueNABC(msiQ.getColumn(0) * blues, msiQ.getColumn(1) * blues, 
                msiQ.getColumn(2) * blues);
        
        //Texture coordinates interpolation
        V3 sNumABC, tNumABC, sABC, tABC;
        if (tcs) {
            V3 ss(tcs[2*vinds[0]+0], tcs[2*vinds[1]+0], tcs[2*vinds[2]+0]);
            V3 ts(tcs[2*vinds[0]+1], tcs[2*vinds[1]+1], tcs[2*vinds[2]+1]);
            sNumABC = V3(msiQ.getColumn(0) * ss, msiQ.getColumn(1) * ss, 
                    msiQ.getColumn(2) * ss);
            tNumABC = V3(msiQ.getColumn(0) * ts, msiQ.getColumn(1) * ts, 
                    msiQ.getColumn(2) * ts);
            sABC = ptm * ss;
            tABC = ptm * ts;
        }

        // For every pixel in the bounding box
        int top = (int) (aabb.corners[0][1] + 0.5f);
        int bottom = (int) (aabb.corners[1][1] - 0.5f);
        int left = (int) (aabb.corners[0][0] + 0.5f);
        int right = (int) (aabb.corners[1][0] - 0.5f);
        for (int v = top; v <= bottom; v++) {
            for (int u = left; u <= right; u++) {

                V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);

                //Check edge equations
                if (eeqs[0]*pixv < 0.0f || 
                    eeqs[1]*pixv < 0.0f || 
                    eeqs[2]*pixv < 0.0f)
                    continue;

                // Check z-buffer
                float currz = zABC * pixv;
                if (fb->IsFarther(u, v, currz))
                    continue;
                fb->SetZ(u, v, currz);

                V3 currColor;
                float msdn = denABC * pixv;

                V3 fullColor;
                if (tcs) {
                    float currs = sABC * pixv;
                    float currt = tABC * pixv;
                    int ut, vt;
                    ut=vt=0;
                    if (tm == 0) { //nearest neighbor
                        ut = (int) (currs * (float) texture->w);
                        vt = (int) (currt * (float) texture->h);
                    }
                    else if(tm == 1) { //bilinear
                    }
                    fullColor.setFromColor(texture->Get(ut, vt));
                }
                else {
                    fullColor = V3(redNABC * pixv, greenNABC * pixv, blueNABC * pixv) / msdn;
                }
                V3 currNormal, lv;
                V3 pp(pixv);
                pp[2] = currz;
                lv = (L - ppc->UnProject(pp)).normalize();
                currNormal = V3(nxABC*pixv, nyABC*pixv, nzABC*pixv).normalize();
                float kd = lv * currNormal;
                kd = (kd < 0.0f) ? 0.0f : kd;
                float ks = 0; //pow(reflectedLightVector * eyeVector, se)
                currColor = fullColor * (ka + (1.0f-ka)*kd + ks);

                fb->Set(u, v, currColor.getColor());
            }
        }
    }

    delete []pverts;
}

void TMesh::RenderHW() {

    //Set vertices array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (float*)verts);

    //Set normals array
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, (float*)normals);

    //Set texture coordinates
    if (texID != -1) {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexCoordPointer(2, GL_FLOAT, 0, tcs);
    }

    else {
        //Set color array
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, 0, (float*)cols);
    }

    //Draw
    glDrawElements(GL_TRIANGLES, 3*trisN, GL_UNSIGNED_INT, tris);

    //Disable vertex values
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    if (texID != -1) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    

    else {
        glDisableClientState(GL_COLOR_ARRAY);
    }
}

void TMesh::RenderWireframeHW() {

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnableClientState(GL_VERTEX_ARRAY);

    // similar for array of colors and array of texture coordinates
    if (normals)
        glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (float*) verts);
    if (normals)
        glNormalPointer(GL_FLOAT, 0, (float*)normals);
    glDrawElements(GL_TRIANGLES, 3*trisN, GL_UNSIGNED_INT, 
            tris);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, (float*)cols);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
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
    //float *tcs = 0; // don't have texture coordinates for now
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

