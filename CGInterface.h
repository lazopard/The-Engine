#pragma once

#include <Cg/cgGL.h>
#include <Cg/cg.h>

class CGInterface {

public:
  CGprofile   vertexCGprofile;
	CGprofile   geometryCGprofile;
	CGprofile   pixelCGprofile;
	CGcontext  cgContext;
  bool needInit;
  void PerSessionInit();
  CGInterface();
  void EnableProfiles();
  void DisableProfiles();
};

class ShaderOneInterface {

  CGprogram geometryProgram;
  CGprogram vertexProgram;
  CGprogram fragmentProgram;
  CGparameter vertexModelViewProj;
  CGparameter geometryModelViewProj;
  CGparameter fragmentBlueHue;

public:
  ShaderOneInterface() {};
  bool PerSessionInit(CGInterface *cgi);
  void BindPrograms();
  void PerFrameInit();
  void PerFrameDisable();

};
