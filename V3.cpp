#include "V3.h"
#include <cmath>
#include "M33.h"

V3::V3(float x, float y, float z) {
  xyz[0] = x;
  xyz[1] = y;
  xyz[2] = z;
}

V3::V3(float val) {
    xyz[0] = val;
    xyz[1] = val;
    xyz[2] = val;
}

float& V3::operator[](unsigned i) {
  return xyz[i];
}

V3 V3::operator*(float c) {
  return V3(xyz[0] * c, xyz[1] * c, xyz[2] * c);
}

V3& V3::operator*=(float c) {
    this->xyz[0] *= c;
    this->xyz[1] *= c;
    this->xyz[2] *= c;
    return *this;
}

V3 V3::operator/(float c) {
  return V3(xyz[0]/c, xyz[1]/c, xyz[2]/c);
}

V3& V3::operator/=(float c) {
    this->xyz[0] /= c;
    this->xyz[1] /= c;
    this->xyz[2] /= c;
    return *this;
}

V3 V3::operator+(V3 v) {
  return V3(xyz[0] + v[0], xyz[1] + v[1], xyz[2] + v[2]);
}

V3& V3::operator+=(V3 v) {
    this->xyz[0] += v[0];
    this->xyz[1] += v[1];
    this->xyz[2] += v[2];
    return (*this);
}

V3 V3::operator-(V3 v) {
  return V3(xyz[0] - v[0], xyz[1] - v[1], xyz[2] - v[2]);
}

V3& V3::operator-=(V3 v) {
    this->xyz[0] -= v[0];
    this->xyz[1] -= v[1];
    this->xyz[2] -= v[2];
    return (*this);
}

bool V3::operator==(V3 v) {
    return abs(xyz[0] - v[0]) < 0.0001 &&
           abs(xyz[1] - v[1]) < 0.0001 &&
           abs(xyz[2] - v[2]) < 0.0001;
}

bool V3::operator!=(V3 v) {
    return !(*this == v);
}

float V3::operator*(V3 v) {
  return xyz[0]*v[0] + xyz[1]*v[1] + xyz[2]*v[2];
}

V3 V3::operator%(V3 v) {
  return V3(xyz[1]*v[2] - xyz[2]*v[1], 
            -(xyz[0]*v[2] - xyz[2]*v[0]),
            xyz[0]*v[1] - xyz[1]*v[0]);
}

float V3::length() {
    return sqrt((xyz[0]*xyz[0]) + (xyz[1]*xyz[1]) + (xyz[2]*xyz[2]));
}

float V3::lengthSquared() {
    return (xyz[0]*xyz[0]) + (xyz[1]*xyz[1]) + (xyz[2]*xyz[2]);
}

V3 V3::normalize() {
    return (*this)/(*this).length();
}

ostream& operator<<(std::ostream& out, V3& v) {
  out 
    << v[0] << " " 
    << v[1] << " " 
    << v[2];
  return out;
}

istream& operator>>(std::istream& in, V3& v) {
  in >> v[0];
  in >> v[1];
  in >> v[2];
  return in;
}

V3 V3::rotate(V3 a, float theta) {
    V3 v = (*this);
    M33 r_m(a.normalize(), theta);
    r_m = M33(1) + (r_m * (sin(theta))) + (r_m * r_m * (2*sin(theta/2) * sin(theta/2)));
    v = r_m * v;
    return v;
}

V3 V3::rotatePoint(V3 o, V3 a, float theta) {

    V3 point = (*this);
    V3 x = V3(1,0,0);
    V3 y = V3(0,1,0);
    V3 z = V3(0,0,1);

    // Create new coordinate system
    V3 new_coord[3];
    new_coord[0] = o + a.normalize(); //a as first axis

    //x/y cross a as second axis b
    unsigned closer_to;
    if (abs(a*x) < abs(a*y)) {
        new_coord[1] = (x % a).normalize();
        closer_to = 0; // x
    }
    else {
        new_coord[1] = (y % a).normalize();
        closer_to = 1; // y
    }
    // a cross b as third axis
    new_coord[2] = (a % new_coord[1]).normalize();

    // Transform point to new coord system
    M33 trans_matrix = M33(new_coord[0], new_coord[1], new_coord[2]);
    point = trans_matrix*(point - o);

    // Rotate about a by theta degrees
    /*M33 r_m(a.normalize(), theta);
    r_m = M33(1) + (r_m * (sin(theta))) + (r_m * r_m * (2*sin(theta/2) * sin(theta/2)));
    */
    M33 r_m(closer_to, theta);
    point = r_m * point;

    //Transform back to original coord system
    point = (trans_matrix.inverse()*point) + o;
    return point;
}

void V3::setFromColor(unsigned int color) {

  V3 &v = *this;
  v[0] = ((unsigned char*)&color)[0];
  v[1] = ((unsigned char*)&color)[1];
  v[2] = ((unsigned char*)&color)[2];
  v = v / 255.0f;

}

unsigned V3::getColor() {

  V3 &v = *this;
  int red = (int) (v[0]*255.0f+0.5f);
  int green = (int) (v[0]*255.0f+0.5f);
  int blue = (int) (v[0]*255.0f+0.5f);

  if (red < 0)
    red = 0;
  else if (red > 255)
    red = 255;
  if (green < 0)
    green = 0;
  else if (green > 255)
    green = 255;
  if (blue < 0)
    blue = 0;
  else if (blue > 255)
    blue = 255;

  unsigned int ret = 0xFF000000 + blue * 256 *256 + green * 256 + red;

  return ret;

}

AABB::AABB(V3 firstPoint) {

    corners[0] = corners[1] = firstPoint;

}

void AABB::AddPoint(V3 pt) {

    if (corners[0][0] > pt[0]) {
        corners[0][0] = pt[0];
    }
    if (corners[0][1] > pt[1]) {
        corners[0][1] = pt[1];
    }
    if (corners[0][2] > pt[2]) {
        corners[0][2] = pt[2];
    }

    if (corners[1][0] < pt[0]) {
        corners[1][0] = pt[0];
    }
    if (corners[1][1] < pt[1]) {
        corners[1][1] = pt[1];
    }
    if (corners[1][2] < pt[2]) {
        corners[1][2] = pt[2];
    }
}

