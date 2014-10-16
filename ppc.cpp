
#include "ppc.h"
#include "m33.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "framebuffer.h"

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {
    C = V3(0.0f, 0.0f, 0.0f);
    a = V3(1.0f, 0.0f, 0.0f);
    b = V3(0.0f, -1.0f, 0.0f);
    float alpha = hfov / 2.0f * 3.1415926f / 180.0f;
    c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f *tanf(alpha)));
}

//load from file
PPC::PPC(const char *filename) {
    string as, bs, cs, Cs,
           ws, hs, hfovs;
    ifstream ppc_f(filename);
    if (ppc_f.is_open()) {
        getline(ppc_f, as);
        getline(ppc_f, bs);
        getline(ppc_f, cs);
        getline(ppc_f, Cs);
        getline(ppc_f, ws);
        getline(ppc_f, hs);
    }
    ppc_f.close();
    a = stof(as);
    b = stof(bs);
    c = stof(cs);
    C = stof(Cs);
    w = stof(ws);
    h = stof(hs);
    h = stof(hs);
}

void PPC::Save(const char *filename) {
    ofstream ppc_f(filename);
    if (ppc_f.is_open()) {
        ppc_f << a << '\n';
        ppc_f << b << '\n';
        ppc_f << c << '\n';
        ppc_f << C << '\n';
        ppc_f << w << '\n';
        ppc_f << h << '\n';
    }
    ppc_f.close();
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

V3 PPC::Ray(int u, int v) {
    return a*(u + 0.5f) + b*(v + 0.5f) + c;
}

V3 PPC::Ray(float uf, float vf) {
    return a*uf + b*vf + c;
}

V3 PPC::PC(int u, int v) {
    return C + Ray(u,v);
}

float PPC::HFOV() {
    return 2 * atan(w/2 * a.length() / GetF());
}

float PPC::PPu() {
     return ((c*(-1.0f)) * a.normalize()) / a.length();
}

float PPC::PPv() {
     return ((c*(-1.0f)) * b.normalize()) / b.length();
}

bool PPC::Project(V3 P, V3 &PP) {
    M33 cam;
    cam.setColumn(0, a);
    cam.setColumn(1, b);
    cam.setColumn(2, c);

    V3 q = cam.inverse() * (P-C);

    if (q[2] <= 0.0f)
        return false;

    PP[0] = q[0] / q[2]; //u 
    PP[1] = q[1] / q[2]; //v
    PP[2] = 1.0f / q[2];
    return true;
}


V3 PPC::UnProject(V3 PP) {

  V3 ret;
  ret = C + (a*PP[0] + b*PP[1] + c)/PP[2];
  return ret;

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

V3 PPC::GetPointOnImagePlane(float uf, float vf) {
    V3 ret;
    ret = C + c + a*uf + b*vf;
    return ret;
}

V3 PPC::GetPointOnFocalPlane(float uf, float vf, float f) {
    float of = GetF();
    V3 ret;
    ret = C + (c +a*uf + b*vf)*f / of;
    return ret;
}

void PPC::Zoom(float step) {
    c = (a * -PPu()) - (b * PPv()) + (GetVD()*GetF() * step);
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

void PPC::SetByInterpolation(PPC* ppc0, PPC* ppc1, float frac) {

  V3 newC = ppc0->C + (ppc1->C - ppc0->C)*frac;
  V3 vd0 = ppc0->a % ppc0->b;
  V3 vd1 = ppc1->a % ppc1->b;
  V3 newvd = (vd0 + (vd1 - vd0)*frac).normalize();
  V3 vpv = (ppc0->b + (ppc1->b - ppc0->b)*frac).normalize()*-1.0f;
  PositionAndOrient(newC, newC + newvd, vpv);

}

void PPC::RenderWireframe(PPC *ppc, FrameBuffer *fb, float f,
    unsigned int color) {

  V3 baseCorners[4];
  baseCorners[0] = GetPointOnFocalPlane(0.0f, 0.0f, f);
  baseCorners[1] = GetPointOnFocalPlane(0.0f, (float)h, f);
  baseCorners[2] = GetPointOnFocalPlane((float)w, (float)h, f);
  baseCorners[3] = GetPointOnFocalPlane((float)w, 0.0f, f);

  V3 c0;
  c0.setFromColor(color);

  for (int si = 0; si < 4; si++) {
    fb->Draw3DSegment(C, baseCorners[si], ppc, color);
    fb->Draw3DSegment(baseCorners[si], baseCorners[(si+1)%4], ppc, color);
    //fb->Draw3DSegment(C, baseCorners[si], ppc, c0, c0);
    //fb->Draw3DSegment(baseCorners[si], baseCorners[(si+1)%4], ppc, c0, c0);
  }

}

