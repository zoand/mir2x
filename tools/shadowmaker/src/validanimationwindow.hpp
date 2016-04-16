// generated by Fast Light User Interface Designer (fluid) version 1.0303

#ifndef validanimationwindow_hpp
#define validanimationwindow_hpp
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "validwilfilewindow.hpp"
#include "animationpreviewwindow.hpp"
#include <FL/Fl_Browser.H>
#include <cstdio>

class ValidAnimationWindow {
  int m_AnimationCount; 
  int m_FileIndex; 
public:
  ValidAnimationWindow();
private:
  Fl_Double_Window *m_Window;
  inline void cb_m_Window_i(Fl_Double_Window*, void*);
  static void cb_m_Window(Fl_Double_Window*, void*);
  Fl_Browser *m_ValidAnimationBrowser;
  inline void cb_m_ValidAnimationBrowser_i(Fl_Browser*, void*);
  static void cb_m_ValidAnimationBrowser(Fl_Browser*, void*);
public:
  ~ValidAnimationWindow();
  void ShowAll();
  void CheckValidAnimation(int nMaxIndex);
  void SetFileIndex(int nFileIndex);
  int AnimationCount();
};
#endif
