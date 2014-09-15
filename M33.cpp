#include "M33.h"
#include <cmath>

M33::M33() {
    M33(0);
}

M33::M33(V3 v1, V3 v2, V3 v3) {
  rows[0] = v1;
  rows[1] = v2;
  rows[2] = v3;
}

M33::M33(float dval) {
  rows[0] = V3(dval, 0, 0);
  rows[1] = V3(0, dval, 0);
  rows[2] = V3(0, 0, dval);
}

M33::M33(float a1, float a2, float a3,
         float a4, float a5, float a6,
         float a7, float a8, float a9) {
  rows[0] = V3(a1, a2, a3);
  rows[1] = V3(a4, a5, a6);
  rows[2] = V3(a7, a8, a9);
}

M33::M33(V3 axis, float theta) {
    float rad = theta * (M_PI/180);
    if (axis == V3(1,0,0)) {
        rows[0] = V3(1, 0, 0);
        rows[1] = V3(0, cos(rad), -sin(rad));
        rows[2] = V3(0, sin(rad), cos(rad));
    }
    else if (axis == V3(0,1,0)) {
        rows[0] = V3(cos(rad), 0, sin(rad));
        rows[1] = V3(0, 1, 0);
        rows[2] = V3(-sin(rad), 0, cos(rad));
    }
    else if (axis == V3(0,0,1)) {
        rows[0] = V3(cos(rad), -sin(rad), 0);
        rows[1] = V3(sin(rad), cos(rad), 0);
        rows[2] = V3(0, 1, 1);
    }
    else {
        M33(0, -axis[2], axis[1],
            axis[2], 0, -axis[0],
            -axis[1], axis[0], 0);
    }
}

V3& M33::operator[](unsigned i) {
  return rows[i];
}

void M33::setColumn(unsigned col, V3 v) {
  rows[0][col] = v[0];
  rows[1][col] = v[1];
  rows[2][col] = v[2];
}

V3 M33::getColumn(unsigned col) {
  return V3(rows[0][col], rows[1][col], rows[2][col]);
}

M33 M33::inverse() {
    M33 &t_m = (*this);
    M33 ret;
    V3 a = t_m.getColumn(0), b = t_m.getColumn(1), c = t_m.getColumn(2);
    V3 _a = b % c; _a = _a / (a * _a);
    V3 _b = c % a; _b = _b / (b * _b);
    V3 _c = a % b; _c = _c / (c * _c);
    ret[0] = _a;
    ret[1] = _b;
    ret[2] = _c;
    return ret;
}

M33 M33::transpose() {
  M33 &t_m = (*this);
  M33 m;
  m[0] = t_m.getColumn(0);
  m[1] = t_m.getColumn(1);
  m[2] = t_m.getColumn(2);
  return m;
}

M33 M33::operator*(float c) {
    return M33(rows[0] * c, rows[1] * c, rows[2] * c);
}

V3 M33::operator*(V3 v) {
  return V3(rows[0]*v, rows[1]*v, rows[2]*v);
}

M33 M33::operator*(M33 m1) {
    M33 ret;
    M33 &m0 = (*this);
    ret.setColumn(0, m0*m1.getColumn(0));
    ret.setColumn(1, m0*m1.getColumn(1));
    ret.setColumn(2, m0*m1.getColumn(2));
    return ret;
}

M33 M33::operator+(M33 m1) {
    return M33(rows[0] + m1[0], rows[1] + m1[1], rows[2] + m1[2]);
}

M33 M33::normalize() {
    M33 m;

    V3 vs = rows[0] + rows[1] + rows[2];
    float sum = vs[0] + vs[1] + vs[2];

    m[0] = rows[0] / sum;
    m[1] = rows[1] / sum;
    m[2] = rows[2] / sum;

    return m;
}

ostream& operator<<(std::ostream& out, M33& a) {
  out << a.rows[0] << std::endl;
  out << a.rows[1] << std::endl;
  out << a.rows[2] << std::endl;
  return out;
}

istream& operator>>(std::istream& in, M33& a) {
  in >> a.rows[0];
  in >> a.rows[1];
  in >> a.rows[2];
  return in;
}

