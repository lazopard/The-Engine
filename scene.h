#pragma once

#include "gui.h"
#include "framebuffer.h"

class Scene {
public:
  GUI *gui;
  FrameBuffer *fb;
  Scene();
  void DBG();
  void Render();
  void loadImage();
  void saveImage();
  void changeBrightness();
  void detectEdges();
};

extern Scene *scene;
