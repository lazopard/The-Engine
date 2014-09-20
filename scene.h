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
  void tiltPPC();
  void panPPC();
  void rollPPC();
};

extern Scene *scene;

