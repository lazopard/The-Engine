#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tmesh.h"

class Scene {
public:
  GUI *gui;
  FrameBuffer *fb;
  PPC *ppc;
  TMesh **tmeshes;
  int tmeshesN;
  float step;
  float ka, se;
  int sm, tm, tl;
  V3 l;
  Scene();
  void DBG();
  void Play();
  void saveTiff(const char *filename);
  void Render();
  void loadImage();
  void loadGeometry();
  void LoadCamera();
  void SaveCamera();
  void saveImage();
  void ZoomIn();
  void ZoomOut();
  void changeBrightness();
  void changeContrast();
  void detectEdges();
  void translateRight();
  void translateLeft();
  void translateUp();
  void translateDown();
  void translateFront();
  void translateBack();
  void adjustStep();
  void tiltUp();
  void tiltDown();
  void panLeft();
  void panRight();
  void rollLeft();
  void rollRight();
  void PlayInterpolationAnimation();
  void PPCMovementAnimation();
  void adjustAmbient();
  void adjustSpecular();
  void lightSourceUp();
  void lightSourceDown();
  void lightSourceLeft();
  void lightSourceRight();
  void lightSourceFront();
  void lightSourceBack();
  void sm1();
  void sm2();
  void sm3();
  void tileByRepeat();
  void tileByMirror();
  void tmBilinear();
  void tmNN();
};

extern Scene *scene;

