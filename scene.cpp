#include "scene.h"
#include "m33.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tiffio.h>
#include <float.h>

using namespace std;

Scene *scene;

char filename[1024] = "";
const float SM_EPS = 0.01f;

Scene::Scene() {

    //GUI and Window
    gui = new GUI();
    gui->show();
    int u0 = 20;
    int v0 = 50;
    int sci = 2;
    int w = sci*320;
    int h = sci*240;
    fb = new FrameBuffer(u0, v0, w, h);
    fb->label("SW Framebuffer");
    //fb->show();
    gui->uiw->position(fb->w+20+20, 50);

    hwfb = new FrameBuffer(u0, v0, fb->w, fb->h);
    hwfb->label("HW Framebuffer");
    hwfb->isHW = true;
    hwfb->show();

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
    //tname = new int[20];

    // Tmeshes
    tmeshesN = 0;
    tmeshes = 0;

    Render();
}



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

void Scene::Render() {

    RenderHW();
    return;
    //if (hwfb)
     //   hwfb->redraw();

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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, (float*) &l);
    */
    glEnable(GL_TEXTURE_2D);

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
        //tmeshes[tmi]->RenderWireframeHW();
        tmeshes[tmi]->RenderHW();
    }

    hwfb->redraw();
}

void Scene::AddMesh(TMesh *tmesh, FrameBuffer *tex) {
    if (tex != NULL)
        tmesh->AddTexture(tex);
    tmeshes[tmeshesN] = tmesh;
    tmeshesN++;
}

void Scene::AddMeshHW(TMesh *tmesh, unsigned int *tex) {
    if (tex != NULL)
        tmesh->AddTextureHW(tex);
    tmeshes[tmeshesN] = tmesh;
    tmeshesN++;
}

//play
void Scene::Play() {
    loadGeometry("geometry/teapot1k.bin");
    BuildRoomForMesh();
    Render();
}

void Scene::BuildRoomForMesh() {

    V3 vs[4];
    V3 colors[4];
    colors[0] = V3(0.0f, 0.0f, 0.0f);
    colors[1] = V3(0.0f, 0.0f, 0.0f);
    colors[2] = V3(0.0f, 0.0f, 0.0f);
    colors[3] = V3(0.0f, 0.0f, 0.0f);

    float down = tmeshes[tmeshesN-1]->aabb->maxy();
    float up = tmeshes[tmeshesN-1]->aabb->miny() * 2.5; 
    float right = tmeshes[tmeshesN-1]->aabb->maxx() * 3;
    float left = tmeshes[tmeshesN-1]->aabb->minx() * 3;
    //float front = tmeshes[tmeshesN-1]->aabb->minz();
    float back = tmeshes[tmeshesN-1]->aabb->maxz() * 3;
    float front = ppc->C[2] - 10;

    loadTextureHW("tex1.tiff");
    
    //Floor
    vs[0] = V3(left, down, back);
    vs[1] = V3(right, down, back);
    vs[2] = V3(right, down, front);
    vs[3] = V3(left, down, front);
    //AddMeshHW( new TMesh(vs, colors), currTexture);

    //Roof
    vs[0] = V3(left, up, back);
    vs[1] = V3(right, up, back);
    vs[2] = V3(right, up, front);
    vs[3] = V3(left, up, front);
    //AddMeshHW( new TMesh(vs, colors), currTexture);

    //Left
    vs[0] = V3(left, down, back);
    vs[1] = V3(left, up, back);
    vs[2] = V3(left, up, front);
    vs[3] = V3(left, down, front);
    //AddMeshHW( new TMesh(vs, colors), currTexture);

    //Right
    vs[0] = V3(right, down, back);
    vs[1] = V3(right, up, back);
    vs[2] = V3(right, up, front);
    vs[3] = V3(right, down, front);
    //AddMeshHW( new TMesh(vs, colors), currTexture);

    //Back
    vs[0] = V3(left, down, back);
    vs[1] = V3(right, down, back);
    vs[2] = V3(right, up, back);
    vs[3] = V3(left, up, back);
    //AddMeshHW( new TMesh(vs, colors), currTexture);
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
            fb->hide();
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

