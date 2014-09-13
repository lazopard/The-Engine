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
    return xyz[0] == v[0] &&
           xyz[1] == v[1] &&
           xyz[2] == v[2];
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


/*
V3 V3::rotateVector() {
}
*/

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
    M33 r_m(closer_to, theta);
    point = r_m*point;

    //Transform back to original coord system
    point = (trans_matrix.inverse()*point) + o;
    return point;
}

