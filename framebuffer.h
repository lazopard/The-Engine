#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <Fl/Fl_File_Chooser.H>
#include <OpenGL/glu.h>
#include "M33.h"
#include "ppc.h"

class FrameBuffer : public Fl_Gl_Window {
public:
  unsigned int *pix, *opix;
  float *zb;
  int w, h;
  static const float DEFAULT_B, DEFAULT_C;
  float brightness; // 0<= brightness <= 100
  float contrast; //1 <= contrast <= 3
  FrameBuffer(int u0, int v0, int _w, int _h);
  FrameBuffer(unsigned int *raster, int u0, int v0, unsigned int _w, unsigned int _h);
  void draw();
  void Set(unsigned int bgr);
  void Set(int u, int v, unsigned int color);
  unsigned int Get(int u, int v);
  void SetSafe(int u, int v, unsigned int color);
  void SetChecker(int csize, unsigned int color0, unsigned int color1);
  void DrawPoint(int u, int v, int psize, unsigned int color);
  void DrawSegment(float u0f, float v0f, float u1f, float v1f, 
                   unsigned int color);
  void DrawCircle(float cx, float cy, float r, unsigned int color);
  void DrawCircleWithThickness(float cx, float cy, float r, float thickness, unsigned int color);
  void FillCircle(float cx, float cy, float r, unsigned int color);
  void AdjustBrightness(float b);
  void AdjustContrast(float c);
  void DetectEdges();
  void Convolve33(M33 kernel, FrameBuffer *& fb1);
  void Draw3DPoint(V3 pt, PPC *ppc, int psize, V3 color);
  void Draw3DSegment(V3 p0, V3 p1, PPC *ppc, unsigned int color);
  void Clear(unsigned int bgr, float z0);
  bool IsOutsideFrame(int u, int v);
  bool IsFarther(int u, int v, float currz);
  void SetZ(int u, int v, float currz);
};

