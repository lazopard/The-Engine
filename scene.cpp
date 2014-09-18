#include "scene.h"
#include "m33.h"

#include <iostream>
#include <tiffio.h>

using namespace std;

Scene *scene;

char filename[1024] = "";

Scene::Scene() {
    gui = new GUI();
    gui->show();

    int u0 = 20;
    int v0 = 50;
    int sci = 2;
    int w = sci*320;
    int h = sci*240;
    fb = new FrameBuffer(u0, v0, w, h);
    fb->label("Framebuffer");
    fb->show();
    gui->uiw->position(fb->w+20+20, 50);

    float hfov = 55.0f;
    ppc = new PPC(hfov, w, h);

    tmeshesN = 2;
    tmeshes = new TMesh*[tmeshesN];

    V3 center(7.0f, -10.0f, -65.0f);
    V3 dims(20.0f, 10.0f, 45.0f);
    unsigned int color = 0xFF00FF00;
    tmeshes[0] = new TMesh(center, dims, color);
    tmeshes[0]->enabled = false;

    tmeshes[1] = new TMesh();
    tmeshes[1]->LoadBin("geometry/teapot1k.bin");
    V3 newCenter = V3(0.0f, 0.0f, -150.0f);
    tmeshes[1]->Position(newCenter);

    Render();
}

void Scene::Render() {
    fb->Set(0xFFFFFFFF);
    for (int tmi = 0; tmi < tmeshesN; tmi++) {
        if (!tmeshes[tmi]->enabled)
            continue;
        tmeshes[tmi]->RenderWireframe(ppc, fb, 0xFF000000);
        tmeshes[tmi]->RenderPoints(ppc, fb, 3);
    }
    fb->redraw();
}

void Scene::DBG() {

    float r = 10;
    int i,j;
    for(j = 0; j < 100; j+=2*r) {
        fb->FillCircle(fb->w/6, fb->h/6 + j, r, 0x00000000);
    }
    for(i = 0; i < 50; i+=2*r) {
        fb->FillCircle(fb->w/6 + i, fb->h/6 + j, r, 0x00000000);
    }

    i+=4*r;
    for(j = 0; j < 100; j+=2*r) {
        fb->FillCircle(fb->w/6 + i, fb->h/6 + j, r, 0x00000000);
    }

    int s = i + 50;
    for(i; i <  s; i+=2*r) {
        fb->FillCircle(fb->w/6 + i, fb->h/6, r, 0x00000000);
        fb->FillCircle(fb->w/6 + i, fb->h/6 + j, r, 0x00000000);
        fb->FillCircle(fb->w/6 + i, fb->h/6 + j/2, r, 0x00000000);
    }

    fb->DrawCircleWithThickness(fb->w/6 + s + 6*r, fb->h/6 + j/2, 5*r, r, 0x00000000);

    Render();

    return;
}

void Scene::changeBrightness() {
    float brightness = gui->BrightnessSlider->value();
    brightness /= 100.0f;
    //fb->AdjustBrightness(brightness);
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

    TIFF *tif = TIFFOpen(chooser.value(), "w");
    if (!tif) {
        fprintf(stderr, "TIFFOPEN failed\n");
        exit(1);
    }

    uint32 width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    uint16 out[1];
    out[0] = EXTRASAMPLE_ASSOCALPHA;
    unsigned int samplePerPixel = 4;
    unsigned int bitsPerSample = 8;

    TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
    TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,samplePerPixel);
    TIFFSetField(tif,TIFFTAG_EXTRASAMPLES,EXTRASAMPLE_ASSOCALPHA, &out);
    TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitsPerSample);
    TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
    //TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, width * samplePerPixel));

    TIFFClose(tif);
}

