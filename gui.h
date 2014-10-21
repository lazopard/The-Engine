// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef gui_h
#define gui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Value_Input.H>

class GUI {
public:
  GUI();
  Fl_Double_Window *uiw;
private:
  inline void cb_Play_i(Fl_Button*, void*);
  static void cb_Play(Fl_Button*, void*);
public:
  Fl_Value_Slider *BrightnessSlider;
private:
  inline void cb_BrightnessSlider_i(Fl_Value_Slider*, void*);
  static void cb_BrightnessSlider(Fl_Value_Slider*, void*);
  inline void cb_Edge_i(Fl_Button*, void*);
  static void cb_Edge(Fl_Button*, void*);
  inline void cb_Load_i(Fl_Button*, void*);
  static void cb_Load(Fl_Button*, void*);
  inline void cb_Save_i(Fl_Button*, void*);
  static void cb_Save(Fl_Button*, void*);
  inline void cb_L_i(Fl_Button*, void*);
  static void cb_L(Fl_Button*, void*);
  inline void cb_R_i(Fl_Button*, void*);
  static void cb_R(Fl_Button*, void*);
  inline void cb_U_i(Fl_Button*, void*);
  static void cb_U(Fl_Button*, void*);
  inline void cb_D_i(Fl_Button*, void*);
  static void cb_D(Fl_Button*, void*);
  inline void cb_TiltU_i(Fl_Button*, void*);
  static void cb_TiltU(Fl_Button*, void*);
  inline void cb_PanL_i(Fl_Button*, void*);
  static void cb_PanL(Fl_Button*, void*);
  inline void cb_RollL_i(Fl_Button*, void*);
  static void cb_RollL(Fl_Button*, void*);
  inline void cb_Load1_i(Fl_Button*, void*);
  static void cb_Load1(Fl_Button*, void*);
public:
  Fl_Value_Slider *ContrastSlider;
private:
  inline void cb_ContrastSlider_i(Fl_Value_Slider*, void*);
  static void cb_ContrastSlider(Fl_Value_Slider*, void*);
  inline void cb_F_i(Fl_Button*, void*);
  static void cb_F(Fl_Button*, void*);
  inline void cb_B_i(Fl_Button*, void*);
  static void cb_B(Fl_Button*, void*);
  inline void cb_PanR_i(Fl_Button*, void*);
  static void cb_PanR(Fl_Button*, void*);
  inline void cb_RollR_i(Fl_Button*, void*);
  static void cb_RollR(Fl_Button*, void*);
  inline void cb_TiltD_i(Fl_Button*, void*);
  static void cb_TiltD(Fl_Button*, void*);
  inline void cb_Load2_i(Fl_Button*, void*);
  static void cb_Load2(Fl_Button*, void*);
  inline void cb_Save1_i(Fl_Button*, void*);
  static void cb_Save1(Fl_Button*, void*);
  inline void cb_Z_i(Fl_Button*, void*);
  static void cb_Z(Fl_Button*, void*);
  inline void cb_Z1_i(Fl_Button*, void*);
  static void cb_Z1(Fl_Button*, void*);
public:
  Fl_Value_Slider *StepSlider;
private:
  inline void cb_StepSlider_i(Fl_Value_Slider*, void*);
  static void cb_StepSlider(Fl_Value_Slider*, void*);
public:
  Fl_Value_Input *AmbientFactor;
private:
  inline void cb_AmbientFactor_i(Fl_Value_Input*, void*);
  static void cb_AmbientFactor(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *SpecularExponent;
private:
  inline void cb_SpecularExponent_i(Fl_Value_Input*, void*);
  static void cb_SpecularExponent(Fl_Value_Input*, void*);
  inline void cb_L1_i(Fl_Button*, void*);
  static void cb_L1(Fl_Button*, void*);
  inline void cb_R1_i(Fl_Button*, void*);
  static void cb_R1(Fl_Button*, void*);
  inline void cb_U1_i(Fl_Button*, void*);
  static void cb_U1(Fl_Button*, void*);
  inline void cb_D1_i(Fl_Button*, void*);
  static void cb_D1(Fl_Button*, void*);
  inline void cb_SM1_i(Fl_Button*, void*);
  static void cb_SM1(Fl_Button*, void*);
  inline void cb_SM2_i(Fl_Button*, void*);
  static void cb_SM2(Fl_Button*, void*);
  inline void cb_SM3_i(Fl_Button*, void*);
  static void cb_SM3(Fl_Button*, void*);
  inline void cb_Mirroring_i(Fl_Button*, void*);
  static void cb_Mirroring(Fl_Button*, void*);
  inline void cb_Repetition_i(Fl_Button*, void*);
  static void cb_Repetition(Fl_Button*, void*);
  inline void cb_Bilinear_i(Fl_Button*, void*);
  static void cb_Bilinear(Fl_Button*, void*);
  inline void cb_Nearest_i(Fl_Button*, void*);
  static void cb_Nearest(Fl_Button*, void*);
public:
  void show();
  void DBG_cb();
  void load_cb();
  void save_cb();
  void brightness_cb();
  void detect_edges_cb();
  void load_geometry_cb();
  void left_cb();
  void right_cb();
  void down_cb();
  void tiltU_cb();
  void panL_cb();
  void rollL_cb();
  void up_cb();
  void contrast_cb();
  void f_cb();
  void b_cb();
  void tiltD_cb();
  void panR_cb();
  void rollR_cb();
  void load_camera_cb();
  void save_camera_cb();
  void zoom_in_cb();
  void zoom_out_cb();
  void step_cb();
  void ambient_cb();
  void specular_cb();
  void upl_cb();
  void downl_cb();
  void leftl_cb();
  void rightl_cb();
  void sm1_cb();
  void sm2_cb();
  void sm3_cb();
  void mirror_cb();
  void bilinear_cb();
  void repeat_cb();
  void nn_cb();
};
#endif
