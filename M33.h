#pragma once

#include <iostream>

#include "V3.h"

using namespace std;

class V3;

class M33 {

V3 rows[3];

public:
  // Constructors
  M33();
  M33(V3, V3, V3);
  M33(float); // Diagonal Constructor
  M33(V3, float); // Rotation Constructor
  M33(float a1, float a2, float a3,
      float a4, float a5, float a6,
      float a7, float a8, float a9);

  // Get/Set Row and get individual elements
  V3& operator[](unsigned);

  // Row/Column Operations
  V3 getColumn(unsigned);
  void setColumn(unsigned, V3);

  // Matrix Operations
  M33 inverse();
  M33 transpose();
  M33 normalize();
  M33 operator*(float);
  V3 operator*(V3); 
  M33 operator*(M33);

  M33 operator+(M33);

  // I/O
  friend ostream& operator<<(ostream&, M33&);
  friend istream& operator>>(istream&, M33&);
};

