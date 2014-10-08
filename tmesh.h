#pragma once

#include "v3.h"
#include "framebuffer.h"
#include "ppc.h"

class TMesh {
    public:
        bool enabled;
        V3 *verts, *normals, *cols;
        int vertsN;
        unsigned int *tris;
        int trisN;
        AABB *aabb;
        TMesh();
        TMesh(V3 center, V3 dims, unsigned int color);
        void LoadBin(const char *fname);
        void RenderPoints(PPC *ppc, FrameBuffer *fb, int psize);
        void RenderWireframe(PPC *ppc, FrameBuffer *fb, unsigned int color);
        void SetAABB();
        void ClipAABB();
        void Translate(V3 tv);
        V3 GetCenter();
        void Position(V3 newCenter);
        void ScaleToNewDiagonal(float newDiagonal);
        void Scale(float scf);
        void RenderFilled(PPC *ppc, FrameBuffer *fb, 
        unsigned int color, V3 L, float ka, int renderMode);

};

