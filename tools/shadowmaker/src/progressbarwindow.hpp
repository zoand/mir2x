// generated by Fast Light User Interface Designer (fluid) version 1.0303

#ifndef progressbarwindow_hpp
#define progressbarwindow_hpp
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Progress.H>

class ProgressBarWindow {
public:
  ProgressBarWindow();
private:
  Fl_Double_Window *m_Window;
  inline void cb_m_Window_i(Fl_Double_Window*, void*);
  static void cb_m_Window(Fl_Double_Window*, void*);
  Fl_Progress *m_ProgressBar;
public:
  void ShowAll();
  void Redraw();
  void SetValue(int nValue);
  void HideAll();
};
#endif
