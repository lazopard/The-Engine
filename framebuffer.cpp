#include "framebuffer.h"
#include "V3.h"
#include <iostream>
#include "scene.h"
#include <cmath>
#include <tiffio.h>

const float FrameBuffer::DEFAULT_B = 50;
const float FrameBuffer::DEFAULT_C = 1;

FrameBuffer::FrameBuffer(int u0, int v0, 
  int _w, int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0) {

  w = _w;
  h = _h;
  pix = new unsigned int[w*h];
  brightness = DEFAULT_B;
  contrast = DEFAULT_C;
}

FrameBuffer::FrameBuffer(unsigned int *raster, int u0, int v0, unsigned int _w, unsigned int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0) {
    w = _w;
    h = _h;
    pix = new unsigned int[w*h];
    _TIFFmemcpy(pix, raster, (w*h) * sizeof(uint32));
    brightness = DEFAULT_B;
    contrast = DEFAULT_C;
}

void FrameBuffer::draw() {
  glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
}

void FrameBuffer::Set(unsigned int bgr) {
  for (int i = 0; i < w*h; i++) {
    pix[i] = bgr;
  }
}

void FrameBuffer::Set(int u, int v, unsigned int color) {
  pix[(h-1-v)*w+u] = color;
}

unsigned int FrameBuffer::Get(int u, int v) {
  return pix[(h-1-v)*w+u];
}

void FrameBuffer::SetSafe(int u, int v, unsigned int color) {
  if (u < 0 || u > w-1 || v < 0 || v > h-1)
    return;
  Set(u, v, color);
}

void FrameBuffer::SetChecker(int csize, unsigned int color0, 
  unsigned int color1) {

  for (int v = 0; v < h; v++) {
    int cv = v / csize;
    for (int u = 0; u < w; u++) {
      int cu = u / csize;
      if ( (cu+cv) % 2)
        Set(u, v, color1);
      else
        Set(u, v, color0);
    }
  }
}

void FrameBuffer::DrawPoint(int u, int v, int psize, unsigned int color) {

  for (int i = v - psize/2; i <= v + psize/2; i++) {
    for (int j = u - psize/2; j <= u + psize/2; j++) {
      SetSafe(j, i, color);
    }
  }
}

void FrameBuffer::DrawSegment(float u0f, float v0f, float u1f, float v1f, 
                              unsigned int color) {

    float du = abs(u1f-u0f);
    float dv = abs(v1f-v0f);
    int stepsN = (du < dv) ? 1 + (int) dv : 1 + (int) du;

    V3 startingPoint(u0f, v0f, 0.0f);
    V3 endingPoint(u1f, v1f, 0.0f);
    V3 currentPoint = startingPoint;
    int segsN;
    if (stepsN == 1)
      segsN = 1;
    else
      segsN = stepsN-1;
    V3 segmentStep = (endingPoint - startingPoint) / (float) segsN;
    for (int i = 0; i < stepsN; i++) {
      int u = (int) currentPoint[0];
      int v = (int) currentPoint[1];
      SetSafe(u, v, color);
      currentPoint = currentPoint + segmentStep;
    }
}

//8-way symmetry circle
void FrameBuffer::DrawCircle(float cx, float cy, float r, unsigned int color) {
    SetSafe(cx + r, cy, color);
    SetSafe(cx, cx + r, color);
    SetSafe(cx, cy - r, color);
    float x = 1.0f;
    float y = r;
    y = sqrt((r*r) - 1) + 0.5;
    while (x < y) {
        SetSafe(cx + y, cy - x, color);
        SetSafe(cx - y, cy + x, color);
        SetSafe(cx - y, cy - x, color);
        SetSafe(cx + x, cy + y, color);
        SetSafe(cx + x, cy - y, color);
        SetSafe(cx - x, cy + y, color);
        SetSafe(cx - x, cy - y, color);
        SetSafe(cx + y, cy + x, color);
        y = sqrt((r*r) - (x*x)) + 0.5;
        x++;
    }
    if (abs(x - y) < 0.01) {
        SetSafe(cx + x, cy + y, color);
        SetSafe(cx + x, cy - y, color);
        SetSafe(cx - x, cy + y, color);
        SetSafe(cx - x, cy - y, color);
    }
}

