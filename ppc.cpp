
#include "ppc.h"
#include "m33.h"

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {

    C = V3(0.0f, 0.0f, 0.0f);
    a = V3(1.0f, 0.0f, 0.0f);
    b = V3(0.0f, -1.0f, 0.0f);
    float alpha = hfov / 2.0f * 3.1415926f / 180.0f;
    c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f *tanf(alpha)));

}


bool PPC::Project(V3 P, V3 &PP) {

    M33 cam;
    cam.SetColumn(0, a);
    cam.SetColumn(1, b);
    cam.SetColumn(2, c);

    V3 q = cam.Inverse() * (P-C);

    if (q[2] <= 0.0f)
        return false;

    PP[0] = q[0] / q[2];
    PP[1] = q[1] / q[2];
    PP[2] = 1.0f / q[2];
    return true;

}

void PPC::PositionAndOrient(V3 newC, V3 lap, V3 vpv) {


    V3 newVD = (lap - newC).UnitVector();

    V3 newa = (newVD ^ vpv).UnitVector();
    newa = newa * a.Length();

    V3 newb = (newVD ^ newa).UnitVector();
    newb = newb * b.Length();

    V3 newc = newVD*GetF() -newa*(float)w/2.0f
        -newb*(float)h/2.0f;

    a = newa;
    b = newb;
    c = newc;
    C = newC;

}

V3 PPC::GetVD() {

    V3 ret;
    ret = (a ^ b).UnitVector();
    return ret;

}

float PPC::GetF() {

    float ret;
    ret = GetVD()*c;
    return ret;

}
