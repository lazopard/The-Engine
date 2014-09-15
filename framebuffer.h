#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <Fl/Fl_File_Chooser.H>
#include <OpenGL/glu.h>
#include "M33.h"

class FrameBuffer : public Fl_Gl_Window {
public:
  unsigned int *pix;
  int w, h;
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
  void AdjustBrightness(float nb);
  void DetectEdges();
  void Convolve33(M33 kernel, FrameBuffer *& fb1);
};

