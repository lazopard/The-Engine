#define EPSILON 0.000001

inline bool areEqual(float a, float b) {
    return fabs(a - b) < EPSILON;
}

bool insideTriangle(V3 pv, V3 e1, V3 e2, V3 e3) {
    bool inside1, inside2, inside3;
    float ee1, ee2, ee3;
    ee1 = e1 * pv;
    ee2 = e2 * pv;
    ee3 = e3 * pv;

    //To fix shared edges
    bool t = (areEqual(e1[0], 0)) ?  e1[0] > 0 : e1[2] > 0;
    inside1 = ee1 > 0 || (areEqual(ee1, 0) && t);

    t = (areEqual(e2[0], 0)) ? e2[0] > 0 : e2[2] > 0;
    inside2 = ee2 > 0 || (areEqual(ee2, 0) && t);

    t = (areEqual(e3[0], 0)) ? e3[0] > 0 : e3[2] > 0;
    inside3 = ee3 > 0 || (areEqual(ee3, 0) && t);

    return inside1 && inside2 && inside3;
}

void TMesh::RenderFilled(PPC *ppc, FrameBuffer *fb, 
        unsigned int color, V3 L, float ka, float se, int renderMode) {

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
        /*
        if (!ppc->Project(verts[vinds[0]], pverts[vinds[0]]) ||
            !ppc->Project(verts[vinds[1]], pverts[vinds[1]]) ||
            !ppc->Project(verts[vinds[2]], pverts[vinds[2]])) {
            continue;
        }
        */

        //Compute bounding box of projected vertices
        AABB bbox(pverts[vinds[0]]);
        bbox.AddPoint(pverts[vinds[1]]);
        bbox.AddPoint(pverts[vinds[2]]);

        //Setup edge normals
        V3 e1, e2, e3;
        e1 = pverts[vinds[0]] % pverts[vinds[1]];
        e2 = pverts[vinds[1]] % pverts[vinds[2]];
        e3 = pverts[vinds[2]] % pverts[vinds[0]];

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

        //Setup linear coefficients of red
        V3 redABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], rr);
            
        //Setup linear coefficients of green 
        V3 greenABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], gr);

        //Setup linear coefficients of blue 
        V3 blueABC = screenSpaceInterpolate(pverts[vinds[0]],
                       pverts[vinds[1]], pverts[vinds[2]], br);

        int left = (int) (bbox[0][0] + .5); 
        int right = (int) (bbox[0][1] - .5); 
        int top= (int) (bbox[1][0] +.5); 
        int bottom = (int) (bbox[1][1] - .5);

        int u, v; // current pixel considered
        V3 pv; //pixel vector
        for(v = top; v <= bottom; v++) { 
            for(u = left; u <= right; u++) {
                pv[0] = u + 0.5f;
                pv[1] = v + 0.5f;
                pv[2] = 1.0f;

                // Check if triangle is closer than previously seen
                float currz = zABC * pv;
                if (fb->IsFarther(u, v, currz))
                    continue;
                fb->SetZ(u, v, currz);

                if (insideTriangle(pv, e1, e2, e3)) {

                    // From model file
                    if (renderMode == 0) {
                    }

                    // Interpolate colors per triangle vertex
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
        }
    }

    delete []pverts;

}

