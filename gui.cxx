// generated by Fast Light User Interface Designer (fluid) version 1.0302

#include "gui.h"

void GUI::cb_DBG_i(Fl_Button*, void*) {
  DBG_cb();
}
void GUI::cb_DBG(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_DBG_i(o,v);
}

void GUI::cb_FlagB_i(Fl_Check_Button*, void*) {
  FlagB_cb();
}
void GUI::cb_FlagB(Fl_Check_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_FlagB_i(o,v);
}

void GUI::cb_BrightnessSlider_i(Fl_Value_Slider*, void*) {
  brightness_cb();
}
void GUI::cb_BrightnessSlider(Fl_Value_Slider* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_BrightnessSlider_i(o,v);
}

void GUI::cb_Edge_i(Fl_Button*, void*) {
  detect_edges_cb();
}
void GUI::cb_Edge(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Edge_i(o,v);
}

void GUI::cb_Load_i(Fl_Button*, void*) {
  load_cb();
}
void GUI::cb_Load(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Load_i(o,v);
}

void GUI::cb_Save_i(Fl_Button*, void*) {
  save_cb();
}
void GUI::cb_Save(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Save_i(o,v);
}
#include "scene.h"

GUI::GUI() {
  { uiw = new Fl_Double_Window(295, 234, "GUI");
    uiw->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(200, 0, 80, 25, "DBG");
      o->selection_color(FL_DARK_RED);
      o->callback((Fl_Callback*)cb_DBG);
    } // Fl_Button* o
    { FlagB = new Fl_Check_Button(200, 100, 40, 25, "Flag");
      FlagB->down_box(FL_DOWN_BOX);
      FlagB->value(1);
      FlagB->callback((Fl_Callback*)cb_FlagB);
    } // Fl_Check_Button* FlagB
    { BrightnessSlider = new Fl_Value_Slider(20, 5, 40, 200, "Brighness %");
      BrightnessSlider->maximum(100);
      BrightnessSlider->step(0.1);
      BrightnessSlider->value(50);
      BrightnessSlider->textsize(14);
      BrightnessSlider->callback((Fl_Callback*)cb_BrightnessSlider);
    } // Fl_Value_Slider* BrightnessSlider
    { Fl_Button* o = new Fl_Button(70, 5, 95, 25, "Edge Detection");
      o->callback((Fl_Callback*)cb_Edge);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(200, 25, 80, 30, "Load file");
      o->callback((Fl_Callback*)cb_Load);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(200, 55, 80, 30, "Save file");
      o->callback((Fl_Callback*)cb_Save);
    } // Fl_Button* o
    uiw->end();
  } // Fl_Double_Window* uiw
}

int main(int argc, char **argv) {
  scene = new Scene;
  return Fl::run();
}

void GUI::show() {
  uiw->show();
}

void GUI::DBG_cb() {
  scene->DBG();
}

void GUI::FlagB_cb() {
  scene->FlagB();
}

void GUI::load_cb() {
  scene->loadImage();
}

void GUI::save_cb() {
  scene->saveImage();
}

void GUI::brightness_cb() {
  scene->changeBrightness();
}

void GUI::detect_edges_cb() {
  scene->detectEdges();
}
