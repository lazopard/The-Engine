#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tmesh.h"

class Scene {
public:
  GUI *gui;
  FrameBuffer *fb, *hwfb;
  bool hwInit;
  FrameBuffer *currTexture;
  PPC *ppc;
  TMesh **tmeshes;
  int tmeshesN;
  float step;
  float ka, se;
  int rm;
  int tm, tl;
  int *smap;
  V3 l;
  Scene();
  void DBG();
  void Play();
  void saveTiff(const char *filename);
  void Render();
  void RenderHW();
  void loadImage();
  void loadTexture(const char *filename);
  void loadGeometry(const char *filename);
  void loadGeometry();
  void LoadCamera();
  void SaveCamera();
  void saveImage();
  void AddMesh(TMesh *tmesh, FrameBuffer *tex);
  void UpdateShadowMap();
  void ZoomIn();
  void ZoomOut();
  void BuildMap(TMesh *tmesh, PPC *ppc);
  void changeBrightness();
  void changeContrast();
  void detectEdges();
  void BuildRoomForMesh();
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
  void tileByRepeat();
  void tileByMirror();
  void tmBilinear();
  void tmNN();
};

extern Scene *scene;

