
#include "ppc.h"
#include "m33.h"
#include <cmath>

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {

    C = V3(0.0f, 0.0f, 0.0f);
    a = V3(1.0f, 0.0f, 0.0f);
    b = V3(0.0f, -1.0f, 0.0f);
    float alpha = hfov / 2.0f * 3.1415926f / 180.0f;
    c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f *tanf(alpha)));
}


bool PPC::Project(V3 P, V3 &PP) {

    M33 cam;
    cam.setColumn(0, a);
    cam.setColumn(1, b);
    cam.setColumn(2, c);

    V3 q = cam.inverse() * (P-C);

    if (q[2] <= 0.0f)
        return false;

    PP[0] = q[0] / q[2];
    PP[1] = q[1] / q[2];
    PP[2] = 1.0f / q[2];
    return true;

}

void PPC::PositionAndOrient(V3 newC, V3 lap, V3 vpv) {


    V3 newVD = (lap - newC).normalize();

    V3 newa = (newVD % vpv).normalize();
    newa = newa * a.length();

    V3 newb = (newVD % newa).normalize();
    newb = newb * b.length();

    V3 newc = newVD*GetF() -newa*(float)w/2.0f
        -newb*(float)h/2.0f;

    a = newa;
    b = newb;
    c = newc;
    C = newC;

}

V3 PPC::GetVD() {

    V3 ret;
    ret = (a % b).normalize();
    return ret;

}

float PPC::GetF() {

    float ret;
    ret = GetVD()*c;
    return ret;

}

void PPC::TranslateX(float step) {
    C = C + a.normalize() * step;
}

void PPC::TranslateY(float step) {
    C = C - b.normalize() * step;
}

void PPC::TranslateZ(float step) {
    C = C + (a%b).normalize() * step;
}

void PPC::Pan(float theta) {
    V3 a1 = a.rotatePoint(C, C - b, theta);
    V3 b1 = b.rotatePoint(C, C - b, theta);
    V3 c1 = c.rotatePoint(C, C - b, theta);
    a = a1;
    b = b1;
    c = c1;
}

void PPC::Tilt(float theta) {
    V3 a1 = a.rotatePoint(C, C + a, theta);
    V3 b1 = b.rotatePoint(C, C + a, theta);
    V3 c1 = c.rotatePoint(C, C + a, theta);
    a = a1;
    b = b1;
    c = c1;
}

void PPC::Roll(float theta) {
    V3 a1 = a.rotatePoint(C, C + (a%b), theta);
    V3 b1 = b.rotatePoint(C, C + (a%b), theta);
    V3 c1 = c.rotatePoint(C, C + (a%b), theta);
    a = a1;
    b = b1;
    c = c1;
}

