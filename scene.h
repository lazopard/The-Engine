#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tmesh.h"

#include "CGInterface.h"

class Scene {
    public:
        GUI *gui;
        FrameBuffer *fb;

        Scene();

        //Rendering
        int renderMode; //Software vs Hardware
        void Render();
        void RenderSW();
        void RenderHW();

        //Shader based rendering
        CGInterface *cgi;
        ShaderOneInterface *soi;
        void RenderSHW();

        //FPS
        double max_fps;
        
        //Textures
        FrameBuffer *currTexture;
        unsigned int texN;
        unsigned int **textures;
        unsigned int *tnames;
        int tm, tl;
        void tileByRepeat();
        void tileByMirror();
        void tmBilinear();
        void tmNN();
        void loadTexture(const char *filename);
        void loadTextureHW(const char *filename);

        //Pinhole Camera
        PPC *ppc;
        float step;
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
        void ZoomIn();
        void ZoomOut();
        void LoadCamera();
        void SaveCamera();

        //Tmeshes
        TMesh **tmeshes;
        int tmeshesN;
        void loadGeometry(const char *filename);
        void loadGeometry();
        void AddMesh(TMesh *tmesh, FrameBuffer *tex);
        void AddMeshHW(TMesh *tmesh, unsigned int tname);


        //Lighting
        V3 l;
        float ka, se;
        void adjustAmbient();
        void adjustSpecular();
        void lightSourceUp();
        void lightSourceDown();
        void lightSourceLeft();
        void lightSourceRight();
        void lightSourceFront();
        void lightSourceBack();

        // Framebuffer
        void changeBrightness();
        void changeContrast();
        void detectEdges();
        void loadImage();
        void saveImage();
        void saveTiff(const char *filename);

        //Animation
        void Play();
        void PlayAnimation();
        void BuildRoomForMesh();
        void BuildEnvironment();
        int p;
        bool env_built;
};

extern Scene *scene;

