#include "scene.h"
#include "m33.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
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
    step = 10;
    fb = new FrameBuffer(u0, v0, w, h);
    fb->label("Framebuffer");
    fb->show();
    gui->uiw->position(fb->w+20+20, 50);

    float hfov = 55.0f;
    ppc = new PPC(hfov, w, h);

    ka=se=0;
    sm = 0;

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
    //use load bin and add to tmesh array
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
    fb->Set(0xFFFFFFFF);
    V3 v(0,0,0);
    for (int tmi = 0; tmi < tmeshesN; tmi++) {
        if (!tmeshes[tmi]->enabled)
            continue;
        tmeshes[tmi]->RenderFilled(ppc, fb, 0xFF000000, v, ka, se, sm);
        //tmeshes[tmi]->RenderPoints(ppc, fb, 3);
    }
    fb->redraw();
}

//play
void Scene::Play() {
    PlayInterpolationAnimation();
}

void Scene::PlayInterpolationAnimation() {

    PPC ppc1(*ppc);
    V3 newC(-50.0f, 30.0f, -150.0f);
    V3 lap = newC + V3(50.0f, -5.0f, -100.0f);
    V3 vpv(0.0f, 1.0f, 0.0f);
    ppc1.PositionAndOrient(newC, lap, vpv);
    float f = 40.0f;

    PPC ppc2(*ppc);
    V3 newC2(30.0f, -30.0f, -150.0f);
    V3 lap2 = newC2 + V3(-50.0f, +5.0f, -100.0f);
    V3 vpv2(0.0f, 1.0f, 0.0f);
    ppc2.PositionAndOrient(newC2, lap2, vpv2);

    int stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
        fb->Clear(0xFFFFFF, 0.0f);
        ppc1.RenderWireframe(ppc, fb, f, 0xFF0000FF);
        ppc2.RenderWireframe(ppc, fb, f, 0xFF00FF00);

        PPC ppc3(*ppc);
        ppc3.SetByInterpolation(&ppc1, &ppc2, (float) si / (float) (stepsN-1));
        ppc3.RenderWireframe(ppc, fb, f, 0xFF000000);
        fb->redraw();
        Fl::check();
    }

    fb->redraw();
    return;
}

void Scene::PPCMovementAnimation() {
    ofstream of;
    of.open("camera_path.txt");
    V3 C0 = ppc->C;
    V3 lap = tmeshes[0]->GetCenter();
    V3 vpv = V3(0.0f, 1.0f, 0.0f);

    int frames = 0;

    //Tilt
    step = 20;
    ppc->Tilt(step);
    V3 C1 = ppc->C;
    ppc->Tilt(-step);
    int stepsN = 100;
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);
      frames++;
    }
    ppc->Tilt(step);

    C0 = ppc->C;

    //Roll 
    ppc->Roll(step);
    C1 = ppc->C;
    ppc->Roll(-step);
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);
      frames++;
    }
    ppc->Roll(step);

    C0 = ppc->C;

    //Pan
    ppc->Pan(step);
    C1 = ppc->C;
    ppc->Pan(-step);
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);
      frames++;
    }
    ppc->Pan(step);

    C0 = ppc->C;

    //Left
    ppc->TranslateX(-step);
    C1 = ppc->C;
    ppc->TranslateX(step);
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);
      frames++;
    }
    ppc->TranslateX(-step);

    C0 = ppc->C;

    //Up
    ppc->TranslateY(step);
    C1 = ppc->C;
    ppc->TranslateY(-step);
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);

      frames++;
    }
    ppc->TranslateY(step);

    C0 = ppc->C;

    //Back
    ppc->TranslateZ(-step);
    C1 = ppc->C;
    ppc->TranslateZ(step);
    for (int si = 0; si < stepsN; si++) {
      V3 newC = C0 + (C1-C0)*(float)si/(float)(stepsN-1);
      of << newC << endl;
      ppc->PositionAndOrient(newC, lap, vpv);
      Render();
      Fl::check();
      string s = to_string(frames);
      s+=".tif";
      const char *f = s.c_str();
      saveTiff(f);

      frames++;
    }
    ppc->TranslateZ(-step);

    C0 = ppc->C;

    of.close();
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
}

void Scene::lightSourceDown() {
}

void Scene::lightSourceLeft() {
}

void Scene::lightSourceRight() {
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
}

void Scene::translateLeft() {
    ppc->TranslateX(-step);
    Render();
}

void Scene::translateUp() {
    ppc->TranslateY(step);
    Render();
}

void Scene::translateDown() {
    ppc->TranslateY(-step);
    Render();
}

void Scene::translateFront() {
    ppc->TranslateZ(step);
    Render();
}

void Scene::translateBack() {
    ppc->TranslateZ(-step);
    Render();
}

void Scene::adjustStep() {
    step = gui->StepSlider->value();
}

void Scene::ZoomIn() {
    ppc->Zoom(step);
    Render();
}

void Scene::ZoomOut() {
    ppc->Zoom(1/step);
    Render();
}

void Scene::tiltUp() {
    ppc->Tilt(step);
    Render();
}

void Scene::tiltDown() {
    ppc->Tilt(-step);
    Render();
}

void Scene::panLeft() {
    ppc->Pan(step);
    Render();
}

void Scene::panRight() {
    ppc->Pan(-step);
    Render();
}

void Scene::rollLeft() {
    ppc->Roll(step);
    Render();
}

void Scene::rollRight() {
    ppc->Roll(-step);
    Render();
}

void Scene::sm1() {
    sm = 0;
    Render();
}

void Scene::sm2() {
    sm = 1;
    Render();
}

void Scene::sm3() {
    sm = 2;
    Render();
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
    if (tmeshesN < 1) {
        newCenter = ppc->C - V3(0,0,100);
        tmeshes[tmeshesN]->Position(newCenter);
        tmeshesN++;
        Render();
        return;
    }

    newCenter = tmeshes[tmeshesN]->GetCenter() + V3(100, 0, 0);
    ZoomOut();
    ZoomOut();
    tmeshes[tmeshesN]->Position(newCenter);
    tmeshesN++;
    Render();
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

