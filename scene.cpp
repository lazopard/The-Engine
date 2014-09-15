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

  fb->Set(0xFFFFFFFF);

  Render();
}

void Scene::Render() {
    fb->redraw();
}

void Scene::DBG() {

  FrameBuffer *convolvedImage = 0;
  fb->SetChecker(64, 0xFF000000, 0xFFFFFFFF);
  M33 blurk;
  blurk[0] = V3(1.0f, 2.0f, 1.0f);
  blurk[1] = V3(2.0f, 4.0f, 1.0f);
  blurk[2] = V3(1.0f, 2.0f, 1.0f);

  blurk[0] = V3(1.0f, 1.0f, 1.0f);
  blurk[1] = V3(1.0f, 1.0f, 1.0f);
  blurk[2] = V3(1.0f, 1.0f, 1.0f);

  blurk.normalize();

  fb->Convolve33(blurk, convolvedImage);
  //convolvedImage->show();
  //return;

  fb->DrawSegment(10.1f, 20.3f, 10.1f, 20.3f, 0xFF000000);
  Render();
  //return;

  fb->DrawPoint(200, fb->h-30, 170, 0xFF00FF00);
  Render();
  //return;

  unsigned int color0 = 0xFF000000;
  unsigned int color1 = 0xFFFFFFFF;
  int slowMotionFactor = 10;
  for (int csize = slowMotionFactor; 
    csize <= fb->h*slowMotionFactor; csize++) { 
    fb->SetChecker(csize/slowMotionFactor, color0, color1);
    Render();
    Fl::check();
  }
  return;

  cerr << "INFO: pressed the DBG button" << endl;

  unsigned bgr = 0xFF00FFFF;
  fb->Set(bgr);
  Render();

  for (int u = 0; u < fb->w; u++) {
    fb->Set(u, fb->h/2, 0xFF000000);
  }
  Render();

}

void Scene::FlagB () {

  if (gui->FlagB->value()) {
    cerr << "Flag button changed to on" << endl;
  }
  else {
    cerr << "Flag button changed to off" << endl;
  }

}

void Scene::changeBrightness() {
    float brightness = gui->BrightnessSlider->value();
}

void Scene::detectEdges() {
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

    TIFFSetField(tif,TIFFTAG_IMAGEWIDTH, fb->w);
    TIFFSetField(tif,TIFFTAG_IMAGELENGTH, fb->h);
    TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,3);
    TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,8);
    TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);

    tdata_t buf = _TIFFmalloc(TIFFScanlineSize(tif));
    for(uint32 row = 0; row < fb->h; row++)
        TIFFWriteScanline(tif, buf, row);
    _TIFFfree(buf);
    TIFFClose(tif);
}

