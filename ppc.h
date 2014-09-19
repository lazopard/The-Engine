#pragma once

#include "v3.h"

class PPC {
    public:
        V3 a, b, c, C;
        int w, h;
        PPC(float hfov, int w, int h);
        bool Project(V3 P, V3 &PP);
        void PositionAndOrient(V3 newC, V3 lap, V3 vpv);
        float GetF();
        V3 GetVD();
        void TranslateX(float step);
        void TranslateY(float step);
        void TranslateZ(float step);
        void Pan(float theta);
        void Tilt(float theta);
        void Roll(float theta);
};

