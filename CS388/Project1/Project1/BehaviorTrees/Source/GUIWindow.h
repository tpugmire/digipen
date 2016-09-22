// Author: Jacques Newman
#pragma once
#include <unordered_map>
#include <imgui.h>
class GUIWindow
{
public:
  typedef void(*DrawFunction)(GUIWindow * self);
  GUIWindow(const char * name, DrawFunction function, bool StartActive = false, ImGuiWindowFlags flags = 0);
  ~GUIWindow();
  const char * GetName();
  
  bool m_active;
  ImGuiWindowFlags m_flags;
private:
  // just calls draw function, extendable in case you want to be able to make
  // multiple types of draw functions. 
  void Draw();
  const char * m_name; // should not change
  DrawFunction m_draw_function;


  /***Static Interface*****************************************/
public:
  static void DrawWindows();
  static void SetActive(const std::string & name, bool active = true);
  typedef std::unordered_map < std::string, GUIWindow * > WindowContainer;
  static WindowContainer * s_windows;
};
