#pragma once

#include "v3.h"

class PPC {
    public:
        V3 a, b, c, C;
        int w, h;
        PPC(float hfov, int w, int h);
        PPC(const char *filename);
        void Save(const char *filename);
        V3 GetVD();
        float GetF();
        V3 Ray(int u, int v);
        V3 Ray(float uf, float vf);
        V3 PC(int u, int v);
        float HFOV();
        float PPv();
        float PPu();
        V3 GetPointOnImagePlane(float uf, float vf);
        V3 GetPointOnFocalPlane(float uf, float vf, float f);
        bool Project(V3 P, V3 &PP);
        void PositionAndOrient(V3 newC, V3 lap, V3 vpv);
        PPC LInterpolate(PPC p, float n);
        void Zoom(float step);
        void TranslateX(float step);
        void TranslateY(float step);
        void TranslateZ(float step);
        void Pan(float theta);
        void Tilt(float theta);
        void Roll(float theta);
};

