#pragma once

#include <iostream>

using namespace std;

class V3 {

  float xyz[3];

public:
  // Constructors
  V3() { };
  V3(float);
  V3(float, float, float);

  // Read/Write with [] syntax
  float& operator[](unsigned);   

  // Assignment operator
  //V3& operator=(V3&);

  // Operations with scalars
  V3 operator*(float);
  V3& operator*=(float);
  V3 operator/(float);
  V3& operator/=(float);

  // Operations with vectors
  V3 operator+(V3);
  V3& operator+=(V3);
  V3 operator-(V3);
  V3& operator-=(V3);
  bool operator==(V3);
  bool operator!=(V3);
  float operator*(V3); // Dot product
  V3 operator%(V3); // Cross product


  //Rotation
  /* Rotate point about arbitrary axis. Parameters are 2 points to define axis and a 
   scalar (float) to define the rotation angle in degrees.*/
   V3 rotatePoint(V3, V3, float);

  /* Rotate vector about arbitrary axis */
  //V3 rotate(float, float, V3&);

  // I/O
  friend ostream& operator<<(ostream&, V3&); //output
  friend istream& operator>>(istream&, V3&); //input

  // Other
  float length(); // length
  float lengthSquared();
  V3 normalize(); // normalization
  
  // Destructor
  ~V3() { };
};