//8-way symmetry circle
void FrameBuffer::DrawCircleWithThickness(float cx, float cy, float r, float thickness, unsigned int color) {
    FillCircle(cx + r, cy, thickness, color);
    FillCircle(cx, cx + r, thickness, color);
    FillCircle(cx, cy - r, thickness, color);
    float x = 1.0f;
    float y = r;
    y = sqrt((r*r) - 1) + 0.5;
    while (x < y) {
        FillCircle(cx + y, cy - x, thickness, color);
        FillCircle(cx - y, cy + x, thickness, color);
        FillCircle(cx - y, cy - x, thickness, color);
        FillCircle(cx + x, cy + y, thickness, color);
        FillCircle(cx + x, cy - y, thickness, color);
        FillCircle(cx - x, cy + y, thickness, color);
        FillCircle(cx - x, cy - y, thickness, color);
        FillCircle(cx + y, cy + x, thickness, color);
        y = sqrt((r*r) - (x*x)) + 0.5;
        x++;
    }
    if (abs(x - y) < 0.01) {
        FillCircle(cx + x, cy + y, thickness, color);
        FillCircle(cx + x, cy - y, thickness, color);
        FillCircle(cx - x, cy + y, thickness, color);
        FillCircle(cx - x, cy - y, thickness, color);
    }
}

void FrameBuffer::FillCircle(float cx, float cy, float r, unsigned int color) {

    for(float x = cx - r; x < cx + r; x++) {
        for(float y = cy - r; y < cy + r; y++) {
            float dx = cx - x;
            float dy = cy - y;
            if ((dx*dx + dy*dy) <= (r*r)) {
                SetSafe(x,y,color);
            }
        }
    }
}

void FrameBuffer::Convolve33(M33 kernel, FrameBuffer *&fb1) {

  fb1 = new FrameBuffer(30 + w, 30, w, h);

  for (int v = 1; v < h-1; v++) {
    for (int u = 1; u < w-1; u++) {
      V3 newColor(0.0f, 0.0f, 0.0f);
      for (int vi = -1; vi <= 1; vi++) {
        for (int ui = -1; ui <= 1; ui++) {
          V3 currColor;
          currColor.setFromColor(Get(u+ui, v+vi));
          newColor = newColor + currColor * kernel[vi+1][ui+1];
        }
      }
      unsigned int newc = newColor.getColor();
      fb1->Set(u, v, newc);
    }
  }
}

void FrameBuffer::Draw3DPoint(V3 pt, PPC *ppc, int psize, V3 color) {

    V3 ppt;
    if (!ppc->Project(pt, ppt))
        return;
    DrawPoint((int)ppt[0], (int)ppt[1], psize, color.getColor());
}

void FrameBuffer::Draw3DSegment(V3 p0, V3 p1, PPC *ppc, unsigned int color) {

    V3 pp0, pp1;
    if (!ppc->Project(p0, pp0))
        return;
    if (!ppc->Project(p1, pp1))
        return;
    DrawSegment(pp0[0], pp0[1], pp1[0], pp1[1], color);  
}

void FrameBuffer::AdjustBrightness(float b) { // 0 <= b <= 100
    brightness = b;
}

void FrameBuffer::AdjustContrast(float c) {
    contrast = c;
}

bool FrameBuffer::IsOutsideFrame(int u, int v) {

  bool ret;
  ret = u < 0 || u > w-1 || v < 0 || v > h-1;
  return ret;

}

void FrameBuffer::Clear(unsigned int bgr, float z0) {

  Set(bgr);
  for (int i = 0; i < w*h; i++) {
    zb[i] = z0;
  }

}

bool FrameBuffer::IsFarther(int u, int v, float currz) {

  if (zb[(h-1-v)*w+u] >= currz)
    return true;

  return false;

}

void FrameBuffer::SetZ(int u, int v, float currz) {

  zb[(h-1-v)*w+u] = currz;

}

