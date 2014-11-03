#pragma once

#include "v3.h"
#include "framebuffer.h"
#include "ppc.h"

class TMesh {
    public:
        bool enabled;

        //Vertex parameters
        V3 *verts; //vertices
        V3 *normals; 
        V3 *cols; //colors
        float *tcs; // (s, t) texture coordinates
        unsigned int texID;

        //Software texture
        FrameBuffer *texture;

        int vertsN;
        unsigned int *tris;
        int trisN;

        AABB *aabb;

        TMesh();
        TMesh(V3 *vs, V3 *colors); // makes a quad
        TMesh(V3 center, V3 dims, unsigned int color);
        void LoadBin(const char *fname);
        void RenderPoints(PPC *ppc, FrameBuffer *fb, int psize);
        void RenderWireframe(PPC *ppc, FrameBuffer *fb, unsigned int color);
        void RenderFilled(PPC *ppc, FrameBuffer *fb, unsigned int color, 
                          V3 L, float ka, float se, int tm, int tl);
        void RenderHW(); 
        void SetAABB();
        void RenderWireframeHW();
        void ClipAABB();
        void AddTexture(FrameBuffer *tex);
        void AddTextureHW(unsigned int tname);
        void Translate(V3 tv);
        V3 GetCenter();
        void Position(V3 newCenter);
        void ScaleToNewDiagonal(float newDiagonal);
        void Scale(float scf);
        void SetFromFB(FrameBuffer *fb, PPC *ppc);
        void SetEEQS(V3 pv0, V3 pv1, V3 pv2, V3 *eeqs);
        M33 GetMSIQ(V3 v0, V3 v1, V3 v2, PPC *ppc);
        ~TMesh();

};

