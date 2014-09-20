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
  Scene();
  void DBG();
  void Render();
  void loadImage();
  void saveImage();
  void changeBrightness();
  void changeContrast();
  void detectEdges();
  void translateRight();
  void translateLeft();
  void translateUp();
  void translateDown();
  void translateFront();
  void translateBack();
  void tiltUp();
  void tiltDown();
  void panLeft();
  void panRight();
  void rollLeft();
  void rollRight();
};

extern Scene *scene;

