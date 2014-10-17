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
    return length()*length();
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


V3 V3::rotatePoint(V3 aO, V3 aD,
        float thetad) {

    V3 xaxis(1.0f, 0.0f, 0.0f);
    V3 yaxis(0.0f, 1.0f, 0.0f);

    float adx = fabsf(xaxis*aD);
    float ady = fabsf(yaxis*aD);
    V3 aux;
    if (adx < ady) {
        aux = xaxis;
    }
    else {
        aux = yaxis;
    }

    M33 lm;
    lm[0] = (aux%aD).normalize();
    lm[1] = aD.normalize();
    lm[2] = (lm[0] % lm[1]).normalize();

    M33 ilm = lm.inverse();

    M33 rotY = M33(V3(0,1,0), thetad);    
    V3 pt(*this);
    V3 lpt = lm*(pt-aO);
    V3 rlpt = rotY*lpt;
    V3 ret = aO + ilm*rlpt;
    return ret;

}

V3 V3::rotate(V3 aD, float thetad) {

    V3 xaxis(1.0f, 0.0f, 0.0f);
    V3 yaxis(0.0f, 1.0f, 0.0f);

    float adx = fabsf(xaxis*aD);
    float ady = fabsf(yaxis*aD);
    V3 aux;
    if (adx < ady) {
        aux = xaxis;
    }
    else {
        aux = yaxis;
    }

    M33 lm;
    lm[0] = (aux%aD).normalize();
    lm[1] = aD.normalize();
    lm[2] = (lm[0] % lm[1]).normalize();

    M33 ilm = lm.inverse();

    M33 rotY = M33(V3(0,1,0), thetad);

    V3 pt(*this);
    V3 lpt = lm*pt;
    V3 rlpt = rotY*lpt;
    V3 ret = ilm*rlpt;
    return ret;

}

void V3::setFromColor(unsigned int color) {

  V3 &v = *this;
  v[0] = ((unsigned char*)&color)[0];
  v[1] = ((unsigned char*)&color)[1];
  v[2] = ((unsigned char*)&color)[2];
  v = v / 255.0f;

}

unsigned int V3::getColor() {

  V3 &v = *this;
  int red = (int) (v[0]*255.0f+0.5f);
  int green = (int) (v[1]*255.0f+0.5f);
  int blue = (int) (v[2]*255.0f+0.5f);

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

V3& AABB::operator[](unsigned i) {
    return corners[i];
}

float AABB::length() {
    return fabs(corners[0][0] - corners[1][0]);
}

float AABB::height() {
    return fabs(corners[0][1] - corners[1][1]);
}

float AABB::width() {
    return fabs(corners[0][2] - corners[1][2]);
}

