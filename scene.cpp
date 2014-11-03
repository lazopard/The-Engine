#include "scene.h"
#include "m33.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tiffio.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

Scene *scene;
double start_t, end_t;

int frameTimes = 0;  
int frames = 0; 
double startTime = 0;
double endTime = 0;

char filename[1024] = "";

Scene::Scene() {

    //Software vs Hardware rendering
    renderMode = 0;

    //GUI and Window
    gui = new GUI();
    gui->show();
    int u0 = 20;
    int v0 = 50;
    int sci = 2;
    int w = sci*320;
    int h = sci*240;
    fb = new FrameBuffer(u0, v0, w, h);
    fb->label("Framebuffer");
    fb->isHW = true;
    fb->show();
    gui->uiw->position(fb->w+20+20, 50);

    //Animation
    p = 1;
    env_built = false;

    //FPS
    max_fps = 30;

    //PPC
    float hfov = 55.0f;
    ppc = new PPC(hfov, w, h);

    // Parameters to modify
    step = 10;
    ka=se=0.2;
    tl = 0;
    tm = 0;
    l = 0;

    //Textures and SM
    currTexture = 0;
    textures = 0;
    tnames = 0;
    texN = 0;

    // Tmeshes
    tmeshesN = 0;
    tmeshes = 0;

    Render();
}

