#pragma once

#include "v3.h"
#include "framebuffer.h"
#include "ppc.h"

class TMesh {
    public:
        bool enabled;

        V3 *verts; //vertices
        V3 *normals; 
        V3 *cols; //colors
        float *ss, *ts; // (s, t) texture coordinates

        int vertsN;
        unsigned int *tris;
        int trisN;

        AABB *aabb;

        TMesh();
        TMesh(V3 center, V3 dims, unsigned int color);
        void LoadBin(const char *fname);
        void RenderPoints(PPC *ppc, FrameBuffer *fb, int psize);
        void RenderWireframe(PPC *ppc, FrameBuffer *fb, unsigned int color);
        void RenderFilled(PPC *ppc, FrameBuffer *fb, unsigned int color, 
                          V3 L, float ka, float se, int renderMode);
        void RenderTM(PPC *ppc, FrameBuffer *fb, unsigned int *texture,
                      int w, int h, int tm_mode, int tl_mode);
        void mapToST(unsigned int *texture, int w, int h);
        void SetAABB();
        void ClipAABB();
        void Translate(V3 tv);
        V3 GetCenter();
        void Position(V3 newCenter);
        void ScaleToNewDiagonal(float newDiagonal);
        void Scale(float scf);
        void SetFromFB(FrameBuffer *fb, PPC *ppc);
        void SetEEQS(V3 pv0, V3 pv1, V3 pv2, V3 *eeqs);
        ~TMesh();

};