//Load Camera coordinates and parameters
void Scene::LoadCamera() {
    Fl_File_Chooser chooser(".",                        // directory
                            "*",                        // filter
                            Fl_File_Chooser::SINGLE,     // chooser type
                            "Open...");        // title
    chooser.show();
     while(chooser.shown())
    { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
    { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

    ppc = new PPC(chooser.value());
    Render();
}

//Save Camera coordinates
void Scene::SaveCamera() {
   Fl_File_Chooser chooser(".",
                            "*",
                            Fl_File_Chooser::CREATE,
                            "Save File as...");
    chooser.show();
    while(chooser.shown())
    { Fl::wait(); }

    // User hit cancel
    if ( chooser.value() == NULL )
    { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

    ppc->Save(chooser.value());
}

void Scene::RenderSW() {
    unsigned int color = 0xFF0000FF;
    fb->Clear(0xFFFFFFFF, 0.0f);
    for (int tmi = 0; tmi < tmeshesN; tmi++) {
        if (!tmeshes[tmi]->enabled)
            continue;
        if (tmeshes[tmi]->trisN == 0)
            tmeshes[tmi]->RenderPoints(ppc, fb, 1);
        else {
            tmeshes[tmi]->RenderFilled(ppc, fb, color, l, ka, se, tm, tl);
        }
    }
    fb->redraw();
}

void Scene::RenderHW() {

    if (!scene)
        return;
    
    // clear the framebuffer
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    /*
    //lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, (float*) &l);
    */

    /*
    //Shadow mapping
    */

    // set the view
    float nearz = 1.0f;
    float farz = 1000.0f;
    glLoadIdentity();
    ppc->SetIntrinsicsHW(nearz, farz);
    ppc->SetExtrinsicsHW();

    // draw the tmeshes
    for (int tmi = 0; tmi < tmeshesN; tmi++) {
        if (!tmeshes[tmi]->enabled)
            continue;
        //tmeshes[tmi]->renderwireframehw();
        tmeshes[tmi]->RenderHW();
    }

    fb->redraw();
}

void startfps() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime  = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
}

void endfps() {

    //count fps
    timeval tv;
    gettimeofday(&tv, NULL);
    endTime = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;

    frameTimes = frameTimes + endTime - startTime;  
    //count one frame  
    ++frames;  

    //if the difference is greater than 5s, print fps
    if(frameTimes >= 5000)  {

        fprintf(stderr, "FPS: %d", frames);

        //reset time differences and number of counted frames  
        frames = 0;  
        frameTimes = 0; 
    }
}

void Scene::Render() {
    startfps();
    
    if (renderMode == 0)
        RenderHW();
    else
        RenderSW();

    endfps();
}

void Scene::AddMesh(TMesh *tmesh, FrameBuffer *tex) {
    if (tex != NULL)
        tmesh->AddTexture(tex);
    tmeshes[tmeshesN] = tmesh;
    tmeshesN++;
}

void Scene::AddMeshHW(TMesh *tmesh, unsigned int tname) {
    if (tname != UINT_MAX)
        tmesh->AddTextureHW(tname);
    tmeshes[tmeshesN] = tmesh;
    tmeshesN++;
}

void Scene::BuildEnvironment() {

    //First teapot with vertex color interpolation
    loadGeometry("geometry/teapot1k.bin");

    //Teapot with texture mapping
    loadGeometry("geometry/teapot1k.bin");
    loadTextureHW("ceramic.tiff");
    tmeshes[1]->AddTextureHW(tnames[0]);
    tmeshes[1]->Position(tmeshes[0]->GetCenter() + V3(100, 0, 0));

    //Room 
    BuildRoomForMesh();
    env_built = true;
    
    Render();
}

//play
void Scene::PlayAnimation() {

    ppc->Save("camera_path.txt");
    PPC ppcInit = *ppc;
    V3 center1 = tmeshes[0]->GetCenter();
    V3 center2 = tmeshes[1]->GetCenter();
    V3 vpv = V3(0, 1, 0);
    
    PPC ppc2 = *ppc;
    ppc2.TranslateX(30);
    ppc2.TranslateY(30);
    ppc2.TranslateZ(30);
    ppc2.PositionAndOrient(ppc2.C, center1, vpv);

    int stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
      ppc->SetByInterpolation(ppc, &ppc2, (float) si / (float) (stepsN-1));
      Render();
      Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.TranslateX(-20);
    ppc2.TranslateY(-20);
    ppc2.TranslateZ(-20);
    ppc2.PositionAndOrient(ppc2.C, center2, vpv);

    stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
      ppc->SetByInterpolation(ppc, &ppc2, (float) si / (float) (stepsN-1));
      Render();
      Fl::check();
    }
    ppc->Save("camera_path.txt");
    
    ppc2 = *ppc;
    ppc2.TranslateX(80);
    ppc2.TranslateY(30);
    ppc2.TranslateZ(-20);
    ppc2.PositionAndOrient(ppc2.C, center2, vpv);

    stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
      ppc->SetByInterpolation(ppc, &ppc2, (float) si / (float) (stepsN-1));
      Render();
      Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.TranslateX(80);
    ppc2.TranslateY(30);
    ppc2.TranslateZ(-40);
    ppc2.PositionAndOrient(ppc2.C, center2, vpv);

    stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
      ppc->SetByInterpolation(ppc, &ppc2, (float) si / (float) (stepsN-1));
      Render();
      Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.Roll(180);
    ppc2.TranslateZ(-20);
    //ppc2.PositionAndOrient(ppc2.C, center2, vpv);
    stepsN = 100;

    for(int si = 0; si < stepsN; si++) {
        ppc->SetByInterpolation(ppc, &ppc2, (float) si/ (float) (stepsN-1));
        Render();
        Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.TranslateZ(100);
    //ppc2.PositionAndOrient(ppc2.C, center2, vpv);
    stepsN = 100;

    for(int si = 0; si < stepsN; si++) {
        ppc->SetByInterpolation(ppc, &ppc2, (float) si/ (float) (stepsN-1));
        Render();
        Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.PositionAndOrient(ppc2.C, center2, vpv);
    ppc2.TranslateZ(30);
    for(int si = 0; si < stepsN; si++) {
        ppc->SetByInterpolation(ppc, &ppc2, (float) si/ (float) (stepsN-1));
        Render();
        Fl::check();
    }
    ppc->Save("camera_path.txt");

    ppc2 = *ppc;
    ppc2.TranslateX(-500);
    ppc2.TranslateY(-20);
    ppc2.TranslateZ(-80);
    ppc2.PositionAndOrient(ppc2.C, center1, vpv);

    stepsN = 600;
    for(int si = 0; si < stepsN; si++) {
        ppc->SetByInterpolation(ppc, &ppc2, (float) si/ (float) (stepsN-1));
        Render();
        Fl::check();
    }
    ppc->Save("camera_path.txt");

    stepsN = 300;
    for(int si = 0; si < stepsN; si++) {
        ppc->SetByInterpolation(ppc, &ppcInit, (float) si/ (float) (stepsN-1));
        Render();
        Fl::check();
    }
    ppc->Save("camera_path.txt");

}

void Scene::Play() {
    if (!env_built)
        BuildEnvironment();
    PlayAnimation();
}

void Scene::BuildRoomForMesh() {

    V3 vs[4];
    V3 colors[4];
    colors[0] = V3(0.0f, 0.0f, 0.0f);
    colors[1] = V3(0.0f, 0.0f, 0.0f);
    colors[2] = V3(0.0f, 0.0f, 0.0f);
    colors[3] = V3(0.0f, 0.0f, 0.0f);

    float down = tmeshes[0]->aabb->maxy();
    float up = tmeshes[0]->aabb->miny() * 2.5; 
    float right = tmeshes[0]->aabb->maxx() * 3;
    float left = tmeshes[0]->aabb->minx() * 3;
    //float front = tmeshes[tmeshesN-1]->aabb->minz();
    float back = tmeshes[0]->aabb->maxz() * 3;
    float front = ppc->C[2] - 10;

    loadTextureHW("grass.tiff");

    //World Floor
    vs[0] = V3(-10000, down - 4, 10000);
    vs[1] = V3(10000, down - 4, 10000);
    vs[2] = V3(10000, down - 4, -10000);
    vs[3] = V3(-10000, down - 4, -10000);
    AddMeshHW( new TMesh(vs, colors), tnames[1]);

    loadTextureHW("sky.tif");

    //World Sky
    vs[0] = V3(-10000, 110, 10000);
    vs[1] = V3(10000, 110, 10000);
    vs[2] = V3(10000, 110, -10000);
    vs[3] = V3(-10000, 110, -10000);
    AddMeshHW( new TMesh(vs, colors), tnames[2]);

    loadTextureHW("mountain.tiff");

    //
    //World Back
    vs[0] = V3(-10000, -110, back - 2000);
    vs[1] = V3(10000, -110, back - 2000);
    vs[2] = V3(10000, 110, back - 2000);
    vs[3] = V3(-10000, 110, back - 2000);
    AddMeshHW( new TMesh(vs, colors), tnames[3]);

    loadTextureHW("tex1.tiff");
    
    //Floor
    vs[0] = V3(left, down, back);
    vs[1] = V3(right, down, back);
    vs[2] = V3(right, down, front);
    vs[3] = V3(left, down, front);
    AddMeshHW( new TMesh(vs, colors), tnames[4]);

    //Roof
    vs[0] = V3(left, up, back);
    vs[1] = V3(right, up, back);
    vs[2] = V3(right, up, front);
    vs[3] = V3(left, up, front);
    AddMeshHW( new TMesh(vs, colors), tnames[4]);

    //Left
    vs[0] = V3(left, down, back);
    vs[1] = V3(left, up, back);
    vs[2] = V3(left, up, front);
    vs[3] = V3(left, down, front);
    AddMeshHW( new TMesh(vs, colors), tnames[4]);

    //Right
    vs[0] = V3(right, down, back);
    vs[1] = V3(right, up, back);
    vs[2] = V3(right, up, front);
    vs[3] = V3(right, down, front);
    AddMeshHW( new TMesh(vs, colors), tnames[4]);

    //Back
    vs[0] = V3(left, down, back);
    vs[1] = V3(right, down, back);
    vs[2] = V3(right, up, back);
    vs[3] = V3(left, up, back);
    AddMeshHW( new TMesh(vs, colors), tnames[4]);
}

void Scene::changeBrightness() {
    float brightness = gui->BrightnessSlider->value();
    fb->AdjustBrightness(brightness);
    Render();
}

void Scene::changeContrast() {
    float contrast = gui->ContrastSlider->value();
    fb->AdjustContrast(contrast);
    Render();
}

void Scene::adjustAmbient() {
    ka = gui->AmbientFactor->value();
    Render();
}

void Scene::adjustSpecular() {
    se = gui->SpecularExponent->value();
    Render();
}

void Scene::lightSourceUp() {
    l += V3(0, step/3, 0);
    Render();
    Fl::check();
}

void Scene::lightSourceDown() {
    l += V3(0, -step/3, 0);
    Render();
    Fl::check();
}

void Scene::lightSourceLeft() {
    l -= V3(step/3, 0, 0);
    Render();
    Fl::check();
}

void Scene::lightSourceRight() {
    l += V3(step/3, 0, 0);
    Render();
    Fl::check();
}

void Scene::lightSourceBack() {
    l += V3(0, 0, step/3);
    Render();
    Fl::check();
}

void Scene::lightSourceFront() {
    l -= V3(0, 0, step/3);
    Render();
    Fl::check();
}

void Scene::detectEdges() {
  M33 edgeDetect;
  edgeDetect[0] = V3(0, 1, 0);
  edgeDetect[1] = V3(1, -3.98, 1);
  edgeDetect[2] = V3(0, 1, 0);
  edgeDetect.normalize();

  fb->Convolve33(edgeDetect, fb);
  fb->show();
  return;
}

void Scene::translateRight() {
    ppc->TranslateX(step);
    Render();
    Fl::check();
}

void Scene::translateLeft() {
    ppc->TranslateX(-step);
    Render();
    Fl::check();
}

void Scene::translateUp() {
    ppc->TranslateY(step);
    Render();
    Fl::check();
}

void Scene::translateDown() {
    ppc->TranslateY(-step);
    Render();
    Fl::check();
}

void Scene::translateFront() {
    ppc->TranslateZ(step);
    Render();
    Fl::check();
}

void Scene::translateBack() {
    ppc->TranslateZ(-step);
    Render();
    Fl::check();
}

void Scene::adjustStep() {
    step = gui->StepSlider->value();
}

void Scene::ZoomIn() {
    ppc->Zoom(step);
    Render();
    Fl::check();
}

void Scene::ZoomOut() {
    if (step < 1.0f)
        step = 1;
    ppc->Zoom(1/step);
    Render();
    Fl::check();
}

void Scene::tiltUp() {
    ppc->Tilt(step);
    Render();
    Fl::check();
}

void Scene::tiltDown() {
    ppc->Tilt(-step);
    Render();
    Fl::check();
}

void Scene::panLeft() {
    ppc->Pan(step);
    Render();
    Fl::check();
}

void Scene::panRight() {
    ppc->Pan(-step);
    Render();
    Fl::check();
}

void Scene::rollLeft() {
    ppc->Roll(step);
    Render();
    Fl::check();
}

void Scene::rollRight() {
    ppc->Roll(-step);
    Render();
    Fl::check();
}

void Scene::tileByRepeat(){
    tl = 0;
    Render();
    Fl::check();
}

void Scene::tileByMirror(){
    tl = 1;
    Render();
    Fl::check();
}

void Scene::tmBilinear(){
    tm = 0;
    Render();
    Fl::check();
}

void Scene::tmNN(){
    tm = 1;
    Render();
    Fl::check();
}

void Scene::loadGeometry(const char *filename) {

    //use load bin and add to tmesh array
    if (!tmeshes) {
        tmeshes = new TMesh*[20];
    }
    tmeshes[tmeshesN] = new TMesh();
    tmeshes[tmeshesN]->LoadBin(filename);

    V3 newCenter;

    //first mesh
    if (tmeshesN == 0) {
        newCenter = ppc->C;
        tmeshes[tmeshesN]->Position(newCenter);
        tmeshes[tmeshesN]->SetAABB();
        ppc->TranslateZ(-2.5 * tmeshes[tmeshesN]->aabb->width());
        l = tmeshes[tmeshesN]->GetCenter() + V3(0.0f, 0.0f, 50.0f);
        tmeshesN++;
        Render();
        Fl::check();
        return;
    }
    newCenter = tmeshes[tmeshesN]->GetCenter() + V3(tmeshes[tmeshesN]->aabb->length(), 0, 0);
    ppc->TranslateX(tmeshes[tmeshesN]->aabb->length() / 3.0f);
    tmeshes[tmeshesN]->Position(newCenter);
    tmeshesN++;
    Render();
    Fl::check();
}

void Scene::loadGeometry() {
    Fl_File_Chooser chooser(".",                        // directory
                            "*",                        // filter
                            Fl_File_Chooser::SINGLE,     // chooser type
                            "Open...");        // title
    chooser.show();
     while(chooser.shown())
    { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
    { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

    //use load bin and add to tmesh array
    if (!tmeshes) {
        tmeshes = new TMesh*[20];
    }
    tmeshes[tmeshesN] = new TMesh();
    tmeshes[tmeshesN]->LoadBin(chooser.value());

    V3 newCenter;

    //first mesh
    if (tmeshesN == 0) {
        newCenter = ppc->C;
        tmeshes[tmeshesN]->Position(newCenter);
        tmeshes[tmeshesN]->SetAABB();
        ppc->TranslateZ(-2.5 * tmeshes[tmeshesN]->aabb->width());
        l = tmeshes[tmeshesN]->GetCenter() + V3(0.0f, 0.0f, 50);
        tmeshesN++;
        Render();
        Fl::check();
        return;
    }
    newCenter = tmeshes[tmeshesN]->GetCenter() + V3(tmeshes[tmeshesN]->aabb->length(), 0, 0);
    ppc->TranslateX(tmeshes[tmeshesN]->aabb->length() / 3.0f);
    tmeshes[tmeshesN]->Position(newCenter);
    tmeshesN++;
    Render();
    Fl::check();
}

void Scene::loadTexture(const char *filename) {
    TIFF *tif = TIFFOpen(filename, "r");
    if (!tif) {
        fprintf(stderr, "TIFFOpen failed\n");
        exit(1);
    }

    unsigned int w, h;
    size_t npixels;
    unsigned int *raster;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
        if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
            if (currTexture)
                delete currTexture;
            int u0 = w;
            int v0 = h;
            currTexture = new FrameBuffer(raster, u0, v0, w, h);
        }
        _TIFFfree(raster);
    }
    TIFFClose(tif);
}

void Scene::loadTextureHW(const char *filename) {
    TIFF *tif = TIFFOpen(filename, "r");
    if (!tif) {
        fprintf(stderr, "TIFFOpen failed\n");
        exit(1);
    }

    //Load file, populate raster
    unsigned int w, h;
    size_t npixels;
    unsigned int *raster;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
        if (!TIFFReadRGBAImage(tif, w, h, raster, 0)) {
            fprintf(stderr, "tiff failed loading texture");
            exit(1);
        }
        _TIFFfree(raster);
    }
    TIFFClose(tif);

    //Allocate space for textures
    if (!textures) {
        textures = new unsigned int*[10];
        tnames = new unsigned int[10];
    }

    textures[texN] = raster;

    //Create texture
    glGenTextures(texN, &tnames[texN]);

    //Bind Texture
    glBindTexture(GL_TEXTURE_2D, tnames[texN]);

    //Give raster to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                 w, h, 0, GL_RGBA, 
                 GL_UNSIGNED_BYTE, textures[texN]);

    //Default parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    // switch to texture mode for projective mapping  
    glMatrixMode (GL_TEXTURE);  
    glLoadIdentity ();  

    // converts -1 to 1 into 0 to 1   
    glTranslatef (1.0f, 1.0f, 1.0f);  
    glScalef (0.5f, 0.5f, 0.5f);  
    
    // our projectors setup and position  
    gluPerspective (90.0f, 1.0f, 0.1f, 1.0f);  
    gluLookAt(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f); 

    texN++;
}

void Scene::loadImage() {
    Fl_File_Chooser chooser(".",                        // directory
            "*",                        // filter
            Fl_File_Chooser::SINGLE,     // chooser type
            "Open...");        // title
    chooser.show();
     while(chooser.shown())
    { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
    { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

    TIFF *tif = TIFFOpen(chooser.value(), "r");
    if (!tif) {
        fprintf(stderr, "TIFFOpen failed\n");
        exit(1);
    }

    unsigned int w, h;
    size_t npixels;
    unsigned int *raster;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
        if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
            //fb->hide();
            delete fb;
            int u0 = w;
            int v0 = h;
            fb = new FrameBuffer(raster, u0, v0, w, h);
            fb->label(chooser.value());
            fb->show();
        }
        _TIFFfree(raster);
    }
    TIFFClose(tif);
}

void Scene::saveTiff(const char *filename) {
    TIFF *tif = TIFFOpen(filename, "w");
    if (!tif) {
        fprintf(stderr, "TIFFOPEN failed\n");
        exit(1);
    }

    //Get tags
    uint16 out[1];
    out[0] = EXTRASAMPLE_ASSOCALPHA;
    unsigned int samplePerPixel = 4;
    unsigned int bitsPerSample = 8;

    //Set tags
    TIFFSetField(tif,TIFFTAG_IMAGEWIDTH, fb->w);
    TIFFSetField(tif,TIFFTAG_IMAGELENGTH, fb->h);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,samplePerPixel);
    TIFFSetField(tif,TIFFTAG_EXTRASAMPLES,EXTRASAMPLE_ASSOCALPHA, &out);
    TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitsPerSample);
    TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
    TIFFSetField(tif,TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, fb->w*samplePerPixel));

    unsigned int *buf = (unsigned int *) _TIFFmalloc(fb->w*4);

    for (int row = 0; row < fb->h; row++) {
        memcpy(buf, &(fb->pix[(fb->h - row - 1)*fb->w]), fb->w*4);
        if (TIFFWriteScanline(tif, buf, row, 0) < 0)
            break;
    }

    if (buf)
        _TIFFfree(buf);
    TIFFClose(tif);

}

void Scene::saveImage() {
    Fl_File_Chooser chooser(".",
                            "*",
                            Fl_File_Chooser::CREATE,
                            "Save File as...");
    chooser.show();
    while(chooser.shown())
    { Fl::wait(); }

    // User hit cancel
    if ( chooser.value() == NULL )
    { fprintf(stderr, "(User hit 'Cancel')\n"); return; }

    saveTiff(chooser.value());
}

